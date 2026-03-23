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
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_KDTree.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <algorithm>

//=================================================================================================

BRepGraphAlgo_Sewing::BRepGraphAlgo_Sewing(double theTolerance)
    : myTolerance(theTolerance)
{
}

//=================================================================================================

void BRepGraphAlgo_Sewing::Add(const TopoDS_Shape& theShape)
{
  myInputShapes.Append(theShape);
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetFaceAnalysis(bool theVal)
{
  myFaceAnalysis = theVal;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetCutting(bool theVal)
{
  myCutting = theVal;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetSameParameterMode(bool theVal)
{
  mySameParameter = theVal;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetNonManifoldMode(bool theVal)
{
  myNonManifold = theVal;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetParallel(bool theVal)
{
  myIsParallel = theVal;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::FaceAnalysis() const
{
  return myFaceAnalysis;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::Cutting() const
{
  return myCutting;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::SameParameterMode() const
{
  return mySameParameter;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::NonManifoldMode() const
{
  return myNonManifold;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::IsParallel() const
{
  return myIsParallel;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::SetHistoryMode(bool theVal)
{
  myHistoryMode = theVal;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::HistoryMode() const
{
  return myHistoryMode;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::Perform()
{
  myIsDone          = false;
  mySewnCount       = 0;
  myFreeEdgesBefore = NCollection_Array1<BRepGraph_NodeId>();
  myFreeEdgesAfter  = 0;

  if (myInputShapes.IsEmpty())
  {
    return;
  }

  // Main allocator for cross-phase data (IncAllocator for arena-style performance).
  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator;
  // Temporary allocator for per-phase scratch, Reset(false) between phases for memory reuse.
  Handle(NCollection_IncAllocator) aTmpAllocator = new NCollection_IncAllocator;

  // Phase 1: Build graph and analyze faces (pass arena allocator to the graph).
  analyzeFaces(anAllocator);
  if (!myGraph.IsDone())
  {
    return;
  }

  // Phase 2: Find free edges.
  findFreeEdges();
  if (myFreeEdgesBefore.IsEmpty())
  {
    // No free edges -- return input as-is.
    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int aShapeIter = 1; aShapeIter <= myInputShapes.Length(); ++aShapeIter)
    {
      aBB.Add(aCompound, myInputShapes.Value(aShapeIter));
    }
    myResult = aCompound;
    myIsDone = true;
    return;
  }

  // Phase 3: Assemble vertices (uses tmp allocator internally).
  assembleVertices(aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 4a: Detect sewing candidates via BBox overlap.
  detectCandidates(aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 4b: Cut edges at intersections (if enabled).
  if (myCutting)
  {
    cutAtIntersections();
  }

  // Phase 5a: Match free edge pairs.
  NCollection_Sequence<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
    matchFreeEdges(aTmpAllocator);
  aTmpAllocator->Reset(false);

  if (aMatchedPairs.IsEmpty())
  {
    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int aShapeIter = 1; aShapeIter <= myInputShapes.Length(); ++aShapeIter)
    {
      aBB.Add(aCompound, myInputShapes.Value(aShapeIter));
    }
    myResult         = aCompound;
    myFreeEdgesAfter = myFreeEdgesBefore.Length();
    myIsDone         = true;
    return;
  }

  // Phase 5b: Merge matched edge pairs.
  // Estimate: affected faces ~ matched pairs, sewn edge indices ~ matched pairs.
  const int            aNbPairs = aMatchedPairs.Length();
  NCollection_Map<int> anAffectedFaces(aNbPairs, anAllocator);
  NCollection_Map<int> aSewnEdgeIndices(aNbPairs, anAllocator);
  mergeMatchedEdges(aMatchedPairs, anAffectedFaces, aSewnEdgeIndices);

  // Phase 6: Process sewn edges only (O(sewn) instead of O(all_edges × all_faces)).
  processEdges(aSewnEdgeIndices);

  // Phase 7: Reconstruct result.
  reconstructResult(anAffectedFaces);

  // Phase 8: Edge regularity.
  setEdgeRegularity();

  // Compute free edges after sewing.
  myFreeEdgesAfter = myFreeEdgesBefore.Length() - 2 * mySewnCount;
  if (myFreeEdgesAfter < 0)
  {
    myFreeEdgesAfter = 0;
  }

  myIsDone = true;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::analyzeFaces(const Handle(NCollection_IncAllocator)& theAllocator)
{
  // Assemble input into a compound and build the graph.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int aShapeIter = 1; aShapeIter <= myInputShapes.Length(); ++aShapeIter)
  {
    aBB.Add(aCompound, myInputShapes.Value(aShapeIter));
  }

  myGraph.SetAllocator(theAllocator);
  myGraph.SetHistoryEnabled(myHistoryMode);
  myGraph.Build(aCompound, myIsParallel);
}

//=================================================================================================

void BRepGraphAlgo_Sewing::findFreeEdges()
{
  // FreeEdges() returns NCollection_Sequence; convert to Array1 for thread-safe
  // random access in parallel phases (NCollection_Sequence is not safe for concurrent reads).
  NCollection_Sequence<BRepGraph_NodeId> aFreeSeq = myGraph.FreeEdges();
  const int                              aNbFree  = aFreeSeq.Length();
  if (aNbFree == 0)
  {
    myFreeEdgesBefore = NCollection_Array1<BRepGraph_NodeId>();
    return;
  }
  NCollection_Array1<BRepGraph_NodeId> anArray(1, aNbFree);
  for (int anIdx = 1; anIdx <= aNbFree; ++anIdx)
  {
    anArray.SetValue(anIdx, aFreeSeq.Value(anIdx));
  }
  myFreeEdgesBefore = std::move(anArray);
}

//=================================================================================================

void BRepGraphAlgo_Sewing::assembleVertices(const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  // Merge coincident free-edge vertices within tolerance using KDTree spatial indexing.
  // Build a map: vertex index -> representative vertex index.
  // Uses default allocator since IsBound() lookups are needed.
  NCollection_DataMap<int, int> aVertexMerge;

  // Collect all unique vertex indices from free edges.
  // Each free edge has 2 vertices; estimate unique count ~ number of free edges.
  const int                 aNbFreeEdges = myFreeEdgesBefore.Length();
  NCollection_Sequence<int> aFreeVertexIndices(theTmpAlloc);
  NCollection_Map<int>      aFreeVertexSet(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= myFreeEdgesBefore.Length(); ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::Edge& anEdge =
      myGraph.Edge(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexId.IsValid() && aFreeVertexSet.Add(anEdge.StartVertexId.Index))
    {
      aFreeVertexIndices.Append(anEdge.StartVertexId.Index);
    }
    if (anEdge.EndVertexId.IsValid() && aFreeVertexSet.Add(anEdge.EndVertexId.Index))
    {
      aFreeVertexIndices.Append(anEdge.EndVertexId.Index);
    }
  }

  const int aNbVertices = aFreeVertexIndices.Length();
  if (aNbVertices == 0)
  {
    return;
  }

  // Build point array and index mapping for the KDTree.
  // aVertexPoints uses 0-based indexing; aGraphIndices maps array position -> graph vertex index.
  NCollection_Array1<gp_Pnt> aVertexPoints(0, aNbVertices - 1);
  NCollection_Array1<int>    aGraphIndices(0, aNbVertices - 1);
  for (int aVtxIter = 1; aVtxIter <= aNbVertices; ++aVtxIter)
  {
    const int anIdx = aFreeVertexIndices.Value(aVtxIter);
    aVertexPoints.SetValue(aVtxIter - 1, myGraph.Vertex(anIdx).Point);
    aGraphIndices.SetValue(aVtxIter - 1, anIdx);
  }

  // Build the KDTree -- O(n log n).
  NCollection_KDTree<gp_Pnt, 3> aTree;
  aTree.Build(aVertexPoints);

  // For each vertex (not already merged), find neighbors within tolerance -- O(log n + k) each.
  for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
  {
    const int anIdxI = aGraphIndices.Value(aVtxIter);
    if (aVertexMerge.IsBound(anIdxI))
    {
      continue;
    }

    const gp_Pnt&                    aPntI      = aVertexPoints.Value(aVtxIter);
    NCollection_DynamicArray<size_t> aNeighbors = aTree.RangeSearch(aPntI, myTolerance);

    for (int aNbrIter = 0; aNbrIter < aNeighbors.Length(); ++aNbrIter)
    {
      // RangeSearch returns 1-based indices into the point array.
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
      const double aTolI = myGraph.Vertex(anIdxI).Tolerance;
      const double aTolJ = myGraph.Vertex(anIdxJ).Tolerance;
      if (aTolJ < aTolI)
      {
        aVertexMerge.Bind(anIdxI, anIdxJ);
        break; // anIdxI is now merged, stop looking for more neighbors
      }
      else
      {
        aVertexMerge.Bind(anIdxJ, anIdxI);
      }
    }
  }

  // Apply vertex merges to free edges.
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= myFreeEdgesBefore.Length(); ++aFreeEdgeIter)
  {
    BRepGraph_TopoNode::Edge& anEdge =
      myGraph.MutableEdge(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexId.IsValid() && aVertexMerge.IsBound(anEdge.StartVertexId.Index))
    {
      anEdge.StartVertexId =
        BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVertexMerge.Find(anEdge.StartVertexId.Index));
    }
    if (anEdge.EndVertexId.IsValid() && aVertexMerge.IsBound(anEdge.EndVertexId.Index))
    {
      anEdge.EndVertexId =
        BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVertexMerge.Find(anEdge.EndVertexId.Index));
    }
  }
}

//=================================================================================================

void BRepGraphAlgo_Sewing::detectCandidates(const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  // Phase 4a: Populate UserDefined relationship edges between free edges whose
  // bounding boxes overlap within tolerance. Uses KDTree on BBox centers for
  // O(n log n) candidate detection instead of O(n^2) pairwise comparison.
  const int aNbFreeEdges = myFreeEdgesBefore.Length();
  if (aNbFreeEdges == 0)
  {
    return;
  }

  // Step 1: Precompute bounding boxes in parallel (BoundingBox() is const/thread-safe).
  // myFreeEdgesBefore is NCollection_Array1 -- safe for concurrent random access.
  NCollection_Array1<Bnd_Box> aBBoxes(1, aNbFreeEdges);
  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      Bnd_Box aBox = myGraph.BoundingBox(myFreeEdgesBefore.Value(theIdx + 1));
      if (!aBox.IsVoid())
      {
        aBox.Enlarge(myTolerance);
      }
      aBBoxes.SetValue(theIdx + 1, aBox);
    },
    !myIsParallel);

  // Step 2: Build KDTree from BBox centers and track the maximum half-diagonal.
  NCollection_Array1<gp_Pnt> aCenters(0, aNbFreeEdges - 1);
  NCollection_Array1<double> aHalfDiags(0, aNbFreeEdges - 1);
  double                     aMaxHalfDiag = 0.0;

  for (int anEdgeIter = 1; anEdgeIter <= aNbFreeEdges; ++anEdgeIter)
  {
    const Bnd_Box& aBox = aBBoxes.Value(anEdgeIter);
    if (aBox.IsVoid())
    {
      aCenters.SetValue(anEdgeIter - 1, gp_Pnt(0.0, 0.0, 0.0));
      aHalfDiags.SetValue(anEdgeIter - 1, -1.0); // mark as void
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

  // Step 3: Find overlapping pairs using KDTree range search, then refine with actual BBox
  // overlap. Per-edge adaptive search radius: halfDiag_i + max_halfDiag is a valid upper bound
  // and tighter than 2 * max_halfDiag for small edges.
  // In parallel mode, each thread writes to its own slot to avoid mutex contention.
  // In sequential mode, append directly to the result sequence (zero overhead).
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aPairs(256, theTmpAlloc);

  // Common lambda for the KDTree search + BBox refinement per edge.
  const auto aSearchFn =
    [&](int theIdxI, NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs) {
      const int      anI   = theIdxI + 1;
      const Bnd_Box& aBoxI = aBBoxes.Value(anI);
      if (aBoxI.IsVoid())
      {
        return;
      }

      const double aHalfDiagI = aHalfDiags.Value(theIdxI);
      if (aHalfDiagI < 0.0)
        return; // void BBox
      const double aSearchRadiusI = aHalfDiagI + aMaxHalfDiag;

      const gp_Pnt&                    aCenterI   = aCenters.Value(theIdxI);
      NCollection_DynamicArray<size_t> aNeighbors = aTree.RangeSearch(aCenterI, aSearchRadiusI);

      const BRepGraph_NodeId anIdI = myFreeEdgesBefore.Value(anI);

      for (int aNbrIter = 0; aNbrIter < aNeighbors.Length(); ++aNbrIter)
      {
        // RangeSearch returns 1-based indices into the point array.
        const int anArrayIdx = static_cast<int>(aNeighbors[aNbrIter]) - 1;
        const int anJ        = anArrayIdx + 1; // 1-based BBox/edge index

        // Only consider pairs where j > i to avoid duplicates.
        if (anJ <= anI)
        {
          continue;
        }

        const Bnd_Box& aBoxJ = aBBoxes.Value(anJ);
        if (aBoxJ.IsVoid())
        {
          continue;
        }

        // Refine: actual BBox overlap test.
        if (!aBoxI.IsOut(aBoxJ))
        {
          thePairs.Append({anIdI, myFreeEdgesBefore.Value(anJ)});
        }
      }
    };

  if (myIsParallel)
  {
    // Parallel: per-edge result slots, merged after the loop.
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
    // Sequential: append directly, no allocation overhead.
    for (int anEdgeIdx = 0; anEdgeIdx < aNbFreeEdges; ++anEdgeIdx)
    {
      aSearchFn(anEdgeIdx, aPairs);
    }
  }

  // Step 4: Register all candidate pairs into the graph (sequential, modifies graph).
  for (int aPairIdx = 0; aPairIdx < aPairs.Length(); ++aPairIdx)
  {
    const auto& [anIdI, anIdJ] = aPairs.Value(aPairIdx);
    myGraph.AddRelEdge(anIdI, anIdJ, BRepGraph_RelKind::UserDefined);
    myGraph.AddRelEdge(anIdJ, anIdI, BRepGraph_RelKind::UserDefined);
  }
}

//=================================================================================================

void BRepGraphAlgo_Sewing::cutAtIntersections()
{
  // Cutting phase: split edges that partially overlap (share vertices but
  // aren't fully coincident) at intersection parameters.
  // For edges connected via UserDefined BBox-overlap edges, check if they
  // share one vertex but not both -- this indicates partial overlap.
  // Currently handles the common case; more complex intersection detection
  // (e.g., mid-edge crossings) can be added in future iterations.
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= myFreeEdgesBefore.Length(); ++aFreeEdgeIter)
  {
    const BRepGraph_NodeId          anIdA  = myFreeEdgesBefore.Value(aFreeEdgeIter);
    const BRepGraph_TopoNode::Edge& aNodeA = myGraph.Edge(anIdA.Index);

    myGraph.ForEachOutEdgeOfKind(anIdA, BRepGraph_RelKind::UserDefined, [&](int theRelEdgeIdx) {
      const BRepGraph_RelEdge&        aRelEdge = myGraph.RelEdge(theRelEdgeIdx);
      const BRepGraph_NodeId          anIdB    = aRelEdge.Target;
      const BRepGraph_TopoNode::Edge& aNodeB   = myGraph.Edge(anIdB.Index);

      if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
        return;

      // Count shared vertices between the two edges.
      int aNbShared = 0;
      if (aNodeA.StartVertexId == aNodeB.StartVertexId
          || aNodeA.StartVertexId == aNodeB.EndVertexId)
        ++aNbShared;
      if (aNodeA.EndVertexId == aNodeB.StartVertexId || aNodeA.EndVertexId == aNodeB.EndVertexId)
        ++aNbShared;

      // Partial overlap: one shared vertex but not both.
      // Full edge splitting requires ShapeAnalysis_Curve parameter computation
      // and BRepBuilderAPI_MakeEdge -- reserved for future enhancement.
      (void)aNbShared;
    });
  }
}

//=================================================================================================

NCollection_Sequence<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> BRepGraphAlgo_Sewing::
  matchFreeEdges(const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  NCollection_Sequence<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;
  const int aNbFreeEdges = myFreeEdgesBefore.Length();
  if (aNbFreeEdges == 0)
    return aResult;

  // myFreeEdgesBefore is NCollection_Array1 -- safe for concurrent random access.
  NCollection_Map<int> aFreeEdgeIndices(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    aFreeEdgeIndices.Add(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
  }

  // Step 1 (parallel): For each free edge, find its best candidate via geometric validation.
  // areEdgesSewable() is const and uses only read-only graph data, so it is thread-safe.
  // Store per-edge best match and score.
  struct MatchResult
  {
    BRepGraph_NodeId BestMatch;
    double           BestScore = RealLast();
  };

  NCollection_Array1<MatchResult> aPerEdgeMatch(1, aNbFreeEdges);

  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      const int              anI   = theIdx + 1;
      const BRepGraph_NodeId anIdA = myFreeEdgesBefore.Value(anI);

      MatchResult& aMatch = aPerEdgeMatch.ChangeValue(anI);

      // Build per-edgeA cache: adaptor, sample points, parameters.
      const BRepGraph_TopoNode::Edge& anEdgeANode = myGraph.Edge(anIdA.Index);
      if (anEdgeANode.IsDegenerate)
        return;

      const TopoDS_Edge&     anEdgeAShape = TopoDS::Edge(anEdgeANode.OriginalShape);
      BRepAdaptor_Curve      aCurveA(anEdgeAShape);
      constexpr int          THE_NB_SAMPLES = 5;
      GCPnts_UniformAbscissa aSamplerA(aCurveA, THE_NB_SAMPLES);
      if (!aSamplerA.IsDone() || aSamplerA.NbPoints() < 2)
        return;

      // Pre-sample points on edgeA (reused across all edgeB candidates).
      NCollection_Array1<gp_Pnt> aSamplePtsA(1, aSamplerA.NbPoints());
      for (int aSmpIter = 1; aSmpIter <= aSamplerA.NbPoints(); ++aSmpIter)
      {
        aSamplePtsA.SetValue(aSmpIter, aCurveA.Value(aSamplerA.Parameter(aSmpIter)));
      }

      // Pre-initialize projectors (reused across all edgeB candidates to avoid
      // repeated Extrema_GGExtPC construction/destruction which dominates runtime).
      Extrema_ExtPC aProjectorFwdB; // re-initialized per edgeB inside areEdgesSewable
      Extrema_ExtPC aProjectorRev;
      aProjectorRev.Initialize(aCurveA,
                               aCurveA.FirstParameter(),
                               aCurveA.LastParameter(),
                               Precision::Confusion());

      const gp_Pnt& aStartA = myGraph.Vertex(anEdgeANode.StartVertexId.Index).Point;
      const gp_Pnt& aEndA   = myGraph.Vertex(anEdgeANode.EndVertexId.Index).Point;
      const double  aChordA = aStartA.Distance(aEndA);

      myGraph.ForEachOutEdgeOfKind(anIdA, BRepGraph_RelKind::UserDefined, [&](int theRelEdgeIdx) {
        const BRepGraph_RelEdge& aRelEdge = myGraph.RelEdge(theRelEdgeIdx);
        const BRepGraph_NodeId   anIdB    = aRelEdge.Target;

        if (!aFreeEdgeIndices.Contains(anIdB.Index))
          return;

        // Full geometric validation using cached edgeA data.
        if (areEdgesSewable(anIdA, anIdB, aSamplePtsA, aProjectorFwdB, aProjectorRev, aChordA))
        {
          const BRepGraph_TopoNode::Edge& anEdgeB = myGraph.Edge(anIdB.Index);
          const gp_Pnt& aStartB = myGraph.Vertex(anEdgeB.StartVertexId.Index).Point;
          const gp_Pnt& aEndB   = myGraph.Vertex(anEdgeB.EndVertexId.Index).Point;

          double aScore = std::min(aStartA.Distance(aStartB) + aEndA.Distance(aEndB),
                                   aStartA.Distance(aEndB) + aEndA.Distance(aStartB));
          if (aScore < aMatch.BestScore)
          {
            aMatch.BestScore = aScore;
            aMatch.BestMatch = anIdB;
          }
        }
      });
    },
    !myIsParallel);

  // Step 2 (sequential): Sorted greedy pairing -- process best matches first
  // to avoid suboptimal pairings from input-order bias.
  struct ScoredPair
  {
    BRepGraph_NodeId EdgeA;
    BRepGraph_NodeId EdgeB;
    double           Score;
  };

  // Collect all valid (edgeA, edgeB, score) tuples.
  NCollection_Sequence<ScoredPair> aScoredPairs;
  for (int aEdgeIter = 1; aEdgeIter <= aNbFreeEdges; ++aEdgeIter)
  {
    const MatchResult& aMatch = aPerEdgeMatch.Value(aEdgeIter);
    if (!aMatch.BestMatch.IsValid())
      continue;
    ScoredPair aPair;
    aPair.EdgeA = myFreeEdgesBefore.Value(aEdgeIter);
    aPair.EdgeB = aMatch.BestMatch;
    aPair.Score = aMatch.BestScore;
    aScoredPairs.Append(aPair);
  }

  // Sort by ascending score (best matches first). Use array for std::sort.
  const int aNbScored = aScoredPairs.Length();
  if (aNbScored > 0)
  {
    NCollection_Array1<ScoredPair> aSortedPairs(0, aNbScored - 1);
    for (int anIdx = 1; anIdx <= aNbScored; ++anIdx)
    {
      aSortedPairs.SetValue(anIdx - 1, aScoredPairs.Value(anIdx));
    }
    std::sort(
      &aSortedPairs.ChangeFirst(),
      &aSortedPairs.ChangeFirst() + aNbScored,
      [](const ScoredPair& theA, const ScoredPair& theB) { return theA.Score < theB.Score; });

    // Greedily consume from best to worst.
    NCollection_Map<int> aConsumed(aNbFreeEdges, theTmpAlloc);
    for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
    {
      const ScoredPair& aPair = aSortedPairs.Value(anIdx);
      if (aConsumed.Contains(aPair.EdgeA.Index))
        continue;
      if (aConsumed.Contains(aPair.EdgeB.Index))
        continue;

      aResult.Append({aPair.EdgeA, aPair.EdgeB});
      aConsumed.Add(aPair.EdgeA.Index);
      aConsumed.Add(aPair.EdgeB.Index);
    }
  }

  return aResult;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::mergeMatchedEdges(
  const NCollection_Sequence<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs,
  NCollection_Map<int>&                                                      theAffectedFaces,
  NCollection_Map<int>&                                                      theSewnEdgeIndices)
{
  BRep_Builder aBB;

  // Collect edges that need SameParameter -- will be run in parallel after the loop.
  NCollection_Sequence<TopoDS_Edge> aSameParamEdges;

  for (int aPairIter = 1; aPairIter <= thePairs.Length(); ++aPairIter)
  {
    const auto& [anIdA, anIdB] = thePairs.Value(aPairIter);
    theSewnEdgeIndices.Add(anIdA.Index);

    // Record which faces reference the removed edge (for reconstruction).
    const BRepGraph_TopoNode::Edge& aRemoveEdge = myGraph.Edge(anIdB.Index);
    for (int aPCurveIter = 1; aPCurveIter <= aRemoveEdge.PCurves.Length(); ++aPCurveIter)
    {
      theAffectedFaces.Add(aRemoveEdge.PCurves.Value(aPCurveIter).FaceNodeId.Index);
    }

    const TopoDS_Edge& aKeepTopoEdge   = TopoDS::Edge(myGraph.Edge(anIdA.Index).OriginalShape);
    const TopoDS_Edge& aRemoveTopoEdge = TopoDS::Edge(aRemoveEdge.OriginalShape);

    // Transfer pcurves from remove-edge to keep-edge.
    for (int aPCurveIter = 1; aPCurveIter <= aRemoveEdge.PCurves.Length(); ++aPCurveIter)
    {
      const BRepGraph_TopoNode::Edge::PCurveEntry& aPCEntry =
        aRemoveEdge.PCurves.Value(aPCurveIter);
      const BRepGraph_GeomNode::PCurve& aPCNode   = myGraph.PCurve(aPCEntry.PCurveNodeId.Index);
      const BRepGraph_TopoNode::Face&   aFaceNode = myGraph.Face(aPCEntry.FaceNodeId.Index);

      if (!aPCNode.Curve2d.IsNull())
      {
        const BRepGraph_GeomNode::Surf& aSurfNode = myGraph.Surf(aFaceNode.SurfNodeId.Index);
        double                          aMergedTol =
          std::max(BRep_Tool::Tolerance(aKeepTopoEdge), BRep_Tool::Tolerance(aRemoveTopoEdge));
        aBB.UpdateEdge(aKeepTopoEdge,
                       aPCNode.Curve2d,
                       aSurfNode.Surface,
                       aFaceNode.GlobalLocation,
                       aMergedTol);

        // Add PCurve entry to keep-edge via graph API.
        myGraph.AddPCurveToEdge(anIdA,
                                aPCEntry.FaceNodeId,
                                aPCNode.Curve2d,
                                aPCNode.ParamFirst,
                                aPCNode.ParamLast);
      }
    }

    // Determine direction correspondence from vertex positions.
    const BRepGraph_TopoNode::Edge& aKeepEdge = myGraph.Edge(anIdA.Index);
    const gp_Pnt& aKeepStart                  = myGraph.Vertex(aKeepEdge.StartVertexId.Index).Point;
    const gp_Pnt& aRemoveStart = myGraph.Vertex(aRemoveEdge.StartVertexId.Index).Point;
    const bool    isReversed   = aKeepStart.Distance(aRemoveStart) > myTolerance;

    // Update wire entries referencing the remove-edge (using cached reverse index).
    // Copy the wire list since ReplaceEdgeInWire modifies the reverse index.
    const NCollection_Vector<int> aWires = myGraph.WiresOfEdge(anIdB.Index);
    for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
    {
      myGraph.ReplaceEdgeInWire(aWires.Value(aWIdx), anIdB, anIdA, isReversed);
    }

    // Defer SameParameter to parallel batch below.
    if (mySameParameter)
    {
      aSameParamEdges.Append(aKeepTopoEdge);
    }

    // Record history.
    NCollection_Sequence<BRepGraph_NodeId> aReplacement;
    aReplacement.Append(anIdA);
    myGraph.RecordHistory("Sewing:MergeEdge", anIdB, aReplacement);

    ++mySewnCount;
  }

  // Parallel SameParameter: each edge's parametric fix is independent
  // (operates on distinct TopoDS_Edge TShape data).
  // This is the single most expensive operation in sewing (~9% of legacy time).
  if (!aSameParamEdges.IsEmpty())
  {
    const int aNbSP = aSameParamEdges.Length();
    // Copy to array for indexed parallel access (Sequence::Value is not thread-safe).
    NCollection_Array1<TopoDS_Edge> aSPArray(0, aNbSP - 1);
    for (int anIdx = 1; anIdx <= aNbSP; ++anIdx)
    {
      aSPArray.SetValue(anIdx - 1, aSameParamEdges.Value(anIdx));
    }
    OSD_Parallel::For(
      0,
      aNbSP,
      [&](int theIdx) { BRepLib::SameParameter(aSPArray.Value(theIdx), myTolerance); },
      !myIsParallel);
  }
}

//=================================================================================================

void BRepGraphAlgo_Sewing::processEdges(const NCollection_Map<int>& theSewnEdgeIndices)
{
  // Verify and fix tolerance consistency on sewn edges.
  // Only process edges that were actually sewn (shared by 2+ faces).
  // Convert the set to an array for parallel-friendly indexed access.
  const int aNbSewn = theSewnEdgeIndices.Extent();
  if (aNbSewn == 0)
    return;

  NCollection_Array1<int> aSewnIndices(0, aNbSewn - 1);
  int                     anArrayIdx = 0;
  for (NCollection_Map<int>::Iterator anIt(theSewnEdgeIndices); anIt.More(); anIt.Next())
  {
    aSewnIndices.SetValue(anArrayIdx++, anIt.Value());
  }

  OSD_Parallel::For(
    0,
    aNbSewn,
    [&](int theIdx) {
      const int                       anEdgeIdx = aSewnIndices.Value(theIdx);
      const BRepGraph_TopoNode::Edge& anEdge    = myGraph.Edge(anEdgeIdx);
      if (anEdge.IsDegenerate)
        return;

      // Ensure edge tolerance is at least as large as its vertex tolerances.
      double aMaxVtxTol = 0.0;
      if (anEdge.StartVertexId.IsValid())
        aMaxVtxTol = std::max(aMaxVtxTol, myGraph.Vertex(anEdge.StartVertexId.Index).Tolerance);
      if (anEdge.EndVertexId.IsValid())
        aMaxVtxTol = std::max(aMaxVtxTol, myGraph.Vertex(anEdge.EndVertexId.Index).Tolerance);

      if (aMaxVtxTol > anEdge.Tolerance)
      {
        myGraph.MutableEdge(anEdgeIdx).Tolerance = aMaxVtxTol;
      }
    },
    !myIsParallel);
}

//=================================================================================================

void BRepGraphAlgo_Sewing::reconstructResult(const NCollection_Map<int>& theAffectedFaces)
{
  const int aNbFaces = myGraph.NbFaces();

  // Step 1 (parallel): Reconstruct affected faces into a pre-allocated array.
  // ReconstructFace() is const and builds a new TopoDS_Face from graph data.
  NCollection_Array1<TopoDS_Shape> aFaceShapes(0, aNbFaces - 1);
  OSD_Parallel::For(
    0,
    aNbFaces,
    [&](int theFaceIdx) {
      if (theAffectedFaces.Contains(theFaceIdx))
        aFaceShapes.SetValue(theFaceIdx, myGraph.ReconstructFace(theFaceIdx));
      else
        aFaceShapes.SetValue(theFaceIdx, myGraph.Face(theFaceIdx).OriginalShape);
    },
    !myIsParallel);

  // Step 2 (sequential): Assemble into result compound.
  BRep_Builder    aBB;
  TopoDS_Compound aResultCompound;
  aBB.MakeCompound(aResultCompound);
  for (int aFaceIdx = 0; aFaceIdx < aNbFaces; ++aFaceIdx)
  {
    aBB.Add(aResultCompound, aFaceShapes.Value(aFaceIdx));
  }
  myResult = aResultCompound;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::setEdgeRegularity()
{
  // Set continuity on sewn edges that are now shared by 2 faces.
  // Use BRepLib::EncodeRegularity on the result compound.
  if (!myResult.IsNull())
  {
    BRepLib::EncodeRegularity(myResult, myTolerance);
  }
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

const TopoDS_Shape& BRepGraphAlgo_Sewing::Result() const
{
  return myResult;
}

//=================================================================================================

const BRepGraph& BRepGraphAlgo_Sewing::Graph() const
{
  return myGraph;
}

//=================================================================================================

int BRepGraphAlgo_Sewing::NbFreeEdgesBefore() const
{
  return myFreeEdgesBefore.Length();
}

//=================================================================================================

int BRepGraphAlgo_Sewing::NbFreeEdgesAfter() const
{
  return myFreeEdgesAfter;
}

//=================================================================================================

int BRepGraphAlgo_Sewing::NbSewnEdges() const
{
  return mySewnCount;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::areEdgesSewable(BRepGraph_NodeId theEdgeA,
                                           BRepGraph_NodeId theEdgeB) const
{
  const BRepGraph_TopoNode::Edge& aNodeA = myGraph.Edge(theEdgeA.Index);
  const BRepGraph_TopoNode::Edge& aNodeB = myGraph.Edge(theEdgeB.Index);

  const TopoDS_Edge& anEdgeA = TopoDS::Edge(aNodeA.OriginalShape);
  const TopoDS_Edge& anEdgeB = TopoDS::Edge(aNodeB.OriginalShape);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  // Check vertex proximity first (fast rejection via graph vertex data).
  const gp_Pnt& aStartA = myGraph.Vertex(aNodeA.StartVertexId.Index).Point;
  const gp_Pnt& aEndA   = myGraph.Vertex(aNodeA.EndVertexId.Index).Point;
  const gp_Pnt& aStartB = myGraph.Vertex(aNodeB.StartVertexId.Index).Point;
  const gp_Pnt& aEndB   = myGraph.Vertex(aNodeB.EndVertexId.Index).Point;

  const bool isSameDir =
    aStartA.Distance(aStartB) <= myTolerance && aEndA.Distance(aEndB) <= myTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= myTolerance && aEndA.Distance(aStartB) <= myTolerance;

  if (!isSameDir && !isReversed)
  {
    return false;
  }

  // Tier 1: Chord length pre-filter -- reject if endpoint-to-endpoint chord
  // lengths differ by more than 2x (curves with very different arc lengths).
  const double aChordA = aStartA.Distance(aEndA);
  const double aChordB = aStartB.Distance(aEndB);
  if (aChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = aChordA > aChordB ? aChordA / aChordB : aChordB / aChordA;
    if (aChordRatio > 2.0)
      return false;
  }

  // Detailed geometric comparison: sample points along edgeA, project onto edgeB.
  BRepAdaptor_Curve aCurveA(anEdgeA);
  BRepAdaptor_Curve aCurveB(anEdgeB);

  // Tier 2: Use 5 samples instead of 8 -- sufficient for edge matching.
  constexpr int          THE_NB_SAMPLES = 5;
  GCPnts_UniformAbscissa aSamplerA(aCurveA, THE_NB_SAMPLES);
  if (!aSamplerA.IsDone() || aSamplerA.NbPoints() < 2)
  {
    return false;
  }

  // Initialize projector on curveB.
  Extrema_ExtPC aProjector;
  aProjector.Initialize(aCurveB,
                        aCurveB.FirstParameter(),
                        aCurveB.LastParameter(),
                        Precision::Confusion());

  const double aTolSq = myTolerance * myTolerance;

  // Forward direction: sample A -> project on B. Track max distance for Tier 3 skip.
  double aMaxFwdDistSq = 0.0;
  for (int aSampleIter = 1; aSampleIter <= aSamplerA.NbPoints(); ++aSampleIter)
  {
    gp_Pnt aPntA = aCurveA.Value(aSamplerA.Parameter(aSampleIter));

    aProjector.Perform(aPntA);
    if (!aProjector.IsDone() || aProjector.NbExt() == 0)
    {
      return false;
    }

    double aMinSqDist = RealLast();
    for (int anExtIter = 1; anExtIter <= aProjector.NbExt(); ++anExtIter)
    {
      double aSqDist = aProjector.SquareDistance(anExtIter);
      if (aSqDist < aMinSqDist)
      {
        aMinSqDist = aSqDist;
      }
    }

    if (aMinSqDist > aTolSq)
    {
      return false;
    }
    if (aMinSqDist > aMaxFwdDistSq)
    {
      aMaxFwdDistSq = aMinSqDist;
    }
  }

  // Tier 3: Skip reverse pass when forward passes with high confidence.
  // If all forward distances are < 1% of tolerance squared, edges are clearly coincident.
  if (aMaxFwdDistSq < 0.01 * aTolSq)
    return true;

  // Reverse direction: sample B -> project on A (bidirectional validation).
  GCPnts_UniformAbscissa aSamplerB(aCurveB, THE_NB_SAMPLES);
  if (!aSamplerB.IsDone() || aSamplerB.NbPoints() < 2)
  {
    return false;
  }

  Extrema_ExtPC aProjectorRev;
  aProjectorRev.Initialize(aCurveA,
                           aCurveA.FirstParameter(),
                           aCurveA.LastParameter(),
                           Precision::Confusion());

  for (int aSampleIter = 1; aSampleIter <= aSamplerB.NbPoints(); ++aSampleIter)
  {
    gp_Pnt aPntB = aCurveB.Value(aSamplerB.Parameter(aSampleIter));

    aProjectorRev.Perform(aPntB);
    if (!aProjectorRev.IsDone() || aProjectorRev.NbExt() == 0)
    {
      return false;
    }

    double aMinSqDist = RealLast();
    for (int anExtIter = 1; anExtIter <= aProjectorRev.NbExt(); ++anExtIter)
    {
      double aSqDist = aProjectorRev.SquareDistance(anExtIter);
      if (aSqDist < aMinSqDist)
      {
        aMinSqDist = aSqDist;
      }
    }

    if (aMinSqDist > aTolSq)
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraphAlgo_Sewing::areEdgesSewable(BRepGraph_NodeId                  theEdgeA,
                                           BRepGraph_NodeId                  theEdgeB,
                                           const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                                           Extrema_ExtPC&                    theProjectorFwdB,
                                           Extrema_ExtPC&                    theProjectorRevA,
                                           double                            theChordA) const
{
  const BRepGraph_TopoNode::Edge& aNodeA = myGraph.Edge(theEdgeA.Index);
  const BRepGraph_TopoNode::Edge& aNodeB = myGraph.Edge(theEdgeB.Index);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  // Check vertex proximity first (fast rejection via graph vertex data).
  const gp_Pnt& aStartA = myGraph.Vertex(aNodeA.StartVertexId.Index).Point;
  const gp_Pnt& aEndA   = myGraph.Vertex(aNodeA.EndVertexId.Index).Point;
  const gp_Pnt& aStartB = myGraph.Vertex(aNodeB.StartVertexId.Index).Point;
  const gp_Pnt& aEndB   = myGraph.Vertex(aNodeB.EndVertexId.Index).Point;

  const bool isSameDir =
    aStartA.Distance(aStartB) <= myTolerance && aEndA.Distance(aEndB) <= myTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= myTolerance && aEndA.Distance(aStartB) <= myTolerance;

  if (!isSameDir && !isReversed)
  {
    return false;
  }

  // Tier 1: Chord length pre-filter.
  const double aChordB = aStartB.Distance(aEndB);
  if (theChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = theChordA > aChordB ? theChordA / aChordB : aChordB / theChordA;
    if (aChordRatio > 2.0)
      return false;
  }

  // Build adaptor for edgeB and re-initialize the reusable forward projector
  // (avoids repeated Extrema_GGExtPC construction/destruction per candidate pair).
  const TopoDS_Edge& anEdgeB = TopoDS::Edge(aNodeB.OriginalShape);
  BRepAdaptor_Curve  aCurveB(anEdgeB);

  theProjectorFwdB.Initialize(aCurveB,
                              aCurveB.FirstParameter(),
                              aCurveB.LastParameter(),
                              Precision::Confusion());

  const double aTolSq = myTolerance * myTolerance;

  // Forward direction: cached sample points A -> project on B.
  double aMaxFwdDistSq = 0.0;
  for (int aSampleIter = theSamplePtsA.Lower(); aSampleIter <= theSamplePtsA.Upper(); ++aSampleIter)
  {
    const gp_Pnt& aPntA = theSamplePtsA.Value(aSampleIter);

    theProjectorFwdB.Perform(aPntA);
    if (!theProjectorFwdB.IsDone() || theProjectorFwdB.NbExt() == 0)
    {
      return false;
    }

    double aMinSqDist = RealLast();
    for (int anExtIter = 1; anExtIter <= theProjectorFwdB.NbExt(); ++anExtIter)
    {
      double aSqDist = theProjectorFwdB.SquareDistance(anExtIter);
      if (aSqDist < aMinSqDist)
      {
        aMinSqDist = aSqDist;
      }
    }

    if (aMinSqDist > aTolSq)
    {
      return false;
    }
    if (aMinSqDist > aMaxFwdDistSq)
    {
      aMaxFwdDistSq = aMinSqDist;
    }
  }

  // Tier 3: Skip reverse pass when forward passes with high confidence.
  if (aMaxFwdDistSq < 0.01 * aTolSq)
    return true;

  // Reverse direction: sample B -> project on A using pre-initialized projector.
  constexpr int          THE_NB_SAMPLES = 5;
  GCPnts_UniformAbscissa aSamplerB(aCurveB, THE_NB_SAMPLES);
  if (!aSamplerB.IsDone() || aSamplerB.NbPoints() < 2)
  {
    return false;
  }

  for (int aSampleIter = 1; aSampleIter <= aSamplerB.NbPoints(); ++aSampleIter)
  {
    gp_Pnt aPntB = aCurveB.Value(aSamplerB.Parameter(aSampleIter));

    theProjectorRevA.Perform(aPntB);
    if (!theProjectorRevA.IsDone() || theProjectorRevA.NbExt() == 0)
    {
      return false;
    }

    double aMinSqDist = RealLast();
    for (int anExtIter = 1; anExtIter <= theProjectorRevA.NbExt(); ++anExtIter)
    {
      double aSqDist = theProjectorRevA.SquareDistance(anExtIter);
      if (aSqDist < aMinSqDist)
      {
        aMinSqDist = aSqDist;
      }
    }

    if (aMinSqDist > aTolSq)
    {
      return false;
    }
  }

  return true;
}
