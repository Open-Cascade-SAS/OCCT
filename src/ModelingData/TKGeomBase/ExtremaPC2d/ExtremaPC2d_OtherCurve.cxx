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

#include <ExtremaPC2d_OtherCurve.hxx>

#include <ExtremaPC2d_GridEvaluator.hxx>

//==================================================================================================

ExtremaPC2d_OtherCurve::ExtremaPC2d_OtherCurve(const Adaptor2d_Curve2d& theCurve)
    : myCurve(&theCurve),
      myDomain{theCurve.FirstParameter(), theCurve.LastParameter()}
{
}

//==================================================================================================

ExtremaPC2d_OtherCurve::ExtremaPC2d_OtherCurve(const Adaptor2d_Curve2d&     theCurve,
                                               const ExtremaPC2d::Domain1D& theDomain)
    : myCurve(&theCurve),
      myDomain(theDomain)
{
}

//==================================================================================================

void ExtremaPC2d_OtherCurve::buildParams() const
{
  if (myCurve == nullptr || !myDomain.IsValid() || !myDomain.IsFinite())
  {
    return;
  }

  math_Vector aParams = ExtremaPC2d_GridEvaluator::BuildCurveAwareParams(*myCurve, myDomain);

  myEvaluator.SetParams(aParams);
}

//==================================================================================================

gp_Pnt2d ExtremaPC2d_OtherCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_OtherCurve::Perform(const gp_Pnt2d&         theP,
                                                           double                  theTol,
                                                           ExtremaPC2d::SearchMode theMode) const
{
  if (myCurve == nullptr)
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC2d::Status::NotDone;
    return myEvaluator.Result();
  }

  buildParams();
  return myEvaluator.Perform(*myCurve, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_OtherCurve::PerformWithEndpoints(
  const gp_Pnt2d&         theP,
  double                  theTol,
  ExtremaPC2d::SearchMode theMode) const
{
  if (myCurve == nullptr)
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC2d::Status::NotDone;
    return myEvaluator.Result();
  }

  // Refresh the curve-aware partition and compute interior extrema.
  (void)Perform(theP, theTol, theMode);

  // Add endpoints to the result
  ExtremaPC2d::Result& aResult = myEvaluator.Result();
  if (aResult.Status == ExtremaPC2d::Status::OK
      || aResult.Status == ExtremaPC2d::Status::NoSolution)
  {
    const bool isClosed = ExtremaPC2d_GridEvaluator::IsClosedDomain(*myCurve, myDomain);
    ExtremaPC2d::AddEndpointExtrema(aResult, theP, myDomain, *this, theMode, isClosed);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC2d::Status::OK;
    }
  }

  return aResult;
}
