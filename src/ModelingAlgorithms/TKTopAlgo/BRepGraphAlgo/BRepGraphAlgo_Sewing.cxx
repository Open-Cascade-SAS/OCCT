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
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepGraphAlgo_FaceAnalysis.hxx>
#include <BRepGraphAlgo_SameParameter.hxx>
#include <BRepLib.hxx>
#include <ExtremaPC_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_KDTree.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_OrderedDataMap.hxx>
#include <NCollection_UBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutationGuard.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_MutView.hxx>
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
constexpr int    THE_INIT_VECTOR_CAPACITY      = 256;  // initial capacity for scratch vectors
constexpr double THE_SEAM_SEPARATION_THRESHOLD = 0.25; // minimum midpoint UV separation as fraction of period

// ---------------------------------------------------------------------------
// UBTree selector for vertex bounding boxes (used in cutAtIntersections).
// Each thread creates its own instance, so this is safe in parallel loops.
// ---------------------------------------------------------------------------

class VtxBoxSelector : public NCollection_UBTree<int, Bnd_Box>::Selector
{
public:
  void SetCurrentBox(const Bnd_Box& theBox)
  {
    myBox = theBox;
    myResults.Clear();
  }

  bool Reject(const Bnd_Box& theBox) const override { return myBox.IsOut(theBox); }

  bool Accept(const int& theIdx) override
  {
    myResults.Append(theIdx);
    return true;
  }

  const NCollection_Vector<int>& Results() const { return myResults; }

private:
  Bnd_Box                 myBox;
  NCollection_Vector<int> myResults;
};

// ---------------------------------------------------------------------------
// Unwrap trimmed/offset surface to its basis surface.
// Ported from BRepBuilderAPI_Sewing::IsUClosedSurface (recursive unwrapping).
// ---------------------------------------------------------------------------

Handle(Geom_Surface) basisSurface(const Handle(Geom_Surface)& theSurf)
{
  Handle(Geom_Surface) aSurf = theSurf;
  for (;;)
  {
    if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf)->BasisSurface();
    }
    else if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      aSurf = Handle(Geom_OffsetSurface)::DownCast(aSurf)->BasisSurface();
    }
    else
    {
      return aSurf;
    }
  }
}

// ---------------------------------------------------------------------------
// Isocurve-based closure check. Ported from BRepBuilderAPI_Sewing::IsClosedByIsos.
// Tests if two isocurves at the PCurve endpoints form closed loops.
// ---------------------------------------------------------------------------

bool isClosedByIsos(const Handle(Geom_Surface)& theSurf,
                    const Handle(Geom2d_Curve)& thePCurve,
                    double                       theFirst,
                    double                       theLast,
                    bool                         theUIsos)
{
  if (theSurf.IsNull() || thePCurve.IsNull())
  {
    return false;
  }
  const gp_Pnt2d aPSurf1 =
    thePCurve->IsPeriodic()
      ? thePCurve->Value(theFirst)
      : thePCurve->Value(std::max(theFirst, thePCurve->FirstParameter()));
  const gp_Pnt2d aPSurf2 =
    thePCurve->IsPeriodic()
      ? thePCurve->Value(theLast)
      : thePCurve->Value(std::min(theLast, thePCurve->LastParameter()));

  Handle(Geom_Curve) anIso1, anIso2;
  if (theUIsos)
  {
    anIso1 = theSurf->UIso(aPSurf1.X());
    anIso2 = theSurf->UIso(aPSurf2.X());
  }
  else
  {
    anIso1 = theSurf->VIso(aPSurf1.Y());
    anIso2 = theSurf->VIso(aPSurf2.Y());
  }
  if (anIso1.IsNull() || anIso2.IsNull())
  {
    return false;
  }

  const double aFirst1 = anIso1->FirstParameter();
  const double aLast1  = anIso1->LastParameter();
  const double aFirst2 = anIso2->FirstParameter();
  const double aLast2  = anIso2->LastParameter();
  const gp_Pnt aPt1F = anIso1->EvalD0(aFirst1);
  const gp_Pnt aPt1M = anIso1->EvalD0((aFirst1 + aLast1) * 0.5);
  const gp_Pnt aPt1L = anIso1->EvalD0(aLast1);
  const gp_Pnt aPt2F = anIso2->EvalD0(aFirst2);
  const gp_Pnt aPt2M = anIso2->EvalD0((aFirst2 + aLast2) * 0.5);
  const gp_Pnt aPt2L = anIso2->EvalD0(aLast2);
  // Closed if endpoints are closer than midpoint distance for both isocurves.
  return ((aPt1F.XYZ() - aPt1L.XYZ()).Modulus()
            < (aPt1F.XYZ() - aPt1M.XYZ()).Modulus() - Precision::Confusion())
         && ((aPt2F.XYZ() - aPt2L.XYZ()).Modulus()
             < (aPt2F.XYZ() - aPt2M.XYZ()).Modulus() - Precision::Confusion());
}

// ---------------------------------------------------------------------------
// Check if surface is U-closed or V-closed (with isocurve fallback).
// Ported from BRepBuilderAPI_Sewing::IsUClosedSurface / IsVClosedSurface.
// ---------------------------------------------------------------------------

bool isSurfaceClosedForEdge(const BRepGraph&                           theGraph,
                            int                                        theFaceDefIdx,
                            const BRepGraph_TopoNode::EdgeDef::PCurveEntry& thePCEntry,
                            bool                                       theCheckU)
{
  const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(theFaceDefIdx);
  if (aFace.Surface.IsNull())
  {
    return false;
  }
  const Handle(Geom_Surface) aBasis = basisSurface(aFace.Surface);
  const bool isClosed = theCheckU ? aBasis->IsUClosed() : aBasis->IsVClosed();
  if (isClosed)
  {
    return true;
  }
  // Isocurve fallback: check if isocurves at PCurve endpoints form closed loops.
  if (!thePCEntry.Curve2d.IsNull())
  {
    return isClosedByIsos(aBasis, thePCEntry.Curve2d, thePCEntry.ParamFirst,
                          thePCEntry.ParamLast, !theCheckU);
  }
  return false;
}

// ---------------------------------------------------------------------------
// Seam edge detection: edge is on a UV-closed surface and has 2 PCurves.
// A seam can ONLY exist on a closed surface. This eliminates false positives
// from non-seam edges on planar/non-closed surfaces.
// ---------------------------------------------------------------------------

bool isSeamEdge(const BRepGraph& theGraph, int theEdgeIdx)
{
  const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  // Collect unique FaceDefIds and count PCurve entries per face.
  for (int i = 0; i < anEdge.PCurves.Length(); ++i)
  {
    const int aFaceId = anEdge.PCurves.Value(i).FaceDefId.Index;
    for (int j = i + 1; j < anEdge.PCurves.Length(); ++j)
    {
      if (anEdge.PCurves.Value(j).FaceDefId.Index == aFaceId)
      {
        // Two PCurve entries on the same face — this is a seam candidate.
        // Verify the surface is actually U or V closed.
        if (isSurfaceClosedForEdge(theGraph, aFaceId, anEdge.PCurves.Value(i), true)
            || isSurfaceClosedForEdge(theGraph, aFaceId, anEdge.PCurves.Value(i), false))
        {
          return true;
        }
      }
    }
  }
  return false;
}

// ---------------------------------------------------------------------------
// Check if two edges on the same face can be sewn (seam-like configuration).
// Allowed only if the surface is UV-closed.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Check if two edges on the same UV-closed face are on opposite sides.
// Ported from BRepBuilderAPI_Sewing::IsMergedClosed (lines 1354-1435).
// Uses 2D bounding boxes of PCurves: on a U-closed surface, two V-long edges
// are on opposite sides if their outer U-gap is smaller than their inner U-gap.
// ---------------------------------------------------------------------------

bool canSewSameFaceEdges(const BRepGraph& theGraph,
                         int              theEdgeA,
                         int              theEdgeB,
                         int              theSharedFace)
{
  if (theSharedFace < 0)
  {
    return false;
  }
  const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(theSharedFace);
  if (aFace.Surface.IsNull())
  {
    return false;
  }
  const Handle(Geom_Surface) aBasis = basisSurface(aFace.Surface);
  const bool                 isUClosed = aBasis->IsUClosed();
  const bool                 isVClosed = aBasis->IsVClosed();
  if (!isUClosed && !isVClosed)
  {
    return false;
  }

  // Get PCurves for both edges on the shared face via graph lookup.
  const BRepGraph_NodeId anEdgeIdA(BRepGraph_NodeId::Kind::Edge, theEdgeA);
  const BRepGraph_NodeId anEdgeIdB(BRepGraph_NodeId::Kind::Edge, theEdgeB);
  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, theSharedFace);
  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntryA = theGraph.Defs().FindPCurve(anEdgeIdA, aFaceId);
  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCEntryB = theGraph.Defs().FindPCurve(anEdgeIdB, aFaceId);
  if (aPCEntryA == nullptr || aPCEntryB == nullptr
      || aPCEntryA->Curve2d.IsNull() || aPCEntryB->Curve2d.IsNull())
  {
    return false;
  }

  // Compute 2D bounding boxes.
  Bnd_Box2d aBBox1, aBBox2;
  Geom2dAdaptor_Curve anAdapt1(aPCEntryA->Curve2d);
  Geom2dAdaptor_Curve anAdapt2(aPCEntryB->Curve2d);
  BndLib_Add2dCurve::Add(anAdapt1, aPCEntryA->ParamFirst, aPCEntryA->ParamLast,
                         Precision::PConfusion(), aBBox1);
  BndLib_Add2dCurve::Add(anAdapt2, aPCEntryB->ParamFirst, aPCEntryB->ParamLast,
                         Precision::PConfusion(), aBBox2);
  double aC1Umin, aC1Vmin, aC1Umax, aC1Vmax;
  double aC2Umin, aC2Vmin, aC2Umax, aC2Vmax;
  aBBox1.Get(aC1Umin, aC1Vmin, aC1Umax, aC1Vmax);
  aBBox2.Get(aC2Umin, aC2Vmin, aC2Umax, aC2Vmax);

  // Determine curve elongation direction.
  const double aDU1 = aC1Umax - aC1Umin;
  const double aDV1 = aC1Vmax - aC1Vmin;
  const double aDU2 = aC2Umax - aC2Umin;
  const double aDV2 = aC2Vmax - aC2Vmin;
  const bool   isVLong1 = (aDU1 <= aDV1);
  const bool   isVLong2 = (aDU2 <= aDV2);
  const bool   isULong1 = (aDV1 <= aDU1);
  const bool   isULong2 = (aDV2 <= aDU2);

  double aSUmin, aSUmax, aSVmin, aSVmax;
  aBasis->Bounds(aSUmin, aSUmax, aSVmin, aSVmax);

  // Evaluate PCurves at midpoints for explicit opposite-side verification.
  // Bounding box inner/outer distance can be inconclusive for short edges
  // or edges near the seam line; midpoint UV separation is definitive.
  const double aMidParam1 = 0.5 * (aPCEntryA->ParamFirst + aPCEntryA->ParamLast);
  const double aMidParam2 = 0.5 * (aPCEntryB->ParamFirst + aPCEntryB->ParamLast);
  const gp_Pnt2d aMid1 = aPCEntryA->Curve2d->EvalD0(aMidParam1);
  const gp_Pnt2d aMid2 = aPCEntryB->Curve2d->EvalD0(aMidParam2);

  // U-closed surface: two V-long edges on opposite U-sides.
  if (isUClosed && isVLong1 && isVLong2)
  {
    // Midpoint U separation must exceed 25% of surface U extent
    // to confirm edges are on different sides of the periodic seam.
    const double aUPeriod = aSUmax - aSUmin;
    if (std::abs(aMid2.X() - aMid1.X()) < THE_SEAM_SEPARATION_THRESHOLD * aUPeriod)
      return false;

    const double aDistV = std::max(aC2Vmin - aC1Vmax, aC1Vmin - aC2Vmax);
    if (aDistV < 0.0) // overlapping in V
    {
      const double aDistInner = std::max(aC2Umin - aC1Umax, aC1Umin - aC2Umax);
      const double aDistOuter =
        (aSUmax - aSUmin) - std::max(aC2Umax - aC1Umin, aC1Umax - aC2Umin);
      if (aDistOuter <= aDistInner)
      {
        return true;
      }
    }
  }

  // V-closed surface: two U-long edges on opposite V-sides.
  if (isVClosed && isULong1 && isULong2)
  {
    // Midpoint V separation must exceed 25% of surface V extent
    // to confirm edges are on different sides of the periodic seam.
    const double aVPeriod = aSVmax - aSVmin;
    if (std::abs(aMid2.Y() - aMid1.Y()) < THE_SEAM_SEPARATION_THRESHOLD * aVPeriod)
      return false;

    const double aDistU = std::max(aC2Umin - aC1Umax, aC1Umin - aC2Umax);
    if (aDistU < 0.0) // overlapping in U
    {
      const double aDistInner = std::max(aC2Vmin - aC1Vmax, aC1Vmin - aC2Vmax);
      const double aDistOuter =
        (aSVmax - aSVmin) - std::max(aC2Vmax - aC1Vmin, aC1Vmax - aC2Vmin);
      if (aDistOuter <= aDistInner)
      {
        return true;
      }
    }
  }

  return false;
}

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

NCollection_Array1<BRepGraph_NodeId> findFreeEdges(const BRepGraph& theGraph,
                                                   bool             theIncludeFloating,
                                                   NCollection_Map<int>& theFloatingEdges)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  NCollection_Vector<BRepGraph_NodeId> aFreeVec;

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeIdx);
    if (anEdge.IsDegenerate || anEdge.IsRemoved)
    {
      continue;
    }

    // Cached face count from reverse index — O(1).
    const int aFaceCount = aDefs.FaceCountOfEdge(anEdgeIdx);

    // Exclude seam edges: on a UV-closed surface (cylinder, sphere, torus) with
    // two PCurves on the same face. Surface closure is verified first via
    // IsUClosed/IsVClosed + isocurve fallback, preventing false positives on
    // non-closed surfaces (planar faces with duplicate PCurve handles).
    if (aFaceCount == 1 && isSeamEdge(theGraph, anEdgeIdx))
    {
      continue;
    }

    if (aFaceCount == 1)
    {
      aFreeVec.Append(anEdge.Id);
    }
    else if (aFaceCount == 0 && theIncludeFloating)
    {
      aFreeVec.Append(anEdge.Id);
      theFloatingEdges.Add(anEdgeIdx);
    }
  }

  const int aNbFree = aFreeVec.Length();
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

void assembleVertices(BRepGraph&                                  theGraph,
                      const NCollection_Array1<BRepGraph_NodeId>& theFreeEdges,
                      double                                      theTolerance,
                      const Handle(NCollection_IncAllocator)&     theTmpAlloc)
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

    const gp_Pnt& aPntI = aVertexPoints.Value(aVtxIter);
    bool          aMergedSelf = false;
    aTree.ForEachInRange(aPntI, theTolerance, [&](size_t theResultIdx) {
      if (aMergedSelf)
      {
        return;
      }
      const int anArrayIdx = static_cast<int>(theResultIdx) - 1;
      if (anArrayIdx == aVtxIter)
      {
        return;
      }

      const int anIdxJ = aGraphIndices.Value(anArrayIdx);
      if (aVertexMerge.IsBound(anIdxJ))
      {
        return;
      }

      // Merge: keep vertex with smaller tolerance.
      const double aTolI = theGraph.Defs().Vertex(anIdxI).Tolerance;
      const double aTolJ = theGraph.Defs().Vertex(anIdxJ).Tolerance;
      if (aTolJ < aTolI)
      {
        aVertexMerge.Bind(anIdxI, anIdxJ);
        aMergedSelf = true;
      }
      else
      {
        aVertexMerge.Bind(anIdxJ, anIdxI);
      }
    });
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
    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge =
      theGraph.Mut().EdgeDef(theFreeEdges.Value(aFreeEdgeIter).Index);
    const int* aMergedStart =
      anEdge->StartVertexIdx >= 0 ? aVertexMerge.Seek(anEdge->StartVertexIdx) : nullptr;
    if (aMergedStart != nullptr)
    {
      anEdge->StartVertexIdx = *aMergedStart;
    }
    const int* aMergedEnd =
      anEdge->EndVertexIdx >= 0 ? aVertexMerge.Seek(anEdge->EndVertexIdx) : nullptr;
    if (aMergedEnd != nullptr)
    {
      anEdge->EndVertexIdx = *aMergedEnd;
    }
  }
}

// ---------------------------------------------------------------------------
// Phase 3 (optional): Cut edges at T-vertex intersections.
// ---------------------------------------------------------------------------

void cutAtIntersections(BRepGraph&                               theGraph,
                        NCollection_Array1<BRepGraph_NodeId>&    theFreeEdges,
                        const BRepGraphAlgo_Sewing::Options&     theOptions,
                        double                                   theMinTol,
                        const NCollection_Map<int>&              theFloatingEdges,
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

  // Build UBTree on vertex point-boxes (no enlargement).
  // Edge BBoxes are already enlarged by tolerance before querying, so the tree overlap
  // test (edgeBox vs vtxPointBox) is equivalent to the original point-in-box test.
  // O(N log N) construction; replaces the O(E*V) brute-force inner loop.
  NCollection_UBTree<int, Bnd_Box>       aVtxTree;
  NCollection_UBTreeFiller<int, Bnd_Box> aTreeFiller(aVtxTree);
  for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
  {
    Bnd_Box aVtxBox;
    aVtxBox.Set(aVtxPoints.Value(aVtxIter));
    aTreeFiller.Add(aVtxIter, aVtxBox);
  }
  aTreeFiller.Fill();

  struct SplitCandidate
  {
    double Param;
    int    VtxIdx;
  };

  // Step 2a (parallel): Find T-vertex split candidates per edge.
  // Each thread creates its own VtxBoxSelector (UBTree::Select is const).
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

      // Skip floating edges (no face — no cutting).
      if (theFloatingEdges.Contains(anEdgeId.Index))
      {
        return;
      }

      const int aStartIdx = anEdge.StartVertexIdx >= 0 ? anEdge.StartVertexIdx : -1;
      const int aEndIdx   = anEdge.EndVertexIdx >= 0 ? anEdge.EndVertexIdx : -1;

      GeomAdaptor_TransformedCurve aCurve = theGraph.Defs().CurveAdaptor(anEdgeId);

      // Skip edges shorter than MinTolerance.
      if (theMinTol > 0.0)
      {
        const double aParamRange = aCurve.LastParameter() - aCurve.FirstParameter();
        if (aParamRange <= Precision::PConfusion())
        {
          return;
        }
        const double aChordLen =
          aCurve.EvalD0(aCurve.FirstParameter()).Distance(aCurve.EvalD0(aCurve.LastParameter()));
        if (aChordLen < theMinTol)
        {
          return;
        }
      }

      ExtremaPC_Curve anExtPC(aCurve);

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

      // Query UBTree for vertices whose boxes overlap the edge box — O(log V).
      VtxBoxSelector aSelector;
      aSelector.SetCurrentBox(aEdgeBBox);
      aVtxTree.Select(aSelector);

      const NCollection_Vector<int>& aCandidates = aSelector.Results();
      for (int aCandIter = 0; aCandIter < aCandidates.Length(); ++aCandIter)
      {
        const int aVtxIter = aCandidates.Value(aCandIter);
        const int aVtxIdx  = aFreeVtxList.Value(aVtxIter);
        if (aVtxIdx == aStartIdx || aVtxIdx == aEndIdx)
        {
          continue;
        }

        const gp_Pnt& aVtxPnt = aVtxPoints.Value(aVtxIter);

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
// Returns local adjacency lists indexed by free-edge position (1..N).
// Eliminates persistent RelEdge storage — candidates live only until matching.
// ---------------------------------------------------------------------------

NCollection_Array1<NCollection_Vector<int>> detectCandidates(
  const BRepGraph&                                  theGraph,
  const NCollection_Array1<BRepGraph_NodeId>&        theFreeEdges,
  const NCollection_Array1<int>&                     theFaceOfPos,
  const BRepGraphAlgo_Sewing::Options&               theOptions,
  const Handle(NCollection_IncAllocator)&             theTmpAlloc,
  const Handle(NCollection_BaseAllocator)&            theGraphAlloc)
{
  const int aNbFreeEdges = theFreeEdges.Length();
  NCollection_Array1<NCollection_Vector<int>> anAdj(1, aNbFreeEdges);
  if (!theGraphAlloc.IsNull())
  {
    for (int i = 1; i <= aNbFreeEdges; ++i)
    {
      anAdj.ChangeValue(i) = NCollection_Vector<int>(256, theGraphAlloc);
    }
  }
  if (aNbFreeEdges == 0)
  {
    return anAdj;
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

  // Step 3: Find overlapping position pairs.
  NCollection_Vector<std::pair<int, int>> aPairs(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

  const auto aSearchFn =
    [&](int theIdxI, NCollection_Vector<std::pair<int, int>>& thePairs) {
      const int      anI   = theIdxI + 1;
      const Bnd_Box& aBoxI = aBBoxes.Value(anI);
      if (aBoxI.IsVoid())
      {
        return;
      }

      const double aHalfDiagI     = aHalfDiags.Value(theIdxI);
      const double aSearchRadiusI = aHalfDiagI + aMaxHalfDiag;

      const gp_Pnt& aCenterI = aCenters.Value(theIdxI);
      aTree.ForEachInRange(aCenterI, aSearchRadiusI, [&](size_t theResultIdx) {
        const int anArrayIdx = static_cast<int>(theResultIdx) - 1;
        const int anJ        = anArrayIdx + 1;

        if (anJ <= anI)
        {
          return;
        }

        const Bnd_Box& aBoxJ = aBBoxes.Value(anJ);
        if (aBoxJ.IsVoid())
        {
          return;
        }

        if (!aBoxI.IsOut(aBoxJ))
        {
          // Same-face rejection using precomputed face indices (Opt 2).
          const int aFaceI = theFaceOfPos(anI);
          const int aFaceJ = theFaceOfPos(anJ);
          if (aFaceI >= 0 && aFaceI == aFaceJ)
          {
            if (!canSewSameFaceEdges(theGraph,
                                     theFreeEdges.Value(anI).Index,
                                     theFreeEdges.Value(anJ).Index,
                                     aFaceI))
            {
              return;
            }
          }

          thePairs.Append({anI, anJ});
        }
      });
    };

  if (theOptions.Parallel)
  {
    NCollection_Array1<NCollection_Vector<std::pair<int, int>>>
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

  // Step 4: Build bidirectional adjacency from position pairs (replaces AddRelEdge).
  for (int aPairIdx = 0; aPairIdx < aPairs.Length(); ++aPairIdx)
  {
    const auto& [aPosI, aPosJ] = aPairs.Value(aPairIdx);
    anAdj.ChangeValue(aPosI).Append(aPosJ);
    anAdj.ChangeValue(aPosJ).Append(aPosI);
  }

  return anAdj;
}

// ---------------------------------------------------------------------------
// Phase 5: Match free-edge pairs.
// Uses local adjacency from detectCandidates and inline union-find for
// connected components (replaces RelationClusters + ForEachOutOfKind).
// ---------------------------------------------------------------------------

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> matchFreeEdges(
  const BRepGraph&                                        theGraph,
  const NCollection_Array1<BRepGraph_NodeId>&              theFreeEdges,
  const NCollection_Array1<NCollection_Vector<int>>&       theAdjacency,
  const NCollection_Array1<int>&                           theFaceOfPos,
  const BRepGraphAlgo_Sewing::Options&                     theOptions,
  double                                                   theMinTol,
  const Handle(NCollection_IncAllocator)&                  theTmpAlloc)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;
  const int aNbFreeEdges = theFreeEdges.Length();
  if (aNbFreeEdges == 0)
  {
    return aResult;
  }

  // Build connected components via inline union-find on positions (Opt 1).
  NCollection_Array1<int> aParent(1, aNbFreeEdges);
  for (int i = 1; i <= aNbFreeEdges; ++i)
  {
    aParent(i) = i;
  }

  auto findRootPos = [&aParent](int theIdx) -> int {
    int aRoot = theIdx;
    while (aParent(aRoot) != aRoot)
    {
      aRoot = aParent(aRoot);
    }
    // Path compression.
    int aCur = theIdx;
    while (aCur != aRoot)
    {
      const int aNext = aParent(aCur);
      aParent.ChangeValue(aCur) = aRoot;
      aCur = aNext;
    }
    return aRoot;
  };

  for (int aPosI = 1; aPosI <= aNbFreeEdges; ++aPosI)
  {
    const NCollection_Vector<int>& aCands = theAdjacency(aPosI);
    for (int j = 0; j < aCands.Length(); ++j)
    {
      const int aPosJ = aCands(j);
      const int aRootI = findRootPos(aPosI);
      const int aRootJ = findRootPos(aPosJ);
      if (aRootI != aRootJ)
      {
        aParent.ChangeValue(aRootJ) = aRootI;
      }
    }
  }

  // Assign component IDs per position.
  NCollection_DataMap<int, int> aRootToComp(aNbFreeEdges, theTmpAlloc);
  int aNbComponents = 0;
  NCollection_Array1<int> aCompByPos(1, aNbFreeEdges);
  for (int aPos = 1; aPos <= aNbFreeEdges; ++aPos)
  {
    if (theAdjacency(aPos).IsEmpty())
    {
      aCompByPos(aPos) = 0;
      continue;
    }
    const int  aRoot    = findRootPos(aPos);
    const int* aCompPtr = aRootToComp.Seek(aRoot);
    if (aCompPtr != nullptr)
    {
      aCompByPos(aPos) = *aCompPtr;
    }
    else
    {
      ++aNbComponents;
      aRootToComp.Bind(aRoot, aNbComponents);
      aCompByPos(aPos) = aNbComponents;
    }
  }

  if (aNbComponents == 0)
  {
    return aResult;
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
      if (aComp == 0)
      {
        return;
      }

      MatchResult& aMatch = aPerEdgeMatch.ChangeValue(anI);

      const BRepGraph_TopoNode::EdgeDef& anEdgeANode = theGraph.Defs().Edge(anIdA.Index);
      if (anEdgeANode.IsDegenerate)
      {
        return;
      }

      GeomAdaptor_TransformedCurve aCurveA = theGraph.Defs().CurveAdaptor(anIdA);

      // Skip edges shorter than MinTolerance.
      if (theMinTol > 0.0)
      {
        const gp_Pnt aPtFirst = aCurveA.EvalD0(aCurveA.FirstParameter());
        const gp_Pnt aPtLast  = aCurveA.EvalD0(aCurveA.LastParameter());
        if (aPtFirst.Distance(aPtLast) < theMinTol)
        {
          return;
        }
      }

      GCPnts_UniformAbscissa aSamplerA(aCurveA, THE_NB_EDGE_MATCH_SAMPLES);
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

      // Iterate candidates from local adjacency (replaces ForEachOutOfKind).
      const NCollection_Vector<int>& aCands = theAdjacency(anI);
      for (int aCandIter = 0; aCandIter < aCands.Length(); ++aCandIter)
      {
        const int aPosB = aCands.Value(aCandIter);
        if (aCompByPos.Value(aPosB) != aComp)
        {
          continue;
        }

        const BRepGraph_NodeId anIdB = theFreeEdges.Value(aPosB);

        // Same-face rejection using precomputed face indices (Opt 2).
        const int aFaceA = theFaceOfPos(anI);
        const int aFaceB = theFaceOfPos(aPosB);
        if (aFaceA >= 0 && aFaceA == aFaceB)
        {
          if (!canSewSameFaceEdges(theGraph, anIdA.Index, anIdB.Index, aFaceA))
          {
            continue;
          }
        }

        // LocalTolerancesMode: use adaptive work tolerance.
        double aWorkTol = theOptions.Tolerance;
        if (theOptions.LocalTolerancesMode)
        {
          aWorkTol += theGraph.Defs().Edge(anIdA.Index).Tolerance
                      + theGraph.Defs().Edge(anIdB.Index).Tolerance;
        }

        if (BRepGraph_Analyze::AreEdgesCompatibleSampled(theGraph,
                                                         anIdA,
                                                         anIdB,
                                                         aSamplePtsA,
                                                         anExtPCRevA,
                                                         aChordA,
                                                         aWorkTol,
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
      }
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

      // In non-manifold mode, normalize pairs so the longest edge is EdgeA (keep-edge).
      // This ensures the longest curve retains its geometry and accumulates PCurves.
      // Uses actual arc length via GCPnts_AbscissaPoint for accuracy on curved edges.
      if (theOptions.NonManifoldMode)
      {
        for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
        {
          ScoredPair& aPair = aSortedPairs.ChangeValue(anIdx);
          double aLenA = 0.0, aLenB = 0.0;
          try
          {
            OCC_CATCH_SIGNALS
            GeomAdaptor_TransformedCurve aCrvA = theGraph.Defs().CurveAdaptor(aPair.EdgeA);
            aLenA = GCPnts_AbscissaPoint::Length(aCrvA);
          }
          catch (const Standard_Failure&)
          {
            aLenA = theGraph.Defs().Edge(aPair.EdgeA.Index).ParamLast
                    - theGraph.Defs().Edge(aPair.EdgeA.Index).ParamFirst;
          }
          try
          {
            OCC_CATCH_SIGNALS
            GeomAdaptor_TransformedCurve aCrvB = theGraph.Defs().CurveAdaptor(aPair.EdgeB);
            aLenB = GCPnts_AbscissaPoint::Length(aCrvB);
          }
          catch (const Standard_Failure&)
          {
            aLenB = theGraph.Defs().Edge(aPair.EdgeB.Index).ParamLast
                    - theGraph.Defs().Edge(aPair.EdgeB.Index).ParamFirst;
          }
          if (aLenB > aLenA)
          {
            std::swap(aPair.EdgeA, aPair.EdgeB);
          }
        }
      }

      NCollection_Map<int> aConsumed;
      for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
      {
        const ScoredPair& aPair = aSortedPairs.Value(anIdx);
        if (aConsumed.Contains(aPair.EdgeB.Index))
        {
          continue;
        }
        // In manifold mode, each edge participates in at most one pair.
        // In non-manifold mode, only the remove-edge (EdgeB) is consumed;
        // the keep-edge (EdgeA) can accumulate PCurves from multiple merges.
        if (!theOptions.NonManifoldMode && aConsumed.Contains(aPair.EdgeA.Index))
        {
          continue;
        }

        aCompPairs.ChangeValue(aComp).Append({aPair.EdgeA, aPair.EdgeB});
        if (!theOptions.NonManifoldMode)
        {
          aConsumed.Add(aPair.EdgeA.Index);
        }
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
  BRepGraph&                                                               theGraph,
  const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs,
  const BRepGraphAlgo_Sewing::Options&                                     theOptions,
  NCollection_IndexedMap<int>&                                             theSewnEdgeIndices,
  BRepGraphAlgo_Sewing::Result&                                            theResult,
  const Handle(NCollection_IncAllocator)&                                  theTmpAlloc)
{
  int aSewnCount = 0;

  // Accumulate history pairs for batch recording (Opt 5).
  // Use the scoped temp allocator — these vectors are consumed by RecordBatch then discarded.
  NCollection_Vector<BRepGraph_NodeId> aHistOriginals(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Vector<BRepGraph_NodeId> aHistReplacements(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

  for (int aPairIter = 0; aPairIter < thePairs.Length(); ++aPairIter)
  {
    const auto& [anIdA, anIdB] = thePairs.Value(aPairIter);

    const BRepGraph_TopoNode::EdgeDef& aRemoveEdge = theGraph.Defs().Edge(anIdB.Index);
    const BRepGraph_TopoNode::EdgeDef& aKeepEdge   = theGraph.Defs().Edge(anIdA.Index);

    // 1. Tolerance merge (graph-only).
    const double aMergedTol = std::max(aKeepEdge.Tolerance, aRemoveEdge.Tolerance);

    // MaxTolerance guard: skip pair if merged tolerance exceeds upper bound.
    if (aMergedTol > theOptions.MaxTolerance)
    {
      ++theResult.NbRejectedByTolerance;
      continue;
    }

    theSewnEdgeIndices.Add(anIdA.Index);
    theGraph.Mut().EdgeDef(anIdA.Index)->Tolerance = aMergedTol;

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

    // Remove the old edge properly — decrements NbActiveEdges, clears cache/RelEdges.
    // Pass replacement (anIdA) so layers can migrate data from remove-edge to keep-edge.
    theGraph.Builder().RemoveNode(anIdB, anIdA);

    // 5. Accumulate for batch history recording.
    aHistOriginals.Append(anIdB);
    aHistReplacements.Append(anIdA);

    theResult.SewnEdgePairs.Append(anIdA);
    ++aSewnCount;
  }

  // 5b. Batch history recording (single HistoryRecord for all merges).
  if (!aHistOriginals.IsEmpty())
  {
    static const TCollection_AsciiString THE_MERGE_LABEL("Sewing:MergeEdge");
    TCollection_AsciiString anExtra("tol=");
    anExtra += TCollection_AsciiString(theOptions.Tolerance);
    anExtra += " pairs=";
    anExtra += TCollection_AsciiString(aHistOriginals.Length());
    theGraph.History().RecordBatch(THE_MERGE_LABEL, aHistOriginals, aHistReplacements, anExtra);
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

void processEdges([[maybe_unused]] BRepGraph&                           theGraph,
                  [[maybe_unused]] const NCollection_IndexedMap<int>&   theSewnEdgeIndices,
                  [[maybe_unused]] const BRepGraphAlgo_Sewing::Options& theOptions)
{
  // Sewn edges already have correct tolerances from the merge phase.
  // Vertex-tolerance re-inflation is skipped: the vertex merge phase may
  // have inflated vertex tolerances beyond the geometric need, and propagating
  // that to edges would undo the careful tolerance set during merging.
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

// ---------------------------------------------------------------------------
// Phase 7: Convert remaining free edges that form degenerate wires.
// Ported from BRepBuilderAPI_Sewing::EdgeProcessing + IsDegeneratedWire +
// DegeneratedSection (lines 4219-4474).
// ---------------------------------------------------------------------------

void convertDegenerateEdges(BRepGraph&                   theGraph,
                            BRepGraphAlgo_Sewing::Result& theResult)
{
  // Collect remaining free edges after merging.
  const NCollection_Vector<BRepGraph_NodeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(theGraph);
  if (aFreeEdges.IsEmpty())
  {
    return;
  }

  // For each free edge, check if it forms a degenerate "wire" by itself:
  // edge chord length <= sum of vertex tolerances.
  for (int aFreeIter = 0; aFreeIter < aFreeEdges.Length(); ++aFreeIter)
  {
    const BRepGraph_NodeId             anEdgeId = aFreeEdges.Value(aFreeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdge   = theGraph.Defs().Edge(anEdgeId.Index);

    if (anEdge.IsDegenerate || anEdge.Curve3d.IsNull())
    {
      continue;
    }

    // Compute edge chord length via 3-point check (start, mid, end).
    const gp_Pnt aPtFirst = anEdge.Curve3d->EvalD0(anEdge.ParamFirst);
    const gp_Pnt aPtLast  = anEdge.Curve3d->EvalD0(anEdge.ParamLast);
    const gp_Pnt aPtMid   = anEdge.Curve3d->EvalD0((anEdge.ParamFirst + anEdge.ParamLast) * 0.5);

    double aLength = 0.0;
    if (aPtFirst.Distance(aPtLast) > aPtFirst.Distance(aPtMid))
    {
      aLength = aPtFirst.Distance(aPtLast);
    }
    else
    {
      // Curved edge shorter than chord — use full arc length.
      try
      {
        OCC_CATCH_SIGNALS
        GeomAdaptor_TransformedCurve aCurve = theGraph.Defs().CurveAdaptor(anEdgeId);
        aLength = GCPnts_AbscissaPoint::Length(aCurve);
      }
      catch (const Standard_Failure&)
      {
        // Fallback to chord length: safe because we only compare against vertex tolerance
        // sum, and chord length is always <= arc length (triangle inequality).
        aLength = aPtFirst.Distance(aPtLast);
      }
    }

    // Sum vertex tolerances.
    double aVertexTolSum = 0.0;
    if (anEdge.StartVertexIdx >= 0)
    {
      aVertexTolSum += theGraph.Defs().Vertex(anEdge.StartVertexIdx).Tolerance;
    }
    if (anEdge.EndVertexIdx >= 0)
    {
      aVertexTolSum += theGraph.Defs().Vertex(anEdge.EndVertexIdx).Tolerance;
    }

    if (aLength <= aVertexTolSum)
    {
      // Mark as degenerate: clear 3D curve, merge vertices to midpoint.
      BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aMutEdge = theGraph.MutEdge(anEdgeId.Index);
      aMutEdge->IsDegenerate                  = true;
      aMutEdge->Curve3d.Nullify();

      // Merge start/end vertices if they differ.
      if (anEdge.StartVertexIdx >= 0 && anEdge.EndVertexIdx >= 0
          && anEdge.StartVertexIdx != anEdge.EndVertexIdx)
      {
        const double aTolStart = theGraph.Defs().Vertex(anEdge.StartVertexIdx).Tolerance;
        const double aTolEnd   = theGraph.Defs().Vertex(anEdge.EndVertexIdx).Tolerance;
        const double aD1       = aTolStart + aPtMid.Distance(aPtFirst);
        const double aD2       = aTolEnd + aPtMid.Distance(aPtLast);
        const double aNewTol   = std::max(aD1, aD2);

        // Keep start vertex, update it to midpoint.
        BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> aVtx = theGraph.MutVertex(anEdge.StartVertexIdx);
        aVtx->Point     = aPtMid;
        aVtx->Tolerance = aNewTol;
        aMutEdge->EndVertexIdx = anEdge.StartVertexIdx;
      }

      theResult.DegeneratedEdges.Append(anEdgeId);
    }
  }

  theResult.NbDegeneratedEdges = theResult.DegeneratedEdges.Length();
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

  // Compute effective MinTolerance.
  const double aMinTol = theOptions.MinTolerance > 0.0
                           ? theOptions.MinTolerance
                           : std::max(theOptions.Tolerance * 1.0e-4, Precision::Confusion());

  // Main allocator for cross-phase data.
  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator;
  // Temporary allocator for per-phase scratch, Reset(false) between phases.
  Handle(NCollection_IncAllocator) aTmpAllocator = new NCollection_IncAllocator;

  // Phase 0 (optional): Face analysis — detect/remove small edges and faces.
  if (theOptions.FaceAnalysis)
  {
    BRepGraphAlgo_FaceAnalysis::Options aFAOptions;
    aFAOptions.MinTolerance = aMinTol;
    aFAOptions.Parallel     = theOptions.Parallel;
    BRepGraphAlgo_FaceAnalysis::Result aFAResult =
      BRepGraphAlgo_FaceAnalysis::Perform(theGraph, aFAOptions);
    aResult.DegeneratedEdges = std::move(aFAResult.DegeneratedEdges);
    aResult.DeletedFaces     = std::move(aFAResult.DeletedFaces);
    aResult.NbDeletedFaces   = aFAResult.NbDeletedFaces;
    aResult.NbDegeneratedEdges = aFAResult.DegeneratedEdges.Length();
  }

  // Phase 1: Find free edges.
  NCollection_Map<int> aFloatingEdges;
  NCollection_Array1<BRepGraph_NodeId> aFreeEdges =
    findFreeEdges(theGraph, theOptions.FloatingEdgesMode, aFloatingEdges);
  aResult.NbFreeEdgesBefore = aFreeEdges.Length();

  if (aFreeEdges.IsEmpty())
  {
    aResult.IsDone           = true;
    aResult.NbFreeEdgesAfter = 0;
    return aResult;
  }

  // Phase 2: Assemble vertices.
  assembleVertices(theGraph, aFreeEdges, theOptions.Tolerance, aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 3 (optional): Cut edges at intersections.
  if (theOptions.Cutting)
  {
    cutAtIntersections(theGraph, aFreeEdges, theOptions, aMinTol, aFloatingEdges, aTmpAllocator);
    aTmpAllocator->Reset(false);
  }

  // Precompute face index per free-edge position (Opt 2).
  // Free edges have exactly 1 face; floating edges have 0 (-1 sentinel).
  const int aNbFreeEdgesForMap = aFreeEdges.Length();
  NCollection_Array1<int> aFaceOfPos(1, aNbFreeEdgesForMap);
  for (int aFreeIdx = 1; aFreeIdx <= aNbFreeEdgesForMap; ++aFreeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      theGraph.Defs().Edge(aFreeEdges.Value(aFreeIdx).Index);
    aFaceOfPos(aFreeIdx) = anEdge.PCurves.IsEmpty() ? -1 : anEdge.PCurves.Value(0).FaceDefId.Index;
  }

  // Phase 4: Detect sewing candidates (returns local adjacency, no RelEdge storage).
  NCollection_Array1<NCollection_Vector<int>> aAdjacency =
    detectCandidates(theGraph, aFreeEdges, aFaceOfPos, theOptions, aTmpAllocator, anAllocator);
  aTmpAllocator->Reset(false);

  // Phase 5: Match free edge pairs (uses local adjacency + inline union-find).
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
    matchFreeEdges(theGraph, aFreeEdges, aAdjacency, aFaceOfPos, theOptions, aMinTol, aTmpAllocator);
  aTmpAllocator->Reset(false);

  if (aMatchedPairs.IsEmpty())
  {
    aResult.IsDone           = true;
    aResult.NbFreeEdgesAfter = aResult.NbFreeEdgesBefore;
    aResult.FreeEdges        = BRepGraph_Analyze::FreeEdges(theGraph);
    return aResult;
  }

  // Phase 6: Merge matched edge pairs.
  const int                   aNbPairs = aMatchedPairs.Length();
  NCollection_IndexedMap<int> aSewnEdgeIndices(aNbPairs, anAllocator);
  aResult.NbSewnEdges =
    mergeMatchedEdges(theGraph, aMatchedPairs, theOptions, aSewnEdgeIndices, aResult, aTmpAllocator);

  // Phase 7: Convert remaining free degenerate edges.
  convertDegenerateEdges(theGraph, aResult);

  // Phase 8: Process sewn edges (tolerance consistency).
  processEdges(theGraph, aSewnEdgeIndices, theOptions);

  // Phase 9: Populate result output.
  aResult.FreeEdges        = BRepGraph_Analyze::FreeEdges(theGraph);
  aResult.NbFreeEdgesAfter = aResult.FreeEdges.Length();

  // Detect multiple edges (shared by >2 faces).
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeIdx);
    if (anEdge.IsRemoved || anEdge.IsDegenerate)
    {
      continue;
    }
    if (aDefs.FaceCountOfEdge(anEdgeIdx) > 2)
    {
      aResult.MultipleEdges.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx));
    }
  }
  aResult.NbMultipleEdges = aResult.MultipleEdges.Length();

  BRepGraph_Mutator::CommitMutation(theGraph);
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

  BRepGraph                        aGraph;
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
