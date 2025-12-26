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
#include <Precision.hxx>

namespace
{
//! Build knot-aware parameter array (like Extrema_GenExtPS::fillParams).
//! Ensures samples are placed at knot boundaries plus intermediate points.
//! @return math_Vector with 1-based indexing
math_Vector BuildKnotAwareParams(const TColStd_Array1OfReal& theKnots,
                                  int                         theDegree,
                                  double                      theParMin,
                                  double                      theParMax)
{
  // First pass: count parameters to allocate exact size
  int    aCount   = 1; // Start with theParMin
  double aPrevPar = theParMin;
  // Samples per knot span: degree + 2 ensures accuracy for all surface types.
  int aSamplesPerSpan = theDegree + 2;

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
        ++aCount;
        aPrevPar = aPar;
      }
    }
  }
  if (theParMax > aPrevPar + Precision::PConfusion())
    ++aCount;

  // Second pass: fill math_Vector
  math_Vector aResult(1, aCount);
  int         aIdx    = 1;
  aPrevPar            = theParMin;
  aResult(aIdx++)     = theParMin;

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
        aResult(aIdx++) = aPar;
        aPrevPar        = aPar;
      }
    }
  }
  if (theParMax > aPrevPar + Precision::PConfusion())
    aResult(aIdx) = theParMax;

  return aResult;
}
} // namespace

//==================================================================================================

ExtremaPS_BSplineSurface::ExtremaPS_BSplineSurface(const Handle(Geom_BSplineSurface)& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface)
{
  // Cache knots for knot-aware sampling
  myUKnots.Resize(1, mySurface->NbUKnots(), false);
  myVKnots.Resize(1, mySurface->NbVKnots(), false);
  mySurface->UKnots(myUKnots);
  mySurface->VKnots(myVKnots);

  myUDegree = mySurface->UDegree();
  myVDegree = mySurface->VDegree();
}

//==================================================================================================

void ExtremaPS_BSplineSurface::updateCacheIfNeeded(const ExtremaPS::Domain2D& theDomain) const
{
  // Check if cache is still valid
  if (myGrid.Size() > 0
      && std::abs(myCachedDomain.UMin - theDomain.UMin) < ExtremaPS::THE_PARAM_TOLERANCE
      && std::abs(myCachedDomain.UMax - theDomain.UMax) < ExtremaPS::THE_PARAM_TOLERANCE
      && std::abs(myCachedDomain.VMin - theDomain.VMin) < ExtremaPS::THE_PARAM_TOLERANCE
      && std::abs(myCachedDomain.VMax - theDomain.VMax) < ExtremaPS::THE_PARAM_TOLERANCE)
  {
    return; // Cache is valid
  }

  // Build knot-aware parameter arrays (ensures sampling at knot boundaries)
  math_Vector aUParams = BuildKnotAwareParams(myUKnots, myUDegree, theDomain.UMin, theDomain.UMax);
  math_Vector aVParams = BuildKnotAwareParams(myVKnots, myVDegree, theDomain.VMin, theDomain.VMax);

  GeomGridEval_BSplineSurface anEval(mySurface);
  myGrid = ExtremaPS_GridEvaluator::BuildGrid(anEval, aUParams, aVParams);

  // Update cached domain
  myCachedDomain = theDomain;
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_BSplineSurface::Perform(const gp_Pnt&              theP,
                                                     const ExtremaPS::Domain2D& theDomain,
                                                     double                     theTol,
                                                     ExtremaPS::SearchMode      theMode) const
{
  // Update cache if parameter range changed
  updateCacheIfNeeded(theDomain);

  // Use the cached grid (interior extrema only)
  return ExtremaPS_GridEvaluator::PerformWithCachedGrid(myGrid, myAdaptor, theP, theDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_BSplineSurface::PerformWithBoundary(const gp_Pnt&              theP,
                                                                 const ExtremaPS::Domain2D& theDomain,
                                                                 double                     theTol,
                                                                 ExtremaPS::SearchMode      theMode) const
{
  // Start with interior extrema
  ExtremaPS::Result aResult = Perform(theP, theDomain, theTol, theMode);

  // Add boundary extrema
  ExtremaPS::AddBoundaryExtrema(aResult, theP, theDomain, *this, theTol, theMode);

  if (!aResult.Extrema.IsEmpty())
  {
    aResult.Status = ExtremaPS::Status::OK;
  }

  return aResult;
}
