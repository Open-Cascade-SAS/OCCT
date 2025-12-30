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

#include <ExtremaPS_SurfaceOfExtrusion.hxx>

#include <cmath>

//==================================================================================================

ExtremaPS_SurfaceOfExtrusion::ExtremaPS_SurfaceOfExtrusion(
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface)
    : mySurface(theSurface),
      myDomain(std::nullopt),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve())
{
  initCache();
}

//==================================================================================================

ExtremaPS_SurfaceOfExtrusion::ExtremaPS_SurfaceOfExtrusion(
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface,
  const ExtremaPS::Domain2D&                        theDomain)
    : mySurface(theSurface),
      myDomain(theDomain),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve(), theDomain.UMin, theDomain.UMax)
{
  initCache();
}

//==================================================================================================

void ExtremaPS_SurfaceOfExtrusion::initCache()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Cache extrusion direction for fast computation
  const gp_Dir& aDir = mySurface->Direction();
  myDirX             = aDir.X();
  myDirY             = aDir.Y();
  myDirZ             = aDir.Z();
}

//==================================================================================================

gp_Pnt ExtremaPS_SurfaceOfExtrusion::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfExtrusion::Perform(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPS::SearchMode theMode) const
{
  myResult.Clear();

  if (mySurface.IsNull())
  {
    myResult.Status = ExtremaPS::Status::NotDone;
    return myResult;
  }

  // Get reference point on the basis curve (at parameter 0 or domain start)
  double aRefU = 0.0;
  if (myDomain.has_value())
  {
    aRefU = myDomain->UMin;
  }
  const gp_Pnt aRefPt = mySurface->BasisCurve()->Value(aRefU);

  // Vector from reference point to query point
  const double aDx = theP.X() - aRefPt.X();
  const double aDy = theP.Y() - aRefPt.Y();
  const double aDz = theP.Z() - aRefPt.Z();

  // V = projection of (P - RefPt) onto extrusion direction
  // This gives the offset along the extrusion direction
  double aV = aDx * myDirX + aDy * myDirY + aDz * myDirZ;

  // Clamp V to domain if bounded
  bool aVInRange = true;
  if (myDomain.has_value())
  {
    if (aV < myDomain->VMin - theTol)
    {
      aV        = myDomain->VMin;
      aVInRange = false;
    }
    else if (aV > myDomain->VMax + theTol)
    {
      aV        = myDomain->VMax;
      aVInRange = false;
    }
    else
    {
      aV = myDomain->V().Clamp(aV);
    }
  }

  // Project point onto the plane at V=aV (perpendicular to extrusion direction)
  // This is the point we use for curve extrema search
  // P_projected = P - V * Dir (relative to the basis curve plane)
  const gp_Pnt aProjPt(theP.X() - aV * myDirX, theP.Y() - aV * myDirY, theP.Z() - aV * myDirZ);

  // Perform curve extrema search on the projected point
  ExtremaPC::SearchMode aCurveMode = ExtremaPC::SearchMode::MinMax;
  if (theMode == ExtremaPS::SearchMode::Min)
    aCurveMode = ExtremaPC::SearchMode::Min;
  else if (theMode == ExtremaPS::SearchMode::Max)
    aCurveMode = ExtremaPC::SearchMode::Max;

  const ExtremaPC::Result& aCurveResult = myCurveExtrema.Perform(aProjPt, theTol, aCurveMode);

  if (!aCurveResult.IsDone())
  {
    // Check for infinite solutions (point on an infinite line curve, etc.)
    if (aCurveResult.Status == ExtremaPC::Status::InfiniteSolutions)
    {
      myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aCurveResult.InfiniteSquareDistance;
      return myResult;
    }
    myResult.Status = ExtremaPS::Status::NumericalError;
    return myResult;
  }

  // Convert curve extrema to surface extrema
  for (int i = 0; i < aCurveResult.Extrema.Length(); ++i)
  {
    const ExtremaPC::ExtremumResult& aCurveExt = aCurveResult.Extrema.Value(i);
    const double                     aU        = aCurveExt.Parameter;

    // Surface point = CurvePoint + V * Direction
    const gp_Pnt& aCurvePt = aCurveExt.Point;
    const gp_Pnt  aSurfPt(aCurvePt.X() + aV * myDirX,
                          aCurvePt.Y() + aV * myDirY,
                          aCurvePt.Z() + aV * myDirZ);

    const double aSqDist = theP.SquareDistance(aSurfPt);

    ExtremaPS::ExtremumResult anExt;
    anExt.U              = aU;
    anExt.V              = aV;
    anExt.Point          = aSurfPt;
    anExt.SquareDistance = aSqDist;
    anExt.IsMinimum      = aCurveExt.IsMinimum;
    myResult.Extrema.Append(anExt);
  }

  // If V was clamped (point outside V range), we need to search along
  // the boundary curves at VMin and VMax for potential extrema
  if (!aVInRange && myDomain.has_value())
  {
    // The extrema we found are at the clamped V boundary
    // These are still valid but might not be true interior extrema
    // Mark them appropriately based on whether V is at a boundary
  }

  myResult.Status =
    myResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfExtrusion::PerformWithBoundary(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPS::SearchMode theMode) const
{
  // Start with interior extrema
  (void)Perform(theP, theTol, theMode);

  // If infinite solutions or natural domain (no boundaries), return immediately
  if (myResult.IsInfinite() || !myDomain.has_value())
  {
    return myResult;
  }

  // Bounded domain - add boundary extrema
  ExtremaPS::AddBoundaryExtrema(myResult, theP, *myDomain, *this, theTol, theMode);

  // Update status
  if (myResult.Extrema.IsEmpty())
  {
    myResult.Status = ExtremaPS::Status::NoSolution;
  }
  else
  {
    myResult.Status = ExtremaPS::Status::OK;
  }

  return myResult;
}
