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

#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_Validate.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Assert.hxx>

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
  const NCollection_DataMap<BRepGraph_CompSolidId, BRepGraph_CompSolidId>& theCompSolidMap,
  const NCollection_DataMap<BRepGraph_CoEdgeId, BRepGraph_CoEdgeId>&       theCoEdgeMap)
{
  if (!theId.IsValid())
  {
    return BRepGraph_NodeId();
  }

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
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId* aNewId = theCoEdgeMap.Seek(BRepGraph_CoEdgeId(theId));
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
uint32_t countActiveDefs(const BRepGraph& theGraph)
{
  uint32_t aCount = 0;
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
  if (theGraph.IsEmpty())
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

  const uint32_t aTotalRemoved = aResult.NbRemovedVertices + aResult.NbRemovedEdges
                                 + aResult.NbRemovedWires + aResult.NbRemovedFaces
                                 + aResult.NbRemovedShells + aResult.NbRemovedSolids
                                 + aResult.NbRemovedCompounds + aResult.NbRemovedCompSolids;

  aResult.NbNodesBefore = theGraph.Topo().Gen().NbNodes();

  // Short-circuit: nothing to compact.
  if (aTotalRemoved == 0)
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    return aResult;
  }

  const occ::handle<NCollection_IncAllocator> aPermAlloc = new NCollection_IncAllocator();

  // Build old->new index maps for each node kind.
  const uint32_t aNbVertices    = theGraph.Topo().Vertices().Nb();
  const uint32_t aNbEdges       = theGraph.Topo().Edges().Nb();
  const uint32_t aNbCoEdges     = theGraph.Topo().CoEdges().Nb();
  const uint32_t aNbWires       = theGraph.Topo().Wires().Nb();
  const uint32_t aNbFaces       = theGraph.Topo().Faces().Nb();
  const uint32_t aNbShells      = theGraph.Topo().Shells().Nb();
  const uint32_t aNbSolids      = theGraph.Topo().Solids().Nb();
  const uint32_t aNbCompounds   = theGraph.Topo().Compounds().Nb();
  const uint32_t aNbCompSolids  = theGraph.Topo().CompSolids().Nb();
  const uint32_t aNbProducts    = theGraph.Topo().Products().Nb();
  const uint32_t aNbOccurrences = theGraph.Topo().Occurrences().Nb();

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(
    static_cast<size_t>(aNbVertices),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> anEdgeMap(static_cast<size_t>(aNbEdges),
                                                                    aPermAlloc);
  NCollection_DataMap<BRepGraph_CoEdgeId, BRepGraph_CoEdgeId> aCoEdgeMap(
    static_cast<size_t>(aNbCoEdges),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId>   aWireMap(static_cast<size_t>(aNbWires),
                                                                   aPermAlloc);
  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>   aFaceMap(static_cast<size_t>(aNbFaces),
                                                                   aPermAlloc);
  NCollection_DataMap<BRepGraph_ShellId, BRepGraph_ShellId> aShellMap(
    static_cast<size_t>(aNbShells),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_SolidId, BRepGraph_SolidId> aSolidMap(
    static_cast<size_t>(aNbSolids),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_CompoundId, BRepGraph_CompoundId> aCompoundMap(
    static_cast<size_t>(aNbCompounds),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_CompSolidId, BRepGraph_CompSolidId> aCompSolidMap(
    static_cast<size_t>(aNbCompSolids),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_ProductId, BRepGraph_ProductId> aProductMap(
    static_cast<size_t>(aNbProducts),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_OccurrenceId, BRepGraph_OccurrenceId> anOccurrenceMap(
    static_cast<size_t>(aNbOccurrences),
    aPermAlloc);

  // Per-kind ref id remap: old RefId -> new RefId.
  // Built during topology rebuild loops; used for RefUID transfer after swap.
  const uint32_t aNbVertexRefs     = theGraph.Refs().Vertices().Nb();
  const uint32_t aNbWireRefs       = theGraph.Refs().Wires().Nb();
  const uint32_t aNbFaceRefs       = theGraph.Refs().Faces().Nb();
  const uint32_t aNbShellRefs      = theGraph.Refs().Shells().Nb();
  const uint32_t aNbChildRefs      = theGraph.Refs().Children().Nb();
  const uint32_t aNbSolidRefs      = theGraph.Refs().Solids().Nb();
  const uint32_t aNbOccurrenceRefs = theGraph.Refs().Occurrences().Nb();

  NCollection_DataMap<BRepGraph_VertexRefId, BRepGraph_VertexRefId> aVertexRefMap(
    static_cast<size_t>(aNbVertexRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_WireRefId, BRepGraph_WireRefId> aWireRefMap(
    static_cast<size_t>(aNbWireRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_FaceRefId, BRepGraph_FaceRefId> aFaceRefMap(
    static_cast<size_t>(aNbFaceRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_ShellRefId, BRepGraph_ShellRefId> aShellRefMap(
    static_cast<size_t>(aNbShellRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_ChildRefId, BRepGraph_ChildRefId> aChildRefMap(
    static_cast<size_t>(aNbChildRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_SolidRefId, BRepGraph_SolidRefId> aSolidRefMap(
    static_cast<size_t>(aNbSolidRefs),
    aPermAlloc);
  NCollection_DataMap<BRepGraph_OccurrenceRefId, BRepGraph_OccurrenceRefId> anOccurrenceRefMap(
    static_cast<size_t>(aNbOccurrenceRefs),
    aPermAlloc);

  NCollection_Map<BRepGraph_NodeId> aReachableNodes(
    static_cast<size_t>(theGraph.Topo().Gen().NbNodes()),
    aPermAlloc);
  const bool hasRootProducts = !theGraph.RootProductIds().IsEmpty();
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

  if (hasRootProducts)
  {
    const uint32_t aMappedNodes = static_cast<uint32_t>(
      aVertexMap.Size() + anEdgeMap.Size() + aWireMap.Size() + aFaceMap.Size() + aShellMap.Size()
      + aSolidMap.Size() + aCompoundMap.Size() + aCompSolidMap.Size());
    aResult.NbUnmappedActiveDefs = countActiveDefs<BRepGraphInc::VertexDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::EdgeDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::WireDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::FaceDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::ShellDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::SolidDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::CompoundDef>(theGraph)
                                   + countActiveDefs<BRepGraphInc::CompSolidDef>(theGraph)
                                   - aMappedNodes;
  }

  BRepGraph_LayerHistory& aHistory = *theGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const bool              wasHistoryEnabled = aHistory.IsEnabled();
  aHistory.SetEnabled(theOptions.HistoryMode);
  const BRepGraph_Data* aGraphData  = theGraph.data();
  const uint32_t        aGeneration = aGraphData->myIncStorage.Generation();

  // Construct a fresh graph and rebuild bottom-up.
  // Geometry nodes (Surface, Curve) are automatically created through Add/Add.
  BRepGraph       aNewGraph;
  BRepGraph_Data* aNewGraphData = aNewGraph.data();
  aNewGraphData->myIncStorage.SetGeneration(aGeneration);
  aNewGraphData->myIncStorage.SetGraphGUID(aGraphData->myIncStorage.GraphGUID());

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
                       aCompSolidMap,
                       aCoEdgeMap);
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
    const BRepGraph_VertexId       aNewVertexId =
      aNewGraph.Editor().Vertices().Add(anOldVtx.Point, anOldVtx.Tolerance);
    const BRepGraph_VertexId* anExpectedId = aVertexMap.Seek(anIt.CurrentId());
    Standard_ASSERT_RAISE(anExpectedId != nullptr && aNewVertexId == *anExpectedId,
                          "BRepGraph_Compact: unexpected vertex id");
  }

  // Copy OwnGen/SubtreeGen for vertices.
  for (BRepGraph_Iterator<BRepGraphInc::VertexDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_VertexId  anOldId = anIt.CurrentId();
    const BRepGraph_VertexId* aNewId  = aVertexMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::VertexDef& aNewDef = aNewGraphData->myIncStorage.ChangeVertex(*aNewId);
    aNewDef.OwnGen                   = anIt.Current().OwnGen;
    aNewDef.SubtreeGen               = anIt.Current().SubtreeGen;
    aNewDef.UID                      = anIt.Current().UID;
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

    const BRepGraph_VertexRefId aOldStartRefId =
      BRepGraph_Tool::Edge::StartVertexId(theGraph, anOldEdgeId);
    const BRepGraph_VertexId aOldStartId =
      aOldStartRefId.IsValid() ? theGraph.Refs().Vertices().Entry(aOldStartRefId).ChildVertexId
                               : BRepGraph_VertexId();
    const BRepGraph_VertexId    aNewStart = aOldStartId.IsValid()
                                              ? BRepGraph_VertexId::FromNodeId(remapId(aOldStartId))
                                              : BRepGraph_VertexId();
    const BRepGraph_VertexRefId aOldEndRefId =
      BRepGraph_Tool::Edge::EndVertexId(theGraph, anOldEdgeId);
    const BRepGraph_VertexId aOldEndId =
      aOldEndRefId.IsValid() ? theGraph.Refs().Vertices().Entry(aOldEndRefId).ChildVertexId
                             : BRepGraph_VertexId();
    const BRepGraph_VertexId aNewEnd = aOldEndId.IsValid()
                                         ? BRepGraph_VertexId::FromNodeId(remapId(aOldEndId))
                                         : BRepGraph_VertexId();

    // Get the curve handle if available.
    const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(theGraph, anOldEdgeId);

    const auto [aParamFirst, aParamLast] = BRepGraph_Tool::Edge::Range(theGraph, anOldEdgeId);

    const BRepGraph_EdgeId aNewEdgeId = aNewGraph.Editor().Edges().Add(aNewStart,
                                                                       aNewEnd,
                                                                       aCurve,
                                                                       aParamFirst,
                                                                       aParamLast,
                                                                       anOldEdge.Tolerance);

    // Copy edge properties.
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aNewGraph.Editor().Edges().Mut(aNewEdgeId);
    if (anOldEdge.StartVertexRefId.IsValid() && aNewEdge->StartVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anOldStartRef =
        theGraph.Refs().Vertices().Entry(anOldEdge.StartVertexRefId);
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewStartRef =
        aNewGraph.Editor().Vertices().MutRef(aNewEdge->StartVertexRefId);
      aNewGraph.Editor().Vertices().SetRefOrientation(aNewStartRef, anOldStartRef.Orientation);

      aVertexRefMap.Bind(anOldEdge.StartVertexRefId, aNewEdge->StartVertexRefId);
    }

    if (anOldEdge.EndVertexRefId.IsValid() && aNewEdge->EndVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anOldEndRef =
        theGraph.Refs().Vertices().Entry(anOldEdge.EndVertexRefId);
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aNewEndRef =
        aNewGraph.Editor().Vertices().MutRef(aNewEdge->EndVertexRefId);
      aNewGraph.Editor().Vertices().SetRefOrientation(aNewEndRef, anOldEndRef.Orientation);

      aVertexRefMap.Bind(anOldEdge.EndVertexRefId, aNewEdge->EndVertexRefId);
    }
  }

  // Copy OwnGen/SubtreeGen for edges.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId  anOldId = anIt.CurrentId();
    const BRepGraph_EdgeId* aNewId  = anEdgeMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::EdgeDef& aNewDef = aNewGraphData->myIncStorage.ChangeEdge(*aNewId);
    aNewDef.OwnGen                 = anIt.Current().OwnGen;
    aNewDef.SubtreeGen             = anIt.Current().SubtreeGen;
    aNewDef.UID                    = anIt.Current().UID;
  }

  // PCurves (added after edges and faces are known).
  // We need faces first, so do PCurves after face creation.

  // Wires.
  NCollection_LinearVector<BRepGraph_CoEdgeId> aWireOldCoEdges(64);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aNewCoEdgeIds(64);
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    aWireOldCoEdges.Clear(false);
    aNewCoEdgeIds.Clear(false);

    const BRepGraph_WireId anOldWireId = anIt.CurrentId();
    if (aWireMap.Seek(anOldWireId) == nullptr)
    {
      continue;
    }
    const BRepGraph_WireId aNewWireId = aNewGraph.Editor().Wires().Add(aNewCoEdgeIds.ToArray1());
    Standard_ASSERT_RAISE(aNewWireId == *aWireMap.Seek(anOldWireId),
                          "BRepGraph_Compact: unexpected wire id");

    for (BRepGraph_CoEdgesOfWire aRefIt(theGraph, anOldWireId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aRefIt.CurrentId());
      const BRepGraph_EdgeId aNewChildEdgeId =
        BRepGraph_EdgeId::FromNodeId(remapId(aCoEdge.ChildEdgeId));
      if (aNewChildEdgeId.IsValid())
      {
        const BRepGraph_CoEdgeId aNewCoEdgeId =
          aNewGraphData->myIncStorage.CreateCoEdgeUse(aNewWireId,
                                                      aNewChildEdgeId,
                                                      BRepGraph_FaceId(),
                                                      aCoEdge.Orientation);
        aNewGraph.allocateUID(aNewCoEdgeId);
        aNewCoEdgeIds.Append(aNewCoEdgeId);
        aWireOldCoEdges.Append(aRefIt.CurrentId());
      }
    }

    for (size_t aRefIdx = 0; aRefIdx < aWireOldCoEdges.Size() && aRefIdx < aNewCoEdgeIds.Size();
         ++aRefIdx)
    {
      aCoEdgeMap.Bind(aWireOldCoEdges.Value(aRefIdx), aNewCoEdgeIds.Value(aRefIdx));
    }
  }

  // Copy OwnGen/SubtreeGen for wires.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_WireId  anOldId = anIt.CurrentId();
    const BRepGraph_WireId* aNewId  = aWireMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::WireDef& aNewDef = aNewGraphData->myIncStorage.ChangeWire(*aNewId);
    aNewDef.OwnGen                 = anIt.Current().OwnGen;
    aNewDef.SubtreeGen             = anIt.Current().SubtreeGen;
    aNewDef.UID                    = anIt.Current().UID;
  }

  // Faces.
  NCollection_LinearVector<BRepGraph_WireId>    aFaceNextWires(64);
  NCollection_LinearVector<BRepGraph_WireRefId> aFaceOldWireRefs(64);
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    aFaceNextWires.Clear(false);
    aFaceOldWireRefs.Clear(false);

    const BRepGraph_FaceId anOldFaceId = anIt.CurrentId();
    if (aFaceMap.Seek(anOldFaceId) == nullptr)
    {
      continue;
    }
    const BRepGraphInc::FaceDef& anOldFace = anIt.Current();

    const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, anOldFaceId);

    BRepGraph_WireId aFirstWire;

    for (BRepGraph_RefsWireOfFace aRefIt(theGraph, anOldFaceId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aRefIt.CurrentId());
      const BRepGraph_WireId aRemapped = BRepGraph_WireId::FromNodeId(remapId(aWR.ChildWireId));
      if (!aRemapped.IsValid())
      {
        continue;
      }
      if (!aFirstWire.IsValid())
      {
        aFirstWire = aRemapped;
      }
      else
      {
        aFaceNextWires.Append(aRemapped);
      }
      aFaceOldWireRefs.Append(aRefIt.CurrentId());
    }

    const BRepGraph_FaceId aNewFaceId = aNewGraph.Editor().Faces().Add(aSurf,
                                                                       aFirstWire,
                                                                       aFaceNextWires.ToArray1(),
                                                                       anOldFace.Tolerance);

    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace = aNewGraph.Editor().Faces().Mut(aNewFaceId);
    const NCollection_LinearVector<BRepGraph_WireRefId>& aNewWireRefs =
      aNewGraph.Topo().Faces().Relations(aNewFaceId).WireRefIds;

    size_t aWireRefIdx = 0;
    for (const BRepGraph_WireRefId& aNewRefId : aNewWireRefs)
    {
      if (aWireRefIdx >= aFaceOldWireRefs.Size())
      {
        break;
      }
      const BRepGraph_WireRefId    anOldRefId = aFaceOldWireRefs.Value(aWireRefIdx++);
      const BRepGraphInc::WireRef& anOldRef   = theGraph.Refs().Wires().Entry(anOldRefId);
      BRepGraph_MutGuard<BRepGraphInc::WireRef> aNewRef =
        aNewGraph.Editor().Wires().MutRef(aNewRefId);
      aNewGraph.Editor().Wires().SetRefOrientation(aNewRef, anOldRef.Orientation);
      aWireRefMap.Bind(anOldRefId, aNewRefId);
    }
  }

  // Copy OwnGen/SubtreeGen for faces.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_FaceId  anOldId = anIt.CurrentId();
    const BRepGraph_FaceId* aNewId  = aFaceMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::FaceDef& aNewDef = aNewGraphData->myIncStorage.ChangeFace(*aNewId);
    aNewDef.OwnGen                 = anIt.Current().OwnGen;
    aNewDef.SubtreeGen             = anIt.Current().SubtreeGen;
    // Preserve UID from old entity.
    aNewDef.UID = anIt.Current().UID;
  }

  // Populate aCoEdgeMap for free coedges (coedges created by AddPCurve that are
  // not bound to any wire). Wire coedges were already mapped during wire rebuild.
  // Free coedges without wire membership can be remapped by matching
  // (ChildEdgeId, FaceId) pairs against the new graph's coedge set.
  // Note: the new graph's derived relation tables have not yet been rebuilt at this
  // point - see RebuildDerivedRelations() further below - so a direct linear scan
  // is used. Free coedges are rare in practice (only PCurves added outside
  // wires create them) so the O(old_free * new_total) cost is bounded.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId anOldCoEdgeId = anIt.CurrentId();
    if (aCoEdgeMap.IsBound(anOldCoEdgeId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& anOldCE = anIt.Current();
    if (theGraph.incStorage().IsRemoved(anOldCoEdgeId))
    {
      continue;
    }
    const BRepGraph_EdgeId aRemappedEdge =
      BRepGraph_EdgeId::FromNodeId(remapId(anOldCE.ChildEdgeId));
    const BRepGraph_FaceId aRemappedFace = BRepGraph_FaceId::FromNodeId(remapId(anOldCE.FaceId));
    if (!aRemappedEdge.IsValid() || !aRemappedFace.IsValid())
    {
      continue;
    }
    for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aNewIt(aNewGraph); aNewIt.More();
         aNewIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aNewCE = aNewIt.Current();
      if (!aNewGraph.incStorage().IsRemoved(aNewIt.CurrentId())
          && aNewCE.ChildEdgeId == aRemappedEdge && aNewCE.FaceId == aRemappedFace
          && !aCoEdgeMap.IsBound(aNewIt.CurrentId()))
      {
        aCoEdgeMap.Bind(anOldCoEdgeId, aNewIt.CurrentId());
        break;
      }
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

    aNewGraph.Editor().CoEdges().SetFaceId(
      aNewCoEdge,
      BRepGraph_FaceId::FromNodeId(remapId(anOldCoEdge.FaceId)));
    aNewGraph.Editor().CoEdges().SetOrientation(aNewCoEdge, anOldCoEdge.Orientation);

    // If the owning face was removed, keep the coedge as free-wire usage only.
    // Drop face-bound parametric representation (PCurve/UV/continuity) to avoid stale
    // face references causing reconstruction/meshing corruption.
    if (!aNewCoEdge->FaceId.IsValid())
    {
      aNewGraph.Editor().CoEdges().ResetPCurveBinding(aNewCoEdge);
      continue;
    }

    if (anOldCoEdge.Curve2DRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& anOldPCurve =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, anOldCoEdgeId);
      if (!anOldPCurve.IsNull())
      {
        const auto [aCoEdgeParamFirst, aCoEdgeParamLast] =
          BRepGraph_Tool::CoEdge::Range(theGraph, anOldCoEdgeId);
        aNewGraph.Editor().CoEdges().SetPCurve(*aNewCoEdgeId,
                                               anOldPCurve,
                                               aCoEdgeParamFirst,
                                               aCoEdgeParamLast);
      }
    }

    // SetParamRange for coedges without PCurve (face-bound range only).
    if (!anOldCoEdge.Curve2DRepId.IsValid())
    {
      const auto [aCoEdgeParamFirst, aCoEdgeParamLast] =
        BRepGraph_Tool::CoEdge::Range(theGraph, anOldCoEdgeId);
      aNewGraph.Editor().CoEdges().SetParamRange(aNewCoEdge, aCoEdgeParamFirst, aCoEdgeParamLast);
    }
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
      const BRepGraphInc::FaceRef& aFR = theGraph.Refs().Faces().Entry(aRefIt.CurrentId());
      const BRepGraph_FaceId aNewFace  = BRepGraph_FaceId::FromNodeId(remapId(aFR.ChildFaceId));
      if (aNewFace.IsValid())
      {
        const BRepGraph_FaceRefId aNewFaceRefId =
          aNewGraph.Editor().Shells().Append(aNewShellId, aNewFace, aFR.Orientation);
        if (aNewFaceRefId.IsValid())
        {
          aFaceRefMap.Bind(aRefIt.CurrentId(), aNewFaceRefId);
        }
      }
    }
  }

  // Copy OwnGen/SubtreeGen for shells.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_ShellId  anOldId = anIt.CurrentId();
    const BRepGraph_ShellId* aNewId  = aShellMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::ShellDef& aNewDef = aNewGraphData->myIncStorage.ChangeShell(*aNewId);
    aNewDef.OwnGen                  = anIt.Current().OwnGen;
    aNewDef.SubtreeGen              = anIt.Current().SubtreeGen;
    aNewDef.UID                     = anIt.Current().UID;
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
      const BRepGraph_ShellId aNewShell = BRepGraph_ShellId::FromNodeId(remapId(aSR.ChildShellId));
      if (aNewShell.IsValid())
      {
        const BRepGraph_ShellRefId aNewShellRefId =
          aNewGraph.Editor().Solids().Append(aNewSolidId, aNewShell, aSR.Orientation);
        if (aNewShellRefId.IsValid())
        {
          aShellRefMap.Bind(aRefIt.CurrentId(), aNewShellRefId);
        }
      }
    }
  }

  // Copy OwnGen/SubtreeGen for solids.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_SolidId  anOldId = anIt.CurrentId();
    const BRepGraph_SolidId* aNewId  = aSolidMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::SolidDef& aNewDef = aNewGraphData->myIncStorage.ChangeSolid(*aNewId);
    aNewDef.OwnGen                  = anIt.Current().OwnGen;
    aNewDef.SubtreeGen              = anIt.Current().SubtreeGen;
    aNewDef.UID                     = anIt.Current().UID;
  }

  // Compounds may reference other compounds. Create all compound definitions first so parent
  // compounds can attach child refs to compound children independently of storage order.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId  anOldCompoundId = anIt.CurrentId();
    const BRepGraph_CompoundId* aExpectedNewId  = aCompoundMap.Seek(anOldCompoundId);
    if (aExpectedNewId == nullptr)
    {
      continue;
    }

    const NCollection_Array1<BRepGraph_NodeId>  anEmptyChildren;
    [[maybe_unused]] const BRepGraph_CompoundId aNewCompoundId =
      aNewGraph.Editor().Compounds().Add(anEmptyChildren);
    Standard_ASSERT_RAISE(aNewCompoundId == *aExpectedNewId,
                          "BRepGraph_Compact: unexpected compound id");
  }

  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId  anOldCompoundId = anIt.CurrentId();
    const BRepGraph_CompoundId* aNewCompoundId  = aCompoundMap.Seek(anOldCompoundId);
    if (aNewCompoundId == nullptr)
    {
      continue;
    }

    for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, anOldCompoundId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildNodeId);
      if (aNewChild.IsValid())
      {
        const BRepGraph_ChildRefId aNewRefId =
          aNewGraph.Editor().Compounds().Append(*aNewCompoundId, aNewChild, aCR.Orientation);
        if (aNewRefId.IsValid())
        {
          aNewGraph.Editor().Gen().SetChildRefLocalLocation(aNewRefId, aCR.LocalLocation);
          aChildRefMap.Bind(aRefIt.CurrentId(), aNewRefId);
        }
      }
    }
  }

  // Copy OwnGen/SubtreeGen for compounds.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId  anOldId = anIt.CurrentId();
    const BRepGraph_CompoundId* aNewId  = aCompoundMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::CompoundDef& aNewDef = aNewGraphData->myIncStorage.ChangeCompound(*aNewId);
    aNewDef.OwnGen                     = anIt.Current().OwnGen;
    aNewDef.SubtreeGen                 = anIt.Current().SubtreeGen;
    aNewDef.UID                        = anIt.Current().UID;
  }

  // CompSolids.
  NCollection_LinearVector<BRepGraph_SolidId>    aCSSolids(64);
  NCollection_LinearVector<BRepGraph_SolidRefId> aCSOldSolidRefs(64);
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    aCSSolids.Clear(false);
    aCSOldSolidRefs.Clear(false);

    const BRepGraph_CompSolidId anOldCompSolidId = anIt.CurrentId();
    if (aCompSolidMap.Seek(anOldCompSolidId) == nullptr)
    {
      continue;
    }

    for (BRepGraph_RefsSolidOfCompSolid aRefIt(theGraph, anOldCompSolidId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::SolidRef& aSR = theGraph.Refs().Solids().Entry(aRefIt.CurrentId());
      const BRepGraph_SolidId aNewSolid = BRepGraph_SolidId::FromNodeId(remapId(aSR.ChildSolidId));
      if (aNewSolid.IsValid())
      {
        aCSSolids.Append(aNewSolid);
        aCSOldSolidRefs.Append(aRefIt.CurrentId());
      }
    }
    const BRepGraph_CompSolidId aNewCompSolidId =
      aNewGraph.Editor().CompSolids().Add(aCSSolids.ToArray1());
    if (aNewCompSolidId.IsValid())
    {
      const NCollection_LinearVector<BRepGraph_SolidRefId>& aNewSolidRefs =
        aNewGraph.Topo().CompSolids().Relations(aNewCompSolidId).SolidRefIds;
      for (size_t aRefIdx = 0; aRefIdx < aCSOldSolidRefs.Size() && aRefIdx < aNewSolidRefs.Size();
           ++aRefIdx)
      {
        const BRepGraphInc::SolidRef& anOldRef =
          theGraph.Refs().Solids().Entry(aCSOldSolidRefs.Value(aRefIdx));
        BRepGraph_MutGuard<BRepGraphInc::SolidRef> aNewRef =
          aNewGraph.Editor().Solids().MutRef(aNewSolidRefs.Value(aRefIdx));
        aNewGraph.Editor().Solids().SetRefOrientation(aNewRef, anOldRef.Orientation);
        aSolidRefMap.Bind(aCSOldSolidRefs.Value(aRefIdx), aNewSolidRefs.Value(aRefIdx));
      }
    }
  }

  // Copy OwnGen/SubtreeGen for compsolids.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompSolidId  anOldId = anIt.CurrentId();
    const BRepGraph_CompSolidId* aNewId  = aCompSolidMap.Seek(anOldId);
    if (aNewId == nullptr)
    {
      continue;
    }
    BRepGraphInc::CompSolidDef& aNewDef = aNewGraphData->myIncStorage.ChangeCompSolid(*aNewId);
    aNewDef.OwnGen                      = anIt.Current().OwnGen;
    aNewDef.SubtreeGen                  = anIt.Current().SubtreeGen;
    aNewDef.UID                         = anIt.Current().UID;
  }

  // Rebuild relation tables from final incidence to guarantee compact output consistency.
  aNewGraph.incStorage().RebuildDerivedRelations();

  // Validate rebuilt graph before swapping it into the source graph.
  // If rebuilt topology is inconsistent, keep source graph unchanged.
  if (!aNewGraph.Editor().ValidateMutationBoundary())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    aHistory.SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  const BRepGraph_Validate::Result aCompactValidation = BRepGraph_Validate::Perform(aNewGraph);
  if (!aCompactValidation.IsValid())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    aHistory.SetEnabled(wasHistoryEnabled);
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
    if (theGraph.incStorage().IsRemoved(anOldProductId))
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
        for (const BRepGraph_OccurrenceRefId& anOldOccRefId :
             theGraph.Topo().Products().Relations(anOldProductId).OccurrenceRefIds)
        {
          const BRepGraphInc::OccurrenceRef& anOldOccRef =
            theGraph.Refs().Occurrences().Entry(anOldOccRefId);
          if (theGraph.incStorage().IsRemoved(anOldOccRefId))
          {
            continue;
          }
          const BRepGraphInc::OccurrenceDef& anOldOccDef =
            theGraph.Topo().Occurrences().Definition(anOldOccRef.ChildOccurrenceId);
          if (!theGraph.incStorage().IsRemoved(anOldOccRef.ChildOccurrenceId)
              && anOldOccDef.ChildNodeId.IsValid()
              && BRepGraph_NodeId::IsTopologyKind(anOldOccDef.ChildNodeId.NodeKind))
          {
            anOldRootLoc = anOldOccRef.LocalLocation;
            break;
          }
        }
        aNewProductId = aNewGraph.Editor().Products().Add(aNewShapeRoot, anOldRootLoc);
        aNewGraph.Editor().Products().AppendDocumentRoot(aNewProductId);
      }
    }

    if (!aNewProductId.IsValid())
    {
      aNewProductId = aNewGraph.Editor().Products().Add();
      aNewGraph.Editor().Products().AppendDocumentRoot(aNewProductId);
    }

    if (!aNewProductId.IsValid())
    {
      continue;
    }

    aProductMap.Bind(anOldProductId, aNewProductId);

    // Map part root occurrence (topology child occurrence inside product).
    BRepGraph_OccurrenceId    anOldPartOccurrenceId;
    BRepGraph_OccurrenceRefId anOldPartOccurrenceRefId;
    for (const BRepGraph_OccurrenceRefId& anOldOccRefId :
         theGraph.Topo().Products().Relations(anOldProductId).OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& anOldOccRef =
        theGraph.Refs().Occurrences().Entry(anOldOccRefId);
      if (theGraph.incStorage().IsRemoved(anOldOccRefId))
      {
        continue;
      }

      const BRepGraphInc::OccurrenceDef& anOldOccDef =
        theGraph.Topo().Occurrences().Definition(anOldOccRef.ChildOccurrenceId);
      if (theGraph.incStorage().IsRemoved(anOldOccRef.ChildOccurrenceId))
      {
        continue;
      }

      if (anOldOccDef.ChildNodeId.IsValid()
          && BRepGraph_NodeId::IsTopologyKind(anOldOccDef.ChildNodeId.NodeKind))
      {
        anOldPartOccurrenceId    = anOldOccRef.ChildOccurrenceId;
        anOldPartOccurrenceRefId = anOldOccRefId;
        break;
      }
    }

    if (!anOldPartOccurrenceId.IsValid())
    {
      continue;
    }

    for (const BRepGraph_OccurrenceRefId& aNewOccRefId :
         aNewGraph.Topo().Products().Relations(aNewProductId).OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& aNewOccRef =
        aNewGraph.Refs().Occurrences().Entry(aNewOccRefId);
      const BRepGraphInc::OccurrenceDef& aNewOccDef =
        aNewGraph.Topo().Occurrences().Definition(aNewOccRef.ChildOccurrenceId);
      if (aNewGraph.incStorage().IsRemoved(aNewOccRef.ChildOccurrenceId))
      {
        continue;
      }

      if (aNewOccDef.ChildNodeId.IsValid()
          && BRepGraph_NodeId::IsTopologyKind(aNewOccDef.ChildNodeId.NodeKind))
      {
        anOccurrenceMap.Bind(anOldPartOccurrenceId, aNewOccRef.ChildOccurrenceId);
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
    if (theGraph.incStorage().IsRemoved(anOldParentProductId))
    {
      continue;
    }

    const BRepGraph_ProductId* aNewParentId = aProductMap.Seek(anOldParentProductId);
    if (aNewParentId == nullptr)
    {
      continue;
    }

    for (const BRepGraph_OccurrenceRefId& anOldOccRefId :
         theGraph.Topo().Products().Relations(anOldParentProductId).OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& anOldOccRef =
        theGraph.Refs().Occurrences().Entry(anOldOccRefId);
      if (theGraph.incStorage().IsRemoved(anOldOccRefId))
      {
        continue;
      }

      const BRepGraphInc::OccurrenceDef& anOldOccDef =
        theGraph.Topo().Occurrences().Definition(anOldOccRef.ChildOccurrenceId);
      if (theGraph.incStorage().IsRemoved(anOldOccRef.ChildOccurrenceId)
          || !anOldOccDef.ChildNodeId.IsValid()
          || anOldOccDef.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
      {
        continue;
      }

      const BRepGraph_ProductId  anOldChildProductId(anOldOccDef.ChildNodeId);
      const BRepGraph_ProductId* aNewChildId = aProductMap.Seek(anOldChildProductId);
      if (aNewChildId == nullptr)
      {
        continue;
      }

      const BRepGraph_OccurrenceId aNewOccId =
        aNewGraph.Editor().Products().Append(*aNewParentId,
                                             *aNewChildId,
                                             anOldOccRef.LocalLocation);

      if (aNewOccId.IsValid())
      {
        if (!anOccurrenceMap.IsBound(anOldOccRef.ChildOccurrenceId))
        {
          anOccurrenceMap.Bind(anOldOccRef.ChildOccurrenceId, aNewOccId);
        }
        // Bind the copied OccurrenceRef entry for RefUID transfer.
        // Occurrence refs are usage records, so the ref identity must move with
        // the exact parent-owned slot instead of only with the occurrence def.
        for (const BRepGraph_OccurrenceRefId& aNewOccRefId :
             aNewGraph.Topo().Products().Relations(*aNewParentId).OccurrenceRefIds)
        {
          if (aNewGraph.Refs().Occurrences().Entry(aNewOccRefId).ChildOccurrenceId == aNewOccId)
          {
            anOccurrenceRefMap.Bind(anOldOccRefId, aNewOccRefId);
            break;
          }
        }
      }
    }
  }

  aResult.NbNodesAfter = aNewGraph.Topo().Gen().NbNodes();

  // UIDs are stored inline in entity/ref structs. During compaction, entity
  // structs are rebuilt from scratch via Add() calls which allocate fresh UIDs.
  // Recompute per-kind node counters as max(UID)+1 per surviving kind.
  BRepGraphInc_Storage& aNewStorage = aNewGraphData->myIncStorage;
  auto                  recomputeNodeCounter =
    [&](const BRepGraph_NodeId::Kind theKind, const uint32_t theNbEntities, auto&& theAccessor) {
      uint32_t aMaxCounter = 0;
      for (uint32_t anIdx = 0; anIdx < theNbEntities; ++anIdx)
      {
        const uint32_t aUID = theAccessor(anIdx).UID;
        if (aUID > aMaxCounter)
        {
          aMaxCounter = aUID;
        }
      }
      aNewStorage.SetNextNodeUIDCounter(theKind, aMaxCounter + 1);
    };

  recomputeNodeCounter(BRepGraph_NodeId::Kind::Vertex,
                       aNewStorage.NbVertices(),
                       [&](uint32_t i) -> const BRepGraphInc::VertexDef& {
                         return aNewStorage.Vertex(BRepGraph_VertexId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Edge,
                       aNewStorage.NbEdges(),
                       [&](uint32_t i) -> const BRepGraphInc::EdgeDef& {
                         return aNewStorage.Edge(BRepGraph_EdgeId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::CoEdge,
                       aNewStorage.NbCoEdges(),
                       [&](uint32_t i) -> const BRepGraphInc::CoEdgeDef& {
                         return aNewStorage.CoEdge(BRepGraph_CoEdgeId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Wire,
                       aNewStorage.NbWires(),
                       [&](uint32_t i) -> const BRepGraphInc::WireDef& {
                         return aNewStorage.Wire(BRepGraph_WireId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Face,
                       aNewStorage.NbFaces(),
                       [&](uint32_t i) -> const BRepGraphInc::FaceDef& {
                         return aNewStorage.Face(BRepGraph_FaceId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Shell,
                       aNewStorage.NbShells(),
                       [&](uint32_t i) -> const BRepGraphInc::ShellDef& {
                         return aNewStorage.Shell(BRepGraph_ShellId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Solid,
                       aNewStorage.NbSolids(),
                       [&](uint32_t i) -> const BRepGraphInc::SolidDef& {
                         return aNewStorage.Solid(BRepGraph_SolidId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Compound,
                       aNewStorage.NbCompounds(),
                       [&](uint32_t i) -> const BRepGraphInc::CompoundDef& {
                         return aNewStorage.Compound(BRepGraph_CompoundId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::CompSolid,
                       aNewStorage.NbCompSolids(),
                       [&](uint32_t i) -> const BRepGraphInc::CompSolidDef& {
                         return aNewStorage.CompSolid(BRepGraph_CompSolidId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Product,
                       aNewStorage.NbProducts(),
                       [&](uint32_t i) -> const BRepGraphInc::ProductDef& {
                         return aNewStorage.Product(BRepGraph_ProductId(i));
                       });
  recomputeNodeCounter(BRepGraph_NodeId::Kind::Occurrence,
                       aNewStorage.NbOccurrences(),
                       [&](uint32_t i) -> const BRepGraphInc::OccurrenceDef& {
                         return aNewStorage.Occurrence(BRepGraph_OccurrenceId(i));
                       });

  // Recompute per-kind ref counters as max(UID)+1 per surviving ref kind.
  auto recomputeRefCounter =
    [&](const BRepGraph_RefId::Kind theKind, const uint32_t theNbRefs, auto&& theAccessor) {
      uint32_t aMaxCounter = 0;
      for (uint32_t anIdx = 0; anIdx < theNbRefs; ++anIdx)
      {
        const uint32_t aUID = theAccessor(anIdx).UID;
        if (aUID > aMaxCounter)
        {
          aMaxCounter = aUID;
        }
      }
      aNewStorage.SetNextRefUIDCounter(theKind, aMaxCounter + 1);
    };

  recomputeRefCounter(BRepGraph_RefId::Kind::Shell,
                      aNewStorage.NbShellRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::ShellRef& {
                        return aNewStorage.ShellRef(BRepGraph_ShellRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Face,
                      aNewStorage.NbFaceRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::FaceRef& {
                        return aNewStorage.FaceRef(BRepGraph_FaceRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Wire,
                      aNewStorage.NbWireRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::WireRef& {
                        return aNewStorage.WireRef(BRepGraph_WireRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Vertex,
                      aNewStorage.NbVertexRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::VertexRef& {
                        return aNewStorage.VertexRef(BRepGraph_VertexRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Solid,
                      aNewStorage.NbSolidRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::SolidRef& {
                        return aNewStorage.SolidRef(BRepGraph_SolidRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Child,
                      aNewStorage.NbChildRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::ChildRef& {
                        return aNewStorage.ChildRef(BRepGraph_ChildRefId(i));
                      });
  recomputeRefCounter(BRepGraph_RefId::Kind::Occurrence,
                      aNewStorage.NbOccurrenceRefs(),
                      [&](uint32_t i) -> const BRepGraphInc::OccurrenceRef& {
                        return aNewStorage.OccurrenceRef(BRepGraph_OccurrenceRefId(i));
                      });

  // Preserve graph generation so inline UIDs remain valid after compaction.
  aNewStorage.SetGeneration(aGraphData->myIncStorage.Generation());
  aNewStorage.SetGraphGUID(aGraphData->myIncStorage.GraphGUID());

  // Rebuild UID reverse indexes from inline UIDs (skipping removed entities).
  aNewStorage.RebuildUIDReverseIndexes();

  static const TCollection_AsciiString THE_COMPACT_REMAP_LABEL("Compact:Remap");

  // Save layers before swap (default move would transfer empty layers from aNewGraph).
  BRepGraph_LayerRegistry aSavedLayerRegistry = std::move(theGraph.layerRegistry());
  BRepGraph_CacheRegistry aSavedCacheRegistry = std::move(theGraph.cacheRegistry());

  // Transfer shape-to-NodeId and NodeId-to-OriginalShape bindings: the rebuilt graph has none.
  NCollection_LinearVector<std::pair<TopoDS_Shape, BRepGraph_NodeId>> aShapeBindings;
  NCollection_LinearVector<std::pair<BRepGraph_NodeId, TopoDS_Shape>> aOriginalBindings;
  aGraphData->myIncStorage.ForEachShapeBinding(
    [&](const TopoDS_Shape& theShape, const BRepGraph_NodeId& theNodeId) {
      aShapeBindings.Append({theShape, theNodeId});
    });
  aGraphData->myIncStorage.ForEachOriginalBinding(
    [&](const BRepGraph_NodeId& theNodeId, const TopoDS_Shape& theShape) {
      aOriginalBindings.Append({theNodeId, theShape});
    });

  // Build full ItemId->ItemId remap covering nodes, refs, and exact geometry reps (not mesh).
  NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId> anItemRemap;
  for (const auto& [anOldId, aNewId] : aVertexMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : anEdgeMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aCoEdgeMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aWireMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aFaceMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aShellMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aSolidMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aCompoundMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aCompSolidMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aProductMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : anOccurrenceMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aVertexRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aWireRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aFaceRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aShellRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aChildRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : aSolidRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  for (const auto& [anOldId, aNewId] : anOccurrenceRefMap.Items())
  {
    anItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
  // RepId records are session-local and not part of ItemId remap.

  if (theOptions.CacheMode == BRepGraph_Compact::Options::CachePolicy::CopyFresh)
  {
    aSavedCacheRegistry.CopyFreshCachesTo(aNewGraph,
                                          anItemRemap,
                                          BRepGraph_CopyRemap::Mode::Compact);
    aSavedCacheRegistry.Clear();
  }

  // Swap.
  theGraph = std::move(aNewGraph);

  // Restore layers and notify about index remapping.
  theGraph.layerRegistry() = std::move(aSavedLayerRegistry);
  if (theOptions.CacheMode == BRepGraph_Compact::Options::CachePolicy::Drop)
  {
    theGraph.cacheRegistry() = std::move(aSavedCacheRegistry);
  }
  theGraph.initViewsAndRegistries();
  if (theOptions.CacheMode == BRepGraph_Compact::Options::CachePolicy::Drop)
  {
    theGraph.cacheRegistry().ClearAll();
  }

  // Migrate all layer data through CopyTo with full item remap.
  theGraph.LayerRegistry().CopyLayersTo(theGraph, anItemRemap, BRepGraph_CopyRemap::Mode::Compact);

  // Build unified NodeId remap for TShape bindings and history recording.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap(
    static_cast<size_t>(theGraph.Topo().Gen().NbNodes()),
    aPermAlloc);
  for (const auto& [anOldId, aNewId] : aVertexMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : anEdgeMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aCoEdgeMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aWireMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aFaceMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aShellMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aSolidMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aCompoundMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aCompSolidMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : aProductMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }
  for (const auto& [anOldId, aNewId] : anOccurrenceMap.Items())
  {
    aRemapMap.Bind(anOldId, aNewId);
  }

  // Restore shape-to-NodeId and NodeId-to-OriginalShape bindings, remapping NodeIds through
  // aRemapMap. Nodes that were removed (dead, no entry in aRemapMap) are simply dropped.
  for (const auto& [aShape, aOldId] : aShapeBindings)
  {
    const BRepGraph_NodeId* aNewId = aRemapMap.Seek(aOldId);
    if (aNewId != nullptr)
    {
      theGraph.incStorage().SetDefinitionShapeBinding(aShape, *aNewId);
    }
  }
  for (const auto& [aOldId, aShape] : aOriginalBindings)
  {
    const BRepGraph_NodeId* aNewId = aRemapMap.Seek(aOldId);
    if (aNewId != nullptr)
    {
      theGraph.incStorage().BindOriginal(*aNewId, aShape);
    }
  }

  if (theOptions.HistoryMode)
  {
    // Re-acquire history layer: CopyLayersTo(Compact) replaced the old instance.
    BRepGraph_LayerHistory& aNewHistory =
      *theGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
    aNewHistory.SetEnabled(true);
    aWireOldCoEdges.Clear(true);
    aFaceNextWires.Clear(true);
    aFaceOldWireRefs.Clear(true);
    aCSSolids.Clear(true);
    aCSOldSolidRefs.Clear(true);

    NCollection_LinearVector<BRepGraph_NodeId> aNewOriginals(64);
    NCollection_LinearVector<BRepGraph_NodeId> aNewReplacements(64);
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator aMapIt(aRemapMap);
         aMapIt.More();
         aMapIt.Next())
    {
      aNewOriginals.Append(aMapIt.Key());
      aNewReplacements.Append(aMapIt.Value());
    }
    if (!aNewOriginals.IsEmpty())
    {
      aNewHistory.RecordBatch(THE_COMPACT_REMAP_LABEL,
                              aNewOriginals.ToArray1(),
                              aNewReplacements.ToArray1());
    }
  }

  theGraph.Editor().CommitMutation();
  // Re-acquire history (may be new instance after CopyLayersTo) to restore enabled state.
  BRepGraph_LayerHistory& aFinalHistory =
    *theGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  aFinalHistory.SetEnabled(wasHistoryEnabled);
  return aResult;
}
