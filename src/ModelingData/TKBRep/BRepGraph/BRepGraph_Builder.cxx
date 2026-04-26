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

#include <BRepGraph_Builder.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_IncAllocator.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

namespace
{

//=================================================================================================

static void assertMutationBoundary(BRepGraph& theGraph, const char* theContext)
{
  (void)theContext;
  const bool isValid = theGraph.Editor().ValidateMutationBoundary();
  Standard_ASSERT_VOID(isValid, theContext);
  (void)isValid;
}

} // namespace

//=================================================================================================

uint32_t BRepGraph_Builder::snapshotCountForKind(const BRepGraph&       theGraph,
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

BRepGraph_NodeId BRepGraph_Builder::detectTopologyRoot(const BRepGraph&       theGraph,
                                                       const TopAbs_ShapeEnum theShapeType,
                                                       const uint32_t theOldCountOfShapeKind)
{
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  const uint32_t              aNewCount =
    snapshotCountForKind(theGraph, theShapeType);
  if (aNewCount <= theOldCountOfShapeKind)
    return BRepGraph_NodeId();

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
      (void)aStorage;
      return BRepGraph_NodeId();
  }
}

//=================================================================================================

void BRepGraph_Builder::createRootProductForTopology(
  BRepGraph&                 theGraph,
  const BRepGraph_NodeId     theTopologyRoot,
  const TopLoc_Location&     thePlacement,
  const bool                 theRegisterAsRoot,
  BRepGraph_ProductId&       theOutProduct,
  BRepGraph_OccurrenceId&    theOutOccurrence,
  BRepGraph_OccurrenceRefId& theOutOccurrenceRef)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  const BRepGraph_ProductId aProductId = aStorage.AppendProduct();
  BRepGraphInc::ProductDef& aProduct   = aStorage.ChangeProduct(aProductId);
  theGraph.allocateUID(aProductId);

  if (theTopologyRoot.IsValid())
  {
    const BRepGraph_OccurrenceId anOccId  = aStorage.AppendOccurrence();
    BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
    anOccDef.ChildDefId                   = theTopologyRoot;
    theGraph.allocateUID(anOccId);

    const BRepGraph_OccurrenceRefId anOccRefId = aStorage.AppendOccurrenceRef();
    BRepGraphInc::OccurrenceRef&    anOccRef   = aStorage.ChangeOccurrenceRef(anOccRefId);
    anOccRef.ParentId                          = BRepGraph_NodeId(aProductId);
    anOccRef.OccurrenceDefId                   = anOccId;
    anOccRef.LocalLocation                     = thePlacement;
    theGraph.allocateRefUID(anOccRefId);
    aProduct.OccurrenceRefIds.Append(anOccRefId);

    theOutOccurrence    = anOccId;
    theOutOccurrenceRef = anOccRefId;
  }

  if (theRegisterAsRoot)
  {
    theGraph.myData->myRootProductIds.Append(aProductId);
  }

  theOutProduct = aProductId;
}

//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (BRepGraph_FullVertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
    theGraph.allocateUID(aVertexIt.CurrentId());
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
    theGraph.allocateUID(anEdgeIt.CurrentId());
  for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(theGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
    theGraph.allocateUID(aCoEdgeIt.CurrentId());
  for (BRepGraph_FullWireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
    theGraph.allocateUID(aWireIt.CurrentId());
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    theGraph.allocateUID(aFaceIt.CurrentId());
  for (BRepGraph_FullShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
    theGraph.allocateUID(aShellIt.CurrentId());
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
    theGraph.allocateUID(aSolidIt.CurrentId());
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph); aCompoundIt.More(); aCompoundIt.Next())
    theGraph.allocateUID(aCompoundIt.CurrentId());
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph); aCompSolidIt.More();
       aCompSolidIt.Next())
    theGraph.allocateUID(aCompSolidIt.CurrentId());
  for (BRepGraph_FullProductIterator aProductIt(theGraph); aProductIt.More(); aProductIt.Next())
    theGraph.allocateUID(aProductIt.CurrentId());
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph); anOccurrenceIt.More();
       anOccurrenceIt.Next())
    theGraph.allocateUID(anOccurrenceIt.CurrentId());

  for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph); aShellRefIt.More(); aShellRefIt.Next())
    theGraph.allocateRefUID(aShellRefIt.CurrentId());
  for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph); aFaceRefIt.More(); aFaceRefIt.Next())
    theGraph.allocateRefUID(aFaceRefIt.CurrentId());
  for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph); aWireRefIt.More(); aWireRefIt.Next())
    theGraph.allocateRefUID(aWireRefIt.CurrentId());
  for (BRepGraph_FullCoEdgeRefIterator aCoEdgeRefIt(theGraph); aCoEdgeRefIt.More();
       aCoEdgeRefIt.Next())
    theGraph.allocateRefUID(aCoEdgeRefIt.CurrentId());
  for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph); aVertexRefIt.More();
       aVertexRefIt.Next())
    theGraph.allocateRefUID(aVertexRefIt.CurrentId());
  for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More(); aSolidRefIt.Next())
    theGraph.allocateRefUID(aSolidRefIt.CurrentId());
  for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More(); aChildRefIt.Next())
    theGraph.allocateRefUID(aChildRefIt.CurrentId());
}

//=================================================================================================

void BRepGraph_Builder::appendImpl(BRepGraph&          theGraph,
                                   const TopoDS_Shape& theShape,
                                   const Options&      theOptions)
{
  BRepGraphInc_Storage& aStorage        = theGraph.myData->myIncStorage;
  const int             anOldVtx        = aStorage.NbVertices();
  const int             anOldEdge       = aStorage.NbEdges();
  const int             anOldCoEdge     = aStorage.NbCoEdges();
  const int             anOldWire       = aStorage.NbWires();
  const int             anOldFace       = aStorage.NbFaces();
  const int             anOldShell      = aStorage.NbShells();
  const int             anOldSolid      = aStorage.NbSolids();
  const int             anOldComp       = aStorage.NbCompounds();
  const int             anOldCS         = aStorage.NbCompSolids();
  const int             anOldProduct    = aStorage.NbProducts();
  const int             anOldOccurrence = aStorage.NbOccurrences();
  const int             anOldShellRef   = aStorage.NbShellRefs();
  const int             anOldFaceRef    = aStorage.NbFaceRefs();
  const int             anOldWireRef    = aStorage.NbWireRefs();
  const int             anOldCoEdgeRef  = aStorage.NbCoEdgeRefs();
  const int             anOldVertexRef  = aStorage.NbVertexRefs();
  const int             anOldSolidRef   = aStorage.NbSolidRefs();
  const int             anOldChildRef   = aStorage.NbChildRefs();

  occ::handle<NCollection_IncAllocator>   aTmpAlloc = new NCollection_IncAllocator;
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();

  if (theOptions.Flatten)
  {
    NCollection_DynamicArray<BRepGraph_NodeId> aAppendedRoots(8, theGraph.Allocator());
    BRepGraphInc_Populate::AppendFlattened(aStorage,
                                           theShape,
                                           theOptions.Parallel,
                                           aAppendedRoots,
                                           theOptions.Populate,
                                           aParamLayer.get(),
                                           aRegularityLayer.get(),
                                           aTmpAlloc);
  }
  else
  {
    BRepGraphInc_Populate::Append(aStorage,
                                  theShape,
                                  theOptions.Parallel,
                                  theOptions.Populate,
                                  aParamLayer.get(),
                                  aRegularityLayer.get(),
                                  aTmpAlloc);
  }

  if (!aStorage.GetIsDone())
    return;

  theGraph.myData->myCurrentShapes.Clear();

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
                          anOldCoEdgeRef,
                          anOldVertexRef,
                          anOldSolidRef,
                          anOldChildRef);

  theGraph.myData->myIsDone = true;

  assertMutationBoundary(theGraph, "BRepGraph_Builder::Add: post-append mutation boundary");
}

//=================================================================================================

BRepGraph_Builder::Result BRepGraph_Builder::Add(BRepGraph&          theGraph,
                                                 const TopoDS_Shape& theShape)
{
  return Add(theGraph, theShape, Options{});
}

//=================================================================================================

BRepGraph_Builder::Result BRepGraph_Builder::Add(BRepGraph&          theGraph,
                                                 const TopoDS_Shape& theShape,
                                                 const Options&      theOptions)
{
  Result aResult;
  if (theShape.IsNull())
    return aResult;

  const uint32_t anOldCount = snapshotCountForKind(theGraph, theShape.ShapeType());

  appendImpl(theGraph, theShape, theOptions);

  if (!theGraph.myData->myIncStorage.GetIsDone())
    return aResult;

  aResult.TopologyRoot = detectTopologyRoot(theGraph, theShape.ShapeType(), anOldCount);

  if (theOptions.CreateAutoProduct)
  {
    BRepGraph_OccurrenceId    anOccId;
    BRepGraph_OccurrenceRefId anOccRefId;
    createRootProductForTopology(theGraph,
                                 aResult.TopologyRoot,
                                 theShape.Location(),
                                 /*registerAsRoot*/ true,
                                 aResult.Product,
                                 anOccId,
                                 anOccRefId);
    aResult.Occurrence = BRepGraph_NodeId(anOccId);
    theGraph.myData->myIncStorage.BuildReverseIndex();
  }

  // Pre-allocate transient cache for lock-free parallel access.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
    int                   aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]    = aStorage.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]      = aStorage.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]    = aStorage.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]      = aStorage.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]      = aStorage.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]     = aStorage.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]     = aStorage.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]  = aStorage.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)] = aStorage.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]   = aStorage.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
    int       aReservedKindCount   = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    theGraph.myTransientCache.Reserve(aReservedKindCount, aCounts);
  }

  aResult.Ok = aResult.TopologyRoot.IsValid()
               || (theOptions.CreateAutoProduct && aResult.Product.IsValid());
  return aResult;
}

//=================================================================================================

BRepGraph_Builder::Result BRepGraph_Builder::Add(BRepGraph&             theGraph,
                                                 const TopoDS_Shape&    theShape,
                                                 const BRepGraph_NodeId theParent)
{
  return Add(theGraph, theShape, theParent, Options{});
}

//=================================================================================================

BRepGraph_Builder::Result BRepGraph_Builder::Add(BRepGraph&             theGraph,
                                                 const TopoDS_Shape&    theShape,
                                                 const BRepGraph_NodeId theParent,
                                                 const Options&         theOptions)
{
  Result aResult;
  if (theShape.IsNull() || !theParent.IsValid())
    return aResult;

  const uint32_t anOldCount = snapshotCountForKind(theGraph, theShape.ShapeType());

  Options anInner          = theOptions;
  anInner.CreateAutoProduct = false;
  appendImpl(theGraph, theShape, anInner);

  if (!theGraph.myData->myIncStorage.GetIsDone())
    return aResult;

  aResult.TopologyRoot = detectTopologyRoot(theGraph, theShape.ShapeType(), anOldCount);
  if (!aResult.TopologyRoot.IsValid())
    return aResult;

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product: {
      BRepGraph_ProductId       aChildProduct;
      BRepGraph_OccurrenceId    anUnusedOccurrence;
      BRepGraph_OccurrenceRefId anUnusedOccurrenceRef;
      createRootProductForTopology(theGraph,
                                   aResult.TopologyRoot,
                                   TopLoc_Location(),
                                   /*registerAsRoot*/ false,
                                   aChildProduct,
                                   anUnusedOccurrence,
                                   anUnusedOccurrenceRef);
      if (!aChildProduct.IsValid())
        return aResult;

      const BRepGraph_OccurrenceId anOccId =
        theGraph.Editor().Products().LinkProducts(BRepGraph_ProductId(theParent),
                                                  aChildProduct,
                                                  theShape.Location());
      if (!anOccId.IsValid())
        return aResult;
      theGraph.myData->myIncStorage.BuildReverseIndex();
      aResult.Product    = aChildProduct;
      aResult.Occurrence = BRepGraph_NodeId(anOccId);
      aResult.Ok         = true;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_ChildRefId aRid =
        theGraph.Editor().Compounds().AddChild(BRepGraph_CompoundId(theParent),
                                               aResult.TopologyRoot,
                                               theShape.Orientation());
      if (!aRid.IsValid())
        return aResult;
      aResult.InsertedRef = BRepGraph_RefId(aRid);
      aResult.Ok          = true;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId aShell(theParent);
      if (aResult.TopologyRoot.NodeKind == BRepGraph_NodeId::Kind::Face)
      {
        const BRepGraph_FaceRefId aRid =
          theGraph.Editor().Shells().AddFace(aShell,
                                             BRepGraph_FaceId(aResult.TopologyRoot),
                                             theShape.Orientation());
        if (!aRid.IsValid())
          return aResult;
        aResult.InsertedRef = BRepGraph_RefId(aRid);
        aResult.Ok          = true;
        return aResult;
      }
      const BRepGraph_ChildRefId aRid =
        theGraph.Editor().Shells().AddChild(aShell,
                                            aResult.TopologyRoot,
                                            theShape.Orientation());
      if (!aRid.IsValid())
        return aResult;
      aResult.InsertedRef = BRepGraph_RefId(aRid);
      aResult.Ok          = true;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId aSolid(theParent);
      if (aResult.TopologyRoot.NodeKind == BRepGraph_NodeId::Kind::Shell)
      {
        const BRepGraph_ShellRefId aRid =
          theGraph.Editor().Solids().AddShell(aSolid,
                                              BRepGraph_ShellId(aResult.TopologyRoot),
                                              theShape.Orientation());
        if (!aRid.IsValid())
          return aResult;
        aResult.InsertedRef = BRepGraph_RefId(aRid);
        aResult.Ok          = true;
        return aResult;
      }
      const BRepGraph_ChildRefId aRid =
        theGraph.Editor().Solids().AddChild(aSolid,
                                            aResult.TopologyRoot,
                                            theShape.Orientation());
      if (!aRid.IsValid())
        return aResult;
      aResult.InsertedRef = BRepGraph_RefId(aRid);
      aResult.Ok          = true;
      return aResult;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (aResult.TopologyRoot.NodeKind != BRepGraph_NodeId::Kind::Solid)
        return aResult;
      const BRepGraph_SolidRefId aRid =
        theGraph.Editor().CompSolids().AddSolid(BRepGraph_CompSolidId(theParent),
                                                BRepGraph_SolidId(aResult.TopologyRoot),
                                                theShape.Orientation());
      if (!aRid.IsValid())
        return aResult;
      aResult.InsertedRef = BRepGraph_RefId(aRid);
      aResult.Ok          = true;
      return aResult;
    }
    default:
      return aResult;
  }
}

//=================================================================================================

void BRepGraph_Builder::populateUIDsIncremental(BRepGraph& theGraph,
                                                const int  theOldVtx,
                                                const int  theOldEdge,
                                                const int  theOldCoEdge,
                                                const int  theOldWire,
                                                const int  theOldFace,
                                                const int  theOldShell,
                                                const int  theOldSolid,
                                                const int  theOldComp,
                                                const int  theOldCS,
                                                const int  theOldProduct,
                                                const int  theOldOccurrence,
                                                const int  theOldShellRef,
                                                const int  theOldFaceRef,
                                                const int  theOldWireRef,
                                                const int  theOldCoEdgeRef,
                                                const int  theOldVertexRef,
                                                const int  theOldSolidRef,
                                                const int  theOldChildRef)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (BRepGraph_FullVertexIterator aVertexIt(theGraph, BRepGraph_VertexId(theOldVtx));
       aVertexIt.More();
       aVertexIt.Next())
    theGraph.allocateUID(aVertexIt.CurrentId());
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph, BRepGraph_EdgeId(theOldEdge)); anEdgeIt.More();
       anEdgeIt.Next())
    theGraph.allocateUID(anEdgeIt.CurrentId());
  for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(theGraph, BRepGraph_CoEdgeId(theOldCoEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
    theGraph.allocateUID(aCoEdgeIt.CurrentId());
  for (BRepGraph_FullWireIterator aWireIt(theGraph, BRepGraph_WireId(theOldWire)); aWireIt.More();
       aWireIt.Next())
    theGraph.allocateUID(aWireIt.CurrentId());
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph, BRepGraph_FaceId(theOldFace)); aFaceIt.More();
       aFaceIt.Next())
    theGraph.allocateUID(aFaceIt.CurrentId());
  for (BRepGraph_FullShellIterator aShellIt(theGraph, BRepGraph_ShellId(theOldShell));
       aShellIt.More();
       aShellIt.Next())
    theGraph.allocateUID(aShellIt.CurrentId());
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph, BRepGraph_SolidId(theOldSolid));
       aSolidIt.More();
       aSolidIt.Next())
    theGraph.allocateUID(aSolidIt.CurrentId());
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph, BRepGraph_CompoundId(theOldComp));
       aCompoundIt.More();
       aCompoundIt.Next())
    theGraph.allocateUID(aCompoundIt.CurrentId());
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph, BRepGraph_CompSolidId(theOldCS));
       aCompSolidIt.More();
       aCompSolidIt.Next())
    theGraph.allocateUID(aCompSolidIt.CurrentId());
  for (BRepGraph_FullProductIterator aProductIt(theGraph, BRepGraph_ProductId(theOldProduct));
       aProductIt.More();
       aProductIt.Next())
    theGraph.allocateUID(aProductIt.CurrentId());
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph,
                                                       BRepGraph_OccurrenceId(theOldOccurrence));
       anOccurrenceIt.More();
       anOccurrenceIt.Next())
    theGraph.allocateUID(anOccurrenceIt.CurrentId());

  for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph, BRepGraph_ShellRefId(theOldShellRef));
       aShellRefIt.More();
       aShellRefIt.Next())
    theGraph.allocateRefUID(aShellRefIt.CurrentId());
  for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph, BRepGraph_FaceRefId(theOldFaceRef));
       aFaceRefIt.More();
       aFaceRefIt.Next())
    theGraph.allocateRefUID(aFaceRefIt.CurrentId());
  for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph, BRepGraph_WireRefId(theOldWireRef));
       aWireRefIt.More();
       aWireRefIt.Next())
    theGraph.allocateRefUID(aWireRefIt.CurrentId());
  for (BRepGraph_FullCoEdgeRefIterator aCoEdgeRefIt(theGraph,
                                                    BRepGraph_CoEdgeRefId(theOldCoEdgeRef));
       aCoEdgeRefIt.More();
       aCoEdgeRefIt.Next())
    theGraph.allocateRefUID(aCoEdgeRefIt.CurrentId());
  for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph,
                                                    BRepGraph_VertexRefId(theOldVertexRef));
       aVertexRefIt.More();
       aVertexRefIt.Next())
    theGraph.allocateRefUID(aVertexRefIt.CurrentId());
  for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph, BRepGraph_SolidRefId(theOldSolidRef));
       aSolidRefIt.More();
       aSolidRefIt.Next())
    theGraph.allocateRefUID(aSolidRefIt.CurrentId());
  for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph, BRepGraph_ChildRefId(theOldChildRef));
       aChildRefIt.More();
       aChildRefIt.Next())
    theGraph.allocateRefUID(aChildRefIt.CurrentId());
}
