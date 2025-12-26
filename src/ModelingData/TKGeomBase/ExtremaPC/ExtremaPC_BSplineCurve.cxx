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

#include <GeomGridEval_BSplineCurve.hxx>

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const Handle(Geom_BSplineCurve)& theCurve)
    : myCurve(theCurve),
      myAdaptor(theCurve)
{
}

//==================================================================================================

TColStd_Array1OfReal ExtremaPC_BSplineCurve::buildKnotAwareParams(double theUMin, double theUMax) const
{
  if (myCurve.IsNull())
  {
    // Fallback to uniform sampling
    return ExtremaPC_GridEvaluator::BuildUniformParams(theUMin, theUMax, 32);
  }

  const int                    aDegree = myCurve->Degree();
  const TColStd_Array1OfReal&  aKnots  = myCurve->Knots();
  NCollection_Vector<double>   aParams;

  // Always include start point
  aParams.Append(theUMin);

  // Add knots and intermediate samples for each span
  // Use 2*(degree+1) samples to ensure we don't miss extrema
  const int aSamplesPerSpan = 2 * (aDegree + 1);

  for (int i = aKnots.Lower(); i < aKnots.Upper(); ++i)
  {
    double aKnotLo = aKnots.Value(i);
    double aKnotHi = aKnots.Value(i + 1);

    // Clamp to requested range
    double aSpanLo = std::max(aKnotLo, theUMin);
    double aSpanHi = std::min(aKnotHi, theUMax);

    if (aSpanHi <= aSpanLo)
    {
      continue; // Span outside range
    }

    // Add intermediate samples within span
    double aStep = (aSpanHi - aSpanLo) / aSamplesPerSpan;
    for (int j = 1; j < aSamplesPerSpan; ++j)
    {
      double aU = aSpanLo + j * aStep;
      if (aU > theUMin && aU < theUMax)
      {
        aParams.Append(aU);
      }
    }

    // Add knot at end of span (if within range and not at end)
    if (aKnotHi > theUMin && aKnotHi < theUMax)
    {
      aParams.Append(aKnotHi);
    }
  }

  // Always include end point
  if (aParams.Last() < theUMax)
  {
    aParams.Append(theUMax);
  }

  // Convert to TColStd_Array1OfReal
  TColStd_Array1OfReal aResult(1, aParams.Length());
  for (int i = 0; i < aParams.Length(); ++i)
  {
    aResult.SetValue(i + 1, aParams.Value(i));
  }

  return aResult;
}

//==================================================================================================

gp_Pnt ExtremaPC_BSplineCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_BSplineCurve::Perform(const gp_Pnt&              theP,
                                                  const ExtremaPC::Domain1D& theDomain,
                                                  double                     theTol,
                                                  ExtremaPC::SearchMode      theMode) const
{
  return performInterior(theP, theDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_BSplineCurve::PerformWithEndpoints(const gp_Pnt&              theP,
                                                               const ExtremaPC::Domain1D& theDomain,
                                                               double                     theTol,
                                                               ExtremaPC::SearchMode      theMode) const
{
  ExtremaPC::Result aResult = performInterior(theP, theDomain, theTol, theMode);

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

void ExtremaPC_BSplineCurve::updateCacheIfNeeded(const ExtremaPC::Domain1D& theDomain) const
{
  // Check if cache is still valid
  if (myGrid.Size() > 0
      && std::abs(myCachedDomain.Min - theDomain.Min) < ExtremaPC::THE_PARAM_TOLERANCE
      && std::abs(myCachedDomain.Max - theDomain.Max) < ExtremaPC::THE_PARAM_TOLERANCE)
  {
    return; // Cache is valid
  }

  // Build knot-aware parameter grid
  TColStd_Array1OfReal aParams = buildKnotAwareParams(theDomain.Min, theDomain.Max);

  // Rebuild grid
  GeomGridEval_BSplineCurve aGridEval(myCurve);
  myGrid = ExtremaPC_GridEvaluator::BuildGrid(aGridEval, aParams);

  // Update cached domain
  myCachedDomain = theDomain;
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_BSplineCurve::performInterior(const gp_Pnt&              theP,
                                                          const ExtremaPC::Domain1D& theDomain,
                                                          double                     theTol,
                                                          ExtremaPC::SearchMode      theMode) const
{
  ExtremaPC::Result aResult;

  if (myCurve.IsNull())
  {
    aResult.Status = ExtremaPC::Status::NotDone;
    return aResult;
  }

  // Update cache if parameter range changed
  updateCacheIfNeeded(theDomain);

  // Use the cached grid (interior extrema only)
  return ExtremaPC_GridEvaluator::PerformWithCachedGrid(myGrid, myAdaptor, theP, theDomain, theTol, theMode);
}
