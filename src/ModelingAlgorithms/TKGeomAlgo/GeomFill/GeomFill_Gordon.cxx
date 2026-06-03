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
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_NetworkSurface.hxx>
#include <GeomFill_Profiler.hxx>
#include <GeomLib_Interpolate.hxx>
#include <GeomLib_Tool.hxx>
#include <Law_BSpline.hxx>
#include <Law_Interpolate.hxx>
#include <NCollection_HArray1.hxx>
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

//=================================================================================================

void GeomFill_Gordon::Perform()
{
  myStatus = ResultStatus::NotStarted;
  mySurface.Nullify();

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

  aNetwork.SnapIntersectionParameters();

  aNetwork.DetectClosedDirections(myIsUClosed, myIsVClosed);
  aNetwork.AdjustClosedBoundaries(myIsUClosed, myIsVClosed);

  if (!aNetwork.EqualizeIntersectionParameters())
  {
    myStatus = ResultStatus::ReparametrizationFailed;
    return;
  }

  if (!aNetwork.CheckIntersectionTable())
  {
    myStatus = ResultStatus::CompatibilityFailed;
    return;
  }

  GeomFill_Profiler aProfileProfiler;
  for (size_t aProfileIdx = 0; aProfileIdx < myProfiles.Size(); ++aProfileIdx)
  {
    aProfileProfiler.AddCurve(myProfiles.At(aProfileIdx));
  }
  aProfileProfiler.Perform(Precision::PConfusion());

  for (size_t aProfileIdx = 0; aProfileIdx < myProfiles.Size(); ++aProfileIdx)
  {
    myProfiles.ChangeAt(aProfileIdx) =
      occ::down_cast<Geom_BSplineCurve>(aProfileProfiler.Curve(static_cast<int>(aProfileIdx) + 1));
  }

  GeomFill_Profiler aGuideProfiler;
  for (size_t aGuideIdx = 0; aGuideIdx < myGuides.Size(); ++aGuideIdx)
  {
    aGuideProfiler.AddCurve(myGuides.At(aGuideIdx));
  }
  aGuideProfiler.Perform(Precision::PConfusion());

  for (size_t aGuideIdx = 0; aGuideIdx < myGuides.Size(); ++aGuideIdx)
  {
    myGuides.ChangeAt(aGuideIdx) =
      occ::down_cast<Geom_BSplineCurve>(aGuideProfiler.Curve(static_cast<int>(aGuideIdx) + 1));
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
                       myTolerance,
                       myIsUClosed,
                       myIsVClosed);
  aNetworkSurface.Perform();

  if (!aNetworkSurface.IsDone())
  {
    myStatus = ResultStatus::ConstructionFailed;
    return;
  }

  mySurface = aNetworkSurface.Surface();
  myStatus  = ResultStatus::Done;
}

//=================================================================================================

GeomFill_Gordon::GeomFill_Gordon() = default;

//=================================================================================================

void GeomFill_Gordon::Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                           const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                           double                                             theTolerance)
{
  myTolerance = theTolerance;
  myStatus    = ResultStatus::NotStarted;
  myIsUClosed = false;
  myIsVClosed = false;
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
