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

#include <ExtremaPS_OffsetSurface.hxx>

#include <math_Vector.hxx>

//==================================================================================================

ExtremaPS_OffsetSurface::ExtremaPS_OffsetSurface(const occ::handle<Geom_OffsetSurface>& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain{myAdaptor.FirstUParameter(), myAdaptor.LastUParameter(),
               myAdaptor.FirstVParameter(), myAdaptor.LastVParameter()}
{
  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

ExtremaPS_OffsetSurface::ExtremaPS_OffsetSurface(const occ::handle<Geom_OffsetSurface>& theSurface,
                                                 const ExtremaPS::Domain2D&        theDomain)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain(theDomain)
{
  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

void ExtremaPS_OffsetSurface::buildGrid()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Build the grid for the current domain using default sample count for offset surfaces.
  // Offset surfaces use the same sampling density as general surfaces.
  math_Vector aUParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.UMin, myDomain.UMax,
                                                ExtremaPS::THE_OTHER_SURFACE_NB_SAMPLES);
  math_Vector aVParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.VMin, myDomain.VMax,
                                                ExtremaPS::THE_OTHER_SURFACE_NB_SAMPLES);

  GeomGridEval_OffsetSurface anEval(mySurface);
  myEvaluator.BuildGrid(anEval, aUParams, aVParams);
}

//==================================================================================================

gp_Pnt ExtremaPS_OffsetSurface::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_OffsetSurface::Perform(const gp_Pnt&         theP,
                                                          double                theTol,
                                                          ExtremaPS::SearchMode theMode) const
{
  return myEvaluator.Perform(myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_OffsetSurface::PerformWithBoundary(const gp_Pnt&         theP,
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
