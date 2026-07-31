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

#include <BSplCLib.hxx>
#include <BSplCLib_EvaluatorFunction.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_NetworkSurface.hxx>
#include <GeomGridEval_Curve.hxx>
#include <GeomGridEval_Surface.hxx>
#include <GeomLib_Interpolate.hxx>
#include <GeomLib_Tool.hxx>
#include <NCollection_LinearVector.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <atomic>

#include "GeomFill_GordonUtilities.pxx"

namespace
{

//=================================================================================================

// Returns whether at least one prepared curve uses rational weights.
bool hasRationalCurve(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    if (!theCurves.At(aCurveIdx).IsNull() && theCurves.At(aCurveIdx)->IsRational())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

// Returns whether either prepared curve family uses rational weights.
bool hasRationalCurve(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                      const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides)
{
  return hasRationalCurve(theProfiles) || hasRationalCurve(theGuides);
}

//=================================================================================================

// Converts detailed network-construction statuses to the public Gordon status.
GeomFill_Gordon::ResultStatus mapNetworkStatus(
  const GeomFill_NetworkSurface::ResultStatus theStatus)
{
  switch (theStatus)
  {
    case GeomFill_NetworkSurface::ResultStatus::Done:
      return GeomFill_Gordon::ResultStatus::Done;
    case GeomFill_NetworkSurface::ResultStatus::InvalidInput:
      return GeomFill_Gordon::ResultStatus::CompatibilityFailed;
    case GeomFill_NetworkSurface::ResultStatus::CurveCompatibilityFailed:
      return GeomFill_Gordon::ResultStatus::CurveCompatibilityFailed;
    case GeomFill_NetworkSurface::ResultStatus::SkinningFailed:
      return GeomFill_Gordon::ResultStatus::SkinningFailed;
    case GeomFill_NetworkSurface::ResultStatus::ReferenceSurfaceFailed:
      return GeomFill_Gordon::ResultStatus::ReferenceSurfaceFailed;
    case GeomFill_NetworkSurface::ResultStatus::KnotAlignmentFailed:
      return GeomFill_Gordon::ResultStatus::KnotAlignmentFailed;
    case GeomFill_NetworkSurface::ResultStatus::RationalDegreeOverflow:
      return GeomFill_Gordon::ResultStatus::RationalDegreeOverflow;
    case GeomFill_NetworkSurface::ResultStatus::RationalConstructionFailed:
      return GeomFill_Gordon::ResultStatus::RationalConstructionFailed;
    case GeomFill_NetworkSurface::ResultStatus::PeriodicityFailed:
      return GeomFill_Gordon::ResultStatus::PeriodicityFailed;
    case GeomFill_NetworkSurface::ResultStatus::NotStarted:
    case GeomFill_NetworkSurface::ResultStatus::ConstructionFailed:
      return GeomFill_Gordon::ResultStatus::ConstructionFailed;
  }
  return GeomFill_Gordon::ResultStatus::ConstructionFailed;
}

//=================================================================================================

// Returns true when a sampled fallback can still use a validated prepared network.
bool canApproximateAfterExactFailure(const GeomFill_Gordon::ResultStatus theStatus)
{
  return theStatus == GeomFill_Gordon::ResultStatus::CurveCompatibilityFailed
         || theStatus == GeomFill_Gordon::ResultStatus::SkinningFailed
         || theStatus == GeomFill_Gordon::ResultStatus::ReferenceSurfaceFailed
         || theStatus == GeomFill_Gordon::ResultStatus::KnotAlignmentFailed
         || theStatus == GeomFill_Gordon::ResultStatus::RationalDegreeOverflow
         || theStatus == GeomFill_Gordon::ResultStatus::RationalConstructionFailed
         || theStatus == GeomFill_Gordon::ResultStatus::ConstructionFailed;
}

// Chooses a deterministic fallback grid density from the prepared network itself.
size_t fallbackSampleCount(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  size_t aNbSamples = std::max<size_t>(theCurves.Size(), 2);
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    if (!theCurves.At(aCurveIdx).IsNull())
    {
      aNbSamples = std::max(aNbSamples, static_cast<size_t>(theCurves.At(aCurveIdx)->NbPoles()));
    }
  }
  return std::max(aNbSamples, static_cast<size_t>(Geom_BSplineCurve::MaxDegree()) / 2);
}

//=================================================================================================

// Returns regularly spaced parameters in the unit interval.
NCollection_Array1<double> unitParameters(const size_t theNbSamples)
{
  NCollection_Array1<double> aParameters(1, static_cast<int>(theNbSamples));
  for (size_t aSampleIdx = 0; aSampleIdx < theNbSamples; ++aSampleIdx)
  {
    aParameters.ChangeAt(aSampleIdx) =
      static_cast<double>(aSampleIdx) / static_cast<double>(theNbSamples - 1);
  }
  return aParameters;
}

//=================================================================================================

// Fits a non-exact B-spline surface through a point grid for approximate fallback.
occ::handle<Geom_BSplineSurface> approximateGrid(const NCollection_Array2<gp_Pnt>& thePoints,
                                                 const double                      theTolerance)
{
  GeomAPI_PointsToBSplineSurface anApprox;
  anApprox.Init(thePoints,
                Approx_IsoParametric,
                1,
                Geom_BSplineSurface::MaxDegree(),
                GeomAbs_C1,
                std::max(theTolerance, Precision::Confusion()));
  return anApprox.IsDone() ? anApprox.Surface() : nullptr;
}

//=================================================================================================

// Samples the prepared profiles as section curves of an approximate profile skin.
NCollection_Array2<gp_Pnt> profileSampleGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const size_t                                              theNbUSamples)
{
  const NCollection_Array1<double> aUParams = unitParameters(theNbUSamples);
  NCollection_Array2<gp_Pnt>       aPoints(1,
                                     static_cast<int>(theNbUSamples),
                                     1,
                                     static_cast<int>(theProfiles.Size()));
  for (size_t aProfileIdx = 0; aProfileIdx < theProfiles.Size(); ++aProfileIdx)
  {
    const GeomGridEval_Curve         anEvaluator(theProfiles.At(aProfileIdx));
    const NCollection_Array1<gp_Pnt> aProfilePoints = anEvaluator.EvaluateGrid(aUParams);
    for (size_t aUIdx = 0; aUIdx < theNbUSamples; ++aUIdx)
    {
      aPoints.ChangeAt(aUIdx, aProfileIdx) = aProfilePoints.At(aUIdx);
    }
  }
  return aPoints;
}

//=================================================================================================

// Samples the prepared guides as section curves of an approximate guide skin.
NCollection_Array2<gp_Pnt> guideSampleGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const size_t                                              theNbVSamples)
{
  const NCollection_Array1<double> aVParams = unitParameters(theNbVSamples);
  NCollection_Array2<gp_Pnt>       aPoints(1,
                                     static_cast<int>(theGuides.Size()),
                                     1,
                                     static_cast<int>(theNbVSamples));
  for (size_t aGuideIdx = 0; aGuideIdx < theGuides.Size(); ++aGuideIdx)
  {
    const GeomGridEval_Curve         anEvaluator(theGuides.At(aGuideIdx));
    const NCollection_Array1<gp_Pnt> aGuidePoints = anEvaluator.EvaluateGrid(aVParams);
    for (size_t aVIdx = 0; aVIdx < theNbVSamples; ++aVIdx)
    {
      aPoints.ChangeAt(aGuideIdx, aVIdx) = aGuidePoints.At(aVIdx);
    }
  }
  return aPoints;
}

//=================================================================================================

// Builds a sampled fallback from approximate profile, guide, and reference surfaces.
occ::handle<Geom_BSplineSurface> approximatePreparedNetwork(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array2<gp_Pnt>&                         theIntersections,
  const double                                              theTolerance)
{
  const size_t aNbUSamples = fallbackSampleCount(theProfiles);
  const size_t aNbVSamples = fallbackSampleCount(theGuides);

  occ::handle<Geom_BSplineSurface> aProfileSurface =
    approximateGrid(profileSampleGrid(theProfiles, aNbUSamples), theTolerance);
  occ::handle<Geom_BSplineSurface> aGuideSurface =
    approximateGrid(guideSampleGrid(theGuides, aNbVSamples), theTolerance);
  occ::handle<Geom_BSplineSurface> aReferenceSurface =
    approximateGrid(theIntersections, theTolerance);
  if (aProfileSurface.IsNull() || aGuideSurface.IsNull() || aReferenceSurface.IsNull())
  {
    return nullptr;
  }
  NCollection_Array2<gp_Pnt>       aPoints(1,
                                     static_cast<int>(aNbUSamples),
                                     1,
                                     static_cast<int>(aNbVSamples));
  const NCollection_Array1<double> aUParams = unitParameters(aNbUSamples);
  const NCollection_Array1<double> aVParams = unitParameters(aNbVSamples);
  const GeomGridEval_Surface       aProfileEvaluator(aProfileSurface);
  const GeomGridEval_Surface       aGuideEvaluator(aGuideSurface);
  const GeomGridEval_Surface       aReferenceEvaluator(aReferenceSurface);
  const NCollection_Array2<gp_Pnt> aProfilePoints =
    aProfileEvaluator.EvaluateGrid(aUParams, aVParams);
  const NCollection_Array2<gp_Pnt> aGuidePoints = aGuideEvaluator.EvaluateGrid(aUParams, aVParams);
  const NCollection_Array2<gp_Pnt> aReferencePoints =
    aReferenceEvaluator.EvaluateGrid(aUParams, aVParams);
  for (size_t aUIdx = 0; aUIdx < aNbUSamples; ++aUIdx)
  {
    for (size_t aVIdx = 0; aVIdx < aNbVSamples; ++aVIdx)
    {
      const gp_XYZ aPoint = aProfilePoints.At(aUIdx, aVIdx).XYZ()
                            + aGuidePoints.At(aUIdx, aVIdx).XYZ()
                            - aReferencePoints.At(aUIdx, aVIdx).XYZ();
      aPoints.ChangeAt(aUIdx, aVIdx) = gp_Pnt(aPoint);
    }
  }
  return approximateGrid(aPoints, theTolerance);
}

//=================================================================================================

// Resets all report fields before starting a construction attempt.
void resetReport(GeomFill_Gordon::BuildReport& theReport)
{
  theReport = GeomFill_Gordon::BuildReport();
}

//=================================================================================================

// Records the result status and its failing construction stage.
void setStatus(GeomFill_Gordon::BuildReport&       theReport,
               const GeomFill_Gordon::ResultStatus theNewStatus,
               const GeomFill_Gordon::BuildStage   theStage)
{
  theReport.Status      = theNewStatus;
  theReport.FailedStage = theNewStatus == GeomFill_Gordon::ResultStatus::Done
                            ? GeomFill_Gordon::BuildStage::NotStarted
                            : theStage;
}

//=================================================================================================

// Returns whether every profile closes geometrically at its parameter endpoints.
bool areClosedCurves(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves,
                     const double                                              theTolerance)
{
  if (theCurves.IsEmpty())
  {
    return false;
  }
  const double aTolerance = std::max(theTolerance, Precision::Confusion());
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    const occ::handle<Geom_BSplineCurve>& aCurve = theCurves.At(aCurveIdx);
    if (aCurve.IsNull()
        || aCurve->Value(aCurve->FirstParameter()).Distance(aCurve->Value(aCurve->LastParameter()))
             > aTolerance)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

// Clones input curves because network preparation modifies its working copies.
bool copyWorkingCurves(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theInput,
                       NCollection_Array1<occ::handle<Geom_BSplineCurve>>&       theWorking)
{
  try
  {
    OCC_CATCH_SIGNALS

    for (size_t aCurveIdx = 0; aCurveIdx < theInput.Size(); ++aCurveIdx)
    {
      if (theInput.At(aCurveIdx).IsNull())
      {
        return false;
      }
      theWorking.ChangeAt(aCurveIdx) =
        occ::down_cast<Geom_BSplineCurve>(theInput.At(aCurveIdx)->Copy());
      if (theWorking.At(aCurveIdx).IsNull())
      {
        return false;
      }
    }
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
  return true;
}

//=================================================================================================

// Returns the first knot in the requested surface parameter direction.
double surfaceStartParameter(const occ::handle<Geom_BSplineSurface>& theSurface, const bool theIsU)
{
  return theIsU ? theSurface->UKnot(1) : theSurface->VKnot(1);
}

//=================================================================================================

// Returns the last knot in the requested surface parameter direction.
double surfaceEndParameter(const occ::handle<Geom_BSplineSurface>& theSurface, const bool theIsU)
{
  return theIsU ? theSurface->UKnot(theSurface->NbUKnots())
                : theSurface->VKnot(theSurface->NbVKnots());
}

//=================================================================================================

// Maps a unit parameter to the knot range of the requested surface direction.
double surfaceParameter(const occ::handle<Geom_BSplineSurface>& theSurface,
                        const bool                              theIsU,
                        const double                            theUnitParameter)
{
  const double aFirst = surfaceStartParameter(theSurface, theIsU);
  return aFirst + theUnitParameter * (surfaceEndParameter(theSurface, theIsU) - aFirst);
}

//=================================================================================================

// Measures the maximum deviation of a curve from its corresponding surface isoparameter.
double maxCurveDeviation(const occ::handle<Geom_BSplineSurface>& theSurface,
                         const occ::handle<Geom_BSplineCurve>&   theCurve,
                         const double                            theFixedParameter,
                         const bool                              theIsProfile)
{
  const GeomAdaptor_Curve anAdaptor(theCurve);
  double                  aMaxDeviation = 0.0;
  const auto              evaluate      = [&](const double theParameter) {
    const double aU = theIsProfile ? surfaceParameter(theSurface, true, theParameter)
                                                     : surfaceParameter(theSurface, true, theFixedParameter);
    const double aV = theIsProfile ? surfaceParameter(theSurface, false, theFixedParameter)
                                                     : surfaceParameter(theSurface, false, theParameter);
    aMaxDeviation =
      std::max(aMaxDeviation, anAdaptor.EvalD0(theParameter).Distance(theSurface->Value(aU, aV)));
  };

  const size_t aSamplesPerSpan =
    static_cast<size_t>(theCurve->Degree())
    + static_cast<size_t>(theIsProfile ? theSurface->UDegree() : theSurface->VDegree());
  for (int aKnotIdx = 1; aKnotIdx < theCurve->NbKnots(); ++aKnotIdx)
  {
    const double aFirst = theCurve->Knot(aKnotIdx);
    const double aLast  = theCurve->Knot(aKnotIdx + 1);
    if (aLast - aFirst <= Precision::PConfusion())
    {
      continue;
    }

    // The sampled difference combines the curve and surface polynomial orders.
    for (size_t aSampleIdx = 0; aSampleIdx <= aSamplesPerSpan; ++aSampleIdx)
    {
      const double aRatio = static_cast<double>(aSampleIdx) / aSamplesPerSpan;
      evaluate(aFirst + aRatio * (aLast - aFirst));
    }
  }
  return aMaxDeviation;
}

//=================================================================================================

// Returns the largest profile-to-surface deviation over all profile isoparameters.
double maxProfileDeviation(const occ::handle<Geom_BSplineSurface>&                   theSurface,
                           const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                           const math_Vector& theProfileParams)
{
  double aMaxDeviation = 0.0;
  for (size_t aProfileIdx = 0; aProfileIdx < theProfiles.Size(); ++aProfileIdx)
  {
    const double aV =
      surfaceParameter(theSurface, false, theProfileParams(static_cast<int>(aProfileIdx) + 1));
    aMaxDeviation =
      std::max(aMaxDeviation, maxCurveDeviation(theSurface, theProfiles.At(aProfileIdx), aV, true));
  }
  return aMaxDeviation;
}

//=================================================================================================

// Returns the largest guide-to-surface deviation over all guide isoparameters.
double maxGuideDeviation(const occ::handle<Geom_BSplineSurface>&                   theSurface,
                         const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
                         const math_Vector&                                        theGuideParams)
{
  double aMaxDeviation = 0.0;
  for (size_t aGuideIdx = 0; aGuideIdx < theGuides.Size(); ++aGuideIdx)
  {
    const double aU =
      surfaceParameter(theSurface, true, theGuideParams(static_cast<int>(aGuideIdx) + 1));
    aMaxDeviation =
      std::max(aMaxDeviation, maxCurveDeviation(theSurface, theGuides.At(aGuideIdx), aU, false));
  }
  return aMaxDeviation;
}

} // namespace

//=================================================================================================

void GeomFill_Gordon::Perform()
{
  resetReport(myReport);
  mySurface.Nullify();
  myIsUClosed = false;
  myIsVClosed = false;

  if (myInputProfiles.Size() < 2 || myInputGuides.Size() < 2)
  {
    setStatus(myReport, ResultStatus::InvalidInput, BuildStage::InputConversion);
    return;
  }

  myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, myInputProfiles.Length());
  myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, myInputGuides.Length());
  if (!copyWorkingCurves(myInputProfiles, myProfiles)
      || !copyWorkingCurves(myInputGuides, myGuides))
  {
    setStatus(myReport, ResultStatus::ConversionFailed, BuildStage::InputConversion);
    return;
  }

  myProfileParams =
    NCollection_Array2<double>(1, myInputProfiles.Length(), 1, myInputGuides.Length());
  myGuideParams =
    NCollection_Array2<double>(1, myInputProfiles.Length(), 1, myInputGuides.Length());

  GordonUtilities::NetworkPreparation aNetwork(myProfiles,
                                               myGuides,
                                               myProfileParams,
                                               myGuideParams,
                                               myTolerance,
                                               myToUseParallel);

  if (!aNetwork.ConvertCurvesToUnitInterval())
  {
    setStatus(myReport, ResultStatus::ConversionFailed, BuildStage::InputConversion);
    return;
  }

  if (!aNetwork.FillIntersectionParameters())
  {
    setStatus(myReport, ResultStatus::IntersectionFailed, BuildStage::ContactDiscovery);
    return;
  }
  myReport.MaxContactGap = aNetwork.MaxContactGap();

  if (!aNetwork.ReorderNetwork())
  {
    setStatus(myReport, ResultStatus::OrderingFailed, BuildStage::NetworkOrdering);
    return;
  }
  if (!aNetwork.NormalizeIntersectionParameters(myIsUClosed, myIsVClosed))
  {
    setStatus(myReport, ResultStatus::CompatibilityFailed, BuildStage::NetworkOrdering);
    return;
  }
  myIsUClosed                           = myIsUClosed || areClosedCurves(myProfiles, myTolerance);
  bool aHasApproximateReparametrization = false;
  if (!aNetwork.EqualizeIntersectionParameters(myApproximationMode
                                                 == ApproximationMode::AllowApproximateFallback,
                                               aHasApproximateReparametrization))
  {
    setStatus(myReport,
              hasRationalCurve(myProfiles, myGuides) ? ResultStatus::RationalReparametrizationFailed
                                                     : ResultStatus::ReparametrizationFailed,
              BuildStage::Reparametrization);
    return;
  }
  myReport.MaxReparametrizationDeviation = aNetwork.MaxReparametrizationDeviation();

  if (!aNetwork.NormalizeIntersectionParameters(myIsUClosed, myIsVClosed))
  {
    setStatus(myReport, ResultStatus::CompatibilityFailed, BuildStage::Reparametrization);
    return;
  }
  myIsUClosed = myIsUClosed || areClosedCurves(myProfiles, myTolerance);

  math_Matrix aProfileParamMatrix = GordonUtilities::toMatrix(myProfileParams);
  math_Matrix aGuideParamMatrix   = GordonUtilities::toMatrix(myGuideParams);

  math_Vector aGuideParamValues   = GordonUtilities::columnMeans(aProfileParamMatrix);
  math_Vector aProfileParamValues = GordonUtilities::rowMeans(aGuideParamMatrix);

  GeomFill_NetworkSurface aNetworkSurface;
  aNetworkSurface.Init(myProfiles,
                       myGuides,
                       aProfileParamValues.Array1(),
                       aGuideParamValues.Array1(),
                       aNetwork.IntersectionPoints(),
                       aNetwork.IntersectionWeights(),
                       myTolerance,
                       myIsUClosed,
                       myIsVClosed);
  aNetworkSurface.Perform();

  if (!aNetworkSurface.IsDone())
  {
    const ResultStatus anExactStatus = mapNetworkStatus(aNetworkSurface.Status());
    if (myApproximationMode == ApproximationMode::AllowApproximateFallback
        && canApproximateAfterExactFailure(anExactStatus))
    {
      mySurface = approximatePreparedNetwork(myProfiles,
                                             myGuides,
                                             aNetwork.IntersectionPoints(),
                                             myTolerance);
      if (!mySurface.IsNull())
      {
        myReport.IsApproximate = true;
        myReport.MaxApproximationDeviation =
          std::max(maxProfileDeviation(mySurface, myProfiles, aProfileParamValues),
                   maxGuideDeviation(mySurface, myGuides, aGuideParamValues));
        setStatus(myReport, ResultStatus::Done, BuildStage::Approximation);
        return;
      }
      setStatus(myReport, ResultStatus::ApproximationFailed, BuildStage::Approximation);
      return;
    }
    setStatus(myReport, anExactStatus, BuildStage::ExactConstruction);
    return;
  }

  mySurface                    = aNetworkSurface.Surface();
  myReport.MaxProfileDeviation = maxProfileDeviation(mySurface, myProfiles, aProfileParamValues);
  myReport.MaxGuideDeviation   = maxGuideDeviation(mySurface, myGuides, aGuideParamValues);
  myReport.IsApproximate       = aHasApproximateReparametrization;
  myReport.MaxApproximationDeviation =
    myReport.IsApproximate ? std::max(myReport.MaxProfileDeviation, myReport.MaxGuideDeviation)
                           : 0.0;
  setStatus(myReport, ResultStatus::Done, BuildStage::Validation);
}

//=================================================================================================

GeomFill_Gordon::GeomFill_Gordon() = default;

//=================================================================================================

void GeomFill_Gordon::Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                           const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                           double                                             theTolerance)
{
  myTolerance = theTolerance;
  myIsUClosed = false;
  myIsVClosed = false;
  resetReport(myReport);
  mySurface.Nullify();

  const size_t aNbProf = theProfiles.Size();
  const size_t aNbGuid = theGuides.Size();

  if (aNbProf < 2 || aNbGuid < 2)
  {
    // Store counts but don't try to convert - Perform() will check and return.
    myInputProfiles =
      NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbProf));
    myInputGuides =
      NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbGuid));
    return;
  }

  // Convert input arrays to immutable B-spline seeds. Perform() deep-copies these
  // seeds before applying destructive preparation steps.
  myInputProfiles =
    NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbProf));
  myInputGuides = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbGuid));

  for (size_t aProfileIdx = 0; aProfileIdx < aNbProf; ++aProfileIdx)
  {
    myInputProfiles.ChangeAt(aProfileIdx) =
      GeomConvert::CurveToBSplineCurve(theProfiles.At(aProfileIdx));
  }
  for (size_t aGuideIdx = 0; aGuideIdx < aNbGuid; ++aGuideIdx)
  {
    myInputGuides.ChangeAt(aGuideIdx) = GeomConvert::CurveToBSplineCurve(theGuides.At(aGuideIdx));
  }

  // Initialize parameter matrices.
  myProfileParams =
    NCollection_Array2<double>(1, static_cast<int>(aNbProf), 1, static_cast<int>(aNbGuid));
  myGuideParams =
    NCollection_Array2<double>(1, static_cast<int>(aNbProf), 1, static_cast<int>(aNbGuid));
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_Gordon::Surface() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone("GeomFill_Gordon::Surface");
  }
  return mySurface;
}
