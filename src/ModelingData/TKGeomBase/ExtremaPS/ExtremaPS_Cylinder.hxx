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

#ifndef _ExtremaPS_Cylinder_HeaderFile
#define _ExtremaPS_Cylinder_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaPS.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>

//! @brief Point-Cylinder extrema computation.
//!
//! Computes the extrema between a 3D point and a cylindrical surface.
//!
//! Algorithm:
//! 1. Project point onto the cylinder axis to get V parameter
//! 2. Project point radially to get U parameter (angle)
//! 3. This gives one minimum (closest point on cylinder)
//! 4. The antipodal point (U + PI) gives one maximum
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only (min and/or max on cylinder)
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
//!
//! @note For a point on the cylinder axis, there are infinite solutions
//!       at all U values for the same V - returns InfiniteSolutions status.
//!
//! @note The U parameter is periodic with period 2*PI.
class ExtremaPS_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with cylinder geometry.
  //! @param theCylinder the cylinder to compute extrema for
  explicit ExtremaPS_Cylinder(const gp_Cylinder& theCylinder)
      : myCylinder(theCylinder)
  {
    // Cache geometry components for fast computation
    const gp_Ax3& aPos  = theCylinder.Position();
    const gp_Pnt& aLoc  = aPos.Location();
    const gp_Dir& aAxis = aPos.Direction();
    const gp_Dir& aXDir = aPos.XDirection();
    const gp_Dir& aYDir = aPos.YDirection();

    myLocX = aLoc.X(); myLocY = aLoc.Y(); myLocZ = aLoc.Z();
    myAxisX = aAxis.X(); myAxisY = aAxis.Y(); myAxisZ = aAxis.Z();
    myXDirX = aXDir.X(); myXDirY = aXDir.Y(); myXDirZ = aXDir.Z();
    myYDirX = aYDir.X(); myYDirY = aYDir.Y(); myYDirZ = aYDir.Z();
    myRadius = theCylinder.Radius();
  }

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter (angle in radians)
  //! @param theV V parameter (axial position)
  //! @return point on cylinder at (U, V)
  gp_Pnt Value(double theU, double theV) const
  {
    const double aCos = std::cos(theU);
    const double aSin = std::sin(theU);
    return gp_Pnt(
      myLocX + theV * myAxisX + myRadius * (aCos * myXDirX + aSin * myYDirX),
      myLocY + theV * myAxisY + myRadius * (aCos * myXDirY + aSin * myYDirY),
      myLocZ + theV * myAxisZ + myRadius * (aCos * myXDirZ + aSin * myYDirZ)
    );
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only (min and/or max on cylinder surface).
  //!
  //! For a cylinder, there are two interior extrema:
  //! - Minimum: closest point (radial projection)
  //! - Maximum: antipodal point (opposite side of cylinder)
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain [UMin,UMax] x [VMin,VMax]
  //! @param theTol tolerance for boundary check
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result with interior extrema only
  ExtremaPS::Result Perform(const gp_Pnt&                theP,
                            const ExtremaPS::Domain2D&   theDomain,
                            double                       theTol,
                            ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const
  {
    ExtremaPS::Result aResult;
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;

    // Vector from location to point
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;

    // V parameter = projection onto axis
    const double aV = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Radial components (point relative to axis at V)
    const double aRadX = aDx - aV * myAxisX;
    const double aRadY = aDy - aV * myAxisY;
    const double aRadZ = aDz - aV * myAxisZ;
    const double aRadialDistSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;

    // Check for degenerate case: point on axis
    if (aRadialDistSq < theTol * theTol)
    {
      aResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
      aResult.InfiniteSquareDistance = myRadius * myRadius;
      return aResult;
    }

    const double aRadialDist = std::sqrt(aRadialDistSq);
    const double aInvRad = 1.0 / aRadialDist;

    // Normalized radial direction
    const double aRadNormX = aRadX * aInvRad;
    const double aRadNormY = aRadY * aInvRad;
    const double aRadNormZ = aRadZ * aInvRad;

    // U parameter from radial direction
    const double aCosU = aRadNormX * myXDirX + aRadNormY * myXDirY + aRadNormZ * myXDirZ;
    const double aSinU = aRadNormX * myYDirX + aRadNormY * myYDirY + aRadNormZ * myYDirZ;
    double aU = std::atan2(aSinU, aCosU);
    if (aU < 0.0) aU += aTwoPi;

    // Check if U domain is full circle (common case)
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Check V range
    const bool aVInRange = theDomain.V().Contains(aV, theTol);

    // FAST PATH: Full U domain - most common case
    // Avoid sin/cos by using the already-computed radial direction
    if (aIsFullU)
    {
      const double aClampedV = theDomain.V().Clamp(aV);

      // Precompute axis offset for clamped V
      const double aAxisOffX = myLocX + aClampedV * myAxisX;
      const double aAxisOffY = myLocY + aClampedV * myAxisY;
      const double aAxisOffZ = myLocZ + aClampedV * myAxisZ;

      // Precompute R * normalized radial direction
      const double aRadCompX = myRadius * aRadNormX;
      const double aRadCompY = myRadius * aRadNormY;
      const double aRadCompZ = myRadius * aRadNormZ;

      // Minimum extremum (closest point): AxisPoint + R * radialDir
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        const gp_Pnt aSurfPt(aAxisOffX + aRadCompX,
                             aAxisOffY + aRadCompY,
                             aAxisOffZ + aRadCompZ);
        const double aDistMinSq = (aRadialDist - myRadius) * (aRadialDist - myRadius) +
                                  (aV - aClampedV) * (aV - aClampedV);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aClampedV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMinSq;
        anExt.IsMinimum      = true;
        aResult.Extrema.Append(anExt);
      }

      // Maximum extremum (antipodal point): AxisPoint - R * radialDir
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt(aAxisOffX - aRadCompX,
                             aAxisOffY - aRadCompY,
                             aAxisOffZ - aRadCompZ);
        const double aDistMaxSq = (aRadialDist + myRadius) * (aRadialDist + myRadius) +
                                  (aV - aClampedV) * (aV - aClampedV);

        double aUOpp = aU + M_PI;
        if (aUOpp >= aTwoPi) aUOpp -= aTwoPi;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = aClampedV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMaxSq;
        anExt.IsMinimum      = false;
        aResult.Extrema.Append(anExt);
      }

      aResult.Status = ExtremaPS::Status::OK;
      return aResult;
    }

    // GENERAL PATH: Handle bounded U domain
    const double aClampedV = theDomain.V().Clamp(aV);

    // Helper: Check if U is in range (with periodicity handling)
    auto checkUInRange = [&](double aTestU) -> bool {
      // Normalize to [UMin, UMin + 2*PI)
      while (aTestU < theDomain.UMin) aTestU += aTwoPi;
      while (aTestU >= theDomain.UMin + aTwoPi) aTestU -= aTwoPi;
      return (aTestU >= theDomain.UMin - theTol && aTestU <= theDomain.UMax + theTol);
    };

    // Add minimum extremum (closest point)
    if (theMode != ExtremaPS::SearchMode::Max && checkUInRange(aU) && aVInRange)
    {
      double aClampedU = aU;
      while (aClampedU < theDomain.UMin) aClampedU += aTwoPi;
      while (aClampedU > theDomain.UMax) aClampedU -= aTwoPi;
      aClampedU = theDomain.U().Clamp(aClampedU);

      const gp_Pnt aSurfPt = Value(aClampedU, aClampedV);
      const double aSqDist = theP.SquareDistance(aSurfPt);

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aClampedU;
      anExt.V              = aClampedV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = true;
      aResult.Extrema.Append(anExt);
    }

    // Antipodal point for maximum
    double aUOpp = aU + M_PI;
    if (aUOpp >= aTwoPi) aUOpp -= aTwoPi;

    // Add maximum extremum (farthest point on opposite side)
    if (theMode != ExtremaPS::SearchMode::Min && checkUInRange(aUOpp) && aVInRange)
    {
      double aClampedU = aUOpp;
      while (aClampedU < theDomain.UMin) aClampedU += aTwoPi;
      while (aClampedU > theDomain.UMax) aClampedU -= aTwoPi;
      aClampedU = theDomain.U().Clamp(aClampedU);

      const gp_Pnt aSurfPt = Value(aClampedU, aClampedV);
      const double aSqDist = theP.SquareDistance(aSurfPt);

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aClampedU;
      anExt.V              = aClampedV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = false;
      aResult.Extrema.Append(anExt);
    }

    aResult.Status = aResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
    return aResult;
  }

  //! Find extrema including boundary edges and corners.
  //!
  //! Adds boundary extrema for truly bounded domains or when the V projection
  //! falls outside the V range.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return result with interior + boundary extrema
  ExtremaPS::Result PerformWithBoundary(const gp_Pnt&                theP,
                                        const ExtremaPS::Domain2D&   theDomain,
                                        double                       theTol,
                                        ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    ExtremaPS::Result aResult = Perform(theP, theDomain, theTol, theMode);

    // If infinite solutions, return immediately
    if (aResult.IsInfinite())
    {
      return aResult;
    }

    // Check if boundary extrema are needed
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Compute V to check if out of range
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;
    const double aV = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;
    const bool aVOutOfRange = !theDomain.V().Contains(aV, theTol);

    // Add boundary if U is bounded or V is out of range
    if (!aIsFullU || aVOutOfRange)
    {
      ExtremaPS::AddBoundaryExtrema(aResult, theP, theDomain, *this, theTol, theMode);
    }

    // Update status
    if (aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPS::Status::NoSolution;
    }
    else
    {
      aResult.Status = ExtremaPS::Status::OK;
    }

    return aResult;
  }

  //! @}

  //! Returns the cylinder geometry.
  const gp_Cylinder& Cylinder() const { return myCylinder; }

private:
  gp_Cylinder myCylinder; //!< Cylinder geometry

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;       //!< Cylinder axis location
  double myAxisX, myAxisY, myAxisZ;    //!< Cylinder axis direction
  double myXDirX, myXDirY, myXDirZ;    //!< Cylinder X direction
  double myYDirX, myYDirY, myYDirZ;    //!< Cylinder Y direction
  double myRadius;                     //!< Cylinder radius
};

#endif // _ExtremaPS_Cylinder_HeaderFile
