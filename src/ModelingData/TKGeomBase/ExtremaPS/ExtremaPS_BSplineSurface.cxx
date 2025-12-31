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

#include <ExtremaPS_BSplineSurface.hxx>

#include <math_Vector.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

namespace
{
//! Build knot-aware parameter array (like Extrema_GenExtPS::fillParams).
//! Ensures samples are placed at knot boundaries plus intermediate points.
//! Uses single-pass algorithm with dynamic vector for efficiency.
//! @return math_Vector with 1-based indexing
math_Vector BuildKnotAwareParams(const NCollection_Array1<double>& theKnots,
                                 int                               theDegree,
                                 double                            theParMin,
                                 double                            theParMax)
{
  // Samples per knot span: degree + offset ensures accuracy for all surface types.
  const int aSamplesPerSpan = theDegree + ExtremaPS::THE_BSPLINE_SPAN_OFFSET;

  // Single-pass algorithm using dynamic vector
  NCollection_Vector<double> aParams;
  double                     aPrevPar = theParMin;
  aParams.Append(theParMin);

  for (int i = theKnots.Lower(); i < theKnots.Upper(); ++i)
  {
    double aKnotLo = theKnots.Value(i);
    double aKnotHi = theKnots.Value(i + 1);

    if (aKnotHi < theParMin + Precision::PConfusion())
      continue;
    if (aKnotLo > theParMax - Precision::PConfusion())
      break;

    aKnotLo = std::max(aKnotLo, theParMin);
    aKnotHi = std::min(aKnotHi, theParMax);

    double aStep = (aKnotHi - aKnotLo) / aSamplesPerSpan;
    for (int k = 0; k <= aSamplesPerSpan; ++k)
    {
      double aPar = aKnotLo + k * aStep;
      if (aPar > theParMax - Precision::PConfusion())
        break;
      if (aPar > aPrevPar + Precision::PConfusion())
      {
        aParams.Append(aPar);
        aPrevPar = aPar;
      }
    }
  }
  if (theParMax > aPrevPar + Precision::PConfusion())
    aParams.Append(theParMax);

  // Convert to math_Vector (required by BuildGrid interface)
  math_Vector aResult(1, aParams.Length());
  for (int i = 0; i < aParams.Length(); ++i)
  {
    aResult(i + 1) = aParams.Value(i);
  }

  return aResult;
}
} // namespace

//==================================================================================================

ExtremaPS_BSplineSurface::ExtremaPS_BSplineSurface(
  const occ::handle<Geom_BSplineSurface>& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface)
{
  // Get bounds from surface
  double aU1, aU2, aV1, aV2;
  theSurface->Bounds(aU1, aU2, aV1, aV2);
  myDomain = ExtremaPS::Domain2D(aU1, aU2, aV1, aV2);

  // Cache knots for knot-aware sampling
  myUKnots.Resize(1, mySurface->NbUKnots(), false);
  myVKnots.Resize(1, mySurface->NbVKnots(), false);
  mySurface->UKnots(myUKnots);
  mySurface->VKnots(myVKnots);

  myUDegree = mySurface->UDegree();
  myVDegree = mySurface->VDegree();

  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

ExtremaPS_BSplineSurface::ExtremaPS_BSplineSurface(
  const occ::handle<Geom_BSplineSurface>& theSurface,
  const ExtremaPS::Domain2D&              theDomain)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain(theDomain)
{
  // Cache knots for knot-aware sampling
  myUKnots.Resize(1, mySurface->NbUKnots(), false);
  myVKnots.Resize(1, mySurface->NbVKnots(), false);
  mySurface->UKnots(myUKnots);
  mySurface->VKnots(myVKnots);

  myUDegree = mySurface->UDegree();
  myVDegree = mySurface->VDegree();

  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

void ExtremaPS_BSplineSurface::buildGrid()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Build knot-aware parameter arrays (ensures sampling at knot boundaries)
  math_Vector aUParams = BuildKnotAwareParams(myUKnots, myUDegree, myDomain.UMin, myDomain.UMax);
  math_Vector aVParams = BuildKnotAwareParams(myVKnots, myVDegree, myDomain.VMin, myDomain.VMax);

  GeomGridEval_BSplineSurface anEval(mySurface);
  myEvaluator.BuildGrid(anEval, aUParams, aVParams);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_BSplineSurface::Perform(const gp_Pnt&         theP,
                                                           double                theTol,
                                                           ExtremaPS::SearchMode theMode) const
{
  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_BSplineSurface::PerformWithBoundary(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPS::SearchMode theMode) const
{
  // Get interior extrema (populates myEvaluator's result)
  (void)myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);

  // Add boundary extrema to the result
  ExtremaPS::Result& aResult = myEvaluator.Result();
  ExtremaPS::AddBoundaryExtrema(aResult, theP, myDomain, *this, theTol, theMode);

  if (!aResult.Extrema.IsEmpty())
  {
    aResult.Status = ExtremaPS::Status::OK;
  }

  return aResult;
}
