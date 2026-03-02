// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomFill_Gordon.hxx>

#include <Approx_BSplineApproxInterp.hxx>
#include <BSplCLib.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_GordonBuilder.hxx>
#include <GeomFill_Profiler.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_HArray1.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <atomic>

namespace
{

//! Kink angle threshold in radians (6 degrees).
constexpr double THE_KINK_ANGLE_RAD = 6.0 / 180.0 * M_PI;

//! Epsilon offset for evaluating derivatives near knots.
constexpr double THE_KINK_EPS = 1.0e-8;

//! Snap tolerance for boundary parameters.
constexpr double THE_SNAP_TOL = 1.0e-5;

//! Relative tolerance for closedness detection.
constexpr double THE_REL_TOL_CLOSED = 1.0e-8;

//! Detects kink (C0 discontinuity) parameters on a B-spline curve.
//! A kink exists where knot multiplicity equals the curve degree
//! and the tangent vectors on either side differ by more than 6 degrees.
//! @param[in] theCurve the B-spline curve to analyze
//! @return sorted array of kink parameter values (may be empty)
NCollection_DynamicArray<double> getKinkParameters(const occ::handle<Geom_BSplineCurve>& theCurve)
{
  NCollection_DynamicArray<double> aKinks;
  const int                        aDegree = theCurve->Degree();

  for (int iKnot = 2; iKnot < theCurve->NbKnots(); ++iKnot)
  {
    if (theCurve->Multiplicity(iKnot) == aDegree)
    {
      double aKnot     = theCurve->Knot(iKnot);
      gp_Vec aTanLeft  = theCurve->DN(aKnot - THE_KINK_EPS, 1);
      gp_Vec aTanRight = theCurve->DN(aKnot + THE_KINK_EPS, 1);

      if (aTanLeft.Magnitude() > Precision::Confusion()
          && aTanRight.Magnitude() > Precision::Confusion())
      {
        double anAngle = aTanLeft.Angle(aTanRight);
        if (anAngle > THE_KINK_ANGLE_RAD)
        {
          aKinks.Append(aKnot);
        }
      }
    }
  }
  return aKinks;
}

//! Creates a parameter grid with uniform spacing that captures exact break locations.
//! Break parameters (intersections, kinks) are snapped to nearby grid points when
//! possible, or inserted as additional points otherwise.
//! @param[in] theUMin      start of parameter range
//! @param[in] theUMax      end of parameter range
//! @param[in] theNbValues  desired number of uniform grid points
//! @param[in] theBreaks    break parameters to capture exactly
//! @return sorted, deduplicated parameter array
NCollection_DynamicArray<double> linspaceWithBreaks(double                            theUMin,
                                                    double                            theUMax,
                                                    int                               theNbValues,
                                                    const NCollection_Array1<double>& theBreaks)
{
  // Create uniform grid.
  const double                     aDu = (theUMax - theUMin) / static_cast<double>(theNbValues - 1);
  NCollection_DynamicArray<double> aResult;
  for (int i = 0; i < theNbValues; ++i)
  {
    aResult.Append(theUMin + i * aDu);
  }

  constexpr double THE_REPLACE_EPS = 0.3;

  for (int iBrk = theBreaks.Lower(); iBrk <= theBreaks.Upper(); ++iBrk)
  {
    double aBreak = theBreaks(iBrk);

    // Try to find a grid point within replace tolerance.
    bool isReplaced = false;
    for (int k = 0; k < aResult.Length(); ++k)
    {
      if (std::abs(aResult[k] - aBreak) < aDu * THE_REPLACE_EPS)
      {
        aResult[k] = aBreak;
        isReplaced = true;
        break;
      }
    }

    if (!isReplaced)
    {
      // Append (will sort later).
      aResult.Append(aBreak);
    }
  }

  // Sort and remove duplicates.
  std::sort(&aResult[0], &aResult[0] + aResult.Length());

  // Compact duplicates in-place.
  int aWriteIdx = 0;
  for (int k = 1; k < aResult.Length(); ++k)
  {
    if (std::abs(aResult[k] - aResult[aWriteIdx]) > 1.0e-14)
    {
      ++aWriteIdx;
      aResult[aWriteIdx] = aResult[k];
    }
  }
  // Trim to unique count.
  const int aNbUnique = aWriteIdx + 1;
  while (aResult.Length() > aNbUnique)
  {
    aResult.EraseLast();
  }

  return aResult;
}

//! Finds the new-parameter-space value corresponding to a given old parameter
//! using bisection on a monotonic reparametrization function.
//! @param[in] theReparamFunc  2D B-spline: newParam -> (oldParam, 0)
//! @param[in] theOldParam     old parameter value to invert
//! @return corresponding new parameter value
double invertReparamBisection(const occ::handle<Geom2d_BSplineCurve>& theReparamFunc,
                              double                                  theOldParam)
{
  double aLo  = theReparamFunc->FirstParameter();
  double aHi  = theReparamFunc->LastParameter();
  double aFLo = theReparamFunc->Value(aLo).X();
  double aFHi = theReparamFunc->Value(aHi).X();

  // Determine monotonicity direction.
  const bool isIncreasing = (aFHi > aFLo);

  constexpr int    THE_MAX_BISECT_ITER = 50;
  constexpr double THE_BISECT_TOL      = 1.0e-12;

  for (int anIter = 0; anIter < THE_MAX_BISECT_ITER; ++anIter)
  {
    double aMid  = 0.5 * (aLo + aHi);
    double aFMid = theReparamFunc->Value(aMid).X();

    if (std::abs(aFMid - theOldParam) < THE_BISECT_TOL)
    {
      return aMid;
    }

    bool isMidBelow = isIncreasing ? (aFMid < theOldParam) : (aFMid > theOldParam);
    if (isMidBelow)
    {
      aLo = aMid;
    }
    else
    {
      aHi = aMid;
    }

    if (std::abs(aHi - aLo) < THE_BISECT_TOL)
    {
      return 0.5 * (aLo + aHi);
    }
  }

  return 0.5 * (aLo + aHi);
}

//! Reparametrizes a single B-spline curve using approximation-based approach.
//! 1. Build 2D interpolation: newParam -> oldParam
//! 2. Detect kinks on original curve
//! 3. Merge intersection + kink params into breaks
//! 4. Use linspaceWithBreaks for smart sampling
//! 5. Approximate with Approx_BSplineApproxInterp (constrained least-squares fit)
//! @param[in,out] theCurve     B-spline curve to reparametrize (replaced on success)
//! @param[in]     theOldParams old intersection parameters on the curve
//! @param[in]     theNewParams target intersection parameters in [0,1]
//! @return true if reparametrization succeeded
bool reparamCurve(occ::handle<Geom_BSplineCurve>&   theCurve,
                  const NCollection_Array1<double>& theOldParams,
                  const NCollection_Array1<double>& theNewParams)
{
  const double aTolParam = 1.0e-10;

  NCollection_DynamicArray<double> anOldSeq;
  NCollection_DynamicArray<double> aNewSeq;

  // Start of curve.
  anOldSeq.Append(theCurve->FirstParameter());
  aNewSeq.Append(0.0);

  // Interior intersection points.
  for (int j = theOldParams.Lower(); j <= theOldParams.Upper(); ++j)
  {
    double anOld = theOldParams(j);
    double aNew  = theNewParams(j);

    if (!anOldSeq.IsEmpty()
        && (std::abs(anOld - anOldSeq.Last()) < aTolParam
            || std::abs(aNew - aNewSeq.Last()) < aTolParam))
    {
      continue;
    }
    if (std::abs(anOld - theCurve->LastParameter()) < aTolParam || std::abs(aNew - 1.0) < aTolParam)
    {
      continue;
    }

    anOldSeq.Append(anOld);
    aNewSeq.Append(aNew);
  }

  // End of curve.
  if (anOldSeq.Length() < 2 || std::abs(anOldSeq.Last() - theCurve->LastParameter()) > aTolParam)
  {
    anOldSeq.Append(theCurve->LastParameter());
    aNewSeq.Append(1.0);
  }

  if (anOldSeq.Length() < 2)
  {
    return true;
  }

  // Build 2D interpolation: newParam -> (oldParam, 0).
  const int                                  aNbPts = anOldSeq.Length();
  occ::handle<NCollection_HArray1<gp_Pnt2d>> aOldPnts2d =
    new NCollection_HArray1<gp_Pnt2d>(1, aNbPts);
  occ::handle<NCollection_HArray1<double>> aNewParamsH = new NCollection_HArray1<double>(1, aNbPts);
  for (int k = 0; k < aNbPts; ++k)
  {
    aOldPnts2d->SetValue(k + 1, gp_Pnt2d(anOldSeq[k], 0.0));
    aNewParamsH->SetValue(k + 1, aNewSeq[k]);
  }

  Geom2dAPI_Interpolate anInterp2d(aOldPnts2d, aNewParamsH, false, Precision::PConfusion());
  anInterp2d.Perform();
  if (!anInterp2d.IsDone())
  {
    return false;
  }

  occ::handle<Geom2d_BSplineCurve> aReparamFunc = anInterp2d.Curve();

  // Detect kinks on the original curve.
  NCollection_DynamicArray<double> aKinks = getKinkParameters(theCurve);

  // Pre-compute sizes for break and kink arrays.
  const int aNbInteriorParams = std::max(0, theNewParams.Upper() - theNewParams.Lower() - 1);
  const int aNbKinks          = aKinks.Length();
  const int aNbBreaks         = aNbInteriorParams + aNbKinks;

  // Build break list: interior new-params + kink params mapped to new space.
  NCollection_Array1<double> aBreaks(1, std::max(aNbBreaks, 1));
  int                        aBrkIdx = 1;
  for (int j = theNewParams.Lower() + 1; j < theNewParams.Upper(); ++j)
  {
    aBreaks(aBrkIdx++) = theNewParams(j);
  }

  // Map kink old-parameters to new-parameter space via bisection inversion.
  NCollection_Array1<double> aKinksInNew(1, std::max(aNbKinks, 1));
  for (int k = 0; k < aNbKinks; ++k)
  {
    double aKinkNew    = invertReparamBisection(aReparamFunc, aKinks[k]);
    aKinksInNew(k + 1) = aKinkNew;
    aBreaks(aBrkIdx++) = aKinkNew;
  }

  // Determine sample count: max(max_cp + 10, nBreaks + 2), clamped to [30, 200].
  const int aMaxCp     = theCurve->NbPoles();
  int       aNbSamples = std::max(aMaxCp + 10, aNbBreaks + 2);
  aNbSamples           = std::max(30, std::min(200, aNbSamples));

  // Use linspaceWithBreaks for intelligent sample distribution.
  // Kink params are already included in aBreaks, so no separate insertion needed.
  NCollection_DynamicArray<double> aSampleParams =
    linspaceWithBreaks(0.0, 1.0, aNbSamples, aBreaks);

  const int aNbActual = aSampleParams.Length();

  // Sample original curve at mapped old-parameter positions.
  NCollection_Array1<gp_Pnt> aSampledPnts(1, aNbActual);
  NCollection_Array1<double> aSampledNewParams(1, aNbActual);

  for (int k = 0; k < aNbActual; ++k)
  {
    double   aNewParam  = aSampleParams[k];
    gp_Pnt2d anOldPt    = aReparamFunc->Value(aNewParam);
    double   anOldParam = anOldPt.X();

    // Clamp to valid range.
    anOldParam =
      std::max(theCurve->FirstParameter(), std::min(theCurve->LastParameter(), anOldParam));

    aSampledPnts(k + 1)      = theCurve->Value(anOldParam);
    aSampledNewParams(k + 1) = aNewParam;
  }

  // Determine number of control points.
  // Must be >= nInterpolated + degree + 1 for the KKT system to be solvable.
  const int     aNbInterp         = 2 + aNbKinks; // endpoints + kinks
  constexpr int THE_APPROX_DEGREE = 3;
  int           aNbCP =
    std::max(theCurve->NbPoles(), std::max(aNbActual / 3, aNbInterp + THE_APPROX_DEGREE + 1));
  aNbCP = std::max(aNbCP, THE_APPROX_DEGREE + 1);

  // Use constrained least-squares approximation with interpolation at endpoints and kinks.
  Approx_BSplineApproxInterp anApprox(aSampledPnts, aNbCP, THE_APPROX_DEGREE, false);

  // Interpolate first and last points exactly.
  anApprox.InterpolatePoint(0);
  anApprox.InterpolatePoint(aNbActual - 1);

  // Interpolate kink points with C0 break.
  for (int k = 1; k <= aNbKinks; ++k)
  {
    double aKinkNew = aKinksInNew(k);
    // Find the closest sample index for this kink parameter.
    int    aBestIdx  = 0;
    double aBestDist = RealLast();
    for (int s = 1; s <= aNbActual; ++s)
    {
      double aDist = std::abs(aSampledNewParams(s) - aKinkNew);
      if (aDist < aBestDist)
      {
        aBestDist = aDist;
        aBestIdx  = s - 1; // 0-based index for InterpolatePoint
      }
    }
    if (aBestIdx > 0 && aBestIdx < aNbActual - 1) // not endpoints (already interpolated)
    {
      anApprox.InterpolatePoint(aBestIdx, true);
    }
  }

  anApprox.Perform(aSampledNewParams);
  if (!anApprox.IsDone())
  {
    return false;
  }
  theCurve = anApprox.Curve();

  return true;
}

} // namespace

//=================================================================================================

GeomFill_Gordon::GeomFill_Gordon() {}

//=================================================================================================

void GeomFill_Gordon::Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                           const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                           double                                             theTolerance)
{
  myTolerance = theTolerance;
  myIsDone    = false;
  myScale     = 1.0;
  myIsUClosed = false;
  myIsVClosed = false;
  mySurface.Nullify();

  const int aNbProf = theProfiles.Length();
  const int aNbGuid = theGuides.Length();

  if (aNbProf < 2 || aNbGuid < 2)
  {
    // Store counts but don't try to convert — Perform() will check and return.
    myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, std::max(aNbProf, 0));
    myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, std::max(aNbGuid, 0));
    return;
  }

  // Convert input arrays to BSpline storage arrays.
  myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, aNbProf);
  myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, aNbGuid);

  for (int i = theProfiles.Lower(); i <= theProfiles.Upper(); ++i)
  {
    myProfiles(i - theProfiles.Lower() + 1) = GeomConvert::CurveToBSplineCurve(theProfiles(i));
  }
  for (int i = theGuides.Lower(); i <= theGuides.Upper(); ++i)
  {
    myGuides(i - theGuides.Lower() + 1) = GeomConvert::CurveToBSplineCurve(theGuides(i));
  }

  // Initialize parameter matrices.
  myProfileParams = NCollection_Array2<double>(1, aNbProf, 1, aNbGuid);
  myGuideParams   = NCollection_Array2<double>(1, aNbProf, 1, aNbGuid);
}

//=================================================================================================

void GeomFill_Gordon::Perform()
{
  myIsDone = false;

  if (myProfiles.Length() < 2 || myGuides.Length() < 2)
  {
    return;
  }

  // Step 1: Convert and normalize to [0,1].
  if (!convertToBSpline())
  {
    return;
  }

  // Step 2: Find all intersections.
  if (!computeIntersections())
  {
    return;
  }

  // Step 3: Sort network.
  if (!sortNetwork())
  {
    return;
  }

  // Step 4: Compute geometric scale for relative tolerances.
  computeScale();

  // Step 5: Snap near-boundary intersection parameters to exact values.
  eliminateInaccuracies();

  // Step 6: Detect closedness.
  detectClosedness();

  // Step 7: Reparametrize curves to average intersection parameters.
  if (!reparametrize())
  {
    return;
  }

  // Step 8: Validate the curve network is consistent after reparametrization.
  if (!checkNetworkCompatibility())
  {
    return;
  }

  // Step 9: Make profiles compatible (same degree/knots) using GeomFill_Profiler.
  GeomFill_Profiler aProfileProfiler;
  for (int i = myProfiles.Lower(); i <= myProfiles.Upper(); ++i)
  {
    aProfileProfiler.AddCurve(myProfiles(i));
  }
  aProfileProfiler.Perform(Precision::PConfusion());

  // Extract unified profile curves.
  for (int i = myProfiles.Lower(); i <= myProfiles.Upper(); ++i)
  {
    myProfiles(i) = occ::down_cast<Geom_BSplineCurve>(aProfileProfiler.Curve(i));
  }

  // Step 10: Make guides compatible.
  GeomFill_Profiler aGuideProfiler;
  for (int i = myGuides.Lower(); i <= myGuides.Upper(); ++i)
  {
    aGuideProfiler.AddCurve(myGuides(i));
  }
  aGuideProfiler.Perform(Precision::PConfusion());

  for (int i = myGuides.Lower(); i <= myGuides.Upper(); ++i)
  {
    myGuides(i) = occ::down_cast<Geom_BSplineCurve>(aGuideProfiler.Curve(i));
  }

  // Step 11: Prepare averaged intersection parameters for GordonBuilder.
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  NCollection_Array1<double> aGuideParamValues(1, aNbGuid);
  for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
  {
    double aSum = 0.0;
    for (int iProf = 1; iProf <= aNbProf; ++iProf)
    {
      aSum += myProfileParams(iProf, jGuid);
    }
    aGuideParamValues(jGuid) = aSum / aNbProf;
  }

  NCollection_Array1<double> aProfileParamValues(1, aNbProf);
  for (int iProf = 1; iProf <= aNbProf; ++iProf)
  {
    double aSum = 0.0;
    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      aSum += myGuideParams(iProf, jGuid);
    }
    aProfileParamValues(iProf) = aSum / aNbGuid;
  }

  // Step 12: Delegate to GordonBuilder.
  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(myProfiles,
                myGuides,
                aProfileParamValues,
                aGuideParamValues,
                myTolerance,
                myIsUClosed,
                myIsVClosed);
  aBuilder.Perform();

  if (!aBuilder.IsDone())
  {
    return;
  }

  mySurface = aBuilder.Surface();
  myIsDone  = true;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_Gordon::Surface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Gordon::Surface");
  }
  return mySurface;
}

//=================================================================================================

bool GeomFill_Gordon::convertToBSpline()
{
  // Reparametrize all curves to [0,1].
  for (int i = myProfiles.Lower(); i <= myProfiles.Upper(); ++i)
  {
    occ::handle<Geom_BSplineCurve>& aCurve = myProfiles(i);
    if (aCurve.IsNull())
    {
      return false;
    }
    // Reject rational and periodic curves.
    if (aCurve->IsRational() || aCurve->IsPeriodic())
    {
      return false;
    }
    NCollection_Array1<double> aKnots(aCurve->Knots());
    BSplCLib::Reparametrize(0.0, 1.0, aKnots);
    aCurve->SetKnots(aKnots);
  }

  for (int i = myGuides.Lower(); i <= myGuides.Upper(); ++i)
  {
    occ::handle<Geom_BSplineCurve>& aCurve = myGuides(i);
    if (aCurve.IsNull())
    {
      return false;
    }
    if (aCurve->IsRational() || aCurve->IsPeriodic())
    {
      return false;
    }
    NCollection_Array1<double> aKnots(aCurve->Knots());
    BSplCLib::Reparametrize(0.0, 1.0, aKnots);
    aCurve->SetKnots(aKnots);
  }

  return true;
}

//=================================================================================================

bool GeomFill_Gordon::computeIntersections()
{
  const int aNbProf  = myProfiles.Length();
  const int aNbGuid  = myGuides.Length();
  const int aNbTotal = aNbProf * aNbGuid;

  // Use atomic flag for thread-safe error reporting.
  std::atomic<bool> isOk{true};

  OSD_Parallel::For(0, aNbTotal, [&](int theIdx) {
    if (!isOk.load(std::memory_order_relaxed))
    {
      return;
    }

    const int iProf = theIdx / aNbGuid + 1;
    const int jGuid = theIdx % aNbGuid + 1;

    const occ::handle<Geom_BSplineCurve>& aProfile = myProfiles(iProf);
    const occ::handle<Geom_BSplineCurve>& aGuide   = myGuides(jGuid);

    GeomAPI_ExtremaCurveCurve anExtrema(aProfile, aGuide);

    // Find the extremum with distance below tolerance.
    bool   isFound    = false;
    double aMinDist   = RealLast();
    double aProfParam = 0.0;
    double aGuidParam = 0.0;

    for (int iExt = 1; iExt <= anExtrema.NbExtrema(); ++iExt)
    {
      double aDist = anExtrema.Distance(iExt);
      if (aDist < myTolerance && aDist < aMinDist)
      {
        aMinDist = aDist;
        anExtrema.Parameters(iExt, aProfParam, aGuidParam);
        isFound = true;
      }
    }

    // Also check total nearest points (endpoints).
    if (!isFound)
    {
      gp_Pnt aP1, aP2;
      if (anExtrema.TotalNearestPoints(aP1, aP2))
      {
        double aDist = aP1.Distance(aP2);
        if (aDist < myTolerance)
        {
          anExtrema.LowerDistanceParameters(aProfParam, aGuidParam);
          isFound = true;
        }
      }
    }

    if (!isFound)
    {
      isOk.store(false, std::memory_order_relaxed);
      return;
    }

    myProfileParams(iProf, jGuid) = aProfParam;
    myGuideParams(iProf, jGuid)   = aGuidParam;
  });

  return isOk.load();
}

//=================================================================================================

void GeomFill_Gordon::computeScale()
{
  // Compute bounding box diagonal from all curve start/end points.
  double aXMin = RealLast(), aYMin = RealLast(), aZMin = RealLast();
  double aXMax = -RealLast(), aYMax = -RealLast(), aZMax = -RealLast();

  auto updateBBox = [&](const gp_Pnt& thePnt) {
    aXMin = std::min(aXMin, thePnt.X());
    aXMax = std::max(aXMax, thePnt.X());
    aYMin = std::min(aYMin, thePnt.Y());
    aYMax = std::max(aYMax, thePnt.Y());
    aZMin = std::min(aZMin, thePnt.Z());
    aZMax = std::max(aZMax, thePnt.Z());
  };

  for (int i = myProfiles.Lower(); i <= myProfiles.Upper(); ++i)
  {
    updateBBox(myProfiles(i)->Value(myProfiles(i)->FirstParameter()));
    updateBBox(myProfiles(i)->Value(myProfiles(i)->LastParameter()));
  }
  for (int i = myGuides.Lower(); i <= myGuides.Upper(); ++i)
  {
    updateBBox(myGuides(i)->Value(myGuides(i)->FirstParameter()));
    updateBBox(myGuides(i)->Value(myGuides(i)->LastParameter()));
  }

  double aDiag = gp_Pnt(aXMin, aYMin, aZMin).Distance(gp_Pnt(aXMax, aYMax, aZMax));
  myScale      = std::max(aDiag, 1.0);
}

//=================================================================================================

void GeomFill_Gordon::detectClosedness()
{
  const int    aNbProf   = myProfiles.Length();
  const int    aNbGuid   = myGuides.Length();
  const double aCloseTol = THE_REL_TOL_CLOSED * myScale;

  // U-direction closure: first and last guides are geometrically equal
  // and intersection grid's first/last columns match.
  {
    gp_Pnt aGuidFirstStart = myGuides(1)->Value(myGuides(1)->FirstParameter());
    gp_Pnt aGuidFirstEnd   = myGuides(1)->Value(myGuides(1)->LastParameter());
    gp_Pnt aGuidLastStart  = myGuides(aNbGuid)->Value(myGuides(aNbGuid)->FirstParameter());
    gp_Pnt aGuidLastEnd    = myGuides(aNbGuid)->Value(myGuides(aNbGuid)->LastParameter());

    bool isGuidesEqual = aGuidFirstStart.IsEqual(aGuidLastStart, aCloseTol)
                         && aGuidFirstEnd.IsEqual(aGuidLastEnd, aCloseTol);

    bool isGridClosed = true;
    for (int iProf = 1; iProf <= aNbProf && isGridClosed; ++iProf)
    {
      gp_Pnt aP1   = myProfiles(iProf)->Value(myProfileParams(iProf, 1));
      gp_Pnt aP2   = myProfiles(iProf)->Value(myProfileParams(iProf, aNbGuid));
      isGridClosed = aP1.IsEqual(aP2, aCloseTol);
    }

    myIsUClosed = isGuidesEqual && isGridClosed;
  }

  // V-direction closure: first and last profiles are geometrically equal
  // and intersection grid's first/last rows match.
  {
    gp_Pnt aProfFirstStart = myProfiles(1)->Value(myProfiles(1)->FirstParameter());
    gp_Pnt aProfFirstEnd   = myProfiles(1)->Value(myProfiles(1)->LastParameter());
    gp_Pnt aProfLastStart  = myProfiles(aNbProf)->Value(myProfiles(aNbProf)->FirstParameter());
    gp_Pnt aProfLastEnd    = myProfiles(aNbProf)->Value(myProfiles(aNbProf)->LastParameter());

    bool isProfilesEqual = aProfFirstStart.IsEqual(aProfLastStart, aCloseTol)
                           && aProfFirstEnd.IsEqual(aProfLastEnd, aCloseTol);

    bool isGridClosed = true;
    for (int jGuid = 1; jGuid <= aNbGuid && isGridClosed; ++jGuid)
    {
      gp_Pnt aP1   = myGuides(jGuid)->Value(myGuideParams(1, jGuid));
      gp_Pnt aP2   = myGuides(jGuid)->Value(myGuideParams(aNbProf, jGuid));
      isGridClosed = aP1.IsEqual(aP2, aCloseTol);
    }

    myIsVClosed = isProfilesEqual && isGridClosed;
  }
}

//=================================================================================================

void GeomFill_Gordon::eliminateInaccuracies()
{
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  for (int iProf = 1; iProf <= aNbProf; ++iProf)
  {
    const double aFirst = myProfiles(iProf)->FirstParameter();
    const double aLast  = myProfiles(iProf)->LastParameter();
    const double aRange = aLast - aFirst;

    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      double& aParam = myProfileParams(iProf, jGuid);
      if (std::abs(aParam - aFirst) < THE_SNAP_TOL * aRange)
      {
        aParam = aFirst;
      }
      else if (std::abs(aParam - aLast) < THE_SNAP_TOL * aRange)
      {
        aParam = aLast;
      }
    }
  }

  for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
  {
    const double aFirst = myGuides(jGuid)->FirstParameter();
    const double aLast  = myGuides(jGuid)->LastParameter();
    const double aRange = aLast - aFirst;

    for (int iProf = 1; iProf <= aNbProf; ++iProf)
    {
      double& aParam = myGuideParams(iProf, jGuid);
      if (std::abs(aParam - aFirst) < THE_SNAP_TOL * aRange)
      {
        aParam = aFirst;
      }
      else if (std::abs(aParam - aLast) < THE_SNAP_TOL * aRange)
      {
        aParam = aLast;
      }
    }
  }
}

//=================================================================================================

bool GeomFill_Gordon::checkNetworkCompatibility() const
{
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  const double aCheckTol = myScale * 1.0e-5;

  for (int iProf = 1; iProf <= aNbProf; ++iProf)
  {
    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      gp_Pnt aProfPt = myProfiles(iProf)->Value(myProfileParams(iProf, jGuid));
      gp_Pnt aGuidPt = myGuides(jGuid)->Value(myGuideParams(iProf, jGuid));

      if (aProfPt.Distance(aGuidPt) > aCheckTol)
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

bool GeomFill_Gordon::sortNetwork()
{
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  if (aNbProf < 2 || aNbGuid < 2)
  {
    return false;
  }

  // Find starting curves: profile and guide that share their minimum parameter
  // at their intersection.
  int  aStartProf        = 1;
  int  aStartGuid        = 1;
  bool aGuideMustReverse = false;
  bool isStartFound      = false;

  for (int iProf = 1; iProf <= aNbProf && !isStartFound; ++iProf)
  {
    // Find guide with minimum parameter on this profile.
    int    jMinGuid = 1;
    double aMinVal  = myProfileParams(iProf, 1);
    for (int jGuid = 2; jGuid <= aNbGuid; ++jGuid)
    {
      if (myProfileParams(iProf, jGuid) < aMinVal)
      {
        aMinVal  = myProfileParams(iProf, jGuid);
        jMinGuid = jGuid;
      }
    }

    // Check if this profile has the minimum parameter on that guide.
    int    iMinProf     = 1;
    double aMinGuideVal = myGuideParams(1, jMinGuid);
    for (int i = 2; i <= aNbProf; ++i)
    {
      if (myGuideParams(i, jMinGuid) < aMinGuideVal)
      {
        aMinGuideVal = myGuideParams(i, jMinGuid);
        iMinProf     = i;
      }
    }

    if (iMinProf == iProf)
    {
      aStartProf        = iProf;
      aStartGuid        = jMinGuid;
      aGuideMustReverse = false;
      isStartFound      = true;
    }
  }

  // Fallback: check for reversed guide start.
  if (!isStartFound)
  {
    for (int iProf = 1; iProf <= aNbProf && !isStartFound; ++iProf)
    {
      int    jMinGuid = 1;
      double aMinVal  = myProfileParams(iProf, 1);
      for (int jGuid = 2; jGuid <= aNbGuid; ++jGuid)
      {
        if (myProfileParams(iProf, jGuid) < aMinVal)
        {
          aMinVal  = myProfileParams(iProf, jGuid);
          jMinGuid = jGuid;
        }
      }

      // Check if this profile has the MAXIMUM parameter on that guide.
      int    iMaxProf     = 1;
      double aMaxGuideVal = myGuideParams(1, jMinGuid);
      for (int i = 2; i <= aNbProf; ++i)
      {
        if (myGuideParams(i, jMinGuid) > aMaxGuideVal)
        {
          aMaxGuideVal = myGuideParams(i, jMinGuid);
          iMaxProf     = i;
        }
      }

      if (iMaxProf == iProf)
      {
        aStartProf        = iProf;
        aStartGuid        = jMinGuid;
        aGuideMustReverse = true;
        isStartFound      = true;
      }
    }
  }

  if (!isStartFound)
  {
    return false;
  }

  // Helper lambdas for swapping.
  auto swapProfiles = [this, aNbGuid](int i1, int i2) {
    if (i1 == i2)
      return;
    std::swap(myProfiles(i1), myProfiles(i2));
    for (int j = 1; j <= aNbGuid; ++j)
    {
      std::swap(myProfileParams(i1, j), myProfileParams(i2, j));
      std::swap(myGuideParams(i1, j), myGuideParams(i2, j));
    }
  };

  auto swapGuides = [this, aNbProf](int j1, int j2) {
    if (j1 == j2)
      return;
    std::swap(myGuides(j1), myGuides(j2));
    for (int i = 1; i <= aNbProf; ++i)
    {
      std::swap(myProfileParams(i, j1), myProfileParams(i, j2));
      std::swap(myGuideParams(i, j1), myGuideParams(i, j2));
    }
  };

  auto reverseProfile = [this, aNbGuid](int iProf) {
    double aFirst = myProfiles(iProf)->FirstParameter();
    double aLast  = myProfiles(iProf)->LastParameter();
    for (int j = 1; j <= aNbGuid; ++j)
    {
      myProfileParams(iProf, j) = -myProfileParams(iProf, j) + aFirst + aLast;
    }
    myProfiles(iProf)->Reverse();
  };

  auto reverseGuide = [this, aNbProf](int jGuid) {
    double aFirst = myGuides(jGuid)->FirstParameter();
    double aLast  = myGuides(jGuid)->LastParameter();
    for (int i = 1; i <= aNbProf; ++i)
    {
      myGuideParams(i, jGuid) = -myGuideParams(i, jGuid) + aFirst + aLast;
    }
    myGuides(jGuid)->Reverse();
  };

  // Move start curves to position 1.
  swapProfiles(1, aStartProf);
  swapGuides(1, aStartGuid);

  if (aGuideMustReverse)
  {
    reverseGuide(1);
  }

  // Bubble sort guides by their intersection parameter on the first profile (ascending).
  for (int n = aNbGuid; n > 1; --n)
  {
    for (int j = 2; j < n; ++j)
    {
      if (myProfileParams(1, j) > myProfileParams(1, j + 1))
      {
        swapGuides(j, j + 1);
      }
    }
  }

  // Bubble sort profiles by their intersection parameter on the first guide (ascending).
  for (int n = aNbProf; n > 1; --n)
  {
    for (int i = 2; i < n; ++i)
    {
      if (myGuideParams(i, 1) > myGuideParams(i + 1, 1))
      {
        swapProfiles(i, i + 1);
      }
    }
  }

  // Reverse individual profiles if their parameters go in the wrong direction.
  for (int iProf = 2; iProf <= aNbProf; ++iProf)
  {
    if (myProfileParams(iProf, 1) > myProfileParams(iProf, aNbGuid))
    {
      reverseProfile(iProf);
    }
  }

  // Reverse individual guides if their parameters go in the wrong direction.
  for (int jGuid = 2; jGuid <= aNbGuid; ++jGuid)
  {
    if (myGuideParams(1, jGuid) > myGuideParams(aNbProf, jGuid))
    {
      reverseGuide(jGuid);
    }
  }

  return true;
}

//=================================================================================================

bool GeomFill_Gordon::reparametrize()
{
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  // Compute averaged intersection parameters.
  NCollection_Array1<double> aAvgProfileParams(1, aNbGuid);
  for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
  {
    double aSum = 0.0;
    for (int iProf = 1; iProf <= aNbProf; ++iProf)
    {
      aSum += myProfileParams(iProf, jGuid);
    }
    aAvgProfileParams(jGuid) = aSum / aNbProf;
  }

  NCollection_Array1<double> aAvgGuideParams(1, aNbProf);
  for (int iProf = 1; iProf <= aNbProf; ++iProf)
  {
    double aSum = 0.0;
    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      aSum += myGuideParams(iProf, jGuid);
    }
    aAvgGuideParams(iProf) = aSum / aNbGuid;
  }

  // Reparametrize each profile.
  for (int iProf = 1; iProf <= aNbProf; ++iProf)
  {
    NCollection_Array1<double> anOldParams(1, aNbGuid);
    NCollection_Array1<double> aNewParams(1, aNbGuid);
    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      anOldParams(jGuid) = myProfileParams(iProf, jGuid);
      aNewParams(jGuid)  = aAvgProfileParams(jGuid);
    }

    if (!reparamCurve(myProfiles(iProf), anOldParams, aNewParams))
    {
      return false;
    }

    for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
    {
      myProfileParams(iProf, jGuid) = aAvgProfileParams(jGuid);
    }
  }

  // Reparametrize each guide.
  for (int jGuid = 1; jGuid <= aNbGuid; ++jGuid)
  {
    NCollection_Array1<double> anOldParams(1, aNbProf);
    NCollection_Array1<double> aNewParams(1, aNbProf);
    for (int iProf = 1; iProf <= aNbProf; ++iProf)
    {
      anOldParams(iProf) = myGuideParams(iProf, jGuid);
      aNewParams(iProf)  = aAvgGuideParams(iProf);
    }

    if (!reparamCurve(myGuides(jGuid), anOldParams, aNewParams))
    {
      return false;
    }

    for (int iProf = 1; iProf <= aNbProf; ++iProf)
    {
      myGuideParams(iProf, jGuid) = aAvgGuideParams(iProf);
    }
  }

  return true;
}
