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
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <GeomHash_CurveHasher.hxx>
#include <GeomHash_SurfaceHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_HashUtils.hxx>

#include <algorithm>
#include <cmath>
#include <cstdint>

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

  // Deduplicate surfaces by comparing Handle pointers on FaceDefs.
  // Map: surface handle -> canonical face index (first face that owns it).
  NCollection_DataMap<occ::handle<Geom_Surface>, int, GeomHash_SurfaceHasher> aSurfToCanonicalFace(
    aSurfHasher,
    std::max(1, theGraph.Defs().NbFaces() * 2),
    aTmpAlloc);
  // Map: face index -> canonical face index (for faces whose surface should be replaced).
  NCollection_DataMap<int, int> aSurfRewriteMap(std::max(1, theGraph.Defs().NbFaces()),
                                                aTmpAlloc);

  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(aFaceIdx);
    if (aFaceDef.Surface.IsNull())
    {
      continue;
    }

    const int* aCanonFaceIdx = aSurfToCanonicalFace.Seek(aFaceDef.Surface);
    if (aCanonFaceIdx == nullptr)
    {
      aSurfToCanonicalFace.Bind(aFaceDef.Surface, aFaceIdx);
    }
    else if (*aCanonFaceIdx != aFaceIdx)
    {
      aSurfRewriteMap.Bind(aFaceIdx, *aCanonFaceIdx);
    }
  }

  // Deduplicate curves by comparing Handle pointers on EdgeDefs.
  NCollection_DataMap<occ::handle<Geom_Curve>, int, GeomHash_CurveHasher> aCurveToCanonicalEdge(
    aCurveHasher,
    std::max(1, theGraph.Defs().NbEdges() * 2),
    aTmpAlloc);
  NCollection_DataMap<int, int> aCurveRewriteMap(std::max(1, theGraph.Defs().NbEdges()),
                                                 aTmpAlloc);

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.Curve3d.IsNull())
    {
      continue;
    }

    const int* aCanonEdgeIdx = aCurveToCanonicalEdge.Seek(anEdgeDef.Curve3d);
    if (aCanonEdgeIdx == nullptr)
    {
      aCurveToCanonicalEdge.Bind(anEdgeDef.Curve3d, anEdgeIdx);
    }
    else if (*aCanonEdgeIdx != anEdgeIdx)
    {
      aCurveRewriteMap.Bind(anEdgeIdx, *aCanonEdgeIdx);
    }
  }

  aResult.NbCanonicalSurfaces = theGraph.Defs().NbFaces() - aSurfRewriteMap.Size();
  aResult.NbCanonicalCurves   = theGraph.Defs().NbEdges() - aCurveRewriteMap.Size();

  if (theOptions.AnalyzeOnly && !theOptions.MergeDefsWhenSafe)
  {
    theGraph.SetHistoryEnabled(wasHistoryEnabled);
    return aResult;
  }

  if (!theOptions.AnalyzeOnly)
  {
  // Rewrite face surfaces: replace duplicate surface handles with canonical ones.
  for (NCollection_DataMap<int, int>::Iterator anIt(aSurfRewriteMap); anIt.More(); anIt.Next())
  {
    const int aFaceIdx       = anIt.Key();
    const int aCanonFaceIdx  = anIt.Value();
    BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Mut().FaceDef(aFaceIdx);
    const Handle(Geom_Surface)& aCanonSurf = theGraph.Defs().Face(aCanonFaceIdx).Surface;
    aFaceDef.Surface = aCanonSurf;
    ++aResult.NbSurfaceRewrites;
    aResult.AffectedFaceDefs.Append(aFaceDef.Id);

    NCollection_Vector<BRepGraph_NodeId> aRepl;
    aRepl.Append(BRepGraph_NodeId::Face(aCanonFaceIdx));
    theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                              aFaceDef.Id,
                              aRepl);
    ++aResult.NbHistoryRecords;
  }

  // Rewrite edge curves: replace duplicate curve handles with canonical ones.
  for (NCollection_DataMap<int, int>::Iterator anIt(aCurveRewriteMap); anIt.More(); anIt.Next())
  {
    const int anEdgeIdx      = anIt.Key();
    const int aCanonEdgeIdx  = anIt.Value();
    BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Mut().EdgeDef(anEdgeIdx);
    const Handle(Geom_Curve)& aCanonCurve = theGraph.Defs().Edge(aCanonEdgeIdx).Curve3d;
    anEdgeDef.Curve3d = aCanonCurve;
    ++aResult.NbCurveRewrites;
    aResult.AffectedEdgeDefs.Append(anEdgeDef.Id);

    NCollection_Vector<BRepGraph_NodeId> aRepl;
    aRepl.Append(BRepGraph_NodeId::Edge(aCanonEdgeIdx));
    theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                              anEdgeDef.Id,
                              aRepl);
    ++aResult.NbHistoryRecords;
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
    // Key: (canonical Curve3d pointer, canonical StartVertexDefId, canonical EndVertexDefId).
    struct EdgeKey
    {
      const Geom_Curve* CurvePtr;
      int StartVtx;
      int EndVtx;

      bool operator==(const EdgeKey& theOther) const
      {
        return CurvePtr == theOther.CurvePtr
            && StartVtx == theOther.StartVtx
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
      if (anEdge.IsRemoved || anEdge.Curve3d.IsNull())
        continue;

      // Use canonical (forward) key: use raw pointer as a stable identity.
      EdgeKey aKey;
      aKey.CurvePtr = anEdge.Curve3d.get();
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
    // Hash wire by its ordered edge sequence (EdgeRef EdgeIdx, Orientation).
    struct WireHash
    {
      size_t operator()(int theWireIdx, const BRepGraph& theGraph) const
      {
        const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(theWireIdx);
        size_t aHash = 0;
        for (int anIdx = 0; anIdx < aWire.EdgeRefs.Length(); ++anIdx)
        {
          const BRepGraphInc::EdgeRef& aER = aWire.EdgeRefs.Value(anIdx);
          size_t aEntryHash[2];
          aEntryHash[0] = opencascade::hash(aER.EdgeIdx);
          aEntryHash[1] = opencascade::hash(static_cast<int>(aER.Orientation));
          aHash ^= opencascade::hashBytes(aEntryHash, sizeof(aEntryHash)) + 0x9e3779b9 + (aHash << 6) + (aHash >> 2);
        }
        return aHash;
      }
    };

    auto wiresEqual = [&](int theA, int theB) -> bool
    {
      const BRepGraph_TopoNode::WireDef& aWireA = theGraph.Defs().Wire(theA);
      const BRepGraph_TopoNode::WireDef& aWireB = theGraph.Defs().Wire(theB);
      if (aWireA.EdgeRefs.Length() != aWireB.EdgeRefs.Length())
        return false;
      for (int anIdx = 0; anIdx < aWireA.EdgeRefs.Length(); ++anIdx)
      {
        const BRepGraphInc::EdgeRef& anA = aWireA.EdgeRefs.Value(anIdx);
        const BRepGraphInc::EdgeRef& aB = aWireB.EdgeRefs.Value(anIdx);
        if (anA.EdgeIdx != aB.EdgeIdx || anA.Orientation != aB.Orientation)
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

      bool operator()(const FaceKey& theA, const FaceKey& theB) const
      {
        return theA == theB;
      }
    };

    auto faceWireHash = [&](int theFaceIdx) -> size_t
    {
      // Collect wire def ids used by this face (via incidence refs).
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(theFaceIdx);
      size_t aHash = 0;

      for (int anIdx = 0; anIdx < aFace.WireRefs.Length(); ++anIdx)
      {
        const BRepGraphInc::WireRef& aWR = aFace.WireRefs.Value(anIdx);
        if (aWR.IsOuter)
        {
          aHash ^= opencascade::hash(aWR.WireIdx);
        }
        else
        {
          aHash ^= opencascade::hash(aWR.WireIdx) + 0x9e3779b9;
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
      if (aFace.IsRemoved || aFace.Surface.IsNull())
        continue;

      FaceKey aKey;
      aKey.SurfPtr  = aFace.Surface.get();
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
