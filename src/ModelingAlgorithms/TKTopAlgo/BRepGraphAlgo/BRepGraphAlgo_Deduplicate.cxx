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
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

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
  NCollection_DataMap<int, int> aSurfRewriteMap(std::max(1, theGraph.Defs().NbFaces()), aTmpAlloc);

  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_FaceId aFaceId(aFaceIdx);
    if (!BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
    {
      continue;
    }

    const occ::handle<Geom_Surface>& aFaceSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
    const int*                       aCanonFaceIdx = aSurfToCanonicalFace.Seek(aFaceSurf);
    if (aCanonFaceIdx == nullptr)
    {
      aSurfToCanonicalFace.Bind(aFaceSurf, aFaceIdx);
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
  NCollection_DataMap<int, int> aCurveRewriteMap(std::max(1, theGraph.Defs().NbEdges()), aTmpAlloc);

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anEdgeIdx);
    if (!BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      continue;
    }

    const occ::handle<Geom_Curve>& anEdgeCurve   = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
    const int*                     aCanonEdgeIdx = aCurveToCanonicalEdge.Seek(anEdgeCurve);
    if (aCanonEdgeIdx == nullptr)
    {
      aCurveToCanonicalEdge.Bind(anEdgeCurve, anEdgeIdx);
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
      const int                                     aFaceIdx      = anIt.Key();
      const int                                     aCanonFaceIdx = anIt.Value();
      BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aFaceDef = theGraph.MutFace(BRepGraph_FaceId(aFaceIdx));
      const auto aCanonSurfRepId = theGraph.Defs().Face(BRepGraph_FaceId(aCanonFaceIdx)).SurfaceRepId;
      aFaceDef->SurfaceRepId     = aCanonSurfRepId;
      ++aResult.NbSurfaceRewrites;
      aResult.AffectedFaceDefs.Append(aFaceDef->Id);

      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(BRepGraph_NodeId::Face(aCanonFaceIdx));
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                                aFaceDef->Id,
                                aRepl);
      ++aResult.NbHistoryRecords;
    }

    // Rewrite edge curves: replace duplicate curve handles with canonical ones.
    for (NCollection_DataMap<int, int>::Iterator anIt(aCurveRewriteMap); anIt.More(); anIt.Next())
    {
      const int                                     anEdgeIdx     = anIt.Key();
      const int                                     aCanonEdgeIdx = anIt.Value();
      BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdgeDef = theGraph.MutEdge(BRepGraph_EdgeId(anEdgeIdx));
      const auto aCanonCurveRepId = theGraph.Defs().Edge(BRepGraph_EdgeId(aCanonEdgeIdx)).Curve3DRepId;
      anEdgeDef->Curve3DRepId     = aCanonCurveRepId;
      ++aResult.NbCurveRewrites;
      aResult.AffectedEdgeDefs.Append(anEdgeDef->Id);

      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(BRepGraph_NodeId::Edge(aCanonEdgeIdx));
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                                anEdgeDef->Id,
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

  // Phase 1: Vertex Merging via KDTree range search.
  {
    const double aTol = theOptions.HashTolerance;

    // Collect active vertices: (point, graph index) pairs.
    const int                                  aNbVertices = theGraph.Defs().NbVertices();
    NCollection_Vector<std::pair<gp_Pnt, int>> aActiveVertices(256, aTmpAlloc);
    for (int aVtxIdx = 0; aVtxIdx < aNbVertices; ++aVtxIdx)
    {
      const BRepGraph_TopoNode::VertexDef& aVtx = theGraph.Defs().Vertex(BRepGraph_VertexId(aVtxIdx));
      if (aVtx.IsRemoved)
        continue;
      aActiveVertices.Append(
        std::make_pair(BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aVtxIdx)), aVtxIdx));
    }

    // Build KDTree from active vertex points - O(n log n).
    const int                  aNbActive = aActiveVertices.Length();
    NCollection_Array1<gp_Pnt> aPointsArr(0, std::max(0, aNbActive - 1));
    for (int i = 0; i < aNbActive; ++i)
      aPointsArr.SetValue(i, aActiveVertices.Value(i).first);

    NCollection_KDTree<gp_Pnt, 3> aTree;
    if (!aPointsArr.IsEmpty())
      aTree.Build(aPointsArr);

    // Canonical vertex map: old graph index -> canonical graph index.
    NCollection_DataMap<int, int> aCanonicalVertex(std::max(1, aNbVertices), aTmpAlloc);

    for (int aLocalIdx = 0; aLocalIdx < aNbActive; ++aLocalIdx)
    {
      const int aBaseVtxIdx = aActiveVertices.Value(aLocalIdx).second;
      if (aCanonicalVertex.IsBound(aBaseVtxIdx))
        continue;

      const gp_Pnt aBaseVtxPnt = BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aBaseVtxIdx));
      const double aBaseVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aBaseVtxIdx));

      aTree.ForEachInRange(aBaseVtxPnt, aTol, [&](size_t theResultIdx) {
        const int anArrayIdx = static_cast<int>(theResultIdx) - 1;
        if (anArrayIdx <= aLocalIdx)
          return; // skip self and already-processed

        const int aCandVtxIdx = aActiveVertices.Value(anArrayIdx).second;
        if (aCanonicalVertex.IsBound(aCandVtxIdx))
          return;

        const double aCandVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aCandVtxIdx));
        const double aMaxTol     = std::max(aBaseVtxTol, aCandVtxTol);
        if (aBaseVtxPnt.Distance(BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aCandVtxIdx))) <= aMaxTol)
        {
          aCanonicalVertex.Bind(aCandVtxIdx, aBaseVtxIdx);
        }
      });
    }

    if (!theOptions.AnalyzeOnly)
    {
      // Redirect edge vertex references and mark non-canonical vertices as removed.
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalVertex); anIt.More(); anIt.Next())
      {
        const int              anOldIdx      = anIt.Key();
        const int              aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId       = BRepGraph_NodeId::Vertex(anOldIdx);
        const BRepGraph_NodeId aCanonId      = BRepGraph_NodeId::Vertex(aCanonicalIdx);

        // Update edges referencing the old vertex.
        for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
        {
          BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge = theGraph.MutEdge(BRepGraph_EdgeId(anEdgeIdx));
          if (anEdge->IsRemoved)
            continue;
          if (anEdge->StartVertexDefId() == anOldId)
            anEdge->StartVertex.VertexDefId = BRepGraph_VertexId(aCanonId.Index);
          if (anEdge->EndVertexDefId() == anOldId)
            anEdge->EndVertex.VertexDefId = BRepGraph_VertexId(aCanonId.Index);
        }

        // Mark non-canonical as removed.
        theGraph.Builder().RemoveNode(anOldId, aCanonId);

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
      int               StartVtx;
      int               EndVtx;

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

    NCollection_DataMap<EdgeKey, NCollection_Vector<int>, EdgeKeyHasher> anEdgeGroups(
      std::max(1, theGraph.Defs().NbEdges()),
      aTmpAlloc);

    for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
    {
      const BRepGraph_EdgeId              anEdgeId(anEdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeId);
      if (anEdge.IsRemoved || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
        continue;

      // Use canonical (forward) key: use raw pointer as a stable identity.
      EdgeKey aKey;
      aKey.CurvePtr = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId).get();
      aKey.StartVtx =
        anEdge.StartVertex.VertexDefId.IsValid() ? anEdge.StartVertex.VertexDefId.Index : -1;
      aKey.EndVtx = anEdge.EndVertex.VertexDefId.IsValid() ? anEdge.EndVertex.VertexDefId.Index : -1;

      // Normalize: always use min vertex index first for undirected matching.
      if (aKey.StartVtx > aKey.EndVtx)
        std::swap(aKey.StartVtx, aKey.EndVtx);

      anEdgeGroups.TryBind(aKey, NCollection_Vector<int>());
      anEdgeGroups.ChangeFind(aKey).Append(anEdgeIdx);
    }

    NCollection_DataMap<int, int> aCanonicalEdge(std::max(1, theGraph.Defs().NbEdges()), aTmpAlloc);

    for (NCollection_DataMap<EdgeKey, NCollection_Vector<int>, EdgeKeyHasher>::Iterator aGroupIter(
           anEdgeGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const NCollection_Vector<int>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const int                          aCanonIdx  = aGroup.Value(0);
      const BRepGraph_TopoNode::EdgeDef& aCanonEdge = theGraph.Defs().Edge(BRepGraph_EdgeId(aCanonIdx));

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const int                          aCandIdx  = aGroup.Value(aCandIter);
        const BRepGraph_TopoNode::EdgeDef& aCandEdge = theGraph.Defs().Edge(BRepGraph_EdgeId(aCandIdx));

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
        const int              anOldIdx      = anIt.Key();
        const int              aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId       = BRepGraph_NodeId::Edge(anOldIdx);
        const BRepGraph_NodeId aCanonId      = BRepGraph_NodeId::Edge(aCanonicalIdx);

        // Determine if the non-canonical edge is reversed relative to canonical.
        const BRepGraph_TopoNode::EdgeDef& aCanonEdge = theGraph.Defs().Edge(BRepGraph_EdgeId(aCanonicalIdx));
        const BRepGraph_TopoNode::EdgeDef& anOldEdge  = theGraph.Defs().Edge(BRepGraph_EdgeId(anOldIdx));
        const bool isReversed = (aCanonEdge.StartVertexDefId() == anOldEdge.EndVertexDefId()
                                 && aCanonEdge.EndVertexDefId() == anOldEdge.StartVertexDefId());

        // Replace in wires.
        const NCollection_Vector<BRepGraph_WireId>& aWires = theGraph.Defs().WiresOfEdge(BRepGraph_EdgeId(anOldIdx));
        for (int aWireIter = 0; aWireIter < aWires.Length(); ++aWireIter)
        {
          BRepGraph_Mutator::ReplaceEdgeInWire(theGraph, aWires.Value(aWireIter).Index, anOldId, aCanonId, isReversed);
        }

        // Transfer PCurves via CoEdges (skip duplicates).
        // When the old edge is reversed relative to canonical, invert orientation
        // so duplicate detection matches correctly against the canonical's CoEdges.
        const NCollection_Vector<BRepGraph_CoEdgeId>& aOldCoEdges = theGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(anOldIdx));
        const NCollection_Vector<BRepGraph_CoEdgeId>& aCanonCoEdges =
          theGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(aCanonId.Index));
        for (int aCEIdx = 0; aCEIdx < aOldCoEdges.Length(); ++aCEIdx)
        {
          const BRepGraphInc::CoEdgeEntity& aOldCE =
            theGraph.Defs().CoEdge(aOldCoEdges.Value(aCEIdx));
          if (!aOldCE.Curve2DRepId.IsValid())
            continue;

          TopAbs_Orientation aTransferOri = aOldCE.Sense;
          if (isReversed)
          {
            if (aTransferOri == TopAbs_FORWARD)
              aTransferOri = TopAbs_REVERSED;
            else if (aTransferOri == TopAbs_REVERSED)
              aTransferOri = TopAbs_FORWARD;
          }

          // Check if canonical edge already has a CoEdge for this face+orientation.
          bool aAlreadyHas = false;
          for (int aCanonCEIdx = 0; aCanonCEIdx < aCanonCoEdges.Length(); ++aCanonCEIdx)
          {
            const BRepGraphInc::CoEdgeEntity& aCanonCE =
              theGraph.Defs().CoEdge(aCanonCoEdges.Value(aCanonCEIdx));
            if (aCanonCE.FaceDefId == aOldCE.FaceDefId && aCanonCE.Sense == aTransferOri)
            {
              aAlreadyHas = true;
              break;
            }
          }

          if (!aAlreadyHas)
          {
            const occ::handle<Geom2d_Curve>& aOldPCurve =
              BRepGraph_Tool::CoEdge::PCurve(theGraph, aOldCoEdges.Value(aCEIdx));
            theGraph.Builder().AddPCurveToEdge(aCanonId,
                                               aOldCE.FaceDefId,
                                               aOldPCurve,
                                               aOldCE.ParamFirst,
                                               aOldCE.ParamLast,
                                               aTransferOri);
          }
        }

        theGraph.Builder().RemoveNode(anOldId, aCanonId);

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
    // Hash wire by its ordered coedge sequence (edge index + sense from CoEdgeEntity).
    struct WireHash
    {
      size_t operator()(int theWireIdx, const BRepGraph& theGraph) const
      {
        const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(BRepGraph_WireId(theWireIdx));
        size_t                             aHash = 0;
        for (int anIdx = 0; anIdx < aWire.CoEdgeRefs.Length(); ++anIdx)
        {
          const BRepGraphInc::CoEdgeRef&       aCR     = aWire.CoEdgeRefs.Value(anIdx);
          const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeDefId);
          size_t                               aEntryHash[2];
          aEntryHash[0] = opencascade::hash(aCoEdge.EdgeDefId);
          aEntryHash[1] = opencascade::hash(static_cast<int>(aCoEdge.Sense));
          aHash ^= opencascade::hashBytes(aEntryHash, sizeof(aEntryHash)) + 0x9e3779b9
                   + (aHash << 6) + (aHash >> 2);
        }
        return aHash;
      }
    };

    auto wiresEqual = [&](int theA, int theB) -> bool {
      const BRepGraph_TopoNode::WireDef& aWireA = theGraph.Defs().Wire(BRepGraph_WireId(theA));
      const BRepGraph_TopoNode::WireDef& aWireB = theGraph.Defs().Wire(BRepGraph_WireId(theB));
      if (aWireA.CoEdgeRefs.Length() != aWireB.CoEdgeRefs.Length())
        return false;
      for (int anIdx = 0; anIdx < aWireA.CoEdgeRefs.Length(); ++anIdx)
      {
        const BRepGraphInc::CoEdgeRef&       aCRA     = aWireA.CoEdgeRefs.Value(anIdx);
        const BRepGraphInc::CoEdgeRef&       aCRB     = aWireB.CoEdgeRefs.Value(anIdx);
        const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeA = theGraph.Defs().CoEdge(aCRA.CoEdgeDefId);
        const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeB = theGraph.Defs().CoEdge(aCRB.CoEdgeDefId);
        if (aCoEdgeA.EdgeDefId != aCoEdgeB.EdgeDefId || aCoEdgeA.Sense != aCoEdgeB.Sense)
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
      const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(BRepGraph_WireId(aWireIdx));
      if (aWire.IsRemoved)
        continue;
      const size_t aH = aHasher(aWireIdx, theGraph);
      aWireHashBuckets.TryBind(aH, NCollection_Vector<int>());
      aWireHashBuckets.ChangeFind(aH).Append(aWireIdx);
    }

    NCollection_DataMap<int, int> aCanonicalWire(std::max(1, theGraph.Defs().NbWires()), aTmpAlloc);

    for (NCollection_DataMap<size_t, NCollection_Vector<int>>::Iterator aBucketIter(
           aWireHashBuckets);
         aBucketIter.More();
         aBucketIter.Next())
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
      // Mark non-canonical wires as removed.
      for (NCollection_DataMap<int, int>::Iterator anIt(aCanonicalWire); anIt.More(); anIt.Next())
      {
        const int              anOldIdx      = anIt.Key();
        const int              aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId       = BRepGraph_NodeId::Wire(anOldIdx);
        const BRepGraph_NodeId aCanonId      = BRepGraph_NodeId::Wire(aCanonicalIdx);

        theGraph.Builder().RemoveNode(anOldId, aCanonId);

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

      bool operator()(const FaceKey& theA, const FaceKey& theB) const { return theA == theB; }
    };

    auto faceWireHash = [&](int theFaceIdx) -> size_t {
      // Collect wire def ids used by this face (via incidence refs).
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(BRepGraph_FaceId(theFaceIdx));
      size_t                             aHash = 0;

      for (int anIdx = 0; anIdx < aFace.WireRefs.Length(); ++anIdx)
      {
        const BRepGraphInc::WireRef& aWR = aFace.WireRefs.Value(anIdx);
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

    NCollection_DataMap<FaceKey, NCollection_Vector<int>, FaceKeyHasher> aFaceGroups(
      std::max(1, theGraph.Defs().NbFaces()),
      aTmpAlloc);

    for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
    {
      const BRepGraph_FaceId              aFaceId(aFaceIdx);
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceId);
      if (aFace.IsRemoved || !BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
        continue;

      FaceKey aKey;
      aKey.SurfPtr  = BRepGraph_Tool::Face::Surface(theGraph, aFaceId).get();
      aKey.WireHash = faceWireHash(aFaceIdx);

      aFaceGroups.TryBind(aKey, NCollection_Vector<int>());
      aFaceGroups.ChangeFind(aKey).Append(aFaceIdx);
    }

    NCollection_DataMap<int, int> aCanonicalFace(std::max(1, theGraph.Defs().NbFaces()), aTmpAlloc);

    for (NCollection_DataMap<FaceKey, NCollection_Vector<int>, FaceKeyHasher>::Iterator aGroupIter(
           aFaceGroups);
         aGroupIter.More();
         aGroupIter.Next())
    {
      const NCollection_Vector<int>& aGroup = aGroupIter.Value();
      if (aGroup.Length() < 2)
        continue;

      const int                          aCanonIdx  = aGroup.Value(0);
      const BRepGraph_TopoNode::FaceDef& aCanonFace = theGraph.Defs().Face(BRepGraph_FaceId(aCanonIdx));

      for (int aCandIter = 1; aCandIter < aGroup.Length(); ++aCandIter)
      {
        const int                          aCandIdx  = aGroup.Value(aCandIter);
        const BRepGraph_TopoNode::FaceDef& aCandFace = theGraph.Defs().Face(BRepGraph_FaceId(aCandIdx));

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
        const int              anOldIdx      = anIt.Key();
        const int              aCanonicalIdx = anIt.Value();
        const BRepGraph_NodeId anOldId       = BRepGraph_NodeId::Face(anOldIdx);
        const BRepGraph_NodeId aCanonId      = BRepGraph_NodeId::Face(aCanonicalIdx);

        theGraph.Builder().RemoveNode(anOldId, aCanonId);

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
                              && (aResult.NbMergedVertices > 0 || aResult.NbMergedEdges > 0
                                  || aResult.NbMergedWires > 0 || aResult.NbMergedFaces > 0);

  BRepGraph_Mutator::CommitMutation(theGraph);
  theGraph.SetHistoryEnabled(wasHistoryEnabled);
  return aResult;
}
