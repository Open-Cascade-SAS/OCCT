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
#include <optional>

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
//! The domain is fixed at construction time for optimal performance with multiple queries.
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

  //! Constructor with cylinder geometry (uses natural unbounded domain).
  //! @param[in] theCylinder the cylinder to compute extrema for
  explicit ExtremaPS_Cylinder(const gp_Cylinder& theCylinder)
      : myCylinder(theCylinder),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with cylinder geometry and parameter domain.
  //! @param[in] theCylinder the cylinder to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Cylinder(const gp_Cylinder& theCylinder, const ExtremaPS::Domain2D& theDomain)
      : myCylinder(theCylinder),
        myDomain(isNaturalDomain(theDomain) ? std::nullopt
                                            : std::optional<ExtremaPS::Domain2D>(theDomain))
  {
    initCache();
  }

private:
  //! Check if domain is natural (full U period and infinite V).
  static bool isNaturalDomain(const ExtremaPS::Domain2D& theDomain)
  {
    return theDomain.IsUFullPeriod(ExtremaPS::THE_TWO_PI) && !theDomain.V().IsFinite();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache geometry components for fast computation
    const gp_Ax3& aPos  = myCylinder.Position();
    const gp_Pnt& aLoc  = aPos.Location();
    const gp_Dir& aAxis = aPos.Direction();
    const gp_Dir& aXDir = aPos.XDirection();
    const gp_Dir& aYDir = aPos.YDirection();

    myLocX   = aLoc.X();
    myLocY   = aLoc.Y();
    myLocZ   = aLoc.Z();
    myAxisX  = aAxis.X();
    myAxisY  = aAxis.Y();
    myAxisZ  = aAxis.Z();
    myXDirX  = aXDir.X();
    myXDirY  = aXDir.Y();
    myXDirZ  = aXDir.Z();
    myYDirX  = aYDir.X();
    myYDirY  = aYDir.Y();
    myYDirZ  = aYDir.Z();
    myRadius = myCylinder.Radius();
  }

public:
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
    return gp_Pnt(myLocX + theV * myAxisX + myRadius * (aCos * myXDirX + aSin * myYDirX),
                  myLocY + theV * myAxisY + myRadius * (aCos * myXDirY + aSin * myYDirY),
                  myLocZ + theV * myAxisZ + myRadius * (aCos * myXDirZ + aSin * myYDirZ));
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only (min and/or max on cylinder surface).
  //! Uses domain specified at construction time.
  //!
  //! For a cylinder, there are two interior extrema:
  //! - Minimum: closest point (radial projection)
  //! - Maximum: antipodal point (opposite side of cylinder)
  //!
  //! @param theP query point
  //! @param theTol tolerance for boundary check
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaPS::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    myResult.Clear();
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;

    // Vector from location to point
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;

    // V parameter = projection onto axis
    const double aV = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Radial components (point relative to axis at V)
    const double aRadX         = aDx - aV * myAxisX;
    const double aRadY         = aDy - aV * myAxisY;
    const double aRadZ         = aDz - aV * myAxisZ;
    const double aRadialDistSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;

    // Check for degenerate case: point on axis
    if (aRadialDistSq < theTol * theTol)
    {
      myResult.Status = ExtremaPS::Status::InfiniteSolutions;

      // For bounded V domain, account for distance along axis
      if (myDomain.has_value() && myDomain->V().IsFinite())
      {
        const double aClampedV = myDomain->V().Clamp(aV);
        const double aDeltaV   = aV - aClampedV;
        // Distance = sqrt(R² + ΔV²), so squared distance = R² + ΔV²
        myResult.InfiniteSquareDistance = myRadius * myRadius + aDeltaV * aDeltaV;
      }
      else
      {
        myResult.InfiniteSquareDistance = myRadius * myRadius;
      }
      return myResult;
    }

    const double aRadialDist = std::sqrt(aRadialDistSq);
    const double aInvRad     = 1.0 / aRadialDist;

    // Normalized radial direction
    const double aRadNormX = aRadX * aInvRad;
    const double aRadNormY = aRadY * aInvRad;
    const double aRadNormZ = aRadZ * aInvRad;

    // U parameter from radial direction
    const double aCosU = aRadNormX * myXDirX + aRadNormY * myXDirY + aRadNormZ * myXDirZ;
    const double aSinU = aRadNormX * myYDirX + aRadNormY * myYDirY + aRadNormZ * myYDirZ;
    double       aU    = std::atan2(aSinU, aCosU);
    if (aU < 0.0)
      aU += aTwoPi;

    // Normalize U to domain range for periodic parameter
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeU(aU, *myDomain);
    }

    // FAST PATH: Natural domain (full U, infinite V) - most common case
    if (!myDomain.has_value())
    {
      // Precompute axis offset for V (no clamping needed - infinite domain)
      const double aAxisOffX = myLocX + aV * myAxisX;
      const double aAxisOffY = myLocY + aV * myAxisY;
      const double aAxisOffZ = myLocZ + aV * myAxisZ;

      // Precompute R * normalized radial direction
      const double aRadCompX = myRadius * aRadNormX;
      const double aRadCompY = myRadius * aRadNormY;
      const double aRadCompZ = myRadius * aRadNormZ;

      // Minimum extremum (closest point): AxisPoint + R * radialDir
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        const gp_Pnt aSurfPt(aAxisOffX + aRadCompX, aAxisOffY + aRadCompY, aAxisOffZ + aRadCompZ);
        const double aDistMinSq = (aRadialDist - myRadius) * (aRadialDist - myRadius);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMinSq;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      // Maximum extremum (antipodal point): AxisPoint - R * radialDir
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt(aAxisOffX - aRadCompX, aAxisOffY - aRadCompY, aAxisOffZ - aRadCompZ);
        const double aDistMaxSq = (aRadialDist + myRadius) * (aRadialDist + myRadius);

        double aUOpp = aU + M_PI;
        if (aUOpp >= aTwoPi)
          aUOpp -= aTwoPi;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMaxSq;
        anExt.IsMinimum      = false;
        myResult.Extrema.Append(anExt);
      }

      myResult.Status = ExtremaPS::Status::OK;
      return myResult;
    }

    // BOUNDED PATH: Handle bounded domain
    const ExtremaPS::Domain2D& theDomain = *myDomain;

    // Check if U domain is full circle
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Check V range
    const bool aVInRange = theDomain.V().Contains(aV, theTol);

    // Full U domain with bounded V
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
        const gp_Pnt aSurfPt(aAxisOffX + aRadCompX, aAxisOffY + aRadCompY, aAxisOffZ + aRadCompZ);
        const double aDistMinSq =
          (aRadialDist - myRadius) * (aRadialDist - myRadius) + (aV - aClampedV) * (aV - aClampedV);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aClampedV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMinSq;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      // Maximum extremum (antipodal point): AxisPoint - R * radialDir
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt(aAxisOffX - aRadCompX, aAxisOffY - aRadCompY, aAxisOffZ - aRadCompZ);
        const double aDistMaxSq =
          (aRadialDist + myRadius) * (aRadialDist + myRadius) + (aV - aClampedV) * (aV - aClampedV);

        double aUOpp = aU + M_PI;
        ExtremaPS::NormalizeU(aUOpp, theDomain);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = aClampedV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistMaxSq;
        anExt.IsMinimum      = false;
        myResult.Extrema.Append(anExt);
      }

      myResult.Status = ExtremaPS::Status::OK;
      return myResult;
    }

    // GENERAL PATH: Handle bounded U domain
    const double aClampedV = theDomain.V().Clamp(aV);

    // Add minimum extremum (closest point)
    if (theMode != ExtremaPS::SearchMode::Max
        && ExtremaPS::IsInPeriodicRange(aU, theDomain.UMin, theDomain.UMax, theTol) && aVInRange)
    {
      double aClampedU = aU;
      ExtremaPS::ClampToPeriodicRange(aClampedU, theDomain.UMin, theDomain.UMax);

      const gp_Pnt aSurfPt = Value(aClampedU, aClampedV);
      const double aSqDist = theP.SquareDistance(aSurfPt);

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aClampedU;
      anExt.V              = aClampedV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = true;
      myResult.Extrema.Append(anExt);
    }

    // Antipodal point for maximum
    double aUOpp = aU + M_PI;
    ExtremaPS::NormalizeU(aUOpp, theDomain);

    // Add maximum extremum (farthest point on opposite side)
    if (theMode != ExtremaPS::SearchMode::Min
        && ExtremaPS::IsInPeriodicRange(aUOpp, theDomain.UMin, theDomain.UMax, theTol) && aVInRange)
    {
      double aClampedU = aUOpp;
      ExtremaPS::ClampToPeriodicRange(aClampedU, theDomain.UMin, theDomain.UMax);

      const gp_Pnt aSurfPt = Value(aClampedU, aClampedV);
      const double aSqDist = theP.SquareDistance(aSurfPt);

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aClampedU;
      anExt.V              = aClampedV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = false;
      myResult.Extrema.Append(anExt);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
    return myResult;
  }

  //! Find extrema including boundary edges and corners.
  //! Uses domain specified at construction time.
  //!
  //! Adds boundary extrema for truly bounded domains or when the V projection
  //! falls outside the V range.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] const ExtremaPS::Result& PerformWithBoundary(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    (void)Perform(theP, theTol, theMode);

    // If infinite solutions or natural domain (no boundaries), return immediately
    if (myResult.IsInfinite() || !myDomain.has_value())
    {
      return myResult;
    }

    const ExtremaPS::Domain2D& theDomain = *myDomain;

    // Check if boundary extrema are needed
    constexpr double aTwoPi   = ExtremaPS::THE_TWO_PI;
    const bool       aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Compute V to check if out of range
    const double aDx          = theP.X() - myLocX;
    const double aDy          = theP.Y() - myLocY;
    const double aDz          = theP.Z() - myLocZ;
    const double aV           = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;
    const bool   aVOutOfRange = !theDomain.V().Contains(aV, theTol);

    // Add boundary if U is bounded or V is out of range
    if (!aIsFullU || aVOutOfRange)
    {
      ExtremaPS::AddBoundaryExtrema(myResult, theP, theDomain, *this, theTol, theMode);
    }

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

  //! @}

  //! Returns the cylinder geometry.
  const gp_Cylinder& Cylinder() const { return myCylinder; }

  //! Returns true if domain is bounded (not natural domain).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  gp_Cylinder                        myCylinder; //!< Cylinder geometry
  std::optional<ExtremaPS::Domain2D> myDomain;   //!< Parameter domain (nullopt for natural)
  mutable ExtremaPS::Result          myResult;   //!< Reusable result storage

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;    //!< Cylinder axis location
  double myAxisX, myAxisY, myAxisZ; //!< Cylinder axis direction
  double myXDirX, myXDirY, myXDirZ; //!< Cylinder X direction
  double myYDirX, myYDirY, myYDirZ; //!< Cylinder Y direction
  double myRadius;                  //!< Cylinder radius
};

#endif // _ExtremaPS_Cylinder_HeaderFile
