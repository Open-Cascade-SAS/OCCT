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
#include <math_Vector.hxx>

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const Handle(Geom_BSplineCurve)& theCurve)
    : myCurve(theCurve),
      myAdaptor(theCurve)
{
}

//==================================================================================================

math_Vector ExtremaPC_BSplineCurve::buildKnotAwareParams(double theUMin, double theUMax) const
{
  if (myCurve.IsNull())
  {
    // Fallback to uniform sampling
    return ExtremaPC_GridEvaluator::BuildUniformParams(theUMin, theUMax, 32);
  }

  const int                   aDegree = myCurve->Degree();
  const TColStd_Array1OfReal& aKnots  = myCurve->Knots();

  // First pass: count parameters
  int aCount = 1; // Start with theUMin
  // Use 2*(degree+1) samples per span
  const int aSamplesPerSpan = 2 * (aDegree + 1);

  for (int i = aKnots.Lower(); i < aKnots.Upper(); ++i)
  {
    double aKnotLo = aKnots.Value(i);
    double aKnotHi = aKnots.Value(i + 1);
    double aSpanLo = std::max(aKnotLo, theUMin);
    double aSpanHi = std::min(aKnotHi, theUMax);
    if (aSpanHi <= aSpanLo)
      continue;

    double aStep = (aSpanHi - aSpanLo) / aSamplesPerSpan;
    for (int j = 1; j < aSamplesPerSpan; ++j)
    {
      double aU = aSpanLo + j * aStep;
      if (aU > theUMin && aU < theUMax)
        ++aCount;
    }
    if (aKnotHi > theUMin && aKnotHi < theUMax)
      ++aCount;
  }
  ++aCount; // For theUMax

  // Second pass: fill math_Vector
  math_Vector aResult(1, aCount);
  int         aIdx = 1;
  aResult(aIdx++)  = theUMin;

  for (int i = aKnots.Lower(); i < aKnots.Upper(); ++i)
  {
    double aKnotLo = aKnots.Value(i);
    double aKnotHi = aKnots.Value(i + 1);
    double aSpanLo = std::max(aKnotLo, theUMin);
    double aSpanHi = std::min(aKnotHi, theUMax);
    if (aSpanHi <= aSpanLo)
      continue;

    double aStep = (aSpanHi - aSpanLo) / aSamplesPerSpan;
    for (int j = 1; j < aSamplesPerSpan; ++j)
    {
      double aU = aSpanLo + j * aStep;
      if (aU > theUMin && aU < theUMax)
        aResult(aIdx++) = aU;
    }
    if (aKnotHi > theUMin && aKnotHi < theUMax)
      aResult(aIdx++) = aKnotHi;
  }
  aResult(aIdx) = theUMax;

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
  math_Vector aParams = buildKnotAwareParams(theDomain.Min, theDomain.Max);

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
