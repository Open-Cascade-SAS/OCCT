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

#include <BRepGraph_Deduplicate.hxx>

#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_DeferredScope.hxx>
#include <GeomHash_CurveHasher.hxx>
#include <GeomHash_SurfaceHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_KDTree.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <Standard_HashUtils.hxx>

#include <algorithm>
#include <cmath>

namespace
{
//! Redirect all OccurrenceDef.ChildNodeId entries that still point to theOldNodeId
//! to theNewNodeId. Snapshots the occurrence ref list before iterating because
//! SetChildNodeId modifies the underlying OccurrenceRefsOfNode vector.
void redirectOccurrenceChildren(BRepGraph&             theGraph,
                                const BRepGraph_NodeId theOldNodeId,
                                const BRepGraph_NodeId theNewNodeId)
{
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aOccRefs =
    theGraph.Topo().Gen().OccurrenceRefIds(theOldNodeId);
  if (aOccRefs.IsEmpty())
  {
    return;
  }
  // Snapshot - SetChildNodeId modifies the underlying vector.
  NCollection_LinearVector<BRepGraph_OccurrenceRefId> aSnapshot(aOccRefs.Size());
  for (const BRepGraph_OccurrenceRefId& aRefId : aOccRefs)
  {
    aSnapshot.Append(aRefId);
  }
  for (const BRepGraph_OccurrenceRefId& aOccRefId : aSnapshot)
  {
    if (!aOccRefId.IsValid() || theGraph.Refs().Gen().IsRemoved(aOccRefId))
    {
      continue;
    }
    const BRepGraph_OccurrenceId aOccId =
      theGraph.Refs().Occurrences().Entry(aOccRefId).ChildOccurrenceId;
    if (!aOccId.IsValid() || aOccId.IsRemoved(theGraph))
    {
      continue;
    }
    if (theGraph.Topo().Occurrences().Definition(aOccId).ChildNodeId == theOldNodeId)
    {
      theGraph.Editor().Occurrences().SetChildNodeId(aOccId, theNewNodeId);
    }
  }
}

} // namespace

//=================================================================================================

void BRepGraph_Deduplicate::CanonicalizeWireOrders(BRepGraph& theGraph, Result& theResult)
{
  BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aStorage.NbWires()); ++aWireId)
  {
    if (aStorage.IsRemoved(aWireId))
    {
      continue;
    }

    const BRepGraphInc_Storage::WireCoEdgeOrderStatus aStatus =
      aStorage.CanonicalizeWireCoEdgeOrderStatus(aWireId);
    switch (aStatus)
    {
      case BRepGraphInc_Storage::WireCoEdgeOrderStatus::Reordered:
        ++theResult.NbReorderedWires;
        theGraph.markModified(aWireId);
        break;
      case BRepGraphInc_Storage::WireCoEdgeOrderStatus::ToleranceOrdered:
        ++theResult.NbToleranceOrderedWires;
        theGraph.markModified(aWireId);
        break;
      case BRepGraphInc_Storage::WireCoEdgeOrderStatus::Partial:
        ++theResult.NbPartialOrderedWires;
        theGraph.markModified(aWireId);
        break;
      default:
        break;
    }
  }
}

//=================================================================================================

BRepGraph_Deduplicate::Result BRepGraph_Deduplicate::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraph_Deduplicate::Result BRepGraph_Deduplicate::Perform(BRepGraph&     theGraph,
                                                             const Options& theOptions)
{
  Result aResult;
  if (theGraph.IsEmpty())
  {
    return aResult;
  }
  BRepGraph_DeferredScope aDeferredScope(theGraph);

  BRepGraph_LayerHistory& aHistory = *theGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const bool              wasHistoryEnabled = aHistory.IsEnabled();
  aHistory.SetEnabled(theOptions.HistoryMode);

  GeomHash_SurfaceHasher aSurfHasher(theOptions.CompTolerance, theOptions.HashTolerance);
  GeomHash_CurveHasher   aCurveHasher(theOptions.CompTolerance, theOptions.HashTolerance);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator();
  using ActiveVertexList = NCollection_LinearVector<std::pair<gp_Pnt, BRepGraph_VertexId>>;
  using EdgeIdList       = NCollection_LinearVector<BRepGraph_EdgeId>;
  using WireIdList       = NCollection_LinearVector<BRepGraph_WireId>;
  using FaceIdList       = NCollection_LinearVector<BRepGraph_FaceId>;

  {
    // Deduplicate surfaces by comparing Handle pointers on FaceDefs.
    // Map: surface handle -> canonical face index (first face that owns it).
    NCollection_DataMap<occ::handle<Geom_Surface>, BRepGraph_FaceId, GeomHash_SurfaceHasher>
      aSurfToCanonicalFace(
        aSurfHasher,
        std::max<size_t>(1, static_cast<size_t>(theGraph.Topo().Faces().Nb()) * 2),
        aTmpAlloc);
    // Map: face id -> canonical face id (for faces whose surface should be replaced).
    NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aSurfRewriteMap(
      std::max<size_t>(1, theGraph.Topo().Faces().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    {
      const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
      if (aFaceId.IsRemoved(theGraph))
      {
        continue;
      }
      if (!BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
      {
        continue;
      }

      const occ::handle<Geom_Surface>& aFaceSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
      const BRepGraph_FaceId*          aCanonFaceId = aSurfToCanonicalFace.Seek(aFaceSurf);
      if (aCanonFaceId == nullptr)
      {
        aSurfToCanonicalFace.Bind(aFaceSurf, aFaceId);
      }
      else if (*aCanonFaceId != aFaceId)
      {
        aSurfRewriteMap.Bind(aFaceId, *aCanonFaceId);
      }
    }

    // Deduplicate curves by comparing Handle pointers on EdgeDefs.
    NCollection_DataMap<occ::handle<Geom_Curve>, BRepGraph_EdgeId, GeomHash_CurveHasher>
      aCurveToCanonicalEdge(
        aCurveHasher,
        std::max<size_t>(1, static_cast<size_t>(theGraph.Topo().Edges().Nb()) * 2),
        aTmpAlloc);
    NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> aCurveRewriteMap(
      std::max<size_t>(1, theGraph.Topo().Edges().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
      if (anEdgeId.IsRemoved(theGraph))
      {
        continue;
      }
      if (!BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
      {
        continue;
      }

      const occ::handle<Geom_Curve>& anEdgeCurve  = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
      const BRepGraph_EdgeId*        aCanonEdgeId = aCurveToCanonicalEdge.Seek(anEdgeCurve);
      if (aCanonEdgeId == nullptr)
      {
        aCurveToCanonicalEdge.Bind(anEdgeCurve, anEdgeId);
      }
      else if (*aCanonEdgeId != anEdgeId)
      {
        aCurveRewriteMap.Bind(anEdgeId, *aCanonEdgeId);
      }
    }

    aResult.NbCanonicalSurfaces =
      theGraph.Topo().Faces().Nb() - static_cast<uint32_t>(aSurfRewriteMap.Size());
    aResult.NbCanonicalCurves =
      theGraph.Topo().Edges().Nb() - static_cast<uint32_t>(aCurveRewriteMap.Size());

    if (theOptions.AnalyzeOnly && !theOptions.MergeEntitiesWhenSafe)
    {
      aHistory.SetEnabled(wasHistoryEnabled);
      return aResult;
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Rewrite face surfaces: replace duplicate surface handles with canonical ones.
      NCollection_LinearVector<BRepGraph_NodeId> aSurfRepl(4);
      for (NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>::Iterator anIt(aSurfRewriteMap);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_FaceId           aFaceId      = anIt.Key();
        const BRepGraph_FaceId           aCanonFaceId = anIt.Value();
        const occ::handle<Geom_Surface>& aCanonSurf =
          BRepGraph_Tool::Face::Surface(theGraph, aCanonFaceId);

        BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef = theGraph.Editor().Faces().Mut(aFaceId);
        theGraph.Editor().Faces().SetSurface(aFaceId, aCanonSurf);
        ++aResult.NbSurfaceRewrites;
        aResult.AffectedFaces.Append(aFaceId);

        aSurfRepl.Clear(false);
        aSurfRepl.Append(aCanonFaceId);
        aHistory.Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                        aFaceId,
                        aSurfRepl.ToArray1());
        ++aResult.NbHistoryRecords;
      }

      // Rewrite edge curves: replace duplicate curve handles with canonical ones.
      NCollection_LinearVector<BRepGraph_NodeId> aCurveRepl(4);
      for (NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>::Iterator anIt(aCurveRewriteMap);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_EdgeId         anEdgeId     = anIt.Key();
        const BRepGraph_EdgeId         aCanonEdgeId = anIt.Value();
        const occ::handle<Geom_Curve>& aCanonCurve =
          BRepGraph_Tool::Edge::Curve(theGraph, aCanonEdgeId);
        if (aCanonCurve.IsNull())
        {
          continue;
        }
        const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(theGraph, anEdgeId);

        theGraph.Editor().Edges().SetCurve(anEdgeId, aCanonCurve, aFirst, aLast);
        ++aResult.NbCurveRewrites;
        aResult.AffectedEdges.Append(anEdgeId);

        aCurveRepl.Clear(false);
        aCurveRepl.Append(aCanonEdgeId);
        aHistory.Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                        anEdgeId,
                        aCurveRepl.ToArray1());
        ++aResult.NbHistoryRecords;
      }

    } // end if (!theOptions.AnalyzeOnly) for geometry rewrites

  } // end geometry dedup scope - frees aSurfToCanonicalFace, aSurfRewriteMap,
    // aCurveToCanonicalEdge, aCurveRewriteMap

  // Definition merge phases (Vertex -> Edge -> Wire -> Face).
  if (!theOptions.MergeEntitiesWhenSafe)
  {
    aResult.IsEntityMergeApplied = false;
    aHistory.SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  aTmpAlloc->Reset(false);

  // Phase 1: Vertex Merging via KDTree range search.
  {
    const double aTol = theOptions.HashTolerance;

    // Collect active vertices: (point, graph id) pairs.
    const uint32_t   aNbVertices = theGraph.Topo().Vertices().Nb();
    ActiveVertexList aActiveVertices(256);
    for (BRepGraph_FullVertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
    {
      const BRepGraph_VertexId aVertexId = aVertexIt.CurrentId();
      if (aVertexId.IsRemoved(theGraph))
      {
        continue;
      }
      aActiveVertices.Append(
        std::make_pair(BRepGraph_Tool::Vertex::Pnt(theGraph, aVertexId), aVertexId));
    }

    // Sort by descending vertex tolerance so high-tolerance vertices iterate
    // as base first. Their search radius (max(aTol, aBaseVtxTol)) is at least
    // their own tolerance, which dominates the per-pair acceptance distance
    // max(baseTol, candTol). This guarantees every potentially-mergeable pair
    // is examined exactly once with a sufficient radius - no global widening.
    std::sort(aActiveVertices.begin(),
              aActiveVertices.end(),
              [&](const std::pair<gp_Pnt, BRepGraph_VertexId>& theA,
                  const std::pair<gp_Pnt, BRepGraph_VertexId>& theB) {
                return BRepGraph_Tool::Vertex::Tolerance(theGraph, theA.second)
                       > BRepGraph_Tool::Vertex::Tolerance(theGraph, theB.second);
              });

    // Build KDTree from active vertex points - O(n log n).
    const size_t                  aNbActive = aActiveVertices.Size();
    NCollection_KDTree<gp_Pnt, 3> aTree;
    if (aNbActive > 0)
    {
      gp_Pnt* aPointsBuf = static_cast<gp_Pnt*>(aTmpAlloc->Allocate(aNbActive * sizeof(gp_Pnt)));
      size_t  i          = 0;
      for (const auto& aVertex : aActiveVertices)
      {
        aPointsBuf[i++] = aVertex.first;
      }
      aTree.Build(aPointsBuf, aNbActive);
    }

    // Canonical vertex map: old graph id -> canonical graph id.
    NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aCanonicalVertex(
      static_cast<size_t>(std::max(1u, aNbVertices)),
      aTmpAlloc);

    for (size_t aLocalIdx = 0; aLocalIdx < aActiveVertices.Size(); ++aLocalIdx)
    {
      const BRepGraph_VertexId aBaseVtxId = aActiveVertices.Value(aLocalIdx).second;
      if (aCanonicalVertex.IsBound(aBaseVtxId))
      {
        continue;
      }

      const gp_Pnt aBaseVtxPnt = BRepGraph_Tool::Vertex::Pnt(theGraph, aBaseVtxId);
      const double aBaseVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, aBaseVtxId);

      aTree.ForEachInRange(aBaseVtxPnt, std::max(aTol, aBaseVtxTol), [&](size_t theResultIdx) {
        const size_t anArrayIdx = theResultIdx - 1;
        if (anArrayIdx <= aLocalIdx)
        {
          return; // skip self and already-processed
        }

        const BRepGraph_VertexId aCandVtxId      = aActiveVertices.Value(anArrayIdx).second;
        BRepGraph_VertexId       aEffectiveCanon = aCandVtxId;
        // Resolve through any prior canonical bindings (path compression with cycle guard).
        for (size_t aHop = 0; aHop < 64 && aCanonicalVertex.IsBound(aEffectiveCanon); ++aHop)
        {
          const BRepGraph_VertexId aNext = aCanonicalVertex.Find(aEffectiveCanon);
          if (aNext == aEffectiveCanon)
          {
            break;
          }
          aEffectiveCanon = aNext;
        }
        if (aEffectiveCanon == aBaseVtxId)
        {
          return;
        }

        const double aCandVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, aEffectiveCanon);
        const double aMaxTol     = std::max(aBaseVtxTol, aCandVtxTol);
        if (aBaseVtxPnt.Distance(BRepGraph_Tool::Vertex::Pnt(theGraph, aEffectiveCanon)) <= aMaxTol)
        {
          aCanonicalVertex.Bind(aCandVtxId, aBaseVtxId);
        }
      });
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Redirect edge vertex references and mark non-canonical vertices as removed.
      for (NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId>::Iterator anIt(
             aCanonicalVertex);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_VertexId anOldVertexId  = anIt.Key();
        const BRepGraph_VertexId aCanonVertexId = anIt.Value();
        const BRepGraph_NodeId   anOldId        = anOldVertexId;
        const BRepGraph_NodeId   aCanonId       = aCanonVertexId;

        // Update edges referencing the old vertex.
        for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
        {
          const BRepGraph_EdgeId                    anEdgeId = anEdgeIt.CurrentId();
          BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
            theGraph.Editor().Edges().Mut(anEdgeId);
          if (anEdgeId.IsRemoved(theGraph))
          {
            continue;
          }
          // Resolve current vertex def ids through ref entries and update them.
          if (anEdge->StartVertexRefId.IsValid())
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> aStartRef =
              theGraph.Editor().Vertices().MutRef(anEdge->StartVertexRefId);
            if (aStartRef->ChildVertexId == anOldVertexId)
            {
              theGraph.Editor().Vertices().SetRefChildVertexId(aStartRef, aCanonVertexId);
            }
          }
          if (anEdge->EndVertexRefId.IsValid())
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> anEndRef =
              theGraph.Editor().Vertices().MutRef(anEdge->EndVertexRefId);
            if (anEndRef->ChildVertexId == anOldVertexId)
            {
              theGraph.Editor().Vertices().SetRefChildVertexId(anEndRef, aCanonVertexId);
            }
          }
        }

        // Redirect CompoundDef.ChildRefIds that still point to the old vertex.
        for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
             aCompIt.Next())
        {
          const BRepGraph_CompoundId aCompId = aCompIt.CurrentId();
          if (aCompId.IsRemoved(theGraph))
          {
            continue;
          }
          for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aCompId); aRefIt.More();
               aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId()) && aCR.ChildNodeId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              theGraph.Editor().Gen().SetChildRefChildNodeId(aMutCR, aCanonId);
            }
          }
        }

        // Redirect OccurrenceDef.ChildNodeId entries that still point to the old vertex.
        redirectOccurrenceChildren(theGraph, anOldId, aCanonId);

        // Mark non-canonical as removed.
        theGraph.Editor().Gen().ReplaceNode(anOldId, aCanonId);
        aHistory.RecordReplaced(TCollection_AsciiString("Dedup:MergeVertex"), anOldId, aCanonId);

        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedVertices;
      }
    }
    else
    {
      aResult.NbMergedVertices = static_cast<uint32_t>(aCanonicalVertex.Size());
    }
  }

  if (!theOptions.AnalyzeOnly)
  {
    CanonicalizeWireOrders(theGraph, aResult);
  }

  aTmpAlloc->Reset(false);

  // Phase 2: Edge Merging.
  {
    // Key: (canonical Curve3d handle, canonical StartChildVertexId, canonical EndChildVertexId).
    struct EdgeKey
    {
      occ::handle<Geom_Curve> Curve;
      BRepGraph_VertexId      StartVtx;
      BRepGraph_VertexId      EndVtx;

      bool operator==(const EdgeKey& theOther) const
      {
        return Curve == theOther.Curve && StartVtx == theOther.StartVtx
               && EndVtx == theOther.EndVtx;
      }
    };

    struct EdgeKeyHasher
    {
      size_t operator()(const EdgeKey& theKey) const noexcept
      {
        size_t aCombination[3];
        aCombination[0] = NCollection_DefaultHasher<occ::handle<Geom_Curve>>{}(theKey.Curve);
        aCombination[1] = opencascade::hash(theKey.StartVtx);
        aCombination[2] = opencascade::hash(theKey.EndVtx);
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }

      bool operator()(const EdgeKey& theA, const EdgeKey& theB) const { return theA == theB; }
    };

    NCollection_DataMap<EdgeKey, EdgeIdList, EdgeKeyHasher> anEdgeGroups(
      std::max<size_t>(1, theGraph.Topo().Edges().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
      if (anEdgeId.IsRemoved(theGraph) || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
      {
        continue;
      }

      EdgeKey aKey;
      aKey.Curve = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
      const BRepGraph_VertexRefId aStartRefId =
        BRepGraph_Tool::Edge::StartVertexId(theGraph, anEdgeId);
      const BRepGraph_VertexRefId anEndRefId =
        BRepGraph_Tool::Edge::EndVertexId(theGraph, anEdgeId);
      const BRepGraph_VertexId aStartVtxId =
        aStartRefId.IsValid() ? theGraph.Refs().Vertices().Entry(aStartRefId).ChildVertexId
                              : BRepGraph_VertexId();
      const BRepGraph_VertexId anEndVtxId =
        anEndRefId.IsValid() ? theGraph.Refs().Vertices().Entry(anEndRefId).ChildVertexId
                             : BRepGraph_VertexId();
      aKey.StartVtx = aStartVtxId;
      aKey.EndVtx   = anEndVtxId;

      // Normalize: always use min vertex index first for undirected matching.
      if (aKey.StartVtx > aKey.EndVtx)
      {
        std::swap(aKey.StartVtx, aKey.EndVtx);
      }

      anEdgeGroups.TryBind(aKey, EdgeIdList());
      anEdgeGroups.ChangeFind(aKey).Append(anEdgeId);
    }

    NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> aCanonicalEdge(
      std::max<size_t>(1, theGraph.Topo().Edges().Nb()),
      aTmpAlloc);

    for (NCollection_DataMap<EdgeKey, EdgeIdList, EdgeKeyHasher>::Iterator aGroupIter(anEdgeGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const EdgeIdList& aGroup = aGroupIter.Value();
      if (aGroup.Size() < 2)
      {
        continue;
      }

      const BRepGraph_EdgeId       aCanonEdgeId = aGroup.Value(0);
      const BRepGraphInc::EdgeDef& aCanonEdge   = theGraph.Topo().Edges().Definition(aCanonEdgeId);

      for (size_t aCandIter = 1; aCandIter < aGroup.Size(); ++aCandIter)
      {
        const BRepGraph_EdgeId       aCandEdgeId = aGroup.Value(aCandIter);
        const BRepGraphInc::EdgeDef& aCandEdge   = theGraph.Topo().Edges().Definition(aCandEdgeId);

        // Compare parameter ranges within tolerance.
        const auto [aCanonFirst, aCanonLast] = BRepGraph_Tool::Edge::Range(theGraph, aCanonEdgeId);
        const auto [aCandFirst, aCandLast]   = BRepGraph_Tool::Edge::Range(theGraph, aCandEdgeId);
        if (std::abs(aCanonFirst - aCandFirst) > theOptions.CompTolerance)
        {
          continue;
        }
        if (std::abs(aCanonLast - aCandLast) > theOptions.CompTolerance)
        {
          continue;
        }

        // Check tolerance compatibility.
        if (aCandEdge.Tolerance > aCanonEdge.Tolerance * 10.0)
        {
          continue;
        }

        aCanonicalEdge.Bind(aCandEdgeId, aCanonEdgeId);
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      WireIdList aWires(64);
      for (NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>::Iterator anIt(aCanonicalEdge);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_EdgeId anOldEdgeId  = anIt.Key();
        const BRepGraph_EdgeId aCanonEdgeId = anIt.Value();
        const BRepGraph_NodeId anOldId      = anOldEdgeId;
        const BRepGraph_NodeId aCanonId     = aCanonEdgeId;

        // Determine if the non-canonical edge is reversed relative to canonical.
        // Resolve vertex def ids for reversal check using Tool helpers.
        const BRepGraph_VertexRefId aCanonStartRef =
          BRepGraph_Tool::Edge::StartVertexId(theGraph, aCanonEdgeId);
        const BRepGraph_VertexRefId aCanonEndRef =
          BRepGraph_Tool::Edge::EndVertexId(theGraph, aCanonEdgeId);
        const BRepGraph_VertexRefId anOldStartRef =
          BRepGraph_Tool::Edge::StartVertexId(theGraph, anOldEdgeId);
        const BRepGraph_VertexRefId anOldEndRef =
          BRepGraph_Tool::Edge::EndVertexId(theGraph, anOldEdgeId);
        const BRepGraph_NodeId aCanonStart =
          aCanonStartRef.IsValid()
            ? BRepGraph_NodeId(theGraph.Refs().Vertices().Entry(aCanonStartRef).ChildVertexId)
            : BRepGraph_NodeId();
        const BRepGraph_NodeId aCanonEnd =
          aCanonEndRef.IsValid()
            ? BRepGraph_NodeId(theGraph.Refs().Vertices().Entry(aCanonEndRef).ChildVertexId)
            : BRepGraph_NodeId();
        const BRepGraph_NodeId anOldStart =
          anOldStartRef.IsValid()
            ? BRepGraph_NodeId(theGraph.Refs().Vertices().Entry(anOldStartRef).ChildVertexId)
            : BRepGraph_NodeId();
        const BRepGraph_NodeId anOldEnd =
          anOldEndRef.IsValid()
            ? BRepGraph_NodeId(theGraph.Refs().Vertices().Entry(anOldEndRef).ChildVertexId)
            : BRepGraph_NodeId();
        bool isReversed = false;
        // Self-loop edges cannot be reversed in the sense that matters for wire
        // replacement - after vertex merging, both ends could resolve to the
        // same canonical vertex, incorrectly satisfying the reversal condition.
        if (!(aCanonStart == aCanonEnd || anOldStart == anOldEnd))
        {
          isReversed = (aCanonStart == anOldEnd && aCanonEnd == anOldStart);
        }

        // Replace in wires - copy wire list before iterating because ReplaceEdge
        // mutates myEdgeToWires[oldEdgeId.Index] via eraseSwapLast.
        aWires.Clear(false);
        for (BRepGraph_WiresOfEdge aWireIt = theGraph.Topo().Edges().WiresOf(anOldEdgeId);
             aWireIt.More();
             aWireIt.Next())
        {
          aWires.Append(aWireIt.CurrentId());
        }
        for (const BRepGraph_WireId& aWireId : aWires)
        {
          theGraph.Editor().Wires().ReplaceEdge(aWireId, anOldEdgeId, aCanonEdgeId, isReversed);
        }

        // ReplaceEdge() above rebinds all CoEdgeDef.ChildEdgeId entries from anOldEdgeId
        // to aCanonEdgeId (and updates the reverse CoEdgesOfEdge index), so
        // theGraph.Topo().Edges().CoEdges(anOldEdgeId) is always empty at this point.
        // PCurve handles are preserved through the CoEdge rebinding.

        // Redirect CompoundDef.ChildRefIds that still point to the old edge.
        for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
             aCompIt.Next())
        {
          const BRepGraph_CompoundId aCompId = aCompIt.CurrentId();
          if (aCompId.IsRemoved(theGraph))
          {
            continue;
          }
          for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aCompId); aRefIt.More();
               aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId()) && aCR.ChildNodeId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              theGraph.Editor().Gen().SetChildRefChildNodeId(aMutCR, aCanonId);
            }
          }
        }

        // Redirect OccurrenceDef.ChildNodeId entries that still point to the old edge.
        redirectOccurrenceChildren(theGraph, anOldId, aCanonId);

        theGraph.Editor().Gen().ReplaceNode(anOldId, aCanonId);
        aHistory.RecordReplaced(TCollection_AsciiString("Dedup:MergeEdge"), anOldId, aCanonId);

        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedEdges;
      }
    }
    else
    {
      aResult.NbMergedEdges = static_cast<uint32_t>(aCanonicalEdge.Size());
    }
  }

  aTmpAlloc->Reset(false);

  // Phase 3: Wire Merging.
  {
    // Hash wire by its ordered coedge sequence (edge index + sense from CoEdgeDef).
    struct WireHash
    {
      size_t operator()(const BRepGraph_WireId theWireId, const BRepGraph& theGraph) const
      {
        size_t aHash = 0;
        for (BRepGraph_CoEdgesOfWire aCEIt(theGraph, theWireId); aCEIt.More(); aCEIt.Next())
        {
          const BRepGraphInc::CoEdgeDef& aCoEdge =
            theGraph.Topo().CoEdges().Definition(aCEIt.CurrentId());
          size_t aEntryHash[2];
          aEntryHash[0] = opencascade::hash(aCoEdge.ChildEdgeId);
          aEntryHash[1] = opencascade::hash(static_cast<int>(aCoEdge.Orientation));
          aHash ^= opencascade::hashBytes(aEntryHash, sizeof(aEntryHash)) + 0x9e3779b9
                   + (aHash << 6) + (aHash >> 2);
        }
        return aHash;
      }
    };

    NCollection_LinearVector<BRepGraph_CoEdgeId> aWireACoEdges(64);
    NCollection_LinearVector<BRepGraph_CoEdgeId> aWireBCoEdges(64);

    auto wiresEqual = [&](const BRepGraph_WireId theA, const BRepGraph_WireId theB) -> bool {
      aWireACoEdges.Clear(false);
      aWireBCoEdges.Clear(false);
      for (BRepGraph_CoEdgesOfWire aCEIt(theGraph, theA); aCEIt.More(); aCEIt.Next())
      {
        aWireACoEdges.Append(aCEIt.CurrentId());
      }
      for (BRepGraph_CoEdgesOfWire aCEIt(theGraph, theB); aCEIt.More(); aCEIt.Next())
      {
        aWireBCoEdges.Append(aCEIt.CurrentId());
      }

      if (aWireACoEdges.Size() != aWireBCoEdges.Size())
      {
        return false;
      }
      for (size_t anIdx = 0; anIdx < aWireACoEdges.Size(); ++anIdx)
      {
        const BRepGraphInc::CoEdgeDef& aCoEdgeA =
          theGraph.Topo().CoEdges().Definition(aWireACoEdges.Value(anIdx));
        const BRepGraphInc::CoEdgeDef& aCoEdgeB =
          theGraph.Topo().CoEdges().Definition(aWireBCoEdges.Value(anIdx));
        if (aCoEdgeA.ChildEdgeId != aCoEdgeB.ChildEdgeId
            || aCoEdgeA.Orientation != aCoEdgeB.Orientation)
        {
          return false;
        }
      }
      return true;
    };

    NCollection_DataMap<size_t, WireIdList> aWireHashBuckets(
      std::max<size_t>(1, theGraph.Topo().Wires().Nb()),
      aTmpAlloc);

    WireHash aHasher;
    for (BRepGraph_FullWireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraph_WireId aWireId = aWireIt.CurrentId();
      if (aWireId.IsRemoved(theGraph))
      {
        continue;
      }
      const size_t aH = aHasher(aWireId, theGraph);
      aWireHashBuckets.TryBind(aH, WireIdList());
      aWireHashBuckets.ChangeFind(aH).Append(aWireId);
    }

    NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId> aCanonicalWire(
      std::max<size_t>(1, theGraph.Topo().Wires().Nb()),
      aTmpAlloc);

    for (NCollection_DataMap<size_t, WireIdList>::Iterator aBucketIter(aWireHashBuckets);
         aBucketIter.More();
         aBucketIter.Next())
    {
      const WireIdList& aBucket = aBucketIter.Value();
      for (size_t aBaseIdx = 0; aBaseIdx < aBucket.Size(); ++aBaseIdx)
      {
        const BRepGraph_WireId aBaseWireId = aBucket.Value(aBaseIdx);
        if (aCanonicalWire.IsBound(aBaseWireId))
        {
          continue;
        }

        for (size_t aCandIdx = aBaseIdx + 1; aCandIdx < aBucket.Size(); ++aCandIdx)
        {
          const BRepGraph_WireId aCandWireId = aBucket.Value(aCandIdx);
          if (aCanonicalWire.IsBound(aCandWireId))
          {
            continue;
          }
          if (wiresEqual(aBaseWireId, aCandWireId))
          {
            aCanonicalWire.Bind(aCandWireId, aBaseWireId);
          }
        }
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Mark non-canonical wires as removed.
      for (NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId>::Iterator anIt(aCanonicalWire);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_WireId anOldWireId  = anIt.Key();
        const BRepGraph_WireId aCanonWireId = anIt.Value();
        const BRepGraph_NodeId anOldId      = anOldWireId;
        const BRepGraph_NodeId aCanonId     = aCanonWireId;

        // Redirect FaceDef.WireRefIds that still point to the old wire.
        for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
        {
          const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
          if (aFaceId.IsRemoved(theGraph))
          {
            continue;
          }

          for (const BRepGraph_WireRefId& aWireRefId :
               theGraph.Topo().Faces().Relations(aFaceId).WireRefIds)
          {
            const BRepGraphInc::WireRef& aWireRef = theGraph.Refs().Wires().Entry(aWireRefId);
            if (!theGraph.Refs().Gen().IsRemoved(aWireRefId) && aWireRef.ChildWireId == anOldWireId)
            {
              BRepGraph_MutGuard<BRepGraphInc::WireRef> aMutWireRef =
                theGraph.Editor().Wires().MutRef(aWireRefId);
              theGraph.Editor().Wires().SetRefChildWireId(aMutWireRef, aCanonWireId);
            }
          }
        }

        // Redirect CompoundDef.ChildRefIds that still point to the old wire.
        for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
             aCompIt.Next())
        {
          const BRepGraph_CompoundId aCompId = aCompIt.CurrentId();
          if (aCompId.IsRemoved(theGraph))
          {
            continue;
          }
          for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aCompId); aRefIt.More();
               aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId()) && aCR.ChildNodeId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              theGraph.Editor().Gen().SetChildRefChildNodeId(aMutCR, aCanonId);
            }
          }
        }

        // Redirect OccurrenceDef.ChildNodeId entries that still point to the old wire.
        redirectOccurrenceChildren(theGraph, anOldId, aCanonId);

        theGraph.Editor().Gen().ReplaceNode(anOldId, aCanonId);
        aHistory.RecordReplaced(TCollection_AsciiString("Dedup:MergeWire"), anOldId, aCanonId);

        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedWires;
      }
    }
    else
    {
      aResult.NbMergedWires = static_cast<uint32_t>(aCanonicalWire.Size());
    }
  }

  aTmpAlloc->Reset(false);

  // Phase 4: Face Merging.
  {
    struct FaceKey
    {
      occ::handle<Geom_Surface> Surface;
      size_t                    WireHash;

      bool operator==(const FaceKey& theOther) const
      {
        return Surface == theOther.Surface && WireHash == theOther.WireHash;
      }
    };

    struct FaceKeyHasher
    {
      size_t operator()(const FaceKey& theKey) const noexcept
      {
        size_t aCombination[2];
        aCombination[0] = NCollection_DefaultHasher<occ::handle<Geom_Surface>>{}(theKey.Surface);
        aCombination[1] = theKey.WireHash;
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }

      bool operator()(const FaceKey& theA, const FaceKey& theB) const { return theA == theB; }
    };

    auto faceWireHash = [&](const BRepGraph_FaceId theFaceId) -> size_t {
      // Collect wire def ids used by this face via typed iterator.
      size_t aHash = 0;
      for (BRepGraph_RefsWireOfFace aWireIt(theGraph, theFaceId); aWireIt.More(); aWireIt.Next())
      {
        const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aWireIt.CurrentId());
        aHash ^= opencascade::hash(aWR.ChildWireId);
      }
      return aHash;
    };

    NCollection_DataMap<FaceKey, FaceIdList, FaceKeyHasher> aFaceGroups(
      std::max<size_t>(1, theGraph.Topo().Faces().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    {
      const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
      if (aFaceId.IsRemoved(theGraph) || !BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
      {
        continue;
      }

      FaceKey aKey;
      aKey.Surface  = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
      aKey.WireHash = faceWireHash(aFaceId);

      aFaceGroups.TryBind(aKey, FaceIdList());
      aFaceGroups.ChangeFind(aKey).Append(aFaceId);
    }

    NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aCanonicalFace(
      std::max<size_t>(1, theGraph.Topo().Faces().Nb()),
      aTmpAlloc);

    WireIdList aCanonOuter(64);
    WireIdList aCandOuter(64);

    for (NCollection_DataMap<FaceKey, FaceIdList, FaceKeyHasher>::Iterator aGroupIter(aFaceGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const FaceIdList& aGroup = aGroupIter.Value();
      if (aGroup.Size() < 2)
      {
        continue;
      }

      const BRepGraph_FaceId       aCanonFaceId = aGroup.Value(0);
      const BRepGraphInc::FaceDef& aCanonFace   = theGraph.Topo().Faces().Definition(aCanonFaceId);

      for (size_t aCandIter = 1; aCandIter < aGroup.Size(); ++aCandIter)
      {
        const BRepGraph_FaceId       aCandFaceId = aGroup.Value(aCandIter);
        const BRepGraphInc::FaceDef& aCandFace   = theGraph.Topo().Faces().Definition(aCandFaceId);

        // Check tolerance compatibility.
        if (aCandFace.Tolerance > aCanonFace.Tolerance * 10.0)
        {
          continue;
        }

        // Verify wire equality - two faces sharing the same surface and wire hash
        // are not mergeable unless they also have the same wire topology.
        if (![&]() -> bool {
              aCanonOuter.Clear(false);
              aCandOuter.Clear(false);
              for (BRepGraph_RefsWireOfFace aWIt(theGraph, aCanonFaceId); aWIt.More(); aWIt.Next())
              {
                const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aWIt.CurrentId());
                if (aWR.ChildWireId.IsRemoved(theGraph))
                {
                  continue;
                }
                aCanonOuter.Append(aWR.ChildWireId);
              }
              for (BRepGraph_RefsWireOfFace aWIt(theGraph, aCandFaceId); aWIt.More(); aWIt.Next())
              {
                const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aWIt.CurrentId());
                if (aWR.ChildWireId.IsRemoved(theGraph))
                {
                  continue;
                }
                aCandOuter.Append(aWR.ChildWireId);
              }
              // Sort wires for order-independent comparison.
              auto sortWires = [](WireIdList& theWires) {
                std::sort(theWires.begin(),
                          theWires.end(),
                          [](const BRepGraph_WireId& a, const BRepGraph_WireId& b) {
                            return a.Index < b.Index;
                          });
              };
              sortWires(aCanonOuter);
              sortWires(aCandOuter);
              if (aCanonOuter.Size() != aCandOuter.Size())
              {
                return false;
              }
              for (size_t i = 0; i < aCanonOuter.Size(); ++i)
              {
                if (aCanonOuter.Value(i) != aCandOuter.Value(i))
                {
                  return false;
                }
              }
              return true;
            }())
        {
          continue;
        }

        aCanonicalFace.Bind(aCandFaceId, aCanonFaceId);
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      for (NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>::Iterator anIt(aCanonicalFace);
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_FaceId anOldFaceId  = anIt.Key();
        const BRepGraph_FaceId aCanonFaceId = anIt.Value();
        const BRepGraph_NodeId anOldId      = anOldFaceId;
        const BRepGraph_NodeId aCanonId     = aCanonFaceId;

        // Redirect Shell face refs that point to the old face.
        for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph); aFaceRefIt.More();
             aFaceRefIt.Next())
        {
          const BRepGraph_FaceRefId    aFaceRefId = aFaceRefIt.CurrentId();
          const BRepGraphInc::FaceRef& aFaceRef   = theGraph.Refs().Faces().Entry(aFaceRefId);
          if (!theGraph.Refs().Gen().IsRemoved(aFaceRefId) && aFaceRef.ChildFaceId == anOldFaceId)
          {
            BRepGraph_MutGuard<BRepGraphInc::FaceRef> aMutFaceRef =
              theGraph.Editor().Faces().MutRef(aFaceRefId);
            theGraph.Editor().Faces().SetRefFaceId(aMutFaceRef, aCanonFaceId);
          }
        }

        // Redirect CoEdgeDef.FaceId entries that point to the old face to the canonical one.
        // This must happen before RemoveNode, otherwise compact will produce dangling FaceId
        // refs (CoEdges with invalid FaceId but live Curve2DRepId - orphaned PCurve state).
        for (BRepGraph_FullCoEdgeIterator aCEIt(theGraph); aCEIt.More(); aCEIt.Next())
        {
          const BRepGraph_CoEdgeId aCEId = aCEIt.CurrentId();
          if (BRepGraph_NodeId(aCEId).IsRemoved(theGraph))
          {
            continue;
          }
          if (theGraph.Topo().CoEdges().Definition(aCEId).FaceId == anOldFaceId)
          {
            BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aMutCE =
              theGraph.Editor().CoEdges().Mut(aCEId);
            theGraph.Editor().CoEdges().SetFaceId(aMutCE, aCanonFaceId);
          }
        }

        // Redirect CompoundDef.ChildRefIds that still point to the old face.
        for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
             aCompIt.Next())
        {
          const BRepGraph_CompoundId aCompId = aCompIt.CurrentId();
          if (aCompId.IsRemoved(theGraph))
          {
            continue;
          }
          for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aCompId); aRefIt.More();
               aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId()) && aCR.ChildNodeId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              theGraph.Editor().Gen().SetChildRefChildNodeId(aMutCR, aCanonId);
            }
          }
        }

        // Redirect OccurrenceDef.ChildNodeId entries that still point to the old face.
        // Snapshot first - SetChildNodeId modifies the underlying OccurrenceRefsOfNode vector.
        redirectOccurrenceChildren(theGraph, anOldId, aCanonId);

        theGraph.Editor().Gen().ReplaceNode(anOldId, aCanonId);
        aHistory.RecordReplaced(TCollection_AsciiString("Dedup:MergeFace"), anOldId, aCanonId);

        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedFaces;
      }
    }
    else
    {
      aResult.NbMergedFaces = static_cast<uint32_t>(aCanonicalFace.Size());
    }
  }

  aResult.IsEntityMergeApplied = !theOptions.AnalyzeOnly
                                 && (aResult.NbMergedVertices > 0 || aResult.NbMergedEdges > 0
                                     || aResult.NbMergedWires > 0 || aResult.NbMergedFaces > 0);

  aHistory.SetEnabled(wasHistoryEnabled);
  return aResult;
}
