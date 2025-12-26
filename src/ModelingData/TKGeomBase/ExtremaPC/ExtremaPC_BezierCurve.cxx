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

ExtremaPC_BezierCurve::ExtremaPC_BezierCurve(const Handle(Geom_BezierCurve)& theCurve)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain{theCurve->FirstParameter(), theCurve->LastParameter()},
      myNbSamples(std::max(24, 3 * (theCurve->Degree() + 1)))
{
  buildGrid();
}

//==================================================================================================

ExtremaPC_BezierCurve::ExtremaPC_BezierCurve(const Handle(Geom_BezierCurve)& theCurve,
                                             const ExtremaPC::Domain1D&      theDomain)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain(theDomain),
      myNbSamples(std::max(24, 3 * (theCurve->Degree() + 1)))
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
  myGrid = ExtremaPC_GridEvaluator::BuildGrid(aGridEval, aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_BezierCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_BezierCurve::Perform(const gp_Pnt&         theP,
                                                 double                theTol,
                                                 ExtremaPC::SearchMode theMode) const
{
  ExtremaPC::Result aResult;

  if (myCurve.IsNull())
  {
    aResult.Status = ExtremaPC::Status::NotDone;
    return aResult;
  }

  // Use the pre-built grid (interior extrema only)
  return ExtremaPC_GridEvaluator::PerformWithCachedGrid(myGrid, myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_BezierCurve::PerformWithEndpoints(const gp_Pnt&         theP,
                                                              double                theTol,
                                                              ExtremaPC::SearchMode theMode) const
{
  ExtremaPC::Result aResult = Perform(theP, theTol, theMode);

  // Add endpoints if interior computation succeeded or found no interior solutions
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
