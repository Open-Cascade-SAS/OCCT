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

#include <ExtremaPC2d_BSplineCurve.hxx>

#include <math_Vector.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Integer.hxx>
#include <Standard_RangeError.hxx>

//==================================================================================================

ExtremaPC2d_BSplineCurve::ExtremaPC2d_BSplineCurve(const occ::handle<Geom2d_BSplineCurve>& theCurve)
    : myCurve(theCurve),
      myDomain{0.0, 1.0}
{
  if (myCurve.IsNull())
  {
    return;
  }
  myDomain.Min = myCurve->FirstParameter();
  myDomain.Max = myCurve->LastParameter();
}

//==================================================================================================

ExtremaPC2d_BSplineCurve::ExtremaPC2d_BSplineCurve(const occ::handle<Geom2d_BSplineCurve>& theCurve,
                                                   const ExtremaPC2d::Domain1D& theDomain)
    : myCurve(theCurve),
      myDomain(theDomain)
{
  if (myCurve.IsNull())
  {
    return;
  }
}

//==================================================================================================

math_Vector ExtremaPC2d_BSplineCurve::buildKnotAwareParams() const
{
  if (myCurve.IsNull())
  {
    // Fallback to uniform sampling
    return ExtremaPC2d_GridEvaluator::BuildUniformParams(myDomain.Min,
                                                         myDomain.Max,
                                                         ExtremaPC2d::THE_BSPLINE_FALLBACK_SAMPLES);
  }

  const double theUMin = myDomain.Min;
  const double theUMax = myDomain.Max;

  const int                         aDegree = myCurve->Degree();
  const NCollection_Array1<double>& aKnots  = myCurve->Knots();

  // Use multiplier*(degree+1) samples per span for accurate extrema detection.
  const size_t aSamplesPerSpan =
    ExtremaPC2d::THE_BSPLINE_SPAN_MULTIPLIER * static_cast<size_t>(aDegree + 1);

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
                               "ExtremaPC2d_BSplineCurve: parameter grid is too large");
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

void ExtremaPC2d_BSplineCurve::buildParams() const
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

gp_Pnt2d ExtremaPC2d_BSplineCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_BSplineCurve::Perform(const gp_Pnt2d&         theP,
                                                             double                  theTol,
                                                             ExtremaPC2d::SearchMode theMode) const
{
  if (myCurve.IsNull())
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC2d::Status::NotDone;
    return myEvaluator.Result();
  }

  buildParams();
  Geom2dAdaptor_Curve anAdaptor(myCurve);
  return myEvaluator.Perform(anAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_BSplineCurve::PerformWithEndpoints(
  const gp_Pnt2d&         theP,
  double                  theTol,
  ExtremaPC2d::SearchMode theMode) const
{
  (void)Perform(theP, theTol, theMode);

  // Add endpoints to the result
  ExtremaPC2d::Result& aResult = myEvaluator.Result();
  if (aResult.Status == ExtremaPC2d::Status::OK
      || aResult.Status == ExtremaPC2d::Status::NoSolution)
  {
    Geom2dAdaptor_Curve anAdaptor(myCurve);
    const bool          isClosed = ExtremaPC2d_GridEvaluator::IsClosedDomain(anAdaptor, myDomain);
    ExtremaPC2d::AddEndpointExtrema(aResult, theP, myDomain, *this, theMode, isClosed);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC2d::Status::OK;
    }
  }

  return aResult;
}
