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

#include <ExtremaPC2d_BezierCurve.hxx>

//==================================================================================================

ExtremaPC2d_BezierCurve::ExtremaPC2d_BezierCurve(const occ::handle<Geom2d_BezierCurve>& theCurve)
    : myCurve(theCurve),
      myDomain{0.0, 1.0},
      myNbSamples(ExtremaPC2d::THE_BEZIER_MIN_SAMPLES)
{
  if (myCurve.IsNull())
  {
    return;
  }
  myDomain.Min = myCurve->FirstParameter();
  myDomain.Max = myCurve->LastParameter();
  myNbSamples = std::max(ExtremaPC2d::THE_BEZIER_MIN_SAMPLES,
                         ExtremaPC2d::THE_BEZIER_DEGREE_MULTIPLIER
                           * static_cast<size_t>(myCurve->Degree() + 1));
}

//==================================================================================================

ExtremaPC2d_BezierCurve::ExtremaPC2d_BezierCurve(const occ::handle<Geom2d_BezierCurve>& theCurve,
                                                 const ExtremaPC2d::Domain1D&           theDomain)
    : myCurve(theCurve),
      myDomain(theDomain),
      myNbSamples(ExtremaPC2d::THE_BEZIER_MIN_SAMPLES)
{
  if (myCurve.IsNull())
  {
    return;
  }
  myNbSamples = std::max(ExtremaPC2d::THE_BEZIER_MIN_SAMPLES,
                         ExtremaPC2d::THE_BEZIER_DEGREE_MULTIPLIER
                           * static_cast<size_t>(myCurve->Degree() + 1));
}

//==================================================================================================

void ExtremaPC2d_BezierCurve::buildParams() const
{
  if (myCurve.IsNull() || !myDomain.IsValid() || !myDomain.IsFinite())
  {
    return;
  }

  myNbSamples = std::max(ExtremaPC2d::THE_BEZIER_MIN_SAMPLES,
                         ExtremaPC2d::THE_BEZIER_DEGREE_MULTIPLIER
                           * static_cast<size_t>(myCurve->Degree() + 1));

  math_Vector aParams =
    ExtremaPC2d_GridEvaluator::BuildUniformParams(myDomain.Min, myDomain.Max, myNbSamples);

  myEvaluator.SetParams(aParams);
}

//==================================================================================================

gp_Pnt2d ExtremaPC2d_BezierCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_BezierCurve::Perform(const gp_Pnt2d&         theP,
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

const ExtremaPC2d::Result& ExtremaPC2d_BezierCurve::PerformWithEndpoints(
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
