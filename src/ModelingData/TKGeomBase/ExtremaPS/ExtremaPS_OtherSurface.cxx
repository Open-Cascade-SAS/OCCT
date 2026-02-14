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

#include <ExtremaPS_OtherSurface.hxx>

#include <math_Vector.hxx>

//==================================================================================================

ExtremaPS_OtherSurface::ExtremaPS_OtherSurface(const occ::handle<Geom_Surface>& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain{myAdaptor.FirstUParameter(), myAdaptor.LastUParameter(),
               myAdaptor.FirstVParameter(), myAdaptor.LastVParameter()}
{
  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

ExtremaPS_OtherSurface::ExtremaPS_OtherSurface(const occ::handle<Geom_Surface>& theSurface,
                                               const ExtremaPS::Domain2D&  theDomain)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain(theDomain)
{
  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

void ExtremaPS_OtherSurface::buildGrid()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Build the grid for the current domain using default sample count for general surfaces
  math_Vector aUParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.UMin, myDomain.UMax,
                                                ExtremaPS::THE_OTHER_SURFACE_NB_SAMPLES);
  math_Vector aVParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.VMin, myDomain.VMax,
                                                ExtremaPS::THE_OTHER_SURFACE_NB_SAMPLES);

  GeomGridEval_OtherSurface anEval(&myAdaptor);
  myEvaluator.BuildGrid(anEval, aUParams, aVParams);
}

//==================================================================================================

gp_Pnt ExtremaPS_OtherSurface::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_OtherSurface::Perform(const gp_Pnt&         theP,
                                                         double                theTol,
                                                         ExtremaPS::SearchMode theMode) const
{
  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_OtherSurface::PerformWithBoundary(const gp_Pnt&         theP,
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
