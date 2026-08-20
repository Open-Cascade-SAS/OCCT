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

#include <ExtremaPC_BSplineCurve.hxx>

#include <ExtremaPC_Planar.hxx>
#include <ExtremaPC2d_BSplineCurve.hxx>
#include <ProjLib.hxx>

#include <math_Vector.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Integer.hxx>
#include <Standard_RangeError.hxx>

#include <cmath>

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve)
    : myCurve(theCurve),
      myDomain{0.0, 1.0}
{
  if (myCurve.IsNull())
  {
    return;
  }
  myAdaptor.Load(myCurve);
  myDomain.Min = myCurve->FirstParameter();
  myDomain.Max = myCurve->LastParameter();
  buildParams();
}

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve,
                                               const ExtremaPC::Domain1D&            theDomain)
    : myCurve(theCurve),
      myDomain(theDomain)
{
  if (myCurve.IsNull())
  {
    return;
  }
  myAdaptor.Load(myCurve);
  buildParams();
}

//==================================================================================================

math_Vector ExtremaPC_BSplineCurve::buildKnotAwareParams() const
{
  if (myCurve.IsNull())
  {
    // Fallback to uniform sampling
    return ExtremaPC_GridEvaluator::BuildUniformParams(myDomain.Min,
                                                       myDomain.Max,
                                                       ExtremaPC::THE_BSPLINE_FALLBACK_SAMPLES);
  }

  const double theUMin = myDomain.Min;
  const double theUMax = myDomain.Max;

  const int                         aDegree = myCurve->Degree();
  const NCollection_Array1<double>& aKnots  = myCurve->Knots();

  // Use multiplier*(degree+1) samples per span for accurate extrema detection.
  const size_t aSamplesPerSpan =
    ExtremaPC::THE_BSPLINE_SPAN_MULTIPLIER * static_cast<size_t>(aDegree + 1);

  const bool   isPeriodic      = myCurve->IsPeriodic();
  const double aPeriod         = isPeriodic ? myCurve->Period() : 0.0;
  const double aFirstShiftReal = isPeriodic ? std::floor((theUMin - aKnots.Last()) / aPeriod) : 0.0;
  const double aLastShiftReal  = isPeriodic ? std::ceil((theUMax - aKnots.First()) / aPeriod) : 0.0;
  const double aNbShifts       = aLastShiftReal - aFirstShiftReal + 1.0;
  const double aMaxParams =
    2.0 + aNbShifts * static_cast<double>(aKnots.Size() - 1) * aSamplesPerSpan;
  Standard_RangeError_Raise_if(aFirstShiftReal < IntegerFirst() || aFirstShiftReal > IntegerLast()
                                 || aLastShiftReal < IntegerFirst()
                                 || aLastShiftReal > IntegerLast() || aNbShifts < 1.0
                                 || aMaxParams > IntegerLast(),
                               "ExtremaPC_BSplineCurve: parameter grid is too large");
  const int                           aFirstShift = static_cast<int>(aFirstShiftReal);
  const int                           aLastShift  = static_cast<int>(aLastShiftReal);
  NCollection_LocalArray<double, 128> aParams(static_cast<size_t>(aMaxParams));
  size_t                              aNbParams = 0;
  aParams[aNbParams++]                          = theUMin;
  for (int aShiftIndex = aFirstShift;; ++aShiftIndex)
  {
    const double aShift = aShiftIndex * aPeriod;
    for (size_t aKnotIndex = 0; aKnotIndex + 1 < aKnots.Size(); ++aKnotIndex)
    {
      const double aKnotLo = aKnots.At(aKnotIndex) + aShift;
      const double aKnotHi = aKnots.At(aKnotIndex + 1) + aShift;
      const double aSpanLo = std::max(aKnotLo, theUMin);
      const double aSpanHi = std::min(aKnotHi, theUMax);
      if (aSpanHi <= aSpanLo)
      {
        continue;
      }

      const double aStep = (aSpanHi - aSpanLo) / aSamplesPerSpan;
      for (size_t aSampleIndex = 1; aSampleIndex < aSamplesPerSpan; ++aSampleIndex)
      {
        const double aU = aSpanLo + static_cast<double>(aSampleIndex) * aStep;
        if (aU > theUMin && aU < theUMax)
        {
          aParams[aNbParams++] = aU;
        }
      }
      if (aKnotHi > theUMin && aKnotHi < theUMax)
      {
        aParams[aNbParams++] = aKnotHi;
      }
    }
    if (aShiftIndex == aLastShift)
    {
      break;
    }
  }
  aParams[aNbParams++] = theUMax;

  // Convert to math_Vector for the numerical evaluator.
  math_Vector aResult(aNbParams);
  for (size_t anIndex = 0; anIndex < aNbParams; ++anIndex)
  {
    aResult.ChangeAt(anIndex) = aParams[anIndex];
  }

  return aResult;
}

//==================================================================================================

void ExtremaPC_BSplineCurve::buildParams()
{
  if (myCurve.IsNull() || !myDomain.IsValid() || !myDomain.IsFinite())
  {
    return;
  }

  // Build knot-aware parameter partition.
  math_Vector aParams = buildKnotAwareParams();

  myEvaluator.SetParams(aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_BSplineCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//=================================================================================================

bool ExtremaPC_BSplineCurve::performPlanar(const gp_Pnt&         theP,
                                           double                theTol,
                                           ExtremaPC::SearchMode theMode,
                                           bool                  theIncludeEndpoints) const
{
  gp_Pln                           aPlane;
  occ::handle<Geom2d_BSplineCurve> aCurve2d;
  if (!ExtremaPC::ProjectBSpline(myCurve, aPlane, aCurve2d))
  {
    return false;
  }

  ExtremaPC2d_BSplineCurve      anEvaluator2d(aCurve2d,
                                              ExtremaPC2d::Domain1D(myDomain.Min, myDomain.Max));
  const ExtremaPC2d::SearchMode aMode    = ExtremaPC::ToSearchMode2d(theMode);
  const gp_Pnt2d                aPoint2d = ProjLib::Project(aPlane, theP);
  const ExtremaPC2d::Result&    aResult2d =
    theIncludeEndpoints ? anEvaluator2d.PerformWithEndpoints(aPoint2d, theTol, aMode)
                        : anEvaluator2d.Perform(aPoint2d, theTol, aMode);
  return ExtremaPC::ConvertPlanarResult(aResult2d, theP, aPlane, *this, myEvaluator.Result());
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_BSplineCurve::Perform(const gp_Pnt&         theP,
                                                         double                theTol,
                                                         ExtremaPC::SearchMode theMode) const
{
  if (myCurve.IsNull())
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }

  if (ExtremaPC::IsValidTolerance(theTol) && myDomain.IsValid()
      && performPlanar(theP, theTol, theMode, false))
  {
    return myEvaluator.Result();
  }

  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_BSplineCurve::PerformWithEndpoints(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPC::SearchMode theMode) const
{
  if (myCurve.IsNull())
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }
  if (ExtremaPC::IsValidTolerance(theTol) && myDomain.IsValid()
      && performPlanar(theP, theTol, theMode, true))
  {
    return myEvaluator.Result();
  }

  (void)Perform(theP, theTol, theMode);

  // Add endpoints to the result
  ExtremaPC::Result& aResult = myEvaluator.Result();
  if (aResult.Status == ExtremaPC::Status::OK || aResult.Status == ExtremaPC::Status::NoSolution)
  {
    const bool isClosed = ExtremaPC_GridEvaluator::IsClosedDomain(myAdaptor, myDomain);
    ExtremaPC::AddEndpointExtrema(aResult, theP, myDomain, *this, theMode, isClosed);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC::Status::OK;
    }
  }

  return aResult;
}
