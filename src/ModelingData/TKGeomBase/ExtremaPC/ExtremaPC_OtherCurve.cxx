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

#include <ExtremaPC_OtherCurve.hxx>

#include <ExtremaPC_GridEvaluator.hxx>
#include <GeomGridEval_OtherCurve.hxx>

//==================================================================================================

ExtremaPC_OtherCurve::ExtremaPC_OtherCurve(const Adaptor3d_Curve& theCurve)
    : myCurve(&theCurve)
{
}

//==================================================================================================

gp_Pnt ExtremaPC_OtherCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_OtherCurve::Perform(const gp_Pnt&         theP,
                                                double                theTol,
                                                ExtremaPC::SearchMode theMode) const
{
  // Use curve's natural parameter bounds
  ExtremaPC::Domain1D aDomain{myCurve->FirstParameter(), myCurve->LastParameter()};
  return performBounded(theP, aDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_OtherCurve::Perform(const gp_Pnt&              theP,
                                                const ExtremaPC::Domain1D& theDomain,
                                                double                     theTol,
                                                ExtremaPC::SearchMode      theMode) const
{
  return performBounded(theP, theDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_OtherCurve::PerformWithEndpoints(const gp_Pnt&              theP,
                                                             const ExtremaPC::Domain1D& theDomain,
                                                             double                     theTol,
                                                             ExtremaPC::SearchMode      theMode) const
{
  ExtremaPC::Result aResult = performBounded(theP, theDomain, theTol, theMode);

  // Add endpoints if interior computation succeeded or found no interior solutions
  if (aResult.Status == ExtremaPC::Status::OK || aResult.Status == ExtremaPC::Status::NoSolution)
  {
    ExtremaPC::AddEndpointExtrema(aResult, theP, theDomain, *this, theTol, theMode);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC::Status::OK;
    }
  }

  return aResult;
}

//==================================================================================================

void ExtremaPC_OtherCurve::updateCacheIfNeeded(const ExtremaPC::Domain1D& theDomain) const
{
  // Check if cache is still valid
  if (myGrid.Size() > 0
      && std::abs(myCachedDomain.Min - theDomain.Min) < ExtremaPC::THE_PARAM_TOLERANCE
      && std::abs(myCachedDomain.Max - theDomain.Max) < ExtremaPC::THE_PARAM_TOLERANCE)
  {
    return; // Cache is valid
  }

  // Use higher number of samples for general curves
  constexpr int aNbSamples = 64;

  // Rebuild grid
  math_Vector aParams =
    ExtremaPC_GridEvaluator::BuildUniformParams(theDomain.Min, theDomain.Max, aNbSamples);

  GeomGridEval_OtherCurve aGridEval(*myCurve);
  myGrid = ExtremaPC_GridEvaluator::BuildGrid(aGridEval, aParams);

  // Update cached domain
  myCachedDomain = theDomain;
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_OtherCurve::performBounded(const gp_Pnt&              theP,
                                                       const ExtremaPC::Domain1D& theDomain,
                                                       double                     theTol,
                                                       ExtremaPC::SearchMode      theMode) const
{
  ExtremaPC::Result aResult;

  if (myCurve == nullptr)
  {
    aResult.Status = ExtremaPC::Status::NotDone;
    return aResult;
  }

  // Update cache if parameter range changed
  updateCacheIfNeeded(theDomain);

  // Use the cached grid (interior extrema only)
  return ExtremaPC_GridEvaluator::PerformWithCachedGrid(myGrid, *myCurve, theP, theDomain, theTol, theMode);
}
