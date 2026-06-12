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

#include <BRepGraph_EditorView.hxx>

#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_SupplementEditor.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRep_Builder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Assert.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>

#include <shared_mutex>

#include "../BRepGraphInc/BRepGraphInc_WireOrder.pxx"

namespace
{

template <typename IdT>
bool containsRelationId(const NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  for (const IdT& anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

template <typename IdT>
void appendUniqueRelationId(NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  if (!containsRelationId(theIds, theId))
  {
    theIds.Append(theId);
  }
}

bool isValidOccurrenceChildKind(const BRepGraph_NodeId::Kind theNodeKind)
{
  return theNodeKind == BRepGraph_NodeId::Kind::Product
         || BRepGraph_NodeId::IsTopologyKind(theNodeKind);
}

//! Check if a child node has any active parent besides theExcludedParent.
//! Uses ParentExplorer(DirectParents) which automatically skips removed parents.
bool hasOtherActiveParent(const BRepGraph&       theGraph,
                          const BRepGraph_NodeId theChild,
                          const BRepGraph_NodeId theExcludedParent)
{
  for (BRepGraph_ParentExplorer anExp(theGraph,
                                      theChild,
                                      BRepGraph_ParentExplorer::TraversalMode::DirectParents);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId != theExcludedParent)
    {
      return true;
    }
  }
  return false;
}

void removeFromRootProducts(NCollection_LinearVector<BRepGraph_ProductId>& theRoots,
                            const BRepGraph_ProductId                      theProduct)
{
  size_t aWriteIdx = 0;
  for (size_t aReadIdx = 0; aReadIdx < theRoots.Size(); ++aReadIdx)
  {
    const BRepGraph_ProductId aRoot = theRoots.Value(aReadIdx);
    if (aRoot != theProduct)
    {
      if (aWriteIdx != aReadIdx)
      {
        theRoots.ChangeValue(aWriteIdx) = aRoot;
      }
      ++aWriteIdx;
    }
  }
  while (theRoots.Size() > aWriteIdx)
  {
    theRoots.EraseLast();
  }
}

void clearCoEdgeFaceScopedRepresentations(BRepGraphInc_Storage&    theStorage,
                                          BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (theCoEdge.Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D()))
  {
    theStorage.MarkRemoved(theCoEdge.Curve2DRepId);
  }
  theCoEdge.Curve2DRepId = BRepGraph_CoEdgeCurve2DRepId();

  if (theCoEdge.Polygon2DRepId.IsValid(theStorage.NbCoEdgePolygons2D()))
  {
    theStorage.MarkRemoved(theCoEdge.Polygon2DRepId);
  }
  theCoEdge.Polygon2DRepId = BRepGraph_CoEdgePolygon2DRepId();

  if (theCoEdge.PolygonOnTriRepId.IsValid(theStorage.NbCoEdgePolygonsOnTri()))
  {
    theStorage.MarkRemoved(theCoEdge.PolygonOnTriRepId);
  }
  theCoEdge.PolygonOnTriRepId = BRepGraph_CoEdgePolygonOnTriRepId();
}

BRepGraph_NodeId refChildNode(const BRepGraph& theGraph, const BRepGraph_RefId theRef)
{
  return theGraph.Refs().Gen().ChildNode(theRef);
}

bool hasAnyActiveUsage(const BRepGraph& theGraph, const BRepGraph_NodeId theChild)
{
  if (!theChild.IsValid())
  {
    return false;
  }

  // DirectParents enumerates every active parent via relation storage across
  // all ref kinds plus the structural Edge->CoEdge and Product->Occurrence
  // links; "any direct parent exists" is exactly "any active usage exists".
  BRepGraph_ParentExplorer anExp(theGraph,
                                 theChild,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  return anExp.More();
}

template <typename RefIdT>
RefIdT findOrderedRef(const NCollection_DynamicArray<RefIdT>& theRefIds, const RefIdT theRefId)
{
  for (typename NCollection_DynamicArray<RefIdT>::Iterator anIt(theRefIds); anIt.More();
       anIt.Next())
  {
    if (anIt.Value() == theRefId)
    {
      return theRefId;
    }
  }
  return RefIdT();
}

template <typename RefIdT>
RefIdT findOrderedRef(const NCollection_LinearVector<RefIdT>& theRefIds, const RefIdT theRefId)
{
  for (const RefIdT& aRefId : theRefIds)
  {
    if (aRefId == theRefId)
    {
      return theRefId;
    }
  }
  return RefIdT();
}

template <typename RefIdT>
bool containsOrderedRef(const NCollection_DynamicArray<RefIdT>& theRefIds, const RefIdT theRefId)
{
  return findOrderedRef(theRefIds, theRefId).IsValid();
}

template <typename RefIdT>
bool containsOrderedRef(const NCollection_LinearVector<RefIdT>& theRefIds, const RefIdT theRefId)
{
  return findOrderedRef(theRefIds, theRefId).IsValid();
}

bool isOccurrenceDefOwnedByAnyProduct(const BRepGraphInc_Storage&  theStorage,
                                      const BRepGraph_OccurrenceId theOccurrence)
{
  if (!theOccurrence.IsValid(theStorage.NbOccurrences()))
  {
    return false;
  }
  for (BRepGraph_ProductId aProductId = BRepGraph_ProductId::Start();
       aProductId.IsValid(theStorage.NbProducts());
       ++aProductId)
  {
    if (!aProductId.IsValid(theStorage.NbProducts()))
    {
      continue;
    }
    if (theStorage.IsRemoved(aProductId))
    {
      continue;
    }
    for (const BRepGraph_OccurrenceRefId& aRefId :
         theStorage.ProductRelations(aProductId).OccurrenceRefIds)
    {
      if (!aRefId.IsValid(theStorage.NbOccurrenceRefs()))
      {
        continue;
      }
      if (!theStorage.IsRemoved(aRefId)
          && theStorage.OccurrenceRef(aRefId).ChildOccurrenceId == theOccurrence)
      {
        return true;
      }
    }
  }
  return false;
}

bool productHasOccurrenceDef(const BRepGraphInc_Storage&  theStorage,
                             const BRepGraph_ProductId    theProduct,
                             const BRepGraph_OccurrenceId theOccurrence)
{
  if (!theProduct.IsValid(theStorage.NbProducts())
      || !theOccurrence.IsValid(theStorage.NbOccurrences()))
  {
    return false;
  }
  if (theStorage.IsRemoved(theProduct))
  {
    return false;
  }
  for (const BRepGraph_OccurrenceRefId& aRefId :
       theStorage.ProductRelations(theProduct).OccurrenceRefIds)
  {
    if (!aRefId.IsValid(theStorage.NbOccurrenceRefs()))
    {
      continue;
    }
    if (!theStorage.IsRemoved(aRefId)
        && theStorage.OccurrenceRef(aRefId).ChildOccurrenceId == theOccurrence)
    {
      return true;
    }
  }
  return false;
}

template <typename IdT, typename FuncT>
void forEachId(const NCollection_DynamicArray<IdT>& theIds, FuncT&& theFunc)
{
  for (const IdT& anId : theIds)
  {
    theFunc(anId);
  }
}

template <typename IdT, typename FuncT>
void forEachId(const NCollection_LinearVector<IdT>& theIds, FuncT&& theFunc)
{
  for (const IdT& anId : theIds)
  {
    theFunc(anId);
  }
}

bool wireHasActiveEdgeThroughAnotherCoEdge(const BRepGraphInc_Storage& theStorage,
                                           const BRepGraph_WireId      theWire,
                                           const BRepGraph_EdgeId      theEdge,
                                           const BRepGraph_CoEdgeId    theExcludedCoEdge)
{
  if (!theWire.IsValid(theStorage.NbWires()) || theStorage.IsRemoved(theWire))
  {
    return false;
  }
  for (const BRepGraph_CoEdgeId& aCoEdgeId : theStorage.WireRelations(theWire).CoEdgeIds)
  {
    if (aCoEdgeId == theExcludedCoEdge || !aCoEdgeId.IsValid(theStorage.NbCoEdges()))
    {
      continue;
    }
    if (!theStorage.IsRemoved(aCoEdgeId) && theStorage.CoEdge(aCoEdgeId).ChildEdgeId == theEdge)
    {
      return true;
    }
  }
  return false;
}

template <typename RefIdT>
void eraseOrderedRef(const RefIdT theRefId, NCollection_DynamicArray<RefIdT>& theRefIds)
{
  uint32_t anIndex = 0;
  for (typename NCollection_DynamicArray<RefIdT>::Iterator anIt(theRefIds); anIt.More();
       anIt.Next(), ++anIndex)
  {
    if (anIt.Value() == theRefId)
    {
      for (uint32_t i = anIndex; i < static_cast<uint32_t>(theRefIds.Size()) - 1u; ++i)
      {
        theRefIds.ChangeValue(static_cast<size_t>(i)) =
          theRefIds.Value(static_cast<size_t>(i + 1u));
      }
      theRefIds.EraseLast();
      return;
    }
  }
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_VertexRefId theRefId)
{
  if (!theRefId.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_VertexId aVertexId = theStorage.VertexRef(theRefId).ChildVertexId;
  if (!aVertexId.IsValid(theStorage.NbVertices()))
  {
    return false;
  }
  for (const BRepGraph_EdgeId& anEdgeId : theStorage.VertexRelations(aVertexId).EdgeIds)
  {
    if (!anEdgeId.IsValid(theStorage.NbEdges()) || theStorage.IsRemoved(anEdgeId))
    {
      continue;
    }
    const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(anEdgeId);
    if (anEdge.StartVertexRefId == theRefId || anEdge.EndVertexRefId == theRefId)
    {
      return true;
    }
  }
  return false;
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_WireRefId   theRefId)
{
  if (!theRefId.IsValid(theStorage.NbWireRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_FaceId aParentId = theStorage.WireRef(theRefId).ParentFaceId;
  return aParentId.IsValid(theStorage.NbFaces()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.FaceRelations(aParentId).WireRefIds, theRefId);
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_FaceRefId   theRefId)
{
  if (!theRefId.IsValid(theStorage.NbFaceRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_ShellId aParentId = theStorage.FaceRef(theRefId).ParentShellId;
  return aParentId.IsValid(theStorage.NbShells()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.ShellRelations(aParentId).FaceRefIds, theRefId);
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_ShellRefId  theRefId)
{
  if (!theRefId.IsValid(theStorage.NbShellRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_SolidId aParentId = theStorage.ShellRef(theRefId).ParentSolidId;
  return aParentId.IsValid(theStorage.NbSolids()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.SolidRelations(aParentId).ShellRefIds, theRefId);
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_SolidRefId  theRefId)
{
  if (!theRefId.IsValid(theStorage.NbSolidRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_CompSolidId aParentId = theStorage.SolidRef(theRefId).ParentCompSolidId;
  return aParentId.IsValid(theStorage.NbCompSolids()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.CompSolidRelations(aParentId).SolidRefIds, theRefId);
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage& theStorage,
                           const BRepGraph_ChildRefId  theRefId)
{
  if (!theRefId.IsValid(theStorage.NbChildRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_CompoundId aParentId = theStorage.ChildRef(theRefId).ParentCompoundId;
  return aParentId.IsValid(theStorage.NbCompounds()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.CompoundRelations(aParentId).ChildRefIds, theRefId);
}

bool isRefOwnedByAnyParent(const BRepGraphInc_Storage&     theStorage,
                           const BRepGraph_OccurrenceRefId theRefId)
{
  if (!theRefId.IsValid(theStorage.NbOccurrenceRefs()) || theStorage.IsRemoved(theRefId))
  {
    return false;
  }
  const BRepGraph_ProductId aParentId = theStorage.OccurrenceRef(theRefId).ParentProductId;
  return aParentId.IsValid(theStorage.NbProducts()) && !theStorage.IsRemoved(aParentId)
         && containsRelationId(theStorage.ProductRelations(aParentId).OccurrenceRefIds, theRefId);
}

bool isExpectedParentKindForRef(const BRepGraph_NodeId      theParent,
                                const BRepGraph_RefId::Kind theRefKind)
{
  switch (theRefKind)
  {
    case BRepGraph_RefId::Kind::Vertex:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Edge;
    case BRepGraph_RefId::Kind::Wire:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Face;
    case BRepGraph_RefId::Kind::Face:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Shell;
    case BRepGraph_RefId::Kind::Shell:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Solid;
    case BRepGraph_RefId::Kind::Solid:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::CompSolid;
    case BRepGraph_RefId::Kind::Child:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Compound;
    case BRepGraph_RefId::Kind::Occurrence:
      return theParent.NodeKind == BRepGraph_NodeId::Kind::Product;
    default:
      return false;
  }
}

bool isRefInRange(const BRepGraphInc_Storage& theStorage, const BRepGraph_RefId theRef)
{
  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Vertex:
      return BRepGraph_VertexRefId(theRef).IsValid(theStorage.NbVertexRefs());
    case BRepGraph_RefId::Kind::Wire:
      return BRepGraph_WireRefId(theRef).IsValid(theStorage.NbWireRefs());
    case BRepGraph_RefId::Kind::Face:
      return BRepGraph_FaceRefId(theRef).IsValid(theStorage.NbFaceRefs());
    case BRepGraph_RefId::Kind::Shell:
      return BRepGraph_ShellRefId(theRef).IsValid(theStorage.NbShellRefs());
    case BRepGraph_RefId::Kind::Solid:
      return BRepGraph_SolidRefId(theRef).IsValid(theStorage.NbSolidRefs());
    case BRepGraph_RefId::Kind::Child:
      return BRepGraph_ChildRefId(theRef).IsValid(theStorage.NbChildRefs());
    case BRepGraph_RefId::Kind::Occurrence:
      return BRepGraph_OccurrenceRefId(theRef).IsValid(theStorage.NbOccurrenceRefs());
    default:
      return false;
  }
}

bool detachRefRelations(BRepGraphInc_Storage& theStorage, const BRepGraph_RefId theRef)
{
  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId    aRefId(theRef);
      const BRepGraphInc::WireRef& aRef = theStorage.WireRef(aRefId);
      return theStorage.DetachWireFromFace(aRef.ParentFaceId, aRefId);
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId    aRefId(theRef);
      const BRepGraphInc::FaceRef& aRef = theStorage.FaceRef(aRefId);
      return theStorage.DetachFaceFromShell(aRef.ParentShellId, aRefId);
    }
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId    aRefId(theRef);
      const BRepGraphInc::ShellRef& aRef = theStorage.ShellRef(aRefId);
      return theStorage.DetachShellFromSolid(aRef.ParentSolidId, aRefId);
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId    aRefId(theRef);
      const BRepGraphInc::SolidRef& aRef = theStorage.SolidRef(aRefId);
      return theStorage.DetachSolidFromCompSolid(aRef.ParentCompSolidId, aRefId);
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId    aRefId(theRef);
      const BRepGraphInc::ChildRef& aRef = theStorage.ChildRef(aRefId);
      return theStorage.DetachChildFromCompound(aRef.ParentCompoundId, aRefId);
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId    aRefId(theRef);
      const BRepGraphInc::OccurrenceRef& aRef = theStorage.OccurrenceRef(aRefId);
      return theStorage.DetachOccurrenceFromProduct(aRef.ParentProductId, aRefId);
    }
    default:
      return true;
  }
}

template <typename RefIdT>
bool detachOrderedParentRef(BRepGraph&                              theGraph,
                            BRepGraphInc_Storage&                   theStorage,
                            const RefIdT                            theRefId,
                            const NCollection_LinearVector<RefIdT>& theParentRefIds,
                            const BRepGraph_NodeId                  theChildNode,
                            const bool                              theToPruneOrphanedChild)
{
  if (!findOrderedRef(theParentRefIds, theRefId).IsValid())
  {
    return false;
  }

  detachRefRelations(theStorage, BRepGraph_RefId(theRefId));
  if (!isRefOwnedByAnyParent(theStorage, theRefId) && !theGraph.Editor().Gen().RemoveRef(theRefId))
  {
    return false;
  }

  if (theToPruneOrphanedChild && theChildNode.IsValid()
      && !hasAnyActiveUsage(theGraph, theChildNode))
  {
    theGraph.Editor().Gen().RemoveSubgraph(theChildNode);
  }
  return true;
}

template <typename RefIdT>
bool detachOrderedParentRef(BRepGraph&                              theGraph,
                            BRepGraphInc_Storage&                   theStorage,
                            const RefIdT                            theRefId,
                            const NCollection_DynamicArray<RefIdT>& theParentRefIds,
                            const BRepGraph_NodeId                  theChildNode,
                            const bool                              theToPruneOrphanedChild)
{
  if (!findOrderedRef(theParentRefIds, theRefId).IsValid())
  {
    return false;
  }

  detachRefRelations(theStorage, BRepGraph_RefId(theRefId));
  if (!isRefOwnedByAnyParent(theStorage, theRefId) && !theGraph.Editor().Gen().RemoveRef(theRefId))
  {
    return false;
  }

  if (theToPruneOrphanedChild && theChildNode.IsValid()
      && !hasAnyActiveUsage(theGraph, theChildNode))
  {
    theGraph.Editor().Gen().RemoveSubgraph(theChildNode);
  }
  return true;
}

const char* kindName(const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return "Vertices";
    case BRepGraph_NodeId::Kind::Edge:
      return "Edges";
    case BRepGraph_NodeId::Kind::CoEdge:
      return "CoEdges";
    case BRepGraph_NodeId::Kind::Wire:
      return "Wires";
    case BRepGraph_NodeId::Kind::Face:
      return "Faces";
    case BRepGraph_NodeId::Kind::Shell:
      return "Shells";
    case BRepGraph_NodeId::Kind::Solid:
      return "Solids";
    case BRepGraph_NodeId::Kind::Compound:
      return "Compounds";
    case BRepGraph_NodeId::Kind::CompSolid:
      return "CompSolids";
    case BRepGraph_NodeId::Kind::Product:
      return "Products";
    case BRepGraph_NodeId::Kind::Occurrence:
      return "Occurrences";
  }
  return "Unknown";
}

//=================================================================================================

static bool isNodeIndexInRange(const BRepGraphInc_Storage& theStorage,
                               const BRepGraph_NodeId      theNode)
{
  if (!theNode.IsValid())
  {
    return false;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_VertexId(theNode).IsValid(theStorage.NbVertices());
    case BRepGraph_NodeId::Kind::Edge:
      return BRepGraph_EdgeId(theNode).IsValid(theStorage.NbEdges());
    case BRepGraph_NodeId::Kind::CoEdge:
      return BRepGraph_CoEdgeId(theNode).IsValid(theStorage.NbCoEdges());
    case BRepGraph_NodeId::Kind::Wire:
      return BRepGraph_WireId(theNode).IsValid(theStorage.NbWires());
    case BRepGraph_NodeId::Kind::Face:
      return BRepGraph_FaceId(theNode).IsValid(theStorage.NbFaces());
    case BRepGraph_NodeId::Kind::Shell:
      return BRepGraph_ShellId(theNode).IsValid(theStorage.NbShells());
    case BRepGraph_NodeId::Kind::Solid:
      return BRepGraph_SolidId(theNode).IsValid(theStorage.NbSolids());
    case BRepGraph_NodeId::Kind::Compound:
      return BRepGraph_CompoundId(theNode).IsValid(theStorage.NbCompounds());
    case BRepGraph_NodeId::Kind::CompSolid:
      return BRepGraph_CompSolidId(theNode).IsValid(theStorage.NbCompSolids());
    case BRepGraph_NodeId::Kind::Product:
      return BRepGraph_ProductId(theNode).IsValid(theStorage.NbProducts());
    case BRepGraph_NodeId::Kind::Occurrence:
      return BRepGraph_OccurrenceId(theNode).IsValid(theStorage.NbOccurrences());
  }

  return false;
}

//=================================================================================================

static bool isActiveNode(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId theNode)
{
  if (!isNodeIndexInRange(theStorage, theNode))
  {
    return false;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return !theStorage.IsRemoved(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return !theStorage.IsRemoved(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return !theStorage.IsRemoved(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return !theStorage.IsRemoved(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return !theStorage.IsRemoved(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return !theStorage.IsRemoved(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return !theStorage.IsRemoved(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return !theStorage.IsRemoved(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return !theStorage.IsRemoved(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return !theStorage.IsRemoved(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return !theStorage.IsRemoved(BRepGraph_OccurrenceId(theNode));
  }
  return false;
}

//=================================================================================================

static bool coEdgeOrientedVertices(const BRepGraphInc_Storage& theStorage,
                                   const BRepGraph_CoEdgeId    theCoEdgeId,
                                   BRepGraph_VertexId&         theStartVertex,
                                   BRepGraph_VertexId&         theEndVertex)
{
  if (!isActiveNode(theStorage, theCoEdgeId))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(theCoEdgeId);
  if (!isActiveNode(theStorage, aCoEdge.ChildEdgeId))
  {
    return false;
  }

  const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(aCoEdge.ChildEdgeId);
  const BRepGraph_VertexRefId  aStartRef =
    (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;
  const BRepGraph_VertexRefId anEndRef =
    (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.EndVertexRefId : anEdge.StartVertexRefId;
  if (!aStartRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(aStartRef)
      || !anEndRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(anEndRef))
  {
    return false;
  }

  theStartVertex = theStorage.VertexRef(aStartRef).ChildVertexId;
  theEndVertex   = theStorage.VertexRef(anEndRef).ChildVertexId;
  return isActiveNode(theStorage, theStartVertex) && isActiveNode(theStorage, theEndVertex);
}

//=================================================================================================

static bool edgeOrientedVertices(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_EdgeId      theEdgeId,
                                 const TopAbs_Orientation    theOrientation,
                                 BRepGraph_VertexId&         theStartVertex,
                                 BRepGraph_VertexId&         theEndVertex)
{
  if (!isActiveNode(theStorage, theEdgeId))
  {
    return false;
  }

  const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(theEdgeId);
  const BRepGraph_VertexRefId  aStartRef =
    (theOrientation == TopAbs_FORWARD) ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;
  const BRepGraph_VertexRefId anEndRef =
    (theOrientation == TopAbs_FORWARD) ? anEdge.EndVertexRefId : anEdge.StartVertexRefId;
  if (!aStartRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(aStartRef)
      || !anEndRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(anEndRef))
  {
    return false;
  }

  theStartVertex = theStorage.VertexRef(aStartRef).ChildVertexId;
  theEndVertex   = theStorage.VertexRef(anEndRef).ChildVertexId;
  return isActiveNode(theStorage, theStartVertex) && isActiveNode(theStorage, theEndVertex);
}

//=================================================================================================

static bool coEdgeOrientedVerticesAfterReplacement(const BRepGraphInc_Storage& theStorage,
                                                   const BRepGraph_CoEdgeId    theCoEdgeId,
                                                   const BRepGraph_EdgeId      theOldEdge,
                                                   const BRepGraph_EdgeId      theNewEdge,
                                                   const bool                  theReversed,
                                                   BRepGraph_VertexId&         theStartVertex,
                                                   BRepGraph_VertexId&         theEndVertex)
{
  if (!isActiveNode(theStorage, theCoEdgeId))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(theCoEdgeId);
  if (aCoEdge.ChildEdgeId != theOldEdge)
  {
    return coEdgeOrientedVertices(theStorage, theCoEdgeId, theStartVertex, theEndVertex);
  }

  TopAbs_Orientation anOrientation = static_cast<TopAbs_Orientation>(aCoEdge.Orientation);
  if (theReversed)
  {
    anOrientation = TopAbs::Reverse(anOrientation);
  }
  return edgeOrientedVertices(theStorage, theNewEdge, anOrientation, theStartVertex, theEndVertex);
}

//=================================================================================================

static BRepGraph::EditorView::WireOps::CoEdgeOrderStatus toEditorCoEdgeOrderStatus(
  const BRepGraphInc_Storage::WireCoEdgeOrderStatus theStatus)
{
  using EditorStatus  = BRepGraph::EditorView::WireOps::CoEdgeOrderStatus;
  using StorageStatus = BRepGraphInc_Storage::WireCoEdgeOrderStatus;
  switch (theStatus)
  {
    case StorageStatus::Connected:
      return EditorStatus::Ready;
    case StorageStatus::Reordered:
      return EditorStatus::Reordered;
    case StorageStatus::ToleranceOrdered:
    case StorageStatus::Partial:
    case StorageStatus::InvalidInput:
      return EditorStatus::Disconnected;
  }
  return EditorStatus::Disconnected;
}

//=================================================================================================

static BRepGraph::EditorView::WireOps::CoEdgeOrderStatus preCheckCoEdgeOrder(
  const BRepGraphInc_Storage&                   theStorage,
  const NCollection_Array1<BRepGraph_CoEdgeId>& theInput,
  const BRepGraph_WireId                        theExpectedWire,
  const bool                                    theRequireFree,
  const bool                                    theRequirePermutation,
  NCollection_LinearVector<BRepGraph_CoEdgeId>& theOrdered)
{
  using CoEdgeOrderStatus = BRepGraph::EditorView::WireOps::CoEdgeOrderStatus;

  theOrdered.Clear(false);
  if (theInput.Size() == 0)
  {
    return CoEdgeOrderStatus::Empty;
  }
  if (!BRepGraphInc_WireOrder::CoEdgeIdsAreUnique(theInput))
  {
    return CoEdgeOrderStatus::DuplicateCoEdge;
  }

  const NCollection_LinearVector<BRepGraph_CoEdgeId>* anOldCoEdges = nullptr;
  if (theExpectedWire.IsValid())
  {
    if (!theExpectedWire.IsValid(theStorage.NbWires()) || theStorage.IsRemoved(theExpectedWire))
    {
      return CoEdgeOrderStatus::InvalidWire;
    }
    anOldCoEdges = &theStorage.WireRelations(theExpectedWire).CoEdgeIds;
    if (theRequirePermutation && theInput.Size() != anOldCoEdges->Size())
    {
      return CoEdgeOrderStatus::SizeMismatch;
    }
  }

  for (const BRepGraph_CoEdgeId& aCoEdgeId : theInput)
  {
    if (!isActiveNode(theStorage, aCoEdgeId))
    {
      return CoEdgeOrderStatus::InvalidCoEdge;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
    if (theRequireFree && aCoEdge.ParentWireId.IsValid())
    {
      return CoEdgeOrderStatus::CoEdgeAlreadyBound;
    }
    if (theExpectedWire.IsValid() && aCoEdge.ParentWireId != theExpectedWire)
    {
      return CoEdgeOrderStatus::CoEdgeNotOwnedByWire;
    }
    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVertices(theStorage, aCoEdgeId, aStart, anEnd))
    {
      return CoEdgeOrderStatus::InvalidCoEdge;
    }
  }

  if (theRequirePermutation && anOldCoEdges != nullptr)
  {
    NCollection_LinearVector<unsigned char> aMatched;
    for (size_t anIdx = 0; anIdx < anOldCoEdges->Size(); ++anIdx)
    {
      aMatched.Append(0);
    }

    for (const BRepGraph_CoEdgeId& aNewCoEdgeId : theInput)
    {
      bool isFound = false;
      for (size_t anOldIdx = 0; anOldIdx < anOldCoEdges->Size(); ++anOldIdx)
      {
        if (aMatched.Value(anOldIdx) == 0 && anOldCoEdges->Value(anOldIdx) == aNewCoEdgeId)
        {
          aMatched.ChangeValue(anOldIdx) = 1;
          isFound                        = true;
          break;
        }
      }
      if (!isFound)
      {
        return CoEdgeOrderStatus::NotPermutation;
      }
    }

    bool isSameOrder = true;
    for (size_t anIdx = 0; anIdx < anOldCoEdges->Size(); ++anIdx)
    {
      if (anOldCoEdges->Value(anIdx) != theInput.Value(static_cast<int>(anIdx)))
      {
        isSameOrder = false;
        break;
      }
    }
    if (isSameOrder && theStorage.ValidateWireCoEdgeOrder(theExpectedWire))
    {
      for (const BRepGraph_CoEdgeId& aCoEdgeId : theInput)
      {
        theOrdered.Append(aCoEdgeId);
      }
      return CoEdgeOrderStatus::AlreadyCurrent;
    }
  }

  return toEditorCoEdgeOrderStatus(
    BRepGraphInc_WireOrder::BuildCoEdgeOrder(theStorage, BRepGraph_WireId(), theInput, theOrdered));
}

//=================================================================================================

static bool rotateConnectedCoEdgesAfterRemoval(
  const BRepGraphInc_Storage&                         theStorage,
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& theCurrentCoEdges,
  const BRepGraph_CoEdgeId                            theRemovedCoEdge,
  NCollection_LinearVector<BRepGraph_CoEdgeId>&       theOrderedRemaining)
{
  NCollection_LinearVector<BRepGraph_CoEdgeId> aRemaining;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : theCurrentCoEdges)
  {
    if (aCoEdgeId != theRemovedCoEdge)
    {
      aRemaining.Append(aCoEdgeId);
    }
  }

  if (aRemaining.Size() < 2)
  {
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aRemaining)
    {
      theOrderedRemaining.Append(aCoEdgeId);
    }
    return true;
  }

  for (size_t aStartIdx = 0; aStartIdx < aRemaining.Size(); ++aStartIdx)
  {
    bool               isConnected = true;
    BRepGraph_VertexId aPrevEnd;
    for (size_t anOffset = 0; anOffset < aRemaining.Size(); ++anOffset)
    {
      const BRepGraph_CoEdgeId aCoEdgeId =
        aRemaining.Value((aStartIdx + anOffset) % aRemaining.Size());
      BRepGraph_VertexId aStart;
      BRepGraph_VertexId anEnd;
      if (!coEdgeOrientedVertices(theStorage, aCoEdgeId, aStart, anEnd)
          || (anOffset > 0 && aStart != aPrevEnd))
      {
        isConnected = false;
        break;
      }
      aPrevEnd = anEnd;
    }
    if (!isConnected)
    {
      continue;
    }

    for (size_t anOffset = 0; anOffset < aRemaining.Size(); ++anOffset)
    {
      theOrderedRemaining.Append(aRemaining.Value((aStartIdx + anOffset) % aRemaining.Size()));
    }
    return true;
  }
  return false;
}

//=================================================================================================

static bool isActiveTopologyNode(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_NodeId      theNode)
{
  return theNode.IsValid() && BRepGraph_NodeId::IsTopologyKind(theNode.NodeKind)
         && isActiveNode(theStorage, theNode);
}

//=================================================================================================

static void rebindCoEdgesForEdgeReplacement(BRepGraphInc_Storage&  theStorage,
                                            const BRepGraph_EdgeId theSourceEdgeId,
                                            const BRepGraph_EdgeId theReplacementEdgeId)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.EdgeRelations(theSourceEdgeId).CoEdgeIds;
  const uint32_t aNbCoEdges = static_cast<uint32_t>(aCoEdgeIdxs.Size());
  NCollection_LocalArray<BRepGraph_CoEdgeId, 16> aCoEdgeSnapshot(aNbCoEdges);
  for (uint32_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    aCoEdgeSnapshot[aCoEdgeIdx] = aCoEdgeIdxs.Value(static_cast<size_t>(aCoEdgeIdx));
  }

  for (uint32_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeSnapshot[aCoEdgeIdx];
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()))
    {
      continue;
    }

    BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);
    if (theStorage.IsRemoved(aCoEdgeId) || aCoEdge.ChildEdgeId != theSourceEdgeId)
    {
      continue;
    }

    aCoEdge.ChildEdgeId = theReplacementEdgeId;
    theStorage.RebindCoEdgeEdge(aCoEdgeId, theSourceEdgeId, theReplacementEdgeId);
  }
}

//=================================================================================================

static void unbindCoEdgesOfRemovedEdge(BRepGraphInc_Storage&  theStorage,
                                       const BRepGraph_EdgeId theEdgeId)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.EdgeRelations(theEdgeId).CoEdgeIds;
  const uint32_t aNbCoEdges = static_cast<uint32_t>(aCoEdgeIdxs.Size());
  NCollection_LocalArray<BRepGraph_CoEdgeId, 16> aCoEdgeSnapshot(aNbCoEdges);
  for (uint32_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    aCoEdgeSnapshot[aCoEdgeIdx] = aCoEdgeIdxs.Value(static_cast<size_t>(aCoEdgeIdx));
  }

  for (uint32_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeSnapshot[aCoEdgeIdx];
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()))
    {
      continue;
    }

    BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);
    if (!theStorage.IsRemoved(aCoEdgeId) && aCoEdge.ChildEdgeId == theEdgeId)
    {
      aCoEdge.ChildEdgeId = BRepGraph_EdgeId();
      theStorage.RebindCoEdgeEdge(aCoEdgeId, theEdgeId, BRepGraph_EdgeId());
    }
  }
}

//=================================================================================================

template <typename DefType>
int countIterator(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (BRepGraph_Iterator<DefType> anIt(theGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  return aCount;
}

int countActiveByKind(const BRepGraph& theGraph, const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return countIterator<BRepGraphInc::VertexDef>(theGraph);
    case BRepGraph_NodeId::Kind::Edge:
      return countIterator<BRepGraphInc::EdgeDef>(theGraph);
    case BRepGraph_NodeId::Kind::CoEdge:
      return countIterator<BRepGraphInc::CoEdgeDef>(theGraph);
    case BRepGraph_NodeId::Kind::Wire:
      return countIterator<BRepGraphInc::WireDef>(theGraph);
    case BRepGraph_NodeId::Kind::Face:
      return countIterator<BRepGraphInc::FaceDef>(theGraph);
    case BRepGraph_NodeId::Kind::Shell:
      return countIterator<BRepGraphInc::ShellDef>(theGraph);
    case BRepGraph_NodeId::Kind::Solid:
      return countIterator<BRepGraphInc::SolidDef>(theGraph);
    case BRepGraph_NodeId::Kind::Compound:
      return countIterator<BRepGraphInc::CompoundDef>(theGraph);
    case BRepGraph_NodeId::Kind::CompSolid:
      return countIterator<BRepGraphInc::CompSolidDef>(theGraph);
    case BRepGraph_NodeId::Kind::Product:
      return countIterator<BRepGraphInc::ProductDef>(theGraph);
    case BRepGraph_NodeId::Kind::Occurrence:
      return countIterator<BRepGraphInc::OccurrenceDef>(theGraph);
  }
  return 0;
}

int cachedActiveByKind(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theStorage.NbActiveVertices();
    case BRepGraph_NodeId::Kind::Edge:
      return theStorage.NbActiveEdges();
    case BRepGraph_NodeId::Kind::CoEdge:
      return theStorage.NbActiveCoEdges();
    case BRepGraph_NodeId::Kind::Wire:
      return theStorage.NbActiveWires();
    case BRepGraph_NodeId::Kind::Face:
      return theStorage.NbActiveFaces();
    case BRepGraph_NodeId::Kind::Shell:
      return theStorage.NbActiveShells();
    case BRepGraph_NodeId::Kind::Solid:
      return theStorage.NbActiveSolids();
    case BRepGraph_NodeId::Kind::Compound:
      return theStorage.NbActiveCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:
      return theStorage.NbActiveCompSolids();
    case BRepGraph_NodeId::Kind::Product:
      return theStorage.NbActiveProducts();
    case BRepGraph_NodeId::Kind::Occurrence:
      return theStorage.NbActiveOccurrences();
  }
  return 0;
}

//! Initialize a sub-edge definition produced by Split.
//! Copies shared properties from the original edge and assigns boundary vertex ref ids.
//! Vertex ref entries must already exist in storage; only their RefId indices are passed.
void initSubEdgeEntity(BRepGraphInc::EdgeDef&           theSub,
                       const BRepGraph_EdgeCurve3DRepId theCurve3DRepId,
                       const double                     theTolerance,
                       const BRepGraph_VertexRefId      theStartVertexRefId,
                       const BRepGraph_VertexRefId      theEndVertexRefId)
{
  theSub.Curve3DRepId     = theCurve3DRepId;
  theSub.Tolerance        = theTolerance;
  theSub.StartVertexRefId = theStartVertexRefId;
  theSub.EndVertexRefId   = theEndVertexRefId;
}

//! Initialize a sub-CoEdge definition produced by Split.
void initSubCoEdgeEntity(BRepGraphInc::CoEdgeDef&           theCE,
                         const BRepGraph_EdgeId             theEdgeId,
                         const BRepGraph_FaceId             theFaceId,
                         const TopAbs_Orientation           theOrientation,
                         const BRepGraph_CoEdgeCurve2DRepId theCurve2DRepId)
{
  theCE.ChildEdgeId  = theEdgeId;
  theCE.FaceId       = theFaceId;
  theCE.Orientation  = theOrientation;
  theCE.Curve2DRepId = theCurve2DRepId;
}

} // namespace

//=================================================================================================

BRepGraph_VertexId BRepGraph::EditorView::VertexOps::Add(const gp_Pnt& thePoint,
                                                         const double  theTolerance)
{
  BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_VertexId aVertexId(aStorage.NbVertices());
  aStorage.AppendVertex();
  BRepGraphInc::VertexDef& aVtxDef = aStorage.ChangeVertex(aVertexId);
  aVtxDef.Point                    = thePoint;
  aVtxDef.Tolerance                = theTolerance;
  myGraph->allocateUID(aVertexId);
  myGraph->markModified(aVertexId);

  return aVertexId;
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph::EditorView::EdgeOps::Add(const BRepGraph_VertexId       theStartVtx,
                                                     const BRepGraph_VertexId       theEndVtx,
                                                     const occ::handle<Geom_Curve>& theCurve,
                                                     const double                   theFirst,
                                                     const double                   theLast,
                                                     const double                   theTolerance)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theStartVtx.IsValid() && !isActiveNode(aStorage, theStartVtx))
  {
    return BRepGraph_EdgeId();
  }
  if (theEndVtx.IsValid() && !isActiveNode(aStorage, theEndVtx))
  {
    return BRepGraph_EdgeId();
  }

  const BRepGraph_EdgeId aEdgeId(aStorage.NbEdges());
  aStorage.AppendEdge();
  BRepGraphInc::EdgeDef& anEdgeDef = aStorage.ChangeEdge(aEdgeId);
  if (theStartVtx.IsValid())
  {
    const BRepGraph_VertexRefId aStartVtxRefId = aStorage.AppendVertexRef();
    BRepGraphInc::VertexRef&    aStartVtxRef   = aStorage.ChangeVertexRef(aStartVtxRefId);
    aStartVtxRef.ChildVertexId                 = theStartVtx;
    aStartVtxRef.ParentEdgeId                  = aEdgeId;
    aStartVtxRef.Orientation                   = TopAbs_FORWARD;
    myGraph->allocateRefUID(aStartVtxRefId);
    anEdgeDef.StartVertexRefId = aStartVtxRefId;
    aStorage.AttachEdgeToVertex(aEdgeId, theStartVtx);
  }
  if (theEndVtx.IsValid())
  {
    const BRepGraph_VertexRefId anEndVtxRefId = aStorage.AppendVertexRef();
    BRepGraphInc::VertexRef&    anEndVtxRef   = aStorage.ChangeVertexRef(anEndVtxRefId);
    anEndVtxRef.ChildVertexId                 = theEndVtx;
    anEndVtxRef.ParentEdgeId                  = aEdgeId;
    anEndVtxRef.Orientation                   = TopAbs_REVERSED;
    myGraph->allocateRefUID(anEndVtxRefId);
    anEdgeDef.EndVertexRefId = anEndVtxRefId;
    aStorage.AttachEdgeToVertex(aEdgeId, theEndVtx);
  }
  anEdgeDef.Tolerance = theTolerance;
  myGraph->allocateUID(aEdgeId);

  if (!theCurve.IsNull())
  {
    // Create owned use record
    const BRepGraph_EdgeCurve3DRepId aRepId = aStorage.AppendEdgeCurve3DRep();
    BRepGraphInc::EdgeCurve3DRep&    aUse   = aStorage.ChangeEdgeCurve3DRep(aRepId);
    aUse.ParentEdgeId                       = aEdgeId;
    aUse.Curve                              = theCurve;
    aUse.ParamFirst                         = theFirst;
    aUse.ParamLast                          = theLast;
    anEdgeDef.Curve3DRepId                  = aRepId;
  }

  myGraph->markModified(aEdgeId);
  return aEdgeId;
}

//=================================================================================================

BRepGraph::EditorView::WireOps::CoEdgeOrderStatus BRepGraph::EditorView::WireOps::CheckCoEdgeOrder(
  const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds) const
{
  NCollection_LinearVector<BRepGraph_CoEdgeId> anOrdered;
  return preCheckCoEdgeOrder(myGraph->myData->myIncStorage,
                             theCoEdgeIds,
                             BRepGraph_WireId(),
                             true,
                             false,
                             anOrdered);
}

//=================================================================================================

BRepGraph::EditorView::WireOps::CoEdgeOrderStatus BRepGraph::EditorView::WireOps::CheckCoEdgeOrder(
  const BRepGraph_WireId                        theWire,
  const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theWire.IsValid(aStorage.NbWires()) || aStorage.IsRemoved(theWire))
  {
    return CoEdgeOrderStatus::InvalidWire;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> anOrdered;
  return preCheckCoEdgeOrder(aStorage, theCoEdgeIds, theWire, false, true, anOrdered);
}

//=================================================================================================

BRepGraph::EditorView::WireOps::CoEdgeOrderStatus BRepGraph::EditorView::WireOps::CheckAppendCoEdge(
  const BRepGraph_WireId   theWire,
  const BRepGraph_CoEdgeId theCoEdgeId) const
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theWire.IsValid(aStorage.NbWires()) || aStorage.IsRemoved(theWire))
  {
    return CoEdgeOrderStatus::InvalidWire;
  }
  if (!isActiveNode(aStorage, theCoEdgeId))
  {
    return CoEdgeOrderStatus::InvalidCoEdge;
  }

  const BRepGraphInc::CoEdgeDef& aCandidate = aStorage.CoEdge(theCoEdgeId);
  if (aCandidate.ParentWireId == theWire)
  {
    return CoEdgeOrderStatus::AlreadyContained;
  }
  if (aCandidate.ParentWireId.IsValid())
  {
    return CoEdgeOrderStatus::CoEdgeAlreadyBound;
  }

  BRepGraph_VertexId aCandidateStart;
  BRepGraph_VertexId aCandidateEnd;
  if (!coEdgeOrientedVertices(aStorage, theCoEdgeId, aCandidateStart, aCandidateEnd))
  {
    return CoEdgeOrderStatus::InvalidCoEdge;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId>        aCandidateOrder;
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCurrent =
    aStorage.WireRelations(theWire).CoEdgeIds;
  for (const BRepGraph_CoEdgeId& aCurrentCoEdgeId : aCurrent)
  {
    if (!isActiveNode(aStorage, aCurrentCoEdgeId)
        || aStorage.CoEdge(aCurrentCoEdgeId).ParentWireId != theWire)
    {
      return CoEdgeOrderStatus::InvalidCoEdge;
    }

    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVertices(aStorage, aCurrentCoEdgeId, aStart, anEnd))
    {
      return CoEdgeOrderStatus::InvalidCoEdge;
    }
    aCandidateOrder.Append(aCurrentCoEdgeId);
  }
  aCandidateOrder.Append(theCoEdgeId);

  bool               isConnected = true;
  bool               hasPrev     = false;
  BRepGraph_VertexId aPrevEnd;
  for (const BRepGraph_CoEdgeId& aCandidateCoEdgeId : aCandidateOrder)
  {
    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVertices(aStorage, aCandidateCoEdgeId, aStart, anEnd)
        || (hasPrev && aStart != aPrevEnd))
    {
      isConnected = false;
      break;
    }
    aPrevEnd = anEnd;
    hasPrev  = true;
  }
  if (isConnected)
  {
    return CoEdgeOrderStatus::Ready;
  }

  const NCollection_Array1<BRepGraph_CoEdgeId> anInput = aCandidateOrder.ToArray1();
  NCollection_LinearVector<BRepGraph_CoEdgeId> anOrdered;
  return toEditorCoEdgeOrderStatus(
    BRepGraphInc_WireOrder::BuildCoEdgeOrder(aStorage, BRepGraph_WireId(), anInput, anOrdered));
}

//=================================================================================================

BRepGraph::EditorView::WireOps::ReplaceEdgeStatus BRepGraph::EditorView::WireOps::CheckReplaceEdge(
  const BRepGraph_WireId theWire,
  const BRepGraph_EdgeId theOldEdge,
  const BRepGraph_EdgeId theNewEdge,
  const bool             theReversed) const
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theWire.IsValid(aStorage.NbWires()) || aStorage.IsRemoved(theWire))
  {
    return ReplaceEdgeStatus::InvalidWire;
  }
  if (!isActiveNode(aStorage, theOldEdge))
  {
    return ReplaceEdgeStatus::InvalidOldEdge;
  }
  if (!isActiveNode(aStorage, theNewEdge))
  {
    return ReplaceEdgeStatus::InvalidNewEdge;
  }
  if (theOldEdge == theNewEdge && !theReversed)
  {
    return ReplaceEdgeStatus::AlreadyCurrent;
  }

  bool                                                hasOldEdge = false;
  bool                                                hasPrev    = false;
  BRepGraph_VertexId                                  aPrevEnd;
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    aStorage.WireRelations(theWire).CoEdgeIds;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
  {
    if (!isActiveNode(aStorage, aCoEdgeId) || aStorage.CoEdge(aCoEdgeId).ParentWireId != theWire)
    {
      return ReplaceEdgeStatus::InvalidOldEdge;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    hasOldEdge                             = hasOldEdge || aCoEdge.ChildEdgeId == theOldEdge;

    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVerticesAfterReplacement(aStorage,
                                                aCoEdgeId,
                                                theOldEdge,
                                                theNewEdge,
                                                theReversed,
                                                aStart,
                                                anEnd)
        || (hasPrev && aStart != aPrevEnd))
    {
      return ReplaceEdgeStatus::Disconnected;
    }

    aPrevEnd = anEnd;
    hasPrev  = true;
  }
  return hasOldEdge ? ReplaceEdgeStatus::Ready : ReplaceEdgeStatus::InvalidOldEdge;
}

//=================================================================================================

BRepGraph_WireId BRepGraph::EditorView::WireOps::Add(
  const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theCoEdgeIds.IsEmpty())
  {
    const BRepGraph_WireId aWireId(aStorage.NbWires());
    aStorage.AppendWire();
    myGraph->allocateUID(aWireId);
    myGraph->markModified(aWireId);
    return aWireId;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> aConnectedCoEdges;
  const CoEdgeOrderStatus                      aStatus =
    preCheckCoEdgeOrder(aStorage, theCoEdgeIds, BRepGraph_WireId(), true, false, aConnectedCoEdges);
  if (aStatus != CoEdgeOrderStatus::Ready && aStatus != CoEdgeOrderStatus::Reordered)
  {
    return BRepGraph_WireId();
  }

  const BRepGraph_WireId aWireId(aStorage.NbWires());
  aStorage.AppendWire();
  myGraph->allocateUID(aWireId);

  for (const BRepGraph_CoEdgeId& aCoEdgeId : aConnectedCoEdges)
  {
    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
    aCoEdge.ParentWireId             = aWireId;

    aStorage.ChangeWireRelationsInternal(aWireId).CoEdgeIds.Append(aCoEdgeId);
    BRepGraphInc::EdgeRelations& anEdgeRel =
      aStorage.ChangeEdgeRelationsInternal(aCoEdge.ChildEdgeId);
    appendUniqueRelationId(anEdgeRel.CoEdgeIds, aCoEdgeId);
    myGraph->markModified(aCoEdgeId);
    myGraph->markModified(aCoEdge.ChildEdgeId);
  }
  myGraph->markModified(aWireId);

  return aWireId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph::EditorView::FaceOps::Add(
  const occ::handle<Geom_Surface>&            theSurface,
  const BRepGraph_WireId                      theOuterWire,
  const NCollection_Array1<BRepGraph_WireId>& theInnerWires,
  const double                                theTolerance)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theOuterWire.IsValid() && !isActiveNode(aStorage, theOuterWire))
  {
    return BRepGraph_FaceId();
  }
  for (const BRepGraph_WireId& aChildWireId : theInnerWires)
  {
    if (aChildWireId.IsValid() && !isActiveNode(aStorage, aChildWireId))
    {
      return BRepGraph_FaceId();
    }
  }

  const BRepGraph_FaceId aFaceId  = aStorage.AppendFace();
  BRepGraphInc::FaceDef& aFaceDef = aStorage.ChangeFace(aFaceId);
  aFaceDef.Tolerance              = theTolerance;
  myGraph->allocateUID(aFaceId);
  if (!theSurface.IsNull())
  {
    // Create owned use record
    const BRepGraph_FaceSurfaceRepId aRepId = aStorage.AppendFaceSurfaceRep();
    BRepGraphInc::FaceSurfaceRep&    aUse   = aStorage.ChangeFaceSurfaceRep(aRepId);
    aUse.ParentFaceId                       = aFaceId;
    aUse.Surface                            = theSurface;
    aFaceDef.SurfaceRepId                   = aRepId;
  }

  // Link wire refs.
  if (theOuterWire.IsValid())
  {
    const BRepGraph_WireRefId anOuterWireRefId = aStorage.AttachWireToFace(aFaceId, theOuterWire);
    myGraph->allocateRefUID(anOuterWireRefId);
  }

  for (const BRepGraph_WireId& aChildWireId : theInnerWires)
  {
    if (!aChildWireId.IsValid())
    {
      continue;
    }
    const BRepGraph_WireRefId aWireRefId = aStorage.AttachWireToFace(aFaceId, aChildWireId);
    myGraph->allocateRefUID(aWireRefId);
  }

  myGraph->markModified(aFaceId);
  return aFaceId;
}

//=================================================================================================

BRepGraph_WireRefId BRepGraph::EditorView::FaceOps::Append(
  const BRepGraph_FaceId                theFaceEntity,
  const BRepGraph_WireId                theWireEntity,
  const BRepGraphInc::ParityOrientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theFaceEntity) || !isActiveNode(aStorage, theWireEntity))
  {
    return BRepGraph_WireRefId();
  }
  myGraph->Editor().requireUnlocked(theFaceEntity, "BRepGraph::EditorView::Append(): locked face");
  myGraph->Editor().requireNoActiveGuard(theFaceEntity,
                                         "BRepGraph::EditorView::Append(): guard active on face");

  const BRepGraph_WireRefId aWireRefId =
    aStorage.AttachWireToFace(theFaceEntity, theWireEntity, theOri);
  myGraph->allocateRefUID(aWireRefId);

  myGraph->markRefModified(aWireRefId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "AddWire: post-mutation relation inconsistency");
  }

  return aWireRefId;
}

//=================================================================================================

BRepGraph_ShellId BRepGraph::EditorView::ShellOps::Add()
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_ShellId aShellId(aStorage.NbShells());
  aStorage.AppendShell();
  myGraph->allocateUID(aShellId);
  myGraph->markModified(aShellId);

  return aShellId;
}

//=================================================================================================

BRepGraph_SolidId BRepGraph::EditorView::SolidOps::Add()
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_SolidId aSolidId(aStorage.NbSolids());
  aStorage.AppendSolid();
  myGraph->allocateUID(aSolidId);
  myGraph->markModified(aSolidId);

  return aSolidId;
}

//=================================================================================================

BRepGraph_FaceRefId BRepGraph::EditorView::ShellOps::Append(
  const BRepGraph_ShellId               theShellEntity,
  const BRepGraph_FaceId                theFaceEntity,
  const BRepGraphInc::ParityOrientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theShellEntity) || !isActiveNode(aStorage, theFaceEntity))
  {
    return BRepGraph_FaceRefId();
  }
  myGraph->Editor().requireUnlocked(theShellEntity,
                                    "BRepGraph::EditorView::Append(): locked shell");
  myGraph->Editor().requireNoActiveGuard(theShellEntity,
                                         "BRepGraph::EditorView::Append(): guard active on shell");

  const BRepGraph_FaceRefId aFaceRefId =
    aStorage.AttachFaceToShell(theShellEntity, theFaceEntity, theOri);
  myGraph->allocateRefUID(aFaceRefId);

  myGraph->markRefModified(aFaceRefId);
  return aFaceRefId;
}

//=================================================================================================

BRepGraph_ShellRefId BRepGraph::EditorView::SolidOps::Append(
  const BRepGraph_SolidId               theSolidEntity,
  const BRepGraph_ShellId               theShellEntity,
  const BRepGraphInc::ParityOrientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theSolidEntity) || !isActiveNode(aStorage, theShellEntity))
  {
    return BRepGraph_ShellRefId();
  }
  myGraph->Editor().requireUnlocked(theSolidEntity,
                                    "BRepGraph::EditorView::Append(): locked solid");
  myGraph->Editor().requireNoActiveGuard(theSolidEntity,
                                         "BRepGraph::EditorView::Append(): guard active on solid");

  const BRepGraph_ShellRefId aShellRefId =
    aStorage.AttachShellToSolid(theSolidEntity, theShellEntity, theOri);
  myGraph->allocateRefUID(aShellRefId);

  myGraph->markRefModified(aShellRefId);
  return aShellRefId;
}

//=================================================================================================

BRepGraph_CompoundId BRepGraph::EditorView::CompoundOps::Add(
  const NCollection_Array1<BRepGraph_NodeId>& theChildEntities)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  for (const BRepGraph_NodeId& aChild : theChildEntities)
  {
    if (!isActiveTopologyNode(aStorage, aChild))
    {
      return BRepGraph_CompoundId();
    }
  }

  const BRepGraph_CompoundId aCompoundId(aStorage.NbCompounds());
  aStorage.AppendCompound();
  myGraph->allocateUID(aCompoundId);

  for (const BRepGraph_NodeId& aChild : theChildEntities)
  {
    const BRepGraph_ChildRefId aChildRefId = aStorage.AttachChildToCompound(aCompoundId, aChild);
    myGraph->allocateRefUID(aChildRefId);
  }

  myGraph->markModified(aCompoundId);
  return aCompoundId;
}

//=================================================================================================

BRepGraph_ChildRefId BRepGraph::EditorView::CompoundOps::Append(
  const BRepGraph_CompoundId            theCompoundEntity,
  const BRepGraph_NodeId                theChildEntity,
  const BRepGraphInc::ParityOrientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompoundEntity) || !isActiveTopologyNode(aStorage, theChildEntity))
  {
    return BRepGraph_ChildRefId();
  }
  myGraph->Editor().requireUnlocked(theCompoundEntity,
                                    "BRepGraph::EditorView::Append(): locked compound");
  myGraph->Editor().requireNoActiveGuard(
    theCompoundEntity,
    "BRepGraph::EditorView::Append(): guard active on compound");

  const BRepGraph_ChildRefId aChildRefId =
    aStorage.AttachChildToCompound(theCompoundEntity, theChildEntity, TopLoc_Location(), theOri);
  myGraph->allocateRefUID(aChildRefId);

  myGraph->markRefModified(aChildRefId);
  return aChildRefId;
}

//=================================================================================================

BRepGraph_CompSolidId BRepGraph::EditorView::CompSolidOps::Add(
  const NCollection_Array1<BRepGraph_SolidId>& theSolidEntities)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  for (const BRepGraph_SolidId& aSolidId : theSolidEntities)
  {
    if (!isActiveNode(aStorage, aSolidId))
    {
      return BRepGraph_CompSolidId();
    }
  }

  const BRepGraph_CompSolidId aCompSolidId(aStorage.NbCompSolids());
  aStorage.AppendCompSolid();
  myGraph->allocateUID(aCompSolidId);

  for (const BRepGraph_SolidId& aSolidId : theSolidEntities)
  {
    const BRepGraph_SolidRefId aSolidRefId =
      aStorage.AttachSolidToCompSolid(aCompSolidId, aSolidId);
    myGraph->allocateRefUID(aSolidRefId);
  }

  myGraph->markModified(aCompSolidId);
  return aCompSolidId;
}

//=================================================================================================

BRepGraph_SolidRefId BRepGraph::EditorView::CompSolidOps::Append(
  const BRepGraph_CompSolidId           theCompSolidEntity,
  const BRepGraph_SolidId               theSolidEntity,
  const BRepGraphInc::ParityOrientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompSolidEntity) || !isActiveNode(aStorage, theSolidEntity))
  {
    return BRepGraph_SolidRefId();
  }
  myGraph->Editor().requireUnlocked(theCompSolidEntity,
                                    "BRepGraph::EditorView::Append(): locked compsolid");
  myGraph->Editor().requireNoActiveGuard(
    theCompSolidEntity,
    "BRepGraph::EditorView::Append(): guard active on compsolid");

  const BRepGraph_SolidRefId aSolidRefId =
    aStorage.AttachSolidToCompSolid(theCompSolidEntity, theSolidEntity, theOri);
  myGraph->allocateRefUID(aSolidRefId);

  myGraph->markRefModified(aSolidRefId);
  return aSolidRefId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::EditorView::ProductOps::Add(const BRepGraph_NodeId theShapeRoot,
                                                           const TopLoc_Location& thePlacement)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveTopologyNode(aStorage, theShapeRoot))
  {
    return BRepGraph_ProductId();
  }

  const BRepGraph_ProductId aProductId(aStorage.NbProducts());
  aStorage.AppendProduct();
  myGraph->allocateUID(aProductId);

  // Link the product to its shape root via an occurrence + occurrence ref.
  const BRepGraph_OccurrenceId anOccId(aStorage.NbOccurrences());
  aStorage.AppendOccurrence();
  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
  anOccDef.ChildNodeId                  = theShapeRoot;
  Standard_ASSERT_RETURN(isValidOccurrenceChildKind(anOccDef.ChildNodeId.NodeKind),
                         "ProductOps::Add: invalid occurrence child kind",
                         BRepGraph_ProductId());
  myGraph->allocateUID(anOccId);

  const BRepGraph_OccurrenceRefId anOccRefId =
    aStorage.AttachOccurrenceToProduct(aProductId, anOccId, thePlacement);
  myGraph->allocateRefUID(anOccRefId);

  myGraph->markModified(anOccId);
  myGraph->markRefModified(anOccRefId);
  return aProductId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::EditorView::ProductOps::Add()
{
  BRepGraphInc_Storage&     aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_ProductId aProductId(aStorage.NbProducts());
  aStorage.AppendProduct();
  myGraph->allocateUID(aProductId);
  myGraph->markModified(aProductId);

  return aProductId;
}

//=================================================================================================

void BRepGraph::EditorView::ProductOps::AppendDocumentRoot(const BRepGraph_ProductId theProductId)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProductId.IsValid(aStorage.NbProducts()) || aStorage.IsRemoved(theProductId))
  {
    return;
  }
  myGraph->Editor().requireUnlocked(theProductId,
                                    "BRepGraph::EditorView::AppendDocumentRoot(): locked product");
  myGraph->Editor().requireNoActiveGuard(
    theProductId,
    "BRepGraph::EditorView::AppendDocumentRoot(): guard active on product");

  NCollection_LinearVector<BRepGraph_ProductId>& aRoots = aStorage.ChangeRootProductIds();
  for (size_t anIdx = 0; anIdx < aRoots.Size(); ++anIdx)
  {
    if (aRoots.Value(anIdx) == theProductId)
    {
      return;
    }
  }

  aRoots.Append(theProductId);
  myGraph->markModified(theProductId);
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::EditorView::ProductOps::Append(
  const BRepGraph_ProductId    theParentProduct,
  const BRepGraph_ProductId    theReferencedProduct,
  const TopLoc_Location&       thePlacement,
  const BRepGraph_OccurrenceId theParentOccurrence,
  BRepGraph_OccurrenceRefId*   theOutOccurrenceRefId)
{
  if (theOutOccurrenceRefId != nullptr)
  {
    *theOutOccurrenceRefId = BRepGraph_OccurrenceRefId();
  }
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (!isActiveNode(aStorage, theParentProduct))
  {
    return BRepGraph_OccurrenceId();
  }
  if (!isActiveNode(aStorage, theReferencedProduct))
  {
    return BRepGraph_OccurrenceId();
  }
  if (theParentProduct == theReferencedProduct)
  {
    return BRepGraph_OccurrenceId();
  }
  if (theParentOccurrence.IsValid() && !isActiveNode(aStorage, theParentOccurrence))
  {
    return BRepGraph_OccurrenceId();
  }
  if (theParentOccurrence.IsValid()
      && aStorage.Occurrence(theParentOccurrence).ChildNodeId != BRepGraph_NodeId(theParentProduct))
  {
    return BRepGraph_OccurrenceId();
  }
  myGraph->Editor().requireUnlocked(theParentProduct,
                                    "BRepGraph::EditorView::Append(): locked parent product");
  myGraph->Editor().requireNoActiveGuard(
    theParentProduct,
    "BRepGraph::EditorView::Append(): guard active on parent product");
  if (theParentOccurrence.IsValid())
  {
    myGraph->Editor().requireUnlocked(theParentOccurrence,
                                      "BRepGraph::EditorView::Append(): locked parent occurrence");
    myGraph->Editor().requireNoActiveGuard(
      theParentOccurrence,
      "BRepGraph::EditorView::Append(): guard active on parent occurrence");
  }

  const BRepGraph_OccurrenceId anOccId(aStorage.NbOccurrences());
  aStorage.AppendOccurrence();
  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
  anOccDef.ChildNodeId                  = theReferencedProduct;
  Standard_ASSERT_RETURN(isValidOccurrenceChildKind(anOccDef.ChildNodeId.NodeKind),
                         "ProductOps::Append: invalid occurrence child kind",
                         BRepGraph_OccurrenceId());
  myGraph->allocateUID(anOccId);

  const BRepGraph_OccurrenceRefId anOccRefId =
    aStorage.AttachOccurrenceToProduct(theParentProduct, anOccId, thePlacement);
  myGraph->allocateRefUID(anOccRefId);

  removeFromRootProducts(myGraph->myData->myIncStorage.ChangeRootProductIds(),
                         theReferencedProduct);

  myGraph->markModified(anOccId);
  myGraph->markRefModified(anOccRefId);
  myGraph->markModified(theReferencedProduct);

  if (theOutOccurrenceRefId != nullptr)
  {
    *theOutOccurrenceRefId = anOccRefId;
  }
  return anOccId;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::RemoveNode(const BRepGraph_NodeId theNode)
{
  if (!theNode.IsValid())
  {
    return;
  }
  myGraph->Editor().requireUnlocked(theNode, "BRepGraph::EditorView::RemoveNode(): locked node");
  myGraph->Editor().requireNoActiveGuard(
    theNode,
    "BRepGraph::EditorView::RemoveNode(): guard active on node");

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                         "RemoveNode: node index is out of range",
                         Standard_VOID_RETURN);
  if (!isActiveNode(aStorage, theNode))
  {
    return;
  }

  // Propagate SubtreeGen upward to parents before unbinding relation entries,
  // so generation-based caches on parent nodes detect the removal.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      break;
    default:
      myGraph->propagateSubtreeGen(theNode);
      break;
  }

  // Notify layers while relation parent context is still available.
  myGraph->myData->myLayerRegistry.DispatchOnNodeRemoved(theNode);

  auto markOwnedRefRemoved = [&](const BRepGraph_RefId theRef) {
    if (!isRefInRange(aStorage, theRef) || theRef.IsRemoved(*myGraph)
        || !aStorage.MarkRemovedRef(theRef))
    {
      return;
    }
    myGraph->markRefModified(theRef);
    const BRepGraph_RefUID aUID(theRef.RefKind, aStorage.BaseRef(theRef).UID);
    if (aUID.IsValid())
    {
      std::unique_lock<std::shared_mutex> aLock(myGraph->myData->myIncStorage.myRefUIDToRefIdMutex);
      myGraph->myData->myIncStorage.myRefUIDToRefId.UnBind(aUID);
    }
    myGraph->myData->myLayerRegistry.DispatchOnRefRemoved(theRef);
  };

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId       anEdgeId = BRepGraph_EdgeId::FromNodeId(theNode);
      const BRepGraphInc::EdgeDef& anEdge   = aStorage.Edge(anEdgeId);
      const BRepGraph_VertexId     aStartVtx =
        anEdge.StartVertexRefId.IsValid(aStorage.NbVertexRefs())
              ? aStorage.VertexRef(anEdge.StartVertexRefId).ChildVertexId
              : BRepGraph_VertexId();
      const BRepGraph_VertexId anEndVtx =
        anEdge.EndVertexRefId.IsValid(aStorage.NbVertexRefs())
          ? aStorage.VertexRef(anEdge.EndVertexRefId).ChildVertexId
          : BRepGraph_VertexId();
      markOwnedRefRemoved(anEdge.StartVertexRefId);
      markOwnedRefRemoved(anEdge.EndVertexRefId);
      aStorage.RebindVertexEdge(aStartVtx, BRepGraph_VertexId(), anEdgeId, BRepGraph_VertexRefId());
      aStorage.RebindVertexEdge(anEndVtx, BRepGraph_VertexId(), anEdgeId, BRepGraph_VertexRefId());
      // Clear geometry RepId fields on the edge definition.
      BRepGraphInc::EdgeDef& aMutEdge = aStorage.ChangeEdge(anEdgeId);
      if (aMutEdge.Curve3DRepId.IsValid())
      {
        aStorage.MarkRemoved(aMutEdge.Curve3DRepId);
      }
      if (aMutEdge.Polygon3DRepId.IsValid())
      {
        aStorage.MarkRemoved(aMutEdge.Polygon3DRepId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::FromNodeId(theNode);
      BRepGraphInc::CoEdgeDef& aCoEdge   = aStorage.ChangeCoEdge(aCoEdgeId);
      aStorage.MarkRemoved(aCoEdge.Curve2DRepId);
      aStorage.MarkRemoved(aCoEdge.Polygon2DRepId);
      aStorage.MarkRemoved(aCoEdge.PolygonOnTriRepId);
      if (aCoEdge.ParentWireId.IsValid(aStorage.NbWires()))
      {
        aStorage.DetachCoEdgeUse(aCoEdge.ParentWireId, aCoEdgeId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::FromNodeId(theNode);
      // Mark face as removed before detaching refs to prevent re-dirtying
      // via markRefModified -> markModified during WireRef cleanup.
      myGraph->myData->myIncStorage.MarkRemoved(theNode);
      // Clear geometry RepId fields on the face definition.
      BRepGraphInc::FaceDef& aMutFace = aStorage.ChangeFace(aFaceId);
      if (aMutFace.SurfaceRepId.IsValid())
      {
        aStorage.MarkRemoved(aMutFace.SurfaceRepId);
      }
      if (aMutFace.TriangulationRepId.IsValid())
      {
        aStorage.MarkRemoved(aMutFace.TriangulationRepId);
      }
      while (!aStorage.FaceRelations(aFaceId).WireRefIds.IsEmpty())
      {
        const BRepGraph_WireRefId aWireRefId = aStorage.FaceRelations(aFaceId).WireRefIds.First();
        markOwnedRefRemoved(aWireRefId);
        aStorage.DetachWireFromFace(aFaceId, aWireRefId);
      }
      while (!aStorage.FaceRelations(aFaceId).ParentFaceRefIds.IsEmpty())
      {
        const BRepGraph_FaceRefId aFaceRefId =
          aStorage.FaceRelations(aFaceId).ParentFaceRefIds.First();
        const BRepGraph_ShellId aParentShellId = aStorage.FaceRef(aFaceRefId).ParentShellId;
        markOwnedRefRemoved(aFaceRefId);
        aStorage.DetachFaceFromShell(aParentShellId, aFaceRefId);
      }
      // FullCoEdgeIterator required: CoEdges are already marked removed by RemoveSubgraph
      // before RemoveNode(Face) runs, so the default iterator would skip them.
      for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(*myGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
      {
        const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();
        const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
        if (aCoEdge.FaceId != aFaceId)
        {
          continue;
        }
        BRepGraphInc::CoEdgeDef& aMutCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
        aMutCoEdge.FaceId                   = BRepGraph_FaceId();
        if (aMutCoEdge.Curve2DRepId.IsValid())
        {
          aStorage.MarkRemoved(aMutCoEdge.Curve2DRepId);
        }
        if (aMutCoEdge.Polygon2DRepId.IsValid())
        {
          aStorage.MarkRemoved(aMutCoEdge.Polygon2DRepId);
        }
        if (aMutCoEdge.PolygonOnTriRepId.IsValid())
        {
          aStorage.MarkRemoved(aMutCoEdge.PolygonOnTriRepId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell:
      forEachId(aStorage.ShellRelations(BRepGraph_ShellId::FromNodeId(theNode)).FaceRefIds,
                markOwnedRefRemoved);
      break;
    case BRepGraph_NodeId::Kind::Solid:
      forEachId(aStorage.SolidRelations(BRepGraph_SolidId::FromNodeId(theNode)).ShellRefIds,
                markOwnedRefRemoved);
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      forEachId(aStorage.CompSolidRelations(BRepGraph_CompSolidId::FromNodeId(theNode)).SolidRefIds,
                markOwnedRefRemoved);
      break;
    case BRepGraph_NodeId::Kind::Compound:
      forEachId(aStorage.CompoundRelations(BRepGraph_CompoundId::FromNodeId(theNode)).ChildRefIds,
                markOwnedRefRemoved);
      break;
    case BRepGraph_NodeId::Kind::Product:
      forEachId(
        aStorage.ProductRelations(BRepGraph_ProductId::FromNodeId(theNode)).OccurrenceRefIds,
        markOwnedRefRemoved);
      break;
    default:
      break;
  }

  // Mark removed on the entity (which is the sole definition store).
  BRepGraphInc::BaseDef* aDef = myGraph->changeTopoEntity(theNode);
  if (aDef != nullptr && !theNode.IsRemoved(*myGraph))
  {
    myGraph->myData->myIncStorage.MarkRemoved(theNode);
    // If this is a product, remove from root products.
    if (theNode.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      removeFromRootProducts(myGraph->myData->myIncStorage.ChangeRootProductIds(),
                             BRepGraph_ProductId::FromNodeId(theNode));
    }
  }

  // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the removal.
  if (aDef != nullptr)
  {
    ++aDef->OwnGen;
    ++aDef->SubtreeGen;
  }

  {
    const BRepGraph_UID aUID =
      (aDef != nullptr) ? BRepGraph_UID(theNode.NodeKind, aDef->UID) : BRepGraph_UID();
    if (aUID.IsValid())
    {
      std::unique_lock<std::shared_mutex> aLock(myGraph->myData->myIncStorage.myUIDToNodeIdMutex);
      myGraph->myData->myIncStorage.myUIDToNodeId.UnBind(aUID);
    }
  }

  myGraph->myData->myIncStorage.UnbindCurrentShape(theNode);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::ReplaceNode(const BRepGraph_NodeId theNode,
                                                const BRepGraph_NodeId theReplacement)
{
  if (!theNode.IsValid() || theReplacement == theNode)
  {
    return;
  }
  myGraph->Editor().requireUnlocked(theNode, "BRepGraph::EditorView::ReplaceNode(): locked node");
  myGraph->Editor().requireNoActiveGuard(
    theNode,
    "BRepGraph::EditorView::ReplaceNode(): guard active on node");

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theReplacement.IsValid() || !isNodeIndexInRange(aStorage, theReplacement)
      || !isActiveNode(aStorage, theReplacement))
  {
    RemoveNode(theNode);
    return;
  }

  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge
      && theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                           "ReplaceNode: source edge index is out of range",
                           Standard_VOID_RETURN);
    Standard_ASSERT_RETURN(!aStorage.IsRemoved(BRepGraph_EdgeId(theReplacement)),
                           "ReplaceNode: replacement edge must be active",
                           Standard_VOID_RETURN);

    rebindCoEdgesForEdgeReplacement(aStorage,
                                    BRepGraph_EdgeId::FromNodeId(theNode),
                                    BRepGraph_EdgeId::FromNodeId(theReplacement));
  }

  Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                         "ReplaceNode: node index is out of range",
                         Standard_VOID_RETURN);
  if (!isActiveNode(aStorage, theNode))
  {
    return;
  }

  // Propagate SubtreeGen upward to parents before unbinding relation entries,
  // so generation-based caches on parent nodes detect the removal.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      break;
    default:
      myGraph->propagateSubtreeGen(theNode);
      break;
  }

  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    const BRepGraph_EdgeId       anEdgeId = BRepGraph_EdgeId::FromNodeId(theNode);
    const BRepGraphInc::EdgeDef& anEdge   = aStorage.Edge(anEdgeId);
    unbindCoEdgesOfRemovedEdge(aStorage, anEdgeId);

    auto markOwnedRefRemoved = [&](const BRepGraph_RefId theRef) {
      if (!isRefInRange(aStorage, theRef) || theRef.IsRemoved(*myGraph)
          || !aStorage.MarkRemovedRef(theRef))
      {
        return;
      }
      myGraph->markRefModified(theRef);
      const BRepGraph_RefUID aUID(theRef.RefKind, aStorage.BaseRef(theRef).UID);
      if (aUID.IsValid())
      {
        std::unique_lock<std::shared_mutex> aLock(
          myGraph->myData->myIncStorage.myRefUIDToRefIdMutex);
        myGraph->myData->myIncStorage.myRefUIDToRefId.UnBind(aUID);
      }
      myGraph->myData->myLayerRegistry.DispatchOnRefRemoved(theRef);
    };
    markOwnedRefRemoved(anEdge.StartVertexRefId);
    markOwnedRefRemoved(anEdge.EndVertexRefId);
  }
  else if (theNode.NodeKind == BRepGraph_NodeId::Kind::CoEdge)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::FromNodeId(theNode);
    if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
    {
      const BRepGraph_WireId aParentWireId = aStorage.CoEdge(aCoEdgeId).ParentWireId;
      if (aParentWireId.IsValid(aStorage.NbWires()))
      {
        aStorage.DetachCoEdgeUse(aParentWireId, aCoEdgeId);
      }
    }
  }
  else if (theNode.NodeKind == BRepGraph_NodeId::Kind::Wire)
  {
    const BRepGraph_WireId aWireId = BRepGraph_WireId::FromNodeId(theNode);
    if (aWireId.IsValid(aStorage.NbWires()))
    {
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
        aStorage.WireRelations(aWireId).CoEdgeIds;
      const size_t                                   aNbCoEdges = aCoEdges.Size();
      NCollection_LocalArray<BRepGraph_CoEdgeId, 16> aCoEdgeSnapshot(
        static_cast<uint32_t>(aNbCoEdges));
      NCollection_LocalArray<BRepGraph_EdgeId, 16> anEdgeSnapshot(
        static_cast<uint32_t>(aNbCoEdges));
      for (size_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
      {
        const BRepGraph_CoEdgeId aCoEdgeId                 = aCoEdges.Value(aCoEdgeIdx);
        aCoEdgeSnapshot[static_cast<uint32_t>(aCoEdgeIdx)] = aCoEdgeId;
        anEdgeSnapshot[static_cast<uint32_t>(aCoEdgeIdx)] =
          aCoEdgeId.IsValid(aStorage.NbCoEdges()) ? aStorage.CoEdge(aCoEdgeId).ChildEdgeId
                                                  : BRepGraph_EdgeId();
      }
      for (size_t aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
      {
        const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeSnapshot[static_cast<uint32_t>(aCoEdgeIdx)];
        if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
        {
          aStorage.DetachCoEdgeUse(aWireId, aCoEdgeId);
          aStorage.SetRemoved(aCoEdgeId, true);
          myGraph->markModified(BRepGraph_NodeId(aCoEdgeId));
        }
      }
      for (BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::Start();
           aCoEdgeId.IsValid(aStorage.NbCoEdges());
           ++aCoEdgeId)
      {
        if (!aStorage.IsRemoved(aCoEdgeId) && aStorage.CoEdge(aCoEdgeId).ParentWireId == aWireId)
        {
          aStorage.DetachCoEdgeUse(aWireId, aCoEdgeId);
          aStorage.SetRemoved(aCoEdgeId, true);
          myGraph->markModified(BRepGraph_NodeId(aCoEdgeId));
        }
      }
    }
  }

  // Mark removed on the entity (which is the sole definition store).
  BRepGraphInc::BaseDef* aDef = myGraph->changeTopoEntity(theNode);
  if (aDef != nullptr && !theNode.IsRemoved(*myGraph))
  {
    myGraph->myData->myIncStorage.MarkRemoved(theNode);
    // If this is a product, remove from root products.
    if (theNode.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      removeFromRootProducts(myGraph->myData->myIncStorage.ChangeRootProductIds(),
                             BRepGraph_ProductId::FromNodeId(theNode));
    }
  }

  // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the removal.
  if (aDef != nullptr)
  {
    ++aDef->OwnGen;
    ++aDef->SubtreeGen;
  }

  {
    const BRepGraph_UID aUID =
      (aDef != nullptr) ? BRepGraph_UID(theNode.NodeKind, aDef->UID) : BRepGraph_UID();
    if (aUID.IsValid())
    {
      std::unique_lock<std::shared_mutex> aLock(myGraph->myData->myIncStorage.myUIDToNodeIdMutex);
      myGraph->myData->myIncStorage.myUIDToNodeId.UnBind(aUID);
    }
  }

  myGraph->myData->myIncStorage.UnbindCurrentShape(theNode);

  // Notify registered layers.
  myGraph->myData->myLayerRegistry.DispatchOnNodeReplaced(theNode, theReplacement);
}

//=================================================================================================

namespace
{
struct RemoveSubgraphScope
{
  BRepGraph_Data& Data;
  bool            IsOutermost;

  explicit RemoveSubgraphScope(BRepGraph_Data& theData)
      : Data(theData),
        IsOutermost(false)
  {
    Data.myIncStorage.IncrementRemoveSubgraphDepth();
    IsOutermost = (Data.myIncStorage.RemoveSubgraphDepth() == 1);
  }

  ~RemoveSubgraphScope() { Data.myIncStorage.DecrementRemoveSubgraphDepth(); }

  RemoveSubgraphScope(const RemoveSubgraphScope&)            = delete;
  RemoveSubgraphScope& operator=(const RemoveSubgraphScope&) = delete;
};
} // namespace

void BRepGraph::EditorView::GenOps::RemoveSubgraph(const BRepGraph_NodeId theNode)
{
  myGraph->Editor().requireUnlocked(theNode,
                                    "BRepGraph::EditorView::RemoveSubgraph(): locked node");
  myGraph->Editor().requireNoActiveGuard(
    theNode,
    "BRepGraph::EditorView::RemoveSubgraph(): guard active on node");

  // Cascade-aware relation maintenance: each removal updates only affected
  // relation entries; the outermost call validates the final invariant.
  RemoveSubgraphScope aScope(*myGraph->myData);
  const bool          isOutermost = aScope.IsOutermost;

  // Collect and recursively remove children BEFORE marking this node as removed,
  // because iterators (DefsIterator, RefsIterator) skip removed parents/children.
  // Children shared with other active parents are NOT removed (ownership-aware).
  //
  // Product and Occurrence require special handling:
  //  - Product: occurrence children cascade.
  //  - Occurrence: child occurrence cascade + parent product ref detachment.
  // All other kinds use the generic ChildExplorer/ParentExplorer cascade.
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product: {
      if (theNode.IsValid(aStorage.NbProducts()))
      {
        // Snapshot occurrence indices before iterating, because RemoveSubgraph(Occurrence)
        // modifies the parent's OccurrenceRefIds via swap-remove.
        NCollection_LinearVector<uint32_t> anOccIndices;
        for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph,
                                                       BRepGraph_ProductId::FromNodeId(theNode));
             anOccIt.More();
             anOccIt.Next())
        {
          anOccIndices.Append(aStorage.OccurrenceRef(anOccIt.CurrentId()).ChildOccurrenceId.Index);
        }
        for (const uint32_t anOccIdx : anOccIndices)
        {
          RemoveSubgraph(BRepGraph_OccurrenceId(anOccIdx));
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      if (theNode.IsValid(aStorage.NbOccurrences()))
      {
        const BRepGraphInc::OccurrenceDef& anOcc =
          myGraph->myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNode));

        // If the child is a topology node and has no other active usage, cascade into it.
        if (anOcc.ChildNodeId.IsValid()
            && BRepGraph_NodeId::IsTopologyKind(anOcc.ChildNodeId.NodeKind))
        {
          if (!hasAnyActiveUsage(*myGraph, anOcc.ChildNodeId))
          {
            RemoveSubgraph(anOcc.ChildNodeId);
          }
        }
        // Detach all parent product memberships for this occurrence.
        NCollection_LinearVector<BRepGraph_ProductId> aParentProducts;
        for (BRepGraph_ProductId aProductId = BRepGraph_ProductId::Start();
             aProductId.IsValid(aStorage.NbProducts());
             ++aProductId)
        {
          if (!productHasOccurrenceDef(aStorage, aProductId, BRepGraph_OccurrenceId(theNode)))
          {
            continue;
          }
          aParentProducts.Append(aProductId);
        }
        for (const BRepGraph_ProductId& aProductId : aParentProducts)
        {
          if (!aProductId.IsValid(aStorage.NbProducts()) || aStorage.IsRemoved(aProductId))
          {
            continue;
          }
          const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aRefIds =
            myGraph->myData->myIncStorage.ProductRelations(aProductId).OccurrenceRefIds;
          for (const BRepGraph_OccurrenceRefId& aOccRefId : aRefIds)
          {
            if (!aOccRefId.IsValid(aStorage.NbOccurrenceRefs()))
            {
              continue;
            }
            const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aOccRefId);
            if (!aStorage.IsRemoved(aOccRefId) && aRef.ChildOccurrenceId == theNode)
            {
              myGraph->myData->myIncStorage.MarkRemovedRef(aOccRefId);
              myGraph->myData->myIncStorage.DetachOccurrenceFromProduct(aProductId, aOccRefId);
              myGraph->markModified(aProductId);
              if (anOcc.ChildNodeId.IsValid())
              {
              }
              break;
            }
          }
        }

        // After detaching, if the child product lost its last active parent occurrence,
        // re-add it to root products.
        if (anOcc.ChildNodeId.IsValid()
            && anOcc.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
        {
          const BRepGraph_ProductId aChildProduct =
            BRepGraph_ProductId::FromNodeId(anOcc.ChildNodeId);
          if (aChildProduct.IsValid(aStorage.NbProducts()) && !aStorage.IsRemoved(aChildProduct)
              && !hasAnyActiveUsage(*myGraph, anOcc.ChildNodeId))
          {
            myGraph->myData->myIncStorage.ChangeRootProductIds().Append(aChildProduct);
          }
        }
      }
      break;
    }
    default: {
      // Generic topology cascade via ChildExplorer(DirectChildren) + ParentExplorer(DirectParents).
      // Covers Compound, CompSolid, Solid, Shell, Face, Wire, CoEdge, Edge, Vertex.
      NCollection_LinearVector<BRepGraph_NodeId> aChildNodes;
      for (BRepGraph_ChildExplorer anExp(*myGraph,
                                         theNode,
                                         BRepGraph_ChildExplorer::TraversalMode::DirectChildren);
           anExp.More();
           anExp.Next())
      {
        aChildNodes.Append(anExp.Current().DefId);
      }
      for (const BRepGraph_NodeId& aChild : aChildNodes)
      {
        if (!hasOtherActiveParent(*myGraph, aChild, theNode))
        {
          RemoveSubgraph(aChild);
        }
      }
      break;
    }
  }

  // Mark the node as removed AFTER children have been collected and processed,
  // so that iterators can still see this node as a valid parent during traversal.
  RemoveNode(theNode);

  if (isOutermost)
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveSubgraph: relation invariant violated");
  }
}

//=================================================================================================

bool BRepGraph::EditorView::GenOps::RemoveRef(const BRepGraph_RefId theRef)
{
  if (!theRef.IsValid())
  {
    return false;
  }
  myGraph->Editor().requireUnlocked(theRef, "BRepGraph::EditorView::RemoveRef(): locked reference");
  myGraph->Editor().requireNoActiveGuard(
    theRef,
    "BRepGraph::EditorView::RemoveRef(): guard active on reference");
  // Snapshot ref state before MarkRemoved flips IsRemoved.
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theRef.IsRemoved(*myGraph))
  {
    return false;
  }
  detachRefRelations(aStorage, theRef);
  if (!aStorage.MarkRemovedRef(theRef))
  {
    return false;
  }
  myGraph->markRefModified(theRef);
  {
    const BRepGraph_RefUID aUID(theRef.RefKind, aStorage.BaseRef(theRef).UID);
    if (aUID.IsValid())
    {
      std::unique_lock<std::shared_mutex> aLock(myGraph->myData->myIncStorage.myRefUIDToRefIdMutex);
      myGraph->myData->myIncStorage.myRefUIDToRefId.UnBind(aUID);
    }
  }

  myGraph->myData->myLayerRegistry.DispatchOnRefRemoved(theRef);
  if (aStorage.RemoveSubgraphDepth() == 0 && !aStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(aStorage.ValidateRelations(),
                         "BRepGraph::RemoveRef: relation invariant violated");
  }
  return true;
}

bool BRepGraph::EditorView::GenOps::RemoveRef(const BRepGraph_NodeId theParent,
                                              const BRepGraph_RefId  theRef,
                                              const bool             theToPruneOrphanedChild)
{
  if (!theRef.IsValid())
  {
    return false;
  }
  myGraph->Editor().requireUnlocked(theRef, "BRepGraph::EditorView::RemoveRef(): locked reference");
  myGraph->Editor().requireNoActiveGuard(
    theRef,
    "BRepGraph::EditorView::RemoveRef(): guard active on reference");
  Standard_ASSERT_RETURN(isExpectedParentKindForRef(theParent, theRef.RefKind),
                         "RemoveRef: reference kind is incompatible with parent kind",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theParent) || !isRefInRange(aStorage, theRef)
      || theRef.IsRemoved(*myGraph))
  {
    return false;
  }

  const BRepGraph_NodeId aChildNode = refChildNode(*myGraph, theRef);
  bool                   isRemoved  = false;
  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_EdgeId      anEdgeId = BRepGraph_EdgeId::FromNodeId(theParent);
      const BRepGraph_VertexRefId aVertexRefId(theRef);
      BRepGraphInc::EdgeDef&      anEdge = aStorage.ChangeEdge(anEdgeId);
      if (anEdge.StartVertexRefId != aVertexRefId && anEdge.EndVertexRefId != aVertexRefId)
      {
        return false;
      }
      if (anEdge.StartVertexRefId == aVertexRefId)
      {
        anEdge.StartVertexRefId = BRepGraph_VertexRefId();
      }
      if (anEdge.EndVertexRefId == aVertexRefId)
      {
        anEdge.EndVertexRefId = BRepGraph_VertexRefId();
      }
      if (!isRefOwnedByAnyParent(myGraph->myData->myIncStorage, aVertexRefId)
          && !RemoveRef(aVertexRefId))
      {
        return false;
      }
      if (theToPruneOrphanedChild && aChildNode.IsValid()
          && !hasAnyActiveUsage(*myGraph, aChildNode))
      {
        RemoveSubgraph(aChildNode);
      }
      isRemoved = true;
      break;
    }
    case BRepGraph_RefId::Kind::Shell:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_ShellRefId(theRef),
        myGraph->myData->myIncStorage.SolidRelations(BRepGraph_SolidId(theParent)).ShellRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    case BRepGraph_RefId::Kind::Face:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_FaceRefId(theRef),
        myGraph->myData->myIncStorage.ShellRelations(BRepGraph_ShellId(theParent)).FaceRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    case BRepGraph_RefId::Kind::Wire:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_WireRefId(theRef),
        myGraph->myData->myIncStorage.FaceRelations(BRepGraph_FaceId(theParent)).WireRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    case BRepGraph_RefId::Kind::Solid:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_SolidRefId(theRef),
        myGraph->myData->myIncStorage.CompSolidRelations(BRepGraph_CompSolidId(theParent))
          .SolidRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    case BRepGraph_RefId::Kind::Child:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_ChildRefId(theRef),
        myGraph->myData->myIncStorage.CompoundRelations(BRepGraph_CompoundId(theParent))
          .ChildRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    case BRepGraph_RefId::Kind::Occurrence:
      isRemoved = detachOrderedParentRef(
        *myGraph,
        myGraph->myData->myIncStorage,
        BRepGraph_OccurrenceRefId(theRef),
        myGraph->myData->myIncStorage.ProductRelations(BRepGraph_ProductId(theParent))
          .OccurrenceRefIds,
        aChildNode,
        theToPruneOrphanedChild);
      break;
    default:
      isRemoved = RemoveRef(theRef);
      break;
  }
  if (!isRemoved)
  {
    return false;
  }

  myGraph->markModified(theParent);
  return true;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::CleanupRemovedReferences()
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Build a set of all removed NodeIds (BRepGraph_FullXxxIterator includes removed).
  NCollection_FlatMap<BRepGraph_NodeId> aRemovedNodes;
  for (BRepGraph_FullVertexIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullEdgeIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullCoEdgeIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullWireIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullFaceIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullShellIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullSolidIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullCompoundIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullCompSolidIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullProductIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }
  for (BRepGraph_FullOccurrenceIterator anIt(*myGraph); anIt.More(); anIt.Next())
  {
    if (aStorage.IsRemoved(anIt.CurrentId()))
    {
      aRemovedNodes.Add(anIt.CurrentId());
    }
  }

  // Helper: erase one value from a DynamicArray using shift-down + EraseLast.
  auto markRemovedRefEntry = [&](const BRepGraph_RefId theRef) {
    if (!isRefInRange(aStorage, theRef) || theRef.IsRemoved(*myGraph)
        || !aStorage.MarkRemovedRef(theRef))
    {
      return;
    }
    myGraph->markRefModified(theRef);
    const BRepGraph_RefUID aUID(theRef.RefKind, aStorage.BaseRef(theRef).UID);
    if (aUID.IsValid())
    {
      std::unique_lock<std::shared_mutex> aLock(myGraph->myData->myIncStorage.myRefUIDToRefIdMutex);
      myGraph->myData->myIncStorage.myRefUIDToRefId.UnBind(aUID);
    }
    myGraph->myData->myLayerRegistry.DispatchOnRefRemoved(theRef);
  };

  for (NCollection_FlatMap<BRepGraph_NodeId>::Iterator anIt(aRemovedNodes); anIt.More();
       anIt.Next())
  {
    const BRepGraph_NodeId aRemovedNode = anIt.Value();
    switch (aRemovedNode.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge =
          aStorage.Edge(BRepGraph_EdgeId::FromNodeId(aRemovedNode));
        markRemovedRefEntry(anEdge.StartVertexRefId);
        markRemovedRefEntry(anEdge.EndVertexRefId);
        break;
      }
      case BRepGraph_NodeId::Kind::Face:
        forEachId(aStorage.FaceRelations(BRepGraph_FaceId::FromNodeId(aRemovedNode)).WireRefIds,
                  markRemovedRefEntry);
        break;
      case BRepGraph_NodeId::Kind::Shell:
        forEachId(aStorage.ShellRelations(BRepGraph_ShellId::FromNodeId(aRemovedNode)).FaceRefIds,
                  markRemovedRefEntry);
        break;
      case BRepGraph_NodeId::Kind::Solid:
        forEachId(aStorage.SolidRelations(BRepGraph_SolidId::FromNodeId(aRemovedNode)).ShellRefIds,
                  markRemovedRefEntry);
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        forEachId(
          aStorage.CompSolidRelations(BRepGraph_CompSolidId::FromNodeId(aRemovedNode)).SolidRefIds,
          markRemovedRefEntry);
        break;
      case BRepGraph_NodeId::Kind::Compound:
        forEachId(
          aStorage.CompoundRelations(BRepGraph_CompoundId::FromNodeId(aRemovedNode)).ChildRefIds,
          markRemovedRefEntry);
        break;
      case BRepGraph_NodeId::Kind::Product:
        forEachId(
          aStorage.ProductRelations(BRepGraph_ProductId::FromNodeId(aRemovedNode)).OccurrenceRefIds,
          markRemovedRefEntry);
        break;
      default:
        break;
    }
  }

  // Phase 2: CoEdgeDefs - clear ChildEdgeId / FaceId.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId       aCEId = anIt.CurrentId();
    const BRepGraphInc::CoEdgeDef& aCE   = anIt.Current();
    if (aCE.ChildEdgeId.IsValid(aStorage.NbEdges()) && aRemovedNodes.Contains(aCE.ChildEdgeId))
    {
      const BRepGraph_EdgeId anOldEdge         = aCE.ChildEdgeId;
      aStorage.ChangeCoEdge(aCEId).ChildEdgeId = BRepGraph_EdgeId();
      aStorage.RebindCoEdgeEdge(aCEId, anOldEdge, BRepGraph_EdgeId());
    }
    if (aCE.FaceId.IsValid(aStorage.NbFaces()) && aRemovedNodes.Contains(aCE.FaceId))
    {
      BRepGraphInc::CoEdgeDef& aMutCE = aStorage.ChangeCoEdge(aCEId);
      aMutCE.FaceId                   = BRepGraph_FaceId();
      clearCoEdgeFaceScopedRepresentations(aStorage, aMutCE);
    }
  }

  // Phase 3: EdgeDefs - clear vertex refs.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anIt.Current();
    if (anEdge.StartVertexRefId.IsValid(aStorage.NbVertexRefs()))
    {
      const BRepGraphInc::VertexRef& aVRef = aStorage.VertexRef(anEdge.StartVertexRefId);
      if (aStorage.IsRemoved(anEdge.StartVertexRefId)
          || (aVRef.ChildVertexId.IsValid(aStorage.NbVertices())
              && aRemovedNodes.Contains(aVRef.ChildVertexId)))
      {
        const BRepGraph_VertexId aVertexId = aVRef.ChildVertexId;
        if (aVertexId.IsValid())
        {
        }
        markRemovedRefEntry(anEdge.StartVertexRefId);
        aStorage.ChangeEdge(anEdgeId).StartVertexRefId = BRepGraph_VertexRefId();
        aStorage.RebindVertexEdge(aVertexId,
                                  BRepGraph_VertexId(),
                                  anEdgeId,
                                  BRepGraph_VertexRefId());
      }
    }
    if (anEdge.EndVertexRefId.IsValid(aStorage.NbVertexRefs()))
    {
      const BRepGraphInc::VertexRef& aVRef = aStorage.VertexRef(anEdge.EndVertexRefId);
      if (aStorage.IsRemoved(anEdge.EndVertexRefId)
          || (aVRef.ChildVertexId.IsValid(aStorage.NbVertices())
              && aRemovedNodes.Contains(aVRef.ChildVertexId)))
      {
        const BRepGraph_VertexId aVertexId = aVRef.ChildVertexId;
        if (aVertexId.IsValid())
        {
        }
        markRemovedRefEntry(anEdge.EndVertexRefId);
        aStorage.ChangeEdge(anEdgeId).EndVertexRefId = BRepGraph_VertexRefId();
        aStorage.RebindVertexEdge(aVertexId,
                                  BRepGraph_VertexId(),
                                  anEdgeId,
                                  BRepGraph_VertexRefId());
      }
    }
  }

  // Phase 4a: Mark orphaned CoEdges as removed. A CoEdge whose ChildEdgeId was
  // cleared in Phase 2 has no topological identity and must be retired before
  // wire refs are pruned (so Phase 4b's IsRemoved check picks it up).
  // Note: do not check FaceId here - free-wire coedges have FaceId
  // intentionally invalid and must not be retired.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId       aCEId = anIt.CurrentId();
    const BRepGraphInc::CoEdgeDef& aCE   = anIt.Current();
    if (aCE.ParentWireId.IsValid(aStorage.NbWires()) && aRemovedNodes.Contains(aCE.ParentWireId))
    {
      aStorage.DetachCoEdgeUse(aCE.ParentWireId, aCEId);
      aStorage.MarkRemoved(BRepGraph_NodeId(aCEId));
    }
    else if (!aCE.ChildEdgeId.IsValid())
    {
      aStorage.MarkRemoved(BRepGraph_NodeId(aCEId));
    }
  }

  // Phase 4b: WireDefs - clean CoEdges that are removed/dangling.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_WireId aWireId = anIt.CurrentId();
    for (size_t aCoEdgeIdx = 0; aCoEdgeIdx < aStorage.WireRelations(aWireId).CoEdgeIds.Size();)
    {
      const BRepGraph_CoEdgeId aCEId = aStorage.WireRelations(aWireId).CoEdgeIds.Value(aCoEdgeIdx);
      if (!aCEId.IsValid(aStorage.NbCoEdges()))
      {
        ++aCoEdgeIdx;
        continue;
      }
      const BRepGraphInc::CoEdgeDef& aCE = aStorage.CoEdge(aCEId);
      if (aStorage.IsRemoved(aCEId) || !aCE.ChildEdgeId.IsValid()
          || (aCE.ChildEdgeId.IsValid(aStorage.NbEdges())
              && aRemovedNodes.Contains(aCE.ChildEdgeId)))
      {
        aStorage.DetachCoEdgeUse(aWireId, aCEId);
        if (aCE.ChildEdgeId.IsValid()
            && !wireHasActiveEdgeThroughAnotherCoEdge(aStorage, aWireId, aCE.ChildEdgeId, aCEId))
        {
        }
        continue;
      }
      ++aCoEdgeIdx;
    }
  }

  // Phase 5: FaceDefs - clean WireRefs.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_FaceId aFaceId = anIt.CurrentId();
    // WireRefs
    for (size_t aWireRefIdx = 0; aWireRefIdx < aStorage.FaceRelations(aFaceId).WireRefIds.Size();)
    {
      const BRepGraph_WireRefId aWRId =
        aStorage.FaceRelations(aFaceId).WireRefIds.Value(aWireRefIdx);
      const BRepGraphInc::WireRef& aWR = aStorage.WireRef(aWRId);
      if (aStorage.IsRemoved(aWRId))
      {
        ++aWireRefIdx;
        continue;
      }
      if (aWR.ChildWireId.IsValid(aStorage.NbWires()) && aRemovedNodes.Contains(aWR.ChildWireId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aWRId));
        aStorage.DetachWireFromFace(aFaceId, aWRId);
        continue;
      }
      ++aWireRefIdx;
    }
  }

  // Phase 6: ShellDefs - clean FaceRefs.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_ShellId aShellId = anIt.CurrentId();
    // FaceRefs
    for (size_t aFaceRefIdx = 0; aFaceRefIdx < aStorage.ShellRelations(aShellId).FaceRefIds.Size();)
    {
      const BRepGraph_FaceRefId aFRId =
        aStorage.ShellRelations(aShellId).FaceRefIds.Value(aFaceRefIdx);
      const BRepGraphInc::FaceRef& aFR = aStorage.FaceRef(aFRId);
      if (aStorage.IsRemoved(aFRId))
      {
        ++aFaceRefIdx;
        continue;
      }
      if (aFR.ChildFaceId.IsValid(aStorage.NbFaces()) && aRemovedNodes.Contains(aFR.ChildFaceId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aFRId));
        aStorage.DetachFaceFromShell(aShellId, aFRId);
        continue;
      }
      ++aFaceRefIdx;
    }
  }

  // Phase 7: SolidDefs - clean ShellRefs.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_SolidId aSolidId = anIt.CurrentId();
    // ShellRefs
    for (size_t aShellRefIdx = 0;
         aShellRefIdx < aStorage.SolidRelations(aSolidId).ShellRefIds.Size();)
    {
      const BRepGraph_ShellRefId aSRId =
        aStorage.SolidRelations(aSolidId).ShellRefIds.Value(aShellRefIdx);
      const BRepGraphInc::ShellRef& aSR = aStorage.ShellRef(aSRId);
      if (aStorage.IsRemoved(aSRId))
      {
        ++aShellRefIdx;
        continue;
      }
      if (aSR.ChildShellId.IsValid(aStorage.NbShells()) && aRemovedNodes.Contains(aSR.ChildShellId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aSRId));
        aStorage.DetachShellFromSolid(aSolidId, aSRId);
        continue;
      }
      ++aShellRefIdx;
    }
  }

  // Phase 8: CompoundDefs - clean ChildRefs.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId aCompId = anIt.CurrentId();
    for (size_t aChildRefIdx = 0;
         aChildRefIdx < aStorage.CompoundRelations(aCompId).ChildRefIds.Size();)
    {
      const BRepGraph_ChildRefId aCRId =
        aStorage.CompoundRelations(aCompId).ChildRefIds.Value(aChildRefIdx);
      const BRepGraphInc::ChildRef& aCR = aStorage.ChildRef(aCRId);
      if (aStorage.IsRemoved(aCRId))
      {
        ++aChildRefIdx;
        continue;
      }
      if (aCR.ChildNodeId.IsValid() && aRemovedNodes.Contains(aCR.ChildNodeId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aCRId));
        aStorage.DetachChildFromCompound(aCompId, aCRId);
        continue;
      }
      ++aChildRefIdx;
    }
  }

  // Phase 9: CompSolidDefs - clean SolidRefs.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompSolidId aCSId = anIt.CurrentId();
    for (size_t aSolidRefIdx = 0;
         aSolidRefIdx < aStorage.CompSolidRelations(aCSId).SolidRefIds.Size();)
    {
      const BRepGraph_SolidRefId aSRId =
        aStorage.CompSolidRelations(aCSId).SolidRefIds.Value(aSolidRefIdx);
      const BRepGraphInc::SolidRef& aSR = aStorage.SolidRef(aSRId);
      if (aStorage.IsRemoved(aSRId))
      {
        ++aSolidRefIdx;
        continue;
      }
      if (aSR.ChildSolidId.IsValid(aStorage.NbSolids()) && aRemovedNodes.Contains(aSR.ChildSolidId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aSRId));
        aStorage.DetachSolidFromCompSolid(aCSId, aSRId);
        continue;
      }
      ++aSolidRefIdx;
    }
  }

  // Phase 10: OccurrenceDefs - clear ChildNodeId and retire orphan Occurrences.
  // An Occurrence with no live child has no semantic meaning, so mark it
  // removed and add it to aRemovedNodes for downstream cleanup phases.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_OccurrenceId       anOccId = anIt.CurrentId();
    const BRepGraphInc::OccurrenceDef& anOcc   = anIt.Current();
    if (anOcc.ChildNodeId.IsValid() && aRemovedNodes.Contains(anOcc.ChildNodeId))
    {
      aStorage.ChangeOccurrence(anOccId).ChildNodeId = BRepGraph_NodeId();
      aStorage.MarkRemoved(BRepGraph_NodeId(anOccId));
      aRemovedNodes.Add(BRepGraph_NodeId(anOccId));
    }
  }

  // Phase 11: ProductDefs - clean OccurrenceRefs.
  for (BRepGraph_Iterator<BRepGraphInc::ProductDef> anIt(*myGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_ProductId aProdId = anIt.CurrentId();
    for (size_t anOccRefIdx = 0;
         anOccRefIdx < aStorage.ProductRelations(aProdId).OccurrenceRefIds.Size();)
    {
      const BRepGraph_OccurrenceRefId aORId =
        aStorage.ProductRelations(aProdId).OccurrenceRefIds.Value(anOccRefIdx);
      const BRepGraphInc::OccurrenceRef& aOR = aStorage.OccurrenceRef(aORId);
      if (aStorage.IsRemoved(aORId))
      {
        ++anOccRefIdx;
        continue;
      }
      if (aOR.ChildOccurrenceId.IsValid(aStorage.NbOccurrences())
          && aRemovedNodes.Contains(aOR.ChildOccurrenceId))
      {
        aStorage.MarkRemovedRef(BRepGraph_RefId(aORId));
        aStorage.DetachOccurrenceFromProduct(aProdId, aORId);
        continue;
      }
      ++anOccRefIdx;
    }
  }

  aStorage.RecountActiveCounts();
  Standard_ASSERT_VOID(aStorage.ValidateRelations(),
                       "CleanupRemovedReferences: relation invariant violated");
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                                 const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
  {
    ClearPCurve(theCoEdge);
    return;
  }

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theCoEdge.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(theCoEdge))
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
    if (aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
        && !aStorage.IsRemoved(aCoEdge.Curve2DRepId))
    {
      const BRepGraphInc::CoEdgeCurve2DRep& aUse = aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
      SetPCurve(theCoEdge, theCurve2d, aUse.ParamFirst, aUse.ParamLast);
      return;
    }
  }

  SetPCurve(theCoEdge, theCurve2d, theCurve2d->FirstParameter(), theCurve2d->LastParameter());
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph::EditorView::CoEdgeOps::Add(
  const BRepGraph_EdgeId                theEdge,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdge))
  {
    return BRepGraph_CoEdgeId();
  }

  const BRepGraph_CoEdgeId aCoEdgeId(aStorage.NbCoEdges());
  aStorage.AppendCoEdge();
  BRepGraphInc::CoEdgeDef& aCoEdge       = aStorage.ChangeCoEdge(aCoEdgeId);
  aCoEdge.ChildEdgeId                    = theEdge;
  aCoEdge.Orientation                    = theOrientation;
  BRepGraphInc::EdgeRelations& anEdgeRel = aStorage.ChangeEdgeRelationsInternal(theEdge);
  appendUniqueRelationId(anEdgeRel.CoEdgeIds, aCoEdgeId);
  myGraph->allocateUID(aCoEdgeId);
  myGraph->markModified(aCoEdgeId);
  myGraph->markModified(theEdge);
  return aCoEdgeId;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph::EditorView::CoEdgeOps::Add(
  const BRepGraph_EdgeId                theEdgeEntity,
  const BRepGraph_FaceId                theFaceEntity,
  const occ::handle<Geom2d_Curve>&      theCurve2d,
  const double                          theFirst,
  const double                          theLast,
  const BRepGraphInc::ParityOrientation theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdgeEntity) || !isActiveNode(aStorage, theFaceEntity)
      || theCurve2d.IsNull())
  {
    return BRepGraph_CoEdgeId();
  }

  // Create CoEdge entity for the new PCurve binding.
  const BRepGraph_CoEdgeId aCoEdgeId(aStorage.NbCoEdges());
  aStorage.AppendCoEdge();
  BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
  aCoEdge.ChildEdgeId              = theEdgeEntity;
  aCoEdge.FaceId                   = theFaceEntity;
  aCoEdge.Orientation              = theEdgeOrientation;
  if (!theCurve2d.IsNull())
  {
    // Create owned use record
    const BRepGraph_CoEdgeCurve2DRepId aRepId = aStorage.AppendCoEdgeCurve2DRep();
    BRepGraphInc::CoEdgeCurve2DRep&    aUse   = aStorage.ChangeCoEdgeCurve2DRep(aRepId);
    aUse.ParentCoEdgeId                       = aCoEdgeId;
    aUse.Curve                                = theCurve2d;
    aUse.ParamFirst                           = theFirst;
    aUse.ParamLast                            = theLast;
    aCoEdge.Curve2DRepId                      = aRepId;
  }

  BRepGraphInc::EdgeRelations& anEdgeRel = aStorage.ChangeEdgeRelationsInternal(theEdgeEntity);
  appendUniqueRelationId(anEdgeRel.CoEdgeIds, aCoEdgeId);

  myGraph->allocateUID(aCoEdgeId);

  myGraph->markModified(aCoEdgeId);
  myGraph->markModified(theEdgeEntity);
  myGraph->markModified(theFaceEntity);
  return aCoEdgeId;
}

//=================================================================================================

void BRepGraph::EditorView::BeginDeferredInvalidation()
{
  myGraph->myData->myIncStorage.SetDeferredMode(true);
}

//=================================================================================================

void BRepGraph::EditorView::EndDeferredInvalidation() noexcept
{
  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    return;
  }

  myGraph->myData->myIncStorage.SetDeferredMode(false);

  NCollection_LinearVector<BRepGraph_NodeId>& aDeferredList =
    myGraph->myData->myIncStorage.ChangeDeferredModified();

  if (aDeferredList.IsEmpty())
  {
    NCollection_LinearVector<BRepGraph_RefId>& aDeferredRefList =
      myGraph->myData->myIncStorage.ChangeDeferredRefModified();
    if (!aDeferredRefList.IsEmpty()
        && myGraph->myData->myLayerRegistry.HasRefModificationSubscribers())
    {
      int aRefKindsMask = 0;
      for (const BRepGraph_RefId& aRef : aDeferredRefList)
      {
        aRefKindsMask |= BRepGraph_Layer::RefKindBit(aRef.RefKind);
      }
      myGraph->myData->myLayerRegistry.DispatchRefsModified(aDeferredRefList.ToArray1(),
                                                            aRefKindsMask);
    }
    myGraph->myData->myIncStorage.ClearDeferredQueues();
    return;
  }

  // Shape cache uses SubtreeGen validation - no bulk clear needed.
  // Stale entries are detected on read via StoredSubtreeGen != entity.SubtreeGen.

  // Propagate SubtreeGen upward from each directly-modified node.
  // Dense per-kind visited flags for O(1) lookup without hashing overhead.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Dense visited arrays indexed by entity index per kind.
  // NCollection_Array1 with bool values: O(1) checked/set by index.
  static constexpr int     THE_KIND_COUNT = BRepGraph_NodeId::THE_KIND_COUNT;
  NCollection_Array1<bool> aVisArrays[THE_KIND_COUNT];
  {
    const uint32_t aKindCounts[THE_KIND_COUNT] = {
      aStorage.NbSolids(),     // Kind::Solid      = 0
      aStorage.NbShells(),     // Kind::Shell      = 1
      aStorage.NbFaces(),      // Kind::Face       = 2
      aStorage.NbWires(),      // Kind::Wire       = 3
      aStorage.NbEdges(),      // Kind::Edge       = 4
      aStorage.NbVertices(),   // Kind::Vertex     = 5
      aStorage.NbCompounds(),  // Kind::Compound   = 6
      aStorage.NbCompSolids(), // Kind::CompSolid  = 7
      aStorage.NbCoEdges(),    // Kind::CoEdge     = 8
      0u,                      // gap at 9
      aStorage.NbProducts(),   // Kind::Product    = 10
      aStorage.NbOccurrences() // Kind::Occurrence = 11
    };
    for (int aKindIdx = 0; aKindIdx < THE_KIND_COUNT; ++aKindIdx)
    {
      const uint32_t aCount = aKindCounts[aKindIdx];
      if (aCount > 0)
      {
        aVisArrays[aKindIdx].Resize(0, static_cast<int>(aCount) - 1, false);
        aVisArrays[aKindIdx].Init(false);
      }
    }
  }

  // Helper lambda: check-and-set visited flag.
  const auto markVisited = [&aVisArrays](const BRepGraph_NodeId theNode) -> bool {
    const int                 aKindIdx = static_cast<int>(theNode.NodeKind);
    NCollection_Array1<bool>& aArr     = aVisArrays[aKindIdx];
    if (aArr.IsEmpty() || static_cast<int>(theNode.Index) > aArr.Upper())
    {
      return false;
    }
    if (aArr.Value(static_cast<int>(theNode.Index)))
    {
      return false;
    }
    aArr.SetValue(static_cast<int>(theNode.Index), true);
    return true;
  };

  // BFS-style upward propagation: process nodes front-to-back, appending
  // newly discovered parents at the end. The loop index advances through
  // the growing vector, so each node is visited exactly once.
  NCollection_LinearVector<BRepGraph_NodeId> aAllModified;
  aAllModified.Reserve(aDeferredList.Size() * 2);
  int aModifiedKindsMask = 0;

  // Seed with directly modified nodes.
  for (const BRepGraph_NodeId& aNode : aDeferredList)
  {
    if (markVisited(aNode))
    {
      aAllModified.Append(aNode);
      aModifiedKindsMask |= BRepGraph_Layer::KindBit(aNode.NodeKind);
    }
  }

  // Propagate upward level by level.
  // Process the list from front to back; newly appended parents will be
  // reached in subsequent iterations of the same loop.
  for (size_t i = 0; i < aAllModified.Size(); ++i)
  {
    const BRepGraph_NodeId aNodeId = aAllModified.Value(i);

    const auto appendParent = [&](const BRepGraph_NodeId theParentId) {
      if (!markVisited(theParentId))
      {
        return;
      }
      BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(theParentId);
      if (aParent == nullptr || theParentId.IsRemoved(*myGraph))
      {
        return;
      }
      ++aParent->SubtreeGen;
      aAllModified.Append(theParentId);
      aModifiedKindsMask |= BRepGraph_Layer::KindBit(theParentId.NodeKind);
    };

    // Collect parent NodeIds via relation and increment SubtreeGen.
    // NOT OwnGen - parent's own data didn't change.
    // The visited set ensures each parent is incremented exactly once per flush,
    // even in diamond topologies. This matches immediate-mode LastPropWave semantics.
    switch (aNodeId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Vertex:
        // Vertex modifications don't propagate in deferred mode.
        break;
      case BRepGraph_NodeId::Kind::Edge: {
        for (BRepGraph_WiresOfEdge aWireIt =
               myGraph->Topo().Edges().WiresOf(BRepGraph_EdgeId(aNodeId));
             aWireIt.More();
             aWireIt.Next())
        {
          appendParent(aWireIt.CurrentId());
        }
        break;
      }
      case BRepGraph_NodeId::Kind::CoEdge: {
        const BRepGraph_CoEdgeId aCoEdgeId(aNodeId);
        if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
        {
          const BRepGraph_WireId aWireId = aStorage.CoEdge(aCoEdgeId).ParentWireId;
          if (aWireId.IsValid(aStorage.NbWires()))
          {
            appendParent(aWireId);
          }
        }
        break;
      }
      case BRepGraph_NodeId::Kind::Wire: {
        for (const BRepGraph_WireRefId& aRefId :
             aStorage.WireRelations(BRepGraph_WireId(aNodeId)).ParentWireRefIds)
        {
          if (aRefId.IsValid(aStorage.NbWireRefs()) && !aStorage.IsRemoved(aRefId))
          {
            appendParent(aStorage.WireRef(aRefId).ParentFaceId);
          }
        }
        break;
      }
      case BRepGraph_NodeId::Kind::Face: {
        for (const BRepGraph_FaceRefId& aRefId :
             aStorage.FaceRelations(BRepGraph_FaceId(aNodeId)).ParentFaceRefIds)
        {
          if (aRefId.IsValid(aStorage.NbFaceRefs()) && !aStorage.IsRemoved(aRefId))
          {
            appendParent(aStorage.FaceRef(aRefId).ParentShellId);
          }
        }
        break;
      }
      case BRepGraph_NodeId::Kind::Shell: {
        for (const BRepGraph_ShellRefId& aRefId :
             aStorage.ShellRelations(BRepGraph_ShellId(aNodeId)).ParentShellRefIds)
        {
          if (aRefId.IsValid(aStorage.NbShellRefs()) && !aStorage.IsRemoved(aRefId))
          {
            appendParent(aStorage.ShellRef(aRefId).ParentSolidId);
          }
        }
        break;
      }
      case BRepGraph_NodeId::Kind::Occurrence: {
        const BRepGraph_OccurrenceId anOccurrenceId(aNodeId);
        for (BRepGraph_ProductId aProductId = BRepGraph_ProductId::Start();
             aProductId.IsValid(aStorage.NbProducts());
             ++aProductId)
        {
          if (aStorage.IsRemoved(aProductId))
          {
            continue;
          }
          for (const BRepGraph_OccurrenceRefId& aRefId :
               aStorage.ProductRelations(aProductId).OccurrenceRefIds)
          {
            if (aRefId.IsValid(aStorage.NbOccurrenceRefs()) && !aStorage.IsRemoved(aRefId)
                && aStorage.OccurrenceRef(aRefId).ChildOccurrenceId == anOccurrenceId)
            {
              appendParent(aProductId);
              break;
            }
          }
        }
        break;
      }
      default: {
        // Solid/Compound/CompSolid/Product: propagate to parent occurrences
        // that reference this node as ChildNodeId - matching immediate-mode
        // propagateSubtreeGen logic.
        if (BRepGraph_NodeId::IsTopologyKind(aNodeId.NodeKind)
            || aNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
        {
          for (const BRepGraph_OccurrenceRefId& aRefId : aStorage.OccurrenceRefsOfNode(aNodeId))
          {
            if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()) || aStorage.IsRemoved(aRefId))
            {
              continue;
            }
            const BRepGraph_OccurrenceId anOccurrenceId =
              aStorage.OccurrenceRef(aRefId).ChildOccurrenceId;
            if (!anOccurrenceId.IsValid(aStorage.NbOccurrences())
                || aStorage.IsRemoved(anOccurrenceId))
            {
              continue;
            }
            appendParent(anOccurrenceId);
          }
        }
        break;
      }
    }
  }

  // Dispatch batch modification event to subscribing layers.
  if (myGraph->myData->myLayerRegistry.HasModificationSubscribers() && !aAllModified.IsEmpty())
  {
    myGraph->myData->myLayerRegistry.DispatchNodesModified(aAllModified.ToArray1(),
                                                           aModifiedKindsMask);
  }

  // Dispatch deferred reference modification events to subscribing layers.
  NCollection_LinearVector<BRepGraph_RefId>& aDeferredRefList =
    myGraph->myData->myIncStorage.ChangeDeferredRefModified();
  if (!aDeferredRefList.IsEmpty()
      && myGraph->myData->myLayerRegistry.HasRefModificationSubscribers())
  {
    int aRefKindsMask = 0;
    for (const BRepGraph_RefId& aRef : aDeferredRefList)
    {
      aRefKindsMask |= BRepGraph_Layer::RefKindBit(aRef.RefKind);
    }
    myGraph->myData->myLayerRegistry.DispatchRefsModified(aDeferredRefList.ToArray1(),
                                                          aRefKindsMask);
  }
  myGraph->myData->myIncStorage.ClearDeferredQueues();
}

//=================================================================================================

bool BRepGraph::EditorView::IsDeferredMode() const
{
  return myGraph->myData->myIncStorage.DeferredMode();
}

//=================================================================================================

bool BRepGraph::EditorView::isOwned(const BRepGraph_ItemId theItem) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node: {
      const BRepGraph_NodeId aNode = theItem.NodeId();
      if (!isNodeIndexInRange(aStorage, aNode))
      {
        return false;
      }
      switch (aNode.NodeKind)
      {
        case BRepGraph_NodeId::Kind::Vertex:
          return aStorage.IsOwned(BRepGraph_VertexId(aNode));
        case BRepGraph_NodeId::Kind::Edge:
          return aStorage.IsOwned(BRepGraph_EdgeId(aNode));
        case BRepGraph_NodeId::Kind::CoEdge:
          return aStorage.IsOwned(BRepGraph_CoEdgeId(aNode));
        case BRepGraph_NodeId::Kind::Wire:
          return aStorage.IsOwned(BRepGraph_WireId(aNode));
        case BRepGraph_NodeId::Kind::Face:
          return aStorage.IsOwned(BRepGraph_FaceId(aNode));
        case BRepGraph_NodeId::Kind::Shell:
          return aStorage.IsOwned(BRepGraph_ShellId(aNode));
        case BRepGraph_NodeId::Kind::Solid:
          return aStorage.IsOwned(BRepGraph_SolidId(aNode));
        case BRepGraph_NodeId::Kind::Compound:
          return aStorage.IsOwned(BRepGraph_CompoundId(aNode));
        case BRepGraph_NodeId::Kind::CompSolid:
          return aStorage.IsOwned(BRepGraph_CompSolidId(aNode));
        case BRepGraph_NodeId::Kind::Product:
          return aStorage.IsOwned(BRepGraph_ProductId(aNode));
        case BRepGraph_NodeId::Kind::Occurrence:
          return aStorage.IsOwned(BRepGraph_OccurrenceId(aNode));
      }
      return false;
    }
    case BRepGraph_ItemId::Domain::Reference: {
      const BRepGraph_RefId aRef = theItem.RefId();
      if (!isRefInRange(aStorage, aRef))
      {
        return false;
      }
      switch (aRef.RefKind)
      {
        case BRepGraph_RefId::Kind::Vertex:
          return aStorage.IsOwned(BRepGraph_VertexRefId(aRef));
        case BRepGraph_RefId::Kind::Wire:
          return aStorage.IsOwned(BRepGraph_WireRefId(aRef));
        case BRepGraph_RefId::Kind::Face:
          return aStorage.IsOwned(BRepGraph_FaceRefId(aRef));
        case BRepGraph_RefId::Kind::Shell:
          return aStorage.IsOwned(BRepGraph_ShellRefId(aRef));
        case BRepGraph_RefId::Kind::Solid:
          return aStorage.IsOwned(BRepGraph_SolidRefId(aRef));
        case BRepGraph_RefId::Kind::Child:
          return aStorage.IsOwned(BRepGraph_ChildRefId(aRef));
        case BRepGraph_RefId::Kind::Occurrence:
          return aStorage.IsOwned(BRepGraph_OccurrenceRefId(aRef));
      }
      return false;
    }
    case BRepGraph_ItemId::Domain::None:
      return false;
  }
  return false;
}

//=================================================================================================

void BRepGraph::EditorView::requireUnlocked(const BRepGraph_ItemId theItem,
                                            const char*            theOperation) const
{
  if (isOwned(theItem))
  {
    throw Standard_ProgramError(theOperation);
  }
}

//=================================================================================================

void BRepGraph::EditorView::requireNoActiveGuard(const BRepGraph_ItemId theItem,
                                                 const char*            theOperation) const
{
  if (myGraph->myData->myIncStorage.IsGuarded(theItem))
  {
    throw Standard_ProgramError(theOperation);
  }
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::applyModificationImpl(
  const BRepGraph_NodeId                       theTarget,
  NCollection_LinearVector<BRepGraph_NodeId>&& theReplacements,
  const TCollection_AsciiString&               theOpLabel)
{
  myGraph->LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(theOpLabel,
                                                                    theTarget,
                                                                    theReplacements.ToArray1());
  myGraph->invalidateSubgraphImpl(theTarget);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::Split(const BRepGraph_EdgeId   theEdgeEntity,
                                           const BRepGraph_VertexId theSplitVertex,
                                           const double             theSplitParam,
                                           BRepGraph_EdgeId&        theSubA,
                                           BRepGraph_EdgeId&        theSubB)
{
  theSubA = BRepGraph_EdgeId();
  theSubB = BRepGraph_EdgeId();
  myGraph->Editor().requireUnlocked(theEdgeEntity, "BRepGraph::EditorView::Split(): owned edge");
  myGraph->Editor().requireNoActiveGuard(BRepGraph_ItemId(theEdgeEntity),
                                         "BRepGraph::EditorView::Split(): guard active on edge");
  Standard_ASSERT_RETURN(theEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "Split: edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theSplitVertex.IsValid(myGraph->myData->myIncStorage.NbVertices()),
                         "Split: split-vertex index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(!myGraph->myData->myIncStorage.IsRemoved(theSplitVertex),
                         "Split: split-vertex is removed",
                         Standard_VOID_RETURN);

  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraphInc::EdgeDef& anOrig = myGraph->myData->myIncStorage.Edge(theEdgeEntity);
  Standard_ASSERT_RETURN(!myGraph->myData->myIncStorage.IsRemoved(theEdgeEntity),
                         "Split: source edge is removed",
                         Standard_VOID_RETURN);

  // Read param range from use record if available.
  const BRepGraphInc_Storage& aConstStorage2  = myGraph->myData->myIncStorage;
  double                      aOrigParamFirst = 0.0, aOrigParamLast = 0.0;
  if (anOrig.Curve3DRepId.IsValid(aConstStorage2.NbEdgeCurves3D())
      && !aConstStorage2.IsRemoved(anOrig.Curve3DRepId))
  {
    const BRepGraphInc::EdgeCurve3DRep& aUse = aConstStorage2.EdgeCurve3DRep(anOrig.Curve3DRepId);
    aOrigParamFirst                          = aUse.ParamFirst;
    aOrigParamLast                           = aUse.ParamLast;
  }
  Standard_ASSERT_RETURN(aOrigParamFirst < theSplitParam && theSplitParam < aOrigParamLast,
                         "Split: split parameter must be inside open edge range",
                         Standard_VOID_RETURN);

  const BRepGraphInc_Storage&        aConstStorage         = aConstStorage2;
  const BRepGraph_EdgeCurve3DRepId   aOrigCurve3DRepId     = anOrig.Curve3DRepId;
  const BRepGraph_EdgePolygon3DRepId aOrigPolygon3DRepId   = anOrig.Polygon3DRepId;
  const double                       aOrigTolerance        = anOrig.Tolerance;
  const BRepGraph_VertexRefId        aOrigStartVertexRefId = anOrig.StartVertexRefId;
  const BRepGraph_VertexRefId        aOrigEndVertexRefId   = anOrig.EndVertexRefId;

  // Resolve original vertex def ids through storage ref entries.
  const BRepGraph_VertexId aOrigStartChildVertexId =
    aOrigStartVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigStartVertexRefId).ChildVertexId
                                    : BRepGraph_VertexId();
  const BRepGraph_VertexId aOrigEndChildVertexId =
    aOrigEndVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigEndVertexRefId).ChildVertexId
                                  : BRepGraph_VertexId();

  // Copy wire indices: relation table may be rebuilt below.
  NCollection_LinearVector<BRepGraph_WireId> aOrigWires;
  for (BRepGraph_WiresOfEdge aWireIt = myGraph->Topo().Edges().WiresOf(theEdgeEntity);
       aWireIt.More();
       aWireIt.Next())
  {
    aOrigWires.Append(aWireIt.CurrentId());
  }

  BRepGraphInc_Storage& aMutStorage = myGraph->myData->myIncStorage;

  const auto aCopyEdgeCurveUse = [&](const BRepGraph_EdgeId theParentEdge,
                                     const double           theFirst,
                                     const double           theLast) -> BRepGraph_EdgeCurve3DRepId {
    if (!aOrigCurve3DRepId.IsValid(aMutStorage.NbEdgeCurves3D())
        || aMutStorage.IsRemoved(aOrigCurve3DRepId))
    {
      return BRepGraph_EdgeCurve3DRepId();
    }
    const BRepGraphInc::EdgeCurve3DRep& anOrigUse = aMutStorage.EdgeCurve3DRep(aOrigCurve3DRepId);
    const BRepGraph_EdgeCurve3DRepId    aRepId    = aMutStorage.AppendEdgeCurve3DRep();
    BRepGraphInc::EdgeCurve3DRep&       aUse      = aMutStorage.ChangeEdgeCurve3DRep(aRepId);
    aUse.ParentEdgeId                             = theParentEdge;
    aUse.Curve                                    = anOrigUse.Curve;
    aUse.ParamFirst                               = theFirst;
    aUse.ParamLast                                = theLast;
    return aRepId;
  };

  const auto aCopyEdgePolygon3DRep =
    [&](const BRepGraph_EdgeId theParentEdge) -> BRepGraph_EdgePolygon3DRepId {
    if (!aOrigPolygon3DRepId.IsValid(aMutStorage.NbEdgePolygons3D())
        || aMutStorage.IsRemoved(aOrigPolygon3DRepId))
    {
      return BRepGraph_EdgePolygon3DRepId();
    }
    const BRepGraphInc::EdgePolygon3DRep& anOrigUse =
      aMutStorage.EdgePolygon3DRep(aOrigPolygon3DRepId);
    const BRepGraph_EdgePolygon3DRepId aRepId = aMutStorage.AppendEdgePolygon3DRep();
    BRepGraphInc::EdgePolygon3DRep&    aUse   = aMutStorage.ChangeEdgePolygon3DRep(aRepId);
    aUse.ParentEdgeId                         = theParentEdge;
    aUse.Polygon                              = anOrigUse.Polygon;
    return aRepId;
  };

  // Allocate SubA slot.
  const BRepGraph_EdgeId aSubAId(aMutStorage.NbEdges());
  aMutStorage.AppendEdge();
  theSubA = aSubAId;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference - use index).
  const BRepGraph_EdgeId aSubBId(aMutStorage.NbEdges());
  aMutStorage.AppendEdge();
  theSubB = aSubBId;

  // Build vertex ref entries for the split vertex (no Location since split vertex is new).
  const BRepGraph_VertexId aSplitChildVertexId = theSplitVertex;

  // Create start vertex ref entry for SubA (copy from original edge's start vertex ref).
  BRepGraph_VertexRefId aSubAStartRefId;
  if (aOrigStartVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRef& aOrigStartRef =
      myGraph->myData->myIncStorage.VertexRef(aOrigStartVertexRefId);
    const BRepGraph_VertexId aOrigStartVertexId = aOrigStartRef.ChildVertexId;
    const TopAbs_Orientation aOrigStartOri      = aOrigStartRef.Orientation;

    const BRepGraph_VertexRefId aSubAStartRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubAStartRef = aMutStorage.ChangeVertexRef(aSubAStartRefId2);
    aSubAStartRef.ChildVertexId            = aOrigStartVertexId;
    aSubAStartRef.ParentEdgeId             = aSubAId;
    aSubAStartRef.Orientation              = aOrigStartOri;
    myGraph->allocateRefUID(aSubAStartRefId2);
    aSubAStartRefId = aSubAStartRefId2;
  }

  // Create end vertex ref entry for SubA (split vertex, REVERSED).
  BRepGraph_VertexRefId aSubAEndRefId;
  if (aSplitChildVertexId.IsValid())
  {
    const BRepGraph_VertexRefId aSubAEndRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubAEndRef = aMutStorage.ChangeVertexRef(aSubAEndRefId2);
    aSubAEndRef.ChildVertexId            = aSplitChildVertexId;
    aSubAEndRef.ParentEdgeId             = aSubAId;
    aSubAEndRef.Orientation              = TopAbs_REVERSED;
    myGraph->allocateRefUID(aSubAEndRefId2);
    aSubAEndRefId = aSubAEndRefId2;
  }

  // Create start vertex ref entry for SubB (split vertex, FORWARD).
  BRepGraph_VertexRefId aSubBStartRefId;
  if (aSplitChildVertexId.IsValid())
  {
    const BRepGraph_VertexRefId aSubBStartRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubBStartRef = aMutStorage.ChangeVertexRef(aSubBStartRefId2);
    aSubBStartRef.ChildVertexId            = aSplitChildVertexId;
    aSubBStartRef.ParentEdgeId             = aSubBId;
    aSubBStartRef.Orientation              = TopAbs_FORWARD;
    myGraph->allocateRefUID(aSubBStartRefId2);
    aSubBStartRefId = aSubBStartRefId2;
  }

  // Create end vertex ref entry for SubB (copy from original edge's end vertex ref).
  BRepGraph_VertexRefId aSubBEndRefId;
  if (aOrigEndVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRef& aOrigEndRef =
      myGraph->myData->myIncStorage.VertexRef(aOrigEndVertexRefId);
    const BRepGraph_VertexId aOrigEndVertexId = aOrigEndRef.ChildVertexId;
    const TopAbs_Orientation aOrigEndOri      = aOrigEndRef.Orientation;

    const BRepGraph_VertexRefId aSubBEndRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubBEndRef = aMutStorage.ChangeVertexRef(aSubBEndRefId2);
    aSubBEndRef.ChildVertexId            = aOrigEndVertexId;
    aSubBEndRef.ParentEdgeId             = aSubBId;
    aSubBEndRef.Orientation              = aOrigEndOri;
    myGraph->allocateRefUID(aSubBEndRefId2);
    aSubBEndRefId = aSubBEndRefId2;
  }

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  const BRepGraph_EdgeCurve3DRepId aSubACurve3DRepId =
    aCopyEdgeCurveUse(aSubAId, aOrigParamFirst, theSplitParam);
  const BRepGraph_EdgePolygon3DRepId aSubAPolygon3DRepId = aCopyEdgePolygon3DRep(aSubAId);
  {
    BRepGraphInc::EdgeDef& aSubA = myGraph->myData->myIncStorage.ChangeEdge(aSubAId);
    initSubEdgeEntity(aSubA, aSubACurve3DRepId, aOrigTolerance, aSubAStartRefId, aSubAEndRefId);
    aSubA.Polygon3DRepId = aSubAPolygon3DRepId;
  }
  const auto anAttachEdgeVertices = [&](const BRepGraph_EdgeId      theEdgeId,
                                        const BRepGraph_VertexRefId theStartRefId,
                                        const BRepGraph_VertexRefId theEndRefId) {
    if (theStartRefId.IsValid(aMutStorage.NbVertexRefs()))
    {
      aMutStorage.AttachEdgeToVertex(theEdgeId, aMutStorage.VertexRef(theStartRefId).ChildVertexId);
    }
    if (theEndRefId.IsValid(aMutStorage.NbVertexRefs()))
    {
      aMutStorage.AttachEdgeToVertex(theEdgeId, aMutStorage.VertexRef(theEndRefId).ChildVertexId);
    }
  };
  anAttachEdgeVertices(aSubAId, aSubAStartRefId, aSubAEndRefId);

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  const BRepGraph_EdgeCurve3DRepId aSubBCurve3DRepId =
    aCopyEdgeCurveUse(aSubBId, theSplitParam, aOrigParamLast);
  const BRepGraph_EdgePolygon3DRepId aSubBPolygon3DRepId = aCopyEdgePolygon3DRep(aSubBId);
  {
    BRepGraphInc::EdgeDef& aSubB = myGraph->myData->myIncStorage.ChangeEdge(aSubBId);
    initSubEdgeEntity(aSubB, aSubBCurve3DRepId, aOrigTolerance, aSubBStartRefId, aSubBEndRefId);
    aSubB.Polygon3DRepId = aSubBPolygon3DRepId;
  }
  anAttachEdgeVertices(aSubBId, aSubBStartRefId, aSubBEndRefId);

  myGraph->allocateUID(theSubA);
  myGraph->allocateUID(theSubB);

  // Rebuild CoEdge incidence in a single pass: snapshot all CoEdges of the
  // original edge, allocate two fully-initialised CoEdges per original, rebuild
  // wire CoEdgeIds, retire each original via relation unbind. The seam
  // relation re-emerges automatically from the resulting (Edge, Face, Orientation)
  // tuples (see BRepGraph_Tool::CoEdge::SeamPair).
  {
    BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

    // Step 1: snapshot old CoEdge ids before any mutation.
    NCollection_LinearVector<BRepGraph_CoEdgeId> anOrigCoEdgeIds;
    if (theEdgeEntity.IsValid(aStorage.NbEdges()))
    {
      for (const BRepGraph_CoEdgeId& anId : aStorage.EdgeRelations(theEdgeEntity).CoEdgeIds)
      {
        anOrigCoEdgeIds.Append(anId);
      }
    }
    const uint32_t aNbOrig = static_cast<uint32_t>(anOrigCoEdgeIds.Size());

    // Step 2: allocate SubA-CE + SubB-CE per original with full representation.
    NCollection_FlatDataMap<BRepGraph_CoEdgeId, uint32_t> aIdxOf;
    NCollection_LinearVector<BRepGraph_CoEdgeId> aNewACoEdgeIds(aNbOrig, BRepGraph_CoEdgeId());
    NCollection_LinearVector<BRepGraph_CoEdgeId> aNewBCoEdgeIds(aNbOrig, BRepGraph_CoEdgeId());
    const double                                 aSplitRatio =
      (theSplitParam - aOrigParamFirst) / (aOrigParamLast - aOrigParamFirst);
    const auto aCopyCoEdgeCurveUse =
      [&](const BRepGraph_CoEdgeCurve2DRepId theOrigRepId,
          const BRepGraph_CoEdgeId           theParentCoEdge,
          const bool                         theIsFirstPart) -> BRepGraph_CoEdgeCurve2DRepId {
      if (!theOrigRepId.IsValid(aStorage.NbCoEdgeCurves2D()) || aStorage.IsRemoved(theOrigRepId))
      {
        return BRepGraph_CoEdgeCurve2DRepId();
      }
      const BRepGraphInc::CoEdgeCurve2DRep& anOrigUse = aStorage.CoEdgeCurve2DRep(theOrigRepId);
      const double                          aParamSplit =
        anOrigUse.ParamFirst + (anOrigUse.ParamLast - anOrigUse.ParamFirst) * aSplitRatio;
      const BRepGraph_CoEdgeCurve2DRepId aRepId = aStorage.AppendCoEdgeCurve2DRep();
      BRepGraphInc::CoEdgeCurve2DRep&    aUse   = aStorage.ChangeCoEdgeCurve2DRep(aRepId);
      aUse.ParentCoEdgeId                       = theParentCoEdge;
      aUse.Curve                                = anOrigUse.Curve;
      aUse.ParamFirst = theIsFirstPart ? anOrigUse.ParamFirst : aParamSplit;
      aUse.ParamLast  = theIsFirstPart ? aParamSplit : anOrigUse.ParamLast;
      return aRepId;
    };
    const auto aCopyCoEdgePolygon2DRep =
      [&](const BRepGraph_CoEdgePolygon2DRepId theOrigRepId,
          const BRepGraph_CoEdgeId             theParentCoEdge) -> BRepGraph_CoEdgePolygon2DRepId {
      if (!theOrigRepId.IsValid(aStorage.NbCoEdgePolygons2D()) || aStorage.IsRemoved(theOrigRepId))
      {
        return BRepGraph_CoEdgePolygon2DRepId();
      }
      const BRepGraphInc::CoEdgePolygon2DRep& anOrigUse = aStorage.CoEdgePolygon2DRep(theOrigRepId);
      const BRepGraph_CoEdgePolygon2DRepId    aRepId    = aStorage.AppendCoEdgePolygon2DRep();
      BRepGraphInc::CoEdgePolygon2DRep&       aUse      = aStorage.ChangeCoEdgePolygon2DRep(aRepId);
      aUse.ParentCoEdgeId                               = theParentCoEdge;
      aUse.Polygon                                      = anOrigUse.Polygon;
      return aRepId;
    };
    const auto aCopyCoEdgePolygonOnTriRep =
      [&](const BRepGraph_CoEdgePolygonOnTriRepId theOrigRepId,
          const BRepGraph_CoEdgeId theParentCoEdge) -> BRepGraph_CoEdgePolygonOnTriRepId {
      if (!theOrigRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
          || aStorage.IsRemoved(theOrigRepId))
      {
        return BRepGraph_CoEdgePolygonOnTriRepId();
      }
      const BRepGraphInc::CoEdgePolygonOnTriRep& anOrigUse =
        aStorage.CoEdgePolygonOnTriRep(theOrigRepId);
      const BRepGraph_CoEdgePolygonOnTriRepId aRepId = aStorage.AppendCoEdgePolygonOnTriRep();
      BRepGraphInc::CoEdgePolygonOnTriRep&    aUse   = aStorage.ChangeCoEdgePolygonOnTriRep(aRepId);
      aUse.ParentCoEdgeId                            = theParentCoEdge;
      aUse.Polygon                                   = anOrigUse.Polygon;
      return aRepId;
    };

    for (uint32_t i = 0; i < aNbOrig; ++i)
    {
      const BRepGraph_CoEdgeId      aOldId = anOrigCoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraphInc::CoEdgeDef aOld   = aStorage.CoEdge(aOldId); // by-value snapshot

      const BRepGraph_CoEdgeId aNewA = aStorage.AppendCoEdge();
      {
        BRepGraphInc::CoEdgeDef& aNewACE = aStorage.ChangeCoEdge(aNewA);
        initSubCoEdgeEntity(aNewACE,
                            theSubA,
                            aOld.FaceId,
                            aOld.Orientation,
                            aCopyCoEdgeCurveUse(aOld.Curve2DRepId, aNewA, true));
        aNewACE.Polygon2DRepId    = aCopyCoEdgePolygon2DRep(aOld.Polygon2DRepId, aNewA);
        aNewACE.PolygonOnTriRepId = aCopyCoEdgePolygonOnTriRep(aOld.PolygonOnTriRepId, aNewA);
      }
      myGraph->allocateUID(aNewA);

      const BRepGraph_CoEdgeId aNewB = aStorage.AppendCoEdge();
      {
        BRepGraphInc::CoEdgeDef& aNewBCE = aStorage.ChangeCoEdge(aNewB);
        initSubCoEdgeEntity(aNewBCE,
                            theSubB,
                            aOld.FaceId,
                            aOld.Orientation,
                            aCopyCoEdgeCurveUse(aOld.Curve2DRepId, aNewB, false));
        aNewBCE.Polygon2DRepId    = aCopyCoEdgePolygon2DRep(aOld.Polygon2DRepId, aNewB);
        aNewBCE.PolygonOnTriRepId = aCopyCoEdgePolygonOnTriRep(aOld.PolygonOnTriRepId, aNewB);
      }
      myGraph->allocateUID(aNewB);

      aNewACoEdgeIds.ChangeValue(static_cast<size_t>(i)) = aNewA;
      aNewBCoEdgeIds.ChangeValue(static_cast<size_t>(i)) = aNewB;
      aIdxOf.Bind(aOldId, i);
    }

    // Step 3: rebuild wire CoEdgeIds (snapshot-before-mutate). For each
    // wire containing the original edge, replace the existing coedge entry
    // with the new SubA-CE in place and insert a fresh SubB-CE immediately
    // after. Track insertion offset so that subsequent inserts land at the
    // correct position in the growing list.
    for (const BRepGraph_WireId& aWireId : aOrigWires)
    {
      NCollection_LinearVector<BRepGraph_CoEdgeId> aSnapshot;
      {
        const BRepGraphInc::WireRelations& aWireRel = aStorage.WireRelations(aWireId);
        for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
        {
          aSnapshot.Append(aCoEdgeId);
        }
      }

      for (size_t i = 0; i < aSnapshot.Size(); ++i)
      {
        const BRepGraph_CoEdgeId aOldCEId = aSnapshot.Value(i);
        if (aStorage.IsRemoved(aOldCEId))
        {
          continue;
        }
        const uint32_t* aJPtr = aIdxOf.Seek(aOldCEId);
        if (aJPtr == nullptr)
        {
          continue;
        }
        const uint32_t           aJ    = *aJPtr;
        const BRepGraph_CoEdgeId aNewA = aNewACoEdgeIds.Value(static_cast<size_t>(aJ));
        const BRepGraph_CoEdgeId aNewB = aNewBCoEdgeIds.Value(static_cast<size_t>(aJ));
        aStorage.ReplaceCoEdgeUseWithPair(aWireId, aOldCEId, aNewA, aNewB);
      }
      aStorage.CanonicalizeWireCoEdgeOrder(aWireId);
    }

    // Step 4: retire the original CoEdges and rebuild relations so
    // EdgeRelations(theSubA/B).CoEdgeIds resolves to the new pair.
    for (uint32_t i = 0; i < aNbOrig; ++i)
    {
      const BRepGraph_CoEdgeId       aOldId = anOrigCoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraphInc::CoEdgeDef& aOld   = aStorage.CoEdge(aOldId);
      aStorage.MarkRemoved(aOld.Curve2DRepId);
      aStorage.MarkRemoved(aOld.Polygon2DRepId);
      aStorage.MarkRemoved(aOld.PolygonOnTriRepId);
      aStorage.MarkRemoved(BRepGraph_NodeId(aOldId));
    }

    // Step 5: retire the original edge's boundary vertex refs before the
    // original edge is removed; otherwise active refs would have no live edge
    // slot owner. Internal vertex refs are dropped rather than reattached to
    // SubA/SubB based on parameter - reattachment is a follow-up if a caller
    // surfaces that need.
    if (aOrigStartVertexRefId.IsValid())
    {
      aStorage.MarkRemovedRef(aOrigStartVertexRefId);
    }
    if (aOrigEndVertexRefId.IsValid())
    {
      aStorage.MarkRemovedRef(aOrigEndVertexRefId);
    }

    // Mark original edge as removed.
    aStorage.MarkRemoved(aOrigCurve3DRepId);
    aStorage.MarkRemoved(aOrigPolygon3DRepId);
    aStorage.MarkRemoved(theEdgeEntity);
  }

  for (const BRepGraph_WireId& aWireId : aOrigWires)
  {
    myGraph->markModified(aWireId);
  }

  myGraph->myData->myIncStorage.RebindVertexEdge(aOrigStartChildVertexId,
                                                 BRepGraph_VertexId(),
                                                 theEdgeEntity,
                                                 BRepGraph_VertexRefId());
  myGraph->myData->myIncStorage.RebindVertexEdge(aOrigEndChildVertexId,
                                                 BRepGraph_VertexId(),
                                                 theEdgeEntity,
                                                 BRepGraph_VertexRefId());

  myGraph->markModified(theEdgeEntity);
  myGraph->markModified(theSubA);
  myGraph->markModified(theSubB);

  Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                       "Split: post-mutation relation inconsistency");
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::ReplaceEdge(const BRepGraph_WireId theChildWireId,
                                                 const BRepGraph_EdgeId theOldEdgeEntity,
                                                 const BRepGraph_EdgeId theNewEdgeEntity,
                                                 const bool             theReversed)
{
  Standard_ASSERT_RETURN(theChildWireId.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "ReplaceEdge: wire index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theOldEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceEdge: old edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theNewEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceEdge: new edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(!myGraph->myData->myIncStorage.IsRemoved(theNewEdgeEntity),
                         "ReplaceEdge: replacement edge must be active",
                         Standard_VOID_RETURN);

  if (!isActiveNode(myGraph->myData->myIncStorage, theChildWireId)
      || !isActiveNode(myGraph->myData->myIncStorage, theOldEdgeEntity))
  {
    return;
  }

  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  const ReplaceEdgeStatus aStatus =
    CheckReplaceEdge(theChildWireId, theOldEdgeEntity, theNewEdgeEntity, theReversed);
  if (aStatus == ReplaceEdgeStatus::AlreadyCurrent)
  {
    return;
  }
  if (aStatus != ReplaceEdgeStatus::Ready)
  {
    return;
  }

  // Update incidence by scanning wire-owned coedge entries.
  for (BRepGraph_CoEdgesOfWire aRefIt(*myGraph, theChildWireId); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraph_CoEdgeId aCoChildEdgeId = aRefIt.CurrentId();
    if (!aCoChildEdgeId.IsValid(aStorage.NbCoEdges()))
    {
      continue;
    }

    if (aStorage.IsRemoved(aCoChildEdgeId))
    {
      continue;
    }

    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoChildEdgeId);
    if (aCoEdge.ChildEdgeId == theOldEdgeEntity)
    {
      aCoEdge.ChildEdgeId = theNewEdgeEntity;
      if (theReversed)
      {
        aCoEdge.Orientation = TopAbs::Reverse(aCoEdge.Orientation);

        // Reverse the PCurve to match the canonical edge's parametric direction.
        // When the canonical edge has opposite vertex ordering (isReversed=true),
        // the PCurve was defined for the old edge's direction and must be reversed
        // so it maps parameter-space coordinates relative to the canonical edge.
        if (aCoEdge.Curve2DRepId.IsValid()
            && aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D()))
        {
          BRepGraphInc::CoEdgeCurve2DRep& aCurveRep =
            aStorage.ChangeCoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
          if (!aCurveRep.Curve.IsNull())
          {
            const double anOldParamFirst = aCurveRep.ParamFirst;
            const double anOldParamLast  = aCurveRep.ParamLast;
            aCurveRep.ParamFirst         = aCurveRep.Curve->ReversedParameter(anOldParamLast);
            aCurveRep.ParamLast          = aCurveRep.Curve->ReversedParameter(anOldParamFirst);
            aCurveRep.Curve              = aCurveRep.Curve->Reversed();
          }
        }
      }

      myGraph->myData->myIncStorage.RebindCoEdgeEdge(aCoChildEdgeId,
                                                     theOldEdgeEntity,
                                                     theNewEdgeEntity);
    }
  }

  myGraph->markModified(theChildWireId);

  // Validate relation only when not in deferred mode.
  // In deferred mode (batch sewing, parallel mutations), intermediate states
  // may have temporarily stale entries; validation runs at CommitMutation().
  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "ReplaceEdge: post-mutation relation inconsistency");
  }
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::Reverse(const BRepGraph_EdgeId theEdge)
{
  Standard_ASSERT_RETURN(theEdge.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "Reverse: edge index is out of range",
                         Standard_VOID_RETURN);
  if (!isActiveNode(myGraph->myData->myIncStorage, theEdge))
  {
    return;
  }

  BRepGraphInc::EdgeDef& aEdgeDef = myGraph->myData->myIncStorage.ChangeEdge(theEdge);
  std::swap(aEdgeDef.StartVertexRefId, aEdgeDef.EndVertexRefId);
  myGraph->markModified(theEdge);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "Reverse: post-mutation relation inconsistency");
  }
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::Reverse(const BRepGraph_WireId theWire)
{
  Standard_ASSERT_RETURN(theWire.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "Reverse: wire index is out of range",
                         Standard_VOID_RETURN);
  if (!isActiveNode(myGraph->myData->myIncStorage, theWire))
  {
    return;
  }

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  aStorage.ReverseWireCoEdges(theWire);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aRefs =
    aStorage.WireRelations(theWire).CoEdgeIds;
  const size_t aNb = aRefs.Size();
  for (size_t i = 0; i < aNb; ++i)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aRefs.Value(i);
    if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()))
    {
      continue;
    }
    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
    if (aStorage.IsRemoved(aCoEdgeId))
    {
      continue;
    }
    aCoEdge.Orientation = TopAbs::Reverse(aCoEdge.Orientation);
    myGraph->markModified(aCoEdgeId);
  }

  myGraph->markModified(theWire);
  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "Reverse: post-mutation relation inconsistency");
  }
}

//=================================================================================================

bool BRepGraph::EditorView::WireOps::SetCoEdgeOrder(
  const BRepGraph_WireId                        theWire,
  const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds)
{
  Standard_ASSERT_RETURN(theWire.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "SetCoEdgeOrder: wire index is out of range",
                         false);
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theWire))
  {
    return false;
  }

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& anOldCoEdges =
    aStorage.WireRelations(theWire).CoEdgeIds;

  NCollection_LinearVector<BRepGraph_CoEdgeId> aConnectedCoEdges;
  const CoEdgeOrderStatus                      aStatus =
    preCheckCoEdgeOrder(aStorage, theCoEdgeIds, theWire, false, true, aConnectedCoEdges);
  if (aStatus == CoEdgeOrderStatus::AlreadyCurrent)
  {
    return true;
  }
  if (aStatus != CoEdgeOrderStatus::Ready && aStatus != CoEdgeOrderStatus::Reordered)
  {
    return false;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> anOldCoEdgeSnapshot;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : anOldCoEdges)
  {
    anOldCoEdgeSnapshot.Append(aCoEdgeId);
  }

  aStorage.SetWireCoEdges(theWire, aConnectedCoEdges.ToArray1());
  if (!aStorage.ValidateWireCoEdgeOrder(theWire))
  {
    aStorage.SetWireCoEdges(theWire, anOldCoEdgeSnapshot.ToArray1());
    return false;
  }
  myGraph->markModified(theWire);
  if (!aStorage.DeferredMode())
  {
    Standard_ASSERT_RETURN(aStorage.ValidateRelations(),
                           "SetCoEdgeOrder: post-mutation relation inconsistency",
                           false);
  }
  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::CompoundOps::RemoveChild(const BRepGraph_CompoundId theCompoundDefId,
                                                     const BRepGraph_ChildRefId theChildRefId)
{
  Standard_ASSERT_RETURN(theCompoundDefId.IsValid(myGraph->myData->myIncStorage.NbCompounds()),
                         "RemoveChild: compound index is out of range",
                         false);
  Standard_ASSERT_RETURN(theChildRefId.IsValid(myGraph->myData->myIncStorage.NbChildRefs()),
                         "RemoveChild: child ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompoundDefId))
  {
    return false;
  }

  const BRepGraphInc::ChildRef&                         aRef = aStorage.ChildRef(theChildRefId);
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aCompoundRefIdsRO =
    aStorage.CompoundRelations(theCompoundDefId).ChildRefIds;
  if (aStorage.IsRemoved(theChildRefId) || !containsOrderedRef(aCompoundRefIdsRO, theChildRefId)
      || !aRef.ChildNodeId.IsValid() || !isActiveNode(aStorage, aRef.ChildNodeId))
  {
    return false;
  }

  const BRepGraph_NodeId aChildNodeId = aRef.ChildNodeId;
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theChildRefId,
                              aCompoundRefIdsRO,
                              aChildNodeId,
                              true))
  {
    return false;
  }

  myGraph->markModified(theCompoundDefId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveChild: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ProductOps::RemoveOccurrence(
  const BRepGraph_ProductId       theProductDefId,
  const BRepGraph_OccurrenceRefId theOccurrenceRefId)
{
  Standard_ASSERT_RETURN(theProductDefId.IsValid(myGraph->myData->myIncStorage.NbProducts()),
                         "RemoveOccurrence: product index is out of range",
                         false);
  Standard_ASSERT_RETURN(
    theOccurrenceRefId.IsValid(myGraph->myData->myIncStorage.NbOccurrenceRefs()),
    "RemoveOccurrence: occurrence ref index is out of range",
    false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theProductDefId))
  {
    return false;
  }

  const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(theOccurrenceRefId);
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aProductRefIdsRO =
    aStorage.ProductRelations(theProductDefId).OccurrenceRefIds;
  if (aStorage.IsRemoved(theOccurrenceRefId)
      || !containsOrderedRef(aProductRefIdsRO, theOccurrenceRefId)
      || !aRef.ChildOccurrenceId.IsValid() || !isActiveNode(aStorage, aRef.ChildOccurrenceId))
  {
    return false;
  }

  const BRepGraph_OccurrenceId anOccDefId = aRef.ChildOccurrenceId;
  // Capture child def id before detach (myProductToOccurrences is keyed by ChildNodeId).
  const BRepGraph_NodeId aChildNodeIdSnap = aStorage.Occurrence(anOccDefId).ChildNodeId;
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theOccurrenceRefId,
                              aProductRefIdsRO,
                              BRepGraph_NodeId(anOccDefId),
                              true))
  {
    return false;
  }

  if (!productHasOccurrenceDef(aStorage, theProductDefId, anOccDefId))
  {
  }

  if (!isOccurrenceDefOwnedByAnyProduct(aStorage, anOccDefId) && aChildNodeIdSnap.IsValid())
  {
  }

  // Check if the removed occurrence's child product was its last parent;
  // if so, re-add the child product to root products.
  const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(anOccDefId);
  if (anOccDef.ChildNodeId.IsValid()
      && anOccDef.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    const BRepGraph_ProductId aChildProduct = BRepGraph_ProductId::FromNodeId(anOccDef.ChildNodeId);
    if (aChildProduct.IsValid(aStorage.NbProducts()) && !aStorage.IsRemoved(aChildProduct)
        && !hasAnyActiveUsage(*myGraph, anOccDef.ChildNodeId))
    {
      myGraph->myData->myIncStorage.ChangeRootProductIds().Append(aChildProduct);
    }
  }

  myGraph->markModified(theProductDefId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveOccurrence: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ProductOps::RemoveShapeRoot(const BRepGraph_ProductId theProductDefId)
{
  Standard_ASSERT_RETURN(theProductDefId.IsValid(myGraph->myData->myIncStorage.NbProducts()),
                         "RemoveShapeRoot: product index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theProductDefId))
  {
    return false;
  }

  // Find the shape-root occurrence (the one whose ChildNodeId is a topology node).
  BRepGraph_OccurrenceRefId aShapeRootRefId;
  BRepGraph_NodeId          aShapeRoot;
  for (const BRepGraph_OccurrenceRefId& aRefId :
       aStorage.ProductRelations(theProductDefId).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aOccRef = aStorage.OccurrenceRef(aRefId);
    if (aStorage.IsRemoved(aRefId))
    {
      continue;
    }
    const BRepGraphInc::OccurrenceDef& anOcc = aStorage.Occurrence(aOccRef.ChildOccurrenceId);
    if (!aStorage.IsRemoved(aOccRef.ChildOccurrenceId)
        && BRepGraph_NodeId::IsTopologyKind(anOcc.ChildNodeId.NodeKind))
    {
      aShapeRootRefId = aRefId;
      aShapeRoot      = anOcc.ChildNodeId;
      break;
    }
  }

  if (!aShapeRootRefId.IsValid() || !aShapeRoot.IsValid() || !isActiveNode(aStorage, aShapeRoot))
  {
    return false;
  }

  // Detach the shape-root occurrence from the product.
  const BRepGraph_OccurrenceId aShapeRootOccId =
    aStorage.OccurrenceRef(aShapeRootRefId).ChildOccurrenceId;
  detachOrderedParentRef(*myGraph,
                         myGraph->myData->myIncStorage,
                         aShapeRootRefId,
                         aStorage.ProductRelations(theProductDefId).OccurrenceRefIds,
                         BRepGraph_NodeId(aShapeRootOccId),
                         true);

  // No relation unbind: shape-root occurrence's ChildNodeId is topology, not a product.

  if (!hasAnyActiveUsage(*myGraph, aShapeRoot))
  {
    myGraph->Editor().Gen().RemoveSubgraph(aShapeRoot);
  }

  myGraph->markModified(theProductDefId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveShapeRoot: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::CompSolidOps::RemoveSolid(
  const BRepGraph_CompSolidId theCompChildSolidId,
  const BRepGraph_SolidRefId  theSolidRefId)
{
  Standard_ASSERT_RETURN(theCompChildSolidId.IsValid(myGraph->myData->myIncStorage.NbCompSolids()),
                         "RemoveSolid: compsolid index is out of range",
                         false);
  Standard_ASSERT_RETURN(theSolidRefId.IsValid(myGraph->myData->myIncStorage.NbSolidRefs()),
                         "RemoveSolid: solid ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompChildSolidId))
  {
    return false;
  }

  const BRepGraphInc::SolidRef&                         aRef = aStorage.SolidRef(theSolidRefId);
  const NCollection_LinearVector<BRepGraph_SolidRefId>& aCompSolidRefIdsRO =
    aStorage.CompSolidRelations(theCompChildSolidId).SolidRefIds;
  if (aStorage.IsRemoved(theSolidRefId) || !containsOrderedRef(aCompSolidRefIdsRO, theSolidRefId)
      || !aRef.ChildSolidId.IsValid() || !isActiveNode(aStorage, aRef.ChildSolidId))
  {
    return false;
  }

  const BRepGraph_SolidId aChildSolidId = aRef.ChildSolidId;
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theSolidRefId,
                              aCompSolidRefIdsRO,
                              BRepGraph_NodeId(aChildSolidId),
                              true))
  {
    return false;
  }

  myGraph->markModified(theCompChildSolidId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveSolid: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::SolidOps::RemoveShell(const BRepGraph_SolidId    theChildSolidId,
                                                  const BRepGraph_ShellRefId theShellRefId)
{
  Standard_ASSERT_RETURN(theChildSolidId.IsValid(myGraph->myData->myIncStorage.NbSolids()),
                         "RemoveShell: solid index is out of range",
                         false);
  Standard_ASSERT_RETURN(theShellRefId.IsValid(myGraph->myData->myIncStorage.NbShellRefs()),
                         "RemoveShell: shell ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theChildSolidId))
  {
    return false;
  }

  const BRepGraphInc::ShellRef&                         aRef = aStorage.ShellRef(theShellRefId);
  const NCollection_LinearVector<BRepGraph_ShellRefId>& aSolidRefIdsRO =
    aStorage.SolidRelations(theChildSolidId).ShellRefIds;
  if (aStorage.IsRemoved(theShellRefId) || !containsOrderedRef(aSolidRefIdsRO, theShellRefId)
      || !aRef.ChildShellId.IsValid() || !isActiveNode(aStorage, aRef.ChildShellId))
  {
    return false;
  }

  const BRepGraph_ShellId aChildShellId = aRef.ChildShellId;
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theShellRefId,
                              aSolidRefIdsRO,
                              BRepGraph_NodeId(aChildShellId),
                              true))
  {
    return false;
  }

  myGraph->markModified(theChildSolidId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveShell: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ShellOps::RemoveFace(const BRepGraph_ShellId   theChildShellId,
                                                 const BRepGraph_FaceRefId theFaceRefId)
{
  Standard_ASSERT_RETURN(theChildShellId.IsValid(myGraph->myData->myIncStorage.NbShells()),
                         "RemoveFace: shell index is out of range",
                         false);
  Standard_ASSERT_RETURN(theFaceRefId.IsValid(myGraph->myData->myIncStorage.NbFaceRefs()),
                         "RemoveFace: face ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theChildShellId))
  {
    return false;
  }

  const BRepGraphInc::FaceRef&                         aRef = aStorage.FaceRef(theFaceRefId);
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aShellRefIdsRO =
    aStorage.ShellRelations(theChildShellId).FaceRefIds;
  if (aStorage.IsRemoved(theFaceRefId) || !containsOrderedRef(aShellRefIdsRO, theFaceRefId)
      || !aRef.ChildFaceId.IsValid() || !isActiveNode(aStorage, aRef.ChildFaceId))
  {
    return false;
  }

  const BRepGraph_FaceId aFaceId = aRef.ChildFaceId;
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theFaceRefId,
                              aShellRefIdsRO,
                              BRepGraph_NodeId(aFaceId),
                              true))
  {
    return false;
  }

  myGraph->markModified(theChildShellId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveFace: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::FaceOps::RemoveWire(const BRepGraph_FaceId    theFaceId,
                                                const BRepGraph_WireRefId theWireRefId)
{
  Standard_ASSERT_RETURN(theFaceId.IsValid(myGraph->myData->myIncStorage.NbFaces()),
                         "RemoveWire: face index is out of range",
                         false);
  Standard_ASSERT_RETURN(theWireRefId.IsValid(myGraph->myData->myIncStorage.NbWireRefs()),
                         "RemoveWire: wire ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theFaceId))
  {
    return false;
  }

  const BRepGraphInc::WireRef&                         aRef = aStorage.WireRef(theWireRefId);
  const NCollection_LinearVector<BRepGraph_WireRefId>& aFaceRefIdsRO =
    aStorage.FaceRelations(theFaceId).WireRefIds;
  if (aStorage.IsRemoved(theWireRefId) || !containsOrderedRef(aFaceRefIdsRO, theWireRefId)
      || !aRef.ChildWireId.IsValid() || !isActiveNode(aStorage, aRef.ChildWireId))
  {
    return false;
  }

  const BRepGraph_WireId                       aChildWireId = aRef.ChildWireId;
  NCollection_LinearVector<BRepGraph_EdgeId>   anAffectedEdges;
  NCollection_LinearVector<BRepGraph_FaceId>   anAffectedFaces;
  NCollection_LinearVector<BRepGraph_CoEdgeId> anAffectedCoEdges;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aStorage.WireRelations(aChildWireId).CoEdgeIds)
  {
    if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(aCoEdgeId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.FaceId == theFaceId)
    {
      anAffectedCoEdges.Append(aCoEdgeId);
    }
    if (aCoEdge.ChildEdgeId.IsValid(aStorage.NbEdges()))
    {
      appendUniqueRelationId(anAffectedEdges, aCoEdge.ChildEdgeId);
    }
    if (aCoEdge.FaceId.IsValid(aStorage.NbFaces()))
    {
      appendUniqueRelationId(anAffectedFaces, aCoEdge.FaceId);
    }
  }
  if (!detachOrderedParentRef(*myGraph,
                              myGraph->myData->myIncStorage,
                              theWireRefId,
                              aFaceRefIdsRO,
                              BRepGraph_NodeId(aChildWireId),
                              true))
  {
    return false;
  }
  for (const BRepGraph_CoEdgeId& aCoEdgeId : anAffectedCoEdges)
  {
    if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(aCoEdgeId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.FaceId.IsValid(aStorage.NbFaces()))
    {
      continue;
    }
    aStorage.MarkRemoved(aCoEdge.Curve2DRepId);
    aStorage.MarkRemoved(aCoEdge.Polygon2DRepId);
    aStorage.MarkRemoved(aCoEdge.PolygonOnTriRepId);
    myGraph->markModified(aCoEdgeId);
  }

  myGraph->markModified(theFaceId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveWire: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::WireOps::RemoveCoEdge(const BRepGraph_WireId   theChildWireId,
                                                  const BRepGraph_CoEdgeId theCoEdgeId)
{
  Standard_ASSERT_RETURN(theChildWireId.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "RemoveCoEdge: wire index is out of range",
                         false);
  Standard_ASSERT_RETURN(theCoEdgeId.IsValid(myGraph->myData->myIncStorage.NbCoEdges()),
                         "RemoveCoEdge: coedge index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theChildWireId))
  {
    return false;
  }

  if (!isActiveNode(aStorage, theCoEdgeId))
  {
    return false;
  }

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdgeIds =
    aStorage.WireRelations(theChildWireId).CoEdgeIds;
  if (!findOrderedRef(aWireCoEdgeIds, theCoEdgeId).IsValid())
  {
    return false;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> aRemainingCoEdges;
  if (!rotateConnectedCoEdgesAfterRemoval(aStorage, aWireCoEdgeIds, theCoEdgeId, aRemainingCoEdges))
  {
    return false;
  }

  const BRepGraph_CoEdgeId aCoEdgeEntity = theCoEdgeId;

  aStorage.DetachCoEdgeUse(theChildWireId, theCoEdgeId);
  if (!aRemainingCoEdges.IsEmpty())
  {
    aStorage.SetWireCoEdges(theChildWireId, aRemainingCoEdges.ToArray1());
  }

  const bool hasRemainingCoEdgeUsage = hasAnyActiveUsage(*myGraph, aCoEdgeEntity);

  myGraph->markModified(theChildWireId);
  if (!hasRemainingCoEdgeUsage)
  {
    myGraph->Editor().Gen().RemoveSubgraph(aCoEdgeEntity);
  }
  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveCoEdge: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::EdgeOps::RemoveVertex(const BRepGraph_EdgeId      theChildEdgeId,
                                                  const BRepGraph_VertexRefId theVertexRefId)
{
  Standard_ASSERT_RETURN(theChildEdgeId.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "RemoveVertex: edge index is out of range",
                         false);
  Standard_ASSERT_RETURN(theVertexRefId.IsValid(myGraph->myData->myIncStorage.NbVertexRefs()),
                         "RemoveVertex: vertex ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theChildEdgeId))
  {
    return false;
  }

  const BRepGraphInc::VertexRef& aRef     = aStorage.VertexRef(theVertexRefId);
  const BRepGraphInc::EdgeDef&   anEdgeRO = aStorage.Edge(theChildEdgeId);
  if (aStorage.IsRemoved(theVertexRefId)
      || (anEdgeRO.StartVertexRefId != theVertexRefId && anEdgeRO.EndVertexRefId != theVertexRefId)
      || !aRef.ChildVertexId.IsValid() || !isActiveNode(aStorage, aRef.ChildVertexId))
  {
    return false;
  }

  BRepGraphInc::EdgeDef& aEdge   = aStorage.ChangeEdge(theChildEdgeId);
  bool                   isFound = false;
  bool                   isFixed = false;
  if (aEdge.StartVertexRefId == theVertexRefId)
  {
    aEdge.StartVertexRefId = BRepGraph_VertexRefId();
    isFound                = true;
    isFixed                = true;
  }
  else if (aEdge.EndVertexRefId == theVertexRefId)
  {
    aEdge.EndVertexRefId = BRepGraph_VertexRefId();
    isFound              = true;
    isFixed              = true;
  }
  else
  {
    return false;
  }

  if (!isFound)
  {
    return false;
  }

  const BRepGraph_VertexId aChildVertexId = aRef.ChildVertexId;
  if (!isRefOwnedByAnyParent(myGraph->myData->myIncStorage, theVertexRefId)
      && !myGraph->Editor().Gen().RemoveRef(theVertexRefId))
  {
    return false;
  }
  if (isFixed && !hasAnyActiveUsage(*myGraph, BRepGraph_NodeId(aChildVertexId)))
  {
    myGraph->Editor().Gen().RemoveSubgraph(BRepGraph_NodeId(aChildVertexId));
  }

  aStorage.RebindVertexEdge(aChildVertexId, BRepGraph_VertexId(), theChildEdgeId, theVertexRefId);
  myGraph->markModified(theChildEdgeId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateRelations(),
                         "RemoveVertex: post-mutation relation inconsistency");
  }

  return true;
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph::EditorView::EdgeOps::ReplaceVertex(
  const BRepGraph_EdgeId      theChildEdgeId,
  const BRepGraph_VertexRefId theOldVertexRefId,
  const BRepGraph_VertexId    theNewChildVertexId)
{
  Standard_ASSERT_RETURN(theChildEdgeId.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceVertex: edge index is out of range",
                         BRepGraph_VertexRefId());
  Standard_ASSERT_RETURN(theOldVertexRefId.IsValid(myGraph->myData->myIncStorage.NbVertexRefs()),
                         "ReplaceVertex: old vertex ref index is out of range",
                         BRepGraph_VertexRefId());
  Standard_ASSERT_RETURN(theNewChildVertexId.IsValid(myGraph->myData->myIncStorage.NbVertices()),
                         "ReplaceVertex: new vertex index is out of range",
                         BRepGraph_VertexRefId());

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theChildEdgeId) || !isActiveNode(aStorage, theNewChildVertexId))
  {
    return BRepGraph_VertexRefId();
  }

  // Capture old ref fields before any mutation (vector may reallocate).
  const BRepGraphInc::VertexRef& aOldRef   = aStorage.VertexRef(theOldVertexRefId);
  const BRepGraphInc::EdgeDef&   anOldEdge = aStorage.Edge(theChildEdgeId);
  if (aStorage.IsRemoved(theOldVertexRefId)
      || (anOldEdge.StartVertexRefId != theOldVertexRefId
          && anOldEdge.EndVertexRefId != theOldVertexRefId)
      || !aOldRef.ChildVertexId.IsValid())
  {
    return BRepGraph_VertexRefId();
  }
  const BRepGraph_VertexId aOldChildVertexId = aOldRef.ChildVertexId;
  const TopAbs_Orientation aOldOri           = aOldRef.Orientation;

  // Short-circuit: no-op if the new vertex is the same as the current one.
  if (aOldChildVertexId == theNewChildVertexId)
  {
    return theOldVertexRefId;
  }

  // Allocate the replacement ref entry.
  const BRepGraph_VertexRefId aNewRefId = aStorage.AppendVertexRef();
  BRepGraphInc::VertexRef&    aNewRef   = aStorage.ChangeVertexRef(aNewRefId);
  aNewRef.ChildVertexId                 = theNewChildVertexId;
  aNewRef.ParentEdgeId                  = theChildEdgeId;
  aNewRef.Orientation                   = aOldOri;
  myGraph->allocateRefUID(aNewRefId);

  // Swap the edge's slot that owned the old ref.
  BRepGraphInc::EdgeDef& anEdge = aStorage.ChangeEdge(theChildEdgeId);
  if (anEdge.StartVertexRefId == theOldVertexRefId)
  {
    anEdge.StartVertexRefId = aNewRefId;
  }
  else if (anEdge.EndVertexRefId == theOldVertexRefId)
  {
    anEdge.EndVertexRefId = aNewRefId;
  }
  else
  {
    // Supplemental direct vertex usages are no longer represented as persisted
    // edge-owned refs.
    aStorage.MarkRemovedRef(BRepGraph_RefId(aNewRefId));
    return BRepGraph_VertexRefId();
  }

  if (!isRefOwnedByAnyParent(myGraph->myData->myIncStorage, theOldVertexRefId))
  {
    myGraph->Editor().Gen().RemoveRef(theOldVertexRefId);
  }

  aStorage.RebindVertexEdge(aOldChildVertexId,
                            theNewChildVertexId,
                            theChildEdgeId,
                            theOldVertexRefId);

  myGraph->markRefModified(aNewRefId);
  myGraph->markModified(theChildEdgeId);

  if (!myGraph->myData->myIncStorage.DeferredMode())
  {
    Standard_ASSERT_VOID(aStorage.ValidateRelations(),
                         "ReplaceVertex: post-mutation relation inconsistency");
  }

  return aNewRefId;
}

//=================================================================================================

void BRepGraph::EditorView::CommitMutation() noexcept
{
  Standard_ASSERT_VOID(ValidateMutationBoundary(nullptr),
                       "CommitMutation: mutation boundary consistency check failed");
}

//=================================================================================================

bool BRepGraph::EditorView::ValidateMutationBoundary(
  NCollection_LinearVector<BoundaryIssue>* const theIssues) const
{
  bool                        isValid  = true;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (!aStorage.ValidateRelations())
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "Mutation boundary relation inconsistency";
      theIssues->Append(anIssue);
    }
  }

  constexpr BRepGraph_NodeId::Kind THE_KINDS[] = {BRepGraph_NodeId::Kind::Vertex,
                                                  BRepGraph_NodeId::Kind::Edge,
                                                  BRepGraph_NodeId::Kind::CoEdge,
                                                  BRepGraph_NodeId::Kind::Wire,
                                                  BRepGraph_NodeId::Kind::Face,
                                                  BRepGraph_NodeId::Kind::Shell,
                                                  BRepGraph_NodeId::Kind::Solid,
                                                  BRepGraph_NodeId::Kind::Compound,
                                                  BRepGraph_NodeId::Kind::CompSolid,
                                                  BRepGraph_NodeId::Kind::Product,
                                                  BRepGraph_NodeId::Kind::Occurrence};
  constexpr int THE_KINDS_COUNT = static_cast<int>(sizeof(THE_KINDS) / sizeof(THE_KINDS[0]));
  for (int aKindIdx = 0; aKindIdx < THE_KINDS_COUNT; ++aKindIdx)
  {
    const BRepGraph_NodeId::Kind aKind       = THE_KINDS[aKindIdx];
    const int                    aCachedCnt  = cachedActiveByKind(aStorage, aKind);
    const int                    anActualCnt = countActiveByKind(*myGraph, aKind);
    if (aCachedCnt == anActualCnt)
    {
      continue;
    }

    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue           anIssue;
      TCollection_AsciiString aDesc("Mutation boundary active count mismatch for ");
      aDesc += kindName(aKind);
      aDesc += ": cached=";
      aDesc += TCollection_AsciiString(aCachedCnt);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(anActualCnt);
      anIssue.NodeId      = BRepGraph_NodeId::Invalid(aKind);
      anIssue.Description = aDesc;
      theIssues->Append(anIssue);
    }
  }

  // Check that every OccurrenceDef::ChildNodeId carries a valid node kind
  // (Product or a topology kind). Positional self-id invariants are enforced
  // structurally by the typed id system and need no runtime check.
  for (BRepGraph_OccurrenceIterator anOccIt(*myGraph); anOccIt.More(); anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc = anOccIt.Current();
    if (!anOcc.ChildNodeId.IsValid() || isValidOccurrenceChildKind(anOcc.ChildNodeId.NodeKind))
    {
      continue;
    }
    isValid = false;
    if (theIssues != nullptr)
    {
      const BRepGraph_OccurrenceId anOccId = anOccIt.CurrentId();
      BoundaryIssue                anIssue;
      TCollection_AsciiString aDesc("Storage occurrence child kind invalid at occurrence idx=");
      aDesc += TCollection_AsciiString(static_cast<int>(anOccId.Index));
      aDesc += ": child kind=";
      aDesc += TCollection_AsciiString(static_cast<int>(anOcc.ChildNodeId.NodeKind));
      anIssue.NodeId      = BRepGraph_NodeId(anOccId);
      anIssue.Description = std::move(aDesc);
      theIssues->Append(anIssue);
    }
  }

  return isValid;
}

//=================================================================================================

void BRepGraph::EditorView::CompoundOps::ReplaceChild(const BRepGraph_ChildRefId theChildRef,
                                                      const BRepGraph_NodeId     theNewChild)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  const BRepGraph_CompoundId aParentCompound = aStorage.ChildRef(theChildRef).ParentCompoundId;
  myGraph->Editor().requireUnlocked(aParentCompound,
                                    "BRepGraph::EditorView::ReplaceChild(): locked compound");
  myGraph->Editor().requireNoActiveGuard(
    aParentCompound,
    "BRepGraph::EditorView::ReplaceChild(): guard active on compound");

  myGraph->Editor().Gen().SetChildRefChildNodeId(theChildRef, theNewChild);
}

//=================================================================================================

void BRepGraph::EditorView::CompSolidOps::ReplaceSolid(const BRepGraph_SolidRefId theSolidRef,
                                                       const BRepGraph_SolidId    theNewSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  const BRepGraph_CompSolidId aParentCompSolid = aStorage.SolidRef(theSolidRef).ParentCompSolidId;
  myGraph->Editor().requireUnlocked(aParentCompSolid,
                                    "BRepGraph::EditorView::ReplaceSolid(): locked compsolid");
  myGraph->Editor().requireNoActiveGuard(
    aParentCompSolid,
    "BRepGraph::EditorView::ReplaceSolid(): guard active on compsolid");

  myGraph->Editor().Solids().SetRefChildSolidId(theSolidRef, theNewSolid);
}

//=================================================================================================

bool BRepGraph::EditorView::ShellOps::RemoveFaces(
  const BRepGraph_ShellId                        theShellId,
  const NCollection_Array1<BRepGraph_FaceRefId>& theFaceRefs)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theShellId))
  {
    return false;
  }

  for (const BRepGraph_FaceRefId& aFaceRefId : theFaceRefs)
  {
    if (!aFaceRefId.IsValid(aStorage.NbFaceRefs()))
    {
      return false;
    }
    if (aStorage.IsRemoved(aFaceRefId))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_FaceRefId>& aShellRefIdsRO =
      aStorage.ShellRelations(theShellId).FaceRefIds;
    if (!containsOrderedRef(aShellRefIdsRO, aFaceRefId))
    {
      return false;
    }
  }

  // Pass 2: Remove all
  for (const BRepGraph_FaceRefId& aFaceRefId : theFaceRefs)
  {
    if (!RemoveFace(theShellId, aFaceRefId))
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::SolidOps::RemoveShells(
  const BRepGraph_SolidId                         theSolidId,
  const NCollection_Array1<BRepGraph_ShellRefId>& theShellRefs)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theSolidId))
  {
    return false;
  }

  for (const BRepGraph_ShellRefId& aShellRefId : theShellRefs)
  {
    if (!aShellRefId.IsValid(aStorage.NbShellRefs()))
    {
      return false;
    }
    if (aStorage.IsRemoved(aShellRefId))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_ShellRefId>& aSolidRefIdsRO =
      aStorage.SolidRelations(theSolidId).ShellRefIds;
    if (!containsOrderedRef(aSolidRefIdsRO, aShellRefId))
    {
      return false;
    }
  }

  // Pass 2: Remove all
  for (const BRepGraph_ShellRefId& aShellRefId : theShellRefs)
  {
    if (!RemoveShell(theSolidId, aShellRefId))
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::CompoundOps::RemoveChildren(
  const BRepGraph_CompoundId                      theCompoundId,
  const NCollection_Array1<BRepGraph_ChildRefId>& theChildRefs)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theCompoundId))
  {
    return false;
  }

  for (const BRepGraph_ChildRefId& aChildRefId : theChildRefs)
  {
    if (!aChildRefId.IsValid(aStorage.NbChildRefs()))
    {
      return false;
    }
    if (aStorage.IsRemoved(aChildRefId))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_ChildRefId>& aCompoundRefIdsRO =
      aStorage.CompoundRelations(theCompoundId).ChildRefIds;
    if (!containsOrderedRef(aCompoundRefIdsRO, aChildRefId))
    {
      return false;
    }
  }

  // Pass 2: Remove all
  for (const BRepGraph_ChildRefId& aChildRefId : theChildRefs)
  {
    if (!RemoveChild(theCompoundId, aChildRefId))
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::CompSolidOps::RemoveSolids(
  const BRepGraph_CompSolidId                     theCompSolidId,
  const NCollection_Array1<BRepGraph_SolidRefId>& theSolidRefs)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theCompSolidId))
  {
    return false;
  }

  for (const BRepGraph_SolidRefId& aSolidRefId : theSolidRefs)
  {
    if (!aSolidRefId.IsValid(aStorage.NbSolidRefs()))
    {
      return false;
    }
    if (aStorage.IsRemoved(aSolidRefId))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_SolidRefId>& aCompSolidRefIdsRO =
      aStorage.CompSolidRelations(theCompSolidId).SolidRefIds;
    if (!containsOrderedRef(aCompSolidRefIdsRO, aSolidRefId))
    {
      return false;
    }
  }

  // Pass 2: Remove all
  for (const BRepGraph_SolidRefId& aSolidRefId : theSolidRefs)
  {
    if (!RemoveSolid(theCompSolidId, aSolidRefId))
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ProductOps::RemoveOccurrences(
  const BRepGraph_ProductId                            theProductId,
  const NCollection_Array1<BRepGraph_OccurrenceRefId>& theOccurrenceRefs)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theProductId))
  {
    return false;
  }

  for (const BRepGraph_OccurrenceRefId& anOccRefId : theOccurrenceRefs)
  {
    if (!anOccRefId.IsValid(aStorage.NbOccurrenceRefs()))
    {
      return false;
    }
    if (aStorage.IsRemoved(anOccRefId))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aProductRefIdsRO =
      aStorage.ProductRelations(theProductId).OccurrenceRefIds;
    if (!containsOrderedRef(aProductRefIdsRO, anOccRefId))
    {
      return false;
    }
  }

  // Pass 2: Remove all
  for (const BRepGraph_OccurrenceRefId& anOccRefId : theOccurrenceRefs)
  {
    if (!RemoveOccurrence(theProductId, anOccRefId))
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

NCollection_Array1<BRepGraph_FaceRefId> BRepGraph::EditorView::ShellOps::Append(
  const BRepGraph_ShellId                                    theShellEntity,
  const NCollection_Array1<BRepGraph_FaceId>&                theFaceIds,
  const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate orientations array size
  if (!theOrientations.IsEmpty() && theOrientations.Size() != theFaceIds.Size())
  {
    return NCollection_Array1<BRepGraph_FaceRefId>();
  }

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theShellEntity))
  {
    return NCollection_Array1<BRepGraph_FaceRefId>();
  }
  myGraph->Editor().requireUnlocked(theShellEntity,
                                    "BRepGraph::EditorView::Append(): locked shell");
  myGraph->Editor().requireNoActiveGuard(theShellEntity,
                                         "BRepGraph::EditorView::Append(): guard active on shell");

  for (const BRepGraph_FaceId& aFaceId : theFaceIds)
  {
    if (!isActiveNode(aStorage, aFaceId))
    {
      return NCollection_Array1<BRepGraph_FaceRefId>();
    }
  }

  // Pass 2: Link all
  NCollection_Array1<BRepGraph_FaceRefId> aResult(theFaceIds.Size());
  for (size_t anIdx = 0; anIdx < theFaceIds.Size(); ++anIdx)
  {
    const TopAbs_Orientation anOri =
      theOrientations.IsEmpty() ? TopAbs_FORWARD : TopAbs_Orientation(theOrientations.At(anIdx));

    aResult.ChangeAt(anIdx) =
      aStorage.AttachFaceToShell(theShellEntity, theFaceIds.At(anIdx), anOri);
    myGraph->allocateRefUID(aResult.At(anIdx));
    myGraph->markRefModified(aResult.At(anIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Array1<BRepGraph_ShellRefId> BRepGraph::EditorView::SolidOps::Append(
  const BRepGraph_SolidId                                    theSolidEntity,
  const NCollection_Array1<BRepGraph_ShellId>&               theShellIds,
  const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate orientations array size
  if (!theOrientations.IsEmpty() && theOrientations.Size() != theShellIds.Size())
  {
    return NCollection_Array1<BRepGraph_ShellRefId>();
  }

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theSolidEntity))
  {
    return NCollection_Array1<BRepGraph_ShellRefId>();
  }
  myGraph->Editor().requireUnlocked(theSolidEntity,
                                    "BRepGraph::EditorView::Append(): locked solid");
  myGraph->Editor().requireNoActiveGuard(theSolidEntity,
                                         "BRepGraph::EditorView::Append(): guard active on solid");

  for (const BRepGraph_ShellId& aShellId : theShellIds)
  {
    if (!isActiveNode(aStorage, aShellId))
    {
      return NCollection_Array1<BRepGraph_ShellRefId>();
    }
  }

  // Pass 2: Link all
  NCollection_Array1<BRepGraph_ShellRefId> aResult(theShellIds.Size());
  for (size_t anIdx = 0; anIdx < theShellIds.Size(); ++anIdx)
  {
    const TopAbs_Orientation anOri =
      theOrientations.IsEmpty() ? TopAbs_FORWARD : TopAbs_Orientation(theOrientations.At(anIdx));

    aResult.ChangeAt(anIdx) =
      aStorage.AttachShellToSolid(theSolidEntity, theShellIds.At(anIdx), anOri);
    myGraph->allocateRefUID(aResult.At(anIdx));
    myGraph->markRefModified(aResult.At(anIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Array1<BRepGraph_ChildRefId> BRepGraph::EditorView::CompoundOps::Append(
  const BRepGraph_CompoundId                                 theCompoundEntity,
  const NCollection_Array1<BRepGraph_NodeId>&                theChildIds,
  const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate orientations array size
  if (!theOrientations.IsEmpty() && theOrientations.Size() != theChildIds.Size())
  {
    return NCollection_Array1<BRepGraph_ChildRefId>();
  }

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theCompoundEntity))
  {
    return NCollection_Array1<BRepGraph_ChildRefId>();
  }
  myGraph->Editor().requireUnlocked(theCompoundEntity,
                                    "BRepGraph::EditorView::Append(): locked compound");
  myGraph->Editor().requireNoActiveGuard(
    theCompoundEntity,
    "BRepGraph::EditorView::Append(): guard active on compound");

  for (const BRepGraph_NodeId& aChildId : theChildIds)
  {
    if (!isActiveTopologyNode(aStorage, aChildId))
    {
      return NCollection_Array1<BRepGraph_ChildRefId>();
    }
  }

  // Pass 2: Link all
  NCollection_Array1<BRepGraph_ChildRefId> aResult(theChildIds.Size());
  for (size_t anIdx = 0; anIdx < theChildIds.Size(); ++anIdx)
  {
    const TopAbs_Orientation anOri =
      theOrientations.IsEmpty() ? TopAbs_FORWARD : TopAbs_Orientation(theOrientations.At(anIdx));

    aResult.ChangeAt(anIdx) = aStorage.AttachChildToCompound(theCompoundEntity,
                                                             theChildIds.At(anIdx),
                                                             TopLoc_Location(),
                                                             anOri);
    myGraph->allocateRefUID(aResult.At(anIdx));
    myGraph->markRefModified(aResult.At(anIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Array1<BRepGraph_SolidRefId> BRepGraph::EditorView::CompSolidOps::Append(
  const BRepGraph_CompSolidId                                theCompSolidEntity,
  const NCollection_Array1<BRepGraph_SolidId>&               theSolidIds,
  const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate orientations array size
  if (!theOrientations.IsEmpty() && theOrientations.Size() != theSolidIds.Size())
  {
    return NCollection_Array1<BRepGraph_SolidRefId>();
  }

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theCompSolidEntity))
  {
    return NCollection_Array1<BRepGraph_SolidRefId>();
  }
  myGraph->Editor().requireUnlocked(theCompSolidEntity,
                                    "BRepGraph::EditorView::Append(): locked compsolid");
  myGraph->Editor().requireNoActiveGuard(
    theCompSolidEntity,
    "BRepGraph::EditorView::Append(): guard active on compsolid");

  for (const BRepGraph_SolidId& aSolidId : theSolidIds)
  {
    if (!isActiveNode(aStorage, aSolidId))
    {
      return NCollection_Array1<BRepGraph_SolidRefId>();
    }
  }

  // Pass 2: Link all
  NCollection_Array1<BRepGraph_SolidRefId> aResult(theSolidIds.Size());
  for (size_t anIdx = 0; anIdx < theSolidIds.Size(); ++anIdx)
  {
    const TopAbs_Orientation anOri =
      theOrientations.IsEmpty() ? TopAbs_FORWARD : TopAbs_Orientation(theOrientations.At(anIdx));

    aResult.ChangeAt(anIdx) =
      aStorage.AttachSolidToCompSolid(theCompSolidEntity, theSolidIds.At(anIdx), anOri);
    myGraph->allocateRefUID(aResult.At(anIdx));
    myGraph->markRefModified(aResult.At(anIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Array1<BRepGraph_OccurrenceRefId> BRepGraph::EditorView::ProductOps::Append(
  const BRepGraph_ProductId                      theParentProduct,
  const NCollection_Array1<BRepGraph_ProductId>& theChildProducts,
  const NCollection_Array1<TopLoc_Location>&     thePlacements)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate placements array size
  if (thePlacements.Size() != theChildProducts.Size())
  {
    return NCollection_Array1<BRepGraph_OccurrenceRefId>();
  }

  // Pass 1: Validate all inputs
  if (!isActiveNode(aStorage, theParentProduct))
  {
    return NCollection_Array1<BRepGraph_OccurrenceRefId>();
  }
  myGraph->Editor().requireUnlocked(theParentProduct,
                                    "BRepGraph::EditorView::Append(): locked product");
  myGraph->Editor().requireNoActiveGuard(
    theParentProduct,
    "BRepGraph::EditorView::Append(): guard active on product");

  for (const BRepGraph_ProductId& aChildProduct : theChildProducts)
  {
    if (!isActiveNode(aStorage, aChildProduct))
    {
      return NCollection_Array1<BRepGraph_OccurrenceRefId>();
    }
    if (aChildProduct == theParentProduct)
    {
      return NCollection_Array1<BRepGraph_OccurrenceRefId>();
    }
  }

  // Pass 2: Link all
  NCollection_Array1<BRepGraph_OccurrenceRefId> aResult(theChildProducts.Size());
  for (size_t anIdx = 0; anIdx < theChildProducts.Size(); ++anIdx)
  {
    BRepGraph_OccurrenceRefId anOccRefId;
    BRepGraph_OccurrenceId    anOccId = Append(theParentProduct,
                                            theChildProducts.At(anIdx),
                                            thePlacements.At(anIdx),
                                            BRepGraph_OccurrenceId(),
                                            &anOccRefId);
    if (!anOccId.IsValid())
    {
      return NCollection_Array1<BRepGraph_OccurrenceRefId>();
    }
    aResult.ChangeAt(anIdx) = anOccRefId;
  }

  return aResult;
}
