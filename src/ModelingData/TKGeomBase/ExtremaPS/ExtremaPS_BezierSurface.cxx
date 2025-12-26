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

#include <ExtremaPS_BezierSurface.hxx>

#include <math_Vector.hxx>

//==================================================================================================

ExtremaPS_BezierSurface::ExtremaPS_BezierSurface(const Handle(Geom_BezierSurface)& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface)
{
  // Grid density based on surface degree - use 6*(deg+1) for robust extrema detection
  myNbUSamples = 6 * (mySurface->UDegree() + 1);
  myNbVSamples = 6 * (mySurface->VDegree() + 1);
  myNbUSamples = std::max(16, std::min(128, myNbUSamples));
  myNbVSamples = std::max(16, std::min(128, myNbVSamples));
}

//==================================================================================================

void ExtremaPS_BezierSurface::updateCacheIfNeeded(const ExtremaPS::Domain2D& theDomain) const
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

  // Rebuild the grid
  math_Vector aUParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(theDomain.UMin, theDomain.UMax, myNbUSamples);
  math_Vector aVParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(theDomain.VMin, theDomain.VMax, myNbVSamples);

  GeomGridEval_BezierSurface anEval(mySurface);
  myGrid = ExtremaPS_GridEvaluator::BuildGrid(anEval, aUParams, aVParams);

  // Update cached domain
  myCachedDomain = theDomain;
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_BezierSurface::Perform(const gp_Pnt&              theP,
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

ExtremaPS::Result ExtremaPS_BezierSurface::PerformWithBoundary(const gp_Pnt&              theP,
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
