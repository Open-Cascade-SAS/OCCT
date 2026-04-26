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

#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_Validate.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

#include <utility>

namespace
{

void collectReachableNodesFromRootProducts(const BRepGraph&                   theGraph,
                                           NCollection_Map<BRepGraph_NodeId>& theReachableNodes)
{
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    const BRepGraph_ProductId aRootProductId = aRootIt.Current();
    const uint32_t            aRootNb        = theGraph.Topo().Products().Nb();
    if (!aRootProductId.IsValid(aRootNb))
    {
      continue;
    }

    theReachableNodes.Add(aRootProductId);

    for (BRepGraph_ChildExplorer aChildIt(theGraph, aRootProductId); aChildIt.More();
         aChildIt.Next())
    {
      const BRepGraph_NodeId aNode = aChildIt.Current().DefId;
      if (aNode.IsValid())
      {
        theReachableNodes.Add(aNode);
      }
    }
  }
}

//! Remap a NodeId through old->new index maps. Returns invalid NodeId if not found.
BRepGraph_NodeId remapNodeId(
  const BRepGraph_NodeId&                                                  theId,
  const NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId>&       theVertexMap,
  const NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>&           theEdgeMap,
  const NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId>&           theWireMap,
  const NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>&           theFaceMap,
  const NCollection_DataMap<BRepGraph_ShellId, BRepGraph_ShellId>&         theShellMap,
  const NCollection_DataMap<BRepGraph_SolidId, BRepGraph_SolidId>&         theSolidMap,
  const NCollection_DataMap<BRepGraph_CompoundId, BRepGraph_CompoundId>&   theCompoundMap,
  const NCollection_DataMap<BRepGraph_CompSolidId, BRepGraph_CompSolidId>& theCompSolidMap)
{
  if (!theId.IsValid())
    return BRepGraph_NodeId();

  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId* aNewId = theVertexMap.Seek(BRepGraph_VertexId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId* aNewId = theEdgeMap.Seek(BRepGraph_EdgeId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId* aNewId = theWireMap.Seek(BRepGraph_WireId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId* aNewId = theFaceMap.Seek(BRepGraph_FaceId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId* aNewId = theShellMap.Seek(BRepGraph_ShellId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId* aNewId = theSolidMap.Seek(BRepGraph_SolidId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId* aNewId = theCompoundMap.Seek(BRepGraph_CompoundId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId* aNewId = theCompSolidMap.Seek(BRepGraph_CompSolidId(theId));
      return aNewId != nullptr ? BRepGraph_NodeId(*aNewId) : BRepGraph_NodeId();
    }
    default:
      // Product/Occurrence kinds are not compacted - they reference topology
      // nodes which are remapped independently.
      break;
  }
  return BRepGraph_NodeId();
}

template <typename NodeDefT>
int countActiveDefs(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (BRepGraph_Iterator<NodeDefT> anIt(theGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  return aCount;
}

template <typename NodeDefT>
void bindActiveIndexMap(const BRepGraph& theGraph,
                        NCollection_DataMap<typename BRepGraph_Iterator<NodeDefT>::TypedId,
                                            typename BRepGraph_Iterator<NodeDefT>::TypedId>& theMap,
                        const NCollection_Map<BRepGraph_NodeId>* theReachableNodes = nullptr)
{
  using IdT      = typename BRepGraph_Iterator<NodeDefT>::TypedId;
  IdT aNextNewId = IdT::Start();
  for (BRepGraph_Iterator<NodeDefT> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const IdT aNodeId = anIt.CurrentId();
    if (theReachableNodes != nullptr && !theReachableNodes->Contains(aNodeId))
    {
      continue;
    }
    theMap.Bind(aNodeId, aNextNewId);
    ++aNextNewId;
  }
}

//! Compute shell closedness from rebuilt incidence.
//! A shell is considered closed if each non-degenerate edge used by shell faces
//! is referenced by exactly two faces of that shell.
bool isShellClosedByIncidence(const BRepGraph& theGraph, const BRepGraph_ShellId theShell)
{
  NCollection_Map<BRepGraph_FaceId> aShellFaces;
  for (BRepGraph_RefsFaceOfShell aFaceIt(theGraph, theShell); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceRef& aFR = theGraph.Refs().Faces().Entry(aFaceIt.CurrentId());
    aShellFaces.Add(aFR.FaceDefId);
  }

  if (aShellFaces.IsEmpty())
  {
    return false;
  }

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
    {
      continue;
    }

    int                                               aFaceCountInShell = 0;
    const NCollection_DynamicArray<BRepGraph_FaceId>& aEdgeFaces =
      theGraph.Topo().Edges().Faces(anEdgeId);
    for (NCollection_DynamicArray<BRepGraph_FaceId>::Iterator aFaceIt(aEdgeFaces); aFaceIt.More();
         aFaceIt.Next())
    {
      if (aShellFaces.Contains(aFaceIt.Value()))
      {
        ++aFaceCountInShell;
      }
    }

    if (aFaceCountInShell == 1 || aFaceCountInShell > 2)
    {
      return false;
    }
  }

  return true;
}

//! Compute wire closedness from rebuilt coedge/vertex incidence.
//! A wire is considered closed if every participating vertex has even degree.
bool isWireClosedByIncidence(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  const BRepGraph::RefsView& aRefs = theGraph.Refs();
  const BRepGraph::TopoView& aTopo = theGraph.Topo();

  NCollection_DataMap<BRepGraph_VertexId, int> aVertexDegree;
  int                                          aNbCoEdges = 0;

  for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, theWire); aCEIt.More(); aCEIt.Next())
  {
    const BRepGraphInc::CoEdgeRef& aCRef   = aRefs.CoEdges().Entry(aCEIt.CurrentId());
    const BRepGraphInc::CoEdgeDef& aCoEdge = aTopo.CoEdges().Definition(aCRef.CoEdgeDefId);
    const BRepGraphInc::EdgeDef&   anEdge  = aTopo.Edges().Definition(aCoEdge.EdgeDefId);

    const BRepGraph_VertexRefId aStartRefId =
      (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;
    const BRepGraph_VertexRefId anEndRefId =
      (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.EndVertexRefId : anEdge.StartVertexRefId;

    if (!aStartRefId.IsValid() || !anEndRefId.IsValid())
    {
      return false;
    }

    const BRepGraph_VertexId aStartVtxId = aRefs.Vertices().Entry(aStartRefId).VertexDefId;
    const BRepGraph_VertexId anEndVtxId  = aRefs.Vertices().Entry(anEndRefId).VertexDefId;

    if (!aVertexDegree.IsBound(aStartVtxId))
    {
      aVertexDegree.Bind(aStartVtxId, 0);
    }
    if (!aVertexDegree.IsBound(anEndVtxId))
    {
      aVertexDegree.Bind(anEndVtxId, 0);
    }

    aVertexDegree.ChangeFind(aStartVtxId)++;
    aVertexDegree.ChangeFind(anEndVtxId)++;
    ++aNbCoEdges;
  }

  if (aNbCoEdges == 0)
  {
    return false;
  }

  for (NCollection_DataMap<BRepGraph_VertexId, int>::Iterator aDegIt(aVertexDegree); aDegIt.More();
       aDegIt.Next())
  {
    if ((aDegIt.Value() % 2) != 0)
    {
      return false;
    }
  }

  return true;
}

} // namespace

//=================================================================================================

BRepGraph_Compact::Result BRepGraph_Compact::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraph_Compact::Result BRepGraph_Compact::Perform(BRepGraph& theGraph, const Options& theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  // Count removed nodes per kind.
  aResult.NbRemovedVertices =
    theGraph.Topo().Vertices().Nb() - countActiveDefs<BRepGraphInc::VertexDef>(theGraph);
  aResult.NbRemovedEdges =
    theGraph.Topo().Edges().Nb() - countActiveDefs<BRepGraphInc::EdgeDef>(theGraph);
  aResult.NbRemovedWires =
    theGraph.Topo().Wires().Nb() - countActiveDefs<BRepGraphInc::WireDef>(theGraph);
  aResult.NbRemovedFaces =
    theGraph.Topo().Faces().Nb() - countActiveDefs<BRepGraphInc::FaceDef>(theGraph);
  aResult.NbRemovedShells =
    theGraph.Topo().Shells().Nb() - countActiveDefs<BRepGraphInc::ShellDef>(theGraph);
  aResult.NbRemovedSolids =
    theGraph.Topo().Solids().Nb() - countActiveDefs<BRepGraphInc::SolidDef>(theGraph);
  aResult.NbRemovedCompounds =
    theGraph.Topo().Compounds().Nb() - countActiveDefs<BRepGraphInc::CompoundDef>(theGraph);
  aResult.NbRemovedCompSolids =
    theGraph.Topo().CompSolids().Nb() - countActiveDefs<BRepGraphInc::CompSolidDef>(theGraph);

  const int aTotalRemoved = aResult.NbRemovedVertices + aResult.NbRemovedEdges
                            + aResult.NbRemovedWires + aResult.NbRemovedFaces
                            + aResult.NbRemovedShells + aResult.NbRemovedSolids
                            + aResult.NbRemovedCompounds + aResult.NbRemovedCompSolids;

  aResult.NbNodesBefore = static_cast<int>(theGraph.Topo().Gen().NbNodes());

  // Short-circuit: nothing to compact.
  if (aTotalRemoved == 0)
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    return aResult;
  }

  // Build old->new index maps for each node kind.
  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId>         aVertexMap;
  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>             anEdgeMap;
  NCollection_DataMap<BRepGraph_CoEdgeId, BRepGraph_CoEdgeId>         aCoEdgeMap;
  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId>             aWireMap;
  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>             aFaceMap;
  NCollection_DataMap<BRepGraph_ShellId, BRepGraph_ShellId>           aShellMap;
  NCollection_DataMap<BRepGraph_SolidId, BRepGraph_SolidId>           aSolidMap;
  NCollection_DataMap<BRepGraph_CompoundId, BRepGraph_CompoundId>     aCompoundMap;
  NCollection_DataMap<BRepGraph_CompSolidId, BRepGraph_CompSolidId>   aCompSolidMap;
  NCollection_DataMap<BRepGraph_ProductId, BRepGraph_ProductId>       aProductMap;
  NCollection_DataMap<BRepGraph_OccurrenceId, BRepGraph_OccurrenceId> anOccurrenceMap;

  // Per-kind ref id remap: old RefId -> new RefId.
  // Built during topology rebuild loops; used for RefUID transfer after swap.
  NCollection_DataMap<BRepGraph_VertexRefId, BRepGraph_VertexRefId>         aVertexRefMap;
  NCollection_DataMap<BRepGraph_CoEdgeRefId, BRepGraph_CoEdgeRefId>         aCoEdgeRefMap;
  NCollection_DataMap<BRepGraph_WireRefId, BRepGraph_WireRefId>             aWireRefMap;
  NCollection_DataMap<BRepGraph_FaceRefId, BRepGraph_FaceRefId>             aFaceRefMap;
  NCollection_DataMap<BRepGraph_ShellRefId, BRepGraph_ShellRefId>           aShellRefMap;
  NCollection_DataMap<BRepGraph_ChildRefId, BRepGraph_ChildRefId>           aChildRefMap;
  NCollection_DataMap<BRepGraph_SolidRefId, BRepGraph_SolidRefId>           aSolidRefMap;
  NCollection_DataMap<BRepGraph_OccurrenceRefId, BRepGraph_OccurrenceRefId> anOccurrenceRefMap;

  NCollection_Map<BRepGraph_NodeId> aReachableNodes;
  const bool                        hasRootProducts = !theGraph.RootProductIds().IsEmpty();
  if (hasRootProducts)
  {
    collectReachableNodesFromRootProducts(theGraph, aReachableNodes);
  }

  const NCollection_Map<BRepGraph_NodeId>* aReachableFilter =
    hasRootProducts ? &aReachableNodes : nullptr;

  bindActiveIndexMap<BRepGraphInc::VertexDef>(theGraph, aVertexMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::EdgeDef>(theGraph, anEdgeMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::WireDef>(theGraph, aWireMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::FaceDef>(theGraph, aFaceMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::ShellDef>(theGraph, aShellMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::SolidDef>(theGraph, aSolidMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::CompoundDef>(theGraph, aCompoundMap, aReachableFilter);
  bindActiveIndexMap<BRepGraphInc::CompSolidDef>(theGraph, aCompSolidMap, aReachableFilter);

  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);
  const BRepGraph_Data* aGraphData  = theGraph.data();
  const uint32_t        aGeneration = aGraphData->myGeneration.load(std::memory_order_relaxed);

  // Construct a fresh graph and rebuild bottom-up.
  // Geometry nodes (Surface, Curve) are automatically created through Add/Add.
  BRepGraph       aNewGraph;
  BRepGraph_Data* aNewGraphData = aNewGraph.data();
  aNewGraphData->myGeneration.store(aGeneration, std::memory_order_relaxed);

  // Helper lambda for remapping NodeIds.
  auto remapId = [&](const BRepGraph_NodeId& theId) -> BRepGraph_NodeId {
    return remapNodeId(theId,
                       aVertexMap,
                       anEdgeMap,
                       aWireMap,
                       aFaceMap,
                       aShellMap,
                       aSolidMap,
                       aCompoundMap,
                       aCompSolidMap);
  };

  // Add topology defs bottom-up (Vertex -> Edge -> Wire -> Face -> Shell -> Solid).
  // Vertices.
  for (BRepGraph_Iterator<BRepGraphInc::VertexDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    if (aVertexMap.Seek(anIt.CurrentId()) == nullptr)
    {
      continue;
    }

    const BRepGraphInc::VertexDef& anOldVtx = anIt.Current();
    (void)aNewGraph.Editor().Vertices().Add(anOldVtx.Point, anOldVtx.Tolerance);
  }

  // Edges.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId anOldEdgeId = anIt.CurrentId();
    if (anEdgeMap.Seek(anOldEdgeId) == nullptr)
    {
      continue;
    }
    const BRepGraphInc::EdgeDef& anOldEdge = anIt.Current();

    const BRepGraph_VertexId aOldStartId =
      BRepGraph_Tool::Edge::StartVertexId(theGraph, anOldEdgeId);
    const BRepGraph_VertexId aNewStart = aOldStartId.IsValid()
                                           ? BRepGraph_VertexId::FromNodeId(remapId(aOldStartId))
                                           : BRepGraph_VertexId();
    const BRepGraph_VertexId aOldEndId = BRepGraph_Tool::Edge::EndVertexId(theGraph, anOldEdgeId);
    const BRepGraph_VertexId aNewEnd   = aOldEndId.IsValid()
                                           ? BRepGraph_VertexId::FromNodeId(remapId(aOldEndId))
                                           : BRepGraph_VertexId();

    // Get the curve handle if available.
    const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(theGraph, anOldEdgeId);

    const BRepGraph_EdgeId aNewEdgeId = aNewGraph.Editor().Edges().Add(aNewStart,
                                                                       aNewEnd,
                                                                       aCurve,
                                                                       anOldEdge.ParamFirst,
                                                                       anOldEdge.ParamLast,
                                                                       anOldEdge.Tolerance);

    // Copy edge properties.
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aNewGraph.Editor().Edges().Mut(aNewEdgeId);
    aNewEdge.Internal().IsDegenerate                   = anOldEdge.IsDegenerate;
    aNewEdge.Internal().IsClosed                       = anOldEdge.IsClosed;
    aNewEdge.Internal().SameParameter                  = anOldEdge.SameParameter;
    aNewEdge.Internal().SameRange                      = anOldEdge.SameRange;
    aNewEdge.MarkDirty();

    if (anOldEdge.StartVertexRefId.IsValid() && aNewEdge->StartVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anOldStartRef =
        theGraph.Refs().Vertices().Entry(anOldEdge.StartVertexRefId);
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewStartRef =
        aNewGraph.Editor().Vertices().MutRef(aNewEdge->StartVertexRefId);
      aNewStartRef.Internal().Orientation   = anOldStartRef.Orientation;
      aNewStartRef.Internal().LocalLocation = anOldStartRef.LocalLocation;
      aNewStartRef.MarkDirty();
      aVertexRefMap.Bind(anOldEdge.StartVertexRefId, aNewEdge->StartVertexRefId);
    }

    if (anOldEdge.EndVertexRefId.IsValid() && aNewEdge->EndVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anOldEndRef =
        theGraph.Refs().Vertices().Entry(anOldEdge.EndVertexRefId);
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewEndRef =
        aNewGraph.Editor().Vertices().MutRef(aNewEdge->EndVertexRefId);
      aNewEndRef.Internal().Orientation   = anOldEndRef.Orientation;
      aNewEndRef.Internal().LocalLocation = anOldEndRef.LocalLocation;
      aNewEndRef.MarkDirty();
      aVertexRefMap.Bind(anOldEdge.EndVertexRefId, aNewEdge->EndVertexRefId);
    }

    for (const BRepGraph_VertexRefId& anOldInternalRefId : anOldEdge.InternalVertexRefIds)
    {
      const BRepGraphInc::VertexRef& anOldInternalRef =
        theGraph.Refs().Vertices().Entry(anOldInternalRefId);
      const BRepGraph_VertexId aNewVertexId =
        BRepGraph_VertexId::FromNodeId(remapId(anOldInternalRef.VertexDefId));
      if (!aNewVertexId.IsValid())
      {
        continue;
      }

      const BRepGraph_VertexRefId aNewInternalRefId =
        aNewGraph.Editor().Edges().AddInternalVertex(aNewEdgeId,
                                                     aNewVertexId,
                                                     anOldInternalRef.Orientation);
      if (!aNewInternalRefId.IsValid())
      {
        continue;
      }

      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewInternalRef =
        aNewGraph.Editor().Vertices().MutRef(aNewInternalRefId);
      aNewInternalRef.Internal().LocalLocation = anOldInternalRef.LocalLocation;
      aNewInternalRef.MarkDirty();
      aVertexRefMap.Bind(anOldInternalRefId, aNewInternalRefId);
    }
  }

  // PCurves (added after edges and faces are known).
  // We need faces first, so do PCurves after face creation.

  // Wires.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_WireId anOldWireId = anIt.CurrentId();
    if (aWireMap.Seek(anOldWireId) == nullptr)
    {
      continue;
    }

    NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    NCollection_DynamicArray<BRepGraph_CoEdgeId>                              anOldCoEdges;
    NCollection_DynamicArray<BRepGraph_CoEdgeRefId>                           anOldCoEdgeRefs;
    for (BRepGraph_RefsCoEdgeOfWire aRefIt(theGraph, anOldWireId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(aRefIt.CurrentId());
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_EdgeId aNewEdgeDefId =
        BRepGraph_EdgeId::FromNodeId(remapId(aCoEdge.EdgeDefId));
      if (aNewEdgeDefId.IsValid())
      {
        aNewEntries.Append(std::make_pair(aNewEdgeDefId, aCoEdge.Orientation));
        anOldCoEdges.Append(aCR.CoEdgeDefId);
        anOldCoEdgeRefs.Append(aRefIt.CurrentId());
      }
    }
    const BRepGraph_WireId aNewWireId = aNewGraph.Editor().Wires().Add(aNewEntries);

    const NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& aNewCoEdgeRefs =
      aNewGraph.Topo().Wires().Definition(aNewWireId).CoEdgeRefIds;
    for (size_t aRefIdx = 0; aRefIdx < anOldCoEdgeRefs.Size() && aRefIdx < aNewCoEdgeRefs.Size();
         ++aRefIdx)
    {
      const BRepGraphInc::CoEdgeRef& anOldRef =
        theGraph.Refs().CoEdges().Entry(anOldCoEdgeRefs.Value(aRefIdx));
      BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> aNewRef =
        aNewGraph.Editor().CoEdges().MutRef(aNewCoEdgeRefs.Value(aRefIdx));
      aNewRef.Internal().LocalLocation = anOldRef.LocalLocation;
      aNewRef.MarkDirty();

      // Build aCoEdgeMap from actual CoEdgeDef IDs returned by the builder,
      // not from a predicted Nb()-based offset which can be wrong.
      const BRepGraphInc::CoEdgeRef& aNewCoEdgeRef =
        aNewGraph.Refs().CoEdges().Entry(aNewCoEdgeRefs.Value(aRefIdx));
      aCoEdgeMap.Bind(anOldCoEdges.Value(aRefIdx), aNewCoEdgeRef.CoEdgeDefId);
      // Track CoEdgeRef index remap for RefUID transfer.
      aCoEdgeRefMap.Bind(anOldCoEdgeRefs.Value(aRefIdx), aNewCoEdgeRefs.Value(aRefIdx));
    }

    BRepGraph_MutGuard<BRepGraphInc::WireDef> aNewWire = aNewGraph.Editor().Wires().Mut(aNewWireId);
    aNewWire.Internal().IsClosed = isWireClosedByIncidence(aNewGraph, aNewWireId);
    aNewWire.MarkDirty();
  }

  // Faces.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_FaceId anOldFaceId = anIt.CurrentId();
    if (aFaceMap.Seek(anOldFaceId) == nullptr)
    {
      continue;
    }
    const BRepGraphInc::FaceDef& anOldFace = anIt.Current();

    const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, anOldFaceId);

    // Find outer wire from incidence ref entries.
    BRepGraph_WireId                              aNewOuterWire;
    NCollection_DynamicArray<BRepGraph_WireId>    aNewInnerWires;
    BRepGraph_WireRefId                           anOldOuterWireRef;
    NCollection_DynamicArray<BRepGraph_WireRefId> anOldInnerWireRefs;

    for (BRepGraph_RefsWireOfFace aRefIt(theGraph, anOldFaceId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::WireRef& aWR       = theGraph.Refs().Wires().Entry(aRefIt.CurrentId());
      const BRepGraph_WireId       aRemapped = BRepGraph_WireId::FromNodeId(remapId(aWR.WireDefId));
      if (!aRemapped.IsValid())
        continue;
      if (aWR.IsOuter)
      {
        aNewOuterWire     = aRemapped;
        anOldOuterWireRef = aRefIt.CurrentId();
      }
      else
      {
        aNewInnerWires.Append(aRemapped);
        anOldInnerWireRefs.Append(aRefIt.CurrentId());
      }
    }

    const BRepGraph_FaceId aNewFaceId =
      aNewGraph.Editor().Faces().Add(aSurf, aNewOuterWire, aNewInnerWires, anOldFace.Tolerance);

    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace = aNewGraph.Editor().Faces().Mut(aNewFaceId);
    aNewFace.Internal().NaturalRestriction             = anOldFace.NaturalRestriction;
    aNewFace.MarkDirty();

    const NCollection_DynamicArray<BRepGraph_WireRefId>& aNewWireRefs = aNewFace->WireRefIds;
    if (anOldOuterWireRef.IsValid() && !aNewWireRefs.IsEmpty())
    {
      const BRepGraphInc::WireRef& anOldOuterRef = theGraph.Refs().Wires().Entry(anOldOuterWireRef);
      BRepGraph_MutGuard<BRepGraphInc::WireRef> aNewOuterRef =
        aNewGraph.Editor().Wires().MutRef(aNewWireRefs.First());
      aNewOuterRef.Internal().IsOuter       = anOldOuterRef.IsOuter;
      aNewOuterRef.Internal().Orientation   = anOldOuterRef.Orientation;
      aNewOuterRef.Internal().LocalLocation = anOldOuterRef.LocalLocation;
      aNewOuterRef.MarkDirty();
      aWireRefMap.Bind(anOldOuterWireRef, aNewWireRefs.First());
    }

    for (size_t anInnerIdx = 0;
         anInnerIdx < anOldInnerWireRefs.Size() && anInnerIdx + 1 < aNewWireRefs.Size();
         ++anInnerIdx)
    {
      const BRepGraphInc::WireRef& anOldInnerRef =
        theGraph.Refs().Wires().Entry(anOldInnerWireRefs.Value(anInnerIdx));
      BRepGraph_MutGuard<BRepGraphInc::WireRef> aNewInnerRef =
        aNewGraph.Editor().Wires().MutRef(aNewWireRefs.Value(anInnerIdx + 1));
      aNewInnerRef.Internal().IsOuter       = anOldInnerRef.IsOuter;
      aNewInnerRef.Internal().Orientation   = anOldInnerRef.Orientation;
      aNewInnerRef.Internal().LocalLocation = anOldInnerRef.LocalLocation;
      aNewInnerRef.MarkDirty();
      aWireRefMap.Bind(anOldInnerWireRefs.Value(anInnerIdx), aNewWireRefs.Value(anInnerIdx + 1));
    }

    for (const BRepGraph_VertexRefId& anOldVertexRefId : anOldFace.VertexRefIds)
    {
      const BRepGraphInc::VertexRef& anOldVertexRef =
        theGraph.Refs().Vertices().Entry(anOldVertexRefId);
      const BRepGraph_VertexId aNewVertexId =
        BRepGraph_VertexId::FromNodeId(remapId(anOldVertexRef.VertexDefId));
      if (!aNewVertexId.IsValid())
      {
        continue;
      }

      const BRepGraph_VertexRefId aNewVertexRefId =
        aNewGraph.Editor().Faces().AddVertex(aNewFaceId, aNewVertexId, anOldVertexRef.Orientation);
      if (!aNewVertexRefId.IsValid())
      {
        continue;
      }

      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewVertexRef =
        aNewGraph.Editor().Vertices().MutRef(aNewVertexRefId);
      aNewVertexRef.Internal().LocalLocation = anOldVertexRef.LocalLocation;
      aNewVertexRef.MarkDirty();
      aVertexRefMap.Bind(anOldVertexRefId, aNewVertexRefId);
    }
  }

  // Transfer face/PCurve attributes onto the remapped coedges created by WireOps::Add().
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId  anOldCoEdgeId = anIt.CurrentId();
    const BRepGraph_CoEdgeId* aNewCoEdgeId  = aCoEdgeMap.Seek(anOldCoEdgeId);
    if (aNewCoEdgeId == nullptr)
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef&              anOldCoEdge = anIt.Current();
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aNewCoEdge =
      aNewGraph.Editor().CoEdges().Mut(*aNewCoEdgeId);

    aNewCoEdge.Internal().FaceDefId      = BRepGraph_FaceId::FromNodeId(remapId(anOldCoEdge.FaceDefId));
    aNewCoEdge.Internal().Orientation    = anOldCoEdge.Orientation;
    aNewCoEdge.Internal().ParamFirst     = anOldCoEdge.ParamFirst;
    aNewCoEdge.Internal().ParamLast      = anOldCoEdge.ParamLast;
    aNewCoEdge.Internal().Continuity     = anOldCoEdge.Continuity;
    aNewCoEdge.Internal().UV1            = anOldCoEdge.UV1;
    aNewCoEdge.Internal().UV2            = anOldCoEdge.UV2;
    aNewCoEdge.Internal().SeamContinuity = anOldCoEdge.SeamContinuity;

    // If the owning face was removed, keep the coedge as free-wire usage only.
    // Drop face-bound parametric payload (PCurve/UV/continuity) to avoid stale
    // face references causing reconstruction/meshing corruption.
    if (!aNewCoEdge->FaceDefId.IsValid())
    {
      aNewCoEdge.Internal().Curve2DRepId = BRepGraph_Curve2DRepId();
      aNewCoEdge.Internal().ParamFirst   = 0.0;
      aNewCoEdge.Internal().ParamLast    = 0.0;
      aNewCoEdge.Internal().Continuity   = GeomAbs_C0;
      aNewCoEdge.Internal().UV1          = gp_Pnt2d();
      aNewCoEdge.Internal().UV2          = gp_Pnt2d();
      aNewCoEdge.MarkDirty();
      continue;
    }

    if (anOldCoEdge.Curve2DRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& anOldPCurve =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, anOldCoEdgeId);
      if (!anOldPCurve.IsNull())
      {
        // Use the builder API to create the Curve2DRep - this ensures internal
        // registration tables are updated correctly (as documented in EditorView.hxx).
        const BRepGraph_Curve2DRepId aNewRepId =
          aNewGraph.Editor().CoEdges().CreateCurve2DRep(anOldPCurve);
        if (aNewRepId.IsValid())
        {
          aNewCoEdge.Internal().Curve2DRepId = aNewRepId;
        }
      }
    }
    aNewCoEdge.MarkDirty();
  }

  // Remap seam-pair links after all coedges have their new ids.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId  anOldCoEdgeId = anIt.CurrentId();
    const BRepGraph_CoEdgeId* aNewCoEdgeId  = aCoEdgeMap.Seek(anOldCoEdgeId);
    if (aNewCoEdgeId == nullptr)
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& anOldCoEdge = anIt.Current();
    if (!anOldCoEdge.SeamPairId.IsValid())
    {
      continue;
    }

    const BRepGraph_CoEdgeId* aNewSeamPairId = aCoEdgeMap.Seek(anOldCoEdge.SeamPairId);
    if (aNewSeamPairId == nullptr)
    {
      continue;
    }

    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aNewCoEdge =
      aNewGraph.Editor().CoEdges().Mut(*aNewCoEdgeId);
    aNewCoEdge.Internal().SeamPairId = *aNewSeamPairId;
    aNewCoEdge.MarkDirty();
  }

  // Shells.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_ShellId anOldShellId = anIt.CurrentId();
    if (aShellMap.Seek(anOldShellId) == nullptr)
    {
      continue;
    }

    const BRepGraph_ShellId aNewShellId = aNewGraph.Editor().Shells().Add();

    // Add faces to shell via incidence ref entries.
    for (BRepGraph_RefsFaceOfShell aRefIt(theGraph, anOldShellId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR      = theGraph.Refs().Faces().Entry(aRefIt.CurrentId());
      const BRepGraph_FaceId       aNewFace = BRepGraph_FaceId::FromNodeId(remapId(aFR.FaceDefId));
      if (aNewFace.IsValid())
      {
        const BRepGraph_FaceRefId aNewFaceRefId =
          aNewGraph.Editor().Shells().AddFace(aNewShellId, aNewFace, aFR.Orientation);
        if (aNewFaceRefId.IsValid())
        {
          BRepGraph_MutGuard<BRepGraphInc::FaceRef> aNewFaceRef =
            aNewGraph.Editor().Faces().MutRef(aNewFaceRefId);
          aNewFaceRef.Internal().LocalLocation = aFR.LocalLocation;
          aNewFaceRef.MarkDirty();
          aFaceRefMap.Bind(aRefIt.CurrentId(), aNewFaceRefId);
        }
      }
    }

    for (BRepGraph_RefsChildOfShell aRefIt(theGraph, anOldShellId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildDefId);
      if (!aNewChild.IsValid())
      {
        continue;
      }
      const BRepGraph_ChildRefId aNewChildRefId =
        aNewGraph.Editor().Shells().AddChild(aNewShellId, aNewChild, aCR.Orientation);
      if (!aNewChildRefId.IsValid())
      {
        continue;
      }
      BRepGraph_MutGuard<BRepGraphInc::ChildRef> aNewChildRef =
        aNewGraph.Editor().Gen().MutChildRef(aNewChildRefId);
      aNewChildRef.Internal().LocalLocation = aCR.LocalLocation;
      aNewChildRef.MarkDirty();
      aChildRefMap.Bind(aRefIt.CurrentId(), aNewChildRefId);
    }

    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aNewShell =
      aNewGraph.Editor().Shells().Mut(aNewShellId);
    aNewShell.Internal().IsClosed = isShellClosedByIncidence(aNewGraph, aNewShellId);
    aNewShell.MarkDirty();
  }

  // Solids.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_SolidId anOldSolidId = anIt.CurrentId();
    if (aSolidMap.Seek(anOldSolidId) == nullptr)
    {
      continue;
    }

    const BRepGraph_SolidId aNewSolidId = aNewGraph.Editor().Solids().Add();

    for (BRepGraph_RefsShellOfSolid aRefIt(theGraph, anOldSolidId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR = theGraph.Refs().Shells().Entry(aRefIt.CurrentId());
      const BRepGraph_ShellId aNewShell = BRepGraph_ShellId::FromNodeId(remapId(aSR.ShellDefId));
      if (aNewShell.IsValid())
      {
        const BRepGraph_ShellRefId aNewShellRefId =
          aNewGraph.Editor().Solids().AddShell(aNewSolidId, aNewShell, aSR.Orientation);
        if (aNewShellRefId.IsValid())
        {
          BRepGraph_MutGuard<BRepGraphInc::ShellRef> aNewShellRef =
            aNewGraph.Editor().Shells().MutRef(aNewShellRefId);
          aNewShellRef.Internal().LocalLocation = aSR.LocalLocation;
          aNewShellRef.MarkDirty();
          aShellRefMap.Bind(aRefIt.CurrentId(), aNewShellRefId);
        }
      }
    }

    for (BRepGraph_RefsChildOfSolid aRefIt(theGraph, anOldSolidId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildDefId);
      if (!aNewChild.IsValid())
      {
        continue;
      }
      const BRepGraph_ChildRefId aNewChildRefId =
        aNewGraph.Editor().Solids().AddChild(aNewSolidId, aNewChild, aCR.Orientation);
      if (!aNewChildRefId.IsValid())
      {
        continue;
      }
      BRepGraph_MutGuard<BRepGraphInc::ChildRef> aNewChildRef =
        aNewGraph.Editor().Gen().MutChildRef(aNewChildRefId);
      aNewChildRef.Internal().LocalLocation = aCR.LocalLocation;
      aNewChildRef.MarkDirty();
      aChildRefMap.Bind(aRefIt.CurrentId(), aNewChildRefId);
    }
  }

  // Compounds.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId anOldCompoundId = anIt.CurrentId();
    if (aCompoundMap.Seek(anOldCompoundId) == nullptr)
    {
      continue;
    }

    NCollection_DynamicArray<BRepGraph_NodeId>     aNewChildren;
    NCollection_DynamicArray<BRepGraph_ChildRefId> anOldChildRefs;
    for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, anOldCompoundId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildDefId);
      if (aNewChild.IsValid())
      {
        aNewChildren.Append(aNewChild);
        anOldChildRefs.Append(aRefIt.CurrentId());
      }
    }
    const BRepGraph_CompoundId aNewCompoundId = aNewGraph.Editor().Compounds().Add(aNewChildren);
    if (aNewCompoundId.IsValid())
    {
      const NCollection_DynamicArray<BRepGraph_ChildRefId>& aNewChildRefs =
        aNewGraph.Topo().Compounds().Definition(aNewCompoundId).ChildRefIds;
      for (size_t aRefIdx = 0; aRefIdx < anOldChildRefs.Size() && aRefIdx < aNewChildRefs.Size();
           ++aRefIdx)
      {
        const BRepGraphInc::ChildRef& anOldRef =
          theGraph.Refs().Children().Entry(anOldChildRefs.Value(aRefIdx));
        BRepGraph_MutGuard<BRepGraphInc::ChildRef> aNewRef =
          aNewGraph.Editor().Gen().MutChildRef(aNewChildRefs.Value(aRefIdx));
        aNewRef.Internal().Orientation   = anOldRef.Orientation;
        aNewRef.Internal().LocalLocation = anOldRef.LocalLocation;
        aNewRef.MarkDirty();
        aChildRefMap.Bind(anOldChildRefs.Value(aRefIdx), aNewChildRefs.Value(aRefIdx));
      }
    }
  }

  // CompSolids.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompSolidId anOldCompSolidId = anIt.CurrentId();
    if (aCompSolidMap.Seek(anOldCompSolidId) == nullptr)
    {
      continue;
    }

    NCollection_DynamicArray<BRepGraph_SolidId>    aNewSolids;
    NCollection_DynamicArray<BRepGraph_SolidRefId> anOldSolidRefs;
    for (BRepGraph_RefsSolidOfCompSolid aRefIt(theGraph, anOldCompSolidId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::SolidRef& aSR = theGraph.Refs().Solids().Entry(aRefIt.CurrentId());
      const BRepGraph_SolidId aNewSolid = BRepGraph_SolidId::FromNodeId(remapId(aSR.SolidDefId));
      if (aNewSolid.IsValid())
      {
        aNewSolids.Append(aNewSolid);
        anOldSolidRefs.Append(aRefIt.CurrentId());
      }
    }
    const BRepGraph_CompSolidId aNewCompSolidId = aNewGraph.Editor().CompSolids().Add(aNewSolids);
    if (aNewCompSolidId.IsValid())
    {
      const NCollection_DynamicArray<BRepGraph_SolidRefId>& aNewSolidRefs =
        aNewGraph.Topo().CompSolids().Definition(aNewCompSolidId).SolidRefIds;
      for (size_t aRefIdx = 0; aRefIdx < anOldSolidRefs.Size() && aRefIdx < aNewSolidRefs.Size();
           ++aRefIdx)
      {
        const BRepGraphInc::SolidRef& anOldRef =
          theGraph.Refs().Solids().Entry(anOldSolidRefs.Value(aRefIdx));
        BRepGraph_MutGuard<BRepGraphInc::SolidRef> aNewRef =
          aNewGraph.Editor().Solids().MutRef(aNewSolidRefs.Value(aRefIdx));
        aNewRef.Internal().Orientation   = anOldRef.Orientation;
        aNewRef.Internal().LocalLocation = anOldRef.LocalLocation;
        aNewRef.MarkDirty();
        aSolidRefMap.Bind(anOldSolidRefs.Value(aRefIdx), aNewSolidRefs.Value(aRefIdx));
      }
    }
  }

  // Rebuild reverse index from final forward incidence to guarantee compact output consistency.
  aNewGraph.incStorage().BuildReverseIndex();

  // Validate rebuilt graph before swapping it into the source graph.
  // If rebuilt topology is inconsistent, keep source graph unchanged.
  if (!aNewGraph.Editor().ValidateMutationBoundary())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    theGraph.History().SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  const BRepGraph_Validate::Result aCompactValidation = BRepGraph_Validate::Perform(aNewGraph);
  if (!aCompactValidation.IsValid())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    theGraph.History().SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  // Rebuild assembly/product structure preserving part roots and assembly placements.
  // Products and occurrences are not topology defs and must be rebuilt explicitly.
  for (BRepGraph_FullProductIterator aProductIt(theGraph); aProductIt.More(); aProductIt.Next())
  {
    const BRepGraph_ProductId anOldProductId = aProductIt.CurrentId();
    if (hasRootProducts && !aReachableNodes.Contains(anOldProductId))
    {
      continue;
    }
    const BRepGraphInc::ProductDef& anOldProduct =
      theGraph.Topo().Products().Definition(anOldProductId);
    if (anOldProduct.IsRemoved)
    {
      continue;
    }

    BRepGraph_ProductId    aNewProductId;
    const BRepGraph_NodeId anOldShapeRoot =
      theGraph.Topo().Products().ShapeRootNode(anOldProductId);
    if (anOldShapeRoot.IsValid() && BRepGraph_NodeId::IsTopologyKind(anOldShapeRoot.NodeKind))
    {
      const BRepGraph_NodeId aNewShapeRoot = remapId(anOldShapeRoot);
      if (aNewShapeRoot.IsValid())
      {
        // Find the old part-root occurrence ref to preserve its placement.
        TopLoc_Location anOldRootLoc;
        for (const BRepGraph_OccurrenceRefId& anOldOccRefId : anOldProduct.OccurrenceRefIds)
        {
          const BRepGraphInc::OccurrenceRef& anOldOccRef =
            theGraph.Refs().Occurrences().Entry(anOldOccRefId);
          if (anOldOccRef.IsRemoved)
            continue;
          const BRepGraphInc::OccurrenceDef& anOldOccDef =
            theGraph.Topo().Occurrences().Definition(anOldOccRef.OccurrenceDefId);
          if (!anOldOccDef.IsRemoved && anOldOccDef.ChildDefId.IsValid()
              && BRepGraph_NodeId::IsTopologyKind(anOldOccDef.ChildDefId.NodeKind))
          {
            anOldRootLoc = anOldOccRef.LocalLocation;
            break;
          }
        }
        aNewProductId =
          aNewGraph.Editor().Products().LinkProductToTopology(aNewShapeRoot, anOldRootLoc);
      }
    }

    if (!aNewProductId.IsValid())
    {
      aNewProductId = aNewGraph.Editor().Products().CreateEmptyProduct();
    }

    if (!aNewProductId.IsValid())
    {
      continue;
    }

    aProductMap.Bind(anOldProductId, aNewProductId);

    // Map part root occurrence (topology child occurrence inside product).
    BRepGraph_OccurrenceId    anOldPartOccurrenceId;
    BRepGraph_OccurrenceRefId anOldPartOccurrenceRefId;
    for (const BRepGraph_OccurrenceRefId& anOldOccRefId : anOldProduct.OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& anOldOccRef =
        theGraph.Refs().Occurrences().Entry(anOldOccRefId);
      if (anOldOccRef.IsRemoved)
      {
        continue;
      }

      const BRepGraphInc::OccurrenceDef& anOldOccDef =
        theGraph.Topo().Occurrences().Definition(anOldOccRef.OccurrenceDefId);
      if (anOldOccDef.IsRemoved)
      {
        continue;
      }

      if (anOldOccDef.ChildDefId.IsValid()
          && BRepGraph_NodeId::IsTopologyKind(anOldOccDef.ChildDefId.NodeKind))
      {
        anOldPartOccurrenceId    = anOldOccRef.OccurrenceDefId;
        anOldPartOccurrenceRefId = anOldOccRefId;
        break;
      }
    }

    if (!anOldPartOccurrenceId.IsValid())
    {
      continue;
    }

    const BRepGraphInc::ProductDef& aNewProduct =
      aNewGraph.Topo().Products().Definition(aNewProductId);
    for (const BRepGraph_OccurrenceRefId& aNewOccRefId : aNewProduct.OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& aNewOccRef =
        aNewGraph.Refs().Occurrences().Entry(aNewOccRefId);
      const BRepGraphInc::OccurrenceDef& aNewOccDef =
        aNewGraph.Topo().Occurrences().Definition(aNewOccRef.OccurrenceDefId);
      if (aNewOccDef.IsRemoved)
      {
        continue;
      }

      if (aNewOccDef.ChildDefId.IsValid()
          && BRepGraph_NodeId::IsTopologyKind(aNewOccDef.ChildDefId.NodeKind))
      {
        anOccurrenceMap.Bind(anOldPartOccurrenceId, aNewOccRef.OccurrenceDefId);
        if (anOldPartOccurrenceRefId.IsValid())
        {
          anOccurrenceRefMap.Bind(anOldPartOccurrenceRefId, aNewOccRefId);
        }
        break;
      }
    }
  }

  // Restore product-product assembly links with local placements.
  for (BRepGraph_FullProductIterator aParentProductIt(theGraph); aParentProductIt.More();
       aParentProductIt.Next())
  {
    const BRepGraph_ProductId anOldParentProductId = aParentProductIt.CurrentId();
    if (hasRootProducts && !aReachableNodes.Contains(anOldParentProductId))
    {
      continue;
    }
    const BRepGraphInc::ProductDef& anOldParentProduct =
      theGraph.Topo().Products().Definition(anOldParentProductId);
    if (anOldParentProduct.IsRemoved)
    {
      continue;
    }

    const BRepGraph_ProductId* aNewParentId = aProductMap.Seek(anOldParentProductId);
    if (aNewParentId == nullptr)
    {
      continue;
    }

    for (const BRepGraph_OccurrenceRefId& anOldOccRefId : anOldParentProduct.OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& anOldOccRef =
        theGraph.Refs().Occurrences().Entry(anOldOccRefId);
      if (anOldOccRef.IsRemoved)
      {
        continue;
      }

      const BRepGraphInc::OccurrenceDef& anOldOccDef =
        theGraph.Topo().Occurrences().Definition(anOldOccRef.OccurrenceDefId);
      if (anOldOccDef.IsRemoved || !anOldOccDef.ChildDefId.IsValid()
          || anOldOccDef.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
      {
        continue;
      }

      const BRepGraph_ProductId  anOldChildProductId(anOldOccDef.ChildDefId);
      const BRepGraph_ProductId* aNewChildId = aProductMap.Seek(anOldChildProductId);
      if (aNewChildId == nullptr)
      {
        continue;
      }

      const BRepGraph_OccurrenceId aNewOccId =
        aNewGraph.Editor().Products().LinkProducts(*aNewParentId,
                                                   *aNewChildId,
                                                   anOldOccRef.LocalLocation);

      if (aNewOccId.IsValid())
      {
        anOccurrenceMap.Bind(anOldOccRef.OccurrenceDefId, aNewOccId);
        // Find the new OccurrenceRef in the parent product for RefUID transfer.
        const BRepGraphInc::ProductDef& aNewParentDef =
          aNewGraph.Topo().Products().Definition(*aNewParentId);
        for (const BRepGraph_OccurrenceRefId& aNewOccRefId : aNewParentDef.OccurrenceRefIds)
        {
          if (aNewGraph.Refs().Occurrences().Entry(aNewOccRefId).OccurrenceDefId == aNewOccId)
          {
            anOccurrenceRefMap.Bind(anOldOccRefId, aNewOccRefId);
            break;
          }
        }
      }
    }
  }

  // Force full mesh/presentation recomputation after compact rebuild.
  // Compaction rewrites ids and references; preserving old mesh reps can leave
  // stale polygon/triangulation bindings and corrupted appearance/bounds.
  aNewGraph.meshCache().Clear();
  aNewGraph.refTransientCache().Clear();

  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(aNewGraph); anIt.More(); anIt.Next())
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace =
      aNewGraph.Editor().Faces().Mut(anIt.CurrentId());
    aFace.Internal().TriangulationRepId = BRepGraph_TriangulationRepId();
    aFace.MarkDirty();
  }

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(aNewGraph); anIt.More(); anIt.Next())
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
      aNewGraph.Editor().Edges().Mut(anIt.CurrentId());
    anEdge.Internal().Polygon3DRepId = BRepGraph_Polygon3DRepId();
    anEdge.MarkDirty();
  }

  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(aNewGraph); anIt.More(); anIt.Next())
  {
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdge =
      aNewGraph.Editor().CoEdges().Mut(anIt.CurrentId());
    aCoEdge.Internal().Polygon2DRepId    = BRepGraph_Polygon2DRepId();
    aCoEdge.Internal().PolygonOnTriRepId = BRepGraph_PolygonOnTriRepId();
    aCoEdge.MarkDirty();
  }

  aResult.NbNodesAfter = static_cast<int>(aNewGraph.Topo().Gen().NbNodes());

  // Transfer per-kind UID vectors from old graph to new graph using index remap maps.
  // Each new graph's UID vector[newIdx] = old graph's UID vector[oldIdx].
  auto transferUIDs = [&](const auto&                                    theMap,
                          const NCollection_DynamicArray<BRepGraph_UID>& theOldVec,
                          NCollection_DynamicArray<BRepGraph_UID>&       theNewVec) {
    // New vector was already populated by EditorView during reconstruction.
    // Overwrite entries that have a mapping from the old graph.
    for (const auto& [anOldId, aNewId] : theMap.Items())
    {
      if (anOldId.IsValidIn(theOldVec)
          && theOldVec.Value(static_cast<size_t>(anOldId.Index)).IsValid())
      {
        theNewVec.ChangeValue(static_cast<size_t>(aNewId.Index)) =
          theOldVec.Value(static_cast<size_t>(anOldId.Index));
      }
    }
  };

  // Same logic for per-kind RefUID vectors.
  auto transferRefUIDs = [&](const auto&                                       theMap,
                             const NCollection_DynamicArray<BRepGraph_RefUID>& theOldVec,
                             NCollection_DynamicArray<BRepGraph_RefUID>&       theNewVec) {
    for (const auto& [anOldId, aNewId] : theMap.Items())
    {
      if (anOldId.IsValidIn(theOldVec)
          && theOldVec.Value(static_cast<size_t>(anOldId.Index)).IsValid())
      {
        theNewVec.ChangeValue(static_cast<size_t>(aNewId.Index)) =
          theOldVec.Value(static_cast<size_t>(anOldId.Index));
      }
    }
  };

  transferUIDs(aVertexMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Vertex),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  transferUIDs(anEdgeMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Edge),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  transferUIDs(aWireMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Wire),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  transferUIDs(aFaceMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Face),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  transferUIDs(aShellMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Shell),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  transferUIDs(aSolidMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Solid),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  transferUIDs(aCompoundMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Compound),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  transferUIDs(aCompSolidMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CompSolid),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));
  transferUIDs(aProductMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Product),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Product));
  transferUIDs(anOccurrenceMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Occurrence),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Occurrence));
  transferUIDs(aCoEdgeMap,
               aGraphData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CoEdge),
               aNewGraphData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CoEdge));

  // Transfer per-kind RefUID vectors. Builder allocates fresh RefUIDs during rebuild;
  // overwrite them with the original UIDs from the old graph so that all RefIdFrom(refUID)
  // lookups remain valid after compaction.
  transferRefUIDs(aVertexRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Vertex),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Vertex));
  transferRefUIDs(aCoEdgeRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::CoEdge),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::CoEdge));
  transferRefUIDs(aWireRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Wire),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Wire));
  transferRefUIDs(aFaceRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Face),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Face));
  transferRefUIDs(aShellRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Shell),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Shell));
  transferRefUIDs(aSolidRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Solid),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Solid));
  transferRefUIDs(aChildRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Child),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Child));
  transferRefUIDs(anOccurrenceRefMap,
                  aGraphData->myIncStorage.RefUIDs(BRepGraph_RefId::Kind::Occurrence),
                  aNewGraphData->myIncStorage.ChangeRefUIDs(BRepGraph_RefId::Kind::Occurrence));

  // Mark UID reverse index dirty so it is rebuilt from the transferred UID vectors
  // on next NodeIdFrom()/Has() call. The reverse index was populated during
  // Add/Add/etc. with intermediate UIDs that are now overwritten.
  {
    std::unique_lock<std::shared_mutex> aUIDLock(aNewGraphData->myUIDToNodeIdMutex);
    aNewGraphData->myUIDToNodeIdDirty = true;
  }
  {
    std::unique_lock<std::shared_mutex> aRefUIDLock(aNewGraphData->myRefUIDToRefIdMutex);
    aNewGraphData->myRefUIDToRefIdDirty = true;
  }

  aNewGraphData->myNextUIDCounter.store(
    aGraphData->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  // Preserve graph generation so transferred UIDs remain valid after compaction.
  aNewGraphData->myGeneration.store(aGraphData->myGeneration.load(std::memory_order_relaxed),
                                    std::memory_order_relaxed);
  aNewGraphData->myIsDone = true;

  // Save layers before swap (default move would transfer empty layers from aNewGraph).
  BRepGraph_LayerRegistry aSavedLayerRegistry = std::move(theGraph.layerRegistry());

  // Transfer TShape-to-NodeId and NodeId-to-OriginalShape bindings: the rebuilt graph has none.
  NCollection_DynamicArray<std::pair<const TopoDS_TShape*, BRepGraph_NodeId>> aTShapeBindings;
  NCollection_DynamicArray<std::pair<BRepGraph_NodeId, TopoDS_Shape>>         aOriginalBindings;
  aGraphData->myIncStorage.ForEachTShapeBinding(
    [&](const TopoDS_TShape* theTShape, const BRepGraph_NodeId& theNodeId) {
      aTShapeBindings.Append({theTShape, theNodeId});
    });
  aGraphData->myIncStorage.ForEachOriginalBinding(
    [&](const BRepGraph_NodeId& theNodeId, const TopoDS_Shape& theShape) {
      aOriginalBindings.Append({theNodeId, theShape});
    });

  // Swap.
  theGraph = std::move(aNewGraph);

  // Restore layers and notify about index remapping.
  theGraph.layerRegistry() = std::move(aSavedLayerRegistry);
  // Direct transientCache() access is intentional here.
  // Compact must clear stale slots keyed by old indices, then re-reserve the
  // cache for the new compacted entity counts before later algorithm use.
  theGraph.transientCache().Clear();
  {
    BRepGraphInc_Storage& aStr                                              = theGraph.incStorage();
    int                   aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]               = aStr.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]                 = aStr.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]               = aStr.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]                 = aStr.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]                 = aStr.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]                = aStr.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]                = aStr.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]             = aStr.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)]            = aStr.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]              = aStr.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)]           = aStr.NbOccurrences();
    int       aReservedKindCount   = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    theGraph.transientCache().Reserve(aReservedKindCount, aCounts);
  }
  // Build unified remap map covering all 8 topology kinds.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  for (const auto& [anOldId, aNewId] : aVertexMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : anEdgeMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aCoEdgeMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aWireMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aFaceMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aShellMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aSolidMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aCompoundMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aCompSolidMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : aProductMap.Items())
    aRemapMap.Bind(anOldId, aNewId);
  for (const auto& [anOldId, aNewId] : anOccurrenceMap.Items())
    aRemapMap.Bind(anOldId, aNewId);

  theGraph.LayerRegistry().DispatchOnCompact(aRemapMap);

  // Restore TShape-to-NodeId and NodeId-to-OriginalShape bindings, remapping NodeIds through
  // aRemapMap. Nodes that were removed (dead, no entry in aRemapMap) are simply dropped.
  for (const auto& [aTShape, aOldId] : aTShapeBindings)
  {
    const BRepGraph_NodeId* aNewId = aRemapMap.Seek(aOldId);
    if (aNewId != nullptr)
      theGraph.incStorage().BindTShapeToNode(aTShape, *aNewId);
  }
  for (const auto& [aOldId, aShape] : aOriginalBindings)
  {
    const BRepGraph_NodeId* aNewId = aRemapMap.Seek(aOldId);
    if (aNewId != nullptr)
      theGraph.incStorage().BindOriginal(*aNewId, aShape);
  }

  // Record history after swap so records survive in the new graph's history log.
  // aRemapMap covers all topology kinds (Vertex..Occurrence + CoEdge).
  if (theOptions.HistoryMode)
  {
    theGraph.History().SetEnabled(true);
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIt(aRemapMap);
         anIt.More();
         anIt.Next())
    {
      NCollection_DynamicArray<BRepGraph_NodeId> aRepl;
      aRepl.Append(anIt.Value());
      theGraph.History().Record(TCollection_AsciiString("Compact:Remap"), anIt.Key(), aRepl);
    }
  }

  theGraph.Editor().CommitMutation();
  theGraph.History().SetEnabled(wasHistoryEnabled);
  return aResult;
}
