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

#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_DeferredScope.hxx>

#include <GeomHash_CurveHasher.hxx>
#include <GeomHash_SurfaceHasher.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_KDTree.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_HashUtils.hxx>

#include <algorithm>
#include <cmath>

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
  if (!theGraph.IsDone())
  {
    return aResult;
  }
  BRepGraph_DeferredScope aDeferredScope(theGraph);

  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);

  GeomHash_SurfaceHasher aSurfHasher(theOptions.CompTolerance, theOptions.HashTolerance);
  GeomHash_CurveHasher   aCurveHasher(theOptions.CompTolerance, theOptions.HashTolerance);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator();

  // Deduplicate surfaces by comparing Handle pointers on FaceDefs.
  // Map: surface handle -> canonical face index (first face that owns it).
  NCollection_DataMap<occ::handle<Geom_Surface>, BRepGraph_FaceId, GeomHash_SurfaceHasher>
    aSurfToCanonicalFace(aSurfHasher, std::max(1, theGraph.Topo().Faces().Nb() * 2), aTmpAlloc);
  // Map: face id -> canonical face id (for faces whose surface should be replaced).
  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aSurfRewriteMap(
    std::max(1, theGraph.Topo().Faces().Nb()),
    aTmpAlloc);

  for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (theGraph.Topo().Faces().Definition(aFaceId).IsRemoved)
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
    aCurveToCanonicalEdge(aCurveHasher, std::max(1, theGraph.Topo().Edges().Nb() * 2), aTmpAlloc);
  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> aCurveRewriteMap(
    std::max(1, theGraph.Topo().Edges().Nb()),
    aTmpAlloc);

  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (theGraph.Topo().Edges().Definition(anEdgeId).IsRemoved)
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

  aResult.NbCanonicalSurfaces = theGraph.Topo().Faces().Nb() - aSurfRewriteMap.Length();
  aResult.NbCanonicalCurves   = theGraph.Topo().Edges().Nb() - aCurveRewriteMap.Length();

  if (theOptions.AnalyzeOnly && !theOptions.MergeEntitiesWhenSafe)
  {
    theGraph.History().SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  if (!theOptions.AnalyzeOnly)
  {
    // Rewrite face surfaces: replace duplicate surface handles with canonical ones.
    for (NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>::Iterator anIt(aSurfRewriteMap);
         anIt.More();
         anIt.Next())
    {
      const BRepGraph_FaceId       aFaceId      = anIt.Key();
      const BRepGraph_FaceId       aCanonFaceId = anIt.Value();
      const BRepGraph_SurfaceRepId aCanonSurfRepId =
        theGraph.Topo().Faces().Definition(aCanonFaceId).SurfaceRepId;
      const BRepGraph_SurfaceRepId aCurrentSurfRepId =
        theGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId;

      // Skip if already canonical (idempotency: avoid re-recording same rewrite).
      if (aCurrentSurfRepId == aCanonSurfRepId)
        continue;

      BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef = theGraph.Editor().Faces().Mut(aFaceId);
      aFaceDef->SurfaceRepId                             = aCanonSurfRepId;
      ++aResult.NbSurfaceRewrites;
      aResult.AffectedFaces.Append(aFaceId);

      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(aCanonFaceId);
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                                aFaceId,
                                aRepl);
      ++aResult.NbHistoryRecords;
    }

    // Rewrite edge curves: replace duplicate curve handles with canonical ones.
    for (NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>::Iterator anIt(aCurveRewriteMap);
         anIt.More();
         anIt.Next())
    {
      const BRepGraph_EdgeId       anEdgeId     = anIt.Key();
      const BRepGraph_EdgeId       aCanonEdgeId = anIt.Value();
      const BRepGraph_Curve3DRepId aCanonCurveRepId =
        theGraph.Topo().Edges().Definition(aCanonEdgeId).Curve3DRepId;
      const BRepGraph_Curve3DRepId aCurrentCurveRepId =
        theGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId;

      // Skip if already canonical (idempotency: avoid re-recording same rewrite).
      if (aCurrentCurveRepId == aCanonCurveRepId)
        continue;

      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdgeDef = theGraph.Editor().Edges().Mut(anEdgeId);
      anEdgeDef->Curve3DRepId                             = aCanonCurveRepId;
      ++aResult.NbCurveRewrites;
      aResult.AffectedEdges.Append(anEdgeId);

      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(aCanonEdgeId);
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                                anEdgeId,
                                aRepl);
      ++aResult.NbHistoryRecords;
    }

  } // end if (!theOptions.AnalyzeOnly) for geometry rewrites

  // Definition merge phases (Vertex -> Edge -> Wire -> Face).
  if (!theOptions.MergeEntitiesWhenSafe)
  {
    aResult.IsEntityMergeApplied = false;
    theGraph.History().SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  // Phase 1: Vertex Merging via KDTree range search.
  {
    const double aTol = theOptions.HashTolerance;

    // Collect active vertices: (point, graph id) pairs.
    const int aNbVertices = theGraph.Topo().Vertices().Nb();
    NCollection_Vector<std::pair<gp_Pnt, BRepGraph_VertexId>> aActiveVertices(256, aTmpAlloc);
    for (BRepGraph_FullVertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
    {
      const BRepGraph_VertexId       aVertexId = aVertexIt.CurrentId();
      const BRepGraphInc::VertexDef& aVtx      = theGraph.Topo().Vertices().Definition(aVertexId);
      if (aVtx.IsRemoved)
        continue;
      aActiveVertices.Append(
        std::make_pair(BRepGraph_Tool::Vertex::Pnt(theGraph, aVertexId), aVertexId));
    }

    // Build KDTree from active vertex points - O(n log n).
    const int                  aNbActive = aActiveVertices.Length();
    NCollection_Array1<gp_Pnt> aPointsArr(0, std::max(0, aNbActive - 1));
    for (int i = 0; i < aNbActive; ++i)
      aPointsArr.SetValue(i, aActiveVertices.Value(i).first);

    NCollection_KDTree<gp_Pnt, 3> aTree;
    if (!aPointsArr.IsEmpty())
      aTree.Build(aPointsArr);

    // Canonical vertex map: old graph id -> canonical graph id.
    NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aCanonicalVertex(
      std::max(1, aNbVertices),
      aTmpAlloc);

    for (int aLocalIdx = 0; aLocalIdx < aNbActive; ++aLocalIdx)
    {
      const BRepGraph_VertexId aBaseVtxId = aActiveVertices.Value(aLocalIdx).second;
      if (aCanonicalVertex.IsBound(aBaseVtxId))
        continue;

      const gp_Pnt aBaseVtxPnt = BRepGraph_Tool::Vertex::Pnt(theGraph, aBaseVtxId);
      const double aBaseVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, aBaseVtxId);

      aTree.ForEachInRange(aBaseVtxPnt, aTol, [&](size_t theResultIdx) {
        const int anArrayIdx = static_cast<int>(theResultIdx) - 1;
        if (anArrayIdx <= aLocalIdx)
          return; // skip self and already-processed

        const BRepGraph_VertexId aCandVtxId = aActiveVertices.Value(anArrayIdx).second;
        if (aCanonicalVertex.IsBound(aCandVtxId))
          return;

        const double aCandVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, aCandVtxId);
        const double aMaxTol     = std::max(aBaseVtxTol, aCandVtxTol);
        if (aBaseVtxPnt.Distance(BRepGraph_Tool::Vertex::Pnt(theGraph, aCandVtxId)) <= aMaxTol)
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
          if (anEdge->IsRemoved)
            continue;
          // Resolve current vertex def ids through ref entries and update them.
          if (anEdge->StartVertexRefId.IsValid())
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> aStartRef =
              theGraph.Editor().Vertices().MutRef(anEdge->StartVertexRefId);
            if (aStartRef->VertexDefId == anOldVertexId)
              aStartRef->VertexDefId = aCanonVertexId;
          }
          if (anEdge->EndVertexRefId.IsValid())
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> anEndRef =
              theGraph.Editor().Vertices().MutRef(anEdge->EndVertexRefId);
            if (anEndRef->VertexDefId == anOldVertexId)
              anEndRef->VertexDefId = aCanonVertexId;
          }
          for (const BRepGraph_VertexRefId& anInternalRefId : anEdge->InternalVertexRefIds)
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> anInternalRef =
              theGraph.Editor().Vertices().MutRef(anInternalRefId);
            if (anInternalRef->VertexDefId == anOldVertexId)
              anInternalRef->VertexDefId = aCanonVertexId;
          }
        }

        // Update faces that directly reference the old vertex via FaceDef.VertexRefIds.
        for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
        {
          const BRepGraph_FaceId       aFaceId  = aFaceIt.CurrentId();
          const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(aFaceId);
          if (aFaceDef.IsRemoved)
            continue;
          for (const BRepGraph_VertexRefId& aVRefId : aFaceDef.VertexRefIds)
          {
            BRepGraph_MutGuard<BRepGraphInc::VertexRef> aVRef =
              theGraph.Editor().Vertices().MutRef(aVRefId);
            if (aVRef->VertexDefId == anOldVertexId)
              aVRef->VertexDefId = aCanonVertexId;
          }
        }

        // Mark non-canonical as removed.
        theGraph.Editor().Gen().RemoveNode(anOldId, aCanonId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeVertex"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedVertices;
      }
    }
    else
    {
      aResult.NbMergedVertices = aCanonicalVertex.Length();
    }
  }

  // Phase 2: Edge Merging.
  {
    // Key: (canonical Curve3d pointer, canonical StartVertexDefId, canonical EndVertexDefId).
    struct EdgeKey
    {
      const Geom_Curve*  CurvePtr;
      BRepGraph_VertexId StartVtx;
      BRepGraph_VertexId EndVtx;

      bool operator==(const EdgeKey& theOther) const
      {
        return CurvePtr == theOther.CurvePtr && StartVtx == theOther.StartVtx
               && EndVtx == theOther.EndVtx;
      }
    };

    struct EdgeKeyHasher
    {
      size_t operator()(const EdgeKey& theKey) const noexcept
      {
        size_t aCombination[3];
        aCombination[0] = std::hash<const void*>{}(theKey.CurvePtr);
        aCombination[1] = opencascade::hash(theKey.StartVtx);
        aCombination[2] = opencascade::hash(theKey.EndVtx);
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }

      bool operator()(const EdgeKey& theA, const EdgeKey& theB) const { return theA == theB; }
    };

    NCollection_DataMap<EdgeKey, NCollection_Vector<BRepGraph_EdgeId>, EdgeKeyHasher> anEdgeGroups(
      std::max(1, theGraph.Topo().Edges().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
      const BRepGraphInc::EdgeDef& anEdge   = theGraph.Topo().Edges().Definition(anEdgeId);
      if (anEdge.IsRemoved || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
        continue;

      // Use canonical (forward) key: use raw pointer as a stable identity.
      EdgeKey aKey;
      aKey.CurvePtr = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId).get();
      const BRepGraph_VertexId aStartVtxId =
        BRepGraph_Tool::Edge::StartVertexId(theGraph, anEdgeId);
      const BRepGraph_VertexId anEndVtxId = BRepGraph_Tool::Edge::EndVertexId(theGraph, anEdgeId);
      aKey.StartVtx                       = aStartVtxId;
      aKey.EndVtx                         = anEndVtxId;

      // Normalize: always use min vertex index first for undirected matching.
      if (aKey.StartVtx > aKey.EndVtx)
        std::swap(aKey.StartVtx, aKey.EndVtx);

      anEdgeGroups.TryBind(aKey, NCollection_Vector<BRepGraph_EdgeId>());
      anEdgeGroups.ChangeFind(aKey).Append(anEdgeId);
    }

    NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> aCanonicalEdge(
      std::max(1, theGraph.Topo().Edges().Nb()),
      aTmpAlloc);

    for (NCollection_DataMap<EdgeKey, NCollection_Vector<BRepGraph_EdgeId>, EdgeKeyHasher>::Iterator
           aGroupIter(anEdgeGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const NCollection_Vector<BRepGraph_EdgeId>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const BRepGraph_EdgeId       aCanonEdgeId = aGroup.Value(0);
      const BRepGraphInc::EdgeDef& aCanonEdge   = theGraph.Topo().Edges().Definition(aCanonEdgeId);

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const BRepGraph_EdgeId       aCandEdgeId = aGroup.Value(aCandIter);
        const BRepGraphInc::EdgeDef& aCandEdge   = theGraph.Topo().Edges().Definition(aCandEdgeId);

        // Compare parameter ranges within tolerance.
        if (std::abs(aCanonEdge.ParamFirst - aCandEdge.ParamFirst) > theOptions.CompTolerance)
          continue;
        if (std::abs(aCanonEdge.ParamLast - aCandEdge.ParamLast) > theOptions.CompTolerance)
          continue;

        // Check tolerance compatibility.
        if (aCandEdge.Tolerance > aCanonEdge.Tolerance * 10.0)
          continue;

        aCanonicalEdge.Bind(aCandEdgeId, aCanonEdgeId);
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
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
        const BRepGraph_NodeId aCanonStart =
          BRepGraph_Tool::Edge::StartVertexId(theGraph, aCanonEdgeId);
        const BRepGraph_NodeId aCanonEnd =
          BRepGraph_Tool::Edge::EndVertexId(theGraph, aCanonEdgeId);
        const BRepGraph_NodeId anOldStart =
          BRepGraph_Tool::Edge::StartVertexId(theGraph, anOldEdgeId);
        const BRepGraph_NodeId anOldEnd = BRepGraph_Tool::Edge::EndVertexId(theGraph, anOldEdgeId);
        const bool             isReversed = (aCanonStart == anOldEnd && aCanonEnd == anOldStart);

        // Replace in wires.
        const NCollection_Vector<BRepGraph_WireId>& aWires =
          theGraph.Topo().Edges().Wires(anOldEdgeId);
        for (int aWireIter = 0; aWireIter < aWires.Length(); ++aWireIter)
        {
          theGraph.Editor().Wires().ReplaceEdge(aWires.Value(aWireIter),
                                                anOldEdgeId,
                                                aCanonEdgeId,
                                                isReversed);
        }

        // ReplaceEdge() above rebinds all CoEdgeDef.EdgeDefId entries from anOldEdgeId
        // to aCanonEdgeId (and updates the reverse CoEdgesOfEdge index), so
        // theGraph.Topo().Edges().CoEdges(anOldEdgeId) is always empty at this point.
        // PCurve handles are preserved through the CoEdge rebinding.

        theGraph.Editor().Gen().RemoveNode(anOldId, aCanonId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeEdge"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedEdges;
      }
    }
    else
    {
      aResult.NbMergedEdges = aCanonicalEdge.Length();
    }
  }

  // Phase 3: Wire Merging.
  {
    // Hash wire by its ordered coedge sequence (edge index + sense from CoEdgeDef).
    struct WireHash
    {
      size_t operator()(const BRepGraph_WireId theWireId, const BRepGraph& theGraph) const
      {
        size_t aHash = 0;
        for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, theWireId); aCEIt.More(); aCEIt.Next())
        {
          const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(
            theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
          size_t aEntryHash[2];
          aEntryHash[0] = opencascade::hash(aCoEdge.EdgeDefId);
          aEntryHash[1] = opencascade::hash(static_cast<int>(aCoEdge.Orientation));
          aHash ^= opencascade::hashBytes(aEntryHash, sizeof(aEntryHash)) + 0x9e3779b9
                   + (aHash << 6) + (aHash >> 2);
        }
        return aHash;
      }
    };

    auto wiresEqual = [&](const BRepGraph_WireId theA, const BRepGraph_WireId theB) -> bool {
      NCollection_Vector<BRepGraph_CoEdgeId> aWireACoEdges;
      NCollection_Vector<BRepGraph_CoEdgeId> aWireBCoEdges;
      for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, theA); aCEIt.More(); aCEIt.Next())
      {
        aWireACoEdges.Append(theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      }
      for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, theB); aCEIt.More(); aCEIt.Next())
      {
        aWireBCoEdges.Append(theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      }

      if (aWireACoEdges.Length() != aWireBCoEdges.Length())
        return false;
      for (int anIdx = 0; anIdx < aWireACoEdges.Length(); ++anIdx)
      {
        const BRepGraphInc::CoEdgeDef& aCoEdgeA =
          theGraph.Topo().CoEdges().Definition(aWireACoEdges.Value(anIdx));
        const BRepGraphInc::CoEdgeDef& aCoEdgeB =
          theGraph.Topo().CoEdges().Definition(aWireBCoEdges.Value(anIdx));
        if (aCoEdgeA.EdgeDefId != aCoEdgeB.EdgeDefId
            || aCoEdgeA.Orientation != aCoEdgeB.Orientation)
          return false;
      }
      return true;
    };

    NCollection_DataMap<size_t, NCollection_Vector<BRepGraph_WireId>> aWireHashBuckets(
      std::max(1, theGraph.Topo().Wires().Nb()),
      aTmpAlloc);

    WireHash aHasher;
    for (BRepGraph_FullWireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraph_WireId       aWireId = aWireIt.CurrentId();
      const BRepGraphInc::WireDef& aWire   = theGraph.Topo().Wires().Definition(aWireId);
      if (aWire.IsRemoved)
        continue;
      const size_t aH = aHasher(aWireId, theGraph);
      aWireHashBuckets.TryBind(aH, NCollection_Vector<BRepGraph_WireId>());
      aWireHashBuckets.ChangeFind(aH).Append(aWireId);
    }

    NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId> aCanonicalWire(
      std::max(1, theGraph.Topo().Wires().Nb()),
      aTmpAlloc);

    for (NCollection_DataMap<size_t, NCollection_Vector<BRepGraph_WireId>>::Iterator aBucketIter(
           aWireHashBuckets);
         aBucketIter.More();
         aBucketIter.Next())
    {
      const NCollection_Vector<BRepGraph_WireId>& aBucket = aBucketIter.Value();
      for (int aBaseIdx = 0; aBaseIdx < aBucket.Length(); ++aBaseIdx)
      {
        const BRepGraph_WireId aBaseWireId = aBucket.Value(aBaseIdx);
        if (aCanonicalWire.IsBound(aBaseWireId))
          continue;

        for (int aCandIdx = aBaseIdx + 1; aCandIdx < aBucket.Length(); ++aCandIdx)
        {
          const BRepGraph_WireId aCandWireId = aBucket.Value(aCandIdx);
          if (aCanonicalWire.IsBound(aCandWireId))
            continue;
          if (wiresEqual(aBaseWireId, aCandWireId))
            aCanonicalWire.Bind(aCandWireId, aBaseWireId);
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
          const BRepGraph_FaceId       aFaceId  = aFaceIt.CurrentId();
          const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(aFaceId);
          if (aFaceDef.IsRemoved)
          {
            continue;
          }

          for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefIds.Length(); ++aWireRefIdx)
          {
            const BRepGraph_WireRefId    aWireRefId = aFaceDef.WireRefIds.Value(aWireRefIdx);
            const BRepGraphInc::WireRef& aWireRef   = theGraph.Refs().Wires().Entry(aWireRefId);
            if (!aWireRef.IsRemoved && aWireRef.WireDefId == anOldWireId)
            {
              BRepGraph_MutGuard<BRepGraphInc::WireRef> aMutWireRef =
                theGraph.Editor().Wires().MutRef(aWireRefId);
              aMutWireRef->WireDefId = aCanonWireId;
            }
          }
        }

        // Redirect ShellDef.AuxChildRefIds and SolidDef.AuxChildRefIds that still
        // point to the old wire as a non-face child.
        for (BRepGraph_FullShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
        {
          const BRepGraph_ShellId aShellId = aShellIt.CurrentId();
          if (theGraph.Topo().Shells().Definition(aShellId).IsRemoved)
            continue;
          for (BRepGraph_RefsChildOfShell aRefIt(theGraph, aShellId); aRefIt.More(); aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!aCR.IsRemoved && aCR.ChildDefId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              aMutCR->ChildDefId = aCanonId;
            }
          }
        }
        for (BRepGraph_FullSolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
        {
          const BRepGraph_SolidId aSolidId = aSolidIt.CurrentId();
          if (theGraph.Topo().Solids().Definition(aSolidId).IsRemoved)
            continue;
          for (BRepGraph_RefsChildOfSolid aRefIt(theGraph, aSolidId); aRefIt.More(); aRefIt.Next())
          {
            const BRepGraphInc::ChildRef& aCR =
              theGraph.Refs().Children().Entry(aRefIt.CurrentId());
            if (!aCR.IsRemoved && aCR.ChildDefId == anOldId)
            {
              BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMutCR =
                theGraph.Editor().Gen().MutChildRef(aRefIt.CurrentId());
              aMutCR->ChildDefId = aCanonId;
            }
          }
        }

        theGraph.Editor().Gen().RemoveNode(anOldId, aCanonId);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedWires;
      }
    }
    else
    {
      aResult.NbMergedWires = aCanonicalWire.Length();
    }
  }

  // Phase 4: Face Merging.
  {
    struct FaceKey
    {
      const Geom_Surface* SurfPtr;
      size_t              WireHash;

      bool operator==(const FaceKey& theOther) const
      {
        return SurfPtr == theOther.SurfPtr && WireHash == theOther.WireHash;
      }
    };

    struct FaceKeyHasher
    {
      size_t operator()(const FaceKey& theKey) const noexcept
      {
        size_t aCombination[2];
        aCombination[0] = std::hash<const void*>{}(theKey.SurfPtr);
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
        if (aWR.IsOuter)
        {
          aHash ^= opencascade::hash(aWR.WireDefId);
        }
        else
        {
          aHash ^= opencascade::hash(aWR.WireDefId) + 0x9e3779b9;
        }
      }
      return aHash;
    };

    NCollection_DataMap<FaceKey, NCollection_Vector<BRepGraph_FaceId>, FaceKeyHasher> aFaceGroups(
      std::max(1, theGraph.Topo().Faces().Nb()),
      aTmpAlloc);

    for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    {
      const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();
      const BRepGraphInc::FaceDef& aFace   = theGraph.Topo().Faces().Definition(aFaceId);
      if (aFace.IsRemoved || !BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
        continue;

      FaceKey aKey;
      aKey.SurfPtr  = BRepGraph_Tool::Face::Surface(theGraph, aFaceId).get();
      aKey.WireHash = faceWireHash(aFaceId);

      aFaceGroups.TryBind(aKey, NCollection_Vector<BRepGraph_FaceId>());
      aFaceGroups.ChangeFind(aKey).Append(aFaceId);
    }

    NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aCanonicalFace(
      std::max(1, theGraph.Topo().Faces().Nb()),
      aTmpAlloc);

    for (NCollection_DataMap<FaceKey, NCollection_Vector<BRepGraph_FaceId>, FaceKeyHasher>::Iterator
           aGroupIter(aFaceGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const NCollection_Vector<BRepGraph_FaceId>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const BRepGraph_FaceId       aCanonFaceId = aGroup.Value(0);
      const BRepGraphInc::FaceDef& aCanonFace   = theGraph.Topo().Faces().Definition(aCanonFaceId);

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const BRepGraph_FaceId       aCandFaceId = aGroup.Value(aCandIter);
        const BRepGraphInc::FaceDef& aCandFace   = theGraph.Topo().Faces().Definition(aCandFaceId);

        // Check tolerance compatibility.
        if (aCandFace.Tolerance > aCanonFace.Tolerance * 10.0)
          continue;

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
          if (!aFaceRef.IsRemoved && aFaceRef.FaceDefId == anOldFaceId)
          {
            BRepGraph_MutGuard<BRepGraphInc::FaceRef> aMutFaceRef =
              theGraph.Editor().Faces().MutRef(aFaceRefId);
            aMutFaceRef->FaceDefId = aCanonFaceId;
          }
        }

        // Redirect CoEdgeDef.FaceDefId entries that point to the old face to the canonical one.
        // This must happen before RemoveNode, otherwise compact will produce dangling FaceDefId
        // refs (CoEdges with invalid FaceDefId but live Curve2DRepId — orphaned PCurve state).
        for (BRepGraph_FullCoEdgeIterator aCEIt(theGraph); aCEIt.More(); aCEIt.Next())
        {
          const BRepGraph_CoEdgeId aCEId = aCEIt.CurrentId();
          if (theGraph.Topo().CoEdges().Definition(aCEId).IsRemoved)
            continue;
          if (theGraph.Topo().CoEdges().Definition(aCEId).FaceDefId == anOldFaceId)
          {
            BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aMutCE =
              theGraph.Editor().CoEdges().Mut(aCEId);
            aMutCE->FaceDefId = aCanonFaceId;
          }
        }

        theGraph.Editor().Gen().RemoveNode(anOldId, aCanonId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeFace"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedFaces;
      }
    }
    else
    {
      aResult.NbMergedFaces = aCanonicalFace.Length();
    }
  }

  aResult.IsEntityMergeApplied = !theOptions.AnalyzeOnly
                                 && (aResult.NbMergedVertices > 0 || aResult.NbMergedEdges > 0
                                     || aResult.NbMergedWires > 0 || aResult.NbMergedFaces > 0);

  theGraph.History().SetEnabled(wasHistoryEnabled);
  return aResult;
}
