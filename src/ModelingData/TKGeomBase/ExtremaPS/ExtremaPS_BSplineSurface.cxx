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

#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>

namespace
{
//! Build knot-aware parameter array (like Extrema_GenExtPS::fillParams).
//! Ensures samples are placed at knot boundaries plus intermediate points.
TColStd_Array1OfReal BuildKnotAwareParams(const TColStd_Array1OfReal& theKnots,
                                           int                         theDegree,
                                           double                      theParMin,
                                           double                      theParMax)
{
  NCollection_Vector<double> aParams;
  aParams.Append(theParMin);

  double aPrevPar = theParMin;
  int    aSamplesPerSpan = std::max(theDegree, 2);

  for (int i = theKnots.Lower(); i < theKnots.Upper(); ++i)
  {
    double aKnotLo = theKnots.Value(i);
    double aKnotHi = theKnots.Value(i + 1);

    // Skip knots outside the parameter range
    if (aKnotHi < theParMin + Precision::PConfusion())
      continue;
    if (aKnotLo > theParMax - Precision::PConfusion())
      break;

    // Clamp to parameter range
    aKnotLo = std::max(aKnotLo, theParMin);
    aKnotHi = std::min(aKnotHi, theParMax);

    double aStep = (aKnotHi - aKnotLo) / aSamplesPerSpan;

    // Add samples within this knot span (including the knot boundary)
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

  // Ensure the end parameter is included
  if (theParMax > aPrevPar + Precision::PConfusion())
  {
    aParams.Append(theParMax);
  }

  // Convert to TColStd_Array1OfReal
  TColStd_Array1OfReal aResult(1, aParams.Length());
  for (int i = 0; i < aParams.Length(); ++i)
  {
    aResult.SetValue(i + 1, aParams.Value(i));
  }
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
  TColStd_Array1OfReal aUParams = BuildKnotAwareParams(myUKnots, myUDegree, theDomain.UMin, theDomain.UMax);
  TColStd_Array1OfReal aVParams = BuildKnotAwareParams(myVKnots, myVDegree, theDomain.VMin, theDomain.VMax);

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
