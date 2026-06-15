// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepGraph_ShapesView.hxx>

#include <BRepGraph_Data.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepTools_History.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <shared_mutex>

namespace
{
struct BRepGraph_ReconstructionContext
{
  BRepGraph*                               Graph   = nullptr;
  const BRepGraphInc_Storage*              Storage = nullptr;
  BRepGraphInc_Reconstruct::Cache          Cache;
  NCollection_FlatMap<BRepGraph_ProductId> ActiveProducts;
};

enum class ShapeParentRoute
{
  Reject,
  Core,
  Supplement
};

static void assertMutationBoundary(BRepGraph& theGraph, [[maybe_unused]] const char* theContext)
{
  [[maybe_unused]] const bool isValid = theGraph.Editor().ValidateMutationBoundary();
  Standard_ASSERT_VOID(isValid, theContext);
}

static bool isCoreParityOrientation(const TopAbs_Orientation theOrientation)
{
  return theOrientation == TopAbs_FORWARD || theOrientation == TopAbs_REVERSED;
}

static TopoDS_Shape shapeWithoutOwnLocation(const TopoDS_Shape& theShape)
{
  TopoDS_Shape aShape = theShape;
  aShape.Location(TopLoc_Location());
  return aShape;
}

static bool shouldStoreRootLocationInRef(const BRepGraph::ShapesView::Options& theOptions,
                                         const BRepGraph_NodeId&               theParent)
{
  if (theParent.IsValid())
  {
    return theParent.NodeKind == BRepGraph_NodeId::Kind::Product
           || theParent.NodeKind == BRepGraph_NodeId::Kind::Compound;
  }
  return theOptions.CreateAutoProduct;
}

static ShapeParentRoute classifyShapeToParent(const TopAbs_ShapeEnum   theShapeType,
                                              const TopAbs_Orientation theOrientation,
                                              const BRepGraph_NodeId&  theParent)
{
  if (!theParent.IsValid())
  {
    return ShapeParentRoute::Reject;
  }

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return theShapeType == TopAbs_SHAPE ? ShapeParentRoute::Reject : ShapeParentRoute::Core;
    case BRepGraph_NodeId::Kind::Compound:
      if (theShapeType == TopAbs_SHAPE)
      {
        return ShapeParentRoute::Reject;
      }
      return isCoreParityOrientation(theOrientation) ? ShapeParentRoute::Core
                                                     : ShapeParentRoute::Supplement;
    case BRepGraph_NodeId::Kind::Solid:
      if (theShapeType == TopAbs_SHAPE)
      {
        return ShapeParentRoute::Reject;
      }
      return theShapeType == TopAbs_SHELL && isCoreParityOrientation(theOrientation)
               ? ShapeParentRoute::Core
               : ShapeParentRoute::Supplement;
    case BRepGraph_NodeId::Kind::Shell:
      if (theShapeType == TopAbs_SHAPE)
      {
        return ShapeParentRoute::Reject;
      }
      return theShapeType == TopAbs_FACE && isCoreParityOrientation(theOrientation)
               ? ShapeParentRoute::Core
               : ShapeParentRoute::Supplement;
    case BRepGraph_NodeId::Kind::Face:
      return theShapeType == TopAbs_VERTEX ? ShapeParentRoute::Supplement
                                           : ShapeParentRoute::Reject;
    case BRepGraph_NodeId::Kind::Edge:
      return theShapeType == TopAbs_VERTEX ? ShapeParentRoute::Supplement
                                           : ShapeParentRoute::Reject;
    case BRepGraph_NodeId::Kind::CompSolid:
      if (theShapeType == TopAbs_SHAPE)
      {
        return ShapeParentRoute::Reject;
      }
      return theShapeType == TopAbs_SOLID && isCoreParityOrientation(theOrientation)
               ? ShapeParentRoute::Core
               : ShapeParentRoute::Supplement;
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::Occurrence:
      return ShapeParentRoute::Reject;
  }

  return ShapeParentRoute::Reject;
}

static uint64_t attachSupplementToParent(BRepGraph&             theGraph,
                                         const TopoDS_Shape&    theShape,
                                         const BRepGraph_NodeId theParent)
{
  TopoDS_Shape aSuppShape = theShape;
  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound:
      return theGraph.Editor().Supplement().AttachToCompound(
        BRepGraph_CompoundId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::CompoundAuxShape);
    case BRepGraph_NodeId::Kind::CompSolid:
      return theGraph.Editor().Supplement().AttachToCompSolid(
        BRepGraph_CompSolidId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape);
    case BRepGraph_NodeId::Kind::Solid:
      return theGraph.Editor().Supplement().AttachToSolid(
        BRepGraph_SolidId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);
    case BRepGraph_NodeId::Kind::Shell:
      return theGraph.Editor().Supplement().AttachToShell(
        BRepGraph_ShellId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape);
    case BRepGraph_NodeId::Kind::Face:
      return theGraph.Editor().Supplement().AttachToFace(
        BRepGraph_FaceId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex);
    case BRepGraph_NodeId::Kind::Edge:
      return theGraph.Editor().Supplement().AttachToEdge(
        BRepGraph_EdgeId(theParent),
        aSuppShape,
        BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex);
    default:
      return 0;
  }
}

} // namespace

//=================================================================================================

uint32_t BRepGraph::ShapesView::snapshotCountForKind(const BRepGraph&       theGraph,
                                                     const TopAbs_ShapeEnum theShapeType)
{
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  switch (theShapeType)
  {
    case TopAbs_COMPOUND:
      return static_cast<uint32_t>(aStorage.NbCompounds());
    case TopAbs_COMPSOLID:
      return static_cast<uint32_t>(aStorage.NbCompSolids());
    case TopAbs_SOLID:
      return static_cast<uint32_t>(aStorage.NbSolids());
    case TopAbs_SHELL:
      return static_cast<uint32_t>(aStorage.NbShells());
    case TopAbs_FACE:
      return static_cast<uint32_t>(aStorage.NbFaces());
    case TopAbs_WIRE:
      return static_cast<uint32_t>(aStorage.NbWires());
    case TopAbs_EDGE:
      return static_cast<uint32_t>(aStorage.NbEdges());
    case TopAbs_VERTEX:
      return static_cast<uint32_t>(aStorage.NbVertices());
    default:
      return 0;
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::detectTopologyRoot(const BRepGraph&       theGraph,
                                                           const TopAbs_ShapeEnum theShapeType,
                                                           const uint32_t theOldCountOfShapeKind)
{
  const uint32_t aNewCount = snapshotCountForKind(theGraph, theShapeType);
  if (aNewCount <= theOldCountOfShapeKind)
  {
    return BRepGraph_NodeId();
  }

  // BRepGraphInc_Populate::Append appends entities in declaration order, so the first entity
  // appended for a given shape type is always the shape root (index == pre-append count).
  // This assumption holds as long as no intermediate entities of the same type are inserted
  // before the root node during population.
  switch (theShapeType)
  {
    case TopAbs_COMPOUND:
      return BRepGraph_CompoundId(theOldCountOfShapeKind);
    case TopAbs_COMPSOLID:
      return BRepGraph_CompSolidId(theOldCountOfShapeKind);
    case TopAbs_SOLID:
      return BRepGraph_SolidId(theOldCountOfShapeKind);
    case TopAbs_SHELL:
      return BRepGraph_ShellId(theOldCountOfShapeKind);
    case TopAbs_FACE:
      return BRepGraph_FaceId(theOldCountOfShapeKind);
    case TopAbs_WIRE:
      return BRepGraph_WireId(theOldCountOfShapeKind);
    case TopAbs_EDGE:
      return BRepGraph_EdgeId(theOldCountOfShapeKind);
    case TopAbs_VERTEX:
      return BRepGraph_VertexId(theOldCountOfShapeKind);
    default:
      return BRepGraph_NodeId();
  }
}

void BRepGraph::ShapesView::populateUIDsIncremental(BRepGraph&     theGraph,
                                                    const uint32_t theOldVtx,
                                                    const uint32_t theOldEdge,
                                                    const uint32_t theOldCoEdge,
                                                    const uint32_t theOldWire,
                                                    const uint32_t theOldFace,
                                                    const uint32_t theOldShell,
                                                    const uint32_t theOldSolid,
                                                    const uint32_t theOldComp,
                                                    const uint32_t theOldCS,
                                                    const uint32_t theOldProduct,
                                                    const uint32_t theOldOccurrence,
                                                    const uint32_t theOldShellRef,
                                                    const uint32_t theOldFaceRef,
                                                    const uint32_t theOldWireRef,
                                                    const uint32_t theOldVertexRef,
                                                    const uint32_t theOldSolidRef,
                                                    const uint32_t theOldChildRef)
{
  for (BRepGraph_FullVertexIterator aVertexIt(theGraph, BRepGraph_VertexId(theOldVtx));
       aVertexIt.More();
       aVertexIt.Next())
  {
    theGraph.allocateUID(aVertexIt.CurrentId());
  }
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph, BRepGraph_EdgeId(theOldEdge)); anEdgeIt.More();
       anEdgeIt.Next())
  {
    theGraph.allocateUID(anEdgeIt.CurrentId());
  }
  for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(theGraph, BRepGraph_CoEdgeId(theOldCoEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    theGraph.allocateUID(aCoEdgeIt.CurrentId());
  }
  for (BRepGraph_FullWireIterator aWireIt(theGraph, BRepGraph_WireId(theOldWire)); aWireIt.More();
       aWireIt.Next())
  {
    theGraph.allocateUID(aWireIt.CurrentId());
  }
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph, BRepGraph_FaceId(theOldFace)); aFaceIt.More();
       aFaceIt.Next())
  {
    theGraph.allocateUID(aFaceIt.CurrentId());
  }
  for (BRepGraph_FullShellIterator aShellIt(theGraph, BRepGraph_ShellId(theOldShell));
       aShellIt.More();
       aShellIt.Next())
  {
    theGraph.allocateUID(aShellIt.CurrentId());
  }
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph, BRepGraph_SolidId(theOldSolid));
       aSolidIt.More();
       aSolidIt.Next())
  {
    theGraph.allocateUID(aSolidIt.CurrentId());
  }
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph, BRepGraph_CompoundId(theOldComp));
       aCompoundIt.More();
       aCompoundIt.Next())
  {
    theGraph.allocateUID(aCompoundIt.CurrentId());
  }
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph, BRepGraph_CompSolidId(theOldCS));
       aCompSolidIt.More();
       aCompSolidIt.Next())
  {
    theGraph.allocateUID(aCompSolidIt.CurrentId());
  }
  for (BRepGraph_FullProductIterator aProductIt(theGraph, BRepGraph_ProductId(theOldProduct));
       aProductIt.More();
       aProductIt.Next())
  {
    theGraph.allocateUID(aProductIt.CurrentId());
  }
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph,
                                                       BRepGraph_OccurrenceId(theOldOccurrence));
       anOccurrenceIt.More();
       anOccurrenceIt.Next())
  {
    theGraph.allocateUID(anOccurrenceIt.CurrentId());
  }

  for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph, BRepGraph_ShellRefId(theOldShellRef));
       aShellRefIt.More();
       aShellRefIt.Next())
  {
    theGraph.allocateRefUID(aShellRefIt.CurrentId());
  }
  for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph, BRepGraph_FaceRefId(theOldFaceRef));
       aFaceRefIt.More();
       aFaceRefIt.Next())
  {
    theGraph.allocateRefUID(aFaceRefIt.CurrentId());
  }
  for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph, BRepGraph_WireRefId(theOldWireRef));
       aWireRefIt.More();
       aWireRefIt.Next())
  {
    theGraph.allocateRefUID(aWireRefIt.CurrentId());
  }
  for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph,
                                                    BRepGraph_VertexRefId(theOldVertexRef));
       aVertexRefIt.More();
       aVertexRefIt.Next())
  {
    theGraph.allocateRefUID(aVertexRefIt.CurrentId());
  }
  for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph, BRepGraph_SolidRefId(theOldSolidRef));
       aSolidRefIt.More();
       aSolidRefIt.Next())
  {
    theGraph.allocateRefUID(aSolidRefIt.CurrentId());
  }
  for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph, BRepGraph_ChildRefId(theOldChildRef));
       aChildRefIt.More();
       aChildRefIt.Next())
  {
    theGraph.allocateRefUID(aChildRefIt.CurrentId());
  }
}

BRepGraph::ShapesView::AddStatus BRepGraph::ShapesView::appendImpl(
  BRepGraph&                                  theGraph,
  const TopoDS_Shape&                         theShape,
  const BRepGraph::ShapesView::Options&       theOptions,
  NCollection_LinearVector<BRepGraph_NodeId>* theOutFlatRoots)
{
  BRepGraphInc_Storage& aStorage        = theGraph.myData->myIncStorage;
  const uint32_t        anOldVtx        = aStorage.NbVertices();
  const uint32_t        anOldEdge       = aStorage.NbEdges();
  const uint32_t        anOldCoEdge     = aStorage.NbCoEdges();
  const uint32_t        anOldWire       = aStorage.NbWires();
  const uint32_t        anOldFace       = aStorage.NbFaces();
  const uint32_t        anOldShell      = aStorage.NbShells();
  const uint32_t        anOldSolid      = aStorage.NbSolids();
  const uint32_t        anOldComp       = aStorage.NbCompounds();
  const uint32_t        anOldCS         = aStorage.NbCompSolids();
  const uint32_t        anOldProduct    = aStorage.NbProducts();
  const uint32_t        anOldOccurrence = aStorage.NbOccurrences();
  const uint32_t        anOldShellRef   = aStorage.NbShellRefs();
  const uint32_t        anOldFaceRef    = aStorage.NbFaceRefs();
  const uint32_t        anOldWireRef    = aStorage.NbWireRefs();
  const uint32_t        anOldVertexRef  = aStorage.NbVertexRefs();
  const uint32_t        anOldSolidRef   = aStorage.NbSolidRefs();
  const uint32_t        anOldChildRef   = aStorage.NbChildRefs();

  BRepGraphInc_Populate::BuildStatus aBuildStatus;
  if (theOptions.Flatten)
  {
    NCollection_LinearVector<BRepGraph_NodeId> aAppendedRoots(8);
    aBuildStatus = BRepGraphInc_Populate::AppendFlattened(theGraph,
                                                          theShape,
                                                          theOptions.Parallel,
                                                          aAppendedRoots,
                                                          theOptions.Populate);
    if (theOutFlatRoots != nullptr)
    {
      for (const BRepGraph_NodeId& anId : aAppendedRoots)
      {
        theOutFlatRoots->Append(anId);
      }
    }
  }
  else
  {
    aBuildStatus =
      BRepGraphInc_Populate::Append(theGraph, theShape, theOptions.Parallel, theOptions.Populate);
  }

  // Map internal BuildStatus to public AddStatus.
  AddStatus aAddStatus = AddStatus::Failed;
  switch (aBuildStatus)
  {
    case BRepGraphInc_Populate::BuildStatus::Failed:
      aAddStatus = AddStatus::Failed;
      break;
    case BRepGraphInc_Populate::BuildStatus::SuccessWithWarnings:
      aAddStatus = AddStatus::SuccessWithWarnings;
      break;
    case BRepGraphInc_Populate::BuildStatus::Success:
      aAddStatus = AddStatus::Success;
      break;
  }

  theGraph.myData->myIncStorage.ClearCurrentShapes();

  populateUIDsIncremental(theGraph,
                          anOldVtx,
                          anOldEdge,
                          anOldCoEdge,
                          anOldWire,
                          anOldFace,
                          anOldShell,
                          anOldSolid,
                          anOldComp,
                          anOldCS,
                          anOldProduct,
                          anOldOccurrence,
                          anOldShellRef,
                          anOldFaceRef,
                          anOldWireRef,
                          anOldVertexRef,
                          anOldSolidRef,
                          anOldChildRef);

  assertMutationBoundary(theGraph, "BRepGraph::ShapesView::Add: post-append mutation boundary");
  return aAddStatus;
}

//=================================================================================================

void BRepGraph::ShapesView::collectAddedNodes(
  const BRepGraph&                                                              theGraph,
  const TopoDS_Shape&                                                           theShape,
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theMap)
{
  if (theShape.IsNull())
  {
    return;
  }

  const BRepGraph::ShapesView& aShapes = theGraph.Shapes();

  // Helper: record (shape -> NodeId) when the shape resolves to a node.
  auto bind = [&](const TopoDS_Shape& aSub) {
    if (aSub.IsNull() || theMap.IsBound(aSub))
    {
      return;
    }
    const BRepGraph_NodeId aNode = aShapes.FindNode(aSub);
    if (aNode.IsValid())
    {
      theMap.Bind(aSub, aNode);
    }
  };

  // Bind the root then enumerate every subshape kind.
  bind(theShape);

  static const TopAbs_ShapeEnum aKinds[] = {TopAbs_COMPOUND,
                                            TopAbs_COMPSOLID,
                                            TopAbs_SOLID,
                                            TopAbs_SHELL,
                                            TopAbs_FACE,
                                            TopAbs_WIRE,
                                            TopAbs_EDGE,
                                            TopAbs_VERTEX};
  for (const TopAbs_ShapeEnum aKind : aKinds)
  {
    for (TopExp_Explorer anExp(theShape, aKind); anExp.More(); anExp.Next())
    {
      bind(anExp.Current());
    }
  }
}

static void bindSourceShapeAlias(BRepGraph&             theGraph,
                                 BRepGraphInc_Storage&  theStorage,
                                 const TopoDS_Shape&    theSourceShape,
                                 const TopLoc_Location& theSourceLocation,
                                 const TopoDS_Shape&    thePopulatedShape,
                                 const TopLoc_Location& theDefinitionLocation)
{
  if (theSourceShape.IsNull() || thePopulatedShape.IsNull())
  {
    return;
  }

  TopoDS_Shape aLookup = thePopulatedShape;
  aLookup.Location(theDefinitionLocation);
  const BRepGraph_NodeId aNodeId = theGraph.Shapes().FindNode(aLookup);
  if (!aNodeId.IsValid())
  {
    return;
  }

  theStorage.SetDefinitionShapeBinding(theSourceShape, aNodeId);
  if (!theSourceShape.Location().IsEqual(theSourceLocation))
  {
    TopoDS_Shape aSourceAlias = theSourceShape;
    aSourceAlias.Location(theSourceLocation);
    theStorage.SetDefinitionShapeBinding(aSourceAlias, aNodeId);
  }
}

static void bindSourceShapeAliasesRecursive(BRepGraph&             theGraph,
                                            BRepGraphInc_Storage&  theStorage,
                                            const TopoDS_Shape&    theSourceShape,
                                            const TopoDS_Shape&    thePopulatedShape,
                                            const TopLoc_Location& theSourceLocation,
                                            const TopLoc_Location& theDefinitionLocation)
{
  bindSourceShapeAlias(theGraph,
                       theStorage,
                       theSourceShape,
                       theSourceLocation,
                       thePopulatedShape,
                       theDefinitionLocation);

  const bool      isCompoundParent = thePopulatedShape.ShapeType() == TopAbs_COMPOUND;
  TopoDS_Iterator aSourceIt(theSourceShape, false, false);
  TopoDS_Iterator aPopulatedIt(thePopulatedShape, false, false);
  for (; aSourceIt.More() && aPopulatedIt.More(); aSourceIt.Next(), aPopulatedIt.Next())
  {
    const TopoDS_Shape& aSourceChild    = aSourceIt.Value();
    const TopoDS_Shape& aPopulatedChild = aPopulatedIt.Value();
    if (aSourceChild.IsNull() || aPopulatedChild.IsNull())
    {
      continue;
    }

    const TopLoc_Location aChildSourceLocation = theSourceLocation * aSourceChild.Location();
    if (isCompoundParent)
    {
      const TopoDS_Shape aChildDefinition = shapeWithoutOwnLocation(aPopulatedChild);
      bindSourceShapeAliasesRecursive(theGraph,
                                      theStorage,
                                      aSourceChild,
                                      aChildDefinition,
                                      aChildSourceLocation,
                                      TopLoc_Location());
    }
    else
    {
      bindSourceShapeAliasesRecursive(theGraph,
                                      theStorage,
                                      aSourceChild,
                                      aPopulatedChild,
                                      aChildSourceLocation,
                                      theDefinitionLocation * aPopulatedChild.Location());
    }
  }
}

void BRepGraph::ShapesView::bindSourceShapeAliases(BRepGraph&          theGraph,
                                                   const TopoDS_Shape& theSourceShape,
                                                   const TopoDS_Shape& thePopulatedShape)
{
  bindSourceShapeAliasesRecursive(theGraph,
                                  theGraph.myData->myIncStorage,
                                  theSourceShape,
                                  thePopulatedShape,
                                  theSourceShape.Location(),
                                  thePopulatedShape.Location());
}

//=================================================================================================

namespace
{
static TopoDS_Shape reconstructProductLocal(BRepGraph_ReconstructionContext& theContext,
                                            const BRepGraph_ProductId        theProduct);

static TopoDS_Shape reconstructOccurrenceLocal(BRepGraph_ReconstructionContext& theContext,
                                               const BRepGraph_OccurrenceId     theOccurrence,
                                               const TopLoc_Location&           theLocalLocation)
{
  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return TopoDS_Shape();
  }

  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (aStorage.IsRemoved(theOccurrence) || !anOccurrence.ChildNodeId.IsValid())
  {
    return TopoDS_Shape();
  }

  TopoDS_Shape aShape;
  if (anOccurrence.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    aShape = reconstructProductLocal(theContext, BRepGraph_ProductId(anOccurrence.ChildNodeId));
  }
  else
  {
    aShape =
      BRepGraphInc_Reconstruct::Node(*theContext.Graph, anOccurrence.ChildNodeId, theContext.Cache);
  }
  if (!aShape.IsNull() && !theLocalLocation.IsIdentity())
  {
    aShape.Move(theLocalLocation);
  }
  return aShape;
}

static TopoDS_Shape reconstructProductLocal(BRepGraph_ReconstructionContext& theContext,
                                            const BRepGraph_ProductId        theProduct)
{
  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return TopoDS_Shape();
  }

  const BRepGraph_NodeId aProductNode = theProduct;
  if (const TopoDS_Shape* aCached = theContext.Cache.Seek(aProductNode))
  {
    return *aCached;
  }

  if (aStorage.IsRemoved(theProduct) || theContext.ActiveProducts.Contains(theProduct))
  {
    return TopoDS_Shape();
  }

  theContext.ActiveProducts.Add(theProduct);

  // Find the shape root: scan occurrence refs for a topology (non-product) child.
  TopoDS_Shape     aResult;
  BRepGraph_NodeId aShapeRootNode;
  TopLoc_Location  aRootLocation;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*theContext.Graph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    const BRepGraphInc::OccurrenceDef& aDef = aStorage.Occurrence(aRef.ChildOccurrenceId);
    if (aDef.ChildNodeId.IsValid() && aDef.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
    {
      aShapeRootNode = aDef.ChildNodeId;
      aRootLocation  = aRef.LocalLocation;
      break;
    }
  }
  if (aShapeRootNode.IsValid())
  {
    aResult = BRepGraphInc_Reconstruct::Node(*theContext.Graph, aShapeRootNode, theContext.Cache);
    if (!aResult.IsNull() && !aRootLocation.IsIdentity())
    {
      aResult.Move(aRootLocation);
    }
  }
  else
  {
    BRep_Builder    aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);

    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*theContext.Graph, theProduct); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& anOccurrenceRef =
        aStorage.OccurrenceRef(anOccIt.CurrentId());
      if (!anOccurrenceRef.ChildOccurrenceId.IsValid(aStorage.NbOccurrences()))
      {
        continue;
      }

      if (aStorage.IsRemoved(anOccurrenceRef.ChildOccurrenceId))
      {
        continue;
      }

      TopoDS_Shape aChild = reconstructOccurrenceLocal(theContext,
                                                       anOccurrenceRef.ChildOccurrenceId,
                                                       anOccurrenceRef.LocalLocation);
      if (!aChild.IsNull())
      {
        aBuilder.Add(aCompound, aChild);
      }
    }

    aResult = aCompound;
  }

  theContext.ActiveProducts.Remove(theProduct);
  if (!aResult.IsNull())
  {
    theContext.Cache.Bind(aProductNode, aResult);
  }
  return aResult;
}

static TopoDS_Shape reconstructShape(BRepGraph_ReconstructionContext& theContext,
                                     const BRepGraph_NodeId           theNode)
{
  if (!theNode.IsValid())
  {
    return TopoDS_Shape();
  }

  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return reconstructProductLocal(theContext, BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anOccurrence(theNode);
      if (!anOccurrence.IsValid(aStorage.NbOccurrences()))
      {
        return TopoDS_Shape();
      }

      const BRepGraphInc::OccurrenceDef& anOccurrenceDef = aStorage.Occurrence(anOccurrence);
      if (aStorage.IsRemoved(anOccurrence) || !anOccurrenceDef.ChildNodeId.IsValid())
      {
        return TopoDS_Shape();
      }

      TopoDS_Shape aShape;
      if (anOccurrenceDef.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        aShape =
          reconstructProductLocal(theContext, BRepGraph_ProductId(anOccurrenceDef.ChildNodeId));
      }
      else
      {
        aShape = BRepGraphInc_Reconstruct::Node(*theContext.Graph,
                                                anOccurrenceDef.ChildNodeId,
                                                theContext.Cache);
      }
      if (!aShape.IsNull())
      {
        const TopLoc_Location aGlobalLocation =
          theContext.Graph->Topo().Occurrences().OccurrenceLocation(anOccurrence);
        if (!aGlobalLocation.IsIdentity())
        {
          aShape.Move(aGlobalLocation);
        }
      }
      return aShape;
    }
    default:
      return BRepGraphInc_Reconstruct::Node(*theContext.Graph, theNode, theContext.Cache);
  }
}

static BRepGraph_ReconstructionContext makeReconstructionContext(
  BRepGraph&                  theGraph,
  const BRepGraphInc_Storage& theStorage)
{
  BRepGraph_ReconstructionContext aContext;
  aContext.Graph   = &theGraph;
  aContext.Storage = &theStorage;
  return aContext;
}
} // namespace

//=================================================================================================

void BRepGraph::ShapesView::CollectHistoryInputs(
  const NCollection_Array1<BRepGraph_NodeId>&                                   theRoots,
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutInputs) const
{
  const BRepGraph& theGraph = *myGraph;

  for (const BRepGraph_NodeId& aRootId : theRoots)
  {
    const TopoDS_Shape aRoot = theGraph.Shapes().Shape(aRootId);
    collectAddedNodes(theGraph, aRoot, theOutInputs);
  }
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::AddWithHistory(
  const TopoDS_Shape& theResultShape,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
  const occ::handle<BRepTools_History>&                                               theHistory,
  const TCollection_AsciiString&                                                      theOpLabel)
{
  Options anOptions;
  anOptions.CreateAutoProduct = false;
  return AddWithHistory(theResultShape, theInputs, theHistory, theOpLabel, anOptions);
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::AddWithHistory(
  const TopoDS_Shape& theResultShape,
  const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
  const occ::handle<BRepTools_History>&                                               theHistory,
  const TCollection_AsciiString&                                                      theOpLabel,
  const BRepGraph::ShapesView::Options&                                               theOptions)
{
  BRepGraph& theGraph = *myGraph;

  BRepGraph_LayerHistory& aHistory = *theGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const bool              shouldAbsorbHistory = aHistory.IsEnabled() && !theHistory.IsNull();
  Options                 aBuildOptions       = theOptions;
  aBuildOptions.TrackAddedNodes               = theOptions.TrackAddedNodes || shouldAbsorbHistory;

  Result aResult = Add(theResultShape, aBuildOptions);
  if (aResult.IsOk() && shouldAbsorbHistory)
  {
    aHistory.Absorb(theInputs, aResult.AddedNodes, theHistory, theOpLabel);
  }
  return aResult;
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::AddWithHistory(
  const TopoDS_Shape&                         theResultShape,
  const NCollection_Array1<BRepGraph_NodeId>& theInputRoots,
  const occ::handle<BRepTools_History>&       theHistory,
  const TCollection_AsciiString&              theOpLabel)
{
  Options anOptions;
  anOptions.CreateAutoProduct = false;
  return AddWithHistory(theResultShape, theInputRoots, theHistory, theOpLabel, anOptions);
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::AddWithHistory(
  const TopoDS_Shape&                         theResultShape,
  const NCollection_Array1<BRepGraph_NodeId>& theInputRoots,
  const occ::handle<BRepTools_History>&       theHistory,
  const TCollection_AsciiString&              theOpLabel,
  const BRepGraph::ShapesView::Options&       theOptions)
{
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  CollectHistoryInputs(theInputRoots, anInputs);
  return AddWithHistory(theResultShape, anInputs, theHistory, theOpLabel, theOptions);
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::Add(const TopoDS_Shape& theShape)
{
  return Add(theShape, Options{});
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::Add(
  const TopoDS_Shape&                   theShape,
  const BRepGraph::ShapesView::Options& theOptions)
{
  BRepGraph& theGraph = *myGraph;
  Result     aResult;
  if (theShape.IsNull())
  {
    return aResult;
  }

  const uint32_t anOldCount = snapshotCountForKind(theGraph, theShape.ShapeType());
  const bool     isRootLocationStoredInRef =
    shouldStoreRootLocationInRef(theOptions, BRepGraph_NodeId());
  const TopoDS_Shape aPopulateShape =
    isRootLocationStoredInRef ? shapeWithoutOwnLocation(theShape) : theShape;

  NCollection_LinearVector<BRepGraph_NodeId> aFlatRoots;
  const AddStatus                            aAddStatus =
    appendImpl(theGraph, aPopulateShape, theOptions, theOptions.Flatten ? &aFlatRoots : nullptr);

  aResult.Status = aAddStatus;

  if (theOptions.Flatten && !aFlatRoots.IsEmpty())
  {
    aResult.TopologyRoot = aFlatRoots.First();
  }
  else
  {
    aResult.TopologyRoot = detectTopologyRoot(theGraph, theShape.ShapeType(), anOldCount);
  }
  if (!aResult.TopologyRoot.IsValid())
  {
    aResult.TopologyRoot = FindNode(aPopulateShape);
  }
  if (isRootLocationStoredInRef && aResult.TopologyRoot.IsValid())
  {
    bindSourceShapeAliases(theGraph, theShape, aPopulateShape);
  }

  if (theOptions.CreateAutoProduct && aResult.TopologyRoot.IsValid())
  {
    aResult.Product = theGraph.Editor().Products().Add(aResult.TopologyRoot, theShape.Location());
    theGraph.Editor().Products().AppendDocumentRoot(aResult.Product);
    if (aResult.Product.IsValid())
    {
      const BRepGraphInc::ProductRelations& aProductRel =
        theGraph.myData->myIncStorage.ProductRelations(aResult.Product);
      if (!aProductRel.OccurrenceRefIds.IsEmpty())
      {
        const BRepGraph_OccurrenceRefId anOccRefId = aProductRel.OccurrenceRefIds.First();
        const BRepGraph_OccurrenceId    anOccId =
          theGraph.myData->myIncStorage.OccurrenceRef(anOccRefId).ChildOccurrenceId;
        aResult.Occurrence = anOccId;
      }
    }
  }

  if (!aResult.TopologyRoot.IsValid()
      && !(theOptions.CreateAutoProduct && aResult.Product.IsValid()))
  {
    aResult.Status = AddStatus::Failed;
  }
  if (theOptions.TrackAddedNodes && aResult.TopologyRoot.IsValid())
  {
    collectAddedNodes(theGraph, theShape, aResult.AddedNodes);
  }
  return aResult;
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::Add(const TopoDS_Shape&    theShape,
                                                         const BRepGraph_NodeId theParent)
{
  return Add(theShape, theParent, Options{});
}

//=================================================================================================

BRepGraph::ShapesView::Result BRepGraph::ShapesView::Add(
  const TopoDS_Shape&                   theShape,
  const BRepGraph_NodeId                theParent,
  const BRepGraph::ShapesView::Options& theOptions)
{
  BRepGraph& theGraph = *myGraph;
  Result     aResult;
  if (theShape.IsNull() || !theParent.IsValid())
  {
    return aResult;
  }

  const ShapeParentRoute aRoute =
    classifyShapeToParent(theShape.ShapeType(), theShape.Orientation(), theParent);
  if (aRoute == ShapeParentRoute::Reject)
  {
    return aResult;
  }
  if (aRoute == ShapeParentRoute::Supplement)
  {
    if (attachSupplementToParent(theGraph, theShape, theParent) != 0)
    {
      aResult.Status = AddStatus::Success;
    }
    return aResult;
  }

  const uint32_t anOldCount                = snapshotCountForKind(theGraph, theShape.ShapeType());
  const bool     isRootLocationStoredInRef = shouldStoreRootLocationInRef(theOptions, theParent);
  const TopoDS_Shape aPopulateShape =
    isRootLocationStoredInRef ? shapeWithoutOwnLocation(theShape) : theShape;

  Options anInner           = theOptions;
  anInner.CreateAutoProduct = false;

  NCollection_LinearVector<BRepGraph_NodeId> aFlatRoots;
  const AddStatus                            aStatus =
    appendImpl(theGraph, aPopulateShape, anInner, anInner.Flatten ? &aFlatRoots : nullptr);

  if (aStatus == AddStatus::Failed)
  {
    return aResult;
  }

  if (anInner.Flatten && !aFlatRoots.IsEmpty())
  {
    aResult.TopologyRoot = aFlatRoots.First();
  }
  else
  {
    aResult.TopologyRoot = detectTopologyRoot(theGraph, theShape.ShapeType(), anOldCount);
  }
  if (!aResult.TopologyRoot.IsValid())
  {
    aResult.TopologyRoot = FindNode(aPopulateShape);
  }
  if (!aResult.TopologyRoot.IsValid())
  {
    return aResult;
  }
  if (isRootLocationStoredInRef)
  {
    bindSourceShapeAliases(theGraph, theShape, aPopulateShape);
  }

  if (anInner.TrackAddedNodes)
  {
    collectAddedNodes(theGraph, theShape, aResult.AddedNodes);
  }

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId aChildProduct =
        theGraph.Editor().Products().Add(aResult.TopologyRoot, TopLoc_Location());
      if (!aChildProduct.IsValid())
      {
        return aResult;
      }

      BRepGraph_OccurrenceRefId    anOccRefId;
      const BRepGraph_OccurrenceId anOccId =
        theGraph.Editor().Products().Append(BRepGraph_ProductId(theParent),
                                            aChildProduct,
                                            theShape.Location(),
                                            BRepGraph_OccurrenceId(),
                                            &anOccRefId);
      if (!anOccId.IsValid())
      {
        return aResult;
      }
      aResult.Product     = aChildProduct;
      aResult.Occurrence  = anOccId;
      aResult.InsertedRef = anOccRefId;
      aResult.Status      = AddStatus::Success;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const TopAbs_Orientation   aShapeOri = theShape.Orientation();
      const BRepGraph_ChildRefId aRid =
        theGraph.Editor().Compounds().Append(BRepGraph_CompoundId(theParent),
                                             aResult.TopologyRoot,
                                             aShapeOri);
      if (!aRid.IsValid())
      {
        return aResult;
      }
      theGraph.Editor().Gen().SetChildRefLocalLocation(aRid, theShape.Location());
      aResult.InsertedRef = aRid;
      aResult.Status      = AddStatus::Success;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const TopAbs_Orientation  aShapeOri = theShape.Orientation();
      const BRepGraph_ShellId   aShell(theParent);
      const BRepGraph_FaceRefId aRid =
        theGraph.Editor().Shells().Append(aShell,
                                          BRepGraph_FaceId(aResult.TopologyRoot),
                                          aShapeOri);
      if (!aRid.IsValid())
      {
        return aResult;
      }
      aResult.InsertedRef = aRid;
      aResult.Status      = AddStatus::Success;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId aSolid(theParent);
      if (aResult.TopologyRoot.NodeKind == BRepGraph_NodeId::Kind::Shell)
      {
        const TopAbs_Orientation   aShapeOri = theShape.Orientation();
        const BRepGraph_ShellRefId aRid =
          theGraph.Editor().Solids().Append(aSolid,
                                            BRepGraph_ShellId(aResult.TopologyRoot),
                                            aShapeOri);
        if (!aRid.IsValid())
        {
          return aResult;
        }
        aResult.InsertedRef = aRid;
        aResult.Status      = AddStatus::Success;
        return aResult;
      }
      return aResult;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (aResult.TopologyRoot.NodeKind != BRepGraph_NodeId::Kind::Solid)
      {
        return aResult;
      }
      const TopAbs_Orientation   aShapeOri = theShape.Orientation();
      const BRepGraph_SolidRefId aRid =
        theGraph.Editor().CompSolids().Append(BRepGraph_CompSolidId(theParent),
                                              BRepGraph_SolidId(aResult.TopologyRoot),
                                              aShapeOri);
      if (!aRid.IsValid())
      {
        return aResult;
      }
      aResult.InsertedRef = aRid;
      aResult.Status      = AddStatus::Success;
      return aResult;
    }
    default:
      return aResult;
  }
}

//=================================================================================================

void BRepGraph::ShapesView::ClearCached(const BRepGraph_NodeId theNode)
{
  if (myGraph == nullptr || !theNode.IsValid() || myGraph->Topo().Gen().IsRemoved(theNode))
  {
    return;
  }

  myGraph->myData->myIncStorage.UnbindCurrentShape(theNode);
}

//=================================================================================================

void BRepGraph::ShapesView::ClearCached(const BRepGraph_RefId theRef)
{
  if (myGraph == nullptr || !theRef.IsValid() || myGraph->Refs().Gen().IsRemoved(theRef))
  {
    return;
  }

  const BRepGraph_NodeId aChildNode = myGraph->Refs().Gen().ChildNode(theRef);
  ClearCached(aChildNode);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Shape(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return TopoDS_Shape();
  }

  if (theNode.IsRemoved(*myGraph))
  {
    return TopoDS_Shape();
  }
  // Fast path: if entity was never mutated, return the original shape.
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return TopoDS_Shape();
  }
  const uint32_t aSubtreeGen = aDef->SubtreeGen;
  if (aSubtreeGen == 0)
  {
    TopoDS_Shape anOrig = Original(theNode);
    if (!anOrig.IsNull())
    {
      return anOrig;
    }
  }

  // Check mutable cache under shared lock with SubtreeGen validation.
  {
    std::shared_lock<std::shared_mutex> aReadLock(
      myGraph->myData->myIncStorage.CurrentShapesMutex());
    const BRepGraphInc_Storage::CachedShape* aCached =
      myGraph->myData->myIncStorage.CurrentShapes().Seek(theNode);
    if (aCached != nullptr && aCached->StoredSubtreeGen == aSubtreeGen)
    {
      return aCached->Shape;
    }
  }

  // Reconstruct from incidence storage / assembly facade.
  BRepGraph_ReconstructionContext aContext =
    makeReconstructionContext(*myGraph, myGraph->myData->myIncStorage);
  TopoDS_Shape aReconstructed = reconstructShape(aContext, theNode);

  // Store under exclusive lock with double-check to avoid redundant writes
  // when multiple threads reconstruct the same parent node concurrently.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(
      myGraph->myData->myIncStorage.CurrentShapesMutex());
    BRepGraphInc_Storage::CachedShape* aExisting =
      myGraph->myData->myIncStorage.ChangeCurrentShapes().ChangeSeek(theNode);
    if (aExisting != nullptr && aExisting->StoredSubtreeGen == aSubtreeGen)
    {
      return aExisting->Shape;
    }
    BRepGraphInc_Storage::CachedShape anEntry;
    anEntry.Shape            = aReconstructed;
    anEntry.StoredSubtreeGen = aSubtreeGen;
    if (aExisting != nullptr)
    {
      *aExisting = anEntry;
    }
    else
    {
      myGraph->myData->myIncStorage.ChangeCurrentShapes().Bind(theNode, anEntry);
    }
    if (theNode.NodeKind != BRepGraph_NodeId::Kind::Product
        && theNode.NodeKind != BRepGraph_NodeId::Kind::Occurrence)
    {
      myGraph->myData->myIncStorage.SetDefinitionShapeBinding(aReconstructed, theNode);
    }
  }
  return aReconstructed;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasOriginal(const BRepGraph_NodeId theNode) const
{
  return !Original(theNode).IsNull();
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Original(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return TopoDS_Shape();
  }

  if (theNode.IsRemoved(*myGraph))
  {
    return TopoDS_Shape();
  }

  return myGraph->myData->myIncStorage.FindOriginal(theNode);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Reconstruct(const BRepGraph_NodeId theRoot) const
{
  if (!theRoot.IsValid() || theRoot.IsRemoved(*myGraph))
  {
    return TopoDS_Shape();
  }
  BRepGraph_ReconstructionContext aContext =
    makeReconstructionContext(*myGraph, myGraph->myData->myIncStorage);
  TopoDS_Shape aResult = reconstructShape(aContext, theRoot);
  return aResult;
}

//=================================================================================================

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::FindNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
  {
    return BRepGraph_NodeId();
  }

  const BRepGraph_NodeId aNodeId = myGraph->myData->myIncStorage.FindDefinitionByShape(theShape);
  if (aNodeId.IsValid())
  {
    if (aNodeId.IsRemoved(*myGraph))
    {
      return BRepGraph_NodeId();
    }
    const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(aNodeId);
    if (aDef == nullptr)
    {
      return BRepGraph_NodeId();
    }
    return aNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::ShapesView::HasNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
  {
    return false;
  }

  return FindNode(theShape).IsValid();
}

//=================================================================================================

bool BRepGraph::ShapesView::RemoveShape(const TopoDS_Shape& theShape)
{
  const BRepGraph_NodeId aNode = FindNode(theShape);
  if (!aNode.IsValid())
  {
    return false;
  }

  myGraph->Editor().Gen().RemoveNode(aNode);
  return true;
}
