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

#ifndef _ExtremaPS_Cone_HeaderFile
#define _ExtremaPS_Cone_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaPS.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>

//! @brief Point-Cone extrema computation.
//!
//! Computes the extrema between a 3D point and a conical surface.
//!
//! Algorithm:
//! 1. Project point onto cone axis to find axial position
//! 2. For each axial position, compute the circle radius at that V
//! 3. Find U parameter (angle) by projecting onto the circle
//! 4. Minimum is at closest point, maximum at antipodal
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
//!
//! @note For a point on the cone axis, there are infinite solutions
//!       at all U values for the corresponding V.
//!
//! @note The cone has a singularity at its apex (V = -RefRadius/tan(SemiAngle)).
class ExtremaPS_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with cone geometry (uses natural cone domain).
  //! @param[in] theCone the cone to compute extrema for
  explicit ExtremaPS_Cone(const gp_Cone& theCone)
      : myCone(theCone),
        myDomain{0.0, ExtremaPS::THE_TWO_PI,
                 -Precision::Infinite(), Precision::Infinite()}
  {
    initCache();
  }

  //! Constructor with cone geometry and parameter domain.
  //! @param[in] theCone the cone to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Cone(const gp_Cone& theCone, const ExtremaPS::Domain2D& theDomain)
      : myCone(theCone),
        myDomain(theDomain)
  {
    initCache();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache cone components for fast computation
    const gp_Ax3& aPos = myCone.Position();
    const gp_Pnt& aLoc = aPos.Location();
    myLocX = aLoc.X();
    myLocY = aLoc.Y();
    myLocZ = aLoc.Z();

    const gp_Dir& aAxis = aPos.Direction();
    myAxisX = aAxis.X();
    myAxisY = aAxis.Y();
    myAxisZ = aAxis.Z();

    const gp_Dir& aXDir = aPos.XDirection();
    myXDirX = aXDir.X();
    myXDirY = aXDir.Y();
    myXDirZ = aXDir.Z();

    const gp_Dir& aYDir = aPos.YDirection();
    myYDirX = aYDir.X();
    myYDirY = aYDir.Y();
    myYDirZ = aYDir.Z();

    myRefRadius = myCone.RefRadius();
    mySemiAngle = myCone.SemiAngle();
    myTanAngle  = std::tan(mySemiAngle);
    myCosAngle  = std::cos(mySemiAngle);
    mySinAngle  = std::sin(mySemiAngle);
    myApexV     = std::abs(myTanAngle) > ExtremaPS::THE_CONE_APEX_TOLERANCE ? -myRefRadius / myTanAngle : 0.0;
  }

public:

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter (angle in radians)
  //! @param theV V parameter (axial position)
  //! @return point on cone at (U, V)
  gp_Pnt Value(double theU, double theV) const
  {
    const double aCos = std::cos(theU);
    const double aSin = std::sin(theU);
    const double aR = myRefRadius + theV * myTanAngle;
    return gp_Pnt(
      myLocX + theV * myAxisX + aR * (aCos * myXDirX + aSin * myYDirX),
      myLocY + theV * myAxisY + aR * (aCos * myXDirY + aSin * myYDirY),
      myLocZ + theV * myAxisZ + aR * (aCos * myXDirZ + aSin * myYDirZ)
    );
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //! Uses domain specified at construction time.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result with interior extrema only
  ExtremaPS::Result Perform(const gp_Pnt&         theP,
                            double                theTol,
                            ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    const ExtremaPS::Domain2D& theDomain = myDomain;
    ExtremaPS::Result aResult;

    // Vector from location to point
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;

    // Z (axial coordinate) = projection onto axis
    const double aZ = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Radial components (point relative to axis at Z)
    const double aRadX = aDx - aZ * myAxisX;
    const double aRadY = aDy - aZ * myAxisY;
    const double aRadZ = aDz - aZ * myAxisZ;
    const double aRhoSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;
    const double aRho = std::sqrt(aRhoSq);

    // Check for degenerate case: point on axis
    if (aRho < theTol)
    {
      // Distance to apex
      const double aApexX = myLocX + myApexV * myAxisX;
      const double aApexY = myLocY + myApexV * myAxisY;
      const double aApexZ = myLocZ + myApexV * myAxisZ;
      const double aDistToApexSq = (theP.X() - aApexX) * (theP.X() - aApexX) +
                                   (theP.Y() - aApexY) * (theP.Y() - aApexY) +
                                   (theP.Z() - aApexZ) * (theP.Z() - aApexZ);

      if (aDistToApexSq < theTol * theTol)
      {
        aResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
        aResult.InfiniteSquareDistance = 0.0;
        return aResult;
      }

      // On axis but not at apex
      double aClosestV = theDomain.V().Clamp(aZ);
      double aRadiusAtV = myRefRadius + aClosestV * myTanAngle;

      if (aRadiusAtV < theTol && theDomain.V().Contains(myApexV, theTol))
      {
        aResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
        aResult.InfiniteSquareDistance = aDistToApexSq;
        return aResult;
      }

      aResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
      const double aDeltaZ = aZ - aClosestV;
      aResult.InfiniteSquareDistance = aRadiusAtV * aRadiusAtV + aDeltaZ * aDeltaZ;
      return aResult;
    }

    // Compute U from radial direction
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;
    const double aInvRho = 1.0 / aRho;
    const double aRadNormX = aRadX * aInvRho;
    const double aRadNormY = aRadY * aInvRho;
    const double aRadNormZ = aRadZ * aInvRho;

    const double aCosU = aRadNormX * myXDirX + aRadNormY * myXDirY + aRadNormZ * myXDirZ;
    const double aSinU = aRadNormX * myYDirX + aRadNormY * myYDirY + aRadNormZ * myYDirZ;
    double aU = std::atan2(aSinU, aCosU);
    if (aU < 0.0) aU += aTwoPi;

    // Compute closest V on the generator
    const double aLocalZ = aZ - myApexV;
    const double aGenDist = aLocalZ * myCosAngle + aRho * mySinAngle;
    const double aClosestV = myApexV + aGenDist * myCosAngle;

    // Antipodal point for maximum
    double aUOpp = aU + M_PI;
    if (aUOpp >= aTwoPi) aUOpp -= aTwoPi;

    // For maximum, project onto opposite generator
    const double aGenDistMax = aLocalZ * myCosAngle - aRho * mySinAngle;
    const double aClosestVMax = myApexV + aGenDistMax * myCosAngle;

    // Check if U domain is full circle
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Helper: Check U in range (with periodicity handling)
    auto checkUInRange = [&](double aTestU) -> bool {
      if (aIsFullU) return true;
      while (aTestU < theDomain.UMin) aTestU += aTwoPi;
      while (aTestU >= theDomain.UMin + aTwoPi) aTestU -= aTwoPi;
      return theDomain.U().Contains(aTestU, theTol);
    };

    // Add minimum extremum
    if (theMode != ExtremaPS::SearchMode::Max)
    {
      if (checkUInRange(aU))
      {
        double aClampedV = theDomain.V().Clamp(aClosestV);
        double aClampedU = aU;
        if (!aIsFullU)
        {
          while (aClampedU < theDomain.UMin) aClampedU += aTwoPi;
          while (aClampedU > theDomain.UMax) aClampedU -= aTwoPi;
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
        aResult.Extrema.Append(anExt);
      }
    }

    // Add maximum extremum
    if (theMode != ExtremaPS::SearchMode::Min)
    {
      if (checkUInRange(aUOpp))
      {
        double aClampedV = theDomain.V().Clamp(aClosestVMax);
        double aClampedU = aUOpp;
        if (!aIsFullU)
        {
          while (aClampedU < theDomain.UMin) aClampedU += aTwoPi;
          while (aClampedU > theDomain.UMax) aClampedU -= aTwoPi;
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
        aResult.Extrema.Append(anExt);
      }
    }

    aResult.Status = aResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
    return aResult;
  }

  //! Find extrema including boundary edges and corners.
  //! Uses domain specified at construction time.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return result with interior + boundary extrema
  ExtremaPS::Result PerformWithBoundary(const gp_Pnt&         theP,
                                        double                theTol,
                                        ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    const ExtremaPS::Domain2D& theDomain = myDomain;
    // Start with interior extrema
    ExtremaPS::Result aResult = Perform(theP, theTol, theMode);

    // If infinite solutions, return immediately
    if (aResult.IsInfinite())
    {
      return aResult;
    }

    // Check if boundary extrema are needed
    constexpr double aTwoPi = ExtremaPS::THE_TWO_PI;
    const bool aIsFullU = theDomain.IsUFullPeriod(aTwoPi, theTol);

    // Add boundary if U is bounded
    if (!aIsFullU)
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

  //! Returns the cone geometry.
  const gp_Cone& Cone() const { return myCone; }

  //! Returns the parameter domain.
  const ExtremaPS::Domain2D& Domain() const { return myDomain; }

private:
  gp_Cone             myCone;    //!< Cone geometry
  ExtremaPS::Domain2D myDomain;  //!< Parameter domain (fixed at construction)

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;        //!< Cone axis location
  double myAxisX, myAxisY, myAxisZ;     //!< Cone axis direction
  double myXDirX, myXDirY, myXDirZ;     //!< Cone X direction
  double myYDirX, myYDirY, myYDirZ;     //!< Cone Y direction
  double myRefRadius;                   //!< Reference radius
  double mySemiAngle;                   //!< Semi-angle
  double myTanAngle;                    //!< tan(semi-angle)
  double myCosAngle;                    //!< cos(semi-angle)
  double mySinAngle;                    //!< sin(semi-angle)
  double myApexV;                       //!< Apex V parameter
};

#endif // _ExtremaPS_Cone_HeaderFile
