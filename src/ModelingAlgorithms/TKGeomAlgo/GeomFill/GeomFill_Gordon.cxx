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
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_NetworkSurface.hxx>
#include <GeomFill_Profiler.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_LinearVector.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt2d.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <atomic>

#include "GeomFill_GordonUtils.pxx"

namespace GordonUtils = GeomFill_GordonUtils;

//=================================================================================================

void GeomFill_Gordon::Perform()
{
  myIsDone = false;

  if (myProfiles.Size() < 2 || myGuides.Size() < 2)
  {
    return;
  }

  if (!GordonUtils::normalizeCurveDomains(myProfiles, myGuides))
  {
    return;
  }

  if (!GordonUtils::locateIntersections(myProfiles,
                                        myGuides,
                                        myProfileParams,
                                        myGuideParams,
                                        myTolerance,
                                        myToUseParallel))
  {
    return;
  }

  if (!GordonUtils::arrangeNetwork(myProfiles, myGuides, myProfileParams, myGuideParams))
  {
    return;
  }

  GordonUtils::snapBoundaryParameters(myProfiles,
                                      myGuides,
                                      myProfileParams,
                                      myGuideParams,
                                      myTolerance);

  GordonUtils::markClosedSeams(myProfiles,
                               myGuides,
                               myProfileParams,
                               myGuideParams,
                               myTolerance,
                               myIsUClosed,
                               myIsVClosed);

  if (myIsUClosed)
  {
    const size_t aLastGuideIdx = myGuides.Size() - 1;
    for (size_t aProfileIdx = 0; aProfileIdx < myProfiles.Size(); ++aProfileIdx)
    {
      myProfileParams.ChangeAt(aProfileIdx, 0) = myProfiles.At(aProfileIdx)->FirstParameter();
      myProfileParams.ChangeAt(aProfileIdx, aLastGuideIdx) =
        myProfiles.At(aProfileIdx)->LastParameter();
    }
  }
  if (myIsVClosed)
  {
    const size_t aLastProfileIdx = myProfiles.Size() - 1;
    for (size_t aGuideIdx = 0; aGuideIdx < myGuides.Size(); ++aGuideIdx)
    {
      myGuideParams.ChangeAt(0, aGuideIdx)               = myGuides.At(aGuideIdx)->FirstParameter();
      myGuideParams.ChangeAt(aLastProfileIdx, aGuideIdx) = myGuides.At(aGuideIdx)->LastParameter();
    }
  }

  if (!GordonUtils::redistributeNetworkParameters(myProfiles,
                                                  myGuides,
                                                  myProfileParams,
                                                  myGuideParams,
                                                  myTolerance))
  {
    return;
  }

  if (!GordonUtils::isIntersectionGridConsistent(myProfiles,
                                                 myGuides,
                                                 myProfileParams,
                                                 myGuideParams,
                                                 myTolerance))
  {
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

  const size_t aNbProf = myProfiles.Size();
  const size_t aNbGuid = myGuides.Size();

  NCollection_Array1<double> aGuideParamValues(1, static_cast<int>(aNbGuid));
  for (size_t aGuideIdx = 0; aGuideIdx < aGuideParamValues.Size(); ++aGuideIdx)
  {
    double aSum = 0.0;
    for (size_t aProfileIdx = 0; aProfileIdx < aNbProf; ++aProfileIdx)
    {
      aSum += myProfileParams.At(aProfileIdx, aGuideIdx);
    }
    aGuideParamValues.ChangeAt(aGuideIdx) = aSum / static_cast<double>(aNbProf);
  }

  NCollection_Array1<double> aProfileParamValues(1, static_cast<int>(aNbProf));
  for (size_t aProfileIdx = 0; aProfileIdx < aProfileParamValues.Size(); ++aProfileIdx)
  {
    double aSum = 0.0;
    for (size_t aGuideIdx = 0; aGuideIdx < aNbGuid; ++aGuideIdx)
    {
      aSum += myGuideParams.At(aProfileIdx, aGuideIdx);
    }
    aProfileParamValues.ChangeAt(aProfileIdx) = aSum / static_cast<double>(aNbGuid);
  }

  GeomFill_NetworkSurface aNetworkSurface;
  aNetworkSurface.Init(myProfiles,
                       myGuides,
                       aProfileParamValues,
                       aGuideParamValues,
                       myTolerance,
                       myIsUClosed,
                       myIsVClosed);
  aNetworkSurface.Perform();

  if (!aNetworkSurface.IsDone())
  {
    return;
  }

  mySurface = aNetworkSurface.Surface();
  myIsDone  = true;
}

//=================================================================================================

GeomFill_Gordon::GeomFill_Gordon() = default;

//=================================================================================================

void GeomFill_Gordon::Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                           const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                           double                                             theTolerance)
{
  myTolerance = theTolerance;
  myIsDone    = false;
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
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Gordon::Surface");
  }
  return mySurface;
}
