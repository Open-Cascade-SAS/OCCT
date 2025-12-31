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

#include <ExtremaPC_BezierCurve.hxx>

#include <GeomGridEval_BezierCurve.hxx>

//==================================================================================================

ExtremaPC_BezierCurve::ExtremaPC_BezierCurve(const occ::handle<Geom_BezierCurve>& theCurve)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain{theCurve->FirstParameter(), theCurve->LastParameter()},
      myNbSamples(std::max(ExtremaPC::THE_BEZIER_MIN_SAMPLES,
                           ExtremaPC::THE_BEZIER_DEGREE_MULTIPLIER * (theCurve->Degree() + 1)))
{
  buildGrid();
}

//==================================================================================================

ExtremaPC_BezierCurve::ExtremaPC_BezierCurve(const occ::handle<Geom_BezierCurve>& theCurve,
                                             const ExtremaPC::Domain1D&           theDomain)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain(theDomain),
      myNbSamples(std::max(ExtremaPC::THE_BEZIER_MIN_SAMPLES,
                           ExtremaPC::THE_BEZIER_DEGREE_MULTIPLIER * (theCurve->Degree() + 1)))
{
  buildGrid();
}

//==================================================================================================

void ExtremaPC_BezierCurve::buildGrid()
{
  if (myCurve.IsNull())
  {
    return;
  }

  math_Vector aParams =
    ExtremaPC_GridEvaluator::BuildUniformParams(myDomain.Min, myDomain.Max, myNbSamples);

  GeomGridEval_BezierCurve aGridEval(myCurve);
  myEvaluator.BuildGrid(aGridEval, aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_BezierCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_BezierCurve::Perform(const gp_Pnt&         theP,
                                                        double                theTol,
                                                        ExtremaPC::SearchMode theMode) const
{
  if (myCurve.IsNull())
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }

  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_BezierCurve::PerformWithEndpoints(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPC::SearchMode theMode) const
{
  // Get interior extrema (populates myEvaluator's result)
  (void)myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);

  // Add endpoints to the result
  ExtremaPC::Result& aResult = myEvaluator.Result();
  if (aResult.Status == ExtremaPC::Status::OK || aResult.Status == ExtremaPC::Status::NoSolution)
  {
    ExtremaPC::AddEndpointExtrema(aResult, theP, myDomain, *this, theTol, theMode);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC::Status::OK;
    }
  }

  return aResult;
}
