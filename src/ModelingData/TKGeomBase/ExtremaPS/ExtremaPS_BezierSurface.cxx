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
      myAdaptor(theSurface),
      myDomain{0.0, 1.0, 0.0, 1.0}  // Bezier surfaces always have domain [0,1]x[0,1]
{
  // Grid density based on surface degree - use 6*(deg+1) for robust extrema detection
  myNbUSamples = 6 * (mySurface->UDegree() + 1);
  myNbVSamples = 6 * (mySurface->VDegree() + 1);
  myNbUSamples = std::max(16, std::min(128, myNbUSamples));
  myNbVSamples = std::max(16, std::min(128, myNbVSamples));

  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

ExtremaPS_BezierSurface::ExtremaPS_BezierSurface(const Handle(Geom_BezierSurface)& theSurface,
                                                 const ExtremaPS::Domain2D&        theDomain)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain(theDomain)
{
  // Grid density based on surface degree - use 6*(deg+1) for robust extrema detection
  myNbUSamples = 6 * (mySurface->UDegree() + 1);
  myNbVSamples = 6 * (mySurface->VDegree() + 1);
  myNbUSamples = std::max(16, std::min(128, myNbUSamples));
  myNbVSamples = std::max(16, std::min(128, myNbVSamples));

  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

void ExtremaPS_BezierSurface::buildGrid()
{
  // Build the grid for the current domain
  math_Vector aUParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.UMin, myDomain.UMax, myNbUSamples);
  math_Vector aVParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.VMin, myDomain.VMax, myNbVSamples);

  GeomGridEval_BezierSurface anEval(mySurface);
  myEvaluator.BuildGrid(anEval, aUParams, aVParams);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_BezierSurface::Perform(const gp_Pnt&         theP,
                                                          double                theTol,
                                                          ExtremaPS::SearchMode theMode) const
{
  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_BezierSurface::PerformWithBoundary(const gp_Pnt&         theP,
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
