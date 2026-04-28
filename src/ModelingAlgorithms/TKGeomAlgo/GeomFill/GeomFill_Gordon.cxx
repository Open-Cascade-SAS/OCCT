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
#include <GeomAdaptor_Curve.hxx>
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

//! Relative threshold for replacing uniform samples by break points.
constexpr double THE_REPLACE_EPS = 0.3;

//! Epsilon for duplicate-parameter compaction.
constexpr double THE_PARAM_UNIQUE_EPS = 1.0e-14;

//! Maximum iterations for reparametrization inversion by bisection.
constexpr int THE_MAX_BISECT_ITER = 50;

//! Convergence tolerance for bisection-based inversion.
constexpr double THE_BISECT_TOL = 1.0e-12;

//! Parameter tolerance for duplicate/interior filtering.
constexpr double THE_REPARAM_PARAM_TOL = 1.0e-10;

//! Offset added to current control-point count when selecting sample count.
constexpr int THE_SAMPLE_COUNT_CP_OFFSET = 10;

//! Offset added to break count when selecting sample count.
constexpr int THE_SAMPLE_COUNT_BREAK_OFFSET = 2;

//! Minimum sample count for robust approximation.
constexpr int THE_SAMPLE_COUNT_MIN = 30;

//! Maximum sample count for robust approximation.
constexpr int THE_SAMPLE_COUNT_MAX = 200;

//! Target approximation degree during curve reparametrization.
constexpr int THE_APPROX_DEGREE = 3;

//! Divisor used for rough control-point estimation from sample count.
constexpr int THE_CP_ESTIMATE_DIV = 3;

//! Relative tolerance factor for post-reparametrization compatibility checks.
constexpr double THE_NETWORK_CHECK_TOL_FACTOR = 1.0e-5;

//! Initializes adaptor arrays for fast repeated curve evaluations.
void loadCurveAdaptors(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves,
                       NCollection_Array1<GeomAdaptor_Curve>&                    theAdaptors)
{
  for (int aCurveIdx = theCurves.Lower(); aCurveIdx <= theCurves.Upper(); ++aCurveIdx)
  {
    theAdaptors(aCurveIdx).Load(theCurves(aCurveIdx));
  }
}

//! Detects kink (C0 discontinuity) parameters on a B-spline curve.
//! A kink exists where knot multiplicity equals the curve degree
//! and the tangent vectors on either side differ by more than 6 degrees.
//! @param[in] theCurve the B-spline curve to analyze
//! @return sorted array of kink parameter values (may be empty)
NCollection_DynamicArray<double> getKinkParameters(const occ::handle<Geom_BSplineCurve>& theCurve)
{
  NCollection_DynamicArray<double> aKinks;
  const int                        aDegree = theCurve->Degree();

  for (int aKnotIdx = 2; aKnotIdx < theCurve->NbKnots(); ++aKnotIdx)
  {
    if (theCurve->Multiplicity(aKnotIdx) == aDegree)
    {
      double aKnot     = theCurve->Knot(aKnotIdx);
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
  for (int aValueIdx = 0; aValueIdx < theNbValues; ++aValueIdx)
  {
    aResult.Append(theUMin + aValueIdx * aDu);
  }

  for (int aBreakIdx = theBreaks.Lower(); aBreakIdx <= theBreaks.Upper(); ++aBreakIdx)
  {
    double aBreak = theBreaks(aBreakIdx);

    // Try to find a grid point within replace tolerance.
    bool isReplaced = false;
    for (int aGridIdx = 0; aGridIdx < aResult.Length(); ++aGridIdx)
    {
      if (std::abs(aResult[aGridIdx] - aBreak) < aDu * THE_REPLACE_EPS)
      {
        aResult[aGridIdx] = aBreak;
        isReplaced        = true;
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
  for (int aUniqueIdx = 1; aUniqueIdx < aResult.Length(); ++aUniqueIdx)
  {
    if (std::abs(aResult[aUniqueIdx] - aResult[aWriteIdx]) > THE_PARAM_UNIQUE_EPS)
    {
      ++aWriteIdx;
      aResult[aWriteIdx] = aResult[aUniqueIdx];
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
  NCollection_DynamicArray<double> anOldSeq;
  NCollection_DynamicArray<double> aNewSeq;

  // Start of curve.
  anOldSeq.Append(theCurve->FirstParameter());
  aNewSeq.Append(0.0);

  // Interior intersection points.
  for (int aParamIdx = theOldParams.Lower(); aParamIdx <= theOldParams.Upper(); ++aParamIdx)
  {
    double anOld = theOldParams(aParamIdx);
    double aNew  = theNewParams(aParamIdx);

    if (!anOldSeq.IsEmpty()
        && (std::abs(anOld - anOldSeq.Last()) < THE_REPARAM_PARAM_TOL
            || std::abs(aNew - aNewSeq.Last()) < THE_REPARAM_PARAM_TOL))
    {
      continue;
    }
    if (std::abs(anOld - theCurve->LastParameter()) < THE_REPARAM_PARAM_TOL
        || std::abs(aNew - 1.0) < THE_REPARAM_PARAM_TOL)
    {
      continue;
    }

    anOldSeq.Append(anOld);
    aNewSeq.Append(aNew);
  }

  // End of curve.
  if (anOldSeq.Length() < 2
      || std::abs(anOldSeq.Last() - theCurve->LastParameter()) > THE_REPARAM_PARAM_TOL)
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
  for (int aPointIdx = 0; aPointIdx < aNbPts; ++aPointIdx)
  {
    aOldPnts2d->SetValue(aPointIdx + 1, gp_Pnt2d(anOldSeq[aPointIdx], 0.0));
    aNewParamsH->SetValue(aPointIdx + 1, aNewSeq[aPointIdx]);
  }

  Geom2dAPI_Interpolate anInterp2d(aOldPnts2d, aNewParamsH, false, Precision::PConfusion());
  anInterp2d.Perform();
  if (!anInterp2d.IsDone())
  {
    return false;
  }

  occ::handle<Geom2d_BSplineCurve> aReparamFunc = anInterp2d.Curve();
  GeomAdaptor_Curve                aCurveAdaptor(theCurve);

  // Detect kinks on the original curve.
  NCollection_DynamicArray<double> aKinks = getKinkParameters(theCurve);

  // Pre-compute sizes for break and kink arrays.
  const int aNbInteriorParams = std::max(0, theNewParams.Upper() - theNewParams.Lower() - 1);
  const int aNbKinks          = aKinks.Length();
  const int aNbBreaks         = aNbInteriorParams + aNbKinks;

  // Map kink old-parameters to new-parameter space via bisection inversion.
  NCollection_Array1<double> aKinksInNew(1, std::max(aNbKinks, 1));
  for (int aKinkIdx = 0; aKinkIdx < aNbKinks; ++aKinkIdx)
  {
    aKinksInNew(aKinkIdx + 1) = invertReparamBisection(aReparamFunc, aKinks[aKinkIdx]);
  }

  // Determine sample count, clamped to a robust range.
  const int aMaxCp = theCurve->NbPoles();
  int       aNbSamples =
    std::max(aMaxCp + THE_SAMPLE_COUNT_CP_OFFSET, aNbBreaks + THE_SAMPLE_COUNT_BREAK_OFFSET);
  aNbSamples = std::max(THE_SAMPLE_COUNT_MIN, std::min(THE_SAMPLE_COUNT_MAX, aNbSamples));

  // Build break list: interior new-params + kink params mapped to new space.
  NCollection_DynamicArray<double> aSampleParams;
  if (aNbBreaks > 0)
  {
    NCollection_Array1<double> aBreaks(1, aNbBreaks);
    int                        aBrkIdx = 1;
    for (int aNewParamIdx = theNewParams.Lower() + 1; aNewParamIdx < theNewParams.Upper();
         ++aNewParamIdx)
    {
      aBreaks(aBrkIdx++) = theNewParams(aNewParamIdx);
    }
    for (int aKinkIdx = 0; aKinkIdx < aNbKinks; ++aKinkIdx)
    {
      aBreaks(aBrkIdx++) = aKinksInNew(aKinkIdx + 1);
    }
    aSampleParams = linspaceWithBreaks(0.0, 1.0, aNbSamples, aBreaks);
  }
  else
  {
    // No breaks - uniform sampling.
    for (int aSampleIdx = 0; aSampleIdx < aNbSamples; ++aSampleIdx)
    {
      aSampleParams.Append(static_cast<double>(aSampleIdx) / static_cast<double>(aNbSamples - 1));
    }
  }

  const int aNbActual = aSampleParams.Length();

  // Sample original curve at mapped old-parameter positions.
  NCollection_Array1<gp_Pnt> aSampledPnts(1, aNbActual);
  NCollection_Array1<double> aSampledNewParams(1, aNbActual);

  for (int aSampleIdx = 0; aSampleIdx < aNbActual; ++aSampleIdx)
  {
    double   aNewParam  = aSampleParams[aSampleIdx];
    gp_Pnt2d anOldPt    = aReparamFunc->Value(aNewParam);
    double   anOldParam = anOldPt.X();

    // Clamp to valid range.
    anOldParam =
      std::max(aCurveAdaptor.FirstParameter(), std::min(aCurveAdaptor.LastParameter(), anOldParam));

    aSampledPnts(aSampleIdx + 1)      = aCurveAdaptor.EvalD0(anOldParam);
    aSampledNewParams(aSampleIdx + 1) = aNewParam;
  }

  // Determine number of control points.
  // Must be >= nInterpolated + degree + 1 for the KKT system to be solvable.
  const int aNbInterp = 2 + aNbKinks; // endpoints + kinks
  int       aNbCP =
    std::max(theCurve->NbPoles(),
             std::max(aNbActual / THE_CP_ESTIMATE_DIV, aNbInterp + THE_APPROX_DEGREE + 1));
  aNbCP = std::max(aNbCP, THE_APPROX_DEGREE + 1);

  // Use constrained least-squares approximation with interpolation at endpoints and kinks.
  Approx_BSplineApproxInterp anApprox(aSampledPnts, aNbCP, THE_APPROX_DEGREE, false);

  // Interpolate first and last points exactly.
  anApprox.InterpolatePoint(0);
  anApprox.InterpolatePoint(aNbActual - 1);

  // Interpolate kink points with C0 break.
  for (int aKinkArrIdx = 1; aKinkArrIdx <= aNbKinks; ++aKinkArrIdx)
  {
    double aKinkNew = aKinksInNew(aKinkArrIdx);
    // Find the closest sample index for this kink parameter.
    int    aBestIdx  = 0;
    double aBestDist = RealLast();
    for (int aSampleArrIdx = 1; aSampleArrIdx <= aNbActual; ++aSampleArrIdx)
    {
      double aDist = std::abs(aSampledNewParams(aSampleArrIdx) - aKinkNew);
      if (aDist < aBestDist)
      {
        aBestDist = aDist;
        aBestIdx  = aSampleArrIdx - 1; // 0-based index for InterpolatePoint
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

GeomFill_Gordon::GeomFill_Gordon() = default;

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
    // Store counts but don't try to convert - Perform() will check and return.
    myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, std::max(aNbProf, 0));
    myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, std::max(aNbGuid, 0));
    return;
  }

  // Convert input arrays to BSpline storage arrays.
  myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, aNbProf);
  myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, aNbGuid);

  for (int aProfileInputIdx = theProfiles.Lower(); aProfileInputIdx <= theProfiles.Upper();
       ++aProfileInputIdx)
  {
    myProfiles(aProfileInputIdx - theProfiles.Lower() + 1) =
      GeomConvert::CurveToBSplineCurve(theProfiles(aProfileInputIdx));
  }
  for (int aGuideInputIdx = theGuides.Lower(); aGuideInputIdx <= theGuides.Upper();
       ++aGuideInputIdx)
  {
    myGuides(aGuideInputIdx - theGuides.Lower() + 1) =
      GeomConvert::CurveToBSplineCurve(theGuides(aGuideInputIdx));
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
  for (int aProfileIdx = myProfiles.Lower(); aProfileIdx <= myProfiles.Upper(); ++aProfileIdx)
  {
    aProfileProfiler.AddCurve(myProfiles(aProfileIdx));
  }
  aProfileProfiler.Perform(Precision::PConfusion());

  // Extract unified profile curves.
  for (int aProfileIdx = myProfiles.Lower(); aProfileIdx <= myProfiles.Upper(); ++aProfileIdx)
  {
    myProfiles(aProfileIdx) =
      occ::down_cast<Geom_BSplineCurve>(aProfileProfiler.Curve(aProfileIdx));
  }

  // Step 10: Make guides compatible.
  GeomFill_Profiler aGuideProfiler;
  for (int aGuideIdx = myGuides.Lower(); aGuideIdx <= myGuides.Upper(); ++aGuideIdx)
  {
    aGuideProfiler.AddCurve(myGuides(aGuideIdx));
  }
  aGuideProfiler.Perform(Precision::PConfusion());

  for (int aGuideIdx = myGuides.Lower(); aGuideIdx <= myGuides.Upper(); ++aGuideIdx)
  {
    myGuides(aGuideIdx) = occ::down_cast<Geom_BSplineCurve>(aGuideProfiler.Curve(aGuideIdx));
  }

  // Step 11: Prepare averaged intersection parameters for GordonBuilder.
  const int aNbProf = myProfiles.Length();
  const int aNbGuid = myGuides.Length();

  NCollection_Array1<double> aGuideParamValues(1, aNbGuid);
  for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
  {
    double aSum = 0.0;
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      aSum += myProfileParams(aProfileIdx, aGuideIdx);
    }
    aGuideParamValues(aGuideIdx) = aSum / aNbProf;
  }

  NCollection_Array1<double> aProfileParamValues(1, aNbProf);
  for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    double aSum = 0.0;
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      aSum += myGuideParams(aProfileIdx, aGuideIdx);
    }
    aProfileParamValues(aProfileIdx) = aSum / aNbGuid;
  }

  // Step 12: Delegate to GordonBuilder.
  GeomFill_GordonBuilder aBuilder;
  aBuilder.SetParallelMode(myToUseParallel);
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
  for (int aProfileIdx = myProfiles.Lower(); aProfileIdx <= myProfiles.Upper(); ++aProfileIdx)
  {
    occ::handle<Geom_BSplineCurve>& aCurve = myProfiles(aProfileIdx);
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

  for (int aGuideIdx = myGuides.Lower(); aGuideIdx <= myGuides.Upper(); ++aGuideIdx)
  {
    occ::handle<Geom_BSplineCurve>& aCurve = myGuides(aGuideIdx);
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

  auto processIntersection = [&](int theIdx) -> bool {
    const int aProfileIdx = theIdx / aNbGuid + 1;
    const int aGuideIdx   = theIdx % aNbGuid + 1;

    const occ::handle<Geom_BSplineCurve>& aProfile = myProfiles(aProfileIdx);
    const occ::handle<Geom_BSplineCurve>& aGuide   = myGuides(aGuideIdx);

    GeomAPI_ExtremaCurveCurve anExtrema(aProfile, aGuide);

    // Find the extremum with distance below tolerance.
    bool   isFound    = false;
    double aMinDist   = RealLast();
    double aProfParam = 0.0;
    double aGuidParam = 0.0;

    for (int anExtIdx = 1; anExtIdx <= anExtrema.NbExtrema(); ++anExtIdx)
    {
      double aDist = anExtrema.Distance(anExtIdx);
      if (aDist < myTolerance && aDist < aMinDist)
      {
        aMinDist = aDist;
        anExtrema.Parameters(anExtIdx, aProfParam, aGuidParam);
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
          if (anExtrema.TotalLowerDistanceParameters(aProfParam, aGuidParam))
          {
            isFound = true;
          }
        }
      }
    }

    if (!isFound)
    {
      return false;
    }

    myProfileParams(aProfileIdx, aGuideIdx) = aProfParam;
    myGuideParams(aProfileIdx, aGuideIdx)   = aGuidParam;
    return true;
  };

  // Use atomic flag for thread-safe error reporting.
  // OSD_Parallel::For executes in single-thread mode when the force flag is true.
  std::atomic<bool> isOk{true};
  OSD_Parallel::For(
    0,
    aNbTotal,
    [&](int theIdx) {
      if (!isOk.load(std::memory_order_relaxed))
      {
        return;
      }
      if (!processIntersection(theIdx))
      {
        isOk.store(false, std::memory_order_relaxed);
      }
    },
    !myToUseParallel);

  return isOk.load();
}

//=================================================================================================

void GeomFill_Gordon::computeScale()
{
  // Compute bounding box diagonal from all curve start/end points.
  if (myProfiles.Length() <= 0 && myGuides.Length() <= 0)
  {
    myScale = 1.0;
    return;
  }

  NCollection_Array1<GeomAdaptor_Curve> aProfileAdaptors(myProfiles.Lower(), myProfiles.Upper());
  NCollection_Array1<GeomAdaptor_Curve> aGuideAdaptors(myGuides.Lower(), myGuides.Upper());
  loadCurveAdaptors(myProfiles, aProfileAdaptors);
  loadCurveAdaptors(myGuides, aGuideAdaptors);

  gp_Pnt aSeedPnt;
  if (myProfiles.Length() > 0)
  {
    const GeomAdaptor_Curve& aSeedAdaptor = aProfileAdaptors(myProfiles.Lower());
    aSeedPnt                              = aSeedAdaptor.EvalD0(aSeedAdaptor.FirstParameter());
  }
  else
  {
    const GeomAdaptor_Curve& aSeedAdaptor = aGuideAdaptors(myGuides.Lower());
    aSeedPnt                              = aSeedAdaptor.EvalD0(aSeedAdaptor.FirstParameter());
  }

  gp_Pnt aMin = aSeedPnt;
  gp_Pnt aMax = aSeedPnt;

  auto updateBBox = [&](const gp_Pnt& thePnt) {
    aMin.SetX(std::min(aMin.X(), thePnt.X()));
    aMin.SetY(std::min(aMin.Y(), thePnt.Y()));
    aMin.SetZ(std::min(aMin.Z(), thePnt.Z()));
    aMax.SetX(std::max(aMax.X(), thePnt.X()));
    aMax.SetY(std::max(aMax.Y(), thePnt.Y()));
    aMax.SetZ(std::max(aMax.Z(), thePnt.Z()));
  };

  for (int aProfileIdx = myProfiles.Lower(); aProfileIdx <= myProfiles.Upper(); ++aProfileIdx)
  {
    const GeomAdaptor_Curve& aProfileAdaptor = aProfileAdaptors(aProfileIdx);
    updateBBox(aProfileAdaptor.EvalD0(aProfileAdaptor.FirstParameter()));
    updateBBox(aProfileAdaptor.EvalD0(aProfileAdaptor.LastParameter()));
  }
  for (int aGuideIdx = myGuides.Lower(); aGuideIdx <= myGuides.Upper(); ++aGuideIdx)
  {
    const GeomAdaptor_Curve& aGuideAdaptor = aGuideAdaptors(aGuideIdx);
    updateBBox(aGuideAdaptor.EvalD0(aGuideAdaptor.FirstParameter()));
    updateBBox(aGuideAdaptor.EvalD0(aGuideAdaptor.LastParameter()));
  }

  const double aDiag = aMax.Distance(aMin);
  myScale            = std::max(aDiag, 1.0);
}

//=================================================================================================

void GeomFill_Gordon::detectClosedness()
{
  const int                             aNbProf   = myProfiles.Length();
  const int                             aNbGuid   = myGuides.Length();
  const double                          aCloseTol = THE_REL_TOL_CLOSED * myScale;
  NCollection_Array1<GeomAdaptor_Curve> aProfileAdaptors(myProfiles.Lower(), myProfiles.Upper());
  NCollection_Array1<GeomAdaptor_Curve> aGuideAdaptors(myGuides.Lower(), myGuides.Upper());
  loadCurveAdaptors(myProfiles, aProfileAdaptors);
  loadCurveAdaptors(myGuides, aGuideAdaptors);

  // U-direction closure: first and last guides are geometrically equal
  // and intersection grid's first/last columns match.
  {
    gp_Pnt aGuidFirstStart = aGuideAdaptors(1).EvalD0(aGuideAdaptors(1).FirstParameter());
    gp_Pnt aGuidFirstEnd   = aGuideAdaptors(1).EvalD0(aGuideAdaptors(1).LastParameter());
    gp_Pnt aGuidLastStart =
      aGuideAdaptors(aNbGuid).EvalD0(aGuideAdaptors(aNbGuid).FirstParameter());
    gp_Pnt aGuidLastEnd = aGuideAdaptors(aNbGuid).EvalD0(aGuideAdaptors(aNbGuid).LastParameter());

    bool isGuidesEqual = aGuidFirstStart.IsEqual(aGuidLastStart, aCloseTol)
                         && aGuidFirstEnd.IsEqual(aGuidLastEnd, aCloseTol);

    bool isGridClosed = true;
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf && isGridClosed; ++aProfileIdx)
    {
      gp_Pnt aP1   = aProfileAdaptors(aProfileIdx).EvalD0(myProfileParams(aProfileIdx, 1));
      gp_Pnt aP2   = aProfileAdaptors(aProfileIdx).EvalD0(myProfileParams(aProfileIdx, aNbGuid));
      isGridClosed = aP1.IsEqual(aP2, aCloseTol);
    }

    myIsUClosed = isGuidesEqual && isGridClosed;
  }

  // V-direction closure: first and last profiles are geometrically equal
  // and intersection grid's first/last rows match.
  {
    gp_Pnt aProfFirstStart = aProfileAdaptors(1).EvalD0(aProfileAdaptors(1).FirstParameter());
    gp_Pnt aProfFirstEnd   = aProfileAdaptors(1).EvalD0(aProfileAdaptors(1).LastParameter());
    gp_Pnt aProfLastStart =
      aProfileAdaptors(aNbProf).EvalD0(aProfileAdaptors(aNbProf).FirstParameter());
    gp_Pnt aProfLastEnd =
      aProfileAdaptors(aNbProf).EvalD0(aProfileAdaptors(aNbProf).LastParameter());

    bool isProfilesEqual = aProfFirstStart.IsEqual(aProfLastStart, aCloseTol)
                           && aProfFirstEnd.IsEqual(aProfLastEnd, aCloseTol);

    bool isGridClosed = true;
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid && isGridClosed; ++aGuideIdx)
    {
      gp_Pnt aP1   = aGuideAdaptors(aGuideIdx).EvalD0(myGuideParams(1, aGuideIdx));
      gp_Pnt aP2   = aGuideAdaptors(aGuideIdx).EvalD0(myGuideParams(aNbProf, aGuideIdx));
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

  for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    const double aFirst = myProfiles(aProfileIdx)->FirstParameter();
    const double aLast  = myProfiles(aProfileIdx)->LastParameter();
    const double aRange = aLast - aFirst;

    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      double& aParam = myProfileParams(aProfileIdx, aGuideIdx);
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

  for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
  {
    const double aFirst = myGuides(aGuideIdx)->FirstParameter();
    const double aLast  = myGuides(aGuideIdx)->LastParameter();
    const double aRange = aLast - aFirst;

    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      double& aParam = myGuideParams(aProfileIdx, aGuideIdx);
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
  const int                             aNbProf = myProfiles.Length();
  const int                             aNbGuid = myGuides.Length();
  NCollection_Array1<GeomAdaptor_Curve> aProfileAdaptors(myProfiles.Lower(), myProfiles.Upper());
  NCollection_Array1<GeomAdaptor_Curve> aGuideAdaptors(myGuides.Lower(), myGuides.Upper());
  loadCurveAdaptors(myProfiles, aProfileAdaptors);
  loadCurveAdaptors(myGuides, aGuideAdaptors);

  const double aCheckTol = myScale * THE_NETWORK_CHECK_TOL_FACTOR;

  for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      gp_Pnt aProfPt =
        aProfileAdaptors(aProfileIdx).EvalD0(myProfileParams(aProfileIdx, aGuideIdx));
      gp_Pnt aGuidPt = aGuideAdaptors(aGuideIdx).EvalD0(myGuideParams(aProfileIdx, aGuideIdx));

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

  for (int aProfileIdx = 1; aProfileIdx <= aNbProf && !isStartFound; ++aProfileIdx)
  {
    // Find guide with minimum parameter on this profile.
    int    aMinGuideIdx = 1;
    double aMinVal      = myProfileParams(aProfileIdx, 1);
    for (int aGuideIdx = 2; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      if (myProfileParams(aProfileIdx, aGuideIdx) < aMinVal)
      {
        aMinVal      = myProfileParams(aProfileIdx, aGuideIdx);
        aMinGuideIdx = aGuideIdx;
      }
    }

    // Check if this profile has the minimum parameter on that guide.
    int    aMinProfileIdx = 1;
    double aMinGuideVal   = myGuideParams(1, aMinGuideIdx);
    for (int aProfileScanIdx = 2; aProfileScanIdx <= aNbProf; ++aProfileScanIdx)
    {
      if (myGuideParams(aProfileScanIdx, aMinGuideIdx) < aMinGuideVal)
      {
        aMinGuideVal   = myGuideParams(aProfileScanIdx, aMinGuideIdx);
        aMinProfileIdx = aProfileScanIdx;
      }
    }

    if (aMinProfileIdx == aProfileIdx)
    {
      aStartProf        = aProfileIdx;
      aStartGuid        = aMinGuideIdx;
      aGuideMustReverse = false;
      isStartFound      = true;
    }
  }

  // Fallback: check for reversed guide start.
  if (!isStartFound)
  {
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf && !isStartFound; ++aProfileIdx)
    {
      int    aMinGuideIdx = 1;
      double aMinVal      = myProfileParams(aProfileIdx, 1);
      for (int aGuideIdx = 2; aGuideIdx <= aNbGuid; ++aGuideIdx)
      {
        if (myProfileParams(aProfileIdx, aGuideIdx) < aMinVal)
        {
          aMinVal      = myProfileParams(aProfileIdx, aGuideIdx);
          aMinGuideIdx = aGuideIdx;
        }
      }

      // Check if this profile has the MAXIMUM parameter on that guide.
      int    aMaxProfileIdx = 1;
      double aMaxGuideVal   = myGuideParams(1, aMinGuideIdx);
      for (int aProfileScanIdx = 2; aProfileScanIdx <= aNbProf; ++aProfileScanIdx)
      {
        if (myGuideParams(aProfileScanIdx, aMinGuideIdx) > aMaxGuideVal)
        {
          aMaxGuideVal   = myGuideParams(aProfileScanIdx, aMinGuideIdx);
          aMaxProfileIdx = aProfileScanIdx;
        }
      }

      if (aMaxProfileIdx == aProfileIdx)
      {
        aStartProf        = aProfileIdx;
        aStartGuid        = aMinGuideIdx;
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
  auto swapProfiles = [this, aNbGuid](int aProfileIdx1, int aProfileIdx2) {
    if (aProfileIdx1 == aProfileIdx2)
      return;
    std::swap(myProfiles(aProfileIdx1), myProfiles(aProfileIdx2));
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      std::swap(myProfileParams(aProfileIdx1, aGuideIdx), myProfileParams(aProfileIdx2, aGuideIdx));
      std::swap(myGuideParams(aProfileIdx1, aGuideIdx), myGuideParams(aProfileIdx2, aGuideIdx));
    }
  };

  auto swapGuides = [this, aNbProf](int aGuideIdx1, int aGuideIdx2) {
    if (aGuideIdx1 == aGuideIdx2)
      return;
    std::swap(myGuides(aGuideIdx1), myGuides(aGuideIdx2));
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      std::swap(myProfileParams(aProfileIdx, aGuideIdx1), myProfileParams(aProfileIdx, aGuideIdx2));
      std::swap(myGuideParams(aProfileIdx, aGuideIdx1), myGuideParams(aProfileIdx, aGuideIdx2));
    }
  };

  auto reverseProfile = [this, aNbGuid](int aProfileIdx) {
    double aFirst = myProfiles(aProfileIdx)->FirstParameter();
    double aLast  = myProfiles(aProfileIdx)->LastParameter();
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      myProfileParams(aProfileIdx, aGuideIdx) =
        -myProfileParams(aProfileIdx, aGuideIdx) + aFirst + aLast;
    }
    myProfiles(aProfileIdx)->Reverse();
  };

  auto reverseGuide = [this, aNbProf](int aGuideIdx) {
    double aFirst = myGuides(aGuideIdx)->FirstParameter();
    double aLast  = myGuides(aGuideIdx)->LastParameter();
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      myGuideParams(aProfileIdx, aGuideIdx) =
        -myGuideParams(aProfileIdx, aGuideIdx) + aFirst + aLast;
    }
    myGuides(aGuideIdx)->Reverse();
  };

  // Move start curves to position 1.
  swapProfiles(1, aStartProf);
  swapGuides(1, aStartGuid);

  if (aGuideMustReverse)
  {
    reverseGuide(1);
  }

  // Bubble sort guides by their intersection parameter on the first profile (ascending).
  for (int aGuideSortEnd = aNbGuid; aGuideSortEnd > 1; --aGuideSortEnd)
  {
    for (int aGuideIdx = 2; aGuideIdx < aGuideSortEnd; ++aGuideIdx)
    {
      if (myProfileParams(1, aGuideIdx) > myProfileParams(1, aGuideIdx + 1))
      {
        swapGuides(aGuideIdx, aGuideIdx + 1);
      }
    }
  }

  // Bubble sort profiles by their intersection parameter on the first guide (ascending).
  for (int aProfileSortEnd = aNbProf; aProfileSortEnd > 1; --aProfileSortEnd)
  {
    for (int aProfileIdx = 2; aProfileIdx < aProfileSortEnd; ++aProfileIdx)
    {
      if (myGuideParams(aProfileIdx, 1) > myGuideParams(aProfileIdx + 1, 1))
      {
        swapProfiles(aProfileIdx, aProfileIdx + 1);
      }
    }
  }

  // Reverse individual profiles if their parameters go in the wrong direction.
  for (int aProfileIdx = 2; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    if (myProfileParams(aProfileIdx, 1) > myProfileParams(aProfileIdx, aNbGuid))
    {
      reverseProfile(aProfileIdx);
    }
  }

  // Reverse individual guides if their parameters go in the wrong direction.
  for (int aGuideIdx = 2; aGuideIdx <= aNbGuid; ++aGuideIdx)
  {
    if (myGuideParams(1, aGuideIdx) > myGuideParams(aNbProf, aGuideIdx))
    {
      reverseGuide(aGuideIdx);
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
  for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
  {
    double aSum = 0.0;
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      aSum += myProfileParams(aProfileIdx, aGuideIdx);
    }
    aAvgProfileParams(aGuideIdx) = aSum / aNbProf;
  }

  NCollection_Array1<double> aAvgGuideParams(1, aNbProf);
  for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    double aSum = 0.0;
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      aSum += myGuideParams(aProfileIdx, aGuideIdx);
    }
    aAvgGuideParams(aProfileIdx) = aSum / aNbGuid;
  }

  // Reparametrize each profile.
  for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
  {
    NCollection_Array1<double> anOldParams(1, aNbGuid);
    NCollection_Array1<double> aNewParams(1, aNbGuid);
    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      anOldParams(aGuideIdx) = myProfileParams(aProfileIdx, aGuideIdx);
      aNewParams(aGuideIdx)  = aAvgProfileParams(aGuideIdx);
    }

    if (!reparamCurve(myProfiles(aProfileIdx), anOldParams, aNewParams))
    {
      return false;
    }

    for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
    {
      myProfileParams(aProfileIdx, aGuideIdx) = aAvgProfileParams(aGuideIdx);
    }
  }

  // Reparametrize each guide.
  for (int aGuideIdx = 1; aGuideIdx <= aNbGuid; ++aGuideIdx)
  {
    NCollection_Array1<double> anOldParams(1, aNbProf);
    NCollection_Array1<double> aNewParams(1, aNbProf);
    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      anOldParams(aProfileIdx) = myGuideParams(aProfileIdx, aGuideIdx);
      aNewParams(aProfileIdx)  = aAvgGuideParams(aProfileIdx);
    }

    if (!reparamCurve(myGuides(aGuideIdx), anOldParams, aNewParams))
    {
      return false;
    }

    for (int aProfileIdx = 1; aProfileIdx <= aNbProf; ++aProfileIdx)
    {
      myGuideParams(aProfileIdx, aGuideIdx) = aAvgGuideParams(aProfileIdx);
    }
  }

  return true;
}
