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
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_NetworkSurface.hxx>
#include <GeomLib_Interpolate.hxx>
#include <GeomLib_Tool.hxx>
#include <NCollection_LinearVector.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <atomic>

#include "GeomFill_GordonUtilities.pxx"

namespace
{
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

bool hasRationalCurve(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                      const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides)
{
  return hasRationalCurve(theProfiles) || hasRationalCurve(theGuides);
}

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

//! Returns true when a sampled fallback can still use a validated prepared network.
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

//! Chooses a deterministic fallback grid density from the prepared network itself.
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

//! Fits a non-exact B-spline surface through a point grid for approximate fallback.
occ::handle<Geom_BSplineSurface> approximateGrid(const NCollection_Array2<gp_Pnt>& thePoints,
                                                 const double                      theTolerance)
{
  GeomAPI_PointsToBSplineSurface anApprox;
  anApprox.Init(thePoints, 1, 5, GeomAbs_C1, std::max(theTolerance, Precision::Confusion()));
  return anApprox.IsDone() ? anApprox.Surface() : nullptr;
}

//! Samples the prepared profiles as section curves of an approximate profile skin.
NCollection_Array2<gp_Pnt> profileSampleGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const size_t                                              theNbUSamples)
{
  NCollection_Array2<gp_Pnt> aPoints(1,
                                     static_cast<int>(theNbUSamples),
                                     1,
                                     static_cast<int>(theProfiles.Size()));
  for (size_t aUIdx = 0; aUIdx < theNbUSamples; ++aUIdx)
  {
    const double aU = static_cast<double>(aUIdx) / static_cast<double>(theNbUSamples - 1);
    for (size_t aProfileIdx = 0; aProfileIdx < theProfiles.Size(); ++aProfileIdx)
    {
      aPoints.ChangeAt(aUIdx, aProfileIdx) = theProfiles.At(aProfileIdx)->Value(aU);
    }
  }
  return aPoints;
}

//! Samples the prepared guides as section curves of an approximate guide skin.
NCollection_Array2<gp_Pnt> guideSampleGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const size_t                                              theNbVSamples)
{
  NCollection_Array2<gp_Pnt> aPoints(1,
                                     static_cast<int>(theGuides.Size()),
                                     1,
                                     static_cast<int>(theNbVSamples));
  for (size_t aGuideIdx = 0; aGuideIdx < theGuides.Size(); ++aGuideIdx)
  {
    for (size_t aVIdx = 0; aVIdx < theNbVSamples; ++aVIdx)
    {
      const double aV = static_cast<double>(aVIdx) / static_cast<double>(theNbVSamples - 1);
      aPoints.ChangeAt(aGuideIdx, aVIdx) = theGuides.At(aGuideIdx)->Value(aV);
    }
  }
  return aPoints;
}

//! Builds a sampled fallback from approximate profile, guide, and reference surfaces.
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

  NCollection_Array2<gp_Pnt> aPoints(1,
                                     static_cast<int>(aNbUSamples),
                                     1,
                                     static_cast<int>(aNbVSamples));
  for (size_t aUIdx = 0; aUIdx < aNbUSamples; ++aUIdx)
  {
    const double aU = static_cast<double>(aUIdx) / static_cast<double>(aNbUSamples - 1);
    for (size_t aVIdx = 0; aVIdx < aNbVSamples; ++aVIdx)
    {
      const double aV     = static_cast<double>(aVIdx) / static_cast<double>(aNbVSamples - 1);
      const gp_XYZ aPoint = aProfileSurface->Value(aU, aV).XYZ()
                            + aGuideSurface->Value(aU, aV).XYZ()
                            - aReferenceSurface->Value(aU, aV).XYZ();
      aPoints.ChangeAt(aUIdx, aVIdx) = gp_Pnt(aPoint);
    }
  }
  return approximateGrid(aPoints, theTolerance);
}

} // namespace

//=================================================================================================

void GeomFill_Gordon::Perform()
{
  myStatus = ResultStatus::NotStarted;
  mySurface.Nullify();
  myIsApproximate = false;

  if (myProfiles.Size() < 2 || myGuides.Size() < 2)
  {
    myStatus = ResultStatus::InvalidInput;
    return;
  }

  GordonUtilities::NetworkPreparation aNetwork(myProfiles,
                                               myGuides,
                                               myProfileParams,
                                               myGuideParams,
                                               myTolerance,
                                               myToUseParallel);

  if (!aNetwork.ConvertCurvesToUnitInterval())
  {
    myStatus = ResultStatus::ConversionFailed;
    return;
  }

  if (!aNetwork.FillIntersectionParameters())
  {
    myStatus = ResultStatus::IntersectionFailed;
    return;
  }

  if (!aNetwork.ReorderNetwork())
  {
    myStatus = ResultStatus::OrderingFailed;
    return;
  }

  if (!aNetwork.NormalizeIntersectionParameters(myIsUClosed, myIsVClosed))
  {
    myStatus = ResultStatus::CompatibilityFailed;
    return;
  }

  bool aHasApproximateReparametrization = false;
  if (!aNetwork.EqualizeIntersectionParameters(myApproximationMode
                                                 == ApproximationMode::AllowApproximateFallback,
                                               aHasApproximateReparametrization))
  {
    myStatus = hasRationalCurve(myProfiles, myGuides)
                 ? ResultStatus::RationalReparametrizationFailed
                 : ResultStatus::ReparametrizationFailed;
    return;
  }

  if (!aNetwork.NormalizeIntersectionParameters(myIsUClosed, myIsVClosed))
  {
    myStatus = ResultStatus::CompatibilityFailed;
    return;
  }

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
        myIsApproximate = true;
        myStatus        = ResultStatus::Done;
        return;
      }
      myStatus = ResultStatus::ApproximationFailed;
      return;
    }
    myStatus = anExactStatus;
    return;
  }

  mySurface       = aNetworkSurface.Surface();
  myIsApproximate = aHasApproximateReparametrization;
  myStatus        = ResultStatus::Done;
}

//=================================================================================================

GeomFill_Gordon::GeomFill_Gordon() = default;

//=================================================================================================

void GeomFill_Gordon::Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                           const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                           double                                             theTolerance)
{
  myTolerance     = theTolerance;
  myStatus        = ResultStatus::NotStarted;
  myIsUClosed     = false;
  myIsVClosed     = false;
  myIsApproximate = false;
  mySurface.Nullify();

  const size_t aNbProf = theProfiles.Size();
  const size_t aNbGuid = theGuides.Size();

  if (aNbProf < 2 || aNbGuid < 2)
  {
    // Store counts but don't try to convert - Perform() will check and return.
    myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbProf));
    myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbGuid));
    return;
  }

  // Convert input arrays to BSpline storage arrays.
  myProfiles = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbProf));
  myGuides   = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(1, static_cast<int>(aNbGuid));

  for (size_t aProfileIdx = 0; aProfileIdx < aNbProf; ++aProfileIdx)
  {
    myProfiles.ChangeAt(aProfileIdx) =
      GeomConvert::CurveToBSplineCurve(theProfiles.At(aProfileIdx));
  }
  for (size_t aGuideIdx = 0; aGuideIdx < aNbGuid; ++aGuideIdx)
  {
    myGuides.ChangeAt(aGuideIdx) = GeomConvert::CurveToBSplineCurve(theGuides.At(aGuideIdx));
  }

  // Initialize parameter matrices.
  myProfileParams =
    NCollection_Array2<double>(1, static_cast<int>(aNbProf), 1, static_cast<int>(aNbGuid));
  myGuideParams =
    NCollection_Array2<double>(1, static_cast<int>(aNbProf), 1, static_cast<int>(aNbGuid));
}

const occ::handle<Geom_BSplineSurface>& GeomFill_Gordon::Surface() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone("GeomFill_Gordon::Surface");
  }
  return mySurface;
}
