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
#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Load.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Replace.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_Validate.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_Assert.hxx>

#include <utility>

namespace
{

template <typename IdT>
using IdRemap = NCollection_FlatDataMap<IdT, IdT>;

BRepGraph_Validate::Options compactValidationOptions(const BRepGraph_Compact::Options& theOptions)
{
  return theOptions.ValidationMode == BRepGraph_Compact::Options::ValidationPolicy::Audit
           ? BRepGraph_Validate::Options::Audit()
           : BRepGraph_Validate::Options::Lightweight();
}

bool isValidCompactCandidate(const BRepGraph&                  theGraph,
                             const BRepGraph_Compact::Options& theOptions)
{
  return BRepGraph_Validate::Perform(theGraph, compactValidationOptions(theOptions)).IsValid();
}

//! Remap a NodeId through old->new index maps. Returns invalid NodeId if not found.
BRepGraph_NodeId remapNodeId(const BRepGraph_NodeId&               theId,
                             const IdRemap<BRepGraph_VertexId>&    theVertexMap,
                             const IdRemap<BRepGraph_EdgeId>&      theEdgeMap,
                             const IdRemap<BRepGraph_WireId>&      theWireMap,
                             const IdRemap<BRepGraph_FaceId>&      theFaceMap,
                             const IdRemap<BRepGraph_ShellId>&     theShellMap,
                             const IdRemap<BRepGraph_SolidId>&     theSolidMap,
                             const IdRemap<BRepGraph_CompoundId>&  theCompoundMap,
                             const IdRemap<BRepGraph_CompSolidId>& theCompSolidMap,
                             const IdRemap<BRepGraph_CoEdgeId>&    theCoEdgeMap)
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

template <typename IdMapT>
void bindItemRemap(NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
                   const IdMapT&                                                theIdMap)
{
  for (const auto& [anOldId, aNewId] : theIdMap.Items())
  {
    theItemRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
  }
}

template <typename NodeDefT, typename IdMapT>
void bindActiveIndexMap(const BRepGraph& theGraph, IdMapT& theMap)
{
  using IdT      = typename BRepGraph_Iterator<NodeDefT>::TypedId;
  IdT aNextNewId = IdT::Start();
  for (BRepGraph_Iterator<NodeDefT> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const IdT aNodeId = anIt.CurrentId();
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
  if (!theGraph.IsValid())
  {
    return aResult;
  }
  if (theGraph.IsEmpty())
  {
    aResult.StatusCode = Status::NoChange;
    return aResult;
  }

  BRepGraphInc_Storage&           aStorage       = theGraph.incStorage();
  const BRepGraphInc_Load::Counts aCounts        = aStorage.Counts();
  const BRepGraphInc_Load::Counts anActiveCounts = aStorage.ActiveCounts();

  // Count removed storage slots per kind.
  aResult.NbRemovedVertices    = aCounts.NbVertices - anActiveCounts.NbVertices;
  aResult.NbRemovedEdges       = aCounts.NbEdges - anActiveCounts.NbEdges;
  aResult.NbRemovedCoEdges     = aCounts.NbCoEdges - anActiveCounts.NbCoEdges;
  aResult.NbRemovedWires       = aCounts.NbWires - anActiveCounts.NbWires;
  aResult.NbRemovedFaces       = aCounts.NbFaces - anActiveCounts.NbFaces;
  aResult.NbRemovedShells      = aCounts.NbShells - anActiveCounts.NbShells;
  aResult.NbRemovedSolids      = aCounts.NbSolids - anActiveCounts.NbSolids;
  aResult.NbRemovedCompounds   = aCounts.NbCompounds - anActiveCounts.NbCompounds;
  aResult.NbRemovedCompSolids  = aCounts.NbCompSolids - anActiveCounts.NbCompSolids;
  aResult.NbRemovedProducts    = aCounts.NbProducts - anActiveCounts.NbProducts;
  aResult.NbRemovedOccurrences = aCounts.NbOccurrences - anActiveCounts.NbOccurrences;
  aResult.NbRemovedRefs        = (aCounts.NbShellRefs - anActiveCounts.NbShellRefs)
                                 + (aCounts.NbFaceRefs - anActiveCounts.NbFaceRefs)
                                 + (aCounts.NbWireRefs - anActiveCounts.NbWireRefs)
                                 + (aCounts.NbVertexRefs - anActiveCounts.NbVertexRefs)
                                 + (aCounts.NbSolidRefs - anActiveCounts.NbSolidRefs)
                                 + (aCounts.NbChildRefs - anActiveCounts.NbChildRefs)
                                 + (aCounts.NbOccurrenceRefs - anActiveCounts.NbOccurrenceRefs);
  aResult.NbRemovedReps =
    (aCounts.NbFaceSurfaceReps - anActiveCounts.NbFaceSurfaceReps)
    + (aCounts.NbEdgeCurve3DReps - anActiveCounts.NbEdgeCurve3DReps)
    + (aCounts.NbCoEdgeCurve2DReps - anActiveCounts.NbCoEdgeCurve2DReps)
    + (aCounts.NbFaceTriangulationReps - anActiveCounts.NbFaceTriangulationReps)
    + (aCounts.NbEdgePolygon3DReps - anActiveCounts.NbEdgePolygon3DReps)
    + (aCounts.NbCoEdgePolygon2DReps - anActiveCounts.NbCoEdgePolygon2DReps)
    + (aCounts.NbCoEdgePolygonOnTriReps - anActiveCounts.NbCoEdgePolygonOnTriReps);

  const uint32_t aTotalRemoved =
    aResult.NbRemovedVertices + aResult.NbRemovedEdges + aResult.NbRemovedCoEdges
    + aResult.NbRemovedWires + aResult.NbRemovedFaces + aResult.NbRemovedShells
    + aResult.NbRemovedSolids + aResult.NbRemovedCompounds + aResult.NbRemovedCompSolids
    + aResult.NbRemovedProducts + aResult.NbRemovedOccurrences + aResult.NbRemovedRefs
    + aResult.NbRemovedReps;

  aResult.NbNodesBefore = theGraph.Topo().Gen().NbNodes();

  // Avoid rebuilding an already dense graph. Supplemental tombstones require the same detached
  // publication path as core tombstones so a throwing custom store cannot partially mutate source.
  if (aTotalRemoved == 0 && !theGraph.supplementStorage().NeedsCompaction())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    aResult.StatusCode   = Status::NoChange;
    return aResult;
  }

  // Build old->new index maps for each node kind.
  IdRemap<BRepGraph_VertexId>     aVertexMap(static_cast<size_t>(aCounts.NbVertices));
  IdRemap<BRepGraph_EdgeId>       anEdgeMap(static_cast<size_t>(aCounts.NbEdges));
  IdRemap<BRepGraph_CoEdgeId>     aCoEdgeMap(static_cast<size_t>(aCounts.NbCoEdges));
  IdRemap<BRepGraph_WireId>       aWireMap(static_cast<size_t>(aCounts.NbWires));
  IdRemap<BRepGraph_FaceId>       aFaceMap(static_cast<size_t>(aCounts.NbFaces));
  IdRemap<BRepGraph_ShellId>      aShellMap(static_cast<size_t>(aCounts.NbShells));
  IdRemap<BRepGraph_SolidId>      aSolidMap(static_cast<size_t>(aCounts.NbSolids));
  IdRemap<BRepGraph_CompoundId>   aCompoundMap(static_cast<size_t>(aCounts.NbCompounds));
  IdRemap<BRepGraph_CompSolidId>  aCompSolidMap(static_cast<size_t>(aCounts.NbCompSolids));
  IdRemap<BRepGraph_ProductId>    aProductMap(static_cast<size_t>(aCounts.NbProducts));
  IdRemap<BRepGraph_OccurrenceId> anOccurrenceMap(static_cast<size_t>(aCounts.NbOccurrences));

  // Per-kind ref id remap: old RefId -> new RefId.
  // Built during topology rebuild loops; used for RefUID transfer after swap.
  IdRemap<BRepGraph_VertexRefId>     aVertexRefMap(static_cast<size_t>(aCounts.NbVertexRefs));
  IdRemap<BRepGraph_WireRefId>       aWireRefMap(static_cast<size_t>(aCounts.NbWireRefs));
  IdRemap<BRepGraph_FaceRefId>       aFaceRefMap(static_cast<size_t>(aCounts.NbFaceRefs));
  IdRemap<BRepGraph_ShellRefId>      aShellRefMap(static_cast<size_t>(aCounts.NbShellRefs));
  IdRemap<BRepGraph_ChildRefId>      aChildRefMap(static_cast<size_t>(aCounts.NbChildRefs));
  IdRemap<BRepGraph_SolidRefId>      aSolidRefMap(static_cast<size_t>(aCounts.NbSolidRefs));
  IdRemap<BRepGraph_OccurrenceRefId> anOccurrenceRefMap(
    static_cast<size_t>(aCounts.NbOccurrenceRefs));

  NCollection_FlatMap<BRepGraph_ProductId> aRootProducts;
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    aRootProducts.Add(aRootIt.Current());
  }

  bindActiveIndexMap<BRepGraphInc::VertexDef>(theGraph, aVertexMap);
  bindActiveIndexMap<BRepGraphInc::EdgeDef>(theGraph, anEdgeMap);
  bindActiveIndexMap<BRepGraphInc::WireDef>(theGraph, aWireMap);
  bindActiveIndexMap<BRepGraphInc::FaceDef>(theGraph, aFaceMap);
  bindActiveIndexMap<BRepGraphInc::ShellDef>(theGraph, aShellMap);
  bindActiveIndexMap<BRepGraphInc::SolidDef>(theGraph, aSolidMap);
  bindActiveIndexMap<BRepGraphInc::CompoundDef>(theGraph, aCompoundMap);
  bindActiveIndexMap<BRepGraphInc::CompSolidDef>(theGraph, aCompSolidMap);

  const BRepGraph_Data* aGraphData      = theGraph.data();
  const uint32_t        aGeneration     = aGraphData->myIncStorage.Generation();
  auto                  rejectCandidate = [&]() -> Result {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    aResult.StatusCode   = Status::CandidateValidationFailed;
    return aResult;
  };

  // Construct a fresh graph and rebuild bottom-up.
  // Geometry nodes (Surface, Curve) are automatically created through Add/Add.
  BRepGraph             aNewGraph;
  BRepGraph_Data*       aNewGraphData = aNewGraph.data();
  BRepGraphInc_Storage& aNewStorage   = aNewGraphData->myIncStorage;
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

  // Rebuild each active free coedge separately. Edge/face identity is not sufficient to merge
  // uses because orientation and representation data belong to the individual coedge.
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

    const BRepGraph_CoEdgeId aNewCoEdgeId =
      aNewGraph.Editor().CoEdges().Add(aRemappedEdge, anOldCE.Orientation);
    if (!aNewCoEdgeId.IsValid())
    {
      continue;
    }

    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aNewCoEdge =
      aNewGraph.Editor().CoEdges().Mut(aNewCoEdgeId);
    aNewGraph.Editor().CoEdges().SetFaceId(aNewCoEdge, aRemappedFace, false);
    aCoEdgeMap.Bind(anOldCoEdgeId, aNewCoEdgeId);
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
    BRepGraphInc::CoEdgeDef& aNewCoEdgeDef = aNewCoEdge.Internal();
    aNewCoEdgeDef.OwnGen                   = anOldCoEdge.OwnGen;
    aNewCoEdgeDef.SubtreeGen               = anOldCoEdge.SubtreeGen;
    aNewCoEdgeDef.UID                      = anOldCoEdge.UID;

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

  // Persistent mesh representations are core storage, not runtime cache state.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_EdgeId*            aNewId     = anEdgeMap.Seek(anIt.CurrentId());
    const BRepGraph_EdgePolygon3DRepId anOldRepId = anIt.Current().Polygon3DRepId;
    if (aNewId == nullptr || !anOldRepId.IsValid(aStorage.NbEdgePolygons3D())
        || aStorage.IsRemoved(anOldRepId))
    {
      continue;
    }
    const BRepGraph_EdgePolygon3DRepId aNewRepId   = aNewStorage.AppendEdgePolygon3DRep();
    BRepGraphInc::EdgePolygon3DRep&    aNewRep     = aNewStorage.ChangeEdgePolygon3DRep(aNewRepId);
    aNewRep.ParentEdgeId                           = *aNewId;
    aNewRep.Polygon                                = aStorage.EdgePolygon3DRep(anOldRepId).Polygon;
    aNewStorage.ChangeEdge(*aNewId).Polygon3DRepId = aNewRepId;
  }
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_CoEdgeId* aNewId = aCoEdgeMap.Seek(anIt.CurrentId());
    if (aNewId == nullptr)
    {
      continue;
    }
    const BRepGraph_CoEdgePolygon2DRepId anOldPolygon2D = anIt.Current().Polygon2DRepId;
    if (anOldPolygon2D.IsValid(aStorage.NbCoEdgePolygons2D())
        && !aStorage.IsRemoved(anOldPolygon2D))
    {
      const BRepGraph_CoEdgePolygon2DRepId aNewRepId = aNewStorage.AppendCoEdgePolygon2DRep();
      BRepGraphInc::CoEdgePolygon2DRep& aNewRep = aNewStorage.ChangeCoEdgePolygon2DRep(aNewRepId);
      aNewRep.ParentCoEdgeId                    = *aNewId;
      aNewRep.Polygon = aStorage.CoEdgePolygon2DRep(anOldPolygon2D).Polygon;
      aNewStorage.ChangeCoEdge(*aNewId).Polygon2DRepId = aNewRepId;
    }
    const BRepGraph_CoEdgePolygonOnTriRepId anOldPolygonOnTri = anIt.Current().PolygonOnTriRepId;
    if (anOldPolygonOnTri.IsValid(aStorage.NbCoEdgePolygonsOnTri())
        && !aStorage.IsRemoved(anOldPolygonOnTri))
    {
      const BRepGraph_CoEdgePolygonOnTriRepId aNewRepId = aNewStorage.AppendCoEdgePolygonOnTriRep();
      BRepGraphInc::CoEdgePolygonOnTriRep&    aNewRep =
        aNewStorage.ChangeCoEdgePolygonOnTriRep(aNewRepId);
      aNewRep.ParentCoEdgeId = *aNewId;
      aNewRep.Polygon        = aStorage.CoEdgePolygonOnTriRep(anOldPolygonOnTri).Polygon;
      aNewStorage.ChangeCoEdge(*aNewId).PolygonOnTriRepId = aNewRepId;
    }
  }
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(theGraph); anIt.More(); anIt.Next())
  {
    const BRepGraph_FaceId*                aNewId     = aFaceMap.Seek(anIt.CurrentId());
    const BRepGraph_FaceTriangulationRepId anOldRepId = anIt.Current().TriangulationRepId;
    if (aNewId == nullptr || !anOldRepId.IsValid(aStorage.NbFaceTriangulations())
        || aStorage.IsRemoved(anOldRepId))
    {
      continue;
    }
    const BRepGraph_FaceTriangulationRepId aNewRepId = aNewStorage.AppendFaceTriangulationRep();
    BRepGraphInc::FaceTriangulationRep& aNewRep = aNewStorage.ChangeFaceTriangulationRep(aNewRepId);
    aNewRep.ParentFaceId                        = *aNewId;
    aNewRep.Triangulation = aStorage.FaceTriangulationRep(anOldRepId).Triangulation;
    aNewStorage.ChangeFace(*aNewId).TriangulationRepId = aNewRepId;
  }

  // Rebuild relation tables from final incidence to guarantee compact output consistency.
  aNewGraph.incStorage().RebuildDerivedRelations();

  // Validate rebuilt graph before swapping it into the source graph.
  // If rebuilt topology is inconsistent, keep source graph unchanged.
  if (!isValidCompactCandidate(aNewGraph, theOptions))
  {
    return rejectCandidate();
  }

  // Rebuild assembly/product structure preserving part roots and assembly placements.
  // Products and occurrences are not topology defs and must be rebuilt explicitly.
  for (BRepGraph_FullProductIterator aProductIt(theGraph); aProductIt.More(); aProductIt.Next())
  {
    const BRepGraph_ProductId anOldProductId = aProductIt.CurrentId();
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
        if (aRootProducts.Contains(anOldProductId))
        {
          aNewGraph.Editor().Products().AppendDocumentRoot(aNewProductId);
        }
      }
    }

    if (!aNewProductId.IsValid())
    {
      aNewProductId = aNewGraph.Editor().Products().Add();
      if (aRootProducts.Contains(anOldProductId))
      {
        aNewGraph.Editor().Products().AppendDocumentRoot(aNewProductId);
      }
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

  for (const auto& [anOldProductId, aNewProductId] : aProductMap.Items())
  {
    const BRepGraphInc::ProductDef& anOldProduct =
      theGraph.Topo().Products().Definition(anOldProductId);
    BRepGraphInc::ProductDef& aNewProduct = aNewStorage.ChangeProduct(aNewProductId);
    aNewProduct.OwnGen                    = anOldProduct.OwnGen;
    aNewProduct.SubtreeGen                = anOldProduct.SubtreeGen;
    aNewProduct.UID                       = anOldProduct.UID;
  }

  for (const auto& [anOldOccurrenceId, aNewOccurrenceId] : anOccurrenceMap.Items())
  {
    const BRepGraphInc::OccurrenceDef& anOldOccurrence =
      theGraph.Topo().Occurrences().Definition(anOldOccurrenceId);
    BRepGraphInc::OccurrenceDef& aNewOccurrence = aNewStorage.ChangeOccurrence(aNewOccurrenceId);
    aNewOccurrence.OwnGen                       = anOldOccurrence.OwnGen;
    aNewOccurrence.SubtreeGen                   = anOldOccurrence.SubtreeGen;
    aNewOccurrence.UID                          = anOldOccurrence.UID;
  }

  for (const auto& [anOldRefId, aNewRefId] : aVertexRefMap.Items())
  {
    aNewStorage.ChangeVertexRef(aNewRefId).UID = theGraph.Refs().Vertices().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : aWireRefMap.Items())
  {
    aNewStorage.ChangeWireRef(aNewRefId).UID = theGraph.Refs().Wires().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : aFaceRefMap.Items())
  {
    aNewStorage.ChangeFaceRef(aNewRefId).UID = theGraph.Refs().Faces().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : aShellRefMap.Items())
  {
    aNewStorage.ChangeShellRef(aNewRefId).UID = theGraph.Refs().Shells().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : aSolidRefMap.Items())
  {
    aNewStorage.ChangeSolidRef(aNewRefId).UID = theGraph.Refs().Solids().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : aChildRefMap.Items())
  {
    aNewStorage.ChangeChildRef(aNewRefId).UID = theGraph.Refs().Children().Entry(anOldRefId).UID;
  }
  for (const auto& [anOldRefId, aNewRefId] : anOccurrenceRefMap.Items())
  {
    aNewStorage.ChangeOccurrenceRef(aNewRefId).UID =
      theGraph.Refs().Occurrences().Entry(anOldRefId).UID;
  }

  if (!isValidCompactCandidate(aNewGraph, theOptions))
  {
    return rejectCandidate();
  }

  aResult.NbNodesAfter = aNewGraph.Topo().Gen().NbNodes();

  // Compaction must not move durable UID watermarks backwards when high-UID tombstones vanish.
  constexpr BRepGraph_NodeId::Kind THE_NODE_KINDS[] = {BRepGraph_NodeId::Kind::Vertex,
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
  for (const BRepGraph_NodeId::Kind aKind : THE_NODE_KINDS)
  {
    aNewStorage.SetNextNodeUIDCounter(aKind, aStorage.NextNodeUIDCounter(aKind));
  }
  constexpr BRepGraph_RefId::Kind THE_REF_KINDS[] = {BRepGraph_RefId::Kind::Shell,
                                                     BRepGraph_RefId::Kind::Face,
                                                     BRepGraph_RefId::Kind::Wire,
                                                     BRepGraph_RefId::Kind::Vertex,
                                                     BRepGraph_RefId::Kind::Solid,
                                                     BRepGraph_RefId::Kind::Child,
                                                     BRepGraph_RefId::Kind::Occurrence};
  for (const BRepGraph_RefId::Kind aKind : THE_REF_KINDS)
  {
    aNewStorage.SetNextRefUIDCounter(aKind, aStorage.NextRefUIDCounter(aKind));
  }

  // Preserve graph generation so inline UIDs remain valid after compaction.
  aNewStorage.SetGeneration(aGraphData->myIncStorage.Generation());
  aNewStorage.SetGraphGUID(aGraphData->myIncStorage.GraphGUID());

  // Rebuild UID reverse indexes from inline UIDs (skipping removed entities).
  aNewStorage.RebuildUIDReverseIndexes();

  // Build full ItemId->ItemId remap covering nodes and refs.
  NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId> anItemRemap;
  anItemRemap.Reserve(aVertexMap.Size() + anEdgeMap.Size() + aCoEdgeMap.Size() + aWireMap.Size()
                      + aFaceMap.Size() + aShellMap.Size() + aSolidMap.Size() + aCompoundMap.Size()
                      + aCompSolidMap.Size() + aProductMap.Size() + anOccurrenceMap.Size()
                      + aVertexRefMap.Size() + aWireRefMap.Size() + aFaceRefMap.Size()
                      + aShellRefMap.Size() + aChildRefMap.Size() + aSolidRefMap.Size()
                      + anOccurrenceRefMap.Size());
  bindItemRemap(anItemRemap, aVertexMap);
  bindItemRemap(anItemRemap, anEdgeMap);
  bindItemRemap(anItemRemap, aCoEdgeMap);
  bindItemRemap(anItemRemap, aWireMap);
  bindItemRemap(anItemRemap, aFaceMap);
  bindItemRemap(anItemRemap, aShellMap);
  bindItemRemap(anItemRemap, aSolidMap);
  bindItemRemap(anItemRemap, aCompoundMap);
  bindItemRemap(anItemRemap, aCompSolidMap);
  bindItemRemap(anItemRemap, aProductMap);
  bindItemRemap(anItemRemap, anOccurrenceMap);
  bindItemRemap(anItemRemap, aVertexRefMap);
  bindItemRemap(anItemRemap, aWireRefMap);
  bindItemRemap(anItemRemap, aFaceRefMap);
  bindItemRemap(anItemRemap, aShellRefMap);
  bindItemRemap(anItemRemap, aChildRefMap);
  bindItemRemap(anItemRemap, aSolidRefMap);
  bindItemRemap(anItemRemap, anOccurrenceRefMap);
  // RepId records are session-local and not part of ItemId remap.

  if (!BRepGraph_Replace::PerformCompaction(theGraph, std::move(aNewGraph), anItemRemap).IsDone())
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    aResult.StatusCode   = Status::MigrationFailed;
    return aResult;
  }
  aResult.StatusCode = Status::Done;
  return aResult;
}
