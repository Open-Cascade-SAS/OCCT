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

#ifndef _ExtremaPS_Sphere_HeaderFile
#define _ExtremaPS_Sphere_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Sphere extrema computation.
//!
//! Computes the extrema between a 3D point and a spherical surface.
//!
//! Algorithm:
//! 1. Compute direction from sphere center to point
//! 2. Closest point is at center + R * direction (minimum)
//! 3. Farthest point is at center - R * direction (maximum)
//!
//! The domain is fixed at construction time for optimal performance with multiple queries.
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only (min and max on sphere)
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
//!
//! @note For a point at the sphere center, there are infinite solutions
//!       at all (U, V) - returns InfiniteSolutions status.
//!
//! @note At the poles (V = +/- PI/2), the U parameter is undefined.
//!       The implementation uses U = 0 for poles.
class ExtremaPS_Sphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with sphere geometry (uses full sphere domain).
  //! @param[in] theSphere the sphere to compute extrema for
  explicit ExtremaPS_Sphere(const gp_Sphere& theSphere)
      : mySphere(theSphere),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with sphere geometry and parameter domain.
  //! @param[in] theSphere the sphere to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Sphere(const gp_Sphere& theSphere, const ExtremaPS::Domain2D& theDomain)
      : mySphere(theSphere),
        myDomain(isNaturalDomain(theDomain) ? std::nullopt
                                            : std::optional<ExtremaPS::Domain2D>(theDomain))
  {
    initCache();
  }

private:
  //! Check if domain is natural (full sphere).
  static bool isNaturalDomain(const ExtremaPS::Domain2D& theDomain)
  {
    return theDomain.IsUFullPeriod(ExtremaPS::THE_TWO_PI)
           && theDomain.VMin <= -ExtremaPS::THE_HALF_PI + Precision::PConfusion()
           && theDomain.VMax >= ExtremaPS::THE_HALF_PI - Precision::PConfusion();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache sphere components for fast computation
    const gp_Ax3& aPos    = mySphere.Position();
    const gp_Pnt& aCenter = aPos.Location();
    myCenterX             = aCenter.X();
    myCenterY             = aCenter.Y();
    myCenterZ             = aCenter.Z();

    const gp_Dir& aAxis = aPos.Direction();
    myAxisX             = aAxis.X();
    myAxisY             = aAxis.Y();
    myAxisZ             = aAxis.Z();

    const gp_Dir& aXDir = aPos.XDirection();
    myXDirX             = aXDir.X();
    myXDirY             = aXDir.Y();
    myXDirZ             = aXDir.Z();

    const gp_Dir& aYDir = aPos.YDirection();
    myYDirX             = aYDir.X();
    myYDirY             = aYDir.Y();
    myYDirZ             = aYDir.Z();

    myRadius = mySphere.Radius();
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter (azimuth angle)
  //! @param theV V parameter (elevation angle, -PI/2 to PI/2)
  //! @return point on sphere at (U, V)
  gp_Pnt Value(double theU, double theV) const
  {
    const double aCosU  = std::cos(theU);
    const double aSinU  = std::sin(theU);
    const double aCosV  = std::cos(theV);
    const double aSinV  = std::sin(theV);
    const double aRcosV = myRadius * aCosV;
    return gp_Pnt(
      myCenterX + aRcosV * (aCosU * myXDirX + aSinU * myYDirX) + myRadius * aSinV * myAxisX,
      myCenterY + aRcosV * (aCosU * myXDirY + aSinU * myYDirY) + myRadius * aSinV * myAxisY,
      myCenterZ + aRcosV * (aCosU * myXDirZ + aSinU * myYDirZ) + myRadius * aSinV * myAxisZ);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only (min and max on sphere surface).
  //! Uses domain specified at construction time.
  //!
  //! For a sphere, there are two interior extrema:
  //! - Minimum: closest point (toward query point)
  //! - Maximum: antipodal point (away from query point)
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaPS::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    myResult.Clear();
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;

    // Vector from center to point
    const double aDx     = theP.X() - myCenterX;
    const double aDy     = theP.Y() - myCenterY;
    const double aDz     = theP.Z() - myCenterZ;
    const double aDistSq = aDx * aDx + aDy * aDy + aDz * aDz;

    // Check for degenerate case: point at center
    if (aDistSq < theTol * theTol)
    {
      myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = myRadius * myRadius;
      return myResult;
    }

    const double aDist    = std::sqrt(aDistSq);
    const double aInvDist = 1.0 / aDist;

    // Normalized direction from center to point
    const double aDirX = aDx * aInvDist;
    const double aDirY = aDy * aInvDist;
    const double aDirZ = aDz * aInvDist;

    // V (elevation) from direction
    double aSinV    = aDirX * myAxisX + aDirY * myAxisY + aDirZ * myAxisZ;
    aSinV           = aSinV < -1.0 ? -1.0 : (aSinV > 1.0 ? 1.0 : aSinV);
    const double aV = std::asin(aSinV);

    // U (azimuth) from direction
    const double aCosV = std::cos(aV);
    double       aU    = 0.0;

    const bool aIsAtPole = (std::abs(aCosV) < theTol);
    if (!aIsAtPole)
    {
      const double aInvCosV = 1.0 / aCosV;
      const double aCosU    = (aDirX * myXDirX + aDirY * myXDirY + aDirZ * myXDirZ) * aInvCosV;
      const double aSinU    = (aDirX * myYDirX + aDirY * myYDirY + aDirZ * myYDirZ) * aInvCosV;
      aU                    = std::atan2(aSinU, aCosU);
      if (aU < 0.0)
        aU += aTwoPi;
    }

    // FAST PATH: Natural domain (full sphere) - most common case
    // Avoid sin/cos by using the already-computed direction
    if (!myDomain.has_value())
    {
      // Minimum: surface point = Center + R * direction (toward query point)
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        const gp_Pnt aSurfPt(myCenterX + myRadius * aDirX,
                             myCenterY + myRadius * aDirY,
                             myCenterZ + myRadius * aDirZ);
        const double aDistToSurf = aDist - myRadius;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistToSurf * aDistToSurf;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      // Maximum: surface point = Center - R * direction (antipodal)
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt(myCenterX - myRadius * aDirX,
                             myCenterY - myRadius * aDirY,
                             myCenterZ - myRadius * aDirZ);
        const double aDistToSurf = aDist + myRadius;

        double aUOpp = aU + M_PI;
        if (aUOpp >= aTwoPi)
          aUOpp -= aTwoPi;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = -aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistToSurf * aDistToSurf;
        anExt.IsMinimum      = false;
        myResult.Extrema.Append(anExt);
      }

      myResult.Status = ExtremaPS::Status::OK;
      return myResult;
    }

    // BOUNDED PATH: Handle bounded domain
    const ExtremaPS::Domain2D& theDomain = *myDomain;
    constexpr double           aHalfPi   = ExtremaPS::THE_HALF_PI;

    // Check if domain is full in each direction
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);
    const bool aIsFullV =
      (theDomain.VMin <= -aHalfPi + theTol && theDomain.VMax >= aHalfPi - theTol);

    // Still full domain? (shouldn't happen if isNaturalDomain worked, but be safe)
    if (aIsFullU && aIsFullV)
    {
      // Minimum: surface point = Center + R * direction (toward query point)
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        const gp_Pnt aSurfPt(myCenterX + myRadius * aDirX,
                             myCenterY + myRadius * aDirY,
                             myCenterZ + myRadius * aDirZ);
        const double aDistToSurf = aDist - myRadius;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistToSurf * aDistToSurf;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      // Maximum: surface point = Center - R * direction (antipodal)
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt(myCenterX - myRadius * aDirX,
                             myCenterY - myRadius * aDirY,
                             myCenterZ - myRadius * aDirZ);
        const double aDistToSurf = aDist + myRadius;

        double aUOpp = aU + M_PI;
        if (aUOpp >= aTwoPi)
          aUOpp -= aTwoPi;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = -aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistToSurf * aDistToSurf;
        anExt.IsMinimum      = false;
        myResult.Extrema.Append(anExt);
      }

      myResult.Status = ExtremaPS::Status::OK;
      return myResult;
    }

    // GENERAL PATH: Bounded domain
    // Helper: Check U in range (with periodicity handling)
    auto checkUInRange = [&](double aTestU) -> bool {
      if (aIsFullU)
        return true;
      while (aTestU < theDomain.UMin)
        aTestU += aTwoPi;
      while (aTestU >= theDomain.UMin + aTwoPi)
        aTestU -= aTwoPi;
      return theDomain.U().Contains(aTestU, theTol);
    };

    // Add minimum extremum (closest point)
    if (theMode != ExtremaPS::SearchMode::Max)
    {
      bool aAddMin = aIsAtPole ? theDomain.V().Contains(aV, theTol)
                               : (checkUInRange(aU) && theDomain.V().Contains(aV, theTol));

      if (aAddMin)
      {
        double aClampedV = theDomain.V().Clamp(aV);
        double aClampedU = aU;
        if (!aIsAtPole && !aIsFullU)
        {
          while (aClampedU < theDomain.UMin)
            aClampedU += aTwoPi;
          while (aClampedU > theDomain.UMax)
            aClampedU -= aTwoPi;
          aClampedU = theDomain.U().Clamp(aClampedU);
        }

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
    }

    // Antipodal point for maximum
    double aUOpp = aU + M_PI;
    if (aUOpp >= aTwoPi)
      aUOpp -= aTwoPi;
    const double aVOpp        = -aV;
    const bool   aIsOppAtPole = (std::abs(std::abs(aVOpp) - aHalfPi) < theTol);

    // Add maximum extremum (farthest point - antipodal)
    if (theMode != ExtremaPS::SearchMode::Min)
    {
      bool aAddMax = aIsOppAtPole ? theDomain.V().Contains(aVOpp, theTol)
                                  : (checkUInRange(aUOpp) && theDomain.V().Contains(aVOpp, theTol));

      if (aAddMax)
      {
        double aClampedV = theDomain.V().Clamp(aVOpp);
        double aClampedU = aUOpp;
        if (!aIsOppAtPole && !aIsFullU)
        {
          while (aClampedU < theDomain.UMin)
            aClampedU += aTwoPi;
          while (aClampedU > theDomain.UMax)
            aClampedU -= aTwoPi;
          aClampedU = theDomain.U().Clamp(aClampedU);
        }

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
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
    return myResult;
  }

  //! Find extrema including boundary edges and corners.
  //! Uses domain specified at construction time.
  //!
  //! Adds boundary extrema for truly bounded domains.
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

  //! @}

  //! Returns the sphere geometry.
  const gp_Sphere& Sphere() const { return mySphere; }

  //! Returns true if domain is bounded (not natural domain).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  gp_Sphere                          mySphere; //!< Sphere geometry
  std::optional<ExtremaPS::Domain2D> myDomain; //!< Parameter domain (nullopt for natural)
  mutable ExtremaPS::Result          myResult; //!< Reusable result storage

  // Cached components for fast computation
  double myCenterX, myCenterY, myCenterZ; //!< Sphere center
  double myAxisX, myAxisY, myAxisZ;       //!< Sphere axis (Z direction)
  double myXDirX, myXDirY, myXDirZ;       //!< Sphere X direction
  double myYDirX, myYDirY, myYDirZ;       //!< Sphere Y direction
  double myRadius;                        //!< Sphere radius
};

#endif // _ExtremaPS_Sphere_HeaderFile
