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

namespace
{
// Default grid density for offset surfaces
constexpr int THE_NB_U_SAMPLES = 32;
constexpr int THE_NB_V_SAMPLES = 32;
} // namespace

//==================================================================================================

ExtremaPS_OffsetSurface::ExtremaPS_OffsetSurface(const Handle(Geom_OffsetSurface)& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface),
      myDomain{myAdaptor.FirstUParameter(), myAdaptor.LastUParameter(),
               myAdaptor.FirstVParameter(), myAdaptor.LastVParameter()}
{
  // Build grid eagerly at construction time
  buildGrid();
}

//==================================================================================================

ExtremaPS_OffsetSurface::ExtremaPS_OffsetSurface(const Handle(Geom_OffsetSurface)& theSurface,
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
  // Build the grid for the current domain
  math_Vector aUParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.UMin, myDomain.UMax, THE_NB_U_SAMPLES);
  math_Vector aVParams =
    ExtremaPS_GridEvaluator::BuildUniformParams(myDomain.VMin, myDomain.VMax, THE_NB_V_SAMPLES);

  GeomGridEval_OffsetSurface anEval(mySurface);
  myGrid = ExtremaPS_GridEvaluator::BuildGrid(anEval, aUParams, aVParams);
}

//==================================================================================================

gp_Pnt ExtremaPS_OffsetSurface::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_OffsetSurface::Perform(const gp_Pnt&         theP,
                                                   double                theTol,
                                                   ExtremaPS::SearchMode theMode) const
{
  // Use the pre-built grid (interior extrema only)
  return ExtremaPS_GridEvaluator::PerformWithCachedGrid(myGrid, myAdaptor, theP, myDomain, theTol, theMode);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_OffsetSurface::PerformWithBoundary(const gp_Pnt&         theP,
                                                               double                theTol,
                                                               ExtremaPS::SearchMode theMode) const
{
  // Start with interior extrema
  ExtremaPS::Result aResult = Perform(theP, theTol, theMode);

  // Add boundary extrema
  ExtremaPS::AddBoundaryExtrema(aResult, theP, myDomain, *this, theTol, theMode);

  if (!aResult.Extrema.IsEmpty())
  {
    aResult.Status = ExtremaPS::Status::OK;
  }

  return aResult;
}
