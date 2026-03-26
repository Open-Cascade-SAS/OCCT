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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutationGuard.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <algorithm>
#include <utility>

namespace
{
constexpr int    THE_NB_EDGE_MATCH_SAMPLES = 5;   // sample count for edge geometric matching
constexpr double THE_MAX_CHORD_RATIO       = 2.0; // maximum chord-length ratio for sewable edges
constexpr double THE_HIGH_CONFIDENCE_RATIO =
  0.01;                                          // forward-distance threshold to skip reverse pass
constexpr int    THE_INIT_VECTOR_CAPACITY = 256; // initial capacity for scratch vectors
constexpr double THE_SEAM_SEPARATION_THRESHOLD =
  0.25; // minimum midpoint UV separation as fraction of period

//! Compute endpoint-based matching score between two edges.
//! Returns min(start-start + end-end, start-end + end-start) distance.
//! Lower score means a better endpoint correspondence.
//! @param[in] theGraph source graph
//! @param[in] theEdgeA first edge id
//! @param[in] theEdgeB second edge id
//! @return endpoint distance score
static double edgeEndpointPairScore(const BRepGraph&       theGraph,
                                    const BRepGraph_EdgeId theEdgeA,
                                    const BRepGraph_EdgeId theEdgeB)
{
  const BRepGraph::TopoView          aDefs  = theGraph.Topo();
  const BRepGraph_TopoNode::EdgeDef& aEdgeA = aDefs.Edge(theEdgeA);
  const BRepGraph_TopoNode::EdgeDef& aEdgeB = aDefs.Edge(theEdgeB);

  const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeA.StartVertex);
  const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeA.EndVertex);
  const gp_Pnt aStartB = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeB.StartVertex);
  const gp_Pnt aEndB   = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeB.EndVertex);

  return std::min(aStartA.Distance(aStartB) + aEndA.Distance(aEndB),
                  aStartA.Distance(aEndB) + aEndA.Distance(aStartB));
}

//! Geometric compatibility test between two edges using sampled
//! bidirectional projection. Forward pass projects edgeA samples onto
//! edgeB; if max distance exceeds theHighConfidenceRatio * tolerance,
//! a reverse pass projects edgeB samples onto edgeA.
//! @param[in] theGraph             source graph
//! @param[in] theEdgeA             first edge id
//! @param[in] theEdgeB             second edge id
//! @param[in] theSamplePtsA        pre-sampled points on edgeA
//! @param[in] theExtPCRevA         pre-initialized reverse projector on edgeA curve
//! @param[in] theChordA            chord length of edgeA
//! @param[in] theTolerance         maximum allowed point-to-curve distance
//! @param[in] theNbSamples         number of samples along edgeB for reverse pass
//! @param[in] theMaxChordRatio     max accepted endpoint chord ratio
//! @param[in] theHighConfidenceRatio forward-pass threshold to skip reverse pass
//! @return true if edges are geometrically compatible
static bool areEdgesCompatibleSampled(const BRepGraph&                  theGraph,
                                      const BRepGraph_EdgeId            theEdgeA,
                                      const BRepGraph_EdgeId            theEdgeB,
                                      const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                                      const ExtremaPC_Curve&            theExtPCRevA,
                                      const double                      theChordA,
                                      const double                      theTolerance,
                                      const int                         theNbSamples = 5,
                                      const double theMaxChordRatio       = 2.0,
                                      const double theHighConfidenceRatio = 0.01)
{
  const BRepGraph::TopoView          aDefs  = theGraph.Topo();
  const BRepGraph_TopoNode::EdgeDef& aNodeA = aDefs.Edge(theEdgeA);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = aDefs.Edge(theEdgeB);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.StartVertex);
  const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.EndVertex);
  const gp_Pnt aStartB = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.StartVertex);
  const gp_Pnt aEndB   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.EndVertex);

  const bool isSameDir =
    aStartA.Distance(aStartB) <= theTolerance && aEndA.Distance(aEndB) <= theTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= theTolerance && aEndA.Distance(aStartB) <= theTolerance;
  if (!isSameDir && !isReversed)
  {
    return false;
  }

  const double aChordB = aStartB.Distance(aEndB);
  if (theChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = theChordA > aChordB ? theChordA / aChordB : aChordB / theChordA;
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
bool isSurfaceClosedForEdge(const BRepGraph&                     theGraph,
                            const BRepGraph_FaceId               theFaceDefId,
                            const BRepGraph_TopoNode::CoEdgeDef& theCoEdge,
                            bool                                 theCheckU)
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
    theGraph.Topo().CoEdgesOfEdge(theEdgeId);
  for (int i = 0; i < aCoEdgeIdxs.Length(); ++i)
  {
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(aCoEdgeIdxs.Value(i));
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

//! Check if two free edges on the same UV-closed face lie on opposite sides of
//! the seam and can be sewn together. Uses 2D bounding boxes of their PCurves:
//! on a U-closed surface, two V-long edges are on opposite sides if their
//! outer U-gap (across the seam) is smaller than their inner U-gap.
//! Midpoint UV separation must exceed 25% of the period to catch same-side false positives.
//! @param[in] theGraph      source graph
//! @param[in] theEdgeA      first edge index
//! @param[in] theEdgeB      second edge index
//! @param[in] theSharedFace shared face index (-1 if none)
//! @return true if the edges can be sewn on the shared face
bool canSewSameFaceEdges(const BRepGraph&       theGraph,
                         const BRepGraph_EdgeId theEdgeA,
                         const BRepGraph_EdgeId theEdgeB,
                         const BRepGraph_FaceId theSharedFace)
{
  if (!theSharedFace.IsValid())
  {
    return false;
  }
  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theSharedFace))
  {
    return false;
  }
  const occ::handle<Geom_Surface>& aFaceSurf2 =
    BRepGraph_Tool::Face::Surface(theGraph, theSharedFace);
  const occ::handle<Geom_Surface> aBasis    = basisSurface(aFaceSurf2);
  const bool                      isUClosed = aBasis->IsUClosed();
  const bool                      isVClosed = aBasis->IsVClosed();
  if (!isUClosed && !isVClosed)
  {
    return false;
  }

  // Get PCurves for both edges on the shared face via graph lookup.
  const BRepGraphInc::CoEdgeEntity* aPCEntryA =
    BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeA, theSharedFace);
  const BRepGraphInc::CoEdgeEntity* aPCEntryB =
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

//! Collect boundary (free) edges - edges with exactly one adjacent face.
//! Seam edges and removed edges are excluded. Optionally includes floating edges
//! (zero adjacent faces) in a separate set.
//! @param[in]  theGraph           source graph
//! @param[in]  theIncludeFloating if true, include edges with zero faces
//! @param[out] theFloatingEdges   set of floating edge indices (populated if theIncludeFloating)
//! @return array of free edge NodeIds
NCollection_Array1<BRepGraph_NodeId> findFreeEdges(const BRepGraph&      theGraph,
                                                   bool                  theIncludeFloating,
                                                   NCollection_Map<int>& theFloatingEdges)
{
  const BRepGraph::TopoView            aDefs = theGraph.Topo();
  NCollection_Vector<BRepGraph_NodeId> aFreeVec;

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId) || anEdge.IsRemoved)
    {
      continue;
    }

    // Cached face count from reverse index - O(1).
    const int aFaceCount = aDefs.FaceCountOfEdge(anEdgeId);

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

//! Merge coincident vertices of free edges using KDTree range search
//! and Union-Find. Updates edge vertex references and reverse indices.
//! @param[in,out] theGraph      graph to modify
//! @param[in]     theFreeEdges  free edges from findFreeEdges()
//! @param[in]     theTolerance  maximum merge distance
//! @param[in,out] theResult     sewing result diagnostics
void assembleVertices(BRepGraph&                                   theGraph,
                      const NCollection_Array1<BRepGraph_NodeId>&  theFreeEdges,
                      double                                       theTolerance,
                      const occ::handle<NCollection_IncAllocator>& theTmpAlloc)
{
  const int               aNbFreeEdges = theFreeEdges.Length();
  NCollection_Vector<int> aFreeVertexIndices(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Map<int>    aFreeVertexSet(aNbFreeEdges, theTmpAlloc);
  for (int aFreeEdgeIter = 1; aFreeEdgeIter <= aNbFreeEdges; ++aFreeEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge =
      theGraph.Topo().Edge(BRepGraph_EdgeId(theFreeEdges.Value(aFreeEdgeIter).Index));
    if (anEdge.StartVertex.VertexDefId.IsValid()
        && aFreeVertexSet.Add(anEdge.StartVertex.VertexDefId.Index))
    {
      aFreeVertexIndices.Append(anEdge.StartVertex.VertexDefId.Index);
    }
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && aFreeVertexSet.Add(anEdge.EndVertex.VertexDefId.Index))
    {
      aFreeVertexIndices.Append(anEdge.EndVertex.VertexDefId.Index);
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
    aVertexPoints.SetValue(aVtxIter,
                           BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(anIdx)));
    aGraphIndices.SetValue(aVtxIter, anIdx);
  }

  // Build the KDTree - O(n log n).
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

      const int anIdxJ = aGraphIndices.Value(anArrayIdx);
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
      theGraph.MutEdge(BRepGraph_EdgeId(theFreeEdges.Value(aFreeEdgeIter).Index));
    const int* aMergedStart = anEdge->StartVertex.VertexDefId.IsValid()
                                ? aVertexMerge.Seek(anEdge->StartVertex.VertexDefId.Index)
                                : nullptr;
    if (aMergedStart != nullptr)
    {
      anEdge->StartVertex.VertexDefId = BRepGraph_VertexId(*aMergedStart);
    }
    const int* aMergedEnd = anEdge->EndVertex.VertexDefId.IsValid()
                              ? aVertexMerge.Seek(anEdge->EndVertex.VertexDefId.Index)
                              : nullptr;
    if (aMergedEnd != nullptr)
    {
      anEdge->EndVertex.VertexDefId = BRepGraph_VertexId(*aMergedEnd);
    }
  }
}

//! Split free edges at T-vertex intersections (vertices from other edges
//! that project onto the edge within tolerance). Uses UBTree for spatial
//! candidate detection. Updates theFreeEdges array in place.
//! @param[in,out] theGraph      graph to modify
//! @param[in,out] theFreeEdges  free edges (may grow due to splits)
//! @param[in]     theOptions    sewing configuration
//! @param[in,out] theResult     sewing result diagnostics
void cutAtIntersections(BRepGraph&                                   theGraph,
                        NCollection_Array1<BRepGraph_NodeId>&        theFreeEdges,
                        const BRepGraphAlgo_Sewing::Options&         theOptions,
                        double                                       theMinTol,
                        const NCollection_Map<int>&                  theFloatingEdges,
                        const occ::handle<NCollection_IncAllocator>& theTmpAlloc)
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
      theGraph.Topo().Edge(BRepGraph_EdgeId(theFreeEdges.Value(aFreeEdgeIter).Index));
    if (anEdge.StartVertex.VertexDefId.IsValid()
        && aFreeVtxSet.Add(anEdge.StartVertex.VertexDefId.Index))
    {
      aFreeVtxList.Append(anEdge.StartVertex.VertexDefId.Index);
    }
    if (anEdge.EndVertex.VertexDefId.IsValid()
        && aFreeVtxSet.Add(anEdge.EndVertex.VertexDefId.Index))
    {
      aFreeVtxList.Append(anEdge.EndVertex.VertexDefId.Index);
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
    aVtxPoints.SetValue(aVtxIter,
                        BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aVtxIdx)));
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
      const BRepGraph_NodeId             anEdgeNodeId  = theFreeEdges.Value(aFreeEdgeIter);
      const BRepGraph_EdgeId             anEdgeId(anEdgeNodeId.Index);
      const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Topo().Edge(anEdgeId);

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

      const int aStartIdx =
        anEdge.StartVertex.VertexDefId.IsValid() ? anEdge.StartVertex.VertexDefId.Index : -1;
      const int aEndIdx =
        anEdge.EndVertex.VertexDefId.IsValid() ? anEdge.EndVertex.VertexDefId.Index : -1;

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

      // Query UBTree for vertices whose boxes overlap the edge box - O(log V).
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
      BRepGraph_Mutator::SplitEdge(theGraph, aCurrentEdge, aVtxNodeId, aCand.Param, aSubA, aSubB);
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
  const BRepGraph&                              theGraph,
  const NCollection_Array1<BRepGraph_NodeId>&   theFreeEdges,
  const NCollection_Array1<int>&                theFaceOfPos,
  const BRepGraphAlgo_Sewing::Options&          theOptions,
  const occ::handle<NCollection_IncAllocator>&  theTmpAlloc,
  const occ::handle<NCollection_BaseAllocator>& theGraphAlloc)
{
  const int                                   aNbFreeEdges = theFreeEdges.Length();
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

  const auto aSearchFn = [&](int theIdxI, NCollection_Vector<std::pair<int, int>>& thePairs) {
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
                                   BRepGraph_EdgeId(theFreeEdges.Value(anI).Index),
                                   BRepGraph_EdgeId(theFreeEdges.Value(anJ).Index),
                                   BRepGraph_FaceId(aFaceI)))
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
    NCollection_Array1<NCollection_Vector<std::pair<int, int>>> aPerEdgePairs(0, aNbFreeEdges - 1);
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

  // Step 4: Build bidirectional adjacency from position pairs.
  for (int aPairIdx = 0; aPairIdx < aPairs.Length(); ++aPairIdx)
  {
    const auto& [aPosI, aPosJ] = aPairs.Value(aPairIdx);
    anAdj.ChangeValue(aPosI).Append(aPosJ);
    anAdj.ChangeValue(aPosJ).Append(aPosI);
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
  const NCollection_Array1<BRepGraph_NodeId>&        theFreeEdges,
  const NCollection_Array1<NCollection_Vector<int>>& theAdjacency,
  const NCollection_Array1<int>&                     theFaceOfPos,
  const BRepGraphAlgo_Sewing::Options&               theOptions,
  double                                             theMinTol,
  const occ::handle<NCollection_IncAllocator>&       theTmpAlloc)
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
      const BRepGraph_EdgeId anEdgeA(anIdA.Index);
      const int              aComp = aCompByPos.Value(anI);
      if (aComp == 0)
      {
        return;
      }

      MatchResult& aMatch = aPerEdgeMatch.ChangeValue(anI);

      const BRepGraph_TopoNode::EdgeDef& anEdgeANode = theGraph.Topo().Edge(anEdgeA);
      if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeA))
      {
        return;
      }

      GeomAdaptor_TransformedCurve aCurveA = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeA);

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

      const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, anEdgeANode.StartVertex);
      const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, anEdgeANode.EndVertex);
      const double aChordA = aStartA.Distance(aEndA);

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
          if (!canSewSameFaceEdges(theGraph,
                                   anEdgeA,
                                   BRepGraph_EdgeId(anIdB.Index),
                                   BRepGraph_FaceId(aFaceA)))
          {
            continue;
          }
        }

        // LocalTolerancesMode: use adaptive work tolerance.
        double aWorkTol = theOptions.Tolerance;
        if (theOptions.LocalTolerancesMode)
        {
          aWorkTol += BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeA)
                      + BRepGraph_Tool::Edge::Tolerance(theGraph, BRepGraph_EdgeId(anIdB.Index));
        }

        const BRepGraph_EdgeId anEdgeIdB(anIdB.Index);
        if (areEdgesCompatibleSampled(theGraph,
                                                         anEdgeA,
                                                         anEdgeIdB,
                                                         aSamplePtsA,
                                                         anExtPCRevA,
                                                         aChordA,
                                                         aWorkTol,
                                                         THE_NB_EDGE_MATCH_SAMPLES,
                                                         THE_MAX_CHORD_RATIO,
                                                         THE_HIGH_CONFIDENCE_RATIO))
        {
          const double aScore =
            edgeEndpointPairScore(theGraph, anEdgeA, anEdgeIdB);
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
  NCollection_IndexedMap<BRepGraph_EdgeId>&                                theSewnEdgeIndices,
  BRepGraphAlgo_Sewing::Result&                                            theResult,
  const occ::handle<NCollection_IncAllocator>&                             theTmpAlloc)
{
  int aSewnCount = 0;

  // Accumulate history pairs for batch recording (Opt 5).
  // Use the scoped temp allocator - these vectors are consumed by RecordBatch then discarded.
  NCollection_Vector<BRepGraph_NodeId> aHistOriginals(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);
  NCollection_Vector<BRepGraph_NodeId> aHistReplacements(THE_INIT_VECTOR_CAPACITY, theTmpAlloc);

  for (int aPairIter = 0; aPairIter < thePairs.Length(); ++aPairIter)
  {
    const auto& [anIdA, anIdB] = thePairs.Value(aPairIter);
    const BRepGraph_EdgeId anEdgeIdA(anIdA.Index);
    const BRepGraph_EdgeId anEdgeIdB(anIdB.Index);

    const BRepGraph_TopoNode::EdgeDef& aRemoveEdge = theGraph.Topo().Edge(anEdgeIdB);
    const BRepGraph_TopoNode::EdgeDef& aKeepEdge   = theGraph.Topo().Edge(anEdgeIdA);

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
    theGraph.MutEdge(anEdgeIdA)->Tolerance = aMergedTol;

    // 2. PCurve transfer from remove-edge to keep-edge via CoEdge data.
    const NCollection_Vector<BRepGraph_CoEdgeId>& aRemoveCoEdges =
      theGraph.Topo().CoEdgesOfEdge(anEdgeIdB);
    for (int aCEIter = 0; aCEIter < aRemoveCoEdges.Length(); ++aCEIter)
    {
      const BRepGraphInc::CoEdgeEntity& aRemoveCE =
        theGraph.Topo().CoEdge(aRemoveCoEdges.Value(aCEIter));
      if (aRemoveCE.Curve2DRepId.IsValid())
      {
        const occ::handle<Geom2d_Curve>& aRemovePCurve =
          BRepGraph_Tool::CoEdge::PCurve(theGraph, aRemoveCoEdges.Value(aCEIter));
        // Add PCurve entry to keep-edge via graph API; preserve original orientation
        // so that seam edges (REVERSED orientation) are correctly reconstructed as C2.
        theGraph.Builder().AddPCurveToEdge(anIdA,
                                           aRemoveCE.FaceDefId,
                                           aRemovePCurve,
                                           aRemoveCE.ParamFirst,
                                           aRemoveCE.ParamLast,
                                           aRemoveCE.Sense);
      }
    }

    // 3. Determine direction correspondence from vertex positions.
    const gp_Pnt aKeepStart   = BRepGraph_Tool::Vertex::Pnt(theGraph, aKeepEdge.StartVertex);
    const gp_Pnt aRemoveStart = BRepGraph_Tool::Vertex::Pnt(theGraph, aRemoveEdge.StartVertex);
    const bool   isReversed   = aKeepStart.Distance(aRemoveStart) > theOptions.Tolerance;

    // 4. Update wire entries referencing the remove-edge.
    // Copy the wire list since ReplaceEdgeInWire modifies the reverse index.
    const NCollection_Vector<BRepGraph_WireId> aWires = theGraph.Topo().WiresOfEdge(anEdgeIdB);
    for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
    {
      BRepGraph_Mutator::ReplaceEdgeInWire(theGraph,
                                           aWires.Value(aWIdx),
                                           anEdgeIdB,
                                           anEdgeIdA,
                                           isReversed);
    }

    // Remove the old edge properly - decrements NbActiveEdges, clears cache.
    // Pass replacement (anIdA) so layers can migrate data from remove-edge to keep-edge.
    theGraph.Builder().RemoveNode(anIdB, anIdA);

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
  if (theOptions.SameParameterMode && !theSewnEdgeIndices.IsEmpty())
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
  const BRepGraph::TopoView aDefs = theGraph.Topo();

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
    // Flat faces - wrap in compound.
    BRep_Builder    aBB;
    TopoDS_Compound aResultCompound;
    aBB.MakeCompound(aResultCompound);
    for (int aFaceIdx = 0; aFaceIdx < aDefs.NbFaces(); ++aFaceIdx)
    {
      if (!aDefs.Face(BRepGraph_FaceId(aFaceIdx)).IsRemoved)
      {
        aBB.Add(aResultCompound, theGraph.Shapes().ReconstructFace(BRepGraph_FaceId(aFaceIdx)));
      }
    }
    return aResultCompound;
  }
}

//! Convert remaining free edges that form degenerate wires into
//! degenerate edges (collapsed to a point on surface).
//! @param[in,out] theGraph  graph to modify
//! @param[in,out] theResult sewing result diagnostics
void convertDegenerateEdges(BRepGraph& theGraph, BRepGraphAlgo_Sewing::Result& theResult)
{
  // Collect remaining free edges after merging.
  const NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(theGraph);
  if (aFreeEdges.IsEmpty())
  {
    return;
  }

  // For each free edge, check if it forms a degenerate "wire" by itself:
  // edge chord length <= sum of vertex tolerances.
  for (int aFreeIter = 0; aFreeIter < aFreeEdges.Length(); ++aFreeIter)
  {
    const BRepGraph_EdgeId             anEdgeId = aFreeEdges.Value(aFreeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdge   = theGraph.Topo().Edge(anEdgeId);

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
    double aVertexTolSum = 0.0;
    if (anEdge.StartVertex.VertexDefId.IsValid())
    {
      aVertexTolSum += BRepGraph_Tool::Vertex::Tolerance(theGraph, anEdge.StartVertex.VertexDefId);
    }
    if (anEdge.EndVertex.VertexDefId.IsValid())
    {
      aVertexTolSum += BRepGraph_Tool::Vertex::Tolerance(theGraph, anEdge.EndVertex.VertexDefId);
    }

    if (aLength <= aVertexTolSum)
    {
      // Mark as degenerate: clear 3D curve, merge vertices to midpoint.
      BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aMutEdge = theGraph.MutEdge(anEdgeId);
      aMutEdge->IsDegenerate                                 = true;
      aMutEdge->Curve3DRepId                                 = BRepGraph_Curve3DRepId();

      // Merge start/end vertices if they differ.
      if (anEdge.StartVertex.VertexDefId.IsValid() && anEdge.EndVertex.VertexDefId.IsValid()
          && anEdge.StartVertex.VertexDefId != anEdge.EndVertex.VertexDefId)
      {
        const double aTolStart =
          BRepGraph_Tool::Vertex::Tolerance(theGraph, anEdge.StartVertex.VertexDefId);
        const double aTolEnd =
          BRepGraph_Tool::Vertex::Tolerance(theGraph, anEdge.EndVertex.VertexDefId);
        const double aD1     = aTolStart + aPtMid.Distance(aPtFirst);
        const double aD2     = aTolEnd + aPtMid.Distance(aPtLast);
        const double aNewTol = std::max(aD1, aD2);

        // Keep start vertex, update it to midpoint.
        BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> aVtx =
          theGraph.MutVertex(anEdge.StartVertex.VertexDefId);
        aVtx->Point                       = aPtMid;
        aVtx->Tolerance                   = aNewTol;
        aMutEdge->EndVertex.VertexDefId   = anEdge.StartVertex.VertexDefId;
        aMutEdge->EndVertex.Orientation   = TopAbs_REVERSED;
        aMutEdge->EndVertex.LocalLocation = anEdge.StartVertex.LocalLocation;
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
  BRepGraph_MutationGuard aMutationGuard(theGraph);

  // Configure history on the graph.
  theGraph.History().SetEnabled(theOptions.HistoryMode);

  // Compute effective MinTolerance.
  const double aMinTol = theOptions.MinTolerance > 0.0
                           ? theOptions.MinTolerance
                           : std::max(theOptions.Tolerance * 1.0e-4, Precision::Confusion());

  // Main allocator for cross-phase data.
  occ::handle<NCollection_IncAllocator> anAllocator = new NCollection_IncAllocator;
  // Temporary allocator for per-phase scratch, Reset(false) between phases.
  occ::handle<NCollection_IncAllocator> aTmpAllocator = new NCollection_IncAllocator;

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
  NCollection_Map<int>                 aFloatingEdges;
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
  // Look up the face via the first CoEdge referencing each edge.
  const int               aNbFreeEdgesForMap = aFreeEdges.Length();
  NCollection_Array1<int> aFaceOfPos(1, aNbFreeEdgesForMap);
  for (int aFreeIdx = 1; aFreeIdx <= aNbFreeEdgesForMap; ++aFreeIdx)
  {
    const int                                     anEdgeIdx = aFreeEdges.Value(aFreeIdx).Index;
    int                                           aFaceIdx  = -1;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIdx));
      if (aCoEdge.FaceDefId.IsValid())
      {
        aFaceIdx = aCoEdge.FaceDefId.Index;
        break;
      }
    }
    aFaceOfPos(aFreeIdx) = aFaceIdx;
  }

  // Phase 4: Detect sewing candidates (returns local adjacency).
  NCollection_Array1<NCollection_Vector<int>> aAdjacency =
    detectCandidates(theGraph, aFreeEdges, aFaceOfPos, theOptions, aTmpAllocator, anAllocator);
  aTmpAllocator->Reset(false);

  // Phase 5: Match free edge pairs (uses local adjacency + inline union-find).
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aMatchedPairs =
    matchFreeEdges(theGraph,
                   aFreeEdges,
                   aAdjacency,
                   aFaceOfPos,
                   theOptions,
                   aMinTol,
                   aTmpAllocator);
  aTmpAllocator->Reset(false);

  if (aMatchedPairs.IsEmpty())
  {
    aResult.IsDone           = true;
    aResult.NbFreeEdgesAfter = aResult.NbFreeEdgesBefore;
    aResult.FreeEdges        = BRepGraph_Analyze::FreeEdges(theGraph);
    return aResult;
  }

  // Phase 6: Merge matched edge pairs.
  const int                                aNbPairs = aMatchedPairs.Length();
  NCollection_IndexedMap<BRepGraph_EdgeId> aSewnEdgeIndices(aNbPairs, anAllocator);
  aResult.NbSewnEdges = mergeMatchedEdges(theGraph,
                                          aMatchedPairs,
                                          theOptions,
                                          aSewnEdgeIndices,
                                          aResult,
                                          aTmpAllocator);

  // Phase 7: Convert remaining free degenerate edges.
  convertDegenerateEdges(theGraph, aResult);

  // Phase 8: Process sewn edges (tolerance consistency).
  processEdges(theGraph, aSewnEdgeIndices, theOptions);

  // Phase 9: Populate result output.
  aResult.FreeEdges        = BRepGraph_Analyze::FreeEdges(theGraph);
  aResult.NbFreeEdgesAfter = aResult.FreeEdges.Length();

  // Detect multiple edges (shared by >2 faces).
  const BRepGraph::TopoView aDefs = theGraph.Topo();
  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (anEdge.IsRemoved || BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
    {
      continue;
    }
    if (aDefs.FaceCountOfEdge(anEdgeId) > 2)
    {
      aResult.MultipleEdges.Append(BRepGraph_EdgeId(anEdgeIdx));
    }
  }
  aResult.NbMultipleEdges = aResult.MultipleEdges.Length();

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
