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

#include <BRepGraphAlgo_Sewing.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepGraphAlgo_SameParameter.hxx>
#include <BRepLib.hxx>
#include <ExtremaPC_Curve.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_KDTree.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_OrderedDataMap.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <algorithm>
#include <utility>

namespace
{
constexpr int    THE_NB_EDGE_MATCH_SAMPLES = 5;   // sample count for edge geometric matching
constexpr double THE_MAX_CHORD_RATIO       = 2.0; // maximum chord-length ratio for sewable edges
constexpr double THE_HIGH_CONFIDENCE_RATIO =
  0.01;                                       // forward-distance threshold to skip reverse pass
constexpr int THE_INIT_VECTOR_CAPACITY = 256; // initial capacity for scratch vectors

// ---------------------------------------------------------------------------
// Union-Find with path compression: O(n * alpha(n)) ~ O(n).
// ---------------------------------------------------------------------------

//! Find root of theIdx with path compression.
int findRoot(NCollection_OrderedDataMap<int, int>& theParent, int theIdx)
{
  int aRoot = theIdx;
  while (const int* aP = theParent.Seek(aRoot))
  {
    aRoot = *aP;
  }
  // Path compression: flatten chain from theIdx to aRoot.
  int aCur = theIdx;
  while (const int* aP = theParent.Seek(aCur))
  {
    const int aNext = *aP;
    if (aNext != aRoot)
    {
      theParent.ChangeFind(aCur) = aRoot;
    }
    aCur = aNext;
  }
  return aRoot;
}

// ---------------------------------------------------------------------------
// Phase 1: Find free edges.
// ---------------------------------------------------------------------------

NCollection_Array1<BRepGraph_NodeId> findFreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aFreeVec = BRepGraph_Analyze::FreeEdges(theGraph);
  const int                            aNbFree  = aFreeVec.Length();
  if (aNbFree == 0)
  {
    return NCollection_Array1<BRepGraph_NodeId>();
  }
  NCollection_Array1<BRepGraph_NodeId> anArray(1, aNbFree);
  for (int anIdx = 0; anIdx < aNbFree; ++anIdx)
  {
    anArray.SetValue(anIdx + 1, aFreeVec.Value(anIdx));
  }
  return anArray;
}

// ---------------------------------------------------------------------------
// Phase 2: Merge coincident free-edge vertices.
// ---------------------------------------------------------------------------

void assembleVertices(BRepGraph&                              theGraph,
                      const NCollection_Array1<BRepGraph_NodeId>& theFreeEdges,
                      double                                  theTolerance,
                      const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  const int               aNbFreeEdges = theFreeEdges.Length();
  NCollection_Vector<int> aFreeVertexIndices(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Map<int>    aFreeVertexSet(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      theGraph.Defs().Edge(theFreeEdges.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexIdx >= 0 && aFreeVertexSet.Add(anEdge.StartVertexIdx))
    {
      aFreeVertexIndices.Append(anEdge.StartVertexIdx);
    }
    if (anEdge.EndVertexIdx >= 0 && aFreeVertexSet.Add(anEdge.EndVertexIdx))
    {
      aFreeVertexIndices.Append(anEdge.EndVertexIdx);
    }
  }

  const int aNbVertices = aFreeVertexIndices.Length();
  if (aNbVertices == 0)
  {
    return;
  }

  NCollection_OrderedDataMap<int, int> aVertexMerge(aNbVertices, theTmpAlloc);

  // Build point array and index mapping for the KDTree.
  NCollection_Array1<gp_Pnt> aVertexPoints(0, aNbVertices - 1);
  NCollection_Array1<int>    aGraphIndices(0, aNbVertices - 1);
  for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
  {
    const int anIdx = aFreeVertexIndices.Value(aVtxIter);
    aVertexPoints.SetValue(aVtxIter, theGraph.Defs().Vertex(anIdx).Point);
    aGraphIndices.SetValue(aVtxIter, anIdx);
  }

  // Build the KDTree -- O(n log n).
  NCollection_KDTree<gp_Pnt, 3> aTree;
  aTree.Build(aVertexPoints);

  // For each vertex (not already merged), find neighbors within tolerance.
  for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
  {
    const int anIdxI = aGraphIndices.Value(aVtxIter);
    if (aVertexMerge.IsBound(anIdxI))
    {
      continue;
    }

    const gp_Pnt&                    aPntI      = aVertexPoints.Value(aVtxIter);
    NCollection_DynamicArray<size_t> aNeighbors = aTree.RangeSearch(aPntI, theTolerance);

    for (int aNbrIter = 0; aNbrIter < aNeighbors.Length(); ++aNbrIter)
    {
      const int anArrayIdx = static_cast<int>(aNeighbors[aNbrIter]) - 1;
      if (anArrayIdx == aVtxIter)
      {
        continue;
      }

      const int anIdxJ = aGraphIndices.Value(anArrayIdx);
      if (aVertexMerge.IsBound(anIdxJ))
      {
        continue;
      }

      // Merge: keep vertex with smaller tolerance.
      const double aTolI = theGraph.Defs().Vertex(anIdxI).Tolerance;
      const double aTolJ = theGraph.Defs().Vertex(anIdxJ).Tolerance;
      if (aTolJ < aTolI)
      {
        aVertexMerge.Bind(anIdxI, anIdxJ);
        break;
      }
      else
      {
        aVertexMerge.Bind(anIdxJ, anIdxI);
      }
    }
  }

  // Union-find collapse with path compression.
  for (NCollection_OrderedDataMap<int, int>::Iterator anIt(aVertexMerge); anIt.More(); anIt.Next())
  {
    const int aRoot = findRoot(aVertexMerge, anIt.Key());
    if (aRoot != anIt.Value())
    {
      anIt.ChangeValue() = aRoot;
    }
  }

  // Apply vertex merges to free edges.
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    BRepGraph_TopoNode::EdgeDef& anEdge =
      theGraph.Mut().EdgeDef(theFreeEdges.Value(aFreeEdgeIter).Index);
    const int* aMergedStart =
      anEdge.StartVertexIdx >= 0 ? aVertexMerge.Seek(anEdge.StartVertexIdx) : nullptr;
    if (aMergedStart != nullptr)
    {
      anEdge.StartVertexIdx = *aMergedStart;
    }
    const int* aMergedEnd =
      anEdge.EndVertexIdx >= 0 ? aVertexMerge.Seek(anEdge.EndVertexIdx) : nullptr;
    if (aMergedEnd != nullptr)
    {
      anEdge.EndVertexIdx = *aMergedEnd;
    }
  }
}

// ---------------------------------------------------------------------------
// Phase 3 (optional): Cut edges at T-vertex intersections.
// ---------------------------------------------------------------------------

void cutAtIntersections(BRepGraph&                               theGraph,
                        NCollection_Array1<BRepGraph_NodeId>&    theFreeEdges,
                        const BRepGraphAlgo_Sewing::Options&     theOptions,
                        const Handle(NCollection_IncAllocator)&  theTmpAlloc)
{
  const int aNbFreeEdges = theFreeEdges.Length();
  if (aNbFreeEdges == 0)
  {
    return;
  }

  // Step 1: Collect all unique free-edge endpoint vertex indices.
  NCollection_Map<int>    aFreeVtxSet(aNbFreeEdges * 2, theTmpAlloc);
  NCollection_Vector<int> aFreeVtxList(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      theGraph.Defs().Edge(theFreeEdges.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexIdx >= 0 && aFreeVtxSet.Add(anEdge.StartVertexIdx))
    {
      aFreeVtxList.Append(anEdge.StartVertexIdx);
    }
    if (anEdge.EndVertexIdx >= 0 && aFreeVtxSet.Add(anEdge.EndVertexIdx))
    {
      aFreeVtxList.Append(anEdge.EndVertexIdx);
    }
  }

  const int aNbVtx = aFreeVtxList.Length();
  if (aNbVtx == 0)
  {
    return;
  }

  // Pre-compute vertex points for fast access.
  NCollection_Array1<gp_Pnt> aVtxPoints(0, aNbVtx - 1);
  for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
  {
    const int aVtxIdx = aFreeVtxList.Value(aVtxIter);
    aVtxPoints.SetValue(aVtxIter, theGraph.Defs().Vertex(aVtxIdx).Point);
  }

  struct SplitCandidate
  {
    double Param;
    int    VtxIdx;
  };

  // Step 2a (parallel): Find T-vertex split candidates per edge.
  NCollection_Array1<NCollection_Vector<SplitCandidate>> aPerEdgeSplits(1, aNbFreeEdges);

  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      const int                          aFreeEdgeIter = theIdx + 1;
      const BRepGraph_NodeId             anEdgeId      = theFreeEdges.Value(aFreeEdgeIter);
      const BRepGraph_TopoNode::EdgeDef& anEdge        = theGraph.Defs().Edge(anEdgeId.Index);

      if (anEdge.IsDegenerate || anEdge.Curve3d.IsNull())
      {
        return;
      }

      const int aStartIdx = anEdge.StartVertexIdx >= 0 ? anEdge.StartVertexIdx : -1;
      const int aEndIdx   = anEdge.EndVertexIdx >= 0 ? anEdge.EndVertexIdx : -1;

      GeomAdaptor_TransformedCurve aCurve = theGraph.Defs().CurveAdaptor(anEdgeId);
      ExtremaPC_Curve              anExtPC(aCurve);

      Bnd_Box aEdgeBBox;
      BRepGraphAlgo_BndLib::Add(theGraph, anEdgeId, aEdgeBBox);
      if (aEdgeBBox.IsVoid())
      {
        return;
      }
      aEdgeBBox.Enlarge(theOptions.Tolerance);

      const double aTolSq      = theOptions.Tolerance * theOptions.Tolerance;
      const double aParamFirst = aCurve.FirstParameter();
      const double aParamLast  = aCurve.LastParameter();
      const double aParamEps   = Precision::Confusion();

      NCollection_Vector<SplitCandidate> aSplits;

      for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
      {
        const int aVtxIdx = aFreeVtxList.Value(aVtxIter);
        if (aVtxIdx == aStartIdx || aVtxIdx == aEndIdx)
        {
          continue;
        }

        const gp_Pnt& aVtxPnt = aVtxPoints.Value(aVtxIter);

        if (aEdgeBBox.IsOut(aVtxPnt))
        {
          continue;
        }

        const ExtremaPC::Result& aRes =
          anExtPC.Perform(aVtxPnt, Precision::Confusion(), ExtremaPC::SearchMode::Min);
        if (!aRes.IsDone() || aRes.NbExt() == 0)
        {
          continue;
        }

        const int    aMinIdx    = aRes.MinIndex();
        const double aMinSqDist = aRes[aMinIdx].SquareDistance;
        const double aMinParam  = aRes[aMinIdx].Parameter;

        if (aMinSqDist <= aTolSq && aMinParam > aParamFirst + aParamEps
            && aMinParam < aParamLast - aParamEps)
        {
          SplitCandidate aCand;
          aCand.Param  = aMinParam;
          aCand.VtxIdx = aVtxIdx;
          aSplits.Append(aCand);
        }
      }

      if (aSplits.IsEmpty())
      {
        return;
      }

      // Sort candidates by parameter (ascending).
      const int                                 aNbSplits = aSplits.Length();
      NCollection_LocalArray<SplitCandidate, 8> aSortedSplitsBuf(aNbSplits);
      NCollection_Array1<SplitCandidate> aSortedSplits(aSortedSplitsBuf[0], 0, aNbSplits - 1);
      for (int aSplitIter = 0; aSplitIter < aNbSplits; ++aSplitIter)
      {
        aSortedSplits.SetValue(aSplitIter, aSplits.Value(aSplitIter));
      }
      std::sort(&aSortedSplits.ChangeFirst(),
                &aSortedSplits.ChangeFirst() + aNbSplits,
                [](const SplitCandidate& theA, const SplitCandidate& theB) {
                  return theA.Param < theB.Param;
                });

      // Deduplicate: remove candidates with parameters too close together.
      NCollection_Vector<SplitCandidate>& aUniqueSplits = aPerEdgeSplits.ChangeValue(aFreeEdgeIter);
      double                              aPrevParam    = aParamFirst - 1.0;
      for (int aSplitIter = 0; aSplitIter < aNbSplits; ++aSplitIter)
      {
        const SplitCandidate& aCand = aSortedSplits.Value(aSplitIter);
        if (std::abs(aCand.Param - aPrevParam) > aParamEps)
        {
          aUniqueSplits.Append(aCand);
          aPrevParam = aCand.Param;
        }
      }
    },
    !theOptions.Parallel);

  // Step 2b (sequential): Apply splits to graph.
  NCollection_Array1<NCollection_Vector<BRepGraph_NodeId>> aSubEdgeChains(1, aNbFreeEdges);

  bool anySplits = false;

  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const NCollection_Vector<SplitCandidate>& aUniqueSplits = aPerEdgeSplits.Value(aFreeEdgeIter);
    if (aUniqueSplits.IsEmpty())
    {
      continue;
    }

    anySplits                                          = true;
    const BRepGraph_NodeId                anEdgeId     = theFreeEdges.Value(aFreeEdgeIter);
    NCollection_Vector<BRepGraph_NodeId>& aChain       = aSubEdgeChains.ChangeValue(aFreeEdgeIter);
    BRepGraph_NodeId                      aCurrentEdge = anEdgeId;
    for (int aSplitIter = 0; aSplitIter < aUniqueSplits.Length(); ++aSplitIter)
    {
      const SplitCandidate&  aCand = aUniqueSplits.Value(aSplitIter);
      const BRepGraph_NodeId aVtxNodeId(BRepGraph_NodeId::Kind::Vertex, aCand.VtxIdx);
      BRepGraph_NodeId       aSubA, aSubB;
      theGraph.Mut().SplitEdge(aCurrentEdge, aVtxNodeId, aCand.Param, aSubA, aSubB);
      aChain.Append(aSubA);
      aCurrentEdge = aSubB;
    }
    aChain.Append(aCurrentEdge);
  }

  if (!anySplits)
  {
    return;
  }

  // Step 3: Rebuild theFreeEdges with sub-edge chains replacing split edges.
  NCollection_Vector<BRepGraph_NodeId> aNewFreeVec(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const NCollection_Vector<BRepGraph_NodeId>& aChain = aSubEdgeChains.Value(aFreeEdgeIter);
    if (aChain.IsEmpty())
    {
      aNewFreeVec.Append(theFreeEdges.Value(aFreeEdgeIter));
    }
    else
    {
      for (int aSubIter = 0; aSubIter < aChain.Length(); ++aSubIter)
      {
        aNewFreeVec.Append(aChain.Value(aSubIter));
      }
    }
  }

  // Convert to Array1 for thread-safe random access.
  const int                            aNbNewFree = aNewFreeVec.Length();
  NCollection_Array1<BRepGraph_NodeId> aNewFreeArray(1, aNbNewFree);
  for (int anIdx = 0; anIdx < aNbNewFree; ++anIdx)
  {
    aNewFreeArray.SetValue(anIdx + 1, aNewFreeVec.Value(anIdx));
  }
  theFreeEdges = std::move(aNewFreeArray);
}

// ---------------------------------------------------------------------------
// Phase 4: Detect sewing candidates via BBox overlap.
// ---------------------------------------------------------------------------

void detectCandidates(BRepGraph&                                   theGraph,
                      const NCollection_Array1<BRepGraph_NodeId>&  theFreeEdges,
                      const BRepGraphAlgo_Sewing::Options&         theOptions,
                      const Handle(NCollection_IncAllocator)&      theTmpAlloc)
{
  const int aNbFreeEdges = theFreeEdges.Length();
  if (aNbFreeEdges == 0)
  {
    return;
  }

  // Step 1: Precompute bounding boxes in parallel.
  NCollection_Array1<Bnd_Box> aBBoxes(1, aNbFreeEdges);
  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      Bnd_Box aBox;
      BRepGraphAlgo_BndLib::Add(theGraph, theFreeEdges.Value(theIdx + 1), aBox);
      if (!aBox.IsVoid())
      {
        aBox.Enlarge(theOptions.Tolerance);
      }
      aBBoxes.SetValue(theIdx + 1, aBox);
    },
    !theOptions.Parallel);

  // Step 2: Build KDTree from BBox centers.
  NCollection_Array1<gp_Pnt> aCenters(0, aNbFreeEdges - 1);
  NCollection_Array1<double> aHalfDiags(0, aNbFreeEdges - 1);
  double                     aMaxHalfDiag = 0.0;

  for (int anEdgeIter = 1; anEdgeIter <= aNbFreeEdges; ++anEdgeIter)
  {
    const Bnd_Box& aBox = aBBoxes.Value(anEdgeIter);
    if (aBox.IsVoid())
    {
      continue;
    }
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    const gp_Pnt aCenter(0.5 * (aXmin + aXmax), 0.5 * (aYmin + aYmax), 0.5 * (aZmin + aZmax));
    const double aHalfDiag =
      0.5
      * std::sqrt((aXmax - aXmin) * (aXmax - aXmin) + (aYmax - aYmin) * (aYmax - aYmin)
                  + (aZmax - aZmin) * (aZmax - aZmin));
    aCenters.SetValue(anEdgeIter - 1, aCenter);
    aHalfDiags.SetValue(anEdgeIter - 1, aHalfDiag);
    if (aHalfDiag > aMaxHalfDiag)
    {
      aMaxHalfDiag = aHalfDiag;
    }
  }

  NCollection_KDTree<gp_Pnt, 3> aTree;
  aTree.Build(aCenters);

  // Step 3: Find overlapping pairs.
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aPairs(THE_INIT_VECTOR_CAPACITY,
                                                                           theTmpAlloc);

  const auto aSearchFn =
    [&](int theIdxI, NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs) {
      const int      anI   = theIdxI + 1;
      const Bnd_Box& aBoxI = aBBoxes.Value(anI);
      if (aBoxI.IsVoid())
      {
        return;
      }

      const double aHalfDiagI     = aHalfDiags.Value(theIdxI);
      const double aSearchRadiusI = aHalfDiagI + aMaxHalfDiag;

      const gp_Pnt&                    aCenterI   = aCenters.Value(theIdxI);
      NCollection_DynamicArray<size_t> aNeighbors = aTree.RangeSearch(aCenterI, aSearchRadiusI);

      const BRepGraph_NodeId anIdI = theFreeEdges.Value(anI);

      for (int aNbrIter = 0; aNbrIter < aNeighbors.Length(); ++aNbrIter)
      {
        const int anArrayIdx = static_cast<int>(aNeighbors[aNbrIter]) - 1;
        const int anJ        = anArrayIdx + 1;

        if (anJ <= anI)
        {
          continue;
        }

        const Bnd_Box& aBoxJ = aBBoxes.Value(anJ);
        if (aBoxJ.IsVoid())
        {
          continue;
        }

        if (!aBoxI.IsOut(aBoxJ))
        {
          thePairs.Append({anIdI, theFreeEdges.Value(anJ)});
        }
      }
    };

  if (theOptions.Parallel)
  {
    NCollection_Array1<NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>>
      aPerEdgePairs(0, aNbFreeEdges - 1);
    OSD_Parallel::For(
      0,
      aNbFreeEdges,
      [&](int theIdxI) { aSearchFn(theIdxI, aPerEdgePairs.ChangeValue(theIdxI)); },
      false);

    for (int anEdgeIdx = 0; anEdgeIdx < aNbFreeEdges; ++anEdgeIdx)
    {
      const auto& anEdgePairs = aPerEdgePairs.Value(anEdgeIdx);
      for (int aPairIdx = 0; aPairIdx < anEdgePairs.Length(); ++aPairIdx)
      {
        aPairs.Append(anEdgePairs.Value(aPairIdx));
      }
    }
  }
  else
  {
    for (int anEdgeIdx = 0; anEdgeIdx < aNbFreeEdges; ++anEdgeIdx)
    {
      aSearchFn(anEdgeIdx, aPairs);
    }
  }

  // Step 4: Register candidate pairs into the graph.
  for (int aPairIdx = 0; aPairIdx < aPairs.Length(); ++aPairIdx)
  {
    const auto& [anIdI, anIdJ] = aPairs.Value(aPairIdx);
    theGraph.Mut().AddRelEdge(anIdI, anIdJ, BRepGraph_RelEdge::Kind::UserDefined);
    theGraph.Mut().AddRelEdge(anIdJ, anIdI, BRepGraph_RelEdge::Kind::UserDefined);
  }
}

// ---------------------------------------------------------------------------
// Phase 5: Match free-edge pairs.
// ---------------------------------------------------------------------------

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> matchFreeEdges(
  const BRepGraph&                            theGraph,
  const NCollection_Array1<BRepGraph_NodeId>& theFreeEdges,
  const BRepGraphAlgo_Sewing::Options&        theOptions,
  const Handle(NCollection_IncAllocator)&     theTmpAlloc)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;
  const int aNbFreeEdges = theFreeEdges.Length();
  if (aNbFreeEdges == 0)
  {
    return aResult;
  }

  // Build edge-index -> position map.
  NCollection_DataMap<int, int> aFreePosByEdgeIdx(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    aFreePosByEdgeIdx.Bind(theFreeEdges.Value(aFreeEdgeIter).Index, aFreeEdgeIter);
  }

  const NCollection_Vector<NCollection_Vector<BRepGraph_NodeId>> aClusters =
    BRepGraph_Analyze::RelationClusters(theGraph,
                                        theFreeEdges,
                                        BRepGraph_RelEdge::Kind::UserDefined);
  const int aNbComponents = aClusters.Length();

  NCollection_Array1<int> aCompByPos(1, aNbFreeEdges);
  for (int aPosIter = 1; aPosIter <= aNbFreeEdges; ++aPosIter)
  {
    aCompByPos.ChangeValue(aPosIter) = 0;
  }

  for (int aCompIter = 0; aCompIter < aNbComponents; ++aCompIter)
  {
    const int                                   aCompId  = aCompIter + 1;
    const NCollection_Vector<BRepGraph_NodeId>& aCluster = aClusters.Value(aCompIter);
    for (int aNodeIter = 0; aNodeIter < aCluster.Length(); ++aNodeIter)
    {
      const BRepGraph_NodeId aNodeId = aCluster.Value(aNodeIter);
      const int*             aPosPtr = aFreePosByEdgeIdx.Seek(aNodeId.Index);
      if (aPosPtr != nullptr)
      {
        aCompByPos.ChangeValue(*aPosPtr) = aCompId;
      }
    }
  }

  struct MatchResult
  {
    BRepGraph_NodeId BestMatch;
    double           BestScore = RealLast();
  };

  struct ScoredPair
  {
    BRepGraph_NodeId EdgeA;
    BRepGraph_NodeId EdgeB;
    double           Score;
  };

  NCollection_Array1<MatchResult> aPerEdgeMatch(1, aNbFreeEdges);

  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      const int              anI   = theIdx + 1;
      const BRepGraph_NodeId anIdA = theFreeEdges.Value(anI);
      const int              aComp = aCompByPos.Value(anI);

      MatchResult& aMatch = aPerEdgeMatch.ChangeValue(anI);

      const BRepGraph_TopoNode::EdgeDef& anEdgeANode = theGraph.Defs().Edge(anIdA.Index);
      if (anEdgeANode.IsDegenerate)
      {
        return;
      }

      GeomAdaptor_TransformedCurve aCurveA = theGraph.Defs().CurveAdaptor(anIdA);
      GCPnts_UniformAbscissa       aSamplerA(aCurveA, THE_NB_EDGE_MATCH_SAMPLES);
      if (!aSamplerA.IsDone() || aSamplerA.NbPoints() < 2)
      {
        return;
      }

      NCollection_LocalArray<gp_Pnt, 8> aSamplePtsABuf(aSamplerA.NbPoints());
      NCollection_Array1<gp_Pnt>        aSamplePtsA(aSamplePtsABuf[0], 1, aSamplerA.NbPoints());
      for (int aSmpIter = 1; aSmpIter <= aSamplerA.NbPoints(); ++aSmpIter)
      {
        aSamplePtsA.SetValue(aSmpIter, aCurveA.EvalD0(aSamplerA.Parameter(aSmpIter)));
      }

      ExtremaPC_Curve anExtPCRevA(aCurveA);

      const gp_Pnt& aStartA = theGraph.Defs().Vertex(anEdgeANode.StartVertexIdx).Point;
      const gp_Pnt& aEndA   = theGraph.Defs().Vertex(anEdgeANode.EndVertexIdx).Point;
      const double  aChordA = aStartA.Distance(aEndA);

      theGraph.RelEdges().ForEachOutOfKind(
        anIdA,
        BRepGraph_RelEdge::Kind::UserDefined,
        [&](const BRepGraph_RelEdge& theRelEdge) {
          const int* aPosBPtr = aFreePosByEdgeIdx.Seek(theRelEdge.Target.Index);
          if (aPosBPtr == nullptr)
          {
            return;
          }
          if (aCompByPos.Value(*aPosBPtr) != aComp)
          {
            return;
          }

          const BRepGraph_NodeId anIdB = theRelEdge.Target;
          if (BRepGraph_Analyze::AreEdgesCompatibleSampled(theGraph,
                                                           anIdA,
                                                           anIdB,
                                                           aSamplePtsA,
                                                           anExtPCRevA,
                                                           aChordA,
                                                           theOptions.Tolerance,
                                                           THE_NB_EDGE_MATCH_SAMPLES,
                                                           THE_MAX_CHORD_RATIO,
                                                           THE_HIGH_CONFIDENCE_RATIO))
          {
            const double aScore = BRepGraph_Analyze::EdgeEndpointPairScore(theGraph, anIdA, anIdB);
            if (aScore < aMatch.BestScore)
            {
              aMatch.BestScore = aScore;
              aMatch.BestMatch = anIdB;
            }
          }
        });
    },
    !theOptions.Parallel);

  NCollection_Array1<NCollection_Vector<ScoredPair>> aCompScoredPairs(1, aNbComponents);
  for (int aEdgeIter = 1; aEdgeIter <= aNbFreeEdges; ++aEdgeIter)
  {
    const MatchResult& aMatch = aPerEdgeMatch.Value(aEdgeIter);
    if (!aMatch.BestMatch.IsValid())
    {
      continue;
    }

    const int aComp = aCompByPos.Value(aEdgeIter);
    if (aComp <= 0)
    {
      continue;
    }

    ScoredPair aPair;
    aPair.EdgeA = theFreeEdges.Value(aEdgeIter);
    aPair.EdgeB = aMatch.BestMatch;
    aPair.Score = aMatch.BestScore;
    aCompScoredPairs.ChangeValue(aComp).Append(aPair);
  }

  NCollection_Array1<NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>> aCompPairs(
    1,
    aNbComponents);

  OSD_Parallel::For(
    0,
    aNbComponents,
    [&](int theCompIdx) {
      const int                       aComp        = theCompIdx + 1;
      NCollection_Vector<ScoredPair>& aScoredPairs = aCompScoredPairs.ChangeValue(aComp);
      const int                       aNbScored    = aScoredPairs.Length();
      if (aNbScored <= 0)
      {
        return;
      }

      NCollection_LocalArray<ScoredPair, 64> aSortedPairsBuf(aNbScored);
      NCollection_Array1<ScoredPair>         aSortedPairs(aSortedPairsBuf[0], 0, aNbScored - 1);
      for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
      {
        aSortedPairs.SetValue(anIdx, aScoredPairs.Value(anIdx));
      }

      std::sort(&aSortedPairs.ChangeFirst(),
                &aSortedPairs.ChangeFirst() + aNbScored,
                [](const ScoredPair& theA, const ScoredPair& theB) {
                  if (theA.Score < theB.Score)
                  {
                    return true;
                  }
                  if (theA.Score > theB.Score)
                  {
                    return false;
                  }
                  if (theA.EdgeA.Index != theB.EdgeA.Index)
                  {
                    return theA.EdgeA.Index < theB.EdgeA.Index;
                  }
                  return theA.EdgeB.Index < theB.EdgeB.Index;
                });

      NCollection_Map<int> aConsumed;
      for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
      {
        const ScoredPair& aPair = aSortedPairs.Value(anIdx);
        if (aConsumed.Contains(aPair.EdgeA.Index) || aConsumed.Contains(aPair.EdgeB.Index))
        {
          continue;
        }

        aCompPairs.ChangeValue(aComp).Append({aPair.EdgeA, aPair.EdgeB});
        aConsumed.Add(aPair.EdgeA.Index);
        aConsumed.Add(aPair.EdgeB.Index);
      }
    },
    !theOptions.Parallel);

  for (int aCompIter = 1; aCompIter <= aNbComponents; ++aCompIter)
  {
    const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& aPairs =
      aCompPairs.Value(aCompIter);
    for (int aPairIter = 0; aPairIter < aPairs.Length(); ++aPairIter)
    {
      aResult.Append(aPairs.Value(aPairIter));
    }
  }

  return aResult;
}

// ---------------------------------------------------------------------------
// Phase 6: Merge matched edge pairs (graph-only, no shape access).
// ---------------------------------------------------------------------------

int mergeMatchedEdges(
  BRepGraph&                                                                 theGraph,
  const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>&   thePairs,
  const BRepGraphAlgo_Sewing::Options&                                       theOptions,
  NCollection_IndexedMap<int>&                                               theSewnEdgeIndices)
{
  int aSewnCount = 0;

  // Reusable single-element buffer for history recording.
  NCollection_Vector<BRepGraph_NodeId> aReplacement;

  for (int aPairIter = 0; aPairIter < thePairs.Length(); ++aPairIter)
  {
    const auto& [anIdA, anIdB] = thePairs.Value(aPairIter);
    theSewnEdgeIndices.Add(anIdA.Index);

    const BRepGraph_TopoNode::EdgeDef& aRemoveEdge = theGraph.Defs().Edge(anIdB.Index);
    const BRepGraph_TopoNode::EdgeDef& aKeepEdge   = theGraph.Defs().Edge(anIdA.Index);

    // 1. Tolerance merge (graph-only).
    const double aMergedTol = std::max(aKeepEdge.Tolerance, aRemoveEdge.Tolerance);
    theGraph.Mut().EdgeDef(anIdA.Index).Tolerance = aMergedTol;

    // 2. PCurve transfer from remove-edge to keep-edge.
    for (int aPCurveIter = 0; aPCurveIter < aRemoveEdge.PCurves.Length(); ++aPCurveIter)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
        aRemoveEdge.PCurves.Value(aPCurveIter);

      if (!aPCEntry.Curve2d.IsNull())
      {
        // Add PCurve entry to keep-edge via graph API; preserve original edge orientation
        // so that seam edges (REVERSED orientation) are correctly reconstructed as C2.
        theGraph.Mut().AddPCurveToEdge(anIdA,
                                       aPCEntry.FaceDefId,
                                       aPCEntry.Curve2d,
                                       aPCEntry.ParamFirst,
                                       aPCEntry.ParamLast,
                                       aPCEntry.EdgeOrientation);
      }
    }

    // 3. Determine direction correspondence from vertex positions.
    const gp_Pnt& aKeepStart   = theGraph.Defs().Vertex(aKeepEdge.StartVertexIdx).Point;
    const gp_Pnt& aRemoveStart = theGraph.Defs().Vertex(aRemoveEdge.StartVertexIdx).Point;
    const bool    isReversed   = aKeepStart.Distance(aRemoveStart) > theOptions.Tolerance;

    // 4. Update wire entries referencing the remove-edge.
    // Copy the wire list since ReplaceEdgeInWire modifies the reverse index.
    const NCollection_Vector<int> aWires = theGraph.Builder().WiresOfEdge(anIdB.Index);
    for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
    {
      theGraph.Mut().ReplaceEdgeInWire(aWires.Value(aWIdx), anIdB, anIdA, isReversed);
    }

    // 5. Record history.
    aReplacement.Clear();
    aReplacement.Append(anIdA);
    theGraph.History().Record("Sewing:MergeEdge", anIdB, aReplacement);

    ++aSewnCount;
  }

  // 6. SameParameter enforcement (graph-only, parallel).
  if (theOptions.SameParameterMode && !theSewnEdgeIndices.IsEmpty())
  {
    BRepGraphAlgo_SameParameter::Perform(theGraph,
                                         theSewnEdgeIndices,
                                         theOptions.Tolerance,
                                         theOptions.Parallel);
  }

  return aSewnCount;
}

// ---------------------------------------------------------------------------
// Phase 7: Process sewn edges (tolerance consistency).
// ---------------------------------------------------------------------------

void processEdges(BRepGraph&                         theGraph,
                  const NCollection_IndexedMap<int>& theSewnEdgeIndices,
                  const BRepGraphAlgo_Sewing::Options& theOptions)
{
  const int aNbSewn = theSewnEdgeIndices.Extent();
  if (aNbSewn == 0)
  {
    return;
  }

  OSD_Parallel::For(
    0,
    aNbSewn,
    [&](int theIdx) {
      const int                          anEdgeIdx = theSewnEdgeIndices.FindKey(theIdx + 1);
      const BRepGraph_TopoNode::EdgeDef& anEdge    = theGraph.Defs().Edge(anEdgeIdx);
      if (anEdge.IsDegenerate)
      {
        return;
      }

      // Ensure edge tolerance is at least as large as its vertex tolerances.
      double aMaxVtxTol = 0.0;
      if (anEdge.StartVertexIdx >= 0)
      {
        aMaxVtxTol =
          std::max(aMaxVtxTol, theGraph.Defs().Vertex(anEdge.StartVertexIdx).Tolerance);
      }
      if (anEdge.EndVertexIdx >= 0)
      {
        aMaxVtxTol =
          std::max(aMaxVtxTol, theGraph.Defs().Vertex(anEdge.EndVertexIdx).Tolerance);
      }

      if (aMaxVtxTol > anEdge.Tolerance)
      {
        theGraph.Mut().EdgeDef(anEdgeIdx).Tolerance = aMaxVtxTol;
        theGraph.Cache().Invalidate(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx));
      }
    },
    !theOptions.Parallel);
}

// ---------------------------------------------------------------------------
// Reconstruct result shape from graph state.
// ---------------------------------------------------------------------------

TopoDS_Shape reconstructFromGraph(const BRepGraph& theGraph)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  if (aDefs.NbCompounds() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Compound(0));
  }
  else if (aDefs.NbCompSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::CompSolid(0));
  }
  else if (aDefs.NbSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Solid(0));
  }
  else if (aDefs.NbShells() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Shell(0));
  }
  else
  {
    // Flat faces -- wrap in compound.
    BRep_Builder    aBB;
    TopoDS_Compound aResultCompound;
    aBB.MakeCompound(aResultCompound);
    for (int aFaceIdx = 0; aFaceIdx < aDefs.NbFaces(); ++aFaceIdx)
    {
      if (!aDefs.Face(aFaceIdx).IsRemoved)
      {
        aBB.Add(aResultCompound, theGraph.Shapes().ReconstructFace(aFaceIdx));
      }
    }
    return aResultCompound;
  }
}

} // anonymous namespace

//=================================================================================================

BRepGraphAlgo_Sewing::Result BRepGraphAlgo_Sewing::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_Sewing::Result BRepGraphAlgo_Sewing::Perform(BRepGraph&     theGraph,
                                                           const Options& theOptions)
{
  Result aResult;

  if (!theGraph.IsDone())
  {
    return aResult;
  }

  // Configure history on the graph.
  theGraph.SetHistoryEnabled(theOptions.HistoryMode);

  // Main allocator for cross-phase data.
  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator;
  // Temporary allocator for per-phase scratch, Reset(false) between phases.
  Handle(NCollection_IncAllocator) aTmpAllocator = new NCollection_IncAllocator;

  // Phase 1: Find free edges.
  NCollection_Array1<BRepGraph_NodeId> aFreeEdges = findFreeEdges(theGraph);
  aResult.NbFreeEdgesBefore = aFreeEdges.Length();

  if (aFreeEdges.IsEmpty())
  {
    aResult.IsDone          = true;
    aResult.NbFreeEdgesAfter = 0;
    return aResult;
  }

  // Phase 2: Assemble vertices.
  assembleVertices(theGraph, aFreeEdges, theOptions.Tolerance, aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 3 (optional): Cut edges at intersections.
  if (theOptions.Cutting)
  {
    cutAtIntersections(theGraph, aFreeEdges, theOptions, aTmpAllocator);
    aTmpAllocator->Reset(false);
  }

  // Phase 4: Detect sewing candidates.
  detectCandidates(theGraph, aFreeEdges, theOptions, aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 5: Match free edge pairs.
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
    matchFreeEdges(theGraph, aFreeEdges, theOptions, aTmpAllocator);
  aTmpAllocator->Reset(false);

  if (aMatchedPairs.IsEmpty())
  {
    aResult.IsDone           = true;
    aResult.NbFreeEdgesAfter = aResult.NbFreeEdgesBefore;
    return aResult;
  }

  // Phase 6: Merge matched edge pairs.
  const int                   aNbPairs = aMatchedPairs.Length();
  NCollection_IndexedMap<int> aSewnEdgeIndices(aNbPairs, anAllocator);
  aResult.NbSewnEdges = mergeMatchedEdges(theGraph, aMatchedPairs, theOptions, aSewnEdgeIndices);

  // Phase 7: Process sewn edges (tolerance consistency).
  processEdges(theGraph, aSewnEdgeIndices, theOptions);

  // Count free edges after sewing by re-analyzing the graph state.
  const NCollection_Vector<BRepGraph_NodeId> aFreeEdgesAfter =
    BRepGraph_Analyze::FreeEdges(theGraph);
  aResult.NbFreeEdgesAfter = aFreeEdgesAfter.Length();

  aResult.IsDone = true;
  return aResult;
}

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Sewing::Sew(const TopoDS_Shape& theShape)
{
  return Sew(theShape, Options());
}

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Sewing::Sew(const TopoDS_Shape& theShape, const Options& theOptions)
{
  if (theShape.IsNull())
  {
    return TopoDS_Shape();
  }

  BRepGraph aGraph;
  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator;
  aGraph.SetAllocator(anAllocator);
  aGraph.Build(theShape, theOptions.Parallel);
  if (!aGraph.IsDone())
  {
    return TopoDS_Shape();
  }

  Result aResult = Perform(aGraph, theOptions);
  if (!aResult.IsDone)
  {
    return TopoDS_Shape();
  }

  // Reconstruct from the topmost graph node.
  TopoDS_Shape aResultShape = reconstructFromGraph(aGraph);

  // Encode regularity on the result shape (post-reconstruction step).
  if (!aResultShape.IsNull())
  {
    BRepLib::EncodeRegularity(aResultShape, theOptions.Tolerance);
  }

  return aResultShape;
}
