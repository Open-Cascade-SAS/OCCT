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

#include <ExtremaPS_SurfaceOfRevolution.hxx>

#include <cmath>

//==================================================================================================

ExtremaPS_SurfaceOfRevolution::ExtremaPS_SurfaceOfRevolution(
  const occ::handle<Geom_SurfaceOfRevolution>& theSurface)
    : mySurface(theSurface),
      myDomain(std::nullopt),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve())
{
  initCache();
}

//==================================================================================================

ExtremaPS_SurfaceOfRevolution::ExtremaPS_SurfaceOfRevolution(
  const occ::handle<Geom_SurfaceOfRevolution>& theSurface,
  const ExtremaPS::Domain2D&                   theDomain)
    : mySurface(theSurface),
      myDomain(theDomain),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve(), theDomain.VMin, theDomain.VMax)
{
  initCache();
}

//==================================================================================================

void ExtremaPS_SurfaceOfRevolution::initCache()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Cache axis components for fast computation
  const gp_Ax1& anAxis = mySurface->Axis();
  const gp_Pnt& aLoc   = anAxis.Location();
  const gp_Dir& aDir   = anAxis.Direction();

  myLocX  = aLoc.X();
  myLocY  = aLoc.Y();
  myLocZ  = aLoc.Z();
  myAxisX = aDir.X();
  myAxisY = aDir.Y();
  myAxisZ = aDir.Z();

  // Compute perpendicular directions (X and Y in the rotation plane)
  // Use the reference direction from the surface if available
  gp_Dir aXDir;
  if (std::abs(myAxisZ) < 0.9)
  {
    // Axis not aligned with Z, use Z cross Axis
    aXDir = gp_Dir(0, 0, 1).Crossed(aDir);
  }
  else
  {
    // Axis nearly aligned with Z, use X cross Axis
    aXDir = gp_Dir(1, 0, 0).Crossed(aDir);
  }
  gp_Dir aYDir = aDir.Crossed(aXDir);

  myXDirX = aXDir.X();
  myXDirY = aXDir.Y();
  myXDirZ = aXDir.Z();
  myYDirX = aYDir.X();
  myYDirY = aYDir.Y();
  myYDirZ = aYDir.Z();
}

//==================================================================================================

gp_Pnt ExtremaPS_SurfaceOfRevolution::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfRevolution::Perform(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPS::SearchMode theMode) const
{
  myResult.Clear();
  constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;

  if (mySurface.IsNull())
  {
    myResult.Status = ExtremaPS::Status::NotDone;
    return myResult;
  }

  // Vector from axis location to point
  const double aDx = theP.X() - myLocX;
  const double aDy = theP.Y() - myLocY;
  const double aDz = theP.Z() - myLocZ;

  // Project onto axis to get axial component
  const double aAxialDist = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

  // Radial components (perpendicular to axis)
  const double aRadX     = aDx - aAxialDist * myAxisX;
  const double aRadY     = aDy - aAxialDist * myAxisY;
  const double aRadZ     = aDz - aAxialDist * myAxisZ;
  const double aRadDistSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;

  // Check for degenerate case: point on axis
  if (aRadDistSq < theTol * theTol)
  {
    // Point is on the rotation axis - infinite solutions at all U values
    // Find the closest point on the meridian curve
    const ExtremaPC::Result& aCurveResult =
      myCurveExtrema.Perform(theP, theTol, ExtremaPC::SearchMode::Min);

    if (aCurveResult.IsDone() && !aCurveResult.Extrema.IsEmpty())
    {
      myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aCurveResult.Extrema.First().SquareDistance;
    }
    else
    {
      myResult.Status = ExtremaPS::Status::InfiniteSolutions;
      // Use distance to axis origin as approximation
      myResult.InfiniteSquareDistance = aAxialDist * aAxialDist;
    }
    return myResult;
  }

  // Compute U (rotation angle) analytically from radial direction
  const double aRadDist   = std::sqrt(aRadDistSq);
  const double aInvRadDist = 1.0 / aRadDist;
  const double aRadNormX  = aRadX * aInvRadDist;
  const double aRadNormY  = aRadY * aInvRadDist;
  const double aRadNormZ  = aRadZ * aInvRadDist;

  // Project normalized radial direction onto XDir and YDir to get angle
  const double aCosU = aRadNormX * myXDirX + aRadNormY * myXDirY + aRadNormZ * myXDirZ;
  const double aSinU = aRadNormX * myYDirX + aRadNormY * myYDirY + aRadNormZ * myYDirZ;
  double       aU    = std::atan2(aSinU, aCosU);
  if (aU < 0.0)
    aU += aTwoPi;

  // Check U domain if bounded
  bool aUInRange = true;
  if (myDomain.has_value())
  {
    const ExtremaPS::Domain2D& aDomain = *myDomain;
    if (!aDomain.IsUFullPeriod(aTwoPi, theTol))
    {
      // Normalize U to domain range
      double aTestU = aU;
      while (aTestU < aDomain.UMin)
        aTestU += aTwoPi;
      while (aTestU >= aDomain.UMin + aTwoPi)
        aTestU -= aTwoPi;
      aUInRange = aDomain.U().Contains(aTestU, theTol);
      if (aUInRange)
        aU = aTestU;
    }
  }

  // Now find extrema along the meridian curve
  // The query point for the curve is the point projected onto the meridian plane
  // In the meridian plane, the point has coordinates (radialDist, axialDist) relative to axis

  // Perform curve extrema search
  ExtremaPC::SearchMode aCurveMode = ExtremaPC::SearchMode::MinMax;
  if (theMode == ExtremaPS::SearchMode::Min)
    aCurveMode = ExtremaPC::SearchMode::Min;
  else if (theMode == ExtremaPS::SearchMode::Max)
    aCurveMode = ExtremaPC::SearchMode::Max;

  const ExtremaPC::Result& aCurveResult = myCurveExtrema.Perform(theP, theTol, aCurveMode);

  if (!aCurveResult.IsDone())
  {
    myResult.Status = ExtremaPS::Status::NumericalError;
    return myResult;
  }

  // Convert curve extrema to surface extrema
  for (int i = 0; i < aCurveResult.Extrema.Length(); ++i)
  {
    const ExtremaPC::ExtremumResult& aCurveExt = aCurveResult.Extrema.Value(i);
    const double                     aV        = aCurveExt.Parameter;

    // Get the point on the meridian curve
    const gp_Pnt& aCurvePt = aCurveExt.Point;

    // Compute the radial distance of this curve point from the axis
    const double aCurveDx     = aCurvePt.X() - myLocX;
    const double aCurveDy     = aCurvePt.Y() - myLocY;
    const double aCurveDz     = aCurvePt.Z() - myLocZ;
    const double aCurveAxial  = aCurveDx * myAxisX + aCurveDy * myAxisY + aCurveDz * myAxisZ;
    const double aCurveRadX   = aCurveDx - aCurveAxial * myAxisX;
    const double aCurveRadY   = aCurveDy - aCurveAxial * myAxisY;
    const double aCurveRadZ   = aCurveDz - aCurveAxial * myAxisZ;
    const double aCurveRadius = std::sqrt(aCurveRadX * aCurveRadX + aCurveRadY * aCurveRadY +
                                          aCurveRadZ * aCurveRadZ);

    // The surface point is at the same V, but rotated to angle U
    // Surface point = AxisPoint + radius * (cos(U)*XDir + sin(U)*YDir) + axial*Axis
    const double aCosUVal = std::cos(aU);
    const double aSinUVal = std::sin(aU);

    const gp_Pnt aSurfPt(myLocX + aCurveAxial * myAxisX +
                           aCurveRadius * (aCosUVal * myXDirX + aSinUVal * myYDirX),
                         myLocY + aCurveAxial * myAxisY +
                           aCurveRadius * (aCosUVal * myXDirY + aSinUVal * myYDirY),
                         myLocZ + aCurveAxial * myAxisZ +
                           aCurveRadius * (aCosUVal * myXDirZ + aSinUVal * myYDirZ));

    const double aSqDist = theP.SquareDistance(aSurfPt);

    // Add extremum if U is in range
    if (aUInRange)
    {
      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aU;
      anExt.V              = aV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aCurveExt.IsMinimum;
      myResult.Extrema.Append(anExt);
    }

    // For maximum search, also check antipodal U (U + PI)
    if (theMode != ExtremaPS::SearchMode::Min && aCurveRadius > theTol)
    {
      double aUOpp = aU + M_PI;
      if (aUOpp >= aTwoPi)
        aUOpp -= aTwoPi;

      bool aUOppInRange = true;
      if (myDomain.has_value())
      {
        const ExtremaPS::Domain2D& aDomain = *myDomain;
        if (!aDomain.IsUFullPeriod(aTwoPi, theTol))
        {
          double aTestU = aUOpp;
          while (aTestU < aDomain.UMin)
            aTestU += aTwoPi;
          while (aTestU >= aDomain.UMin + aTwoPi)
            aTestU -= aTwoPi;
          aUOppInRange = aDomain.U().Contains(aTestU, theTol);
          if (aUOppInRange)
            aUOpp = aTestU;
        }
      }

      if (aUOppInRange)
      {
        const double aCosUOpp = std::cos(aUOpp);
        const double aSinUOpp = std::sin(aUOpp);

        const gp_Pnt aSurfPtOpp(myLocX + aCurveAxial * myAxisX +
                                  aCurveRadius * (aCosUOpp * myXDirX + aSinUOpp * myYDirX),
                                myLocY + aCurveAxial * myAxisY +
                                  aCurveRadius * (aCosUOpp * myXDirY + aSinUOpp * myYDirY),
                                myLocZ + aCurveAxial * myAxisZ +
                                  aCurveRadius * (aCosUOpp * myXDirZ + aSinUOpp * myYDirZ));

        const double aSqDistOpp = theP.SquareDistance(aSurfPtOpp);

        ExtremaPS::ExtremumResult anExtOpp;
        anExtOpp.U              = aUOpp;
        anExtOpp.V              = aV;
        anExtOpp.Point          = aSurfPtOpp;
        anExtOpp.SquareDistance = aSqDistOpp;
        anExtOpp.IsMinimum      = false; // Antipodal point is always a maximum
        myResult.Extrema.Append(anExtOpp);
      }
    }
  }

  myResult.Status =
    myResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfRevolution::PerformWithBoundary(
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
