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
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_PackedMap.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Assert.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>

#include <shared_mutex>

namespace
{

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
      return true;
  }
  return false;
}

void removeFromRootProducts(NCollection_DynamicArray<BRepGraph_ProductId>& theRoots,
                            const BRepGraph_ProductId                      theProduct)
{
  NCollection_DynamicArray<BRepGraph_ProductId> aFiltered;
  for (const BRepGraph_ProductId& aRoot : theRoots)
  {
    if (aRoot != theProduct)
    {
      aFiltered.Append(aRoot);
    }
  }
  theRoots = std::move(aFiltered);
}

BRepGraph_NodeId refChildNode(const BRepGraph& theGraph, const BRepGraph_RefId theRef)
{
  return theGraph.Refs().ChildNode(theRef);
}

bool hasAnyActiveUsage(const BRepGraph& theGraph, const BRepGraph_NodeId theChild)
{
  if (!theChild.IsValid())
  {
    return false;
  }

  // DirectParents enumerates every active parent via the reverse index across
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
      return theRefId;
  }
  return RefIdT();
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
        theRefIds.ChangeValue(static_cast<size_t>(i)) =
          theRefIds.Value(static_cast<size_t>(i + 1u));
      theRefIds.EraseLast();
      return;
    }
  }
}

template <typename RefIdT>
bool detachOrderedParentRef(BRepGraph&                        theGraph,
                            const RefIdT                      theRefId,
                            NCollection_DynamicArray<RefIdT>& theParentRefIds,
                            const BRepGraph_NodeId            theChildNode,
                            const bool                        theToPruneOrphanedChild)
{
  if (!findOrderedRef(theParentRefIds, theRefId).IsValid())
  {
    return false;
  }

  if (!theGraph.Editor().Gen().RemoveRef(theRefId))
  {
    return false;
  }

  eraseOrderedRef(theRefId, theParentRefIds);

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
    return false;

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

static const BRepGraphInc::BaseDef* topoEntity(const BRepGraphInc_Storage& theStorage,
                                               const BRepGraph_NodeId      theNode)
{
  if (!isNodeIndexInRange(theStorage, theNode))
  {
    return nullptr;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return &theStorage.Vertex(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return &theStorage.Edge(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return &theStorage.CoEdge(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return &theStorage.Wire(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return &theStorage.Face(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return &theStorage.Shell(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return &theStorage.Solid(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return &theStorage.Compound(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return &theStorage.CompSolid(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return &theStorage.Product(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return &theStorage.Occurrence(BRepGraph_OccurrenceId(theNode));
  }

  return nullptr;
}

//=================================================================================================

static bool isActiveNode(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId theNode)
{
  const BRepGraphInc::BaseDef* aDef = topoEntity(theStorage, theNode);
  return aDef != nullptr && !aDef->IsRemoved;
}

//=================================================================================================

static bool isActiveTopologyNode(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_NodeId      theNode)
{
  return theNode.IsValid() && BRepGraph_NodeId::IsTopologyKind(theNode.NodeKind)
         && isActiveNode(theStorage, theNode);
}

//=================================================================================================

[[maybe_unused]] static bool isRepIndexInRange(const BRepGraphInc_Storage& theStorage,
                                               const BRepGraph_RepId       theRepId)
{
  if (!theRepId.IsValid())
  {
    return false;
  }

  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::Surface:
      return theRepId.IsValid(theStorage.NbSurfaces());
    case BRepGraph_RepId::Kind::Curve3D:
      return theRepId.IsValid(theStorage.NbCurves3D());
    case BRepGraph_RepId::Kind::Curve2D:
      return theRepId.IsValid(theStorage.NbCurves2D());
    case BRepGraph_RepId::Kind::Triangulation:
      return theRepId.IsValid(theStorage.NbTriangulations());
    case BRepGraph_RepId::Kind::Polygon3D:
      return theRepId.IsValid(theStorage.NbPolygons3D());
    case BRepGraph_RepId::Kind::Polygon2D:
      return theRepId.IsValid(theStorage.NbPolygons2D());
    case BRepGraph_RepId::Kind::PolygonOnTri:
      return theRepId.IsValid(theStorage.NbPolygonsOnTri());
  }

  return false;
}

//=================================================================================================

static void rebindCoEdgesForEdgeReplacement(BRepGraphInc_Storage&  theStorage,
                                            const BRepGraph_EdgeId theSourceEdgeId,
                                            const BRepGraph_EdgeId theReplacementEdgeId)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.ReverseIndex().CoEdgesOfEdgeRef(theSourceEdgeId);
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
      continue;

    BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);
    if (aCoEdge.IsRemoved || aCoEdge.EdgeDefId != theSourceEdgeId)
      continue;

    theStorage.ChangeReverseIndex().UnbindEdgeFromCoEdge(theSourceEdgeId, aCoEdgeId);
    aCoEdge.EdgeDefId = theReplacementEdgeId;
    theStorage.ChangeReverseIndex().BindEdgeToCoEdge(theReplacementEdgeId, aCoEdgeId);
  }
}

//=================================================================================================

static void unbindCoEdgesOfRemovedEdge(BRepGraphInc_Storage&  theStorage,
                                       const BRepGraph_EdgeId theEdgeId)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdgeId);
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
      continue;

    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.IsRemoved && aCoEdge.EdgeDefId == theEdgeId)
      theStorage.ChangeReverseIndex().UnbindEdgeFromCoEdge(theEdgeId, aCoEdgeId);
  }
}

//=================================================================================================

template <typename DefType>
int countIterator(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (BRepGraph_Iterator<DefType> anIt(theGraph); anIt.More(); anIt.Next())
    ++aCount;
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

[[maybe_unused]] const NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& wireCoEdgeRefIds(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  return theStorage.Wire(theWireId).CoEdgeRefIds;
}

//! Initialize a sub-edge definition produced by Split.
//! Copies shared properties from the original edge and assigns boundary vertex ref ids.
//! Vertex ref entries must already exist in storage; only their RefId indices are passed.
void initSubEdgeEntity(BRepGraphInc::EdgeDef&       theSub,
                       const BRepGraph_Curve3DRepId theCurve3DRepId,
                       const double                 theTolerance,
                       const bool                   theSameParameter,
                       const BRepGraph_VertexRefId  theStartVertexRefId,
                       const BRepGraph_VertexRefId  theEndVertexRefId,
                       const double                 theParamFirst,
                       const double                 theParamLast)
{
  theSub.Curve3DRepId     = theCurve3DRepId;
  theSub.Tolerance        = theTolerance;
  theSub.SameParameter    = theSameParameter;
  theSub.SameRange        = false;
  theSub.IsDegenerate     = false;
  theSub.StartVertexRefId = theStartVertexRefId;
  theSub.EndVertexRefId = theEndVertexRefId;
  theSub.ParamFirst       = theParamFirst;
  theSub.ParamLast        = theParamLast;
}

//! Initialize a sub-CoEdge definition produced by Split.
void initSubCoEdgeEntity(BRepGraphInc::CoEdgeDef&     theCE,
                         const BRepGraph_EdgeId       theEdgeId,
                         const BRepGraph_FaceId       theFaceId,
                         const TopAbs_Orientation     theOrientation,
                         const BRepGraph_Curve2DRepId theCurve2DRepId,
                         const double                 theParamFirst,
                         const double                 theParamLast,
                         const GeomAbs_Shape          theContinuity)
{
  theCE.EdgeDefId = theEdgeId;
  theCE.FaceDefId = theFaceId;
  theCE.Orientation  = theOrientation;
  theCE.Curve2DRepId = theCurve2DRepId;
  theCE.ParamFirst   = theParamFirst;
  theCE.ParamLast    = theParamLast;
  theCE.Continuity   = theContinuity;
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
    const BRepGraph_VertexRefId aStartVtxRefId(aStorage.NbVertexRefs());
    aStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aStartVtxRef = aStorage.ChangeVertexRef(aStartVtxRefId);
    aStartVtxRef.ParentId = aEdgeId;
    aStartVtxRef.VertexDefId = theStartVtx;
    aStartVtxRef.Orientation              = TopAbs_FORWARD;
    myGraph->allocateRefUID(aStartVtxRefId);
    anEdgeDef.StartVertexRefId = aStartVtxRefId;
  }
  if (theEndVtx.IsValid())
  {
    const BRepGraph_VertexRefId anEndVtxRefId(aStorage.NbVertexRefs());
    aStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& anEndVtxRef = aStorage.ChangeVertexRef(anEndVtxRefId);
    anEndVtxRef.ParentId = aEdgeId;
    anEndVtxRef.VertexDefId = theEndVtx;
    anEndVtxRef.Orientation              = TopAbs_REVERSED;
    myGraph->allocateRefUID(anEndVtxRefId);
    anEdgeDef.EndVertexRefId = anEndVtxRefId;
  }
  anEdgeDef.ParamFirst    = theFirst;
  anEdgeDef.ParamLast     = theLast;
  anEdgeDef.Tolerance     = theTolerance;
  anEdgeDef.SameParameter = true;
  anEdgeDef.SameRange     = true;
  myGraph->allocateUID(aEdgeId);

  if (!theCurve.IsNull())
  {
    const BRepGraph_Curve3DRepId aCurveRepId     = aStorage.AppendCurve3DRep();
    aStorage.ChangeCurve3DRep(aCurveRepId).Curve = theCurve;
    anEdgeDef.Curve3DRepId                       = aCurveRepId;
  }

  return aEdgeId;
}

//=================================================================================================

BRepGraph_WireId BRepGraph::EditorView::WireOps::Add(
  const NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>>& theEdges)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  for (const std::pair<BRepGraph_EdgeId, TopAbs_Orientation>& anEdgeEntry : theEdges)
  {
    if (!isActiveNode(aStorage, anEdgeEntry.first))
    {
      return BRepGraph_WireId();
    }
  }

  const BRepGraph_WireId aWireId(aStorage.NbWires());
  aStorage.AppendWire();
  myGraph->allocateUID(aWireId);

  for (const std::pair<BRepGraph_EdgeId, TopAbs_Orientation>& anEdgeEntry : theEdges)
  {
    const BRepGraph_EdgeId   anEdgeDefId = anEdgeEntry.first;
    const TopAbs_Orientation anOri       = anEdgeEntry.second;

    // Create CoEdge entity for this edge-wire binding.
    const BRepGraph_CoEdgeId aCoEdgeId(aStorage.NbCoEdges());
    aStorage.AppendCoEdge();
    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
    aCoEdge.EdgeDefId = anEdgeDefId;
    aCoEdge.Orientation              = anOri;
    myGraph->allocateUID(aCoEdgeId);

    // CoEdgeRef in ref-table.
    const BRepGraph_CoEdgeRefId aCoEdgeRefId(aStorage.NbCoEdgeRefs());
    aStorage.AppendCoEdgeRef();
    BRepGraphInc::CoEdgeRef& aCoEdgeRef = aStorage.ChangeCoEdgeRef(aCoEdgeRefId);
    aCoEdgeRef.ParentId = aWireId;
    aCoEdgeRef.CoEdgeDefId = aCoEdgeId;
    myGraph->allocateRefUID(aCoEdgeRefId);
    aStorage.ChangeWire(aWireId).CoEdgeRefIds.Append(aCoEdgeRefId);

    aStorage.ChangeReverseIndex().BindEdgeToWire(aCoEdge.EdgeDefId, aWireId);
    aStorage.ChangeReverseIndex().BindEdgeToCoEdge(aCoEdge.EdgeDefId, aCoEdgeId);
    aStorage.ChangeReverseIndex().BindCoEdgeToWire(aCoEdgeId, aWireId);
  }

  // Check closure.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_EdgeId   aFirstEdgeNodeId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri        = theEdges.First().second;
    const BRepGraph_EdgeId   aLastEdgeNodeId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri         = theEdges.Last().second;

    const BRepGraphInc::EdgeDef& aFirstEdge = aStorage.Edge(aFirstEdgeNodeId);
    const BRepGraphInc::EdgeDef& aLastEdge  = aStorage.Edge(aLastEdgeNodeId);
    const BRepGraph_VertexRefId  aFirstRefId =
      (aFirstOri == TopAbs_FORWARD) ? aFirstEdge.StartVertexRefId : aFirstEdge.EndVertexRefId;
    const BRepGraph_VertexRefId aLastRefId =
      (aLastOri == TopAbs_FORWARD) ? aLastEdge.EndVertexRefId : aLastEdge.StartVertexRefId;
    const BRepGraph_NodeId aFirstVtx =
      aFirstRefId.IsValid() ? BRepGraph_NodeId(aStorage.VertexRef(aFirstRefId).VertexDefId)
                            : BRepGraph_NodeId();
    const BRepGraph_NodeId aLastVtx =
      aLastRefId.IsValid() ? BRepGraph_NodeId(aStorage.VertexRef(aLastRefId).VertexDefId)
                           : BRepGraph_NodeId();

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    aStorage.ChangeWire(aWireId).IsClosed = aIsClosed;
  }

  return aWireId;
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph::EditorView::EdgeOps::AddInternalVertex(
  const BRepGraph_EdgeId   theEdgeEntity,
  const BRepGraph_VertexId theVertexEntity,
  const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdgeEntity) || !isActiveNode(aStorage, theVertexEntity))
  {
    return BRepGraph_VertexRefId();
  }

  const BRepGraph_VertexRefId aVertexRefId(aStorage.NbVertexRefs());
  aStorage.AppendVertexRef();
  BRepGraphInc::VertexRef& aVertexRef = aStorage.ChangeVertexRef(aVertexRefId);
  aVertexRef.ParentId = theEdgeEntity;
  aVertexRef.VertexDefId = theVertexEntity;
  aVertexRef.Orientation              = theOri;
  myGraph->allocateRefUID(aVertexRefId);
  aStorage.ChangeEdge(theEdgeEntity).InternalVertexRefIds.Append(aVertexRefId);

  aStorage.ChangeReverseIndex().BindVertexToEdge(theVertexEntity, theEdgeEntity);
  myGraph->markModified(theEdgeEntity);
  return aVertexRefId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph::EditorView::FaceOps::Add(
  const occ::handle<Geom_Surface>&                  theSurface,
  const BRepGraph_WireId                            theOuterWire,
  const NCollection_DynamicArray<BRepGraph_WireId>& theInnerWires,
  const double                                      theTolerance)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theOuterWire.IsValid() && !isActiveNode(aStorage, theOuterWire))
  {
    return BRepGraph_FaceId();
  }
  for (const BRepGraph_WireId& aWireDefId : theInnerWires)
  {
    if (aWireDefId.IsValid() && !isActiveNode(aStorage, aWireDefId))
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
    const BRepGraph_SurfaceRepId aSurfRepId       = aStorage.AppendSurfaceRep();
    aStorage.ChangeSurfaceRep(aSurfRepId).Surface = theSurface;
    aFaceDef.SurfaceRepId                         = aSurfRepId;
  }

  // Link wire refs.
  if (theOuterWire.IsValid())
  {
    const BRepGraph_WireRefId anOuterWireRefId(aStorage.NbWireRefs());
    aStorage.AppendWireRef();
    BRepGraphInc::WireRef& anOuterWireRef = aStorage.ChangeWireRef(anOuterWireRefId);
    anOuterWireRef.ParentId = aFaceId;
    anOuterWireRef.WireDefId = theOuterWire;
    anOuterWireRef.IsOuter                = true;
    myGraph->allocateRefUID(anOuterWireRefId);
    aStorage.ChangeFace(aFaceId).WireRefIds.Append(anOuterWireRefId);
    aStorage.ChangeReverseIndex().BindWireToFace(theOuterWire, aFaceId);
  }

  for (const BRepGraph_WireId& aWireDefId : theInnerWires)
  {
    if (!aWireDefId.IsValid())
    {
      continue;
    }
    const BRepGraph_WireRefId aWireRefId(aStorage.NbWireRefs());
    aStorage.AppendWireRef();
    BRepGraphInc::WireRef& aWireRef = aStorage.ChangeWireRef(aWireRefId);
    aWireRef.ParentId = aFaceId;
    aWireRef.WireDefId = aWireDefId;
    aWireRef.IsOuter                = false;
    myGraph->allocateRefUID(aWireRefId);
    aStorage.ChangeFace(aFaceId).WireRefIds.Append(aWireRefId);
    aStorage.ChangeReverseIndex().BindWireToFace(aWireDefId, aFaceId);
  }

  return aFaceId;
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph::EditorView::FaceOps::AddVertex(
  const BRepGraph_FaceId   theFaceEntity,
  const BRepGraph_VertexId theVertexEntity,
  const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theFaceEntity) || !isActiveNode(aStorage, theVertexEntity))
  {
    return BRepGraph_VertexRefId();
  }

  const BRepGraph_VertexRefId aVertexRefId(aStorage.NbVertexRefs());
  aStorage.AppendVertexRef();
  BRepGraphInc::VertexRef& aVertexRef = aStorage.ChangeVertexRef(aVertexRefId);
  aVertexRef.ParentId = theFaceEntity;
  aVertexRef.VertexDefId = theVertexEntity;
  aVertexRef.Orientation              = theOri;
  myGraph->allocateRefUID(aVertexRefId);
  aStorage.ChangeFace(theFaceEntity).VertexRefIds.Append(aVertexRefId);

  myGraph->markModified(theFaceEntity);
  return aVertexRefId;
}

//=================================================================================================

BRepGraph_ShellId BRepGraph::EditorView::ShellOps::Add()
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_ShellId aShellId(aStorage.NbShells());
  aStorage.AppendShell();
  myGraph->allocateUID(aShellId);

  return aShellId;
}

//=================================================================================================

BRepGraph_SolidId BRepGraph::EditorView::SolidOps::Add()
{
  BRepGraphInc_Storage&   aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_SolidId aSolidId(aStorage.NbSolids());
  aStorage.AppendSolid();
  myGraph->allocateUID(aSolidId);

  return aSolidId;
}

//=================================================================================================

BRepGraph_FaceRefId BRepGraph::EditorView::ShellOps::AddFace(const BRepGraph_ShellId theShellEntity,
                                                             const BRepGraph_FaceId  theFaceEntity,
                                                             const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theShellEntity) || !isActiveNode(aStorage, theFaceEntity))
  {
    return BRepGraph_FaceRefId();
  }

  // Append FaceRef to the shell definition.
  const BRepGraph_FaceRefId aFaceRefId(aStorage.NbFaceRefs());
  aStorage.AppendFaceRef();
  BRepGraphInc::FaceRef& aFREntry = aStorage.ChangeFaceRef(aFaceRefId);
  aFREntry.ParentId = theShellEntity;
  aFREntry.FaceDefId = theFaceEntity;
  aFREntry.Orientation            = theOri;
  myGraph->allocateRefUID(aFaceRefId);
  aStorage.ChangeShell(theShellEntity).FaceRefIds.Append(aFaceRefId);

  aStorage.ChangeReverseIndex().BindFaceToShell(theFaceEntity, theShellEntity);
  myGraph->markModified(theShellEntity);
  return aFaceRefId;
}

//=================================================================================================

BRepGraph_ChildRefId BRepGraph::EditorView::ShellOps::AddChild(
  const BRepGraph_ShellId  theShellEntity,
  const BRepGraph_NodeId   theChildEntity,
  const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theShellEntity) || !isActiveTopologyNode(aStorage, theChildEntity))
  {
    return BRepGraph_ChildRefId();
  }
  if (theChildEntity.NodeKind != BRepGraph_NodeId::Kind::Wire
      && theChildEntity.NodeKind != BRepGraph_NodeId::Kind::Edge)
  {
    return BRepGraph_ChildRefId();
  }

  const BRepGraph_ChildRefId aChildRefId(aStorage.NbChildRefs());
  aStorage.AppendChildRef();
  BRepGraphInc::ChildRef& aChildRef = aStorage.ChangeChildRef(aChildRefId);
  aChildRef.ParentId = theShellEntity;
  aChildRef.ChildDefId = theChildEntity;
  aChildRef.Orientation             = theOri;
  myGraph->allocateRefUID(aChildRefId);
  aStorage.ChangeShell(theShellEntity).AuxChildRefIds.Append(aChildRefId);

  myGraph->markModified(theShellEntity);
  return aChildRefId;
}

//=================================================================================================

BRepGraph_ShellRefId BRepGraph::EditorView::SolidOps::AddShell(
  const BRepGraph_SolidId  theSolidEntity,
  const BRepGraph_ShellId  theShellEntity,
  const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theSolidEntity) || !isActiveNode(aStorage, theShellEntity))
  {
    return BRepGraph_ShellRefId();
  }

  // Append ShellRef to the solid definition.
  const BRepGraph_ShellRefId aShellRefId(aStorage.NbShellRefs());
  aStorage.AppendShellRef();
  BRepGraphInc::ShellRef& aSREntry = aStorage.ChangeShellRef(aShellRefId);
  aSREntry.ParentId = theSolidEntity;
  aSREntry.ShellDefId = theShellEntity;
  aSREntry.Orientation             = theOri;
  myGraph->allocateRefUID(aShellRefId);
  aStorage.ChangeSolid(theSolidEntity).ShellRefIds.Append(aShellRefId);

  aStorage.ChangeReverseIndex().BindShellToSolid(theShellEntity, theSolidEntity);
  myGraph->markModified(theSolidEntity);
  return aShellRefId;
}

//=================================================================================================

BRepGraph_ChildRefId BRepGraph::EditorView::SolidOps::AddChild(
  const BRepGraph_SolidId  theSolidEntity,
  const BRepGraph_NodeId   theChildEntity,
  const TopAbs_Orientation theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theSolidEntity) || !isActiveTopologyNode(aStorage, theChildEntity))
  {
    return BRepGraph_ChildRefId();
  }
  if (theChildEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      && theChildEntity.NodeKind != BRepGraph_NodeId::Kind::Vertex)
  {
    return BRepGraph_ChildRefId();
  }

  const BRepGraph_ChildRefId aChildRefId(aStorage.NbChildRefs());
  aStorage.AppendChildRef();
  BRepGraphInc::ChildRef& aChildRef = aStorage.ChangeChildRef(aChildRefId);
  aChildRef.ParentId = theSolidEntity;
  aChildRef.ChildDefId = theChildEntity;
  aChildRef.Orientation             = theOri;
  myGraph->allocateRefUID(aChildRefId);
  aStorage.ChangeSolid(theSolidEntity).AuxChildRefIds.Append(aChildRefId);

  myGraph->markModified(theSolidEntity);
  return aChildRefId;
}

//=================================================================================================

BRepGraph_CompoundId BRepGraph::EditorView::CompoundOps::Add(
  const NCollection_DynamicArray<BRepGraph_NodeId>& theChildEntities)
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
  BRepGraphInc::CompoundDef& aCompDef = aStorage.ChangeCompound(aCompoundId);
  myGraph->allocateUID(aCompoundId);

  for (const BRepGraph_NodeId& aChild : theChildEntities)
  {
    const BRepGraph_ChildRefId aChildRefId(aStorage.NbChildRefs());
    aStorage.AppendChildRef();
    BRepGraphInc::ChildRef& aCREntry = aStorage.ChangeChildRef(aChildRefId);
    aCREntry.ParentId = aCompoundId;
    aCREntry.ChildDefId = aChild;
    myGraph->allocateRefUID(aChildRefId);
    aCompDef.ChildRefIds.Append(aChildRefId);
    aStorage.ChangeReverseIndex().BindCompoundChild(aChild, aCompoundId);
  }

  return aCompoundId;
}

//=================================================================================================

BRepGraph_ChildRefId BRepGraph::EditorView::CompoundOps::AddChild(
  const BRepGraph_CompoundId theCompoundEntity,
  const BRepGraph_NodeId     theChildEntity,
  const TopAbs_Orientation   theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompoundEntity) || !isActiveTopologyNode(aStorage, theChildEntity))
  {
    return BRepGraph_ChildRefId();
  }

  const BRepGraph_ChildRefId aChildRefId(aStorage.NbChildRefs());
  aStorage.AppendChildRef();
  BRepGraphInc::ChildRef& aChildRef = aStorage.ChangeChildRef(aChildRefId);
  aChildRef.ParentId = theCompoundEntity;
  aChildRef.ChildDefId = theChildEntity;
  aChildRef.Orientation             = theOri;
  myGraph->allocateRefUID(aChildRefId);
  aStorage.ChangeCompound(theCompoundEntity).ChildRefIds.Append(aChildRefId);

  aStorage.ChangeReverseIndex().BindCompoundChild(theChildEntity, theCompoundEntity);
  myGraph->markModified(theCompoundEntity);
  return aChildRefId;
}

//=================================================================================================

BRepGraph_CompSolidId BRepGraph::EditorView::CompSolidOps::Add(
  const NCollection_DynamicArray<BRepGraph_SolidId>& theSolidEntities)
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
  BRepGraphInc::CompSolidDef& aCSolDef = aStorage.ChangeCompSolid(aCompSolidId);
  myGraph->allocateUID(aCompSolidId);

  for (const BRepGraph_SolidId& aSolidId : theSolidEntities)
  {
    const BRepGraph_SolidRefId aSolidRefId(aStorage.NbSolidRefs());
    aStorage.AppendSolidRef();
    BRepGraphInc::SolidRef& aSREntry = aStorage.ChangeSolidRef(aSolidRefId);
    aSREntry.ParentId = aCompSolidId;
    aSREntry.SolidDefId = aSolidId;
    myGraph->allocateRefUID(aSolidRefId);
    aCSolDef.SolidRefIds.Append(aSolidRefId);
    aStorage.ChangeReverseIndex().BindSolidToCompSolid(aSolidId, aCompSolidId);
  }

  return aCompSolidId;
}

//=================================================================================================

BRepGraph_SolidRefId BRepGraph::EditorView::CompSolidOps::AddSolid(
  const BRepGraph_CompSolidId theCompSolidEntity,
  const BRepGraph_SolidId     theSolidEntity,
  const TopAbs_Orientation    theOri)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompSolidEntity) || !isActiveNode(aStorage, theSolidEntity))
  {
    return BRepGraph_SolidRefId();
  }

  const BRepGraph_SolidRefId aSolidRefId(aStorage.NbSolidRefs());
  aStorage.AppendSolidRef();
  BRepGraphInc::SolidRef& aSREntry = aStorage.ChangeSolidRef(aSolidRefId);
  aSREntry.ParentId = theCompSolidEntity;
  aSREntry.SolidDefId = theSolidEntity;
  aSREntry.Orientation             = theOri;
  myGraph->allocateRefUID(aSolidRefId);
  aStorage.ChangeCompSolid(theCompSolidEntity).SolidRefIds.Append(aSolidRefId);

  aStorage.ChangeReverseIndex().BindSolidToCompSolid(theSolidEntity, theCompSolidEntity);
  myGraph->markModified(theCompSolidEntity);
  return aSolidRefId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::EditorView::ProductOps::LinkProductToTopology(
  const BRepGraph_NodeId theShapeRoot,
  const TopLoc_Location& thePlacement)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveTopologyNode(aStorage, theShapeRoot))
  {
    return BRepGraph_ProductId();
  }

  const BRepGraph_ProductId aProductId(aStorage.NbProducts());
  aStorage.AppendProduct();
  BRepGraphInc::ProductDef& aProductDef = aStorage.ChangeProduct(aProductId);
  myGraph->allocateUID(aProductId);

  // Link the product to its shape root via an occurrence + occurrence ref.
  const BRepGraph_OccurrenceId anOccId(aStorage.NbOccurrences());
  aStorage.AppendOccurrence();
  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
  anOccDef.ChildDefId = theShapeRoot;
  Standard_ASSERT_RETURN(isValidOccurrenceChildKind(anOccDef.ChildDefId.NodeKind),
                         "ProductOps::LinkProductToTopology: invalid occurrence child kind",
                         BRepGraph_ProductId());
  myGraph->allocateUID(anOccId);

  const BRepGraph_OccurrenceRefId anOccRefId(aStorage.NbOccurrenceRefs());
  aStorage.AppendOccurrenceRef();
  BRepGraphInc::OccurrenceRef& anOccRef = aStorage.ChangeOccurrenceRef(anOccRefId);
  anOccRef.ParentId = aProductId;
  anOccRef.OccurrenceDefId = anOccId;
  anOccRef.LocalLocation                = thePlacement;
  myGraph->allocateRefUID(anOccRefId);
  aProductDef.OccurrenceRefIds.Append(anOccRefId);

  // No reverse-index bind: the occurrence's child is a topology root, not a product;
  // myProductToOccurrences only indexes occurrences whose ChildDefId is a Product.
  myGraph->myData->myRootProductIds.Append(aProductId);

  return aProductId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::EditorView::ProductOps::CreateEmptyProduct()
{
  BRepGraphInc_Storage&     aStorage = myGraph->myData->myIncStorage;
  const BRepGraph_ProductId aProductId(aStorage.NbProducts());
  aStorage.AppendProduct();
  myGraph->allocateUID(aProductId);

  myGraph->myData->myRootProductIds.Append(aProductId);

  return aProductId;
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::EditorView::ProductOps::LinkProducts(
  const BRepGraph_ProductId theParentProduct,
  const BRepGraph_ProductId theReferencedProduct,
  const TopLoc_Location&    thePlacement)
{
  return LinkProducts(theParentProduct,
                      theReferencedProduct,
                      thePlacement,
                      BRepGraph_OccurrenceId());
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::EditorView::ProductOps::LinkProducts(
  const BRepGraph_ProductId    theParentProduct,
  const BRepGraph_ProductId    theReferencedProduct,
  const TopLoc_Location&       thePlacement,
  const BRepGraph_OccurrenceId theParentOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate that both arguments are active Product nodes with valid indices.
  if (!isActiveNode(aStorage, theParentProduct))
    return BRepGraph_OccurrenceId();
  if (!isActiveNode(aStorage, theReferencedProduct))
    return BRepGraph_OccurrenceId();
  // Prevent self-referencing cycles in the assembly DAG.
  if (theParentProduct == theReferencedProduct)
    return BRepGraph_OccurrenceId();
  // Validate parent occurrence if provided.
  if (theParentOccurrence.IsValid() && !isActiveNode(aStorage, theParentOccurrence))
    return BRepGraph_OccurrenceId();
  if (theParentOccurrence.IsValid()
      && aStorage.Occurrence(theParentOccurrence).ChildDefId != BRepGraph_NodeId(theParentProduct))
  {
    return BRepGraph_OccurrenceId();
  }

  const BRepGraph_OccurrenceId anOccId(aStorage.NbOccurrences());
  aStorage.AppendOccurrence();
  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
  anOccDef.ChildDefId = theReferencedProduct;
  Standard_ASSERT_RETURN(isValidOccurrenceChildKind(anOccDef.ChildDefId.NodeKind),
                         "ProductOps::LinkProducts: invalid occurrence child kind",
                         BRepGraph_OccurrenceId());
  myGraph->allocateUID(anOccId);

  // Add OccurrenceRef to the parent product.
  const BRepGraph_OccurrenceRefId anOccRefId(aStorage.NbOccurrenceRefs());
  aStorage.AppendOccurrenceRef();
  BRepGraphInc::OccurrenceRef& anOccRef = aStorage.ChangeOccurrenceRef(anOccRefId);
  anOccRef.ParentId = theParentProduct;
  anOccRef.OccurrenceDefId = anOccId;
  anOccRef.LocalLocation                = thePlacement;
  myGraph->allocateRefUID(anOccRefId);
  aStorage.ChangeProduct(theParentProduct).OccurrenceRefIds.Append(anOccRefId);

  // If the referenced product was a root, it is no longer (it now has a parent).
  removeFromRootProducts(myGraph->myData->myRootProductIds, theReferencedProduct);

  // myProductToOccurrences is keyed by ChildDefId (the referenced product).
  aStorage.ChangeReverseIndex().BindProductOccurrence(anOccId, theReferencedProduct);

  return anOccId;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::RemoveNode(const BRepGraph_NodeId theNode)
{
  RemoveNode(theNode, BRepGraph_NodeId());
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::RemoveNode(const BRepGraph_NodeId theNode,
                                               const BRepGraph_NodeId theReplacement)
{
  if (!theNode.IsValid())
    return;

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // When removing an Edge with a replacement, reparent all CoEdges from the
  // removed edge to the replacement edge. This prevents orphaned CoEdges
  // that would be excluded from queries via CoEdgesOfEdge().
  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid()
      && theReplacement != theNode && theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                           "RemoveNode: source edge index is out of range",
                           Standard_VOID_RETURN);
    Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theReplacement),
                           "RemoveNode: replacement edge index is out of range",
                           Standard_VOID_RETURN);
    Standard_ASSERT_RETURN(!aStorage.Edge(BRepGraph_EdgeId(theReplacement)).IsRemoved,
                           "RemoveNode: replacement edge must be active",
                           Standard_VOID_RETURN);

    rebindCoEdgesForEdgeReplacement(aStorage,
                                    BRepGraph_EdgeId::FromNodeId(theNode),
                                    BRepGraph_EdgeId::FromNodeId(theReplacement));
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::Edge:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::Face:
    case BRepGraph_NodeId::Kind::Shell:
    case BRepGraph_NodeId::Kind::Solid:
    case BRepGraph_NodeId::Kind::Compound:
    case BRepGraph_NodeId::Kind::CompSolid:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      break;
    default:
      Standard_ASSERT_RETURN(false, "RemoveNode: unsupported node kind", Standard_VOID_RETURN);
  }
  Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                         "RemoveNode: node index is out of range",
                         Standard_VOID_RETURN);
  if (!isActiveNode(aStorage, theNode))
  {
    return;
  }

  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    // Keep reverse edge->coedge table coherent for pure removals too.
    unbindCoEdgesOfRemovedEdge(aStorage, BRepGraph_EdgeId::FromNodeId(theNode));
  }

  // Mark removed on the entity (which is the sole definition store).
  BRepGraphInc::BaseDef* aDef = myGraph->changeTopoEntity(theNode);
  if (aDef != nullptr && !aDef->IsRemoved)
  {
    myGraph->myData->myIncStorage.MarkRemoved(theNode);
    // If this is a product, remove from root products.
    if (theNode.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      removeFromRootProducts(myGraph->myData->myRootProductIds,
                             BRepGraph_ProductId::FromNodeId(theNode));
    }
  }

  // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the removal.
  BRepGraphInc::BaseDef* aRemovedDef = myGraph->changeTopoEntity(theNode);
  if (aRemovedDef != nullptr)
  {
    ++aRemovedDef->OwnGen;
    ++aRemovedDef->SubtreeGen;
  }

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    myGraph->myData->myCurrentShapes.UnBind(theNode);
  }

  // Notify registered layers.
  myGraph->myLayerRegistry.DispatchOnNodeRemoved(theNode, theReplacement);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::RemoveSubgraph(const BRepGraph_NodeId theNode)
{
  // Cascade-aware reverse-index maintenance: per-kind unbinds inside the recursion
  // are skipped; we rebuild once at the outermost call exit. Inner mutations stay O(1).
  ++myGraph->myData->myRemoveSubgraphDepth;
  const bool isOutermost = myGraph->myData->myRemoveSubgraphDepth == 1;

  // Collect and recursively remove children BEFORE marking this node as removed,
  // because iterators (DefsIterator, RefsIterator) skip removed parents/children.
  // Children shared with other active parents are NOT removed (ownership-aware).
  //
  // Product and Occurrence require special handling:
  //  - Product: occurrence children cascade.
  //  - Occurrence: child occurrence cascade + parent product ref detachment.
  // All other kinds use the generic ChildExplorer/ParentExplorer cascade.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product: {
      if (theNode.IsValid(aStorage.NbProducts()))
      {
        // Snapshot occurrence indices before iterating, because RemoveSubgraph(Occurrence)
        // modifies the parent's OccurrenceRefIds via swap-remove.
        NCollection_DynamicArray<int> anOccIndices;
        for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph,
                                                       BRepGraph_ProductId::FromNodeId(theNode));
             anOccIt.More();
             anOccIt.Next())
        {
          anOccIndices.Append(aStorage.OccurrenceRef(anOccIt.CurrentId()).OccurrenceDefId.Index);
        }
        for (const int anOccIdx : anOccIndices)
          RemoveSubgraph(BRepGraph_OccurrenceId(anOccIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      if (theNode.IsValid(aStorage.NbOccurrences()))
      {
        const BRepGraphInc::OccurrenceDef& anOcc =
          myGraph->myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNode));

        // If the child is a topology node and has no other active usage, cascade into it.
        if (anOcc.ChildDefId.IsValid()
            && BRepGraph_NodeId::IsTopologyKind(anOcc.ChildDefId.NodeKind))
        {
          if (!hasAnyActiveUsage(*myGraph, anOcc.ChildDefId))
            RemoveSubgraph(anOcc.ChildDefId);
        }
        // Detach from parent product's OccurrenceRefIds.
        // Find the parent product by scanning OccurrenceRefs.
        for (BRepGraph_OccurrenceRefId aOccRefId = myGraph->Refs().Occurrences().StartId();
             aOccRefId < myGraph->Refs().Occurrences().EndId();
             ++aOccRefId)
        {
          const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aOccRefId);
          if (aRef.IsRemoved || aRef.OccurrenceDefId != theNode)
            continue;

          const BRepGraph_ProductId aParentProduct = BRepGraph_ProductId::FromNodeId(aRef.ParentId);
          if (!aParentProduct.IsValid(aStorage.NbProducts()))
            break;

          NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& aRefIds =
            myGraph->myData->myIncStorage.ChangeProduct(aParentProduct).OccurrenceRefIds;
          uint32_t anIdx = 0;
          for (NCollection_DynamicArray<BRepGraph_OccurrenceRefId>::Iterator anIt(aRefIds);
               anIt.More();
               anIt.Next(), ++anIdx)
          {
            if (anIt.Value() == aOccRefId)
            {
              myGraph->myData->myIncStorage.MarkRemovedRef(aOccRefId);
              if (anIdx < static_cast<uint32_t>(aRefIds.Size()) - 1u)
                aRefIds.ChangeValue(static_cast<size_t>(anIdx)) =
                  aRefIds.Value(aRefIds.Size() - 1u);
              aRefIds.EraseLast();
              myGraph->markModified(aParentProduct);
              break;
            }
          }
          break;
        }

        // After detaching, if the child product lost its last active parent occurrence,
        // re-add it to root products.
        if (anOcc.ChildDefId.IsValid()
            && anOcc.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
        {
          const BRepGraph_ProductId aChildProduct =
            BRepGraph_ProductId::FromNodeId(anOcc.ChildDefId);
          if (aChildProduct.IsValid(aStorage.NbProducts())
              && !aStorage.Product(aChildProduct).IsRemoved
              && !hasAnyActiveUsage(*myGraph, anOcc.ChildDefId))
          {
            myGraph->myData->myRootProductIds.Append(aChildProduct);
          }
        }
      }
      break;
    }
    default: {
      // Generic topology cascade via ChildExplorer(DirectChildren) + ParentExplorer(DirectParents).
      // Covers Compound, CompSolid, Solid, Shell, Face, Wire, CoEdge, Edge, Vertex.
      NCollection_DynamicArray<BRepGraph_NodeId> aChildNodes;
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
          RemoveSubgraph(aChild);
      }
      break;
    }
  }

  // Mark the node as removed AFTER children have been collected and processed,
  // so that iterators can still see this node as a valid parent during traversal.
  RemoveNode(theNode);

  --myGraph->myData->myRemoveSubgraphDepth;
  if (isOutermost)
  {
    myGraph->myData->myIncStorage.BuildReverseIndex();
  }
}

//=================================================================================================

bool BRepGraph::EditorView::GenOps::RemoveRef(const BRepGraph_RefId theRef)
{
  if (!theRef.IsValid())
  {
    return false;
  }

  if (!myGraph->myData->myIncStorage.MarkRemovedRef(theRef))
  {
    return false;
  }

  myGraph->myLayerRegistry.DispatchOnRefRemoved(theRef);
  myGraph->markRefModified(theRef);
  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::GenOps::RemoveRef(const BRepGraph_NodeId theParent,
                                              const BRepGraph_RefId  theRef,
                                              const bool             theToPruneOrphanedChild)
{
  if (!theRef.IsValid())
  {
    return false;
  }

  const BRepGraphInc::BaseRef& aRef = myGraph->myData->myIncStorage.BaseRef(theRef);
  Standard_ASSERT_RETURN(aRef.ParentId == theParent, "RemoveRef: reference parent mismatch", false);

  const BRepGraph_NodeId aChildNode = refChildNode(*myGraph, theRef);
  if (!RemoveRef(theRef))
  {
    return false;
  }

  if (theToPruneOrphanedChild && aChildNode.IsValid() && !hasAnyActiveUsage(*myGraph, aChildNode))
  {
    RemoveSubgraph(aChildNode);
  }
  return true;
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::RemoveRep(const BRepGraph_RepId theRep)
{
  if (!theRep.IsValid())
    return;

  if (myGraph->myData->myIncStorage.MarkRemovedRep(theRep))
  {
    myGraph->markRepModified(theRep);
  }
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::EditorView::CoEdgeOps::CreateCurve2DRep(
  const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
    return BRepGraph_Curve2DRepId();

  const BRepGraph_Curve2DRepId aRepId = myGraph->myData->myIncStorage.AppendCurve2DRep();
  myGraph->myData->myIncStorage.ChangeCurve2DRep(aRepId).Curve = theCurve2d;
  return aRepId;
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                                 const occ::handle<Geom2d_Curve>& theCurve2d)
{
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdge = myGraph->Editor().CoEdges().Mut(theCoEdge);
  aCoEdge.Internal().Curve2DRepId =
    theCurve2d.IsNull() ? BRepGraph_Curve2DRepId() : CreateCurve2DRep(theCurve2d);
  aCoEdge.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::AddPCurve(const BRepGraph_EdgeId           theEdgeEntity,
                                                 const BRepGraph_FaceId           theFaceEntity,
                                                 const occ::handle<Geom2d_Curve>& theCurve2d,
                                                 const double                     theFirst,
                                                 const double                     theLast,
                                                 const TopAbs_Orientation theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdgeEntity) || !isActiveNode(aStorage, theFaceEntity)
      || theCurve2d.IsNull())
  {
    return;
  }

  // Create CoEdge entity for the new PCurve binding.
  const BRepGraph_CoEdgeId aCoEdgeId(aStorage.NbCoEdges());
  aStorage.AppendCoEdge();
  BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeId);
  aCoEdge.EdgeDefId = theEdgeEntity;
  aCoEdge.FaceDefId = theFaceEntity;
  aCoEdge.Orientation              = theEdgeOrientation;
  if (!theCurve2d.IsNull())
  {
    const BRepGraph_Curve2DRepId aCurve2DRepId     = aStorage.AppendCurve2DRep();
    aStorage.ChangeCurve2DRep(aCurve2DRepId).Curve = theCurve2d;
    aCoEdge.Curve2DRepId                           = aCurve2DRepId;
  }
  aCoEdge.ParamFirst = theFirst;
  aCoEdge.ParamLast  = theLast;

  // Update reverse indices.
  aStorage.ChangeReverseIndex().BindEdgeToCoEdge(theEdgeEntity, aCoEdgeId);
  aStorage.ChangeReverseIndex().BindEdgeToFace(theEdgeEntity, theFaceEntity);
  myGraph->allocateUID(aCoEdgeId);

  myGraph->markModified(theEdgeEntity);
}

//=================================================================================================

void BRepGraph::EditorView::BeginDeferredInvalidation()
{
  myGraph->myData->myDeferredMode.store(true, std::memory_order_relaxed);
}

//=================================================================================================

void BRepGraph::EditorView::EndDeferredInvalidation() noexcept
{
  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
    return;

  myGraph->myData->myDeferredMode.store(false, std::memory_order_relaxed);

  NCollection_DynamicArray<BRepGraph_NodeId>& aDeferredList = myGraph->myData->myDeferredModified;

  if (aDeferredList.IsEmpty())
    return;

  // Shape cache uses SubtreeGen validation - no bulk clear needed.
  // Stale entries are detected on read via StoredSubtreeGen != entity.SubtreeGen.

  // Propagate SubtreeGen upward from each directly-modified node.
  // Dense per-kind visited flags for O(1) lookup without hashing overhead.
  const BRepGraphInc_ReverseIndex& aRevIdx  = myGraph->myData->myIncStorage.ReverseIndex();
  const BRepGraphInc_Storage&      aStorage = myGraph->myData->myIncStorage;

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
      return false;
    if (aArr.Value(static_cast<int>(theNode.Index)))
      return false;
    aArr.SetValue(static_cast<int>(theNode.Index), true);
    return true;
  };

  // BFS-style upward propagation: process nodes front-to-back, appending
  // newly discovered parents at the end. The loop index advances through
  // the growing vector, so each node is visited exactly once.
  NCollection_DynamicArray<BRepGraph_NodeId> aAllModified;
  aAllModified.SetIncrement(aDeferredList.Size() * 2);
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

    // Collect parent NodeIds via reverse index and increment SubtreeGen.
    // NOT OwnGen - parent's own data didn't change.
    // The visited set ensures each parent is incremented exactly once per flush,
    // even in diamond topologies. This matches immediate-mode LastPropWave semantics.
    switch (aNodeId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Vertex:
        // Vertex modifications don't propagate in deferred mode.
        break;
      case BRepGraph_NodeId::Kind::Edge: {
        const NCollection_DynamicArray<BRepGraph_WireId>* aWires =
          aRevIdx.WiresOfEdge(BRepGraph_EdgeId(aNodeId));
        if (aWires != nullptr)
          for (const BRepGraph_WireId& aWireId : *aWires)
          {
            const BRepGraph_NodeId aParentId = aWireId;
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::CoEdge:
        break;
      case BRepGraph_NodeId::Kind::Wire: {
        const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
          aRevIdx.FacesOfWire(BRepGraph_WireId(aNodeId));
        if (aFaces != nullptr)
          for (const BRepGraph_FaceId& aFaceId : *aFaces)
          {
            const BRepGraph_NodeId aParentId = aFaceId;
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::Face: {
        const NCollection_DynamicArray<BRepGraph_ShellId>* aShells =
          aRevIdx.ShellsOfFace(BRepGraph_FaceId(aNodeId));
        if (aShells != nullptr)
          for (const BRepGraph_ShellId& aShellId : *aShells)
          {
            const BRepGraph_NodeId aParentId = aShellId;
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::Shell: {
        const NCollection_DynamicArray<BRepGraph_SolidId>* aSolids =
          aRevIdx.SolidsOfShell(BRepGraph_ShellId(aNodeId));
        if (aSolids != nullptr)
          for (const BRepGraph_SolidId& aSolidId : *aSolids)
          {
            const BRepGraph_NodeId aParentId = aSolidId;
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::Occurrence: {
        // Occurrence modifications propagate to the parent product.
        // Find the parent product via OccurrenceRef.ParentId.
        for (BRepGraph_OccurrenceRefId aOccRefId = myGraph->Refs().Occurrences().StartId();
             aOccRefId < myGraph->Refs().Occurrences().EndId();
             ++aOccRefId)
        {
          const BRepGraphInc::OccurrenceRef& aRef =
            myGraph->myData->myIncStorage.OccurrenceRef(aOccRefId);
          if (!aRef.IsRemoved && aRef.OccurrenceDefId == aNodeId)
          {
            const BRepGraph_NodeId aParentId = aRef.ParentId;
            if (markVisited(aParentId))
            {
              BRepGraphInc::BaseDef* aParent = myGraph->changeTopoEntity(aParentId);
              if (aParent != nullptr && !aParent->IsRemoved)
              {
                ++aParent->SubtreeGen;
                aAllModified.Append(aParentId);
                aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
              }
            }
            break;
          }
        }
        break;
      }
      default:
        break;
    }
  }

  // Dispatch batch modification event to subscribing layers.
  if (myGraph->myLayerRegistry.HasModificationSubscribers() && !aAllModified.IsEmpty())
    myGraph->myLayerRegistry.DispatchNodesModified(aAllModified, aModifiedKindsMask);

  // Clear deferred list for next scope.
  aDeferredList.Clear();

  // Dispatch deferred reference modification events to subscribing layers.
  NCollection_DynamicArray<BRepGraph_RefId>& aDeferredRefList =
    myGraph->myData->myDeferredRefModified;
  if (!aDeferredRefList.IsEmpty() && myGraph->myLayerRegistry.HasRefModificationSubscribers())
  {
    int aRefKindsMask = 0;
    for (const BRepGraph_RefId& aRef : aDeferredRefList)
    {
      aRefKindsMask |= BRepGraph_Layer::RefKindBit(aRef.RefKind);
    }
    myGraph->myLayerRegistry.DispatchRefsModified(aDeferredRefList, aRefKindsMask);
  }
  aDeferredRefList.Clear();
}

//=================================================================================================

bool BRepGraph::EditorView::IsDeferredMode() const
{
  return myGraph->myData->myDeferredMode.load(std::memory_order_relaxed);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::applyModificationImpl(
  const BRepGraph_NodeId                       theTarget,
  NCollection_DynamicArray<BRepGraph_NodeId>&& theReplacements,
  const TCollection_AsciiString&               theOpLabel)
{
  myGraph->myData->myHistoryLog.Record(theOpLabel, theTarget, theReplacements);
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
  Standard_ASSERT_RETURN(theEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "Split: edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theSplitVertex.IsValid(myGraph->myData->myIncStorage.NbVertices()),
                         "Split: split-vertex index is out of range",
                         Standard_VOID_RETURN);

  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraphInc::EdgeDef& anOrig = myGraph->myData->myIncStorage.Edge(theEdgeEntity);
  Standard_ASSERT_RETURN(!anOrig.IsRemoved, "Split: source edge is removed", Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(!anOrig.IsDegenerate,
                         "Split: degenerate edge cannot be split",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(anOrig.ParamFirst < theSplitParam && theSplitParam < anOrig.ParamLast,
                         "Split: split parameter must be inside open edge range",
                         Standard_VOID_RETURN);

  const BRepGraphInc_Storage&  aConstStorage         = myGraph->myData->myIncStorage;
  const BRepGraph_Curve3DRepId aOrigCurve3DRepId     = anOrig.Curve3DRepId;
  const double                 aOrigTolerance        = anOrig.Tolerance;
  const bool                   aOrigSameParameter    = anOrig.SameParameter;
  const double                 aOrigParamFirst       = anOrig.ParamFirst;
  const double                 aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_VertexRefId  aOrigStartVertexRefId = anOrig.StartVertexRefId;
  const BRepGraph_VertexRefId  aOrigEndVertexRefId   = anOrig.EndVertexRefId;
  const bool                   aOrigSameRange        = anOrig.SameRange;

  // Resolve original vertex def ids through storage ref entries.
  const BRepGraph_VertexId aOrigStartVertexDefId =
    aOrigStartVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigStartVertexRefId).VertexDefId
                                    : BRepGraph_VertexId();
  const BRepGraph_VertexId aOrigEndVertexDefId =
    aOrigEndVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigEndVertexRefId).VertexDefId
                                  : BRepGraph_VertexId();

  // Copy wire indices: ReverseIdx may be rebuilt below.
  const NCollection_DynamicArray<BRepGraph_WireId>* aOrigWiresPtr =
    myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdge(theEdgeEntity);
  const NCollection_DynamicArray<BRepGraph_WireId> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_DynamicArray<BRepGraph_WireId>();

  BRepGraphInc_Storage& aMutStorage = myGraph->myData->myIncStorage;

  // Allocate SubA slot.
  const BRepGraph_EdgeId aSubAId(aMutStorage.NbEdges());
  aMutStorage.AppendEdge();
  theSubA = aSubAId;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference - use index).
  const BRepGraph_EdgeId aSubBId(aMutStorage.NbEdges());
  aMutStorage.AppendEdge();
  theSubB = aSubBId;

  // Build vertex ref entries for the split vertex (no Location since split vertex is new).
  const BRepGraph_VertexId aSplitVertexDefId = theSplitVertex;

  // Create start vertex ref entry for SubA (copy from original edge's start vertex ref).
  BRepGraph_VertexRefId aSubAStartRefId;
  if (aOrigStartVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRef& aOrigStartRef =
      myGraph->myData->myIncStorage.VertexRef(aOrigStartVertexRefId);
    const BRepGraph_VertexId aOrigStartVertexId = aOrigStartRef.VertexDefId;
    const TopAbs_Orientation aOrigStartOri      = aOrigStartRef.Orientation;
    const TopLoc_Location    aOrigStartLoc      = aOrigStartRef.LocalLocation;

    const BRepGraph_VertexRefId aSubAStartRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubAStartRef = aMutStorage.ChangeVertexRef(aSubAStartRefId2);
    aSubAStartRef.ParentId = BRepGraph_NodeId(aSubAId);
    aSubAStartRef.VertexDefId = aOrigStartVertexId;
    aSubAStartRef.Orientation              = aOrigStartOri;
    aSubAStartRef.LocalLocation            = aOrigStartLoc;
    myGraph->allocateRefUID(aSubAStartRefId2);
    aSubAStartRefId = aSubAStartRefId2;
  }

  // Create end vertex ref entry for SubA (split vertex, REVERSED).
  BRepGraph_VertexRefId aSubAEndRefId;
  if (aSplitVertexDefId.IsValid())
  {
    const BRepGraph_VertexRefId aSubAEndRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubAEndRef = aMutStorage.ChangeVertexRef(aSubAEndRefId2);
    aSubAEndRef.ParentId = BRepGraph_NodeId(aSubAId);
    aSubAEndRef.VertexDefId = aSplitVertexDefId;
    aSubAEndRef.Orientation              = TopAbs_REVERSED;
    myGraph->allocateRefUID(aSubAEndRefId2);
    aSubAEndRefId = aSubAEndRefId2;
  }

  // Create start vertex ref entry for SubB (split vertex, FORWARD).
  BRepGraph_VertexRefId aSubBStartRefId;
  if (aSplitVertexDefId.IsValid())
  {
    const BRepGraph_VertexRefId aSubBStartRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubBStartRef = aMutStorage.ChangeVertexRef(aSubBStartRefId2);
    aSubBStartRef.ParentId = BRepGraph_NodeId(aSubBId);
    aSubBStartRef.VertexDefId = aSplitVertexDefId;
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
    const BRepGraph_VertexId aOrigEndVertexId = aOrigEndRef.VertexDefId;
    const TopAbs_Orientation aOrigEndOri      = aOrigEndRef.Orientation;
    const TopLoc_Location    aOrigEndLoc      = aOrigEndRef.LocalLocation;

    const BRepGraph_VertexRefId aSubBEndRefId2(aMutStorage.NbVertexRefs());
    aMutStorage.AppendVertexRef();
    BRepGraphInc::VertexRef& aSubBEndRef = aMutStorage.ChangeVertexRef(aSubBEndRefId2);
    aSubBEndRef.ParentId = BRepGraph_NodeId(aSubBId);
    aSubBEndRef.VertexDefId = aOrigEndVertexId;
    aSubBEndRef.Orientation              = aOrigEndOri;
    aSubBEndRef.LocalLocation            = aOrigEndLoc;
    myGraph->allocateRefUID(aSubBEndRefId2);
    aSubBEndRefId = aSubBEndRefId2;
  }

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraphInc::EdgeDef& aSubA = myGraph->myData->myIncStorage.ChangeEdge(aSubAId);
    initSubEdgeEntity(aSubA,
                      aOrigCurve3DRepId,
                      aOrigTolerance,
                      aOrigSameParameter,
                      aSubAStartRefId,
                      aSubAEndRefId,
                      aOrigParamFirst,
                      theSplitParam);
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraphInc::EdgeDef& aSubB = myGraph->myData->myIncStorage.ChangeEdge(aSubBId);
    initSubEdgeEntity(aSubB,
                      aOrigCurve3DRepId,
                      aOrigTolerance,
                      aOrigSameParameter,
                      aSubBStartRefId,
                      aSubBEndRefId,
                      theSplitParam,
                      aOrigParamLast);
  }

  myGraph->allocateUID(theSubA);
  myGraph->allocateUID(theSubB);

  // Rebuild CoEdge incidence in a single coherent pass. The previous two-pass
  // design (one wire walk, one reverse-index walk) produced incomplete SubB
  // CoEdges under wire membership, orphan PCurve-bearing CoEdges off-wire, and
  // did not preserve SeamPairId linkage between new sub-pairs. The unified
  // pass below: snapshots all original CoEdges (forward + seam partners),
  // allocates two fully-initialised CoEdges per original, rebuilds SeamPairId
  // on the new pairs, rebuilds wire CoEdgeRefIds, and retires each original
  // CoEdge with a reverse-index unbind.
  NCollection_DynamicArray<BRepGraph_FaceId> aOrigFaces;
  {
    BRepGraphInc_Storage&      aStorage = myGraph->myData->myIncStorage;
    BRepGraphInc_ReverseIndex& aRevIdx  = aStorage.ChangeReverseIndex();

    // Step 1: snapshot old CoEdge ids before any mutation.
    NCollection_DynamicArray<BRepGraph_CoEdgeId> anOrigCoEdgeIds;
    if (const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aSrc =
          aStorage.ReverseIndex().CoEdgesOfEdge(theEdgeEntity))
    {
      for (const BRepGraph_CoEdgeId& anId : *aSrc)
        anOrigCoEdgeIds.Append(anId);
    }
    const uint32_t aNbOrig = static_cast<uint32_t>(anOrigCoEdgeIds.Size());

    // Step 2: allocate SubA-CE + SubB-CE per original with full payload.
    NCollection_DataMap<BRepGraph_CoEdgeId, int> aIdxOf;
    NCollection_DynamicArray<BRepGraph_CoEdgeId> aNewACoEdgeIds;
    NCollection_DynamicArray<BRepGraph_CoEdgeId> aNewBCoEdgeIds;
    const double                                 aParamRange = aOrigParamLast - aOrigParamFirst;
    for (uint32_t i = 0; i < aNbOrig; ++i)
    {
      const BRepGraph_CoEdgeId      aOldId = anOrigCoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraphInc::CoEdgeDef aOld   = aStorage.CoEdge(aOldId); // by-value snapshot

      double aPCSplit;
      if (aOrigSameRange)
      {
        aPCSplit = theSplitParam;
      }
      else
      {
        const double aPCRange = aOld.ParamLast - aOld.ParamFirst;
        if (aParamRange > 0.0)
          aPCSplit = aOld.ParamFirst + ((theSplitParam - aOrigParamFirst) / aParamRange) * aPCRange;
        else
          aPCSplit = 0.5 * (aOld.ParamFirst + aOld.ParamLast);
      }

      const BRepGraph_CoEdgeId aNewA = aStorage.AppendCoEdge();
      {
        BRepGraphInc::CoEdgeDef& aNewACE = aStorage.ChangeCoEdge(aNewA);
        initSubCoEdgeEntity(aNewACE,
                            theSubA,
                            aOld.FaceDefId,
                            aOld.Orientation,
                            aOld.Curve2DRepId,
                            aOld.ParamFirst,
                            aPCSplit,
                            aOld.Continuity);
        aNewACE.SeamContinuity = aOld.SeamContinuity;
        // UV at the split-point end is left default (0,0); callers needing
        // the exact point should evaluate the Curve2DRep at aPCSplit.
        aNewACE.UV1               = aOld.UV1;
        aNewACE.Polygon2DRepId    = aOld.Polygon2DRepId;
        aNewACE.PolygonOnTriRepId = aOld.PolygonOnTriRepId;
      }
      myGraph->allocateUID(aNewA);

      const BRepGraph_CoEdgeId aNewB = aStorage.AppendCoEdge();
      {
        BRepGraphInc::CoEdgeDef& aNewBCE = aStorage.ChangeCoEdge(aNewB);
        initSubCoEdgeEntity(aNewBCE,
                            theSubB,
                            aOld.FaceDefId,
                            aOld.Orientation,
                            aOld.Curve2DRepId,
                            aPCSplit,
                            aOld.ParamLast,
                            aOld.Continuity);
        aNewBCE.SeamContinuity = aOld.SeamContinuity;
        // UV at the split-point start is left default (0,0) - see aNewACE.
        aNewBCE.UV2               = aOld.UV2;
        aNewBCE.Polygon2DRepId    = aOld.Polygon2DRepId;
        aNewBCE.PolygonOnTriRepId = aOld.PolygonOnTriRepId;
      }
      myGraph->allocateUID(aNewB);

      aNewACoEdgeIds.Append(aNewA);
      aNewBCoEdgeIds.Append(aNewB);
      aIdxOf.Bind(aOldId, i);
      if (aOld.FaceDefId.IsValid())
        aOrigFaces.Append(aOld.FaceDefId);
    }

    // Step 3: re-establish SeamPairId linkage on new pairs. If the old
    // partner CoEdge is also in our map (true for intra-edge seam pairs),
    // wire SubA<->partnerSubA and SubB<->partnerSubB; otherwise leave the
    // new CoEdges unpaired - an orphan SeamPairId would point into a dead
    // slot.
    for (uint32_t i = 0; i < aNbOrig; ++i)
    {
      const BRepGraph_CoEdgeId aOldId       = anOrigCoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraph_CoEdgeId aOldSeamPair = aStorage.CoEdge(aOldId).SeamPairId;
      if (!aOldSeamPair.IsValid() || !aIdxOf.IsBound(aOldSeamPair))
        continue;
      int aJ = -1;
      aIdxOf.Find(aOldSeamPair, aJ);
      aStorage.ChangeCoEdge(aNewACoEdgeIds.Value(static_cast<size_t>(i))).SeamPairId =
        aNewACoEdgeIds.Value(aJ);
      aStorage.ChangeCoEdge(aNewBCoEdgeIds.Value(static_cast<size_t>(i))).SeamPairId =
        aNewBCoEdgeIds.Value(aJ);
    }

    // Step 4: rebuild wire CoEdgeRefIds (snapshot-before-mutate). For each
    // wire containing the original edge, rebind its existing CoEdgeRef to
    // the new SubA-CE in place and insert a fresh CoEdgeRef for SubB-CE
    // immediately after. Track insertion offset so that subsequent inserts
    // land at the correct position in the growing list.
    for (const BRepGraph_WireId& aWireId : aOrigWires)
    {
      NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aSnapshot;
      {
        const BRepGraphInc::WireDef& aWireDef = aStorage.Wire(aWireId);
        for (NCollection_DynamicArray<BRepGraph_CoEdgeRefId>::Iterator aRefIt(
               aWireDef.CoEdgeRefIds);
             aRefIt.More();
             aRefIt.Next())
        {
          aSnapshot.Append(aRefIt.Value());
        }
      }

      size_t anInsertOffset = 0;
      for (size_t i = 0; i < aSnapshot.Size(); ++i)
      {
        const BRepGraph_CoEdgeRefId    aRefId  = aSnapshot.Value(i);
        const BRepGraphInc::CoEdgeRef& aRefEnt = aStorage.CoEdgeRef(aRefId);
        // Skip refs the caller has already retired: their slot stays in the
        // wire's list but is filtered out of any live walk. Split must not
        // revive them.
        if (aRefEnt.IsRemoved)
          continue;
        const BRepGraph_CoEdgeId aOldCEId = aRefEnt.CoEdgeDefId;
        int                      aJ       = -1;
        if (!aIdxOf.Find(aOldCEId, aJ))
          continue;
        const BRepGraph_CoEdgeId aNewA = aNewACoEdgeIds.Value(aJ);
        const BRepGraph_CoEdgeId aNewB = aNewBCoEdgeIds.Value(aJ);
        const TopLoc_Location    aLoc  = aStorage.CoEdgeRef(aRefId).LocalLocation;

        // Rebind existing ref in-place to SubA-CE (preserves order slot).
        aStorage.ChangeCoEdgeRef(aRefId).CoEdgeDefId = aNewA;

        // Allocate fresh ref for SubB-CE and insert into wire after aRefId.
        const BRepGraph_CoEdgeRefId aNewRefId = aStorage.AppendCoEdgeRef();
        {
          BRepGraphInc::CoEdgeRef& aNewRef = aStorage.ChangeCoEdgeRef(aNewRefId);
          aNewRef.ParentId = aWireId;
          aNewRef.CoEdgeDefId = aNewB;
          aNewRef.LocalLocation            = aLoc;
        }
        myGraph->allocateRefUID(aNewRefId);

        const size_t           aPos     = i + anInsertOffset;
        BRepGraphInc::WireDef& aWireEnt = aStorage.ChangeWire(aWireId);
        auto&                  aCoEdgeRefIds = aWireEnt.CoEdgeRefIds;
        if (aPos < aCoEdgeRefIds.Size())
          aCoEdgeRefIds.InsertAfter(aPos, aNewRefId);
        else
          aCoEdgeRefIds.Append(aNewRefId);
        ++anInsertOffset;

        aRevIdx.UnbindCoEdgeFromWire(aOldCEId, aWireId);
        aRevIdx.BindCoEdgeToWire(aNewA, aWireId);
        aRevIdx.BindCoEdgeToWire(aNewB, aWireId);
      }
    }

    // Step 5: retire the original CoEdges and rebuild the Edge->CoEdge
    // reverse index so CoEdgesOfEdge(theSubA/B) resolves to the new pair.
    for (uint32_t i = 0; i < aNbOrig; ++i)
    {
      const BRepGraph_CoEdgeId aOldId = anOrigCoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraph_CoEdgeId aNewA  = aNewACoEdgeIds.Value(static_cast<size_t>(i));
      const BRepGraph_CoEdgeId aNewB  = aNewBCoEdgeIds.Value(static_cast<size_t>(i));
      aStorage.MarkRemoved(BRepGraph_NodeId(aOldId));
      aRevIdx.UnbindEdgeFromCoEdge(theEdgeEntity, aOldId);
      aRevIdx.BindEdgeToCoEdge(theSubA, aNewA);
      aRevIdx.BindEdgeToCoEdge(theSubB, aNewB);
    }

    // Step 6: retire the original edge's vertex refs. Their ParentId
    // points at the about-to-be-removed edge; leaving them live would
    // trip the "Orphan VertexRef: ParentId is not a live Edge" Audit
    // rule. Internal vertex refs are dropped rather than reparented to
    // SubA/SubB based on parameter - reparenting is a follow-up when a
    // caller surfaces that needs it.
    if (aOrigStartVertexRefId.IsValid())
      aStorage.MarkRemovedRef(aOrigStartVertexRefId);
    if (aOrigEndVertexRefId.IsValid())
      aStorage.MarkRemovedRef(aOrigEndVertexRefId);
    // Defensive retirement of internal vertex refs. No existing test
    // populates InternalVertexRefIds before a Split, so the loop is a
    // no-op under the current suite; it guards against orphan refs the
    // moment a caller does populate them.
    {
      const BRepGraphInc::EdgeDef& aOrigEdgeRef = aStorage.Edge(theEdgeEntity);
      for (NCollection_DynamicArray<BRepGraph_VertexRefId>::Iterator anIntRefIt(
             aOrigEdgeRef.InternalVertexRefIds);
           anIntRefIt.More();
           anIntRefIt.Next())
      {
        const BRepGraph_VertexRefId anIntRef = anIntRefIt.Value();
        if (anIntRef.IsValid())
          aStorage.MarkRemovedRef(anIntRef);
      }
    }

    // Mark original edge as removed.
    aStorage.MarkRemoved(theEdgeEntity);
  }

  // Update edge-to-wire reverse index incrementally.
  BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ChangeReverseIndex();
  for (const BRepGraph_WireId& aWireId : aOrigWires)
  {
    aRevIdx.UnbindEdgeFromWire(BRepGraph_EdgeId(theEdgeEntity), aWireId);
    aRevIdx.BindEdgeToWire(aSubAId, aWireId);
    aRevIdx.BindEdgeToWire(aSubBId, aWireId);
    myGraph->markModified(aWireId);
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc_Storage&  aStorageRef = myGraph->myData->myIncStorage;
    const BRepGraphInc::EdgeDef& aSubAEnt    = aStorageRef.Edge(aSubAId);
    const BRepGraphInc::EdgeDef& aSubBEnt    = aStorageRef.Edge(aSubBId);
    BRepGraphInc_ReverseIndex&   aRevIdxMut  = myGraph->myData->myIncStorage.ChangeReverseIndex();

    // Resolve vertex def ids from the sub-edge ref entries.
    if (aSubAEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRef(aSubAEnt.StartVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, aSubAId);
    }
    if (aSubAEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId = aStorageRef.VertexRef(aSubAEnt.EndVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, aSubAId);
    }
    if (aSubBEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRef(aSubBEnt.StartVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, aSubBId);
    }
    if (aSubBEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId = aStorageRef.VertexRef(aSubBEnt.EndVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, aSubBId);
    }

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigStartVertexDefId, BRepGraph_EdgeId(theEdgeEntity));
    if (aOrigEndVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigEndVertexDefId, BRepGraph_EdgeId(theEdgeEntity));

    // Edge-to-face: unbind the original edge and bind both sub-edges for
    // each face the original edge touched. aOrigFaces was captured during
    // the CoEdge rebuild above (may contain duplicates for seam edges - the
    // Bind/Unbind helpers are dedup-safe).
    for (const BRepGraph_FaceId& aFaceId : aOrigFaces)
    {
      aRevIdxMut.UnbindEdgeFromFace(BRepGraph_EdgeId(theEdgeEntity), aFaceId);
      aRevIdxMut.BindEdgeToFace(aSubAId, aFaceId);
      aRevIdxMut.BindEdgeToFace(aSubBId, aFaceId);
    }
  }

  myGraph->markModified(theEdgeEntity);
  myGraph->markModified(theSubA);
  myGraph->markModified(theSubB);

  Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                       "Split: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::ReplaceEdge(const BRepGraph_WireId theWireDefId,
                                                 const BRepGraph_EdgeId theOldEdgeEntity,
                                                 const BRepGraph_EdgeId theNewEdgeEntity,
                                                 const bool             theReversed)
{
  Standard_ASSERT_RETURN(theWireDefId.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "ReplaceEdge: wire index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theOldEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceEdge: old edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theNewEdgeEntity.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceEdge: new edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(!myGraph->myData->myIncStorage.Edge(theNewEdgeEntity).IsRemoved,
                         "ReplaceEdge: replacement edge must be active",
                         Standard_VOID_RETURN);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Update incidence by scanning wire-owned coedge ref entries.
  for (BRepGraph_RefsCoEdgeOfWire aRefIt(*myGraph, theWireDefId); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraphInc::CoEdgeRef& aRef         = aStorage.CoEdgeRef(aRefIt.CurrentId());
    const BRepGraph_CoEdgeId       aCoEdgeDefId = aRef.CoEdgeDefId;
    if (!aCoEdgeDefId.IsValid(aStorage.NbCoEdges()))
      continue;

    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(aCoEdgeDefId);
    if (aCoEdge.EdgeDefId == theOldEdgeEntity)
    {
      aCoEdge.EdgeDefId = theNewEdgeEntity;
      if (theReversed)
      {
        aCoEdge.Orientation = TopAbs::Reverse(aCoEdge.Orientation);

        // Reverse the PCurve to match the canonical edge's parametric direction.
        // When the canonical edge has opposite vertex ordering (isReversed=true),
        // the PCurve was defined for the old edge's direction and must be reversed
        // so it maps parameter-space coordinates relative to the canonical edge.
        if (aCoEdge.Curve2DRepId.IsValid())
        {
          BRepGraphInc::Curve2DRep& aCurveRep = aStorage.ChangeCurve2DRep(aCoEdge.Curve2DRepId);
          if (!aCurveRep.Curve.IsNull())
          {
            const double anOldParamFirst = aCoEdge.ParamFirst;
            const double anOldParamLast  = aCoEdge.ParamLast;
            aCoEdge.ParamFirst           = aCurveRep.Curve->ReversedParameter(anOldParamLast);
            aCoEdge.ParamLast            = aCurveRep.Curve->ReversedParameter(anOldParamFirst);
            aCurveRep.Curve              = aCurveRep.Curve->Reversed();
            std::swap(aCoEdge.UV1, aCoEdge.UV2);
          }
        }
      }

      // Update reverse indices incrementally.
      BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ChangeReverseIndex();
      aRevIdx.ReplaceEdgeInWireMap(theOldEdgeEntity, theNewEdgeEntity, theWireDefId);
      aRevIdx.UnbindEdgeFromCoEdge(theOldEdgeEntity, aCoEdgeDefId);
      aRevIdx.BindEdgeToCoEdge(theNewEdgeEntity, aCoEdgeDefId);

      // Update edge-to-face: bind new edge, unbind old edge for all faces of this wire.
      const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(theWireDefId);
      if (aFaces != nullptr)
      {
        for (const BRepGraph_FaceId& aFaceId : *aFaces)
        {
          aRevIdx.BindEdgeToFace(theNewEdgeEntity, aFaceId);
          aRevIdx.UnbindEdgeFromFace(theOldEdgeEntity, aFaceId);
        }
      }
    }
  }

  myGraph->markModified(theWireDefId);

  // Validate reverse index only when not in deferred mode.
  // In deferred mode (batch sewing, parallel mutations), intermediate states
  // may have temporarily stale entries; validation runs at CommitMutation().
  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "ReplaceEdge: post-mutation reverse index inconsistency");
  }
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

  const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(theChildRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theCompoundDefId)
      || !aRef.ChildDefId.IsValid() || !isActiveNode(aStorage, aRef.ChildDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_ChildRefId>& aCompoundRefIds =
    aStorage.ChangeCompound(theCompoundDefId).ChildRefIds;
  const BRepGraph_NodeId aChildDefId = aRef.ChildDefId;
  if (!detachOrderedParentRef(*myGraph, theChildRefId, aCompoundRefIds, aChildDefId, true))
  {
    return false;
  }

  aStorage.ChangeReverseIndex().UnbindCompoundChild(aChildDefId, theCompoundDefId);
  myGraph->markModified(theCompoundDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveChild: post-mutation reverse index inconsistency");
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
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theProductDefId)
      || !aRef.OccurrenceDefId.IsValid() || !isActiveNode(aStorage, aRef.OccurrenceDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& aProductRefIds =
    aStorage.ChangeProduct(theProductDefId).OccurrenceRefIds;
  const BRepGraph_OccurrenceId anOccDefId = aRef.OccurrenceDefId;
  // Capture child def id before detach (myProductToOccurrences is keyed by ChildDefId).
  const BRepGraph_NodeId aChildDefIdSnap = aStorage.Occurrence(anOccDefId).ChildDefId;
  if (!detachOrderedParentRef(*myGraph,
                              theOccurrenceRefId,
                              aProductRefIds,
                              BRepGraph_NodeId(anOccDefId),
                              true))
  {
    return false;
  }

  if (aChildDefIdSnap.IsValid() && aChildDefIdSnap.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    aStorage.ChangeReverseIndex().UnbindProductOccurrence(
      anOccDefId,
      BRepGraph_ProductId::FromNodeId(aChildDefIdSnap));
  }

  // Check if the removed occurrence's child product was its last parent;
  // if so, re-add the child product to root products.
  const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(anOccDefId);
  if (anOccDef.ChildDefId.IsValid()
      && anOccDef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    const BRepGraph_ProductId aChildProduct = BRepGraph_ProductId::FromNodeId(anOccDef.ChildDefId);
    if (aChildProduct.IsValid(aStorage.NbProducts()) && !aStorage.Product(aChildProduct).IsRemoved
        && !hasAnyActiveUsage(*myGraph, anOccDef.ChildDefId))
    {
      myGraph->myData->myRootProductIds.Append(aChildProduct);
    }
  }

  myGraph->markModified(theProductDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveOccurrence: post-mutation reverse index inconsistency");
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

  // Find the shape-root occurrence (the one whose ChildDefId is a topology node).
  BRepGraphInc::ProductDef& aProduct = aStorage.ChangeProduct(theProductDefId);
  BRepGraph_OccurrenceRefId aShapeRootRefId;
  BRepGraph_NodeId          aShapeRoot;
  for (const BRepGraph_OccurrenceRefId& aRefId : aProduct.OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& aOccRef = aStorage.OccurrenceRef(aRefId);
    if (aOccRef.IsRemoved)
      continue;
    const BRepGraphInc::OccurrenceDef& anOcc = aStorage.Occurrence(aOccRef.OccurrenceDefId);
    if (!anOcc.IsRemoved && BRepGraph_NodeId::IsTopologyKind(anOcc.ChildDefId.NodeKind))
    {
      aShapeRootRefId = aRefId;
      aShapeRoot      = anOcc.ChildDefId;
      break;
    }
  }

  if (!aShapeRootRefId.IsValid() || !aShapeRoot.IsValid() || !isActiveNode(aStorage, aShapeRoot))
  {
    return false;
  }

  // Detach the shape-root occurrence from the product.
  const BRepGraph_OccurrenceId aShapeRootOccId =
    aStorage.OccurrenceRef(aShapeRootRefId).OccurrenceDefId;
  detachOrderedParentRef(*myGraph,
                         aShapeRootRefId,
                         aProduct.OccurrenceRefIds,
                         BRepGraph_NodeId(aShapeRootOccId),
                         true);

  // No reverse-index unbind: shape-root occurrence's ChildDefId is topology, not a product.

  if (!hasAnyActiveUsage(*myGraph, aShapeRoot))
  {
    myGraph->Editor().Gen().RemoveSubgraph(aShapeRoot);
  }

  myGraph->markModified(theProductDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveShapeRoot: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ShellOps::RemoveChild(const BRepGraph_ShellId    theShellDefId,
                                                  const BRepGraph_ChildRefId theChildRefId)
{
  Standard_ASSERT_RETURN(theShellDefId.IsValid(myGraph->myData->myIncStorage.NbShells()),
                         "RemoveChild: shell index is out of range",
                         false);
  Standard_ASSERT_RETURN(theChildRefId.IsValid(myGraph->myData->myIncStorage.NbChildRefs()),
                         "RemoveChild: child ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theShellDefId))
  {
    return false;
  }

  const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(theChildRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theShellDefId)
      || !aRef.ChildDefId.IsValid() || !isActiveNode(aStorage, aRef.ChildDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_ChildRefId>& aShellRefIds =
    aStorage.ChangeShell(theShellDefId).AuxChildRefIds;
  if (!detachOrderedParentRef(*myGraph, theChildRefId, aShellRefIds, aRef.ChildDefId, true))
  {
    return false;
  }

  myGraph->markModified(theShellDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveChild: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::SolidOps::RemoveChild(const BRepGraph_SolidId    theSolidDefId,
                                                  const BRepGraph_ChildRefId theChildRefId)
{
  Standard_ASSERT_RETURN(theSolidDefId.IsValid(myGraph->myData->myIncStorage.NbSolids()),
                         "RemoveChild: solid index is out of range",
                         false);
  Standard_ASSERT_RETURN(theChildRefId.IsValid(myGraph->myData->myIncStorage.NbChildRefs()),
                         "RemoveChild: child ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theSolidDefId))
  {
    return false;
  }

  const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(theChildRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theSolidDefId)
      || !aRef.ChildDefId.IsValid() || !isActiveNode(aStorage, aRef.ChildDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_ChildRefId>& aSolidRefIds =
    aStorage.ChangeSolid(theSolidDefId).AuxChildRefIds;
  if (!detachOrderedParentRef(*myGraph, theChildRefId, aSolidRefIds, aRef.ChildDefId, true))
  {
    return false;
  }

  myGraph->markModified(theSolidDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveChild: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::CompSolidOps::RemoveSolid(const BRepGraph_CompSolidId theCompSolidDefId,
                                                      const BRepGraph_SolidRefId  theSolidRefId)
{
  Standard_ASSERT_RETURN(theCompSolidDefId.IsValid(myGraph->myData->myIncStorage.NbCompSolids()),
                         "RemoveSolid: compsolid index is out of range",
                         false);
  Standard_ASSERT_RETURN(theSolidRefId.IsValid(myGraph->myData->myIncStorage.NbSolidRefs()),
                         "RemoveSolid: solid ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theCompSolidDefId))
  {
    return false;
  }

  const BRepGraphInc::SolidRef& aRef = aStorage.SolidRef(theSolidRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theCompSolidDefId)
      || !aRef.SolidDefId.IsValid() || !isActiveNode(aStorage, aRef.SolidDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_SolidRefId>& aCompSolidRefIds =
    aStorage.ChangeCompSolid(theCompSolidDefId).SolidRefIds;
  const BRepGraph_SolidId aSolidDefId = aRef.SolidDefId;
  if (!detachOrderedParentRef(*myGraph,
                              theSolidRefId,
                              aCompSolidRefIds,
                              BRepGraph_NodeId(aSolidDefId),
                              true))
  {
    return false;
  }

  aStorage.ChangeReverseIndex().UnbindSolidFromCompSolid(aSolidDefId, theCompSolidDefId);
  myGraph->markModified(theCompSolidDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveSolid: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::SolidOps::RemoveShell(const BRepGraph_SolidId    theSolidDefId,
                                                  const BRepGraph_ShellRefId theShellRefId)
{
  Standard_ASSERT_RETURN(theSolidDefId.IsValid(myGraph->myData->myIncStorage.NbSolids()),
                         "RemoveShell: solid index is out of range",
                         false);
  Standard_ASSERT_RETURN(theShellRefId.IsValid(myGraph->myData->myIncStorage.NbShellRefs()),
                         "RemoveShell: shell ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theSolidDefId))
  {
    return false;
  }

  const BRepGraphInc::ShellRef& aRef = aStorage.ShellRef(theShellRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theSolidDefId)
      || !aRef.ShellDefId.IsValid() || !isActiveNode(aStorage, aRef.ShellDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_ShellRefId>& aSolidRefIds =
    aStorage.ChangeSolid(theSolidDefId).ShellRefIds;
  const BRepGraph_ShellId aShellDefId = aRef.ShellDefId;
  if (!detachOrderedParentRef(*myGraph,
                              theShellRefId,
                              aSolidRefIds,
                              BRepGraph_NodeId(aShellDefId),
                              true))
  {
    return false;
  }

  aStorage.ChangeReverseIndex().UnbindShellFromSolid(aShellDefId, theSolidDefId);
  myGraph->markModified(theSolidDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveShell: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::ShellOps::RemoveFace(const BRepGraph_ShellId   theShellDefId,
                                                 const BRepGraph_FaceRefId theFaceRefId)
{
  Standard_ASSERT_RETURN(theShellDefId.IsValid(myGraph->myData->myIncStorage.NbShells()),
                         "RemoveFace: shell index is out of range",
                         false);
  Standard_ASSERT_RETURN(theFaceRefId.IsValid(myGraph->myData->myIncStorage.NbFaceRefs()),
                         "RemoveFace: face ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theShellDefId))
  {
    return false;
  }

  const BRepGraphInc::FaceRef& aRef = aStorage.FaceRef(theFaceRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theShellDefId)
      || !aRef.FaceDefId.IsValid() || !isActiveNode(aStorage, aRef.FaceDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_FaceRefId>& aShellRefIds =
    aStorage.ChangeShell(theShellDefId).FaceRefIds;
  const BRepGraph_FaceId aFaceDefId = aRef.FaceDefId;
  if (!detachOrderedParentRef(*myGraph,
                              theFaceRefId,
                              aShellRefIds,
                              BRepGraph_NodeId(aFaceDefId),
                              true))
  {
    return false;
  }

  aStorage.ChangeReverseIndex().UnbindFaceFromShell(aFaceDefId, theShellDefId);
  myGraph->markModified(theShellDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveFace: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::FaceOps::RemoveVertex(const BRepGraph_FaceId      theFaceDefId,
                                                  const BRepGraph_VertexRefId theVertexRefId)
{
  Standard_ASSERT_RETURN(theFaceDefId.IsValid(myGraph->myData->myIncStorage.NbFaces()),
                         "RemoveVertex: face index is out of range",
                         false);
  Standard_ASSERT_RETURN(theVertexRefId.IsValid(myGraph->myData->myIncStorage.NbVertexRefs()),
                         "RemoveVertex: vertex ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theFaceDefId))
  {
    return false;
  }

  const BRepGraphInc::VertexRef& aRef = aStorage.VertexRef(theVertexRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theFaceDefId)
      || !aRef.VertexDefId.IsValid() || !isActiveNode(aStorage, aRef.VertexDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_VertexRefId>& aFaceRefIds =
    aStorage.ChangeFace(theFaceDefId).VertexRefIds;
  if (!detachOrderedParentRef(*myGraph,
                              theVertexRefId,
                              aFaceRefIds,
                              BRepGraph_NodeId(aRef.VertexDefId),
                              true))
  {
    return false;
  }

  myGraph->markModified(theFaceDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveVertex: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::FaceOps::RemoveWire(const BRepGraph_FaceId    theFaceDefId,
                                                const BRepGraph_WireRefId theWireRefId)
{
  Standard_ASSERT_RETURN(theFaceDefId.IsValid(myGraph->myData->myIncStorage.NbFaces()),
                         "RemoveWire: face index is out of range",
                         false);
  Standard_ASSERT_RETURN(theWireRefId.IsValid(myGraph->myData->myIncStorage.NbWireRefs()),
                         "RemoveWire: wire ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theFaceDefId))
  {
    return false;
  }

  const BRepGraphInc::WireRef& aRef = aStorage.WireRef(theWireRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theFaceDefId) || !aRef.WireDefId.IsValid()
      || !isActiveNode(aStorage, aRef.WireDefId))
  {
    return false;
  }

  NCollection_DynamicArray<BRepGraph_WireRefId>& aFaceRefIds =
    aStorage.ChangeFace(theFaceDefId).WireRefIds;
  const BRepGraph_WireId aWireDefId = aRef.WireDefId;
  if (!detachOrderedParentRef(*myGraph,
                              theWireRefId,
                              aFaceRefIds,
                              BRepGraph_NodeId(aWireDefId),
                              true))
  {
    return false;
  }

  aStorage.ChangeReverseIndex().UnbindWireFromFace(aWireDefId, theFaceDefId);
  myGraph->markModified(theFaceDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveWire: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::WireOps::RemoveCoEdge(const BRepGraph_WireId      theWireDefId,
                                                  const BRepGraph_CoEdgeRefId theCoEdgeRefId)
{
  Standard_ASSERT_RETURN(theWireDefId.IsValid(myGraph->myData->myIncStorage.NbWires()),
                         "RemoveCoEdge: wire index is out of range",
                         false);
  Standard_ASSERT_RETURN(theCoEdgeRefId.IsValid(myGraph->myData->myIncStorage.NbCoEdgeRefs()),
                         "RemoveCoEdge: coedge ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theWireDefId))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeRef& aRef = aStorage.CoEdgeRef(theCoEdgeRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theWireDefId)
      || !aRef.CoEdgeDefId.IsValid() || !isActiveNode(aStorage, aRef.CoEdgeDefId))
  {
    return false;
  }

  const BRepGraph_CoEdgeId       aCoEdgeEntity = aRef.CoEdgeDefId;
  const BRepGraphInc::CoEdgeDef& aCoEdgeDef    = aStorage.CoEdge(aCoEdgeEntity);
  const BRepGraph_EdgeId         anEdgeId      = aCoEdgeDef.EdgeDefId;
  const BRepGraph_FaceId         aFaceId       = aCoEdgeDef.FaceDefId;

  NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& aWireRefIds =
    aStorage.ChangeWire(theWireDefId).CoEdgeRefIds;
  if (!detachOrderedParentRef(*myGraph,
                              theCoEdgeRefId,
                              aWireRefIds,
                              BRepGraph_NodeId(aCoEdgeEntity),
                              true))
  {
    return false;
  }

  const bool hasRemainingCoEdgeUsage = hasAnyActiveUsage(*myGraph, aCoEdgeEntity);

  auto hasRemainingEdgeUseInWire = [&]() {
    if (!anEdgeId.IsValid())
    {
      return false;
    }

    for (BRepGraph_RefsCoEdgeOfWire aRefIt(*myGraph, theWireDefId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aRef = aStorage.CoEdgeRef(aRefIt.CurrentId());
      if (!aRef.CoEdgeDefId.IsValid())
      {
        continue;
      }

      const BRepGraphInc::CoEdgeDef& aCandidate = aStorage.CoEdge(aRef.CoEdgeDefId);
      if (!aCandidate.IsRemoved && aCandidate.EdgeDefId == anEdgeId)
      {
        return true;
      }
    }
    return false;
  };

  auto hasRemainingEdgeUseOnFace = [&]() {
    if (!anEdgeId.IsValid() || !aFaceId.IsValid())
    {
      return false;
    }

    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aStorage.ReverseIndex().CoEdgesOfEdgeRef(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCandidateId : aCoEdges)
    {
      if (!aCandidateId.IsValid())
      {
        continue;
      }
      if (aCandidateId == aCoEdgeEntity && !hasRemainingCoEdgeUsage)
      {
        continue;
      }

      const BRepGraphInc::CoEdgeDef& aCandidate = aStorage.CoEdge(aCandidateId);
      if (!aCandidate.IsRemoved && aCandidate.FaceDefId == aFaceId)
      {
        return true;
      }
    }
    return false;
  };

  BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ChangeReverseIndex();
  aRevIdx.UnbindCoEdgeFromWire(aCoEdgeEntity, theWireDefId);

  if (anEdgeId.IsValid() && !hasRemainingEdgeUseInWire())
  {
    aRevIdx.UnbindEdgeFromWire(anEdgeId, theWireDefId);
  }

  if (anEdgeId.IsValid() && !hasRemainingCoEdgeUsage)
  {
    aRevIdx.UnbindEdgeFromCoEdge(anEdgeId, aCoEdgeEntity);
    if (aFaceId.IsValid() && !hasRemainingEdgeUseOnFace())
    {
      aRevIdx.UnbindEdgeFromFace(anEdgeId, aFaceId);
    }
  }

  myGraph->markModified(theWireDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveCoEdge: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

bool BRepGraph::EditorView::EdgeOps::RemoveVertex(const BRepGraph_EdgeId      theEdgeDefId,
                                                  const BRepGraph_VertexRefId theVertexRefId)
{
  Standard_ASSERT_RETURN(theEdgeDefId.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "RemoveVertex: edge index is out of range",
                         false);
  Standard_ASSERT_RETURN(theVertexRefId.IsValid(myGraph->myData->myIncStorage.NbVertexRefs()),
                         "RemoveVertex: vertex ref index is out of range",
                         false);

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdgeDefId))
  {
    return false;
  }

  const BRepGraphInc::VertexRef& aRef = aStorage.VertexRef(theVertexRefId);
  if (aRef.IsRemoved || aRef.ParentId != BRepGraph_NodeId(theEdgeDefId)
      || !aRef.VertexDefId.IsValid() || !isActiveNode(aStorage, aRef.VertexDefId))
  {
    return false;
  }

  BRepGraphInc::EdgeDef& aEdge   = aStorage.ChangeEdge(theEdgeDefId);
  bool                   isFound = false;
  bool                   isFixed = false;
  if (aEdge.StartVertexRefId == theVertexRefId)
  {
    if (!myGraph->Editor().Gen().RemoveRef(theVertexRefId))
    {
      return false;
    }
    aEdge.StartVertexRefId = BRepGraph_VertexRefId();
    isFound                = true;
    isFixed                = true;
  }
  else if (aEdge.EndVertexRefId == theVertexRefId)
  {
    if (!myGraph->Editor().Gen().RemoveRef(theVertexRefId))
    {
      return false;
    }
    aEdge.EndVertexRefId = BRepGraph_VertexRefId();
    isFound              = true;
    isFixed              = true;
  }
  else
  {
    NCollection_DynamicArray<BRepGraph_VertexRefId>& anEdgeRefIds = aEdge.InternalVertexRefIds;
    isFound                                                       = detachOrderedParentRef(*myGraph,
                                     theVertexRefId,
                                     anEdgeRefIds,
                                     BRepGraph_NodeId(aRef.VertexDefId),
                                     true);
  }

  if (!isFound)
  {
    return false;
  }

  const BRepGraph_VertexId aVertexDefId = aRef.VertexDefId;
  if (isFixed && !hasAnyActiveUsage(*myGraph, BRepGraph_NodeId(aVertexDefId)))
  {
    myGraph->Editor().Gen().RemoveSubgraph(BRepGraph_NodeId(aVertexDefId));
  }

  // Unbind vertex->edge only when no other active ref from this edge points to the same vertex.
  bool aStillReferenced = false;
  {
    const BRepGraphInc::EdgeDef& anEdgeRO = aStorage.Edge(theEdgeDefId);
    auto refTargets                       = [&](const BRepGraph_VertexRefId theRefId) {
      if (!theRefId.IsValid())
        return false;
      const BRepGraphInc::VertexRef& aVR = aStorage.VertexRef(theRefId);
      return !aVR.IsRemoved && aVR.VertexDefId == aVertexDefId;
    };
    if (refTargets(anEdgeRO.StartVertexRefId) || refTargets(anEdgeRO.EndVertexRefId))
    {
      aStillReferenced = true;
    }
    else
    {
      for (const BRepGraph_VertexRefId& anIVRefId : anEdgeRO.InternalVertexRefIds)
      {
        if (refTargets(anIVRefId))
        {
          aStillReferenced = true;
          break;
        }
      }
    }
  }
  if (!aStillReferenced)
  {
    aStorage.ChangeReverseIndex().UnbindVertexFromEdge(aVertexDefId, theEdgeDefId);
  }
  myGraph->markModified(theEdgeDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "RemoveVertex: post-mutation reverse index inconsistency");
  }

  return true;
}

//=================================================================================================

BRepGraph_VertexRefId BRepGraph::EditorView::EdgeOps::ReplaceVertex(
  const BRepGraph_EdgeId      theEdgeDefId,
  const BRepGraph_VertexRefId theOldVertexRefId,
  const BRepGraph_VertexId    theNewVertexDefId)
{
  Standard_ASSERT_RETURN(theEdgeDefId.IsValid(myGraph->myData->myIncStorage.NbEdges()),
                         "ReplaceVertex: edge index is out of range",
                         BRepGraph_VertexRefId());
  Standard_ASSERT_RETURN(theOldVertexRefId.IsValid(myGraph->myData->myIncStorage.NbVertexRefs()),
                         "ReplaceVertex: old vertex ref index is out of range",
                         BRepGraph_VertexRefId());
  Standard_ASSERT_RETURN(theNewVertexDefId.IsValid(myGraph->myData->myIncStorage.NbVertices()),
                         "ReplaceVertex: new vertex index is out of range",
                         BRepGraph_VertexRefId());

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!isActiveNode(aStorage, theEdgeDefId) || !isActiveNode(aStorage, theNewVertexDefId))
  {
    return BRepGraph_VertexRefId();
  }

  // Capture old ref fields before any mutation (vector may reallocate).
  const BRepGraphInc::VertexRef& aOldRef = aStorage.VertexRef(theOldVertexRefId);
  if (aOldRef.IsRemoved || aOldRef.ParentId != BRepGraph_NodeId(theEdgeDefId)
      || !aOldRef.VertexDefId.IsValid())
  {
    return BRepGraph_VertexRefId();
  }
  const BRepGraph_VertexId aOldVertexDefId = aOldRef.VertexDefId;
  const TopAbs_Orientation aOldOri         = aOldRef.Orientation;
  const TopLoc_Location    aOldLoc         = aOldRef.LocalLocation;

  // Short-circuit: no-op if the new vertex is the same as the current one.
  if (aOldVertexDefId == theNewVertexDefId)
  {
    return theOldVertexRefId;
  }

  // Allocate the replacement ref entry.
  const BRepGraph_VertexRefId aNewRefId = aStorage.AppendVertexRef();
  BRepGraphInc::VertexRef&    aNewRef   = aStorage.ChangeVertexRef(aNewRefId);
  aNewRef.ParentId = BRepGraph_NodeId(theEdgeDefId);
  aNewRef.VertexDefId = theNewVertexDefId;
  aNewRef.Orientation                   = aOldOri;
  aNewRef.LocalLocation                 = aOldLoc;
  myGraph->allocateRefUID(aNewRefId);

  // Swap the edge's slot that owned the old ref.
  BRepGraphInc::EdgeDef& anEdge = aStorage.ChangeEdge(theEdgeDefId);
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
    bool isFound = false;
    for (NCollection_DynamicArray<BRepGraph_VertexRefId>::Iterator anIt(
           anEdge.InternalVertexRefIds);
         anIt.More();
         anIt.Next())
    {
      if (anIt.Value() == theOldVertexRefId)
      {
        anIt.ChangeValue() = aNewRefId;
        isFound            = true;
        break;
      }
    }
    if (!isFound)
    {
      // The ref belongs to this edge (ParentId matched) but is not owned by
      // any of its slot vectors. Roll back the append and report failure.
      aStorage.MarkRemovedRef(BRepGraph_RefId(aNewRefId));
      return BRepGraph_VertexRefId();
    }
  }

  // Retire the old ref entry.
  aStorage.MarkRemovedRef(BRepGraph_RefId(theOldVertexRefId));

  // Keep vertex->edge reverse index in sync.
  BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ChangeReverseIndex();
  aRevIdx.UnbindVertexFromEdge(aOldVertexDefId, theEdgeDefId);
  aRevIdx.BindVertexToEdge(theNewVertexDefId, theEdgeDefId);

  myGraph->markRefModified(aNewRefId, aNewRef);
  myGraph->markModified(theEdgeDefId);

  if (!myGraph->myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    Standard_ASSERT_VOID(aStorage.ValidateReverseIndex(),
                         "ReplaceVertex: post-mutation reverse index inconsistency");
  }

  return aNewRefId;
}

//=================================================================================================

void BRepGraph::EditorView::CommitMutation() noexcept
{
  NCollection_DynamicArray<BoundaryIssue> anIssues;
  const bool                              isValid = ValidateMutationBoundary(&anIssues);
  Standard_ASSERT_VOID(isValid, "CommitMutation: mutation boundary consistency check failed");
  (void)isValid;
  (void)anIssues;
}

//=================================================================================================

bool BRepGraph::EditorView::ValidateMutationBoundary(
  NCollection_DynamicArray<BoundaryIssue>* const theIssues) const
{
  bool                        isValid  = true;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (!aStorage.ValidateReverseIndex())
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "Mutation boundary reverse index inconsistency";
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
      continue;

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
      anIssue.NodeId      = BRepGraph_NodeId(aKind, -1);
      anIssue.Description = aDesc;
      theIssues->Append(anIssue);
    }
  }

  // Validate built-in layer consistency: layers must not have bindings
  // for entity kinds that have zero active entities in the graph.
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    myGraph->LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  if (!aParamLayer.IsNull() && aParamLayer->HasBindings() && aStorage.NbVertices() == 0)
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "ParamLayer has bindings but graph has no vertices";
      theIssues->Append(anIssue);
    }
  }

  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    myGraph->LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  if (!aRegularityLayer.IsNull() && aRegularityLayer->HasBindings() && aStorage.NbEdges() == 0)
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "RegularityLayer has bindings but graph has no edges";
      theIssues->Append(anIssue);
    }
  }

  // Check that every OccurrenceDef::ChildDefId carries a valid node kind
  // (Product or a topology kind). Positional self-id invariants are enforced
  // structurally by the typed id system and need no runtime check.
  for (BRepGraph_OccurrenceIterator anOccIt(*myGraph); anOccIt.More(); anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc = anOccIt.Current();
    if (!anOcc.ChildDefId.IsValid() || isValidOccurrenceChildKind(anOcc.ChildDefId.NodeKind))
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
      aDesc += TCollection_AsciiString(static_cast<int>(anOcc.ChildDefId.NodeKind));
      anIssue.NodeId      = BRepGraph_NodeId(anOccId);
      anIssue.Description = std::move(aDesc);
      theIssues->Append(anIssue);
    }
  }

  return isValid;
}
