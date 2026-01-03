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

#ifndef _ExtremaPS_Torus_HeaderFile
#define _ExtremaPS_Torus_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Torus extrema computation.
//!
//! Computes the extrema between a 3D point and a toroidal surface.
//!
//! A torus can have up to 4 extrema for a general point position:
//! - 2 on the nearest generating circle (min and max along minor radius)
//! - 2 on the farthest generating circle (min and max along minor radius)
//!
//! Algorithm:
//! 1. Find the generating circle nearest and farthest from the point
//! 2. On each circle, find closest and farthest points
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
//!
//! @note For a point on the torus axis, there are infinite solutions
//!       at all U values for two V values.
//!
//! @note Both U and V parameters are periodic with period 2*PI.
class ExtremaPS_Torus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with torus geometry (uses full torus domain).
  //! @param[in] theTorus the torus to compute extrema for
  explicit ExtremaPS_Torus(const gp_Torus& theTorus)
      : myTorus(theTorus),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with torus geometry and parameter domain.
  //! @param[in] theTorus the torus to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Torus(const gp_Torus& theTorus, const ExtremaPS::Domain2D& theDomain)
      : myTorus(theTorus),
        myDomain(isNaturalDomain(theDomain) ? std::nullopt
                                            : std::optional<ExtremaPS::Domain2D>(theDomain))
  {
    initCache();
  }

private:
  //! Check if domain is natural (full torus).
  static bool isNaturalDomain(const ExtremaPS::Domain2D& theDomain)
  {
    return theDomain.IsUFullPeriod(ExtremaPS::THE_TWO_PI)
           && theDomain.IsVFullPeriod(ExtremaPS::THE_TWO_PI);
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache torus components for fast computation
    const gp_Ax3& aPos    = myTorus.Position();
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

    myMajorRadius = myTorus.MajorRadius();
    myMinorRadius = myTorus.MinorRadius();
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter (major angle)
  //! @param theV V parameter (minor angle)
  //! @return point on torus at (U, V)
  gp_Pnt Value(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    const double aCosV = std::cos(theV);
    const double aSinV = std::sin(theV);
    const double aReff = myMajorRadius + myMinorRadius * aCosV;
    return gp_Pnt(
      myCenterX + aReff * (aCosU * myXDirX + aSinU * myYDirX) + myMinorRadius * aSinV * myAxisX,
      myCenterY + aReff * (aCosU * myXDirY + aSinU * myYDirY) + myMinorRadius * aSinV * myAxisY,
      myCenterZ + aReff * (aCosU * myXDirZ + aSinU * myYDirZ) + myMinorRadius * aSinV * myAxisZ);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //! Uses domain specified at construction time.
  //!
  //! A torus can have up to 4 interior extrema for a general point.
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
    const double aDx = theP.X() - myCenterX;
    const double aDy = theP.Y() - myCenterY;
    const double aDz = theP.Z() - myCenterZ;

    // Height (along axis)
    const double aHeight = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Radial components (in plane perpendicular to axis)
    const double aRadX         = aDx - aHeight * myAxisX;
    const double aRadY         = aDy - aHeight * myAxisY;
    const double aRadZ         = aDz - aHeight * myAxisZ;
    const double aRadialDistSq = aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ;

    // Check for degenerate case: point on axis
    if (aRadialDistSq < theTol * theTol)
    {
      myResult.Status            = ExtremaPS::Status::InfiniteSolutions;
      const double aDistToCircle = std::sqrt(aHeight * aHeight + myMajorRadius * myMajorRadius);
      const double aMinDist      = std::abs(aDistToCircle - myMinorRadius);
      myResult.InfiniteSquareDistance = aMinDist * aMinDist;
      return myResult;
    }

    const double aRadialDist = std::sqrt(aRadialDistSq);
    const double aInvRadDist = 1.0 / aRadialDist;

    // Normalized radial direction (mutable for apple torus case)
    double aRadNormX = aRadX * aInvRadDist;
    double aRadNormY = aRadY * aInvRadDist;
    double aRadNormZ = aRadZ * aInvRadDist;

    // U parameter (major angle)
    const double aCosU = aRadNormX * myXDirX + aRadNormY * myXDirY + aRadNormZ * myXDirZ;
    const double aSinU = aRadNormX * myYDirX + aRadNormY * myYDirY + aRadNormZ * myYDirZ;
    double       aU    = std::atan2(aSinU, aCosU);
    if (aU < 0.0)
      aU += aTwoPi;

    // Apple torus case (MajorRadius < MinorRadius): need to check which generating circle
    // is actually closer. The radial projection may point to the wrong one.
    // This matches ElSLib::TorusParameters behavior.
    if (myMajorRadius < myMinorRadius)
    {
      // Circle center at U
      const double aCircleCenterX1 = myCenterX + myMajorRadius * aRadNormX;
      const double aCircleCenterY1 = myCenterY + myMajorRadius * aRadNormY;
      const double aCircleCenterZ1 = myCenterZ + myMajorRadius * aRadNormZ;

      // Circle center at U + PI (opposite)
      const double aCircleCenterX2 = myCenterX - myMajorRadius * aRadNormX;
      const double aCircleCenterY2 = myCenterY - myMajorRadius * aRadNormY;
      const double aCircleCenterZ2 = myCenterZ - myMajorRadius * aRadNormZ;

      // Vector from circle center 1 to point
      const double aDx1            = theP.X() - aCircleCenterX1;
      const double aDy1            = theP.Y() - aCircleCenterY1;
      const double aDz1            = theP.Z() - aCircleCenterZ1;
      const double aDistSq1        = aDx1 * aDx1 + aDy1 * aDy1 + aDz1 * aDz1;
      const double aMinorRadiusSq  = myMinorRadius * myMinorRadius;
      const double aD1             = std::abs(aDistSq1 - aMinorRadiusSq);

      // Vector from circle center 2 to point
      const double aDx2     = theP.X() - aCircleCenterX2;
      const double aDy2     = theP.Y() - aCircleCenterY2;
      const double aDz2     = theP.Z() - aCircleCenterZ2;
      const double aDistSq2 = aDx2 * aDx2 + aDy2 * aDy2 + aDz2 * aDz2;
      const double aD2      = std::abs(aDistSq2 - aMinorRadiusSq);

      // If the opposite circle is closer, use U + PI and flip radial direction
      if (aD2 < aD1)
      {
        aU += M_PI;
        if (aU >= aTwoPi)
          aU -= aTwoPi;
        // Flip radial norm to point to the correct generating circle center
        aRadNormX = -aRadNormX;
        aRadNormY = -aRadNormY;
        aRadNormZ = -aRadNormZ;
      }
    }

    // Normalize U to domain range for periodic parameter
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeU(aU, *myDomain);
    }

    // Antipodal U
    double aUOpp = aU + M_PI;
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeU(aUOpp, *myDomain);
    }
    else if (aUOpp >= aTwoPi)
    {
      aUOpp -= aTwoPi;
    }

    // Center of nearest generating circle
    const double aCircleCenterX = myCenterX + myMajorRadius * aRadNormX;
    const double aCircleCenterY = myCenterY + myMajorRadius * aRadNormY;
    const double aCircleCenterZ = myCenterZ + myMajorRadius * aRadNormZ;

    // Vector from circle center to point
    const double aCircleDx = theP.X() - aCircleCenterX;
    const double aCircleDy = theP.Y() - aCircleCenterY;
    const double aCircleDz = theP.Z() - aCircleCenterZ;
    const double aCircleDistSq =
      aCircleDx * aCircleDx + aCircleDy * aCircleDy + aCircleDz * aCircleDz;
    const double aCircleDist = std::sqrt(aCircleDistSq);

    // V parameter (minor angle)
    double aV = 0.0;
    double aCircleNormX, aCircleNormY, aCircleNormZ;
    if (aCircleDist < Precision::Confusion())
    {
      aCircleNormX = aRadNormX;
      aCircleNormY = aRadNormY;
      aCircleNormZ = aRadNormZ;
      aV           = 0.0;
    }
    else
    {
      const double aInvCircleDist = 1.0 / aCircleDist;
      aCircleNormX                = aCircleDx * aInvCircleDist;
      aCircleNormY                = aCircleDy * aInvCircleDist;
      aCircleNormZ                = aCircleDz * aInvCircleDist;
      const double aCosV =
        aCircleNormX * aRadNormX + aCircleNormY * aRadNormY + aCircleNormZ * aRadNormZ;
      const double aSinV = aCircleNormX * myAxisX + aCircleNormY * myAxisY + aCircleNormZ * myAxisZ;
      aV                 = std::atan2(aSinV, aCosV);
      if (aV < 0.0)
        aV += aTwoPi;
    }

    // Normalize V to domain range for periodic parameter
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeV(aV, *myDomain);
    }

    // Antipodal V
    double aVOpp = aV + M_PI;
    if (myDomain.has_value())
    {
      ExtremaPS::NormalizeV(aVOpp, *myDomain);
    }
    else if (aVOpp >= aTwoPi)
    {
      aVOpp -= aTwoPi;
    }

    // FAST PATH: Natural domain (full torus) - compute extrema directly without bounds checking
    if (!myDomain.has_value())
    {
      if (theMode != ExtremaPS::SearchMode::Max)
      {
        // Minimum: closest point on nearest generating circle
        const gp_Pnt aSurfPt(aCircleCenterX + myMinorRadius * aCircleNormX,
                             aCircleCenterY + myMinorRadius * aCircleNormY,
                             aCircleCenterZ + myMinorRadius * aCircleNormZ);
        const double aDistToSurf = aCircleDist - myMinorRadius;

        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aU;
        anExt.V              = aV;
        anExt.Point          = aSurfPt;
        anExt.SquareDistance = aDistToSurf * aDistToSurf;
        anExt.IsMinimum      = true;
        myResult.Extrema.Append(anExt);
      }

      if (theMode != ExtremaPS::SearchMode::Min)
      {
        // Maximum 1: antipodal on nearest circle (same U, V + PI)
        const gp_Pnt aSurfPt1(aCircleCenterX - myMinorRadius * aCircleNormX,
                              aCircleCenterY - myMinorRadius * aCircleNormY,
                              aCircleCenterZ - myMinorRadius * aCircleNormZ);
        const double aDistToSurf1 = aCircleDist + myMinorRadius;

        ExtremaPS::ExtremumResult anExt1;
        anExt1.U              = aU;
        anExt1.V              = aVOpp;
        anExt1.Point          = aSurfPt1;
        anExt1.SquareDistance = aDistToSurf1 * aDistToSurf1;
        anExt1.IsMinimum      = false;
        myResult.Extrema.Append(anExt1);

        // Maximum 2 & 3: opposite generating circle
        const double aOppCircleCenterX = myCenterX - myMajorRadius * aRadNormX;
        const double aOppCircleCenterY = myCenterY - myMajorRadius * aRadNormY;
        const double aOppCircleCenterZ = myCenterZ - myMajorRadius * aRadNormZ;

        const double aOppCircleDx = theP.X() - aOppCircleCenterX;
        const double aOppCircleDy = theP.Y() - aOppCircleCenterY;
        const double aOppCircleDz = theP.Z() - aOppCircleCenterZ;
        const double aOppCircleDistSq =
          aOppCircleDx * aOppCircleDx + aOppCircleDy * aOppCircleDy + aOppCircleDz * aOppCircleDz;
        const double aOppCircleDist = std::sqrt(aOppCircleDistSq);

        if (aOppCircleDist > theTol)
        {
          const double aInvOppDist = 1.0 / aOppCircleDist;
          const double aOppNormX   = aOppCircleDx * aInvOppDist;
          const double aOppNormY   = aOppCircleDy * aInvOppDist;
          const double aOppNormZ   = aOppCircleDz * aInvOppDist;

          // V for opposite circle
          const double aOppCosV =
            aOppNormX * (-aRadNormX) + aOppNormY * (-aRadNormY) + aOppNormZ * (-aRadNormZ);
          const double aOppSinV = aOppNormX * myAxisX + aOppNormY * myAxisY + aOppNormZ * myAxisZ;
          double       aVOpp2   = std::atan2(aOppSinV, aOppCosV);
          if (aVOpp2 < 0.0)
            aVOpp2 += aTwoPi;

          const gp_Pnt aSurfPt2(aOppCircleCenterX + myMinorRadius * aOppNormX,
                                aOppCircleCenterY + myMinorRadius * aOppNormY,
                                aOppCircleCenterZ + myMinorRadius * aOppNormZ);
          const double aDistToSurf2 = aOppCircleDist - myMinorRadius;

          ExtremaPS::ExtremumResult anExt2;
          anExt2.U              = aUOpp;
          anExt2.V              = aVOpp2;
          anExt2.Point          = aSurfPt2;
          anExt2.SquareDistance = aDistToSurf2 * aDistToSurf2;
          anExt2.IsMinimum      = false;
          myResult.Extrema.Append(anExt2);

          double aVOpp3 = aVOpp2 + M_PI;
          if (aVOpp3 >= aTwoPi)
            aVOpp3 -= aTwoPi;

          const gp_Pnt aSurfPt3(aOppCircleCenterX - myMinorRadius * aOppNormX,
                                aOppCircleCenterY - myMinorRadius * aOppNormY,
                                aOppCircleCenterZ - myMinorRadius * aOppNormZ);
          const double aDistToSurf3 = aOppCircleDist + myMinorRadius;

          ExtremaPS::ExtremumResult anExt3;
          anExt3.U              = aUOpp;
          anExt3.V              = aVOpp3;
          anExt3.Point          = aSurfPt3;
          anExt3.SquareDistance = aDistToSurf3 * aDistToSurf3;
          anExt3.IsMinimum      = false;
          myResult.Extrema.Append(anExt3);
        }
      }

      myResult.Status = ExtremaPS::Status::OK;
      return myResult;
    }

    // GENERAL PATH: Bounded domains - myDomain is guaranteed to have value here
    const ExtremaPS::Domain2D& aDomain  = *myDomain;
    const bool                 aIsFullU = aDomain.IsUFullPeriod(aTwoPi, theTol);
    const bool                 aIsFullV = aDomain.IsVFullPeriod(aTwoPi, theTol);

    auto addExtremum = [&](double aExtU, double aExtV, bool aIsMin) {
      // Check if in range
      const bool aUInRange =
        aIsFullU || ExtremaPS::IsInPeriodicRange(aExtU, aDomain.UMin, aDomain.UMax, theTol);
      const bool aVInRange =
        aIsFullV || ExtremaPS::IsInPeriodicRange(aExtV, aDomain.VMin, aDomain.VMax, theTol);
      if (!aUInRange || !aVInRange)
        return;

      double aClampedU = aExtU;
      double aClampedV = aExtV;
      if (!aIsFullU)
      {
        ExtremaPS::ClampToPeriodicRange(aClampedU, aDomain.UMin, aDomain.UMax);
      }
      if (!aIsFullV)
      {
        ExtremaPS::ClampToPeriodicRange(aClampedV, aDomain.VMin, aDomain.VMax);
      }

      const gp_Pnt aSurfPt = Value(aClampedU, aClampedV);
      const double aSqDist = theP.SquareDistance(aSurfPt);

      // Check for duplicates
      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        const ExtremaPS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
        if (std::abs(anExisting.U - aClampedU) < theTol
            && std::abs(anExisting.V - aClampedV) < theTol)
          return;
        if (anExisting.Point.SquareDistance(aSurfPt) < theTol * theTol)
          return;
      }

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aClampedU;
      anExt.V              = aClampedV;
      anExt.Point          = aSurfPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      myResult.Extrema.Append(anExt);
    };

    if (theMode != ExtremaPS::SearchMode::Max)
    {
      addExtremum(aU, aV, true);
    }

    if (theMode != ExtremaPS::SearchMode::Min)
    {
      addExtremum(aU, aVOpp, false);
      addExtremum(aUOpp, aV, false);
      addExtremum(aUOpp, aVOpp, false);
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

    // If infinite solutions, return immediately
    if (myResult.IsInfinite())
    {
      return myResult;
    }

    // Natural domain (full torus) - no boundary extrema needed
    if (!myDomain.has_value())
    {
      return myResult;
    }

    // Check if boundary extrema are needed
    constexpr double           aTwoPi   = ExtremaPS::THE_TWO_PI;
    const ExtremaPS::Domain2D& aDomain  = *myDomain;
    const bool                 aIsFullU = aDomain.IsUFullPeriod(aTwoPi, theTol);
    const bool                 aIsFullV = aDomain.IsVFullPeriod(aTwoPi, theTol);

    // Add boundary if not full domain
    if (!aIsFullU || !aIsFullV)
    {
      ExtremaPS::AddBoundaryExtrema(myResult, theP, aDomain, *this, theTol, theMode);
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

  //! Returns the torus geometry.
  const gp_Torus& Torus() const { return myTorus; }

  //! Returns true if domain is bounded (not natural full torus).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  gp_Torus                           myTorus;  //!< Torus geometry
  std::optional<ExtremaPS::Domain2D> myDomain; //!< Parameter domain (nullopt for full torus)
  mutable ExtremaPS::Result          myResult; //!< Reusable result storage

  // Cached components for fast computation
  double myCenterX, myCenterY, myCenterZ; //!< Torus center
  double myAxisX, myAxisY, myAxisZ;       //!< Torus axis (Z direction)
  double myXDirX, myXDirY, myXDirZ;       //!< Torus X direction
  double myYDirX, myYDirY, myYDirZ;       //!< Torus Y direction
  double myMajorRadius;                   //!< Major radius
  double myMinorRadius;                   //!< Minor radius
};

#endif // _ExtremaPS_Torus_HeaderFile
