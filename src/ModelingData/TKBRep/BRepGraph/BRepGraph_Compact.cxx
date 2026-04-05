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

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_UID.hxx>

#include <NCollection_DataMap.hxx>

#include <utility>

namespace
{

//! Remap a NodeId through old->new index maps. Returns invalid NodeId if not found.
BRepGraph_NodeId remapNodeId(const BRepGraph_NodeId&              theId,
                             const NCollection_DataMap<int, int>& theVertexMap,
                             const NCollection_DataMap<int, int>& theEdgeMap,
                             const NCollection_DataMap<int, int>& theWireMap,
                             const NCollection_DataMap<int, int>& theFaceMap,
                             const NCollection_DataMap<int, int>& theShellMap,
                             const NCollection_DataMap<int, int>& theSolidMap,
                             const NCollection_DataMap<int, int>& theCompoundMap,
                             const NCollection_DataMap<int, int>& theCompSolidMap)
{
  if (!theId.IsValid())
    return BRepGraph_NodeId();

  const NCollection_DataMap<int, int>* aMap = nullptr;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      aMap = &theVertexMap;
      break;
    case BRepGraph_NodeId::Kind::Edge:
      aMap = &theEdgeMap;
      break;
    case BRepGraph_NodeId::Kind::Wire:
      aMap = &theWireMap;
      break;
    case BRepGraph_NodeId::Kind::Face:
      aMap = &theFaceMap;
      break;
    case BRepGraph_NodeId::Kind::Shell:
      aMap = &theShellMap;
      break;
    case BRepGraph_NodeId::Kind::Solid:
      aMap = &theSolidMap;
      break;
    case BRepGraph_NodeId::Kind::Compound:
      aMap = &theCompoundMap;
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      aMap = &theCompSolidMap;
      break;
    default:
      // Product/Occurrence kinds are not compacted - they reference topology
      // nodes which are remapped independently.
      break;
  }

  if (aMap == nullptr)
    return BRepGraph_NodeId();

  const int* aNewIdx = aMap->Seek(theId.Index);
  if (aNewIdx == nullptr)
    return BRepGraph_NodeId();

  return BRepGraph_NodeId(theId.NodeKind, *aNewIdx);
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
void bindActiveIndexMap(const BRepGraph& theGraph, NCollection_DataMap<int, int>& theMap)
{
  int aNewIdx = 0;
  for (BRepGraph_Iterator<NodeDefT> anIt(theGraph); anIt.More(); anIt.Next())
  {
    theMap.Bind(anIt.CurrentId().Index, aNewIdx++);
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
  NCollection_DataMap<int, int> aVertexMap, anEdgeMap, aCoEdgeMap, aWireMap, aFaceMap;
  NCollection_DataMap<int, int> aShellMap, aSolidMap, aCompoundMap, aCompSolidMap;

  bindActiveIndexMap<BRepGraphInc::VertexDef>(theGraph, aVertexMap);
  bindActiveIndexMap<BRepGraphInc::EdgeDef>(theGraph, anEdgeMap);
  bindActiveIndexMap<BRepGraphInc::WireDef>(theGraph, aWireMap);
  bindActiveIndexMap<BRepGraphInc::FaceDef>(theGraph, aFaceMap);
  bindActiveIndexMap<BRepGraphInc::ShellDef>(theGraph, aShellMap);
  bindActiveIndexMap<BRepGraphInc::SolidDef>(theGraph, aSolidMap);
  bindActiveIndexMap<BRepGraphInc::CompoundDef>(theGraph, aCompoundMap);
  bindActiveIndexMap<BRepGraphInc::CompSolidDef>(theGraph, aCompSolidMap);

  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);
  const BRepGraph_Data* aGraphData  = theGraph.data();
  const uint32_t        aGeneration = aGraphData->myGeneration.load(std::memory_order_relaxed);

  // Record history before swap (remap entries for topology defs).
  if (theOptions.HistoryMode)
  {
    for (const auto& [aOldIdx, aNewIdx] : aVertexMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_VertexId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_VertexId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : anEdgeMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_EdgeId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_EdgeId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aFaceMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_FaceId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_FaceId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aWireMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_WireId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_WireId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aShellMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_ShellId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_ShellId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aSolidMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_SolidId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_SolidId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aCompoundMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_CompoundId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_CompoundId(aOldIdx),
                                  aRepl);
      }
    }
    for (const auto& [aOldIdx, aNewIdx] : aCompSolidMap.Items())
    {
      if (aOldIdx != aNewIdx)
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_CompSolidId(aNewIdx));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_CompSolidId(aOldIdx),
                                  aRepl);
      }
    }
  }

  // Construct a fresh graph and rebuild bottom-up.
  // Geometry nodes (Surface, Curve) are automatically created through AddFace/AddEdge.
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
    const BRepGraphInc::VertexDef& anOldVtx = anIt.Current();
    (void)aNewGraph.Builder().AddVertex(anOldVtx.Point, anOldVtx.Tolerance);
  }

  // Edges.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId       anOldEdgeId = anIt.CurrentId();
    const BRepGraphInc::EdgeDef& anOldEdge   = anIt.Current();

    const BRepGraph_VertexId aNewStart =
      anOldEdge.StartVertexRefId.IsValid()
        ? BRepGraph_VertexId::FromNodeId(
            remapId(BRepGraph_Tool::Edge::StartVertex(theGraph, anOldEdgeId).VertexDefId))
        : BRepGraph_VertexId();
    const BRepGraph_VertexId aNewEnd =
      anOldEdge.EndVertexRefId.IsValid()
        ? BRepGraph_VertexId::FromNodeId(
            remapId(BRepGraph_Tool::Edge::EndVertex(theGraph, anOldEdgeId).VertexDefId))
        : BRepGraph_VertexId();

    // Get the curve handle if available.
    const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(theGraph, anOldEdgeId);

    const BRepGraph_EdgeId aNewEdgeId = aNewGraph.Builder().AddEdge(aNewStart,
                                                                    aNewEnd,
                                                                    aCurve,
                                                                    anOldEdge.ParamFirst,
                                                                    anOldEdge.ParamLast,
                                                                    anOldEdge.Tolerance);

    // Copy edge properties.
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aNewGraph.Builder().MutEdge(aNewEdgeId);
    aNewEdge->IsDegenerate                             = anOldEdge.IsDegenerate;
    aNewEdge->SameParameter                            = anOldEdge.SameParameter;
    aNewEdge->SameRange                                = anOldEdge.SameRange;
  }

  // PCurves (added after edges and faces are known).
  // We need faces first, so do PCurves after face creation.

  // Wires.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_WireId anOldWireId = anIt.CurrentId();

    NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    NCollection_Vector<BRepGraph_CoEdgeId>                              anOldCoEdges;
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
      }
    }
    const int aNewFirstCoEdge = aNewGraph.Topo().CoEdges().Nb();
    (void)aNewGraph.Builder().AddWire(aNewEntries);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < anOldCoEdges.Length(); ++aCoEdgeIdx)
    {
      aCoEdgeMap.Bind(anOldCoEdges.Value(aCoEdgeIdx).Index, aNewFirstCoEdge + aCoEdgeIdx);
    }
  }

  // Faces.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_FaceId       anOldFaceId = anIt.CurrentId();
    const BRepGraphInc::FaceDef& anOldFace   = anIt.Current();

    const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, anOldFaceId);

    // Find outer wire from transitional incidence entries.
    BRepGraph_WireId                     aNewOuterWire;
    NCollection_Vector<BRepGraph_WireId> aNewInnerWires;

    for (BRepGraph_RefsWireOfFace aRefIt(theGraph, anOldFaceId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::WireRef& aWR       = theGraph.Refs().Wires().Entry(aRefIt.CurrentId());
      const BRepGraph_WireId       aRemapped = BRepGraph_WireId::FromNodeId(remapId(aWR.WireDefId));
      if (!aRemapped.IsValid())
        continue;
      if (aWR.IsOuter)
      {
        aNewOuterWire = aRemapped;
      }
      else
      {
        aNewInnerWires.Append(aRemapped);
      }
    }

    (void)aNewGraph.Builder().AddFace(aSurf, aNewOuterWire, aNewInnerWires, anOldFace.Tolerance);

    // Copy triangulations from old FaceDef to new FaceDef.
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace =
      aNewGraph.Builder().MutFace(BRepGraph_FaceId(aFaceMap.Find(anOldFaceId.Index)));
    aNewFace->TriangulationRepIds      = anOldFace.TriangulationRepIds;
    aNewFace->ActiveTriangulationIndex = anOldFace.ActiveTriangulationIndex;
  }

  // Add PCurves to edges in the new graph via CoEdge data.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId anOldEdgeId = anIt.CurrentId();
    const int              aNewEdgeIdx = anEdgeMap.Find(anOldEdgeId.Index);

    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().Edges().CoEdges(anOldEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const BRepGraph_FaceId aNewFaceId = BRepGraph_FaceId::FromNodeId(remapId(aCoEdge.FaceDefId));
      if (!aNewFaceId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCompactPCurve =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      aNewGraph.Builder().AddPCurveToEdge(BRepGraph_EdgeId(aNewEdgeIdx),
                                          aNewFaceId,
                                          aCompactPCurve,
                                          aCoEdge.ParamFirst,
                                          aCoEdge.ParamLast,
                                          aCoEdge.Orientation);
    }
  }

  // Shells.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_ShellId anOldShellId = anIt.CurrentId();

    const BRepGraph_ShellId aNewShellId = aNewGraph.Builder().AddShell();

    // Add faces to shell via transitional incidence entries.
    for (BRepGraph_RefsFaceOfShell aRefIt(theGraph, anOldShellId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR      = theGraph.Refs().Faces().Entry(aRefIt.CurrentId());
      const BRepGraph_FaceId       aNewFace = BRepGraph_FaceId::FromNodeId(remapId(aFR.FaceDefId));
      if (aNewFace.IsValid())
        aNewGraph.Builder().AddFaceToShell(aNewShellId, aNewFace, aFR.Orientation);
    }
  }

  // Solids.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_SolidId anOldSolidId = anIt.CurrentId();

    const BRepGraph_SolidId aNewSolidId = aNewGraph.Builder().AddSolid();

    for (BRepGraph_RefsShellOfSolid aRefIt(theGraph, anOldSolidId); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR = theGraph.Refs().Shells().Entry(aRefIt.CurrentId());
      const BRepGraph_ShellId aNewShell = BRepGraph_ShellId::FromNodeId(remapId(aSR.ShellDefId));
      if (aNewShell.IsValid())
        aNewGraph.Builder().AddShellToSolid(aNewSolidId, aNewShell, aSR.Orientation);
    }
  }

  // Compounds.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompoundId anOldCompoundId = anIt.CurrentId();

    NCollection_Vector<BRepGraph_NodeId> aNewChildren;
    for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, anOldCompoundId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildDefId);
      if (aNewChild.IsValid())
        aNewChildren.Append(aNewChild);
    }
    (void)aNewGraph.Builder().AddCompound(aNewChildren);
  }

  // CompSolids.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CompSolidId anOldCompSolidId = anIt.CurrentId();

    NCollection_Vector<BRepGraph_SolidId> aNewSolids;
    for (BRepGraph_RefsSolidOfCompSolid aRefIt(theGraph, anOldCompSolidId); aRefIt.More();
         aRefIt.Next())
    {
      const BRepGraphInc::SolidRef& aSR = theGraph.Refs().Solids().Entry(aRefIt.CurrentId());
      const BRepGraph_SolidId aNewSolid = BRepGraph_SolidId::FromNodeId(remapId(aSR.SolidDefId));
      if (aNewSolid.IsValid())
        aNewSolids.Append(aNewSolid);
    }
    (void)aNewGraph.Builder().AddCompSolid(aNewSolids);
  }

  // Rebuild reverse index from final forward incidence to guarantee compact output consistency.
  aNewGraph.incStorage().BuildReverseIndex();

  aResult.NbNodesAfter = static_cast<int>(aNewGraph.Topo().Gen().NbNodes());

  // Transfer per-kind UID vectors from old graph to new graph using index remap maps.
  // Each new graph's UID vector[newIdx] = old graph's UID vector[oldIdx].
  auto transferUIDs = [&](const NCollection_DataMap<int, int>&     theMap,
                          const NCollection_Vector<BRepGraph_UID>& theOldVec,
                          NCollection_Vector<BRepGraph_UID>&       theNewVec) {
    // New vector was already populated by BuilderView during reconstruction.
    // Overwrite entries that have a mapping from the old graph.
    for (const auto& [anOldIdx, aNewIdx] : theMap.Items())
    {
      if (anOldIdx < theOldVec.Length() && theOldVec.Value(anOldIdx).IsValid())
      {
        theNewVec.ChangeValue(aNewIdx) = theOldVec.Value(anOldIdx);
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

  // Mark UID reverse index dirty so it is rebuilt from the transferred UID vectors
  // on next NodeIdFrom()/Has() call. The reverse index was populated during
  // AddVertex/AddEdge/etc. with intermediate UIDs that are now overwritten.
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
  aNewGraphData->myGeneration.store(aGraphData->myGeneration.load(std::memory_order_relaxed),
                                    std::memory_order_relaxed);
  aNewGraphData->myIsDone = true;

  // Save layers before swap (default move would transfer empty layers from aNewGraph).
  BRepGraph_LayerRegistry aSavedLayerRegistry = std::move(theGraph.layerRegistry());

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
  for (const auto& [aOldIdx, aNewIdx] : aVertexMap.Items())
    aRemapMap.Bind(BRepGraph_VertexId(aOldIdx), BRepGraph_VertexId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : anEdgeMap.Items())
    aRemapMap.Bind(BRepGraph_EdgeId(aOldIdx), BRepGraph_EdgeId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aCoEdgeMap.Items())
    aRemapMap.Bind(BRepGraph_CoEdgeId(aOldIdx), BRepGraph_CoEdgeId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aWireMap.Items())
    aRemapMap.Bind(BRepGraph_WireId(aOldIdx), BRepGraph_WireId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aFaceMap.Items())
    aRemapMap.Bind(BRepGraph_FaceId(aOldIdx), BRepGraph_FaceId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aShellMap.Items())
    aRemapMap.Bind(BRepGraph_ShellId(aOldIdx), BRepGraph_ShellId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aSolidMap.Items())
    aRemapMap.Bind(BRepGraph_SolidId(aOldIdx), BRepGraph_SolidId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aCompoundMap.Items())
    aRemapMap.Bind(BRepGraph_CompoundId(aOldIdx), BRepGraph_CompoundId(aNewIdx));
  for (const auto& [aOldIdx, aNewIdx] : aCompSolidMap.Items())
    aRemapMap.Bind(BRepGraph_CompSolidId(aOldIdx), BRepGraph_CompSolidId(aNewIdx));

  theGraph.LayerRegistry().DispatchOnCompact(aRemapMap);

  theGraph.Builder().CommitMutation();
  theGraph.History().SetEnabled(wasHistoryEnabled);
  return aResult;
}
