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

#include <ExtremaPC_OffsetCurve.hxx>

#include <ExtremaPC_GridEvaluator.hxx>
#include <GeomGridEval_OtherCurve.hxx>

//==================================================================================================

ExtremaPC_OffsetCurve::ExtremaPC_OffsetCurve(const Adaptor3d_Curve& theCurve)
    : myCurve(&theCurve),
      myDomain{theCurve.FirstParameter(), theCurve.LastParameter()}
{
  buildGrid();
}

//==================================================================================================

ExtremaPC_OffsetCurve::ExtremaPC_OffsetCurve(const Adaptor3d_Curve&     theCurve,
                                             const ExtremaPC::Domain1D& theDomain)
    : myCurve(&theCurve),
      myDomain(theDomain)
{
  buildGrid();
}

//==================================================================================================

void ExtremaPC_OffsetCurve::buildGrid()
{
  if (myCurve == nullptr)
  {
    return;
  }

  // Use higher number of samples for offset curves (they can be complex)
  constexpr int aNbSamples = 64;

  math_Vector aParams =
    ExtremaPC_GridEvaluator::BuildUniformParams(myDomain.Min, myDomain.Max, aNbSamples);

  GeomGridEval_OtherCurve aGridEval(*myCurve);
  myEvaluator.BuildGrid(aGridEval, aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_OffsetCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_OffsetCurve::Perform(const gp_Pnt&         theP,
                                                        double                theTol,
                                                        ExtremaPC::SearchMode theMode) const
{
  if (myCurve == nullptr)
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }

  return myEvaluator.Perform(*myCurve, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_OffsetCurve::PerformWithEndpoints(const gp_Pnt&         theP,
                                                                      double                theTol,
                                                                      ExtremaPC::SearchMode theMode) const
{
  // Get interior extrema (populates myEvaluator's result)
  (void)myEvaluator.Perform(*myCurve, theP, myDomain, theTol, theMode);

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
