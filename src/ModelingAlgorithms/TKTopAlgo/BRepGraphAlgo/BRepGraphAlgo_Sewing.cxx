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
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRepGraph_Analyze.hxx>

#include <algorithm>
#include <functional>

namespace
{
constexpr int    THE_NB_EDGE_MATCH_SAMPLES = 5;    // sample count for edge geometric matching
constexpr double THE_MAX_CHORD_RATIO       = 2.0;  // maximum chord-length ratio for sewable edges
constexpr double THE_HIGH_CONFIDENCE_RATIO = 0.01; // forward-distance threshold to skip reverse pass
constexpr int    THE_INIT_VECTOR_CAPACITY  = 256;  // initial capacity for scratch vectors
} // anonymous namespace

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
    for (int aShapeIter = 0; aShapeIter < myInputShapes.Length(); ++aShapeIter)
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

  // Phase 4a: Cut edges at intersections (if enabled).
  // Run BEFORE detectCandidates so sub-edges get proper candidate rel-edges.
  if (myCutting)
  {
    cutAtIntersections(aTmpAllocator);
    aTmpAllocator->Reset(false);
  }

  // Phase 4b: Detect sewing candidates via BBox overlap.
  detectCandidates(aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 5a: Match free edge pairs.
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
    matchFreeEdges(aTmpAllocator);
  aTmpAllocator->Reset(false);

  if (aMatchedPairs.IsEmpty())
  {
    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int aShapeIter = 0; aShapeIter < myInputShapes.Length(); ++aShapeIter)
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

  // Compute free edges after sewing by re-analyzing the graph state.
  // Using the formula is incorrect in non-manifold mode; query the graph directly.
  const NCollection_Vector<BRepGraph_NodeId> aFreeEdgesAfter =
    BRepGraph_Analyze::FreeEdges(myGraph);
  myFreeEdgesAfter = aFreeEdgesAfter.Length();

  myIsDone = true;
}

//=================================================================================================

void BRepGraphAlgo_Sewing::analyzeFaces(const Handle(NCollection_IncAllocator)& theAllocator)
{
  // Assemble input into a compound and build the graph.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int aShapeIter = 0; aShapeIter < myInputShapes.Length(); ++aShapeIter)
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
  // FreeEdges() returns NCollection_Vector (0-based); convert to Array1 for thread-safe
  // random access in parallel phases.
  NCollection_Vector<BRepGraph_NodeId> aFreeVec = BRepGraph_Analyze::FreeEdges(myGraph);
  const int                            aNbFree  = aFreeVec.Length();
  if (aNbFree == 0)
  {
    myFreeEdgesBefore = NCollection_Array1<BRepGraph_NodeId>();
    return;
  }
  NCollection_Array1<BRepGraph_NodeId> anArray(1, aNbFree);
  for (int anIdx = 0; anIdx < aNbFree; ++anIdx)
  {
    anArray.SetValue(anIdx + 1, aFreeVec.Value(anIdx));
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
  NCollection_Vector<int> aFreeVertexIndices(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Map<int>      aFreeVertexSet(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= myFreeEdgesBefore.Length(); ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      myGraph.EdgeDefinition(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexDefId.IsValid() && aFreeVertexSet.Add(anEdge.StartVertexDefId.Index))
    {
      aFreeVertexIndices.Append(anEdge.StartVertexDefId.Index);
    }
    if (anEdge.EndVertexDefId.IsValid() && aFreeVertexSet.Add(anEdge.EndVertexDefId.Index))
    {
      aFreeVertexIndices.Append(anEdge.EndVertexDefId.Index);
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
  for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
  {
    const int anIdx = aFreeVertexIndices.Value(aVtxIter);
    aVertexPoints.SetValue(aVtxIter, myGraph.VertexDefinition(anIdx).Point);
    aGraphIndices.SetValue(aVtxIter, anIdx);
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
      const double aTolI = myGraph.VertexDefinition(anIdxI).Tolerance;
      const double aTolJ = myGraph.VertexDefinition(anIdxJ).Tolerance;
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

  // Union-find collapse: ensure transitive chains are resolved.
  // e.g., if A->B and B->C, make A->C so all N-1 vertices in a cluster merge to one.
  bool aChanged = true;
  while (aChanged)
  {
    aChanged = false;
    for (NCollection_DataMap<int, int>::Iterator anIt(aVertexMerge); anIt.More(); anIt.Next())
    {
      int aTarget = anIt.Value();
      if (aVertexMerge.IsBound(aTarget))
      {
        anIt.ChangeValue() = aVertexMerge.Find(aTarget);
        aChanged = true;
      }
    }
  }

  // Apply vertex merges to free edges.
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= myFreeEdgesBefore.Length(); ++aFreeEdgeIter)
  {
    BRepGraph_TopoNode::EdgeDef& anEdge =
      myGraph.MutableEdgeDefinition(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexDefId.IsValid() && aVertexMerge.IsBound(anEdge.StartVertexDefId.Index))
    {
      anEdge.StartVertexDefId =
        BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVertexMerge.Find(anEdge.StartVertexDefId.Index));
    }
    if (anEdge.EndVertexDefId.IsValid() && aVertexMerge.IsBound(anEdge.EndVertexDefId.Index))
    {
      anEdge.EndVertexDefId =
        BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVertexMerge.Find(anEdge.EndVertexDefId.Index));
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
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aPairs(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

  // Common lambda for the KDTree search + BBox refinement per edge.
  const auto aSearchFn =
    [&](int theIdxI, NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs) {
      const int      anI   = theIdxI + 1;
      const Bnd_Box& aBoxI = aBBoxes.Value(anI);
      if (aBoxI.IsVoid())
      {
        return;
      }

      const double aHalfDiagI    = aHalfDiags.Value(theIdxI);
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

void BRepGraphAlgo_Sewing::cutAtIntersections(
  const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  const int aNbFreeEdges = myFreeEdgesBefore.Length();
  if (aNbFreeEdges == 0)
    return;

  // Step 1: Collect all unique free-edge endpoint vertex indices.
  NCollection_Map<int>    aFreeVtxSet(aNbFreeEdges * 2, theTmpAlloc);
  NCollection_Vector<int> aFreeVtxList(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      myGraph.EdgeDefinition(myFreeEdgesBefore.Value(aFreeEdgeIter).Index);
    if (anEdge.StartVertexDefId.IsValid() && aFreeVtxSet.Add(anEdge.StartVertexDefId.Index))
      aFreeVtxList.Append(anEdge.StartVertexDefId.Index);
    if (anEdge.EndVertexDefId.IsValid() && aFreeVtxSet.Add(anEdge.EndVertexDefId.Index))
      aFreeVtxList.Append(anEdge.EndVertexDefId.Index);
  }

  const int aNbVtx = aFreeVtxList.Length();
  if (aNbVtx == 0)
    return;

  // Pre-compute vertex points for fast access.
  NCollection_Array1<gp_Pnt> aVtxPoints(0, aNbVtx - 1);
  for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
  {
    const int aVtxIdx = aFreeVtxList.Value(aVtxIter);
    aVtxPoints.SetValue(aVtxIter, myGraph.VertexDefinition(aVtxIdx).Point);
  }

  // Per-edge result: list of sub-edge node ids after splitting (empty = no split).
  struct SplitCandidate
  {
    double Param;
    int    VtxIdx;
  };

  // aSubEdgeChains[i] holds sub-edge chain for free edge (i+1); empty means no split.
  NCollection_Vector<NCollection_Vector<BRepGraph_NodeId>> aSubEdgeChains(
    THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  for (int anIdx = 0; anIdx < aNbFreeEdges; ++anIdx)
    aSubEdgeChains.Append(NCollection_Vector<BRepGraph_NodeId>());

  bool anySplits = false;

  // Step 2: For each free edge, find T-vertex split candidates.
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_NodeId             anEdgeId = myFreeEdgesBefore.Value(aFreeEdgeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdge   = myGraph.EdgeDefinition(anEdgeId.Index);

    if (anEdge.IsDegenerate || !anEdge.CurveNodeId.IsValid())
      continue;
    if (!myGraph.HasOriginalShape(anEdgeId))
      continue;

    // Own endpoint indices to exclude from T-vertex candidates.
    const int aStartIdx = anEdge.StartVertexDefId.IsValid() ? anEdge.StartVertexDefId.Index : -1;
    const int aEndIdx   = anEdge.EndVertexDefId.IsValid()   ? anEdge.EndVertexDefId.Index   : -1;

    // Build BRepAdaptor_Curve on the original edge for projection.
    const TopoDS_Edge& anEdgeShape = TopoDS::Edge(myGraph.OriginalOf(anEdgeId));
    BRepAdaptor_Curve  aCurve(anEdgeShape);

    Extrema_ExtPC aProjector;
    aProjector.Initialize(aCurve,
                          aCurve.FirstParameter(),
                          aCurve.LastParameter(),
                          Precision::Confusion());

    // Get edge bounding box (enlarged by tolerance) for candidate pre-filtering.
    Bnd_Box aEdgeBBox = myGraph.BoundingBox(anEdgeId);
    if (aEdgeBBox.IsVoid())
      continue;
    aEdgeBBox.Enlarge(myTolerance);

    const double aTolSq     = myTolerance * myTolerance;
    const double aParamFirst = aCurve.FirstParameter();
    const double aParamLast  = aCurve.LastParameter();
    const double aParamEps   = Precision::Confusion();

    NCollection_Vector<SplitCandidate> aSplits(4, theTmpAlloc);

    for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
    {
      const int aVtxIdx = aFreeVtxList.Value(aVtxIter);
      if (aVtxIdx == aStartIdx || aVtxIdx == aEndIdx)
        continue;

      const gp_Pnt& aVtxPnt = aVtxPoints.Value(aVtxIter);

      // Quick pre-check: vertex must be inside the enlarged edge bbox.
      if (aEdgeBBox.IsOut(aVtxPnt))
        continue;

      aProjector.Perform(aVtxPnt);
      if (!aProjector.IsDone())
        continue;

      // Find closest extremal point.
      double aMinSqDist = RealLast();
      double aMinParam  = 0.0;
      for (int aExtIdx = 1; aExtIdx <= aProjector.NbExt(); ++aExtIdx)
      {
        const double aSqDist = aProjector.SquareDistance(aExtIdx);
        if (aSqDist < aMinSqDist)
        {
          aMinSqDist = aSqDist;
          aMinParam  = aProjector.Point(aExtIdx).Parameter();
        }
      }

      // Accept if within tolerance AND strictly inside the parameter range.
      if (aMinSqDist <= aTolSq
          && aMinParam > aParamFirst + aParamEps
          && aMinParam < aParamLast  - aParamEps)
      {
        SplitCandidate aCand;
        aCand.Param  = aMinParam;
        aCand.VtxIdx = aVtxIdx;
        aSplits.Append(aCand);
      }
    }

    if (aSplits.IsEmpty())
      continue;

    // Sort candidates by parameter (ascending).
    const int aNbSplits = aSplits.Length();
    NCollection_Array1<SplitCandidate> aSortedSplits(0, aNbSplits - 1);
    for (int aSplitIter = 0; aSplitIter < aNbSplits; ++aSplitIter)
      aSortedSplits.SetValue(aSplitIter, aSplits.Value(aSplitIter));
    std::sort(&aSortedSplits.ChangeFirst(),
              &aSortedSplits.ChangeFirst() + aNbSplits,
              [](const SplitCandidate& theA, const SplitCandidate& theB) {
                return theA.Param < theB.Param;
              });

    // Deduplicate: remove candidates with parameters too close together.
    NCollection_Vector<SplitCandidate> aUniqueSplits(4, theTmpAlloc);
    double                             aPrevParam = aParamFirst - 1.0;
    for (int aSplitIter = 0; aSplitIter < aNbSplits; ++aSplitIter)
    {
      const SplitCandidate& aCand = aSortedSplits.Value(aSplitIter);
      if (std::abs(aCand.Param - aPrevParam) > aParamEps)
      {
        aUniqueSplits.Append(aCand);
        aPrevParam = aCand.Param;
      }
    }

    if (aUniqueSplits.IsEmpty())
      continue;

    // Apply splits: chain SplitEdge calls along the edge.
    anySplits = true;
    NCollection_Vector<BRepGraph_NodeId>& aChain = aSubEdgeChains.ChangeValue(aFreeEdgeIter - 1);
    BRepGraph_NodeId aCurrentEdge = anEdgeId;
    for (int aSplitIter = 0; aSplitIter < aUniqueSplits.Length(); ++aSplitIter)
    {
      const SplitCandidate& aCand = aUniqueSplits.Value(aSplitIter);
      const BRepGraph_NodeId aVtxNodeId(BRepGraph_NodeKind::Vertex, aCand.VtxIdx);
      BRepGraph_NodeId       aSubA, aSubB;
      myGraph.SplitEdge(aCurrentEdge, aVtxNodeId, aCand.Param, aSubA, aSubB);
      aChain.Append(aSubA);
      aCurrentEdge = aSubB;
    }
    aChain.Append(aCurrentEdge);  // final sub-edge (last SubB)
  }

  if (!anySplits)
    return;

  // Step 3: Rebuild myFreeEdgesBefore with sub-edge chains replacing split edges.
  NCollection_Vector<BRepGraph_NodeId> aNewFreeVec(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const NCollection_Vector<BRepGraph_NodeId>& aChain =
      aSubEdgeChains.Value(aFreeEdgeIter - 1);
    if (aChain.IsEmpty())
    {
      // No splits: keep original free edge.
      aNewFreeVec.Append(myFreeEdgesBefore.Value(aFreeEdgeIter));
    }
    else
    {
      // Replace with sub-edge chain.
      for (int aSubIter = 0; aSubIter < aChain.Length(); ++aSubIter)
        aNewFreeVec.Append(aChain.Value(aSubIter));
    }
  }

  // Convert to Array1 for thread-safe random access in subsequent phases.
  const int aNbNewFree = aNewFreeVec.Length();
  NCollection_Array1<BRepGraph_NodeId> aNewFreeArray(1, aNbNewFree);
  for (int anIdx = 0; anIdx < aNbNewFree; ++anIdx)
    aNewFreeArray.SetValue(anIdx + 1, aNewFreeVec.Value(anIdx));
  myFreeEdgesBefore = std::move(aNewFreeArray);
}

//=================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> BRepGraphAlgo_Sewing::
  matchFreeEdges(const Handle(NCollection_IncAllocator)& theTmpAlloc)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;
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
      const BRepGraph_TopoNode::EdgeDef& anEdgeANode = myGraph.EdgeDefinition(anIdA.Index);
      if (anEdgeANode.IsDegenerate)
        return;

      const TopoDS_Edge&     anEdgeAShape = TopoDS::Edge(myGraph.OriginalOf(anIdA));
      BRepAdaptor_Curve      aCurveA(anEdgeAShape);
      GCPnts_UniformAbscissa aSamplerA(aCurveA, THE_NB_EDGE_MATCH_SAMPLES);
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

      const gp_Pnt& aStartA = myGraph.VertexDefinition(anEdgeANode.StartVertexDefId.Index).Point;
      const gp_Pnt& aEndA   = myGraph.VertexDefinition(anEdgeANode.EndVertexDefId.Index).Point;
      const double  aChordA = aStartA.Distance(aEndA);

      myGraph.ForEachOutEdgeOfKind(anIdA, BRepGraph_RelKind::UserDefined, [&](const BRepGraph_RelEdge& aRelEdge) {
        const BRepGraph_NodeId   anIdB    = aRelEdge.Target;

        if (!aFreeEdgeIndices.Contains(anIdB.Index))
          return;

        // Full geometric validation using cached edgeA data.
        if (areEdgesSewable(anIdA, anIdB, aSamplePtsA, aProjectorFwdB, aProjectorRev, aChordA))
        {
          const BRepGraph_TopoNode::EdgeDef& anEdgeB = myGraph.EdgeDefinition(anIdB.Index);
          const gp_Pnt& aStartB = myGraph.VertexDefinition(anEdgeB.StartVertexDefId.Index).Point;
          const gp_Pnt& aEndB   = myGraph.VertexDefinition(anEdgeB.EndVertexDefId.Index).Point;

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
  NCollection_Vector<ScoredPair> aScoredPairs;
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
    for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
    {
      aSortedPairs.SetValue(anIdx, aScoredPairs.Value(anIdx));
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
  const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs,
  NCollection_Map<int>&                                                      theAffectedFaces,
  NCollection_Map<int>&                                                      theSewnEdgeIndices)
{
  BRep_Builder aBB;

  // Collect edges that need SameParameter -- will be run in parallel after the loop.
  NCollection_Vector<TopoDS_Edge> aSameParamEdges;

  for (int aPairIter = 0; aPairIter < thePairs.Length(); ++aPairIter)
  {
    const auto& [anIdA, anIdB] = thePairs.Value(aPairIter);
    theSewnEdgeIndices.Add(anIdA.Index);

    // Record which faces reference the removed edge (for reconstruction).
    const BRepGraph_TopoNode::EdgeDef& aRemoveEdge = myGraph.EdgeDefinition(anIdB.Index);
    for (int aPCurveIter = 0; aPCurveIter < aRemoveEdge.PCurves.Length(); ++aPCurveIter)
    {
      theAffectedFaces.Add(aRemoveEdge.PCurves.Value(aPCurveIter).FaceDefId.Index);
    }

    const TopoDS_Edge& aKeepTopoEdge   = TopoDS::Edge(myGraph.OriginalOf(anIdA));
    const TopoDS_Edge& aRemoveTopoEdge = TopoDS::Edge(myGraph.OriginalOf(anIdB));

    // Transfer pcurves from remove-edge to keep-edge.
    for (int aPCurveIter = 0; aPCurveIter < aRemoveEdge.PCurves.Length(); ++aPCurveIter)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
        aRemoveEdge.PCurves.Value(aPCurveIter);
      const BRepGraph_GeomNode::PCurve& aPCNode   = myGraph.PCurveNode(aPCEntry.PCurveNodeId.Index);
      const BRepGraph_TopoNode::FaceDef&   aFaceNode = myGraph.FaceDefinition(aPCEntry.FaceDefId.Index);

      if (!aPCNode.Curve2d.IsNull())
      {
        const BRepGraph_GeomNode::Surf& aSurfNode = myGraph.SurfNode(aFaceNode.SurfNodeId.Index);
        double                          aMergedTol =
          std::max(BRep_Tool::Tolerance(aKeepTopoEdge), BRep_Tool::Tolerance(aRemoveTopoEdge));
        TopLoc_Location aFaceLoc(myGraph.GlobalTransform(aPCEntry.FaceDefId));
        aBB.UpdateEdge(aKeepTopoEdge,
                       aPCNode.Curve2d,
                       aSurfNode.Surface,
                       aFaceLoc,
                       aMergedTol);

        // Add PCurve entry to keep-edge via graph API; preserve original edge orientation
        // so that seam edges (REVERSED orientation) are correctly reconstructed as C2.
        myGraph.AddPCurveToEdge(anIdA,
                                aPCEntry.FaceDefId,
                                aPCNode.Curve2d,
                                aPCNode.ParamFirst,
                                aPCNode.ParamLast,
                                aPCEntry.EdgeOrientation);
      }
    }

    // Determine direction correspondence from vertex positions.
    const BRepGraph_TopoNode::EdgeDef& aKeepEdge = myGraph.EdgeDefinition(anIdA.Index);
    const gp_Pnt& aKeepStart                  = myGraph.VertexDefinition(aKeepEdge.StartVertexDefId.Index).Point;
    const gp_Pnt& aRemoveStart = myGraph.VertexDefinition(aRemoveEdge.StartVertexDefId.Index).Point;
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
    NCollection_Vector<BRepGraph_NodeId> aReplacement;
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
    // Copy to array for indexed parallel access.
    NCollection_Array1<TopoDS_Edge> aSPArray(0, aNbSP - 1);
    for (int anIdx = 0; anIdx < aNbSP; ++anIdx)
    {
      aSPArray.SetValue(anIdx, aSameParamEdges.Value(anIdx));
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
      const BRepGraph_TopoNode::EdgeDef& anEdge    = myGraph.EdgeDefinition(anEdgeIdx);
      if (anEdge.IsDegenerate)
        return;

      // Ensure edge tolerance is at least as large as its vertex tolerances.
      double aMaxVtxTol = 0.0;
      if (anEdge.StartVertexDefId.IsValid())
        aMaxVtxTol = std::max(aMaxVtxTol, myGraph.VertexDefinition(anEdge.StartVertexDefId.Index).Tolerance);
      if (anEdge.EndVertexDefId.IsValid())
        aMaxVtxTol = std::max(aMaxVtxTol, myGraph.VertexDefinition(anEdge.EndVertexDefId.Index).Tolerance);

      if (aMaxVtxTol > anEdge.Tolerance)
      {
        myGraph.MutableEdgeDefinition(anEdgeIdx).Tolerance = aMaxVtxTol;
        myGraph.Invalidate(BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeIdx));
      }
    },
    !myIsParallel);
}

//=================================================================================================

void BRepGraphAlgo_Sewing::reconstructResult(const NCollection_Map<int>& theAffectedFaces)
{
  const int aNbFaces = myGraph.NbFaceDefs();

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
        aFaceShapes.SetValue(theFaceIdx, myGraph.Shape(BRepGraph_NodeId(BRepGraph_NodeKind::Face, theFaceIdx)));
    },
    !myIsParallel);

  // Step 2 (sequential): Reassemble preserving the original hierarchy.
  // Helper lambda: build a shell from its face usages using reconstructed faces.
  BRep_Builder aBB;
  auto buildShell = [&](int theShellUsageIdx) -> TopoDS_Shell
  {
    const BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph.ShellUsageNode(theShellUsageIdx);
    TopoDS_Shell aNewShell;
    aBB.MakeShell(aNewShell);
    for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
    {
      const int aFaceUsageIdx = aShellUsage.FaceUsages.Value(aFaceIter).Index;
      const int aFaceDefIdx   = myGraph.FaceUsageNode(aFaceUsageIdx).DefId.Index;
      aBB.Add(aNewShell, aFaceShapes.Value(aFaceDefIdx));
    }
    aNewShell.Orientation(aShellUsage.Orientation);
    return aNewShell;
  };

  // Helper lambda: build a solid from its shell usages.
  auto buildSolid = [&](int theSolidUsageIdx) -> TopoDS_Solid
  {
    const BRepGraph_TopoNode::SolidUsage& aSolidUsage = myGraph.SolidUsageNode(theSolidUsageIdx);
    TopoDS_Solid aNewSolid;
    aBB.MakeSolid(aNewSolid);
    for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
    {
      const int aShellUsageIdx = aSolidUsage.ShellUsages.Value(aShellIter).Index;
      aBB.Add(aNewSolid, buildShell(aShellUsageIdx));
    }
    aNewSolid.Orientation(aSolidUsage.Orientation);
    return aNewSolid;
  };

  // Recursive lambda: build a compound from its child usages.
  std::function<TopoDS_Compound(int)> buildCompound;
  buildCompound = [&](int theCompUsageIdx) -> TopoDS_Compound
  {
    const BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph.CompoundUsageNode(theCompUsageIdx);
    TopoDS_Compound aNewCompound;
    aBB.MakeCompound(aNewCompound);
    for (int aChildIter = 0; aChildIter < aCompUsage.ChildUsages.Length(); ++aChildIter)
    {
      const BRepGraph_UsageId& aChildId = aCompUsage.ChildUsages.Value(aChildIter);
      switch (aChildId.Kind)
      {
        case BRepGraph_NodeKind::Compound:
          aBB.Add(aNewCompound, buildCompound(aChildId.Index));
          break;
        case BRepGraph_NodeKind::CompSolid:
        {
          const BRepGraph_TopoNode::CompSolidUsage& aCSUsage = myGraph.CompSolidUsageNode(aChildId.Index);
          TopoDS_CompSolid aNewCS;
          aBB.MakeCompSolid(aNewCS);
          for (int aSolIter = 0; aSolIter < aCSUsage.SolidUsages.Length(); ++aSolIter)
          {
            aBB.Add(aNewCS, buildSolid(aCSUsage.SolidUsages.Value(aSolIter).Index));
          }
          aNewCS.Orientation(aCSUsage.Orientation);
          aBB.Add(aNewCompound, aNewCS);
          break;
        }
        case BRepGraph_NodeKind::Solid:
          aBB.Add(aNewCompound, buildSolid(aChildId.Index));
          break;
        case BRepGraph_NodeKind::Shell:
          aBB.Add(aNewCompound, buildShell(aChildId.Index));
          break;
        case BRepGraph_NodeKind::Face:
        {
          const int aFaceDefIdx = myGraph.FaceUsageNode(aChildId.Index).DefId.Index;
          aBB.Add(aNewCompound, aFaceShapes.Value(aFaceDefIdx));
          break;
        }
        default:
          break;
      }
    }
    aNewCompound.Orientation(aCompUsage.Orientation);
    return aNewCompound;
  };

  // Determine which assembly strategy to use based on graph hierarchy.
  // Note: analyzeFaces() always wraps input into a compound, so NbCompoundUsages() >= 1.
  // A root compound with non-empty ChildUsages indicates genuine compound hierarchy;
  // an empty ChildUsages means it was the artificial wrapper around loose faces.
  bool aHasCompoundHierarchy = false;
  if (myGraph.NbCompoundUsages() > 0)
  {
    for (int aCompIdx = 0; aCompIdx < myGraph.NbCompoundUsages(); ++aCompIdx)
    {
      const BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph.CompoundUsageNode(aCompIdx);
      if (!aCompUsage.ParentUsage.IsValid() && aCompUsage.ChildUsages.Length() > 0)
      {
        aHasCompoundHierarchy = true;
        break;
      }
    }
  }

  if (aHasCompoundHierarchy)
  {
    // Find root compound(s) (those with no parent and non-empty children).
    TopoDS_Compound aResultCompound;
    aBB.MakeCompound(aResultCompound);
    int aRootIdx = -1;
    int aNbRoots = 0;
    for (int aCompIdx = 0; aCompIdx < myGraph.NbCompoundUsages(); ++aCompIdx)
    {
      const BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph.CompoundUsageNode(aCompIdx);
      if (!aCompUsage.ParentUsage.IsValid() && aCompUsage.ChildUsages.Length() > 0)
      {
        aRootIdx = aCompIdx;
        ++aNbRoots;
      }
    }
    if (aNbRoots == 1)
    {
      myResult = buildCompound(aRootIdx);
    }
    else
    {
      // Multiple root compounds: wrap them.
      for (int aCompIdx = 0; aCompIdx < myGraph.NbCompoundUsages(); ++aCompIdx)
      {
        const BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph.CompoundUsageNode(aCompIdx);
        if (!aCompUsage.ParentUsage.IsValid() && aCompUsage.ChildUsages.Length() > 0)
          aBB.Add(aResultCompound, buildCompound(aCompIdx));
      }
      myResult = aResultCompound;
    }
  }
  else if (myGraph.NbCompSolidUsages() > 0)
  {
    // CompSolid hierarchy without enclosing compound.
    if (myGraph.NbCompSolidUsages() == 1)
    {
      const BRepGraph_TopoNode::CompSolidUsage& aCSUsage = myGraph.CompSolidUsageNode(0);
      TopoDS_CompSolid aNewCS;
      aBB.MakeCompSolid(aNewCS);
      for (int aSolIter = 0; aSolIter < aCSUsage.SolidUsages.Length(); ++aSolIter)
      {
        aBB.Add(aNewCS, buildSolid(aCSUsage.SolidUsages.Value(aSolIter).Index));
      }
      aNewCS.Orientation(aCSUsage.Orientation);
      myResult = aNewCS;
    }
    else
    {
      TopoDS_Compound aResultCompound;
      aBB.MakeCompound(aResultCompound);
      for (int aCSIdx = 0; aCSIdx < myGraph.NbCompSolidUsages(); ++aCSIdx)
      {
        const BRepGraph_TopoNode::CompSolidUsage& aCSUsage = myGraph.CompSolidUsageNode(aCSIdx);
        TopoDS_CompSolid aNewCS;
        aBB.MakeCompSolid(aNewCS);
        for (int aSolIter = 0; aSolIter < aCSUsage.SolidUsages.Length(); ++aSolIter)
        {
          aBB.Add(aNewCS, buildSolid(aCSUsage.SolidUsages.Value(aSolIter).Index));
        }
        aNewCS.Orientation(aCSUsage.Orientation);
        aBB.Add(aResultCompound, aNewCS);
      }
      myResult = aResultCompound;
    }
  }
  else if (myGraph.NbSolidUsages() > 0)
  {
    // Solid hierarchy without enclosing compound/compsolid.
    if (myGraph.NbSolidUsages() == 1)
    {
      myResult = buildSolid(0);
    }
    else
    {
      TopoDS_Compound aResultCompound;
      aBB.MakeCompound(aResultCompound);
      for (int aSolidIdx = 0; aSolidIdx < myGraph.NbSolidUsages(); ++aSolidIdx)
      {
        aBB.Add(aResultCompound, buildSolid(aSolidIdx));
      }
      myResult = aResultCompound;
    }
  }
  else if (myGraph.NbShellUsages() > 0)
  {
    // Shell hierarchy without enclosing solid.
    if (myGraph.NbShellUsages() == 1)
    {
      myResult = buildShell(0);
    }
    else
    {
      TopoDS_Compound aResultCompound;
      aBB.MakeCompound(aResultCompound);
      for (int aShellIdx = 0; aShellIdx < myGraph.NbShellUsages(); ++aShellIdx)
      {
        aBB.Add(aResultCompound, buildShell(aShellIdx));
      }
      myResult = aResultCompound;
    }
  }
  else
  {
    // No hierarchy: flat compound of faces (typical for loose face input).
    TopoDS_Compound aResultCompound;
    aBB.MakeCompound(aResultCompound);
    for (int aFaceIdx = 0; aFaceIdx < aNbFaces; ++aFaceIdx)
    {
      aBB.Add(aResultCompound, aFaceShapes.Value(aFaceIdx));
    }
    myResult = aResultCompound;
  }
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
  const BRepGraph_TopoNode::EdgeDef& aNodeA = myGraph.EdgeDefinition(theEdgeA.Index);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = myGraph.EdgeDefinition(theEdgeB.Index);

  const TopoDS_Edge& anEdgeA = TopoDS::Edge(myGraph.OriginalOf(theEdgeA));
  const TopoDS_Edge& anEdgeB = TopoDS::Edge(myGraph.OriginalOf(theEdgeB));

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  // Check vertex proximity first (fast rejection via graph vertex data).
  const gp_Pnt& aStartA = myGraph.VertexDefinition(aNodeA.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndA   = myGraph.VertexDefinition(aNodeA.EndVertexDefId.Index).Point;
  const gp_Pnt& aStartB = myGraph.VertexDefinition(aNodeB.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndB   = myGraph.VertexDefinition(aNodeB.EndVertexDefId.Index).Point;

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
    if (aChordRatio > THE_MAX_CHORD_RATIO)
      return false;
  }

  // Detailed geometric comparison: sample points along edgeA, project onto edgeB.
  BRepAdaptor_Curve aCurveA(anEdgeA);
  BRepAdaptor_Curve aCurveB(anEdgeB);

  // Tier 2: Use 5 samples instead of 8 -- sufficient for edge matching.
  GCPnts_UniformAbscissa aSamplerA(aCurveA, THE_NB_EDGE_MATCH_SAMPLES);
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
  if (aMaxFwdDistSq < THE_HIGH_CONFIDENCE_RATIO * aTolSq)
    return true;

  // Reverse direction: sample B -> project on A (bidirectional validation).
  GCPnts_UniformAbscissa aSamplerB(aCurveB, THE_NB_EDGE_MATCH_SAMPLES);
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
  const BRepGraph_TopoNode::EdgeDef& aNodeA = myGraph.EdgeDefinition(theEdgeA.Index);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = myGraph.EdgeDefinition(theEdgeB.Index);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  // Check vertex proximity first (fast rejection via graph vertex data).
  const gp_Pnt& aStartA = myGraph.VertexDefinition(aNodeA.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndA   = myGraph.VertexDefinition(aNodeA.EndVertexDefId.Index).Point;
  const gp_Pnt& aStartB = myGraph.VertexDefinition(aNodeB.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndB   = myGraph.VertexDefinition(aNodeB.EndVertexDefId.Index).Point;

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
    if (aChordRatio > THE_MAX_CHORD_RATIO)
      return false;
  }

  // Build adaptor for edgeB and re-initialize the reusable forward projector
  // (avoids repeated Extrema_GGExtPC construction/destruction per candidate pair).
  const TopoDS_Edge& anEdgeB = TopoDS::Edge(myGraph.OriginalOf(theEdgeB));
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
  if (aMaxFwdDistSq < THE_HIGH_CONFIDENCE_RATIO * aTolSq)
    return true;

  // Reverse direction: sample B -> project on A using pre-initialized projector.
  GCPnts_UniformAbscissa aSamplerB(aCurveB, THE_NB_EDGE_MATCH_SAMPLES);
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
