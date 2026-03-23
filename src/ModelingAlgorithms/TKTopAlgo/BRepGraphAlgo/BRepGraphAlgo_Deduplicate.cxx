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

#include <BRepGraphAlgo_Deduplicate.hxx>

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_UsagesView.hxx>

#include <GeomHash_CurveHasher.hxx>
#include <GeomHash_SurfaceHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_HashUtils.hxx>

#include <algorithm>
#include <cmath>

//=================================================================================================

BRepGraphAlgo_Deduplicate::Result BRepGraphAlgo_Deduplicate::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_Deduplicate::Result BRepGraphAlgo_Deduplicate::Perform(BRepGraph&     theGraph,
                                                                     const Options& theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  const bool wasHistoryEnabled = theGraph.IsHistoryEnabled();
  theGraph.SetHistoryEnabled(theOptions.HistoryMode);

  GeomHash_SurfaceHasher aSurfHasher(theOptions.CompTolerance, theOptions.HashTolerance);
  GeomHash_CurveHasher   aCurveHasher(theOptions.CompTolerance, theOptions.HashTolerance);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator();

  NCollection_DataMap<occ::handle<Geom_Surface>, int, GeomHash_SurfaceHasher> aSurfToCanonical(
    aSurfHasher,
    std::max(1, theGraph.Geom().NbSurfaces() * 2),
    aTmpAlloc);
  NCollection_DataMap<occ::handle<Geom_Curve>, int, GeomHash_CurveHasher> aCurveToCanonical(
    aCurveHasher,
    std::max(1, theGraph.Geom().NbCurves() * 2),
    aTmpAlloc);
  NCollection_DataMap<int, int> aCanonicalBySurf(std::max(1, theGraph.Geom().NbSurfaces() * 2),
                                                 aTmpAlloc);
  NCollection_DataMap<int, int> aCanonicalByCurve(std::max(1, theGraph.Geom().NbCurves() * 2),
                                                  aTmpAlloc);

  for (int aSurfIter = 0; aSurfIter < theGraph.Geom().NbSurfaces(); ++aSurfIter)
  {
    const occ::handle<Geom_Surface>& aSurf = theGraph.Geom().Surface(aSurfIter).Surface;
    if (aSurf.IsNull())
    {
      continue;
    }

    const int* aCanonicalIdx = aSurfToCanonical.Seek(aSurf);
    if (aCanonicalIdx == nullptr)
    {
      aSurfToCanonical.Bind(aSurf, aSurfIter);
    }
    else if (*aCanonicalIdx != aSurfIter)
    {
      aCanonicalBySurf.Bind(aSurfIter, *aCanonicalIdx);
    }
  }

  for (int aCurveIter = 0; aCurveIter < theGraph.Geom().NbCurves(); ++aCurveIter)
  {
    const occ::handle<Geom_Curve>& aCurve = theGraph.Geom().Curve(aCurveIter).CurveGeom;
    if (aCurve.IsNull())
    {
      continue;
    }

    const int* aCanonicalIdx = aCurveToCanonical.Seek(aCurve);
    if (aCanonicalIdx == nullptr)
    {
      aCurveToCanonical.Bind(aCurve, aCurveIter);
    }
    else if (*aCanonicalIdx != aCurveIter)
    {
      aCanonicalByCurve.Bind(aCurveIter, *aCanonicalIdx);
    }
  }

  aResult.NbCanonicalSurfaces = theGraph.Geom().NbSurfaces() - aCanonicalBySurf.Size();
  aResult.NbCanonicalCurves   = theGraph.Geom().NbCurves() - aCanonicalByCurve.Size();

  if (theOptions.AnalyzeOnly && !theOptions.MergeDefsWhenSafe)
  {
    theGraph.SetHistoryEnabled(wasHistoryEnabled);
    return aResult;
  }

  if (!theOptions.AnalyzeOnly)
  {
  NCollection_Map<int> aRecordedSurfaceHistory(std::max(1, aCanonicalBySurf.Size() * 2), aTmpAlloc);
  NCollection_Map<int> aRecordedCurveHistory(std::max(1, aCanonicalByCurve.Size() * 2), aTmpAlloc);

  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Mut().FaceDef(aFaceIdx);
    if (!aFaceDef.SurfNodeId.IsValid())
    {
      continue;
    }

    const int* aCanonSurf = aCanonicalBySurf.Seek(aFaceDef.SurfNodeId.Index);
    if (aCanonSurf == nullptr)
    {
      continue;
    }

    const BRepGraph_NodeId anOldSurfId = aFaceDef.SurfNodeId;
    aFaceDef.SurfNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, *aCanonSurf);
    ++aResult.NbSurfaceRewrites;
    aResult.AffectedFaceDefs.Append(aFaceDef.Id);

    if (!aRecordedSurfaceHistory.Contains(anOldSurfId.Index))
    {
      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, *aCanonSurf));
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                                anOldSurfId,
                                aRepl);
      aRecordedSurfaceHistory.Add(anOldSurfId.Index);
      ++aResult.NbHistoryRecords;
    }
  }

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Mut().EdgeDef(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
    {
      const int* aCanonCurve = aCanonicalByCurve.Seek(anEdgeDef.CurveNodeId.Index);
      if (aCanonCurve != nullptr)
      {
        const BRepGraph_NodeId anOldCurveId = anEdgeDef.CurveNodeId;
        anEdgeDef.CurveNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, *aCanonCurve);
        ++aResult.NbCurveRewrites;
        aResult.AffectedEdgeDefs.Append(anEdgeDef.Id);

        if (!aRecordedCurveHistory.Contains(anOldCurveId.Index))
        {
          NCollection_Vector<BRepGraph_NodeId> aRepl;
          aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, *aCanonCurve));
          theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                                    anOldCurveId,
                                    aRepl);
          aRecordedCurveHistory.Add(anOldCurveId.Index);
          ++aResult.NbHistoryRecords;
        }
      }
    }
  }

  // Nullify orphaned geometry handles to free memory.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    BRepGraph_GeomNode::Surf& aSurfNode = theGraph.Mut().SurfNode(aSurfIdx);
    if (aSurfNode.Surface.IsNull())
    {
      continue;
    }
    // Check if any FaceDef references this surface.
    bool aIsReferenced = false;
    for (int aFIdx = 0; aFIdx < theGraph.Defs().NbFaces() && !aIsReferenced; ++aFIdx)
    {
      if (theGraph.Defs().Face(aFIdx).SurfNodeId.Index == aSurfIdx)
      {
        aIsReferenced = true;
      }
    }
    if (!aIsReferenced)
    {
      aSurfNode.Surface.Nullify();
      ++aResult.NbNullifiedSurfaces;

      NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
      theGraph.History().Record(TCollection_AsciiString("Dedup:NullifyOrphanSurface"),
                                aSurfNode.Id,
                                aEmptyRepl);
      ++aResult.NbHistoryRecords;
    }
  }

  for (int aCurveIdx = 0; aCurveIdx < theGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    BRepGraph_GeomNode::Curve& aCurveNode = theGraph.Mut().CurveNode(aCurveIdx);
    if (aCurveNode.CurveGeom.IsNull())
    {
      continue;
    }
    // Check if any EdgeDef references this curve.
    bool aIsReferenced = false;
    for (int aEIdx = 0; aEIdx < theGraph.Defs().NbEdges() && !aIsReferenced; ++aEIdx)
    {
      if (theGraph.Defs().Edge(aEIdx).CurveNodeId.Index == aCurveIdx)
      {
        aIsReferenced = true;
      }
    }
    if (!aIsReferenced)
    {
      aCurveNode.CurveGeom.Nullify();
      ++aResult.NbNullifiedCurves;

      NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
      theGraph.History().Record(TCollection_AsciiString("Dedup:NullifyOrphanCurve"),
                                aCurveNode.Id,
                                aEmptyRepl);
      ++aResult.NbHistoryRecords;
    }
  }

  } // end if (!theOptions.AnalyzeOnly) for geometry rewrites

  // Definition merge phases (Vertex -> Edge -> Wire -> Face).
  if (!theOptions.MergeDefsWhenSafe)
  {
    aResult.IsDefMergeApplied = false;
    theGraph.SetHistoryEnabled(wasHistoryEnabled);
    return aResult;
  }

  // Phase 1: Vertex Merging via spatial hash.
  {
    const double aTol = theOptions.HashTolerance;
    const double aInvTol = (aTol > 0.0) ? 1.0 / aTol : 1.0;

    // Spatial hash buckets: discretized coords -> list of vertex indices.
    NCollection_DataMap<int64_t, NCollection_Vector<int>> aSpatialBuckets(
      std::max(1, theGraph.Defs().NbVertices()),
      aTmpAlloc);

    auto spatialKey = [&](const gp_Pnt& thePnt) -> int64_t
    {
      const int64_t aX = static_cast<int64_t>(std::floor(thePnt.X() * aInvTol));
      const int64_t aY = static_cast<int64_t>(std::floor(thePnt.Y() * aInvTol));
      const int64_t aZ = static_cast<int64_t>(std::floor(thePnt.Z() * aInvTol));
      // Simple hash combining.
      return (aX * 73856093) ^ (aY * 19349663) ^ (aZ * 83492791);
    };

    for (int aVtxIdx = 0; aVtxIdx < theGraph.Defs().NbVertices(); ++aVtxIdx)
    {
      const BRepGraph_TopoNode::VertexDef& aVtx = theGraph.Defs().Vertex(aVtxIdx);
      if (aVtx.IsRemoved)
        continue;
      const int64_t aKey = spatialKey(aVtx.Point);
      aSpatialBuckets.TryBind(aKey, NCollection_Vector<int>());
      aSpatialBuckets.ChangeFind(aKey).Append(aVtxIdx);
    }

    // Canonical vertex map: old index -> canonical index.
    NCollection_DataMap<int, int> aCanonicalVertex(
      std::max(1, theGraph.Defs().NbVertices()),
      aTmpAlloc);

    for (NCollection_DataMap<int64_t, NCollection_Vector<int>>::Iterator
           aBucketIter(aSpatialBuckets);
         aBucketIter.More(); aBucketIter.Next())
    {
      const NCollection_Vector<int>& aBucket = aBucketIter.Value();
      for (int aBaseIdx = 0; aBaseIdx < aBucket.Length(); ++aBaseIdx)
      {
        const int aBaseVtxIdx = aBucket.Value(aBaseIdx);
        if (aCanonicalVertex.IsBound(aBaseVtxIdx))
          continue;

        const BRepGraph_TopoNode::VertexDef& aBaseVtx = theGraph.Defs().Vertex(aBaseVtxIdx);
        if (aBaseVtx.IsRemoved)
          continue;

        for (int aCandIdx = aBaseIdx + 1; aCandIdx < aBucket.Length(); ++aCandIdx)
        {
          const int aCandVtxIdx = aBucket.Value(aCandIdx);
          if (aCanonicalVertex.IsBound(aCandVtxIdx))
            continue;

          const BRepGraph_TopoNode::VertexDef& aCandVtx = theGraph.Defs().Vertex(aCandVtxIdx);
          if (aCandVtx.IsRemoved)
            continue;

          const double aMaxTol = std::max(aBaseVtx.Tolerance, aCandVtx.Tolerance);
          if (aBaseVtx.Point.Distance(aCandVtx.Point) <= aMaxTol)
          {
            aCanonicalVertex.Bind(aCandVtxIdx, aBaseVtxIdx);
          }
        }
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Redirect edge vertex references and mark non-canonical vertices as removed.
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalVertex); anIt.More(); anIt.Next())
      {
        const int anOldIdx      = anIt.Key();
        const int aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId = BRepGraph_NodeId::Vertex(anOldIdx);
        const BRepGraph_NodeId aCanonId = BRepGraph_NodeId::Vertex(aCanonicalIdx);

        // Update edges referencing the old vertex.
        for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
        {
          BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Mut().EdgeDef(anEdgeIdx);
          if (anEdge.IsRemoved)
            continue;
          if (anEdge.StartVertexDefId == anOldId)
            anEdge.StartVertexDefId = aCanonId;
          if (anEdge.EndVertexDefId == anOldId)
            anEdge.EndVertexDefId = aCanonId;
        }

        // Transfer usages from old vertex to canonical.
        BRepGraph_TopoNode::VertexDef& aCanonVtx = theGraph.Mut().VertexDef(aCanonicalIdx);
        const BRepGraph_TopoNode::VertexDef& anOldVtx = theGraph.Defs().Vertex(anOldIdx);
        for (int anUsIdx = 0; anUsIdx < anOldVtx.Usages.Length(); ++anUsIdx)
          aCanonVtx.Usages.Append(anOldVtx.Usages.Value(anUsIdx));

        // Mark non-canonical as removed.
        theGraph.Builder().RemoveNode(anOldId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeVertex"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedVertices;
      }
    }
    else
    {
      aResult.NbMergedVertices = aCanonicalVertex.Size();
    }
  }

  // Phase 2: Edge Merging.
  {
    // Key: (canonical CurveNodeId, canonical StartVertexDefId, canonical EndVertexDefId).
    struct EdgeKey
    {
      int CurveIdx;
      int StartVtx;
      int EndVtx;

      bool operator==(const EdgeKey& theOther) const
      {
        return CurveIdx == theOther.CurveIdx
            && StartVtx == theOther.StartVtx
            && EndVtx == theOther.EndVtx;
      }
    };

    struct EdgeKeyHasher
    {
      size_t operator()(const EdgeKey& theKey) const noexcept
      {
        size_t aCombination[3];
        aCombination[0] = opencascade::hash(theKey.CurveIdx);
        aCombination[1] = opencascade::hash(theKey.StartVtx);
        aCombination[2] = opencascade::hash(theKey.EndVtx);
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }

      bool operator()(const EdgeKey& theA, const EdgeKey& theB) const
      {
        return theA == theB;
      }
    };

    NCollection_DataMap<EdgeKey, NCollection_Vector<int>, EdgeKeyHasher> anEdgeGroups(
      std::max(1, theGraph.Defs().NbEdges()),
      aTmpAlloc);

    for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
      if (anEdge.IsRemoved || !anEdge.CurveNodeId.IsValid())
        continue;

      // Use canonical (forward) key.
      EdgeKey aKey;
      aKey.CurveIdx = anEdge.CurveNodeId.Index;
      aKey.StartVtx = anEdge.StartVertexDefId.IsValid() ? anEdge.StartVertexDefId.Index : -1;
      aKey.EndVtx   = anEdge.EndVertexDefId.IsValid() ? anEdge.EndVertexDefId.Index : -1;

      // Normalize: always use min vertex index first for undirected matching.
      if (aKey.StartVtx > aKey.EndVtx)
        std::swap(aKey.StartVtx, aKey.EndVtx);

      anEdgeGroups.TryBind(aKey, NCollection_Vector<int>());
      anEdgeGroups.ChangeFind(aKey).Append(anEdgeIdx);
    }

    NCollection_DataMap<int, int> aCanonicalEdge(
      std::max(1, theGraph.Defs().NbEdges()),
      aTmpAlloc);

    for (NCollection_DataMap<EdgeKey, NCollection_Vector<int>, EdgeKeyHasher>::Iterator
           aGroupIter(anEdgeGroups);
         aGroupIter.More(); aGroupIter.Next())
    {
      const NCollection_Vector<int>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const int aCanonIdx = aGroup.Value(0);
      const BRepGraph_TopoNode::EdgeDef& aCanonEdge = theGraph.Defs().Edge(aCanonIdx);

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const int aCandIdx = aGroup.Value(aCandIter);
        const BRepGraph_TopoNode::EdgeDef& aCandEdge = theGraph.Defs().Edge(aCandIdx);

        // Compare parameter ranges within tolerance.
        if (std::abs(aCanonEdge.ParamFirst - aCandEdge.ParamFirst) > theOptions.CompTolerance)
          continue;
        if (std::abs(aCanonEdge.ParamLast - aCandEdge.ParamLast) > theOptions.CompTolerance)
          continue;

        // Check tolerance compatibility.
        if (aCandEdge.Tolerance > aCanonEdge.Tolerance * 10.0)
          continue;

        aCanonicalEdge.Bind(aCandIdx, aCanonIdx);
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalEdge); anIt.More(); anIt.Next())
      {
        const int anOldIdx      = anIt.Key();
        const int aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId  = BRepGraph_NodeId::Edge(anOldIdx);
        const BRepGraph_NodeId aCanonId = BRepGraph_NodeId::Edge(aCanonicalIdx);

        // Determine if the non-canonical edge is reversed relative to canonical.
        const BRepGraph_TopoNode::EdgeDef& aCanonEdge = theGraph.Defs().Edge(aCanonicalIdx);
        const BRepGraph_TopoNode::EdgeDef& anOldEdge  = theGraph.Defs().Edge(anOldIdx);
        const bool isReversed = (aCanonEdge.StartVertexDefId == anOldEdge.EndVertexDefId
                                 && aCanonEdge.EndVertexDefId == anOldEdge.StartVertexDefId);

        // Replace in wires.
        const NCollection_Vector<int>& aWires = theGraph.RelEdges().WiresOfEdge(anOldIdx);
        for (int aWireIter = 0; aWireIter < aWires.Length(); ++aWireIter)
        {
          theGraph.Mut().ReplaceEdgeInWire(aWires.Value(aWireIter), anOldId, aCanonId, isReversed);
        }

        // Transfer PCurves (skip duplicates).
        for (int aPCIdx = 0; aPCIdx < anOldEdge.PCurves.Length(); ++aPCIdx)
        {
          const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
            anOldEdge.PCurves.Value(aPCIdx);
          if (aPCEntry.Curve2d.IsNull())
          {
            continue;
          }

          // Check if canonical edge already has a PCurve for this face+orientation.
          bool aAlreadyHas = false;
          for (int aCanonPCIdx = 0; aCanonPCIdx < aCanonEdge.PCurves.Length(); ++aCanonPCIdx)
          {
            const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aCanonPC =
              aCanonEdge.PCurves.Value(aCanonPCIdx);
            if (aCanonPC.FaceDefId == aPCEntry.FaceDefId
                && aCanonPC.EdgeOrientation == aPCEntry.EdgeOrientation)
            {
              aAlreadyHas = true;
              break;
            }
          }

          if (!aAlreadyHas)
          {
            theGraph.Mut().AddPCurveToEdge(aCanonId,
                                           aPCEntry.FaceDefId,
                                           aPCEntry.Curve2d,
                                           aPCEntry.ParamFirst,
                                           aPCEntry.ParamLast,
                                           aPCEntry.EdgeOrientation);
          }
        }

        // Transfer usages.
        BRepGraph_TopoNode::EdgeDef& aCanonMut = theGraph.Mut().EdgeDef(aCanonicalIdx);
        for (int anUsIdx = 0; anUsIdx < anOldEdge.Usages.Length(); ++anUsIdx)
          aCanonMut.Usages.Append(anOldEdge.Usages.Value(anUsIdx));

        theGraph.Builder().RemoveNode(anOldId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeEdge"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedEdges;
      }
    }
    else
    {
      aResult.NbMergedEdges = aCanonicalEdge.Size();
    }
  }

  // Phase 3: Wire Merging.
  {
    // Hash wire by its ordered edge sequence (EdgeDefId.Index, Orientation).
    struct WireHash
    {
      size_t operator()(int theWireIdx, const BRepGraph& theGraph) const
      {
        const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(theWireIdx);
        size_t aHash = 0;
        for (int anIdx = 0; anIdx < aWire.OrderedEdges.Length(); ++anIdx)
        {
          const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
            aWire.OrderedEdges.Value(anIdx);
          size_t aEntryHash[2];
          aEntryHash[0] = opencascade::hash(anEntry.EdgeDefId.Index);
          aEntryHash[1] = opencascade::hash(static_cast<int>(anEntry.OrientationInWire));
          aHash ^= opencascade::hashBytes(aEntryHash, sizeof(aEntryHash)) + 0x9e3779b9 + (aHash << 6) + (aHash >> 2);
        }
        return aHash;
      }
    };

    auto wiresEqual = [&](int theA, int theB) -> bool
    {
      const BRepGraph_TopoNode::WireDef& aWireA = theGraph.Defs().Wire(theA);
      const BRepGraph_TopoNode::WireDef& aWireB = theGraph.Defs().Wire(theB);
      if (aWireA.OrderedEdges.Length() != aWireB.OrderedEdges.Length())
        return false;
      for (int anIdx = 0; anIdx < aWireA.OrderedEdges.Length(); ++anIdx)
      {
        const BRepGraph_TopoNode::WireDef::EdgeEntry& anA = aWireA.OrderedEdges.Value(anIdx);
        const BRepGraph_TopoNode::WireDef::EdgeEntry& aB  = aWireB.OrderedEdges.Value(anIdx);
        if (anA.EdgeDefId != aB.EdgeDefId || anA.OrientationInWire != aB.OrientationInWire)
          return false;
      }
      return true;
    };

    NCollection_DataMap<size_t, NCollection_Vector<int>> aWireHashBuckets(
      std::max(1, theGraph.Defs().NbWires()),
      aTmpAlloc);

    WireHash aHasher;
    for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
    {
      const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
      if (aWire.IsRemoved)
        continue;
      const size_t aH = aHasher(aWireIdx, theGraph);
      aWireHashBuckets.TryBind(aH, NCollection_Vector<int>());
      aWireHashBuckets.ChangeFind(aH).Append(aWireIdx);
    }

    NCollection_DataMap<int, int> aCanonicalWire(
      std::max(1, theGraph.Defs().NbWires()),
      aTmpAlloc);

    for (NCollection_DataMap<size_t, NCollection_Vector<int>>::Iterator
           aBucketIter(aWireHashBuckets);
         aBucketIter.More(); aBucketIter.Next())
    {
      const NCollection_Vector<int>& aBucket = aBucketIter.Value();
      for (int aBaseIdx = 0; aBaseIdx < aBucket.Length(); ++aBaseIdx)
      {
        const int aBaseWireIdx = aBucket.Value(aBaseIdx);
        if (aCanonicalWire.IsBound(aBaseWireIdx))
          continue;

        for (int aCandIdx = aBaseIdx + 1; aCandIdx < aBucket.Length(); ++aCandIdx)
        {
          const int aCandWireIdx = aBucket.Value(aCandIdx);
          if (aCanonicalWire.IsBound(aCandWireIdx))
            continue;
          if (wiresEqual(aBaseWireIdx, aCandWireIdx))
            aCanonicalWire.Bind(aCandWireIdx, aBaseWireIdx);
        }
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Redirect face usage wire references.
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalWire); anIt.More(); anIt.Next())
      {
        const int anOldIdx      = anIt.Key();
        const int aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId  = BRepGraph_NodeId::Wire(anOldIdx);
        const BRepGraph_NodeId aCanonId = BRepGraph_NodeId::Wire(aCanonicalIdx);

        // Update face usages referencing the old wire.
        for (int aFaceUsIdx = 0; aFaceUsIdx < theGraph.Usages().NbFaces(); ++aFaceUsIdx)
        {
          // We cannot mutate usages through public API easily; wire merge
          // is only effective when the graph is later compacted.
          // For now, just transfer usages and mark removed.
        }

        BRepGraph_TopoNode::WireDef& aCanonWire = theGraph.Mut().WireDef(aCanonicalIdx);
        const BRepGraph_TopoNode::WireDef& anOldWire = theGraph.Defs().Wire(anOldIdx);
        for (int anUsIdx = 0; anUsIdx < anOldWire.Usages.Length(); ++anUsIdx)
          aCanonWire.Usages.Append(anOldWire.Usages.Value(anUsIdx));

        theGraph.Builder().RemoveNode(anOldId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeWire"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedWires;
      }
    }
    else
    {
      aResult.NbMergedWires = aCanonicalWire.Size();
    }
  }

  // Phase 4: Face Merging.
  {
    struct FaceKey
    {
      int    SurfIdx;
      size_t WireHash;

      bool operator==(const FaceKey& theOther) const
      {
        return SurfIdx == theOther.SurfIdx && WireHash == theOther.WireHash;
      }
    };

    struct FaceKeyHasher
    {
      size_t operator()(const FaceKey& theKey) const noexcept
      {
        size_t aCombination[2];
        aCombination[0] = opencascade::hash(theKey.SurfIdx);
        aCombination[1] = theKey.WireHash;
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }

      bool operator()(const FaceKey& theA, const FaceKey& theB) const
      {
        return theA == theB;
      }
    };

    auto faceWireHash = [&](int theFaceIdx) -> size_t
    {
      // Collect wire def ids used by this face (via first usage).
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(theFaceIdx);
      if (aFace.Usages.IsEmpty())
        return 0;

      const BRepGraph_UsageId& aFirstUsage = aFace.Usages.Value(0);
      if (!aFirstUsage.IsValid() || aFirstUsage.Index >= theGraph.Usages().NbFaces())
        return 0;

      const BRepGraph_TopoNode::FaceUsage& aFaceUs = theGraph.Usages().Face(aFirstUsage.Index);
      size_t aHash = 0;

      if (aFaceUs.OuterWireUsage.IsValid()
          && aFaceUs.OuterWireUsage.Index < theGraph.Usages().NbWires())
      {
        const BRepGraph_TopoNode::WireUsage& aWireUs =
          theGraph.Usages().Wire(aFaceUs.OuterWireUsage.Index);
        aHash ^= opencascade::hash(aWireUs.DefId.Index);
      }

      for (int anIdx = 0; anIdx < aFaceUs.InnerWireUsages.Length(); ++anIdx)
      {
        const BRepGraph_UsageId& aInnerId = aFaceUs.InnerWireUsages.Value(anIdx);
        if (aInnerId.IsValid() && aInnerId.Index < theGraph.Usages().NbWires())
        {
          const BRepGraph_TopoNode::WireUsage& aInnerWireUs =
            theGraph.Usages().Wire(aInnerId.Index);
          aHash ^= opencascade::hash(aInnerWireUs.DefId.Index) + 0x9e3779b9;
        }
      }
      return aHash;
    };

    NCollection_DataMap<FaceKey, NCollection_Vector<int>, FaceKeyHasher> aFaceGroups(
      std::max(1, theGraph.Defs().NbFaces()),
      aTmpAlloc);

    for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
    {
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceIdx);
      if (aFace.IsRemoved || !aFace.SurfNodeId.IsValid())
        continue;

      FaceKey aKey;
      aKey.SurfIdx  = aFace.SurfNodeId.Index;
      aKey.WireHash = faceWireHash(aFaceIdx);

      aFaceGroups.TryBind(aKey, NCollection_Vector<int>());
      aFaceGroups.ChangeFind(aKey).Append(aFaceIdx);
    }

    NCollection_DataMap<int, int> aCanonicalFace(
      std::max(1, theGraph.Defs().NbFaces()),
      aTmpAlloc);

    for (NCollection_DataMap<FaceKey, NCollection_Vector<int>, FaceKeyHasher>::Iterator
           aGroupIter(aFaceGroups);
         aGroupIter.More(); aGroupIter.Next())
    {
      const NCollection_Vector<int>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const int aCanonIdx = aGroup.Value(0);
      const BRepGraph_TopoNode::FaceDef& aCanonFace = theGraph.Defs().Face(aCanonIdx);

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const int aCandIdx = aGroup.Value(aCandIter);
        const BRepGraph_TopoNode::FaceDef& aCandFace = theGraph.Defs().Face(aCandIdx);

        // Check tolerance compatibility.
        if (aCandFace.Tolerance > aCanonFace.Tolerance * 10.0)
          continue;

        aCanonicalFace.Bind(aCandIdx, aCanonIdx);
      }
    }

    if (!theOptions.AnalyzeOnly)
    {
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalFace); anIt.More(); anIt.Next())
      {
        const int anOldIdx      = anIt.Key();
        const int aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId  = BRepGraph_NodeId::Face(anOldIdx);
        const BRepGraph_NodeId aCanonId = BRepGraph_NodeId::Face(aCanonicalIdx);

        BRepGraph_TopoNode::FaceDef& aCanonMut = theGraph.Mut().FaceDef(aCanonicalIdx);
        const BRepGraph_TopoNode::FaceDef& anOldFace = theGraph.Defs().Face(anOldIdx);
        for (int anUsIdx = 0; anUsIdx < anOldFace.Usages.Length(); ++anUsIdx)
          aCanonMut.Usages.Append(anOldFace.Usages.Value(anUsIdx));

        theGraph.Builder().RemoveNode(anOldId);

        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(aCanonId);
        theGraph.History().Record(TCollection_AsciiString("Dedup:MergeFace"), anOldId, aRepl);
        ++aResult.NbHistoryRecords;
        ++aResult.NbMergedFaces;
      }
    }
    else
    {
      aResult.NbMergedFaces = aCanonicalFace.Size();
    }
  }

  aResult.IsDefMergeApplied = !theOptions.AnalyzeOnly
                              && (aResult.NbMergedVertices > 0
                                  || aResult.NbMergedEdges > 0
                                  || aResult.NbMergedWires > 0
                                  || aResult.NbMergedFaces > 0);

  theGraph.SetHistoryEnabled(wasHistoryEnabled);
  return aResult;
}
