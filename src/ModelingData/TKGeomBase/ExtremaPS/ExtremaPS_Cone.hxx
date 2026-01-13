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
#include <optional>

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

  //! Constructor with cone geometry (uses natural unbounded domain).
  //! @param[in] theCone the cone to compute extrema for
  explicit ExtremaPS_Cone(const gp_Cone& theCone)
      : myCone(theCone),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with cone geometry and parameter domain.
  //! @param[in] theCone the cone to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Cone(const gp_Cone& theCone, const ExtremaPS::Domain2D& theDomain)
      : myCone(theCone),
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
    // Cache cone components for fast computation
    const gp_Ax3& aPos = myCone.Position();
    const gp_Pnt& aLoc = aPos.Location();
    myLocX             = aLoc.X();
    myLocY             = aLoc.Y();
    myLocZ             = aLoc.Z();

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

    myRefRadius = myCone.RefRadius();
    mySemiAngle = myCone.SemiAngle();
    myTanAngle  = std::tan(mySemiAngle);
    myCosAngle  = std::cos(mySemiAngle);
    mySinAngle  = std::sin(mySemiAngle);
    // Apex is where R(V) = RefRadius + V*sin(α) = 0, so V_apex = -RefRadius/sin(α)
    myApexV =
      std::abs(mySinAngle) > ExtremaPS::THE_CONE_APEX_TOLERANCE ? -myRefRadius / mySinAngle : 0.0;
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
    // OCCT cone parametrization (from ElSLib::ConeValue):
    // R(V) = RefRadius + V * sin(SemiAngle)
    // Axial = V * cos(SemiAngle)
    const double aR     = myRefRadius + theV * mySinAngle;
    const double aAxial = theV * myCosAngle;
    return gp_Pnt(myLocX + aAxial * myAxisX + aR * (aCos * myXDirX + aSin * myYDirX),
                  myLocY + aAxial * myAxisY + aR * (aCos * myXDirY + aSin * myYDirY),
                  myLocZ + aAxial * myAxisZ + aR * (aCos * myXDirZ + aSin * myYDirZ));
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

    // Z (axial coordinate) = projection onto axis
    const double aZ = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Radial components (point relative to axis at Z)
    const double aRadX  = aDx - aZ * myAxisX;
    const double aRadY  = aDy - aZ * myAxisY;
    const double aRadZ  = aDz - aZ * myAxisZ;
    const double aRhoSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;
    const double aRho   = std::sqrt(aRhoSq);

    // Check for degenerate case: point on axis
    if (aRho < theTol)
    {
      // Distance to apex
      const double aApexX        = myLocX + myApexV * myAxisX;
      const double aApexY        = myLocY + myApexV * myAxisY;
      const double aApexZ        = myLocZ + myApexV * myAxisZ;
      const double aDistToApexSq = (theP.X() - aApexX) * (theP.X() - aApexX)
                                   + (theP.Y() - aApexY) * (theP.Y() - aApexY)
                                   + (theP.Z() - aApexZ) * (theP.Z() - aApexZ);

      if (aDistToApexSq < theTol * theTol)
      {
        myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = 0.0;
        return myResult;
      }

      // Natural domain: all V values are valid, apex is always accessible
      if (!myDomain.has_value())
      {
        myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = aDistToApexSq;
        return myResult;
      }

      // On axis but not at apex - bounded domain
      // Convert axial coordinate aZ to V parameter: V = Axial / cos(SemiAngle)
      const ExtremaPS::Domain2D& theDomain  = *myDomain;
      const double               aVFromZ    = aZ / myCosAngle;
      double                     aClosestV  = theDomain.V().Clamp(aVFromZ);
      double                     aRadiusAtV = myRefRadius + aClosestV * mySinAngle;

      if (aRadiusAtV < theTol && theDomain.V().Contains(myApexV, theTol))
      {
        myResult.Status                 = ExtremaPS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = aDistToApexSq;
        return myResult;
      }

      myResult.Status = ExtremaPS::Status::InfiniteSolutions;
      // Axial delta: point's axial position minus surface point's axial position
      const double aDeltaZ            = aZ - aClosestV * myCosAngle;
      myResult.InfiniteSquareDistance = aRadiusAtV * aRadiusAtV + aDeltaZ * aDeltaZ;
      return myResult;
    }

    // Compute U from radial direction
    const double aInvRho   = 1.0 / aRho;
    const double aRadNormX = aRadX * aInvRho;
    const double aRadNormY = aRadY * aInvRho;
    const double aRadNormZ = aRadZ * aInvRho;

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

    // Compute closest V on the generator
    // In OCCT cone parametrization: Z = V * cos(SemiAngle), R = RefRadius + V * sin(SemiAngle)
    // The apex is at V = myApexV, but its AXIAL position is myApexV * cos(α)
    const double aApexZ   = myApexV * myCosAngle; // Axial position of apex
    const double aLocalZ  = aZ - aApexZ;          // Point's axial distance from apex
    const double aGenDist = aLocalZ * myCosAngle + aRho * mySinAngle;
    double       aClosestV = myApexV + aGenDist;

    // Antipodal point for maximum
    double aUOpp = aU + M_PI;
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeU(aUOpp, *myDomain);
    }
    else if (aUOpp >= aTwoPi)
    {
      aUOpp -= aTwoPi;
    }

    // For maximum, project onto opposite generator (antipodal direction)
    const double aGenDistOpp = aLocalZ * myCosAngle - aRho * mySinAngle;
    double       aClosestVOpp = myApexV + aGenDistOpp;

    // Check if projections go past the apex (R < 0 means other nappe of cone)
    // When a query point is on the nappe with R < 0, its radial direction is flipped by π.
    // So aU (computed from radial direction) is U_true + π, and the correct V is aClosestVOpp.
    const double aR    = myRefRadius + aClosestV * mySinAngle;
    const double aROpp = myRefRadius + aClosestVOpp * mySinAngle;

    // The correct minimum is at whichever (U, V) pair gives smaller distance.
    // When R < 0, the surface point is valid but on the "other nappe" past the apex.
    // We need to swap if the opposite generator gives the closer point.
    if (aR >= 0.0 && aROpp < 0.0)
    {
      // The opposite generator (with R < 0) may be the correct solution.
      // This happens when query point is on the nappe past the apex.
      // Compare actual distances to decide.
      const gp_Pnt aPt1   = Value(aU, aClosestV);
      const gp_Pnt aPt2   = Value(aUOpp, aClosestVOpp);
      const double aDist1 = theP.SquareDistance(aPt1);
      const double aDist2 = theP.SquareDistance(aPt2);
      if (aDist2 < aDist1)
      {
        std::swap(aU, aUOpp);
        std::swap(aClosestV, aClosestVOpp);
      }
    }
    else if (aR < 0.0 && aROpp >= 0.0)
    {
      // Current projection went past apex, opposite has valid R.
      // Compare distances to decide.
      const gp_Pnt aPt1   = Value(aU, aClosestV);
      const gp_Pnt aPt2   = Value(aUOpp, aClosestVOpp);
      const double aDist1 = theP.SquareDistance(aPt1);
      const double aDist2 = theP.SquareDistance(aPt2);
      if (aDist2 < aDist1)
      {
        std::swap(aU, aUOpp);
        std::swap(aClosestV, aClosestVOpp);
      }
    }
    else if (aR < 0.0 && aROpp < 0.0)
    {
      // Both past apex - pick based on distance
      const gp_Pnt aPt1   = Value(aU, aClosestV);
      const gp_Pnt aPt2   = Value(aUOpp, aClosestVOpp);
      const double aDist1 = theP.SquareDistance(aPt1);
      const double aDist2 = theP.SquareDistance(aPt2);
      if (aDist2 < aDist1)
      {
        std::swap(aU, aUOpp);
        std::swap(aClosestV, aClosestVOpp);
      }
    }
    // else both R >= 0, keep current (smaller distance by construction)

    // Rename for clarity after potential swap
    const double aClosestVMax = aClosestVOpp;

    // FAST PATH: Natural domain (full U, infinite V) - most common case
    if (!myDomain.has_value())
    {
      // Minimum: point on nearest generator
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        const gp_Pnt aSurfPt = Value(aU, aClosestV);
        const double aSqDist = theP.SquareDistance(aSurfPt);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aClosestV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aSqDist;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      // Maximum: point on opposite generator
      if (theMode != ExtremaPS::SearchMode::Min)
      {
        const gp_Pnt aSurfPt = Value(aUOpp, aClosestVMax);
        const double aSqDist = theP.SquareDistance(aSurfPt);

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aUOpp;
        anExt.V              = aClosestVMax;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aSqDist;
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

    // Add minimum extremum
    if (theMode != ExtremaPS::SearchMode::Max)
    {
      if (aIsFullU || ExtremaPS::IsInPeriodicRange(aU, theDomain.UMin, theDomain.UMax, theTol))
      {
        double aClampedV = theDomain.V().Clamp(aClosestV);
        double aClampedU = aU;
        if (!aIsFullU)
        {
          ExtremaPS::ClampToPeriodicRange(aClampedU, theDomain.UMin, theDomain.UMax);
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

    // Add maximum extremum
    if (theMode != ExtremaPS::SearchMode::Min)
    {
      if (aIsFullU || ExtremaPS::IsInPeriodicRange(aUOpp, theDomain.UMin, theDomain.UMax, theTol))
      {
        double aClampedV = theDomain.V().Clamp(aClosestVMax);
        double aClampedU = aUOpp;
        if (!aIsFullU)
        {
          ExtremaPS::ClampToPeriodicRange(aClampedU, theDomain.UMin, theDomain.UMax);
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

  //! Returns the cone geometry.
  const gp_Cone& Cone() const { return myCone; }

  //! Returns true if domain is bounded (not natural domain).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  gp_Cone                            myCone;   //!< Cone geometry
  std::optional<ExtremaPS::Domain2D> myDomain; //!< Parameter domain (nullopt for natural)
  mutable ExtremaPS::Result          myResult; //!< Reusable result storage

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;    //!< Cone axis location
  double myAxisX, myAxisY, myAxisZ; //!< Cone axis direction
  double myXDirX, myXDirY, myXDirZ; //!< Cone X direction
  double myYDirX, myYDirY, myYDirZ; //!< Cone Y direction
  double myRefRadius;               //!< Reference radius
  double mySemiAngle;               //!< Semi-angle
  double myTanAngle;                //!< tan(semi-angle)
  double myCosAngle;                //!< cos(semi-angle)
  double mySinAngle;                //!< sin(semi-angle)
  double myApexV;                   //!< Apex V parameter
};

#endif // _ExtremaPS_Cone_HeaderFile
