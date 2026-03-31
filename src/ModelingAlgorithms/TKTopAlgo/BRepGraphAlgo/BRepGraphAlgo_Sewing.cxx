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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

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

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_ParallelPolicy.hxx>
#include <BRepGraph_DeferredScope.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>

#include <algorithm>
#include <optional>
#include <utility>

namespace
{
// 5 evenly-spaced samples along normalized arc-length for edge geometric matching.
// Balance between accuracy and cost: fewer samples miss mid-span deviations,
// more samples add GCPnts evaluation overhead with diminishing returns.
constexpr int THE_NB_EDGE_MATCH_SAMPLES = 5;

// Maximum allowed ratio of chord lengths between candidate merge edges.
// Edges with chord-length ratio > 2.0 are geometrically incompatible
// (one edge is more than twice as long as the other).
constexpr double THE_MAX_CHORD_RATIO = 2.0;

// If forward-pass max normalized distance is below 1% of tolerance,
// the reverse pass is redundant (edges match tightly in one direction).
constexpr double THE_HIGH_CONFIDENCE_RATIO = 0.01;

// Initial capacity for scratch NCollection_Vectors used in sewing phases.
// 256 covers most box/cylinder-class models without reallocation;
// larger models grow via IncAllocator bump pages.
constexpr int THE_INIT_VECTOR_CAPACITY = 256;

// Minimum UV midpoint separation (as fraction of surface period) to
// distinguish a true seam edge from a non-seam edge that merely shares
// surface parameter-space boundary. 0.25 = quarter-period threshold.
constexpr double THE_SEAM_SEPARATION_THRESHOLD = 0.25;

struct EdgeEndpointCache
{
  gp_Pnt Start;
  gp_Pnt End;
  double Chord     = 0.0;
  double Tolerance = 0.0; //!< Edge tolerance, populated only in LocalTolerancesMode.
};

//! Compute endpoint-based matching score between two edges.
//! Returns min(start-start + end-end, start-end + end-start) distance.
//! Lower score means a better endpoint correspondence.
//! @param[in] theStartA first edge start point
//! @param[in] theEndA first edge end point
//! @param[in] theStartB second edge start point
//! @param[in] theEndB second edge end point
//! @return endpoint distance score
static double edgeEndpointPairScore(const gp_Pnt& theStartA,
                                    const gp_Pnt& theEndA,
                                    const gp_Pnt& theStartB,
                                    const gp_Pnt& theEndB)
{
  return std::min(theStartA.Distance(theStartB) + theEndA.Distance(theEndB),
                  theStartA.Distance(theEndB) + theEndA.Distance(theStartB));
}

static bool isEdgeCompatibleMinTol(const EdgeEndpointCache& theEndpoint, const double theMinTol)
{
  return theMinTol <= 0.0 || theEndpoint.Chord >= theMinTol;
}

//! Geometric compatibility test between two edges using sampled
//! bidirectional projection. Forward pass projects edgeA samples onto
//! edgeB; if max distance exceeds theHighConfidenceRatio * tolerance,
//! a reverse pass projects edgeB samples onto edgeA.
//! @param[in] theGraph             source graph
//! @param[in] theEdgeA             first edge id
//! @param[in] theEdgeB             second edge id
//! @param[in] theEndpointA         cached endpoint data for edge A
//! @param[in] theEndpointB         cached endpoint data for edge B
//! @param[in] theSamplePtsA        pre-sampled points on edgeA
//! @param[in] theExtPCRevA         pre-initialized reverse projector on edgeA curve
//! @param[in] theTolerance         maximum allowed point-to-curve distance
//! @param[in] theNbSamples         number of samples along edgeB for reverse pass
//! @param[in] theMaxChordRatio     max accepted endpoint chord ratio
//! @param[in] theHighConfidenceRatio forward-pass threshold to skip reverse pass
//! @return true if edges are geometrically compatible
static bool areEdgesCompatibleSampled(const BRepGraph&                  theGraph,
                                      const BRepGraph_EdgeId            theEdgeA,
                                      const BRepGraph_EdgeId            theEdgeB,
                                      const EdgeEndpointCache&          theEndpointA,
                                      const EdgeEndpointCache&          theEndpointB,
                                      const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                                      const ExtremaPC_Curve&            theExtPCRevA,
                                      const double                      theTolerance,
                                      const int                         theNbSamples     = 5,
                                      const double                      theMaxChordRatio = 2.0,
                                      const double theHighConfidenceRatio                = 0.01)
{
  const BRepGraph::TopoView&   aDefs  = theGraph.Topo();
  const BRepGraphInc::EdgeDef& aNodeA = aDefs.Edges().Definition(theEdgeA);
  const BRepGraphInc::EdgeDef& aNodeB = aDefs.Edges().Definition(theEdgeB);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  const gp_Pnt& aStartA = theEndpointA.Start;
  const gp_Pnt& aEndA   = theEndpointA.End;
  const gp_Pnt& aStartB = theEndpointB.Start;
  const gp_Pnt& aEndB   = theEndpointB.End;

  const bool isSameDir =
    aStartA.Distance(aStartB) <= theTolerance && aEndA.Distance(aEndB) <= theTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= theTolerance && aEndA.Distance(aStartB) <= theTolerance;
  if (!isSameDir && !isReversed)
  {
    return false;
  }

  const double aChordA = theEndpointA.Chord;
  const double aChordB = theEndpointB.Chord;
  if (aChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = aChordA > aChordB ? aChordA / aChordB : aChordB / aChordA;
    if (aChordRatio > theMaxChordRatio)
    {
      return false;
    }
  }

  GeomAdaptor_TransformedCurve aCurveB = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeB);
  ExtremaPC_Curve              anExtPCB(aCurveB);

  const double aTolSq        = theTolerance * theTolerance;
  double       aMaxFwdDistSq = 0.0;
  for (int aSampleIter = theSamplePtsA.Lower(); aSampleIter <= theSamplePtsA.Upper(); ++aSampleIter)
  {
    const gp_Pnt&            aPntA = theSamplePtsA.Value(aSampleIter);
    const ExtremaPC::Result& aRes =
      anExtPCB.Perform(aPntA, Precision::Confusion(), ExtremaPC::SearchMode::Min);
    if (!aRes.IsDone() || aRes.NbExt() == 0)
    {
      return false;
    }

    const double aMinSqDist = aRes.MinSquareDistance();
    if (aMinSqDist > aTolSq)
    {
      return false;
    }
    if (aMinSqDist > aMaxFwdDistSq)
    {
      aMaxFwdDistSq = aMinSqDist;
    }
  }

  if (aMaxFwdDistSq < theHighConfidenceRatio * aTolSq)
  {
    return true;
  }

  GCPnts_UniformAbscissa aSamplerB(aCurveB, theNbSamples);
  if (!aSamplerB.IsDone() || aSamplerB.NbPoints() < 2)
  {
    return false;
  }

  for (int aSampleIter = 1; aSampleIter <= aSamplerB.NbPoints(); ++aSampleIter)
  {
    const gp_Pnt             aPntB = aCurveB.EvalD0(aSamplerB.Parameter(aSampleIter));
    const ExtremaPC::Result& aRes =
      theExtPCRevA.Perform(aPntB, Precision::Confusion(), ExtremaPC::SearchMode::Min);
    if (!aRes.IsDone() || aRes.NbExt() == 0)
    {
      return false;
    }
    if (aRes.MinSquareDistance() > aTolSq)
    {
      return false;
    }
  }

  return true;
}

//! UBTree selector for vertex bounding boxes (used in cutAtIntersections).
//! Each thread creates its own instance, so this is safe in parallel loops.
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

//! Unwrap trimmed/offset surface to its underlying basis surface.
//! @param[in] theSurf surface to unwrap (may be Geom_RectangularTrimmedSurface or
//! Geom_OffsetSurface)
//! @return the innermost basis surface
occ::handle<Geom_Surface> basisSurface(const occ::handle<Geom_Surface>& theSurf)
{
  occ::handle<Geom_Surface> aSurf = theSurf;
  for (;;)
  {
    if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      aSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf)->BasisSurface();
    }
    else if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      aSurf = occ::down_cast<Geom_OffsetSurface>(aSurf)->BasisSurface();
    }
    else
    {
      return aSurf;
    }
  }
}

//! Test surface closure via isocurve sampling at the PCurve endpoints.
//! Returns true if the isocurves at both ends form closed loops,
//! indicating the surface is closed in the direction perpendicular to the edge.
//! @param[in] theSurf  face surface
//! @param[in] thePCurve  parametric curve on the surface
//! @param[in] theFirst  PCurve first parameter
//! @param[in] theLast   PCurve last parameter
//! @param[in] theIsU    true to test U-closure, false for V-closure
//! @return true if isocurves at both endpoints close within tolerance
bool isClosedByIsos(const occ::handle<Geom_Surface>& theSurf,
                    const occ::handle<Geom2d_Curve>& thePCurve,
                    double                           theFirst,
                    double                           theLast,
                    bool                             theUIsos)
{
  if (theSurf.IsNull() || thePCurve.IsNull())
  {
    return false;
  }
  const gp_Pnt2d aPSurf1 = thePCurve->IsPeriodic()
                             ? thePCurve->Value(theFirst)
                             : thePCurve->Value(std::max(theFirst, thePCurve->FirstParameter()));
  const gp_Pnt2d aPSurf2 = thePCurve->IsPeriodic()
                             ? thePCurve->Value(theLast)
                             : thePCurve->Value(std::min(theLast, thePCurve->LastParameter()));

  occ::handle<Geom_Curve> anIso1, anIso2;
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
  const gp_Pnt aPt1F   = anIso1->EvalD0(aFirst1);
  const gp_Pnt aPt1M   = anIso1->EvalD0((aFirst1 + aLast1) * 0.5);
  const gp_Pnt aPt1L   = anIso1->EvalD0(aLast1);
  const gp_Pnt aPt2F   = anIso2->EvalD0(aFirst2);
  const gp_Pnt aPt2M   = anIso2->EvalD0((aFirst2 + aLast2) * 0.5);
  const gp_Pnt aPt2L   = anIso2->EvalD0(aLast2);
  // Closed if endpoints are closer than midpoint distance for both isocurves.
  return ((aPt1F.XYZ() - aPt1L.XYZ()).Modulus()
          < (aPt1F.XYZ() - aPt1M.XYZ()).Modulus() - Precision::Confusion())
         && ((aPt2F.XYZ() - aPt2L.XYZ()).Modulus()
             < (aPt2F.XYZ() - aPt2M.XYZ()).Modulus() - Precision::Confusion());
}

//! Check if the face surface is closed in the direction perpendicular to an edge.
//! First checks Surface::IsUClosed()/IsVClosed(), then falls back to
//! isocurve-based sampling for surfaces that report non-closed but are
//! geometrically closed (e.g., trimmed periodic surfaces).
//! @param[in] theGraph      source graph
//! @param[in] theFaceDefIdx face definition index
//! @param[in] theCoEdge     coedge entity on the face
//! @param[out] theIsUClosed set to true if closed in U
//! @param[out] theIsVClosed set to true if closed in V
//! @return true if closed in at least one direction
bool isSurfaceClosedForEdge(const BRepGraph&               theGraph,
                            const BRepGraph_FaceId         theFaceDefId,
                            const BRepGraphInc::CoEdgeDef& theCoEdge,
                            bool                           theCheckU)
{
  if (!theFaceDefId.IsValid() || !BRepGraph_Tool::Face::HasSurface(theGraph, theFaceDefId))
  {
    return false;
  }
  const occ::handle<Geom_Surface>& aFaceSurf =
    BRepGraph_Tool::Face::Surface(theGraph, theFaceDefId);
  const occ::handle<Geom_Surface> aBasis   = basisSurface(aFaceSurf);
  const bool                      isClosed = theCheckU ? aBasis->IsUClosed() : aBasis->IsVClosed();
  if (isClosed)
  {
    return true;
  }
  // Isocurve fallback: check if isocurves at PCurve endpoints form closed loops.
  if (theCoEdge.Curve2DRepId.IsValid())
  {
    const occ::handle<Geom2d_Curve>& aCoEdgePCurve =
      BRepGraph_Tool::CoEdge::PCurve(theGraph, theCoEdge);
    return isClosedByIsos(aBasis,
                          aCoEdgePCurve,
                          theCoEdge.ParamFirst,
                          theCoEdge.ParamLast,
                          !theCheckU);
  }
  return false;
}

//! Detect if an edge is a seam (has two CoEdges with opposite Sense on the same
//! UV-closed face, linked via SeamPairId). Eliminates false positives from
//! non-seam edges on non-closed surfaces.
//! @param[in] theGraph   source graph
//! @param[in] theEdgeId edge definition identifier
//! @return true if edge is a seam
bool isSeamEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
{
  // Use reverse index for O(1) lookup instead of scanning all coedges.
  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theGraph.Topo().Edges().CoEdges(theEdgeId);
  for (int i = 0; i < aCoEdgeIdxs.Length(); ++i)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(i));
    if (aCoEdge.SeamPairId.IsValid() && aCoEdge.FaceDefId.IsValid())
    {
      if (isSurfaceClosedForEdge(theGraph, aCoEdge.FaceDefId, aCoEdge, true)
          || isSurfaceClosedForEdge(theGraph, aCoEdge.FaceDefId, aCoEdge, false))
      {
        return true;
      }
    }
  }
  return false;
}

//! Caches pre-computed closure flags to avoid repeated expensive
//! IsUClosed/IsVClosed queries on the same surface in parallel loops.
struct FaceClosureInfo
{
  bool IsUClosed = false;
  bool IsVClosed = false;
};

//! Check if two free edges on the same UV-closed face lie on opposite sides of
//! the seam and can be sewn together. Uses 2D bounding boxes of their PCurves:
//! on a U-closed surface, two V-long edges are on opposite sides if their
//! outer U-gap (across the seam) is smaller than their inner U-gap.
//! Midpoint UV separation must exceed 25% of the period to catch same-side false positives.
//! @param[in] theGraph      source graph
//! @param[in] theEdgeA      first edge index
//! @param[in] theEdgeB      second edge index
//! @param[in] theSharedFace shared face index (-1 if none)
//! @param[in] theClosure    pre-computed closure flags (nullptr = compute on the fly)
//! @return true if the edges can be sewn on the shared face
bool canSewSameFaceEdges(const BRepGraph&          theGraph,
                         const BRepGraph_EdgeId    theEdgeA,
                         const BRepGraph_EdgeId    theEdgeB,
                         const BRepGraph_FaceId    theSharedFace,
                         const FaceClosureInfo*    theClosure = nullptr)
{
  if (!theSharedFace.IsValid())
  {
    return false;
  }

  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theSharedFace))
  {
    return false;
  }

  const occ::handle<Geom_Surface> aBasis =
    basisSurface(BRepGraph_Tool::Face::Surface(theGraph, theSharedFace));

  bool isUClosed = false;
  bool isVClosed = false;
  if (theClosure != nullptr)
  {
    isUClosed = theClosure->IsUClosed;
    isVClosed = theClosure->IsVClosed;
  }
  else
  {
    isUClosed = aBasis->IsUClosed();
    isVClosed = aBasis->IsVClosed();
  }
  if (!isUClosed && !isVClosed)
  {
    return false;
  }

  // Get PCurves for both edges on the shared face via graph lookup.
  const BRepGraphInc::CoEdgeDef* aPCEntryA =
    BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeA, theSharedFace);
  const BRepGraphInc::CoEdgeDef* aPCEntryB =
    BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeB, theSharedFace);
  if (aPCEntryA == nullptr || aPCEntryB == nullptr || !aPCEntryA->Curve2DRepId.IsValid()
      || !aPCEntryB->Curve2DRepId.IsValid())
  {
    return false;
  }

  const occ::handle<Geom2d_Curve>& aCurve2dA = BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCEntryA);
  const occ::handle<Geom2d_Curve>& aCurve2dB = BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCEntryB);
  if (aCurve2dA.IsNull() || aCurve2dB.IsNull())
    return false;

  // Compute 2D bounding boxes.
  Bnd_Box2d           aBBox1, aBBox2;
  Geom2dAdaptor_Curve anAdapt1(aCurve2dA);
  Geom2dAdaptor_Curve anAdapt2(aCurve2dB);
  BndLib_Add2dCurve::Add(anAdapt1,
                         aPCEntryA->ParamFirst,
                         aPCEntryA->ParamLast,
                         Precision::PConfusion(),
                         aBBox1);
  BndLib_Add2dCurve::Add(anAdapt2,
                         aPCEntryB->ParamFirst,
                         aPCEntryB->ParamLast,
                         Precision::PConfusion(),
                         aBBox2);
  double aC1Umin, aC1Vmin, aC1Umax, aC1Vmax;
  double aC2Umin, aC2Vmin, aC2Umax, aC2Vmax;
  aBBox1.Get(aC1Umin, aC1Vmin, aC1Umax, aC1Vmax);
  aBBox2.Get(aC2Umin, aC2Vmin, aC2Umax, aC2Vmax);

  // Determine curve elongation direction.
  const double aDU1     = aC1Umax - aC1Umin;
  const double aDV1     = aC1Vmax - aC1Vmin;
  const double aDU2     = aC2Umax - aC2Umin;
  const double aDV2     = aC2Vmax - aC2Vmin;
  const bool   isVLong1 = (aDU1 <= aDV1);
  const bool   isVLong2 = (aDU2 <= aDV2);
  const bool   isULong1 = (aDV1 <= aDU1);
  const bool   isULong2 = (aDV2 <= aDU2);

  double aSUmin, aSUmax, aSVmin, aSVmax;
  aBasis->Bounds(aSUmin, aSUmax, aSVmin, aSVmax);

  // Evaluate PCurves at midpoints for explicit opposite-side verification.
  // Bounding box inner/outer distance can be inconclusive for short edges
  // or edges near the seam line; midpoint UV separation is definitive.
  const double   aMidParam1 = 0.5 * (aPCEntryA->ParamFirst + aPCEntryA->ParamLast);
  const double   aMidParam2 = 0.5 * (aPCEntryB->ParamFirst + aPCEntryB->ParamLast);
  const gp_Pnt2d aMid1      = aCurve2dA->Value(aMidParam1);
  const gp_Pnt2d aMid2      = aCurve2dB->Value(aMidParam2);

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
      const double aDistOuter = (aSUmax - aSUmin) - std::max(aC2Umax - aC1Umin, aC1Umax - aC2Umin);
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
      const double aDistOuter = (aSVmax - aSVmin) - std::max(aC2Vmax - aC1Vmin, aC1Vmax - aC2Vmin);
      if (aDistOuter <= aDistInner)
      {
        return true;
      }
    }
  }

  return false;
}

//! Find root of theIdx with path compression (Union-Find, amortized O(alpha(n))).
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

using FreeEdgeList = NCollection_Vector<BRepGraph_NodeId>;

//! Collect boundary (free) edges - edges with exactly one adjacent face.
//! Seam edges and removed edges are excluded. Optionally includes floating edges
//! (zero adjacent faces) in a separate set.
//! @param[in]  theGraph           source graph
//! @param[in]  theIncludeFloating if true, include edges with zero faces
//! @param[out] theFloatingEdges   set of floating edge indices (populated if theIncludeFloating)
//! @param[in]  theAllocator       optional allocator for temporary storage
//! @return ordered free-edge list following BRepGraph_EdgeIterator traversal order
FreeEdgeList findFreeEdges(const BRepGraph&                        theGraph,
                           bool                                    theIncludeFloating,
                           NCollection_Map<int>&                   theFloatingEdges,
                           const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
{
  const int  aCapacity  = std::max(1, theGraph.Topo().NbEdges());
  FreeEdgeList aFreeEdges(aCapacity, theAllocator);

  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
    {
      continue;
    }

    // Cached face count from reverse index - O(1).
    const int aFaceCount = theGraph.Topo().Edges().NbFaces(anEdgeId);

    // Exclude seam edges: on a UV-closed surface (cylinder, sphere, torus) with
    // two PCurves on the same face. Surface closure is verified first via
    // IsUClosed/IsVClosed + isocurve fallback, preventing false positives on
    // non-closed surfaces (planar faces with duplicate PCurve handles).
    if (aFaceCount == 1 && isSeamEdge(theGraph, anEdgeId))
    {
      continue;
    }

    if (aFaceCount == 1)
    {
      aFreeEdges.Append(anEdge.Id);
    }
    else if (aFaceCount == 0 && theIncludeFloating)
    {
      aFreeEdges.Append(anEdge.Id);
      theFloatingEdges.Add(anEdgeId.Index);
    }
  }
  return aFreeEdges;
}

//! Merge coincident vertices of free edges using KDTree range search
//! and Union-Find. Updates edge vertex references and reverse indices.
//! @param[in,out] theGraph      graph to modify
//! @param[in]     theFreeEdges  free edges from findFreeEdges()
//! @param[in]     theTolerance  maximum merge distance
//! @param[in,out] theResult     sewing result diagnostics
void assembleVertices(BRepGraph&                                   theGraph,
                      const FreeEdgeList&                          theFreeEdges,
                      double                                       theTolerance,
                      bool                                         theUseLightweightSearch,
                      const occ::handle<NCollection_IncAllocator>& theTmpAlloc)
{
  const int aNbFreeEdges = theFreeEdges.Length();
  NCollection_IndexedMap<int> aFreeVertexIndices(std::max(1, aNbFreeEdges * 2), theTmpAlloc);
  for (const BRepGraph_NodeId& aFreeEdgeNodeId : theFreeEdges)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      theGraph.Topo().Edges().Definition(BRepGraph_EdgeId(aFreeEdgeNodeId.Index));
    const BRepGraph::RefsView& aRefs = theGraph.Refs();
    if (anEdge.StartVertexRefId.IsValid())
    {
      const int aStartVtxIdx = aRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index;
      aFreeVertexIndices.Add(aStartVtxIdx);
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const int aEndVtxIdx = aRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index;
      aFreeVertexIndices.Add(aEndVtxIdx);
    }
  }

  const int aNbVertices = aFreeVertexIndices.Extent();
  if (aNbVertices == 0)
  {
    return;
  }

  NCollection_OrderedDataMap<int, int> aVertexMerge(aNbVertices, theTmpAlloc);
  NCollection_Array1<gp_Pnt> aVertexPoints(0, aNbVertices - 1);
  for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
  {
    const int anIdx = aFreeVertexIndices.FindKey(aVtxIter + 1);
    aVertexPoints.SetValue(aVtxIter,
                           BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(anIdx)));
  }

  if (theUseLightweightSearch)
  {
    for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
    {
      const int anIdxI = aFreeVertexIndices.FindKey(aVtxIter + 1);
      if (aVertexMerge.IsBound(anIdxI))
      {
        continue;
      }

      const gp_Pnt& aPntI       = aVertexPoints.Value(aVtxIter);
      bool          aMergedSelf = false;
      for (int aCandIter = aVtxIter + 1; aCandIter < aNbVertices && !aMergedSelf; ++aCandIter)
      {
        const int anIdxJ = aFreeVertexIndices.FindKey(aCandIter + 1);
        if (aVertexMerge.IsBound(anIdxJ) || aPntI.Distance(aVertexPoints.Value(aCandIter)) > theTolerance)
        {
          continue;
        }

        // Merge: keep vertex with smaller tolerance.
        const double aTolI =
          BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(anIdxI));
        const double aTolJ =
          BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(anIdxJ));
        if (aTolJ < aTolI)
        {
          aVertexMerge.Bind(anIdxI, anIdxJ);
          aMergedSelf = true;
        }
        else
        {
          aVertexMerge.Bind(anIdxJ, anIdxI);
        }
      }
    }
  }
  else
  {
    // Build the KDTree - O(n log n).
    NCollection_KDTree<gp_Pnt, 3> aTree;
    aTree.Build(aVertexPoints);

    // For each vertex (not already merged), find neighbors within tolerance.
    for (int aVtxIter = 0; aVtxIter < aNbVertices; ++aVtxIter)
    {
      const int anIdxI = aFreeVertexIndices.FindKey(aVtxIter + 1);
      if (aVertexMerge.IsBound(anIdxI))
      {
        continue;
      }

      const gp_Pnt& aPntI       = aVertexPoints.Value(aVtxIter);
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

        const int anIdxJ = aFreeVertexIndices.FindKey(anArrayIdx + 1);
        if (aVertexMerge.IsBound(anIdxJ))
        {
          return;
        }

        // Merge: keep vertex with smaller tolerance.
        const double aTolI = BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(anIdxI));
        const double aTolJ = BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(anIdxJ));
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
  const BRepGraph::RefsView& aMergeRefs = theGraph.Refs();
  for (const BRepGraph_NodeId& aFreeEdgeNodeId : theFreeEdges)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      theGraph.Topo().Edges().Definition(BRepGraph_EdgeId(aFreeEdgeNodeId.Index));
    if (anEdge.StartVertexRefId.IsValid())
    {
      const int  aStartIdx    = aMergeRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index;
      const int* aMergedStart = aVertexMerge.Seek(aStartIdx);
      if (aMergedStart != nullptr)
      {
        theGraph.Builder().MutVertexRef(anEdge.StartVertexRefId)->VertexDefId =
          BRepGraph_VertexId(*aMergedStart);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const int  aEndIdx    = aMergeRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index;
      const int* aMergedEnd = aVertexMerge.Seek(aEndIdx);
      if (aMergedEnd != nullptr)
      {
        theGraph.Builder().MutVertexRef(anEdge.EndVertexRefId)->VertexDefId =
          BRepGraph_VertexId(*aMergedEnd);
      }
    }
  }
}

//! Split free edges at T-vertex intersections (vertices from other edges
//! that project onto the edge within tolerance). Uses UBTree for spatial
//! candidate detection. Updates theFreeEdges list in place.
//! @param[in,out] theGraph      graph to modify
//! @param[in,out] theFreeEdges  free edges (may grow due to splits)
//! @param[in]     theOptions    sewing configuration
//! @param[in,out] theResult     sewing result diagnostics
void cutAtIntersections(BRepGraph&                                   theGraph,
                        FreeEdgeList&                                theFreeEdges,
                        const BRepGraphAlgo_Sewing::Options&         theOptions,
                        double                                       theMinTol,
                        const NCollection_Map<int>&                  theFloatingEdges,
                        int                                          theParallelWorkers,
                        const occ::handle<NCollection_IncAllocator>& theTmpAlloc)
{
  const int aNbFreeEdges = theFreeEdges.Length();
  if (aNbFreeEdges == 0)
  {
    return;
  }

  // Step 1: Collect all unique free-edge endpoint vertex indices.
  NCollection_IndexedMap<int> aFreeVertexIndices(std::max(1, aNbFreeEdges * 2), theTmpAlloc);
  for (int aFreeEdgeIter = 0; aFreeEdgeIter < aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      theGraph.Topo().Edges().Definition(BRepGraph_EdgeId(theFreeEdges.Value(aFreeEdgeIter).Index));
    const BRepGraph::RefsView& aCutRefs = theGraph.Refs();
    if (anEdge.StartVertexRefId.IsValid())
    {
      const int aStartVtxIdx = aCutRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index;
      aFreeVertexIndices.Add(aStartVtxIdx);
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const int aEndVtxIdx = aCutRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index;
      aFreeVertexIndices.Add(aEndVtxIdx);
    }
  }

  const int aNbVtx = aFreeVertexIndices.Extent();
  if (aNbVtx == 0)
  {
    return;
  }

  // Pre-compute vertex points for fast access.
  NCollection_Array1<gp_Pnt> aVtxPoints(0, aNbVtx - 1);
  for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
  {
    const int aVtxIdx = aFreeVertexIndices.FindKey(aVtxIter + 1);
    aVtxPoints.SetValue(aVtxIter,
                        BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aVtxIdx)));
  }

  struct SplitCandidate
  {
    double Param;
    int    VtxIdx;
  };

  // Step 2a (parallel): Find T-vertex split candidates per edge.
  // Each thread creates its own VtxBoxSelector (UBTree::Select is const).
  NCollection_Array1<NCollection_Vector<SplitCandidate>> aPerEdgeSplits(1, aNbFreeEdges);
  BRepGraph_ParallelPolicy::Workload                     aSplitSearchWork;
  aSplitSearchWork.PrimaryItems   = aNbFreeEdges;
  aSplitSearchWork.AuxiliaryItems = aNbVtx;
  const bool isParallelSplitSearch = BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel,
                                                                         theParallelWorkers,
                                                                         aSplitSearchWork);
  const bool isLightweightSplitSearch = theOptions.Parallel && !isParallelSplitSearch;
  NCollection_UBTree<int, Bnd_Box> aVtxTree;
  if (!isLightweightSplitSearch)
  {
    // Build UBTree on vertex point-boxes (no enlargement).
    // Edge BBoxes are already enlarged by tolerance before querying, so the tree overlap
    // test (edgeBox vs vtxPointBox) is equivalent to the original point-in-box test.
    // O(N log N) construction; replaces the O(E*V) brute-force inner loop.
    NCollection_UBTreeFiller<int, Bnd_Box> aTreeFiller(aVtxTree);
    for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
    {
      Bnd_Box aVtxBox;
      aVtxBox.Set(aVtxPoints.Value(aVtxIter));
      aTreeFiller.Add(aVtxIter, aVtxBox);
    }
    aTreeFiller.Fill();
  }

  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      const int                    aFreeEdgeIter = theIdx;
      const BRepGraph_NodeId       anEdgeNodeId  = theFreeEdges.Value(aFreeEdgeIter);
      const BRepGraph_EdgeId       anEdgeId(anEdgeNodeId.Index);
      const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anEdgeId);

      if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
          || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
      {
        return;
      }

      // Skip floating edges (no face - no cutting).
      if (theFloatingEdges.Contains(anEdgeNodeId.Index))
      {
        return;
      }

      const BRepGraph::RefsView& aParRefs  = theGraph.Refs();
      const int                  aStartIdx = anEdge.StartVertexRefId.IsValid()
                                               ? aParRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index
                                               : -1;
      const int                  aEndIdx   = anEdge.EndVertexRefId.IsValid()
                                               ? aParRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index
                                               : -1;

      GeomAdaptor_TransformedCurve aCurve = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeId);

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

      Bnd_Box aEdgeBBox = BRepGraphAlgo_BndLib::AddCached(theGraph, anEdgeId);
      if (aEdgeBBox.IsVoid())
      {
        return;
      }
      aEdgeBBox.Enlarge(theOptions.Tolerance);

      const double aTolSq      = theOptions.Tolerance * theOptions.Tolerance;
      const double aParamFirst = aCurve.FirstParameter();
      const double aParamLast  = aCurve.LastParameter();
      const double aParamEps   = Precision::Confusion();

      // Defer ExtremaPC_Curve construction until a candidate vertex passes
      // the bounding box filter. This avoids expensive BSpline grid building
      // for edges with no nearby vertices.
      std::optional<ExtremaPC_Curve> anExtPC;

      NCollection_Vector<SplitCandidate> aSplits;

      const auto aTryCandidate = [&](const int theVtxIter) {
        const int aVtxIdx = aFreeVertexIndices.FindKey(theVtxIter + 1);
        if (aVtxIdx == aStartIdx || aVtxIdx == aEndIdx)
        {
          return;
        }

        const gp_Pnt& aVtxPnt = aVtxPoints.Value(theVtxIter);
        if (isLightweightSplitSearch && aEdgeBBox.IsOut(aVtxPnt))
        {
          return;
        }

        if (!anExtPC.has_value())
        {
          anExtPC.emplace(aCurve);
        }
        const ExtremaPC::Result& aRes =
          anExtPC->Perform(aVtxPnt, Precision::Confusion(), ExtremaPC::SearchMode::Min);
        if (!aRes.IsDone() || aRes.NbExt() == 0)
        {
          return;
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
      };

      if (isLightweightSplitSearch)
      {
        for (int aVtxIter = 0; aVtxIter < aNbVtx; ++aVtxIter)
        {
          aTryCandidate(aVtxIter);
        }
      }
      else
      {
        // Query UBTree for vertices whose boxes overlap the edge box - O(log V).
        VtxBoxSelector aSelector;
        aSelector.SetCurrentBox(aEdgeBBox);
        aVtxTree.Select(aSelector);

        const NCollection_Vector<int>& aCandidates = aSelector.Results();
        for (int aCandIter = 0; aCandIter < aCandidates.Length(); ++aCandIter)
        {
          aTryCandidate(aCandidates.Value(aCandIter));
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
      NCollection_Vector<SplitCandidate>& aUniqueSplits = aPerEdgeSplits.ChangeValue(aFreeEdgeIter + 1);
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
    !isParallelSplitSearch);

  bool hasAnySplits = false;
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    if (!aPerEdgeSplits.Value(aFreeEdgeIter).IsEmpty())
    {
      hasAnySplits = true;
      break;
    }
  }

  if (!hasAnySplits)
  {
    return;
  }

  // Step 2b (sequential): Apply splits directly while rebuilding the free-edge list.
  FreeEdgeList aRebuiltFreeEdges(std::max(THE_INIT_VECTOR_CAPACITY, aNbFreeEdges));
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_NodeId                anEdgeId      = theFreeEdges.Value(aFreeEdgeIter - 1);
    const NCollection_Vector<SplitCandidate>& aUniqueSplits = aPerEdgeSplits.Value(aFreeEdgeIter);
    if (aUniqueSplits.IsEmpty())
    {
      aRebuiltFreeEdges.Append(anEdgeId);
    }
    else
    {
      BRepGraph_NodeId aCurrentEdge = anEdgeId;
      for (int aSplitIter = 0; aSplitIter < aUniqueSplits.Length(); ++aSplitIter)
      {
        const SplitCandidate& aCand = aUniqueSplits.Value(aSplitIter);
        const BRepGraph_VertexId aVtxNodeId(aCand.VtxIdx);
        BRepGraph_EdgeId         aSubA, aSubB;
        theGraph.Builder().SplitEdge(BRepGraph_EdgeId::FromNodeId(aCurrentEdge),
                                     aVtxNodeId,
                                     aCand.Param,
                                     aSubA,
                                     aSubB);
        aRebuiltFreeEdges.Append(aSubA);
        aCurrentEdge = aSubB;
      }
      aRebuiltFreeEdges.Append(aCurrentEdge);
      // Invalidate cached bbox for the original edge since its geometry changed.
      BRepGraphAlgo_BndLib::InvalidateCached(theGraph, anEdgeId);
    }
  }

  theFreeEdges = std::move(aRebuiltFreeEdges);
}

//! Detect sewing candidates via bounding-box overlap using UBTree.
//! Returns local adjacency lists indexed by free-edge position (1..N).
//! Candidate pairs are ephemeral - they live only until edge matching.
//! @param[in]  theGraph         source graph
//! @param[in]  theFreeEdges     free edges to match
//! @param[in]  theFloatingEdges floating edge indices (excluded from candidates)
//! @param[in]  theTolerance     distance tolerance for BBox expansion
//! @param[in]  theParallel      enable parallel BBox computation
//! @return per-edge adjacency lists of candidate indices
NCollection_Array1<NCollection_Vector<int>> detectCandidates(
  BRepGraph&                                    theGraph,
  const FreeEdgeList&                           theFreeEdges,
  const NCollection_Array1<int>&                theFaceOfPos,
  const BRepGraphAlgo_Sewing::Options&          theOptions,
  int                                           theParallelWorkers,
  const occ::handle<NCollection_BaseAllocator>& theWorkAlloc)
{
  const int                                   aNbFreeEdges = theFreeEdges.Length();
  NCollection_Array1<NCollection_Vector<int>> anAdj(1, aNbFreeEdges);
  if (!theWorkAlloc.IsNull())
  {
    for (int i = 1; i <= aNbFreeEdges; ++i)
    {
      anAdj.ChangeValue(i) = NCollection_Vector<int>(THE_INIT_VECTOR_CAPACITY, theWorkAlloc);
    }
  }
  if (aNbFreeEdges == 0)
  {
    return anAdj;
  }

  // Step 1: Precompute bounding boxes in parallel.
  NCollection_Array1<Bnd_Box> aBBoxes(1, aNbFreeEdges);
  BRepGraph_ParallelPolicy::Workload aBBoxWork;
  aBBoxWork.PrimaryItems = aNbFreeEdges;
  const bool isParallelBBoxBuild = BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel,
                                                                       theParallelWorkers,
                                                                       aBBoxWork);
  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      Bnd_Box aBox = BRepGraphAlgo_BndLib::AddCached(theGraph, theFreeEdges.Value(theIdx));
      if (!aBox.IsVoid())
      {
        aBox.Enlarge(theOptions.Tolerance);
      }
      aBBoxes.SetValue(theIdx + 1, aBox);
    },
    !isParallelBBoxBuild);

  // Precompute surface closure flags per face to avoid repeated
  // expensive IsUClosed/IsVClosed calls inside the parallel candidate loop.
  NCollection_DataMap<int, FaceClosureInfo> aClosureCache;
  for (int aPosI = 1; aPosI <= aNbFreeEdges; ++aPosI)
  {
    const int aFaceIdx = theFaceOfPos(aPosI);
    if (aFaceIdx < 0 || aClosureCache.IsBound(aFaceIdx))
    {
      continue;
    }
    const BRepGraph_FaceId aFaceId(aFaceIdx);
    FaceClosureInfo        anInfo;
    if (BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
    {
      const occ::handle<Geom_Surface> aBasis =
        basisSurface(BRepGraph_Tool::Face::Surface(theGraph, aFaceId));
      anInfo.IsUClosed = aBasis->IsUClosed();
      anInfo.IsVClosed = aBasis->IsVClosed();
    }
    aClosureCache.Bind(aFaceIdx, anInfo);
  }

  BRepGraph_ParallelPolicy::Workload aCandidateSearchWork;
  aCandidateSearchWork.PrimaryItems = aNbFreeEdges;
  const bool isParallelCandidateSearch = BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel,
                                                                              theParallelWorkers,
                                                                              aCandidateSearchWork);
  if (theOptions.Parallel && !isParallelCandidateSearch)
  {
    for (int aPosI = 1; aPosI <= aNbFreeEdges; ++aPosI)
    {
      const Bnd_Box& aBoxI = aBBoxes.Value(aPosI);
      if (aBoxI.IsVoid())
      {
        continue;
      }

      for (int aPosJ = aPosI + 1; aPosJ <= aNbFreeEdges; ++aPosJ)
      {
        const Bnd_Box& aBoxJ = aBBoxes.Value(aPosJ);
        if (aBoxJ.IsVoid() || aBoxI.IsOut(aBoxJ))
        {
          continue;
        }

        const int aFaceI = theFaceOfPos(aPosI);
        const int aFaceJ = theFaceOfPos(aPosJ);
        if (aFaceI >= 0 && aFaceI == aFaceJ)
        {
          const FaceClosureInfo* aClosure = aClosureCache.Seek(aFaceI);
          if (!canSewSameFaceEdges(theGraph,
                                   BRepGraph_EdgeId(theFreeEdges.Value(aPosI - 1).Index),
                                   BRepGraph_EdgeId(theFreeEdges.Value(aPosJ - 1).Index),
                                   BRepGraph_FaceId(aFaceI),
                                   aClosure))
          {
            continue;
          }
        }

        anAdj.ChangeValue(aPosI).Append(aPosJ);
        anAdj.ChangeValue(aPosJ).Append(aPosI);
      }
    }
    return anAdj;
  }

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

  const auto isAcceptedPair = [&](const int thePosI, const int thePosJ) {
    const int aFaceI = theFaceOfPos(thePosI);
    const int aFaceJ = theFaceOfPos(thePosJ);
    if (aFaceI >= 0 && aFaceI == aFaceJ)
    {
      const FaceClosureInfo* aClosure = aClosureCache.Seek(aFaceI);
      return canSewSameFaceEdges(theGraph,
                                 BRepGraph_EdgeId(theFreeEdges.Value(thePosI - 1).Index),
                                 BRepGraph_EdgeId(theFreeEdges.Value(thePosJ - 1).Index),
                                 BRepGraph_FaceId(aFaceI),
                                 aClosure);
    }
    return true;
  };

  // Step 3: Find overlapping position pairs and emit them via callback.
  const auto aSearchFn = [&](int theIdxI, const auto& theAppendPair) {
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
        if (!isAcceptedPair(anI, anJ))
        {
          return;
        }
        theAppendPair(anI, anJ);
      }
    });
  };

  if (isParallelCandidateSearch)
  {
    NCollection_Array1<NCollection_Vector<int>> aPerEdgeAdj(0, aNbFreeEdges - 1);
    OSD_Parallel::For(
      0,
      aNbFreeEdges,
      [&](int theIdxI) {
        NCollection_Vector<int>& aLocalAdj = aPerEdgeAdj.ChangeValue(theIdxI);
        aSearchFn(theIdxI, [&](int, int thePosJ) { aLocalAdj.Append(thePosJ); });
      },
      false);

    for (int anEdgeIdx = 0; anEdgeIdx < aNbFreeEdges; ++anEdgeIdx)
    {
      const int                      aPosI     = anEdgeIdx + 1;
      const NCollection_Vector<int>& anEdgeAdj = aPerEdgeAdj.Value(anEdgeIdx);
      for (int aPairIdx = 0; aPairIdx < anEdgeAdj.Length(); ++aPairIdx)
      {
        const int aPosJ = anEdgeAdj.Value(aPairIdx);
        anAdj.ChangeValue(aPosI).Append(aPosJ);
        anAdj.ChangeValue(aPosJ).Append(aPosI);
      }
    }
  }
  else
  {
    for (int anEdgeIdx = 0; anEdgeIdx < aNbFreeEdges; ++anEdgeIdx)
    {
      aSearchFn(anEdgeIdx, [&](int thePosI, int thePosJ) {
        anAdj.ChangeValue(thePosI).Append(thePosJ);
        anAdj.ChangeValue(thePosJ).Append(thePosI);
      });
    }
  }

  return anAdj;
}

//! Match free-edge pairs by geometric proximity scoring.
//! Uses local adjacency from detectCandidates() and inline Union-Find for
//! connected component grouping. Greedy best-score pairing within each component.
//! @param[in]  theGraph        source graph
//! @param[in]  theFreeEdges    free edges from findFreeEdges()
//! @param[in]  theAdj          adjacency lists from detectCandidates()
//! @param[in]  theOptions      sewing configuration
//! @param[in,out] theResult    sewing result diagnostics
//! @return matched edge pairs ready for merging
NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> matchFreeEdges(
  const BRepGraph&                                   theGraph,
  const FreeEdgeList&                                theFreeEdges,
  const NCollection_Array1<NCollection_Vector<int>>& theAdjacency,
  const BRepGraphAlgo_Sewing::Options&               theOptions,
  double                                             theMinTol,
  int                                                theParallelWorkers,
  const occ::handle<NCollection_IncAllocator>&       theTmpAlloc)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult(
    THE_INIT_VECTOR_CAPACITY,
    theTmpAlloc);
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
      const int aNext           = aParent(aCur);
      aParent.ChangeValue(aCur) = aRoot;
      aCur                      = aNext;
    }
    return aRoot;
  };

  for (int aPosI = 1; aPosI <= aNbFreeEdges; ++aPosI)
  {
    const NCollection_Vector<int>& aCands = theAdjacency(aPosI);
    for (int j = 0; j < aCands.Length(); ++j)
    {
      const int aPosJ  = aCands(j);
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
  int                           aNbComponents = 0;
  NCollection_Array1<int>       aCompByPos(1, aNbFreeEdges);
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

  int aTotalAdjacencyEntries = 0;
  for (int aPos = 1; aPos <= aNbFreeEdges; ++aPos)
  {
    aTotalAdjacencyEntries += theAdjacency(aPos).Length();
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

  const bool isLocalTolMode = theOptions.LocalTolerancesMode;

  NCollection_Array1<MatchResult>       aPerEdgeMatch(1, aNbFreeEdges);
  NCollection_Array1<EdgeEndpointCache> aEndpointByPos(1, aNbFreeEdges);
  const BRepGraph::TopoView&            aTopo = theGraph.Topo();
  const BRepGraph::RefsView&            aRefs = theGraph.Refs();
  for (int aPos = 1; aPos <= aNbFreeEdges; ++aPos)
  {
    const BRepGraph_EdgeId       anEdgeId(theFreeEdges.Value(aPos - 1).Index);
    const BRepGraphInc::EdgeDef& anEdgeNode = aTopo.Edges().Definition(anEdgeId);
    const gp_Pnt                 aStart =
      BRepGraph_Tool::Vertex::Pnt(theGraph, aRefs.Vertex(anEdgeNode.StartVertexRefId).VertexDefId);
    const gp_Pnt aEnd =
      BRepGraph_Tool::Vertex::Pnt(theGraph, aRefs.Vertex(anEdgeNode.EndVertexRefId).VertexDefId);
    EdgeEndpointCache& anEndpoint = aEndpointByPos.ChangeValue(aPos);
    anEndpoint.Start              = aStart;
    anEndpoint.End                = aEnd;
    anEndpoint.Chord              = aStart.Distance(aEnd);
    if (isLocalTolMode)
    {
      anEndpoint.Tolerance = BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeId);
    }
  }

  BRepGraph_ParallelPolicy::Workload aMatchWork;
  aMatchWork.PrimaryItems     = aNbFreeEdges;
  aMatchWork.InteractionCount = aTotalAdjacencyEntries;
  const bool isParallelMatching = BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel,
                                                                      theParallelWorkers,
                                                                      aMatchWork);

  if (theOptions.Parallel && !isParallelMatching && !theOptions.NonManifoldMode)
  {
    NCollection_Vector<ScoredPair> aScoredPairs(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

    for (int anI = 1; anI <= aNbFreeEdges; ++anI)
    {
      const BRepGraph_NodeId   anIdA       = theFreeEdges.Value(anI - 1);
      const BRepGraph_EdgeId   anEdgeA(anIdA.Index);
      const EdgeEndpointCache& anEndpointA = aEndpointByPos.Value(anI);

      if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeA)
          || !isEdgeCompatibleMinTol(anEndpointA, theMinTol))
      {
        continue;
      }

      GeomAdaptor_TransformedCurve aCurveA = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeA);
      GCPnts_UniformAbscissa       aSamplerA(aCurveA, THE_NB_EDGE_MATCH_SAMPLES);
      if (!aSamplerA.IsDone() || aSamplerA.NbPoints() < 2)
      {
        continue;
      }

      NCollection_LocalArray<gp_Pnt, 8> aSamplePtsABuf(aSamplerA.NbPoints());
      NCollection_Array1<gp_Pnt>        aSamplePtsA(aSamplePtsABuf[0], 1, aSamplerA.NbPoints());
      for (int aSmpIter = 1; aSmpIter <= aSamplerA.NbPoints(); ++aSmpIter)
      {
        aSamplePtsA.SetValue(aSmpIter, aCurveA.EvalD0(aSamplerA.Parameter(aSmpIter)));
      }

      ExtremaPC_Curve anExtPCRevA(aCurveA);
      const gp_Pnt&   aStartA = anEndpointA.Start;
      const gp_Pnt&   aEndA   = anEndpointA.End;

      const NCollection_Vector<int>& aCands = theAdjacency(anI);
      for (int aCandIter = 0; aCandIter < aCands.Length(); ++aCandIter)
      {
        const int aPosB = aCands.Value(aCandIter);
        if (aPosB <= anI)
        {
          continue;
        }

        const BRepGraph_NodeId   anIdB       = theFreeEdges.Value(aPosB - 1);
        const EdgeEndpointCache& anEndpointB = aEndpointByPos.Value(aPosB);

        double aWorkTol = theOptions.Tolerance;
        if (isLocalTolMode)
        {
          aWorkTol += anEndpointA.Tolerance + anEndpointB.Tolerance;
        }

        const BRepGraph_EdgeId anEdgeIdB(anIdB.Index);
        if (!areEdgesCompatibleSampled(theGraph,
                                       anEdgeA,
                                       anEdgeIdB,
                                       anEndpointA,
                                       anEndpointB,
                                       aSamplePtsA,
                                       anExtPCRevA,
                                       aWorkTol,
                                       THE_NB_EDGE_MATCH_SAMPLES,
                                       THE_MAX_CHORD_RATIO,
                                       THE_HIGH_CONFIDENCE_RATIO))
        {
          continue;
        }

        ScoredPair aPair;
        aPair.EdgeA = anIdA;
        aPair.EdgeB = anIdB;
        aPair.Score = edgeEndpointPairScore(aStartA, aEndA, anEndpointB.Start, anEndpointB.End);
        aScoredPairs.Append(aPair);
      }
    }

    const int aNbScored = aScoredPairs.Length();
    if (aNbScored == 0)
    {
      return aResult;
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

      aResult.Append({aPair.EdgeA, aPair.EdgeB});
      aConsumed.Add(aPair.EdgeA.Index);
      aConsumed.Add(aPair.EdgeB.Index);
    }
    return aResult;
  }

  OSD_Parallel::For(
    0,
    aNbFreeEdges,
    [&](int theIdx) {
      const int              anI   = theIdx + 1;
      const BRepGraph_NodeId anIdA = theFreeEdges.Value(anI - 1);
      const BRepGraph_EdgeId anEdgeA(anIdA.Index);
      const int              aComp = aCompByPos.Value(anI);
      if (aComp == 0)
      {
        return;
      }
      const EdgeEndpointCache& anEndpointA = aEndpointByPos.Value(anI);

      MatchResult& aMatch = aPerEdgeMatch.ChangeValue(anI);

      if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeA))
      {
        return;
      }

      GeomAdaptor_TransformedCurve aCurveA = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeA);

      // Skip edges shorter than MinTolerance.
      if (!isEdgeCompatibleMinTol(anEndpointA, theMinTol))
      {
        return;
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

      const gp_Pnt& aStartA = anEndpointA.Start;
      const gp_Pnt& aEndA   = anEndpointA.End;

      // Iterate candidates from local adjacency (replaces ForEachOutOfKind).
      const NCollection_Vector<int>& aCands = theAdjacency(anI);
      for (int aCandIter = 0; aCandIter < aCands.Length(); ++aCandIter)
      {
        const int aPosB = aCands.Value(aCandIter);
        if (aCompByPos.Value(aPosB) != aComp)
        {
          continue;
        }
        const BRepGraph_NodeId   anIdB       = theFreeEdges.Value(aPosB - 1);
        const EdgeEndpointCache& anEndpointB = aEndpointByPos.Value(aPosB);

        // LocalTolerancesMode: use adaptive work tolerance.
        double aWorkTol = theOptions.Tolerance;
        if (isLocalTolMode)
        {
          aWorkTol += anEndpointA.Tolerance + anEndpointB.Tolerance;
        }

        const BRepGraph_EdgeId anEdgeIdB(anIdB.Index);
        if (areEdgesCompatibleSampled(theGraph,
                                      anEdgeA,
                                      anEdgeIdB,
                                      anEndpointA,
                                      anEndpointB,
                                      aSamplePtsA,
                                      anExtPCRevA,
                                      aWorkTol,
                                      THE_NB_EDGE_MATCH_SAMPLES,
                                      THE_MAX_CHORD_RATIO,
                                      THE_HIGH_CONFIDENCE_RATIO))
        {
          const double aScore =
            edgeEndpointPairScore(aStartA, aEndA, anEndpointB.Start, anEndpointB.End);
          if (aScore < aMatch.BestScore)
          {
            aMatch.BestScore = aScore;
            aMatch.BestMatch = anIdB;
          }
        }
      }
    },
    !isParallelMatching);

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
    aPair.EdgeA = theFreeEdges.Value(aEdgeIter - 1);
    aPair.EdgeB = aMatch.BestMatch;
    aPair.Score = aMatch.BestScore;
    aCompScoredPairs.ChangeValue(aComp).Append(aPair);
  }

  NCollection_Array1<NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>> aCompPairs(
    1,
    aNbComponents);

  int aTotalScoredPairs = 0;
  for (int aCompIter = 1; aCompIter <= aNbComponents; ++aCompIter)
  {
    aTotalScoredPairs += aCompScoredPairs.Value(aCompIter).Length();
  }

  BRepGraph_ParallelPolicy::Workload aComponentReduceWork;
  aComponentReduceWork.PrimaryItems     = aNbComponents;
  aComponentReduceWork.InteractionCount = aTotalScoredPairs;
  const bool isParallelComponentReduce = BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel,
                                                                              theParallelWorkers,
                                                                              aComponentReduceWork);

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
      // For equal-length edges, use lower index as EdgeA for deterministic ordering
      // and to deduplicate symmetric pairs (A->B and B->A normalize to the same form).
      if (theOptions.NonManifoldMode)
      {
        for (int anIdx = 0; anIdx < aNbScored; ++anIdx)
        {
          ScoredPair& aPair = aSortedPairs.ChangeValue(anIdx);
          double      aLenA = 0.0, aLenB = 0.0;
          try
          {
            OCC_CATCH_SIGNALS
            GeomAdaptor_TransformedCurve aCrvA =
              BRepGraph_Tool::Edge::CurveAdaptor(theGraph, BRepGraph_EdgeId(aPair.EdgeA.Index));
            aLenA = GCPnts_AbscissaPoint::Length(aCrvA);
          }
          catch (const Standard_Failure&)
          {
            const auto [aFirstA, aLastA] =
              BRepGraph_Tool::Edge::Range(theGraph, BRepGraph_EdgeId(aPair.EdgeA.Index));
            aLenA = aLastA - aFirstA;
          }
          try
          {
            OCC_CATCH_SIGNALS
            GeomAdaptor_TransformedCurve aCrvB =
              BRepGraph_Tool::Edge::CurveAdaptor(theGraph, BRepGraph_EdgeId(aPair.EdgeB.Index));
            aLenB = GCPnts_AbscissaPoint::Length(aCrvB);
          }
          catch (const Standard_Failure&)
          {
            const auto [aFirstB, aLastB] =
              BRepGraph_Tool::Edge::Range(theGraph, BRepGraph_EdgeId(aPair.EdgeB.Index));
            aLenB = aLastB - aFirstB;
          }
          if (aLenB > aLenA
              || (aLenB == aLenA && aPair.EdgeB.Index < aPair.EdgeA.Index))
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
    !isParallelComponentReduce);

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

//! Merge matched edge pairs by transferring PCurves and updating wire references.
//! Operates entirely on graph entities (no TopoDS shape access).
//! @param[in,out] theGraph      graph to modify
//! @param[in]     thePairs      matched pairs from matchFreeEdges()
//! @param[in]     theOptions    sewing configuration
//! @param[in,out] theResult     sewing result diagnostics
//! @return number of successfully sewn edge pairs
int mergeMatchedEdges(
  BRepGraph&                                                               theGraph,
  const NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>& thePairs,
  const BRepGraphAlgo_Sewing::Options&                                     theOptions,
  const bool                                                                theParallelRequested,
  NCollection_IndexedMap<BRepGraph_EdgeId>&                                theSewnEdgeIndices,
  BRepGraphAlgo_Sewing::Result&                                            theResult,
  const occ::handle<NCollection_IncAllocator>&                             theTmpAlloc)
{
  int aSewnCount = 0;
  bool hasNonSameParameterInput = false;

  // Accumulate history pairs for batch recording (Opt 5).
  // Use the scoped temp allocator - these vectors are consumed by RecordBatch then discarded.
  NCollection_Vector<BRepGraph_NodeId> aHistOriginals(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Vector<BRepGraph_NodeId> aHistReplacements(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

  // Replacement map for resolving edges removed by earlier pairs in the loop.
  NCollection_DataMap<int, int> aReplacementMap(16, theTmpAlloc);

  for (int aPairIter = 0; aPairIter < thePairs.Length(); ++aPairIter)
  {
    const auto& [anOrigIdA, anOrigIdB] = thePairs.Value(aPairIter);

    // Resolve both edges through the replacement chain.
    BRepGraph_NodeId anIdA = anOrigIdA;
    for (const int* aRepl = aReplacementMap.Seek(anIdA.Index); aRepl != nullptr;
         aRepl            = aReplacementMap.Seek(anIdA.Index))
      anIdA = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, *aRepl);

    BRepGraph_NodeId anIdB = anOrigIdB;
    for (const int* aRepl = aReplacementMap.Seek(anIdB.Index); aRepl != nullptr;
         aRepl            = aReplacementMap.Seek(anIdB.Index))
      anIdB = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, *aRepl);

    const BRepGraph_EdgeId anEdgeIdA(anIdA.Index);
    const BRepGraph_EdgeId anEdgeIdB(anIdB.Index);

    // Skip self-merges and already-removed edges.
    if (anIdA.Index == anIdB.Index)
      continue;
    if (theGraph.Topo().Edges().Definition(anEdgeIdA).IsRemoved || theGraph.Topo().Edges().Definition(anEdgeIdB).IsRemoved)
      continue;

    const BRepGraphInc::EdgeDef& aRemoveEdge = theGraph.Topo().Edges().Definition(anEdgeIdB);
    const BRepGraphInc::EdgeDef& aKeepEdge   = theGraph.Topo().Edges().Definition(anEdgeIdA);
    hasNonSameParameterInput =
      hasNonSameParameterInput || !aKeepEdge.SameParameter || !aRemoveEdge.SameParameter;

    // 1. Tolerance merge (graph-only).
    const double aMergedTol = std::max(BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeIdA),
                                       BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeIdB));

    // MaxTolerance guard: skip pair if merged tolerance exceeds upper bound.
    if (aMergedTol > theOptions.MaxTolerance)
    {
      ++theResult.NbRejectedByTolerance;
      continue;
    }

    theSewnEdgeIndices.Add(anEdgeIdA);
    theGraph.Builder().MutEdge(anEdgeIdA)->Tolerance = aMergedTol;

    // 2. PCurve transfer from remove-edge to keep-edge via CoEdge data.
    const NCollection_Vector<BRepGraph_CoEdgeId>& aRemoveCoEdges =
      theGraph.Topo().Edges().CoEdges(anEdgeIdB);
    for (int aCEIter = 0; aCEIter < aRemoveCoEdges.Length(); ++aCEIter)
    {
      const BRepGraphInc::CoEdgeDef& aRemoveCE =
        theGraph.Topo().CoEdges().Definition(aRemoveCoEdges.Value(aCEIter));
      if (aRemoveCE.Curve2DRepId.IsValid())
      {
        const occ::handle<Geom2d_Curve>& aRemovePCurve =
          BRepGraph_Tool::CoEdge::PCurve(theGraph, aRemoveCoEdges.Value(aCEIter));
        // Add PCurve entry to keep-edge via graph API; preserve original orientation
        // so that seam edges (REVERSED orientation) are correctly reconstructed as C2.
        theGraph.Builder().AddPCurveToEdge(anEdgeIdA,
                                           aRemoveCE.FaceDefId,
                                           aRemovePCurve,
                                           aRemoveCE.ParamFirst,
                                           aRemoveCE.ParamLast,
                                           aRemoveCE.Sense);
      }
    }

    // 3. Determine direction correspondence from vertex positions.
    const BRepGraph::RefsView& aSewRefs = theGraph.Refs();
    const gp_Pnt               aKeepStart =
      BRepGraph_Tool::Vertex::Pnt(theGraph,
                                  aSewRefs.Vertex(aKeepEdge.StartVertexRefId).VertexDefId);
    const gp_Pnt aRemoveStart =
      BRepGraph_Tool::Vertex::Pnt(theGraph,
                                  aSewRefs.Vertex(aRemoveEdge.StartVertexRefId).VertexDefId);
    const bool isReversed = aKeepStart.Distance(aRemoveStart) > theOptions.Tolerance;

    // 4. Update wire entries referencing the remove-edge.
    // Copy the wire list since ReplaceEdgeInWire modifies the reverse index.
    const NCollection_Vector<BRepGraph_WireId> aWires = theGraph.Topo().Edges().Wires(anEdgeIdB);
    for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
    {
      theGraph.Builder().ReplaceEdgeInWire(aWires.Value(aWIdx),
                                          anEdgeIdB,
                                          anEdgeIdA,
                                          isReversed);
    }

    // Remove the old edge properly - decrements NbActiveEdges, clears cache.
    // Pass replacement (anIdA) so layers can migrate data from remove-edge to keep-edge.
    theGraph.Builder().RemoveNode(anIdB, anIdA);
    aReplacementMap.Bind(anIdB.Index, anIdA.Index);

    // 5. Accumulate for batch history recording.
    aHistOriginals.Append(anIdB);
    aHistReplacements.Append(anIdA);

    theResult.SewnEdgePairs.Append(BRepGraph_EdgeId(anIdA.Index));
    ++aSewnCount;
  }

  // 5b. Batch history recording (single HistoryRecord for all merges).
  if (!aHistOriginals.IsEmpty())
  {
    static const TCollection_AsciiString THE_MERGE_LABEL("Sewing:MergeEdge");
    TCollection_AsciiString              anExtra("tol=");
    anExtra += TCollection_AsciiString(theOptions.Tolerance);
    anExtra += " pairs=";
    anExtra += TCollection_AsciiString(aHistOriginals.Length());
    theGraph.History().RecordBatch(THE_MERGE_LABEL, aHistOriginals, aHistReplacements, anExtra);
  }

  // 6. SameParameter enforcement (graph-only, parallel).
  if (theOptions.SameParameterMode
      && !theSewnEdgeIndices.IsEmpty()
      && (!theParallelRequested || hasNonSameParameterInput))
  {
    (void)BRepGraphAlgo_SameParameter::Perform(theGraph, theSewnEdgeIndices, theOptions.Tolerance);
  }

  return aSewnCount;
}

//! Post-process sewn edges for tolerance consistency.
//! Currently a no-op: SameParameter enforcement is handled during merging.
void processEdges(
  [[maybe_unused]] BRepGraph&                                      theGraph,
  [[maybe_unused]] const NCollection_IndexedMap<BRepGraph_EdgeId>& theSewnEdgeIndices,
  [[maybe_unused]] const BRepGraphAlgo_Sewing::Options&            theOptions)
{
  // Sewn edges already have correct tolerances from the merge phase.
  // Vertex-tolerance re-inflation is skipped: the vertex merge phase may
  // have inflated vertex tolerances beyond the geometric need, and propagating
  // that to edges would undo the careful tolerance set during merging.
}

//! Reconstruct a TopoDS_Shape from the current graph state.
//! Dispatches to the appropriate root node (Compound, Solid, Shell, or Face).
//! @param[in] theGraph source graph
//! @return reconstructed shape
TopoDS_Shape reconstructFromGraph(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  if (aDefs.NbCompounds() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_CompoundId(0));
  }
  else if (aDefs.NbCompSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_CompSolidId(0));
  }
  else if (aDefs.NbSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_SolidId(0));
  }
  else if (aDefs.NbShells() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_ShellId(0));
  }
  else
  {
    // Flat faces - wrap in compound.
    BRep_Builder    aBB;
    TopoDS_Compound aResultCompound;
    aBB.MakeCompound(aResultCompound);
    for (int aFaceIdx = 0; aFaceIdx < aDefs.NbFaces(); ++aFaceIdx)
    {
      if (!aDefs.Faces().Definition(BRepGraph_FaceId(aFaceIdx)).IsRemoved)
      {
        aBB.Add(aResultCompound, theGraph.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx)));
      }
    }
    return aResultCompound;
  }
}

//! Convert remaining free edges that form degenerate wires into
//! degenerate edges (collapsed to a point on surface).
//! @param[in,out] theGraph  graph to modify
//! @param[in,out] theResult sewing result diagnostics
void convertDegenerateEdges(BRepGraph&                                   theGraph,
                            BRepGraphAlgo_Sewing::Result&                theResult,
                            const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  NCollection_Map<int> aFloatingEdges;
  const FreeEdgeList   aFreeEdges = findFreeEdges(theGraph, false, aFloatingEdges, theAllocator);
  if (aFreeEdges.IsEmpty())
  {
    return;
  }

  // For each free edge, check if it forms a degenerate "wire" by itself:
  // edge chord length <= sum of vertex tolerances.
  for (const BRepGraph_NodeId& aFreeEdgeNodeId : aFreeEdges)
  {
    const BRepGraph_EdgeId       anEdgeId = BRepGraph_EdgeId::FromNodeId(aFreeEdgeNodeId);
    const BRepGraphInc::EdgeDef& anEdge   = theGraph.Topo().Edges().Definition(anEdgeId);

    if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
        || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      continue;
    }

    // Compute edge chord length via 3-point check (start, mid, end).
    const occ::handle<Geom_Curve>& anEdgeCurve3d = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
    const auto [aEdgeFirst, aEdgeLast]           = BRepGraph_Tool::Edge::Range(theGraph, anEdgeId);
    const gp_Pnt aPtFirst                        = anEdgeCurve3d->EvalD0(aEdgeFirst);
    const gp_Pnt aPtLast                         = anEdgeCurve3d->EvalD0(aEdgeLast);
    const gp_Pnt aPtMid = anEdgeCurve3d->EvalD0((aEdgeFirst + aEdgeLast) * 0.5);

    double aLength = 0.0;
    if (aPtFirst.Distance(aPtLast) > aPtFirst.Distance(aPtMid))
    {
      aLength = aPtFirst.Distance(aPtLast);
    }
    else
    {
      // Curved edge shorter than chord - use full arc length.
      try
      {
        OCC_CATCH_SIGNALS
        GeomAdaptor_TransformedCurve aCurve =
          BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeId);
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
    const BRepGraph::RefsView& aDegRefs      = theGraph.Refs();
    double                     aVertexTolSum = 0.0;
    if (anEdge.StartVertexRefId.IsValid())
    {
      aVertexTolSum +=
        BRepGraph_Tool::Vertex::Tolerance(theGraph,
                                          aDegRefs.Vertex(anEdge.StartVertexRefId).VertexDefId);
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      aVertexTolSum +=
        BRepGraph_Tool::Vertex::Tolerance(theGraph,
                                          aDegRefs.Vertex(anEdge.EndVertexRefId).VertexDefId);
    }

    if (aLength <= aVertexTolSum)
    {
      // Mark as degenerate: clear 3D curve, merge vertices to midpoint.
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMutEdge = theGraph.Builder().MutEdge(anEdgeId);
      aMutEdge->IsDegenerate                           = true;
      aMutEdge->Curve3DRepId                           = BRepGraph_Curve3DRepId();

      // Merge start/end vertices if they differ.
      const BRepGraph_VertexId aStartVertexId =
        anEdge.StartVertexRefId.IsValid() ? aDegRefs.Vertex(anEdge.StartVertexRefId).VertexDefId
                                          : BRepGraph_VertexId();
      const BRepGraph_VertexId aEndVertexId = anEdge.EndVertexRefId.IsValid()
                                                ? aDegRefs.Vertex(anEdge.EndVertexRefId).VertexDefId
                                                : BRepGraph_VertexId();
      if (aStartVertexId.IsValid() && aEndVertexId.IsValid() && aStartVertexId != aEndVertexId)
      {
        const double aTolStart = BRepGraph_Tool::Vertex::Tolerance(theGraph, aStartVertexId);
        const double aTolEnd   = BRepGraph_Tool::Vertex::Tolerance(theGraph, aEndVertexId);
        const double aD1       = aTolStart + aPtMid.Distance(aPtFirst);
        const double aD2       = aTolEnd + aPtMid.Distance(aPtLast);
        const double aNewTol   = std::max(aD1, aD2);

        // Keep start vertex, update it to midpoint.
        BRepGraph_MutGuard<BRepGraphInc::VertexDef> aVtx =
          theGraph.Builder().MutVertex(aStartVertexId);
        aVtx->Point     = aPtMid;
        aVtx->Tolerance = aNewTol;
        // Point end vertex ref to the same definition as start, with REVERSED orientation.
        const BRepGraphInc::VertexRef& aStartRef = aDegRefs.Vertex(anEdge.StartVertexRefId);
        BRepGraph_MutGuard<BRepGraphInc::VertexRef> aEndRef =
          theGraph.Builder().MutVertexRef(anEdge.EndVertexRefId);
        aEndRef->VertexDefId   = aStartVertexId;
        aEndRef->Orientation   = TopAbs_REVERSED;
        aEndRef->LocalLocation = aStartRef.LocalLocation;
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
  BRepGraph_DeferredScope aDeferredScope(theGraph);

  // Configure history on the graph; preserve caller's state for restoration on exit.
  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);

  // Compute effective MinTolerance.
  const double aMinTol = theOptions.MinTolerance > 0.0
                           ? theOptions.MinTolerance
                           : std::max(theOptions.Tolerance * 1.0e-4, Precision::Confusion());

  // Temporary allocator reused across sequential lifecycle phases.
  // Reset(false) is applied at safe boundaries once dependent containers are destroyed.
  occ::handle<NCollection_IncAllocator> aTmpAllocator = new NCollection_IncAllocator;
  const auto fillRemainingFreeEdges = [&]() {
    NCollection_Map<int> aNoFloatingEdges;
    const FreeEdgeList   aRemainingFreeEdges = findFreeEdges(theGraph,
                                                           false,
                                                           aNoFloatingEdges,
                                                           aTmpAllocator);
    aResult.FreeEdges =
      NCollection_Vector<BRepGraph_EdgeId>(std::max(THE_INIT_VECTOR_CAPACITY,
                                                    aRemainingFreeEdges.Length()));
    for (const BRepGraph_NodeId& aFreeEdgeNodeId : aRemainingFreeEdges)
    {
      aResult.FreeEdges.Append(BRepGraph_EdgeId::FromNodeId(aFreeEdgeNodeId));
    }
    aResult.NbFreeEdgesAfter = aResult.FreeEdges.Length();
  };
  const int aParallelWorkers =
    theOptions.Parallel ? BRepGraph_ParallelPolicy::WorkerCount() : 1;

  // Phase 0 (optional): Face analysis - detect/remove small edges and faces.
  if (theOptions.FaceAnalysis)
  {
    BRepGraphAlgo_FaceAnalysis::Options aFAOptions;
    aFAOptions.MinTolerance = aMinTol;
    aFAOptions.Parallel     = theOptions.Parallel;
    BRepGraphAlgo_FaceAnalysis::Result aFAResult =
      BRepGraphAlgo_FaceAnalysis::Perform(theGraph, aFAOptions);
    aResult.DegeneratedEdges   = std::move(aFAResult.DegeneratedEdges);
    aResult.DeletedFaces       = std::move(aFAResult.DeletedFaces);
    aResult.NbDeletedFaces     = aFAResult.NbDeletedFaces;
    aResult.NbDegeneratedEdges = aFAResult.DegeneratedEdges.Length();
  }

  // Phase 1: Find free edges.
  NCollection_Map<int> aFloatingEdges;
  FreeEdgeList         aFreeEdges = findFreeEdges(theGraph,
                                                  theOptions.FloatingEdgesMode,
                                                  aFloatingEdges);
  aResult.NbFreeEdgesBefore = aFreeEdges.Length();

  if (aFreeEdges.IsEmpty())
  {
    aResult.IsDone           = true;
    aResult.NbFreeEdgesAfter = 0;
    theGraph.History().SetEnabled(wasHistoryEnabled);
    return aResult;
  }

  BRepGraph_ParallelPolicy::Workload aCoreWork;
  aCoreWork.PrimaryItems = aFreeEdges.Length();
  BRepGraphAlgo_Sewing::Options aExecOptions = theOptions;
  aExecOptions.Parallel =
    BRepGraph_ParallelPolicy::ShouldRun(theOptions.Parallel, aParallelWorkers, aCoreWork);

  // Phase 2: Assemble vertices.
  BRepGraph_ParallelPolicy::Workload aVertexAssembleWork;
  aVertexAssembleWork.PrimaryItems = aFreeEdges.Length();
  const bool isLightweightVertexAssemble =
    aExecOptions.Parallel
    && !BRepGraph_ParallelPolicy::ShouldRun(aExecOptions.Parallel,
                                            aParallelWorkers,
                                            aVertexAssembleWork);
  assembleVertices(theGraph,
                   aFreeEdges,
                   theOptions.Tolerance,
                   isLightweightVertexAssemble,
                   aTmpAllocator);
  aTmpAllocator->Reset(false);

  // Phase 3 (optional): Cut edges at intersections.
  if (theOptions.Cutting)
  {
    cutAtIntersections(theGraph,
                       aFreeEdges,
                       aExecOptions,
                       aMinTol,
                       aFloatingEdges,
                       aParallelWorkers,
                       aTmpAllocator);
    aTmpAllocator->Reset(false);
  }

  // Precompute face index per free-edge position (Opt 2).
  // Free edges have exactly 1 face; floating edges have 0 (-1 sentinel).
  // Look up the face via the first CoEdge referencing each edge.
  const int               aNbFreeEdgesForMap = aFreeEdges.Length();
  NCollection_Array1<int> aFaceOfPos(1, aNbFreeEdgesForMap);
  for (int aFreeIdx = 1; aFreeIdx <= aNbFreeEdgesForMap; ++aFreeIdx)
  {
    const int                                     anEdgeIdx = aFreeEdges.Value(aFreeIdx - 1).Index;
    int                                           aFaceIdx  = -1;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(aCEIdx));
      if (aCoEdge.FaceDefId.IsValid())
      {
        aFaceIdx = aCoEdge.FaceDefId.Index;
        break;
      }
    }
    aFaceOfPos(aFreeIdx) = aFaceIdx;
  }

  // Phase 4: Detect sewing candidates (returns local adjacency).
  // Phase 5: Match free edge pairs (uses local adjacency + inline union-find).
  // Phase 6-8: Merge/process sewn edges while phase-local buffers are alive.
  {
    NCollection_Array1<NCollection_Vector<int>> aAdjacency =
      detectCandidates(theGraph,
                       aFreeEdges,
                       aFaceOfPos,
                       aExecOptions,
                       aParallelWorkers,
                       aTmpAllocator);
    NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
      matchFreeEdges(theGraph,
                     aFreeEdges,
                     aAdjacency,
                     aExecOptions,
                     aMinTol,
                     aParallelWorkers,
                     aTmpAllocator);

    if (aMatchedPairs.IsEmpty())
    {
      aResult.IsDone           = true;
      fillRemainingFreeEdges();
      theGraph.History().SetEnabled(wasHistoryEnabled);
      return aResult;
    }

    // Phase 6: Merge matched edge pairs.
    const int                                aNbPairs = aMatchedPairs.Length();
    NCollection_IndexedMap<BRepGraph_EdgeId> aSewnEdgeIndices(aNbPairs, aTmpAllocator);
    aResult.NbSewnEdges = mergeMatchedEdges(theGraph,
                                            aMatchedPairs,
                                            aExecOptions,
                                            theOptions.Parallel,
                                            aSewnEdgeIndices,
                                            aResult,
                                            aTmpAllocator);

    // Phase 7: Convert remaining free degenerate edges.
    convertDegenerateEdges(theGraph, aResult, aTmpAllocator);

    // Phase 8: Process sewn edges (tolerance consistency).
    processEdges(theGraph, aSewnEdgeIndices, aExecOptions);
  }
  aTmpAllocator->Reset(false);

  // Phase 9: Populate result output.
  fillRemainingFreeEdges();

  // Detect multiple edges (shared by >2 faces).
  const BRepGraph::TopoView& aDefs = theGraph.Topo();
  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = aDefs.Edges().Definition(anEdgeId);
    if (anEdge.IsRemoved || BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
    {
      continue;
    }
    if (aDefs.Edges().NbFaces(anEdgeId) > 2)
    {
      aResult.MultipleEdges.Append(BRepGraph_EdgeId(anEdgeIdx));
    }
  }
  aResult.NbMultipleEdges = aResult.MultipleEdges.Length();

  aResult.IsDone = true;
  theGraph.History().SetEnabled(wasHistoryEnabled);
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

  BRepGraph                             aGraph;
  occ::handle<NCollection_IncAllocator> anAllocator = new NCollection_IncAllocator;
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
