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
#include <NCollection_Vector.hxx>

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain{theCurve->FirstParameter(), theCurve->LastParameter()}
{
  buildGrid();
}

//==================================================================================================

ExtremaPC_BSplineCurve::ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve,
                                               const ExtremaPC::Domain1D&       theDomain)
    : myCurve(theCurve),
      myAdaptor(theCurve),
      myDomain(theDomain)
{
  buildGrid();
}

//==================================================================================================

math_Vector ExtremaPC_BSplineCurve::buildKnotAwareParams() const
{
  if (myCurve.IsNull())
  {
    // Fallback to uniform sampling
    return ExtremaPC_GridEvaluator::BuildUniformParams(myDomain.Min, myDomain.Max,
                                                       ExtremaPC::THE_BSPLINE_FALLBACK_SAMPLES);
  }

  const double theUMin = myDomain.Min;
  const double theUMax = myDomain.Max;

  const int                         aDegree = myCurve->Degree();
  const NCollection_Array1<double>& aKnots  = myCurve->Knots();

  // Use multiplier*(degree+1) samples per span for accurate extrema detection.
  const int aSamplesPerSpan = ExtremaPC::THE_BSPLINE_SPAN_MULTIPLIER * (aDegree + 1);

  // Single-pass algorithm using dynamic vector
  NCollection_Vector<double> aParams;
  aParams.Append(theUMin);

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
        aParams.Append(aU);
    }
    if (aKnotHi > theUMin && aKnotHi < theUMax)
      aParams.Append(aKnotHi);
  }
  aParams.Append(theUMax);

  // Convert to math_Vector (required by BuildGrid interface)
  math_Vector aResult(1, aParams.Length());
  for (int i = 0; i < aParams.Length(); ++i)
  {
    aResult(i + 1) = aParams.Value(i);
  }

  return aResult;
}

//==================================================================================================

void ExtremaPC_BSplineCurve::buildGrid()
{
  if (myCurve.IsNull())
  {
    return;
  }

  // Build knot-aware parameter grid
  math_Vector aParams = buildKnotAwareParams();

  // Build grid using the evaluator
  GeomGridEval_BSplineCurve aGridEval(myCurve);
  myEvaluator.BuildGrid(aGridEval, aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_BSplineCurve::Value(double theU) const
{
  return myCurve->Value(theU);
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

  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_BSplineCurve::PerformWithEndpoints(const gp_Pnt&         theP,
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
