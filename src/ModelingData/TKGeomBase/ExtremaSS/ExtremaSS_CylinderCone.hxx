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

#ifndef _ExtremaSS_CylinderCone_HeaderFile
#define _ExtremaSS_CylinderCone_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <MathOpt_Brent.hxx>
#include <MathOpt_Powell.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! Analytical computation of extrema between a cylinder and a cone.
//!
//! The algorithm handles several cases based on axis relationship:
//!
//! **Case 1: Parallel axes**
//! The distance function depends on V_cone (which determines cone radius).
//! For each V_cone, we have a cylinder-circle problem.
//! Extrema occur where the derivative with respect to V_cone is zero.
//!
//! **Case 2: Intersecting axes**
//! The axes meet at a point. The problem reduces to finding the optimal
//! V_cone value where the cone circle at that V has optimal distance to
//! the cylinder.
//!
//! **Case 3: Skew axes**
//! Most general case. Need to search over V_cone to find optimal positions
//! where the derivative of distance with respect to all parameters is zero.
//!
//! **Parameterization:**
//! - Cylinder: P(U, V) = Center + R*(cos(U)*XDir + sin(U)*YDir) + V*Axis
//! - Cone: P(U, V) = Apex + V*cos(alpha)*Axis + V*sin(alpha)*(cos(U)*XDir + sin(U)*YDir)
//!   where alpha is the semi-angle
class ExtremaSS_CylinderCone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from cylinder and cone.
  //! @param theCylinder The cylinder
  //! @param theCone The cone
  ExtremaSS_CylinderCone(const gp_Cylinder& theCylinder, const gp_Cone& theCone)
      : myCylinder(theCylinder),
        myCone(theCone),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor from cone and cylinder (swapped order).
  //! @param theCone The cone
  //! @param theCylinder The cylinder
  ExtremaSS_CylinderCone(const gp_Cone& theCone, const gp_Cylinder& theCylinder)
      : myCylinder(theCylinder),
        myCone(theCone),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with bounded domain.
  //! @param theCylinder The cylinder
  //! @param theCone The cone
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_CylinderCone(const gp_Cylinder&         theCylinder,
                         const gp_Cone&             theCone,
                         const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        myCone(theCone),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with bounded domain (swapped order).
  //! @param theCone The cone
  //! @param theCylinder The cylinder
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_CylinderCone(const gp_Cone&             theCone,
                         const gp_Cylinder&         theCylinder,
                         const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        myCone(theCone),
        myDomain(ExtremaSS::Domain4D{theDomain.Domain2, theDomain.Domain1}),
        mySwapped(true)
  {
    initCache();
  }

  //! Perform extrema computation.
  //! @param theTol Tolerance for considering points as equal
  //! @param theMode Search mode (Min, Max, or MinMax)
  //! @return Result containing extremum points and distances
  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    if (myDomain.has_value())
    {
      return PerformWithBoundary(theTol, theMode);
    }

    if (myAxesParallel)
    {
      computeParallelCase(theTol, theMode);
    }
    else
    {
      computeSkewCase(theTol, theMode);
    }

    return myResult;
  }

  //! Perform extrema computation including boundary analysis.
  //! @param theTol Tolerance for considering points as equal
  //! @param theMode Search mode (Min, Max, or MinMax)
  //! @return Result containing extremum points and distances
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    if (myAxesParallel)
    {
      computeParallelCase(theTol, theMode);
    }
    else
    {
      computeSkewCase(theTol, theMode);
    }

    if (myDomain.has_value())
    {
      checkBoundaryExtrema(theTol, theMode);
    }

    return myResult;
  }

  //! Evaluate point on cylinder (surface 1).
  //! @param theU U parameter (angle)
  //! @param theV V parameter (height)
  //! @return Point on cylinder
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(myCylCenter.X() + myCylRadius * (aCosU * myCylXDir.X() + aSinU * myCylYDir.X())
                    + theV * myCylAxis.X(),
                  myCylCenter.Y() + myCylRadius * (aCosU * myCylXDir.Y() + aSinU * myCylYDir.Y())
                    + theV * myCylAxis.Y(),
                  myCylCenter.Z() + myCylRadius * (aCosU * myCylXDir.Z() + aSinU * myCylYDir.Z())
                    + theV * myCylAxis.Z());
  }

  //! Evaluate point on cone (surface 2).
  //! @param theU U parameter (angle)
  //! @param theV V parameter (distance from apex along generator)
  //! @return Point on cone
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = theV * mySinSemiAngle;
    const double aHeight = theV * myCosSemiAngle;
    return gp_Pnt(myConeApex.X() + aHeight * myConeAxis.X()
                    + aRadius * (aCosU * myConeXDir.X() + aSinU * myConeYDir.X()),
                  myConeApex.Y() + aHeight * myConeAxis.Y()
                    + aRadius * (aCosU * myConeXDir.Y() + aSinU * myConeYDir.Y()),
                  myConeApex.Z() + aHeight * myConeAxis.Z()
                    + aRadius * (aCosU * myConeXDir.Z() + aSinU * myConeYDir.Z()));
  }

  //! Check if surfaces were swapped.
  //! @return True if cone was provided first
  bool IsSwapped() const { return mySwapped; }

  //! Check if domain is bounded.
  //! @return True if bounded domain was provided
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Initialize cached values from geometry.
  void initCache()
  {
    // Extract cylinder data
    const gp_Ax3& aCylAx = myCylinder.Position();
    myCylCenter          = aCylAx.Location();
    myCylAxis            = aCylAx.Direction();
    myCylXDir            = aCylAx.XDirection();
    myCylYDir            = aCylAx.YDirection();
    myCylRadius          = myCylinder.Radius();

    // Extract cone data
    const gp_Ax3& aConeAx = myCone.Position();
    myConeApex            = myCone.Apex();
    myConeAxis            = aConeAx.Direction();
    myConeXDir            = aConeAx.XDirection();
    myConeYDir            = aConeAx.YDirection();
    mySemiAngle           = myCone.SemiAngle();
    mySinSemiAngle        = std::sin(mySemiAngle);
    myCosSemiAngle        = std::cos(mySemiAngle);
    myConeRefRadius       = myCone.RefRadius();

    // Check if axes are parallel using gp_Vec to avoid gp_Dir exception for parallel vectors
    const gp_Vec aCrossVec = gp_Vec(myCylAxis).Crossed(gp_Vec(myConeAxis));
    const double aCrossMag = aCrossVec.Magnitude();
    myAxesParallel         = (aCrossMag < ExtremaSS::THE_ANGULAR_TOLERANCE);

    if (!myAxesParallel)
    {
      myCrossProduct =
        gp_Dir(aCrossVec.X() / aCrossMag, aCrossVec.Y() / aCrossMag, aCrossVec.Z() / aCrossMag);
    }

    // Vector from cylinder center to cone apex
    myDeltaCenter = gp_Vec(myConeApex.X() - myCylCenter.X(),
                           myConeApex.Y() - myCylCenter.Y(),
                           myConeApex.Z() - myCylCenter.Z());
  }

  //! Compute extrema for parallel axes case.
  void computeParallelCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Project cone apex onto plane perpendicular to cylinder axis through cylinder center
    const double aDotAxis = myDeltaCenter.Dot(gp_Vec(myCylAxis));
    const gp_Vec aPerp(-myDeltaCenter.X() + aDotAxis * myCylAxis.X(),
                       -myDeltaCenter.Y() + aDotAxis * myCylAxis.Y(),
                       -myDeltaCenter.Z() + aDotAxis * myCylAxis.Z());

    const double aAxisDist = aPerp.Magnitude();

    // Direction from cone axis toward cylinder axis (in perpendicular plane)
    gp_Dir aDirToCyl;
    if (aAxisDist > theTol)
    {
      aDirToCyl = gp_Dir(aPerp.X() / aAxisDist, aPerp.Y() / aAxisDist, aPerp.Z() / aAxisDist);
    }
    else
    {
      // Coaxial: use arbitrary direction
      aDirToCyl = myCylXDir;
    }

    // For parallel axes, at each V_cone, the cone has radius R_cone = V_cone * sin(alpha)
    // The distance between surfaces at optimal U values is:
    // d_min = |aAxisDist - R_cyl - R_cone| = |aAxisDist - R_cyl - V*sin(alpha)|
    // d_max = aAxisDist + R_cyl + R_cone

    // For minimum: we want R_cone = aAxisDist - R_cyl (if positive)
    // V_cone = (aAxisDist - R_cyl) / sin(alpha)

    // For maximum: larger V gives larger cone radius, but we need bounded cone

    // Check sign of dot product between axes (same or opposite direction)
    const double aAxisDot = myCylAxis.X() * myConeAxis.X() + myCylAxis.Y() * myConeAxis.Y()
                            + myCylAxis.Z() * myConeAxis.Z();

    if (aAxisDist < theTol)
    {
      // Coaxial case
      computeCoaxialCase(theTol, theMode);
      return;
    }

    myResult.Status = ExtremaSS::Status::OK;

    // Find U_cone where cone points toward cylinder axis
    const double aDotConeX = aDirToCyl.X() * myConeXDir.X() + aDirToCyl.Y() * myConeXDir.Y()
                             + aDirToCyl.Z() * myConeXDir.Z();
    const double aDotConeY = aDirToCyl.X() * myConeYDir.X() + aDirToCyl.Y() * myConeYDir.Y()
                             + aDirToCyl.Z() * myConeYDir.Z();
    const double aUConeToward = std::atan2(aDotConeY, aDotConeX);
    const double aUConeAway   = aUConeToward + M_PI;

    // Find U_cyl where cylinder points toward cone axis
    const gp_Dir aDirFromCyl(-aDirToCyl.X(), -aDirToCyl.Y(), -aDirToCyl.Z());
    const double aDotCylX = aDirFromCyl.X() * myCylXDir.X() + aDirFromCyl.Y() * myCylXDir.Y()
                            + aDirFromCyl.Z() * myCylXDir.Z();
    const double aDotCylY = aDirFromCyl.X() * myCylYDir.X() + aDirFromCyl.Y() * myCylYDir.Y()
                            + aDirFromCyl.Z() * myCylYDir.Z();
    const double aUCylToward = std::atan2(aDotCylY, aDotCylX);
    const double aUCylAway   = aUCylToward + M_PI;

    // For minimum distance with parallel axes:
    // Distance = |aAxisDist - R_cyl - V*sin(alpha)| where V is cone parameter
    // Optimal when R_cone = aAxisDist - R_cyl => V_opt = (aAxisDist - R_cyl) / sin(alpha)

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      if (std::abs(mySinSemiAngle) > theTol)
      {
        const double aVOpt = (aAxisDist - myCylRadius) / mySinSemiAngle;

        if (aVOpt > theTol) // Valid cone parameter (positive side)
        {
          // Find corresponding V_cyl
          const double aVCyl = -aDotAxis + aVOpt * myCosSemiAngle * aAxisDot;

          // Distance should be approximately zero
          addExtremum(aUCylToward, aVCyl, aUConeToward, aVOpt, 0.0, true, theTol);
        }
        else if (aVOpt < -theTol) // Other side of cone
        {
          const double aVCyl = -aDotAxis + aVOpt * myCosSemiAngle * aAxisDot;
          addExtremum(aUCylAway, aVCyl, aUConeAway, aVOpt, 0.0, true, theTol);
        }
        else
        {
          // Optimal at apex
          const double aMinDist = aAxisDist - myCylRadius;
          const double aVCyl    = -aDotAxis;
          addExtremum(aUCylToward, aVCyl, 0.0, 0.0, aMinDist * aMinDist, true, theTol);
        }
      }
      else
      {
        // Degenerate cone (cylinder-like)
        const double aMinDist = std::abs(aAxisDist - myCylRadius - myConeRefRadius);
        addExtremum(aUCylToward, 0.0, aUConeToward, 1.0, aMinDist * aMinDist, true, theTol);
      }
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum distance: at large V values, distance grows
      // For unbounded domain, no maximum exists
      // For bounded domain, check at boundary V values
      if (myDomain.has_value())
      {
        // Check at V_cone boundaries
        const double aVConeMin = myDomain->Domain2.VMin;
        const double aVConeMax = myDomain->Domain2.VMax;

        for (double aVCone : {aVConeMin, aVConeMax})
        {
          const double aRCone   = std::abs(aVCone * mySinSemiAngle);
          const double aMaxDist = aAxisDist + myCylRadius + aRCone;
          const double aVCyl    = -aDotAxis + aVCone * myCosSemiAngle * aAxisDot;

          addExtremum(aUCylAway, aVCyl, aUConeAway, aVCone, aMaxDist * aMaxDist, false, theTol);
        }
      }
      else
      {
        // Report a representative maximum at arbitrary large V
        const double aVCone   = 100.0; // Arbitrary for infinite surface
        const double aRCone   = std::abs(aVCone * mySinSemiAngle);
        const double aMaxDist = aAxisDist + myCylRadius + aRCone;
        const double aVCyl    = -aDotAxis + aVCone * myCosSemiAngle * aAxisDot;

        addExtremum(aUCylAway, aVCyl, aUConeAway, aVCone, aMaxDist * aMaxDist, false, theTol);
      }
    }
  }

  //! Compute extrema when axes are coaxial.
  void computeCoaxialCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // For coaxial cylinder and cone, extrema depend on cone radius vs cylinder radius
    // At V where R_cone = R_cyl, surfaces touch

    if (std::abs(mySinSemiAngle) > theTol)
    {
      const double aVTouch = myCylRadius / mySinSemiAngle;

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        // Minimum distance is 0 where radii match
        addExtremum(0.0, 0.0, 0.0, aVTouch, 0.0, true, theTol);
      }

      if (theMode != ExtremaSS::SearchMode::Min)
      {
        // Maximum is at opposite radial directions
        const double aMaxDist = myCylRadius + std::abs(aVTouch * mySinSemiAngle);
        addExtremum(0.0, 0.0, M_PI, aVTouch, aMaxDist * aMaxDist, false, theTol);
      }
    }
    else
    {
      // Degenerate cone with zero semi-angle (cylinder)
      const double aDist = std::abs(myCylRadius - myConeRefRadius);
      if (theMode != ExtremaSS::SearchMode::Max)
      {
        addExtremum(0.0, 0.0, 0.0, 1.0, aDist * aDist, true, theTol);
      }
      if (theMode != ExtremaSS::SearchMode::Min)
      {
        const double aMaxDist = myCylRadius + myConeRefRadius;
        addExtremum(0.0, 0.0, M_PI, 1.0, aMaxDist * aMaxDist, false, theTol);
      }
    }
  }

  //! Compute extrema for skew (non-parallel) axes case.
  void computeSkewCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // For skew axes, we need to find the closest approach considering the cone's varying radius

    // First, find the closest points on the two axes
    const double aA1DotA1 = 1.0;
    const double aA2DotA2 = 1.0;
    const double aA1DotA2 = myCylAxis.X() * myConeAxis.X() + myCylAxis.Y() * myConeAxis.Y()
                            + myCylAxis.Z() * myConeAxis.Z();
    const double aDeltaDotA1 = -myDeltaCenter.Dot(gp_Vec(myCylAxis));
    const double aDeltaDotA2 = -myDeltaCenter.Dot(gp_Vec(myConeAxis));

    const double aDenom = aA1DotA1 * aA2DotA2 - aA1DotA2 * aA1DotA2;

    double aT = 0.0; // Parameter on cylinder axis
    double aS = 0.0; // Parameter on cone axis (V parameter)

    if (std::abs(aDenom) > ExtremaSS::THE_ANGULAR_TOLERANCE * ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      aT = (aDeltaDotA1 * aA2DotA2 - aDeltaDotA2 * aA1DotA2) / aDenom;
      aS = (aDeltaDotA2 * aA1DotA1 - aDeltaDotA1 * aA1DotA2) / aDenom;
    }

    // Points on axes
    const gp_Pnt aPOnCylAxis(myCylCenter.X() + aT * myCylAxis.X(),
                             myCylCenter.Y() + aT * myCylAxis.Y(),
                             myCylCenter.Z() + aT * myCylAxis.Z());

    // For cone, S is distance along axis from apex, need to convert to V parameter
    const double aVCone = (std::abs(myCosSemiAngle) > theTol) ? aS / myCosSemiAngle : aS;

    const gp_Pnt aPOnConeAxis(myConeApex.X() + aS * myConeAxis.X(),
                              myConeApex.Y() + aS * myConeAxis.Y(),
                              myConeApex.Z() + aS * myConeAxis.Z());

    // Direction between closest points on axes
    gp_Vec aDirBetween(aPOnConeAxis.X() - aPOnCylAxis.X(),
                       aPOnConeAxis.Y() - aPOnCylAxis.Y(),
                       aPOnConeAxis.Z() - aPOnCylAxis.Z());

    const double aAxisDist = aDirBetween.Magnitude();

    myResult.Status = ExtremaSS::Status::OK;

    if (aAxisDist < theTol)
    {
      // Axes intersect
      computeIntersectingAxesCase(aPOnCylAxis, aT, theTol, theMode);
      return;
    }

    aDirBetween.Divide(aAxisDist);

    // Project direction onto each surface's perpendicular plane
    // For cylinder
    const double aDirDotCylAxis = aDirBetween.Dot(gp_Vec(myCylAxis));
    gp_Vec       aDirCylPerp    = aDirBetween - gp_Vec(myCylAxis) * aDirDotCylAxis;
    const double aCylPerpMag    = aDirCylPerp.Magnitude();

    // For cone at the current V
    const double aDirDotConeAxis = aDirBetween.Dot(gp_Vec(myConeAxis));
    gp_Vec       aDirConePerp    = aDirBetween - gp_Vec(myConeAxis) * aDirDotConeAxis;
    const double aConePerpMag    = aDirConePerp.Magnitude();

    // Cone radius at this V
    const double aRCone = std::abs(aVCone * mySinSemiAngle);

    // Find optimal U angles
    double aUCyl  = 0.0;
    double aUCone = 0.0;

    if (aCylPerpMag > theTol)
    {
      aDirCylPerp.Divide(aCylPerpMag);
      const double aX = aDirCylPerp.Dot(gp_Vec(myCylXDir));
      const double aY = aDirCylPerp.Dot(gp_Vec(myCylYDir));
      aUCyl           = std::atan2(aY, aX);
    }

    if (aConePerpMag > theTol)
    {
      aDirConePerp.Divide(aConePerpMag);
      const double aX = aDirConePerp.Dot(gp_Vec(myConeXDir));
      const double aY = aDirConePerp.Dot(gp_Vec(myConeYDir));
      aUCone          = std::atan2(aY, aX);
    }

    // Compute projections for distance calculation
    const double aCylProj  = myCylRadius * aCylPerpMag;
    const double aConeProj = aRCone * aConePerpMag;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Minimum: surfaces pointing toward each other
      const double aMinDist = std::max(0.0, aAxisDist - aCylProj - aConeProj);
      addExtremum(aUCyl + M_PI, aT, aUCone, aVCone, aMinDist * aMinDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum: surfaces pointing away
      const double aMaxDist = aAxisDist + aCylProj + aConeProj;
      addExtremum(aUCyl, aT, aUCone + M_PI, aVCone, aMaxDist * aMaxDist, false, theTol);
    }

    // Also search for other extrema by varying V_cone
    searchAlongConeAxis(theTol, theMode);
  }

  //! Compute extrema when axes intersect.
  void computeIntersectingAxesCase(const gp_Pnt&         theIntersection,
                                   double                theVCyl,
                                   double                theTol,
                                   ExtremaSS::SearchMode theMode) const
  {
    // Find V_cone for intersection point
    const gp_Vec aVecFromApex(myConeApex, theIntersection);
    const double aDotAxis = aVecFromApex.Dot(gp_Vec(myConeAxis));
    double       aVCone =
      (std::abs(myCosSemiAngle) > theTol) ? aDotAxis / myCosSemiAngle : aVecFromApex.Magnitude();

    // For single-nappe cones (RefRadius = 0), V should be >= 0
    // If V < 0, the intersection is on the "wrong" side of the apex
    // In this case, search for where surfaces might actually intersect
    const bool aValidConeV = (myConeRefRadius > theTol) || (aVCone >= -theTol);

    if (!aValidConeV)
    {
      // The axis intersection is outside the valid cone surface
      // Search for where the cone surface might reach the cylinder
      searchIntersectingAxesSurfaceContact(theVCyl, theTol, theMode);
      return;
    }

    // Cross product gives direction perpendicular to both axes
    const gp_Dir& aCrossDir = myCrossProduct;

    // Project onto cylinder's perpendicular plane
    const double aCylX  = aCrossDir.Dot(gp_Dir(myCylXDir));
    const double aCylY  = aCrossDir.Dot(gp_Dir(myCylYDir));
    const double aUCyl1 = std::atan2(aCylY, aCylX);
    const double aUCyl2 = aUCyl1 + M_PI;

    // Project onto cone's perpendicular plane
    const double aConeX  = aCrossDir.Dot(gp_Dir(myConeXDir));
    const double aConeY  = aCrossDir.Dot(gp_Dir(myConeYDir));
    const double aUCone1 = std::atan2(aConeY, aConeX);
    const double aUCone2 = aUCone1 + M_PI;

    myResult.Status = ExtremaSS::Status::OK;

    // Four combinations
    for (int i = 0; i < 2; ++i)
    {
      const double aUCyl = (i == 0) ? aUCyl1 : aUCyl2;

      for (int j = 0; j < 2; ++j)
      {
        const double aUCone = (j == 0) ? aUCone1 : aUCone2;

        const gp_Pnt aP1     = Value1(aUCyl, theVCyl);
        const gp_Pnt aP2     = Value2(aUCone, aVCone);
        const double aSqDist = aP1.SquareDistance(aP2);

        const bool aIsMin = (i == j);

        if ((aIsMin && theMode != ExtremaSS::SearchMode::Max)
            || (!aIsMin && theMode != ExtremaSS::SearchMode::Min))
        {
          addExtremum(aUCyl, theVCyl, aUCone, aVCone, aSqDist, aIsMin, theTol);
        }
      }
    }
  }

  //! Search for surface contact when axes intersect but axis intersection is outside cone surface.
  void searchIntersectingAxesSurfaceContact(double                theVCyl,
                                            double                theTol,
                                            ExtremaSS::SearchMode theMode) const
  {
    (void)theVCyl; // Not used directly; we search over cone V
    myResult.Status = ExtremaSS::Status::OK;

    // The axes intersect but the intersection point is at a negative V on the cone
    // (i.e., the cylinder axis passes through the region "behind" the cone apex)

    // First, always check the apex (V = 0) as a candidate for minimum
    const gp_Vec aVecFromCyl(myCylCenter, myConeApex);
    const double aVCylApex = aVecFromCyl.Dot(gp_Vec(myCylAxis));

    // Point on cylinder axis closest to apex
    const gp_Pnt aPOnCylAxis(myCylCenter.X() + aVCylApex * myCylAxis.X(),
                             myCylCenter.Y() + aVCylApex * myCylAxis.Y(),
                             myCylCenter.Z() + aVCylApex * myCylAxis.Z());

    // Direction from cylinder axis to apex
    gp_Vec aDirToApex(aPOnCylAxis, myConeApex);
    aDirToApex = aDirToApex - gp_Vec(myCylAxis) * aDirToApex.Dot(gp_Vec(myCylAxis));
    const double aApexDistToCylAxis = aDirToApex.Magnitude();

    // Distance from apex to cylinder surface
    const double aApexToCylSurf = std::abs(aApexDistToCylAxis - myCylRadius);

    double aBestMinSqDist = aApexToCylSurf * aApexToCylSurf;
    double aBestMinU1 = 0, aBestMinV1 = aVCylApex, aBestMinU2 = 0, aBestMinV2 = 0;

    if (aApexDistToCylAxis > theTol)
    {
      aDirToApex.Divide(aApexDistToCylAxis);
      const double aApexCylX = aDirToApex.Dot(gp_Vec(myCylXDir));
      const double aApexCylY = aDirToApex.Dot(gp_Vec(myCylYDir));
      aBestMinU1             = std::atan2(aApexCylY, aApexCylX);
    }

    // Functor to compute squared error: (dist_to_cyl_axis - R_cyl)^2
    // This is minimized when the cone surface intersects the cylinder.
    struct ConeDistFunc
    {
      const ExtremaSS_CylinderCone* myEval;
      double                        myUCone;

      ConeDistFunc(const ExtremaSS_CylinderCone* theEval, double theUCone)
          : myEval(theEval),
            myUCone(theUCone)
      {
      }

      bool Value(double theV, double& theF) const
      {
        if (theV < 0.0)
        {
          theF = std::numeric_limits<double>::max();
          return true;
        }

        const gp_Pnt aPCone = myEval->Value2(myUCone, theV);

        // Project cone point onto cylinder axis
        const gp_Vec aVecFromCyl(myEval->myCylCenter, aPCone);
        const double aVCyl = aVecFromCyl.Dot(gp_Vec(myEval->myCylAxis));

        const gp_Pnt aPOnCylAxis(myEval->myCylCenter.X() + aVCyl * myEval->myCylAxis.X(),
                                 myEval->myCylCenter.Y() + aVCyl * myEval->myCylAxis.Y(),
                                 myEval->myCylCenter.Z() + aVCyl * myEval->myCylAxis.Z());

        // Perpendicular distance from cone point to cylinder axis
        gp_Vec aPerpVec(aPOnCylAxis, aPCone);
        aPerpVec = aPerpVec - gp_Vec(myEval->myCylAxis) * aPerpVec.Dot(gp_Vec(myEval->myCylAxis));
        const double aDistAxis = aPerpVec.Magnitude();

        // Squared error from desired cylinder radius
        const double aError = aDistAxis - myEval->myCylRadius;
        theF                = aError * aError;
        return true;
      }
    };

    // For each U on the cone, use Brent's method to find V where the cone surface
    // reaches distance R_cyl from the cylinder axis (i.e., intersects the cylinder)
    constexpr int aNbUSamples = 72;

    MathUtils::Config aConfig(theTol, 100);

    for (int iU = 0; iU < aNbUSamples; ++iU)
    {
      const double aUCone = 2.0 * M_PI * iU / aNbUSamples;

      ConeDistFunc aFunc(this, aUCone);

      // Search in V range [0, 100] for intersection
      MathUtils::ScalarResult aOptResult = MathOpt::Brent(aFunc, 0.0, 100.0, aConfig);

      if (aOptResult.IsDone() && aOptResult.Value.has_value() && aOptResult.Root.has_value())
      {
        const double aMinError = *aOptResult.Value;
        const double aVCone    = *aOptResult.Root;

        // Check if we found an intersection (error close to zero)
        if (aMinError < theTol * theTol && aVCone >= 0.0)
        {
          const gp_Pnt aPCone = Value2(aUCone, aVCone);

          // Compute cylinder point at this location
          const gp_Vec aVecFromCylPt(myCylCenter, aPCone);
          const double aVCyl = aVecFromCylPt.Dot(gp_Vec(myCylAxis));

          const gp_Pnt aPOnCylAxisPt(myCylCenter.X() + aVCyl * myCylAxis.X(),
                                     myCylCenter.Y() + aVCyl * myCylAxis.Y(),
                                     myCylCenter.Z() + aVCyl * myCylAxis.Z());

          gp_Vec aPerpVec(aPOnCylAxisPt, aPCone);
          aPerpVec = aPerpVec - gp_Vec(myCylAxis) * aPerpVec.Dot(gp_Vec(myCylAxis));
          const double aDistToCylAxis = aPerpVec.Magnitude();

          if (aDistToCylAxis > theTol)
          {
            aPerpVec.Divide(aDistToCylAxis);
            const double aCylX = aPerpVec.Dot(gp_Vec(myCylXDir));
            const double aCylY = aPerpVec.Dot(gp_Vec(myCylYDir));
            const double aUCyl = std::atan2(aCylY, aCylX);

            const gp_Pnt aPCyl   = Value1(aUCyl, aVCyl);
            const double aSqDist = aPCyl.SquareDistance(aPCone);

            if (aSqDist < aBestMinSqDist)
            {
              aBestMinSqDist = aSqDist;
              aBestMinU1     = aUCyl;
              aBestMinV1     = aVCyl;
              aBestMinU2     = aUCone;
              aBestMinV2     = aVCone;
            }
          }
        }
      }
    }

    // Add the best minimum found
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      if (aBestMinSqDist < theTol * theTol)
      {
        // Exact intersection found
        addExtremum(aBestMinU1, aBestMinV1, aBestMinU2, aBestMinV2, 0.0, true, theTol);
      }
      else if (aBestMinV2 < theTol)
      {
        // Apex is closest
        addExtremum(aBestMinU1, aBestMinV1, 0.0, 0.0, aBestMinSqDist, true, theTol);
      }
      else
      {
        // Refine non-apex solutions
        refineExtremum(aBestMinU1, aBestMinV1, aBestMinU2, aBestMinV2, true, theTol);
      }
    }
  }

  //! Search for extrema by sampling along cone axis.
  void searchAlongConeAxis(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Sample V_cone values to find additional extrema
    constexpr int    aNbSamples = 50;
    constexpr double aVMin      = -100.0;
    constexpr double aVMax      = 100.0;

    // Compute tan(semi-angle) for correct OCCT radius formula
    const double aTanSemiAngle = (std::abs(myCosSemiAngle) > ExtremaSS::THE_ANGULAR_TOLERANCE)
                                   ? mySinSemiAngle / myCosSemiAngle
                                   : 1e10;

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 0, aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU1 = 0, aBestMaxV1 = 0, aBestMaxU2 = 0, aBestMaxV2 = 0;

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aVCone = aVMin + i * (aVMax - aVMin) / aNbSamples;
      // OCCT cone radius formula: RefRadius + V * tan(SemiAngle)
      const double aRCone = myConeRefRadius + std::abs(aVCone) * aTanSemiAngle;

      // Point on cone axis at this V
      const double aHeight = aVCone * myCosSemiAngle;
      const gp_Pnt aPOnConeAxis(myConeApex.X() + aHeight * myConeAxis.X(),
                                myConeApex.Y() + aHeight * myConeAxis.Y(),
                                myConeApex.Z() + aHeight * myConeAxis.Z());

      // Find closest point on cylinder axis to this point
      const gp_Vec aVecToConeAxis(myCylCenter, aPOnConeAxis);
      const double aVCyl = aVecToConeAxis.Dot(gp_Vec(myCylAxis));

      const gp_Pnt aPOnCylAxis(myCylCenter.X() + aVCyl * myCylAxis.X(),
                               myCylCenter.Y() + aVCyl * myCylAxis.Y(),
                               myCylCenter.Z() + aVCyl * myCylAxis.Z());

      // Direction between axis points
      gp_Vec aDir(aPOnConeAxis.X() - aPOnCylAxis.X(),
                  aPOnConeAxis.Y() - aPOnCylAxis.Y(),
                  aPOnConeAxis.Z() - aPOnCylAxis.Z());

      const double aDist = aDir.Magnitude();
      if (aDist < theTol)
        continue;

      aDir.Divide(aDist);

      // Project onto perpendicular planes
      gp_Vec aDirCylPerp  = aDir - gp_Vec(myCylAxis) * aDir.Dot(gp_Vec(myCylAxis));
      gp_Vec aDirConePerp = aDir - gp_Vec(myConeAxis) * aDir.Dot(gp_Vec(myConeAxis));

      const double aCylPerpMag  = aDirCylPerp.Magnitude();
      const double aConePerpMag = aDirConePerp.Magnitude();

      double aUCylMin = 0, aUConeMin = 0;
      double aUCylMax = 0, aUConeMax = 0;

      if (aCylPerpMag > theTol)
      {
        aDirCylPerp.Divide(aCylPerpMag);
        const double aX = aDirCylPerp.Dot(gp_Vec(myCylXDir));
        const double aY = aDirCylPerp.Dot(gp_Vec(myCylYDir));
        aUCylMin        = std::atan2(aY, aX) + M_PI; // Toward cone
        aUCylMax        = aUCylMin + M_PI;           // Away from cone
      }

      if (aConePerpMag > theTol)
      {
        aDirConePerp.Divide(aConePerpMag);
        const double aX = aDirConePerp.Dot(gp_Vec(myConeXDir));
        const double aY = aDirConePerp.Dot(gp_Vec(myConeYDir));
        aUConeMin       = std::atan2(aY, aX) + M_PI; // Toward cylinder
        aUConeMax       = aUConeMin + M_PI;          // Away from cylinder
      }

      const double aCylProj  = myCylRadius * aCylPerpMag;
      const double aConeProj = aRCone * aConePerpMag;

      const double aMinDist   = std::max(0.0, aDist - aCylProj - aConeProj);
      const double aMaxDist   = aDist + aCylProj + aConeProj;
      const double aMinSqDist = aMinDist * aMinDist;
      const double aMaxSqDist = aMaxDist * aMaxDist;

      if (aMinSqDist < aBestMinSqDist)
      {
        aBestMinSqDist = aMinSqDist;
        aBestMinU1     = aUCylMin;
        aBestMinV1     = aVCyl;
        aBestMinU2     = aUConeMin;
        aBestMinV2     = aVCone;
      }

      if (aMaxSqDist > aBestMaxSqDist)
      {
        aBestMaxSqDist = aMaxSqDist;
        aBestMaxU1     = aUCylMax;
        aBestMaxV1     = aVCyl;
        aBestMaxU2     = aUConeMax;
        aBestMaxV2     = aVCone;
      }
    }

    // Refine the found extrema
    if (theMode != ExtremaSS::SearchMode::Max
        && aBestMinSqDist < std::numeric_limits<double>::max())
    {
      refineExtremum(aBestMinU1, aBestMinV1, aBestMinU2, aBestMinV2, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min && aBestMaxSqDist > 0.0)
    {
      refineExtremum(aBestMaxU1, aBestMaxV1, aBestMaxU2, aBestMaxV2, false, theTol);
    }
  }

  //! Refine an extremum using Powell's method (gradient-free optimization).
  void refineExtremum(double theU1,
                      double theV1,
                      double theU2,
                      double theV2,
                      bool   theIsMin,
                      double theTol) const
  {
    // Functor for squared distance between surfaces
    // For maximum, we minimize -distance^2
    struct DistanceFunc
    {
      const ExtremaSS_CylinderCone* myEval;
      bool                          myIsMin;

      DistanceFunc(const ExtremaSS_CylinderCone* theEval, bool theIsMin)
          : myEval(theEval),
            myIsMin(theIsMin)
      {
      }

      bool Value(const math_Vector& theX, double& theF) const
      {
        const double aU1 = theX(1);
        const double aV1 = theX(2);
        const double aU2 = theX(3);
        const double aV2 = theX(4);

        const gp_Pnt aP1     = myEval->Value1(aU1, aV1);
        const gp_Pnt aP2     = myEval->Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        // For minimization return distance^2, for maximization return -distance^2
        theF = myIsMin ? aSqDist : -aSqDist;
        return true;
      }
    };

    // Set up starting point
    math_Vector aStartPt(1, 4);
    aStartPt(1) = theU1;
    aStartPt(2) = theV1;
    aStartPt(3) = theU2;
    aStartPt(4) = theV2;

    // Configure Powell's method
    MathUtils::Config aConfig(theTol, 50);

    DistanceFunc aFunc(this, theIsMin);

    // Run Powell optimization
    MathUtils::VectorResult aOptResult = MathOpt::Powell(aFunc, aStartPt, aConfig);

    double aU1 = theU1, aV1 = theV1, aU2 = theU2, aV2 = theV2;
    double aSqDist = 0.0;

    if (aOptResult.IsDone() && aOptResult.Solution.has_value())
    {
      const math_Vector& aSol = *aOptResult.Solution;
      aU1                     = aSol(1);
      aV1                     = aSol(2);
      aU2                     = aSol(3);
      aV2                     = aSol(4);

      const gp_Pnt aP1 = Value1(aU1, aV1);
      const gp_Pnt aP2 = Value2(aU2, aV2);
      aSqDist          = aP1.SquareDistance(aP2);
    }
    else
    {
      // Fall back to input values if optimization failed
      const gp_Pnt aP1 = Value1(theU1, theV1);
      const gp_Pnt aP2 = Value2(theU2, theV2);
      aSqDist          = aP1.SquareDistance(aP2);
    }

    addExtremum(aU1, aV1, aU2, aV2, aSqDist, theIsMin, theTol);
  }

  //! Check boundary extrema for bounded domains.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
      return;

    const ExtremaSS::Domain4D& aDom  = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    constexpr int aNbSamples = 20;

    // Check cylinder boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkPointAgainstCone(aU1, aDom1.VMin, theTol, theMode);
      checkPointAgainstCone(aU1, aDom1.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkPointAgainstCone(aDom1.UMin, aV1, theTol, theMode);
      checkPointAgainstCone(aDom1.UMax, aV1, theTol, theMode);
    }

    // Check cone boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkPointAgainstCylinder(aU2, aDom2.VMin, theTol, theMode);
      checkPointAgainstCylinder(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkPointAgainstCylinder(aDom2.UMin, aV2, theTol, theMode);
      checkPointAgainstCylinder(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on cylinder against cone.
  void checkPointAgainstCone(double                theU1,
                             double                theV1,
                             double                theTol,
                             ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Project point onto cone axis
    const gp_Vec aVecFromApex(myConeApex, aP1);
    const double aDotAxis = aVecFromApex.Dot(gp_Vec(myConeAxis));

    // V parameter on cone
    const double aVCone =
      (std::abs(myCosSemiAngle) > theTol) ? aDotAxis / myCosSemiAngle : aDotAxis;

    // Clamp to domain if bounded
    double aVConeClamped = aVCone;
    if (myDomain.has_value())
    {
      aVConeClamped = std::clamp(aVCone, myDomain->Domain2.VMin, myDomain->Domain2.VMax);
    }

    // Find optimal U on cone
    const double aHeight = aVConeClamped * myCosSemiAngle;
    const gp_Pnt aPOnAxis(myConeApex.X() + aHeight * myConeAxis.X(),
                          myConeApex.Y() + aHeight * myConeAxis.Y(),
                          myConeApex.Z() + aHeight * myConeAxis.Z());

    gp_Vec aDirToP1(aPOnAxis, aP1);
    aDirToP1                 = aDirToP1 - gp_Vec(myConeAxis) * aDirToP1.Dot(gp_Vec(myConeAxis));
    const double aDistToAxis = aDirToP1.Magnitude();

    if (aDistToAxis < theTol)
      return;

    aDirToP1.Divide(aDistToAxis);

    const double aX    = aDirToP1.Dot(gp_Vec(myConeXDir));
    const double aY    = aDirToP1.Dot(gp_Vec(myConeYDir));
    const double aUMin = std::atan2(aY, aX);
    const double aUMax = aUMin + M_PI;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP2     = Value2(aUMin, aVConeClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(theU1, theV1, aUMin, aVConeClamped, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const gp_Pnt aP2     = Value2(aUMax, aVConeClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(theU1, theV1, aUMax, aVConeClamped, aSqDist, false, theTol);
    }
  }

  //! Check a point on cone against cylinder.
  void checkPointAgainstCylinder(double                theU2,
                                 double                theV2,
                                 double                theTol,
                                 ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    // Project point onto cylinder axis
    const gp_Vec aVecFromCenter(myCylCenter, aP2);
    const double aVCyl = aVecFromCenter.Dot(gp_Vec(myCylAxis));

    double aVCylClamped = aVCyl;
    if (myDomain.has_value())
    {
      aVCylClamped = std::clamp(aVCyl, myDomain->Domain1.VMin, myDomain->Domain1.VMax);
    }

    const gp_Pnt aPOnAxis(myCylCenter.X() + aVCylClamped * myCylAxis.X(),
                          myCylCenter.Y() + aVCylClamped * myCylAxis.Y(),
                          myCylCenter.Z() + aVCylClamped * myCylAxis.Z());

    gp_Vec aDirToP2(aPOnAxis, aP2);
    aDirToP2                 = aDirToP2 - gp_Vec(myCylAxis) * aDirToP2.Dot(gp_Vec(myCylAxis));
    const double aDistToAxis = aDirToP2.Magnitude();

    if (aDistToAxis < theTol)
      return;

    aDirToP2.Divide(aDistToAxis);

    const double aX    = aDirToP2.Dot(gp_Vec(myCylXDir));
    const double aY    = aDirToP2.Dot(gp_Vec(myCylYDir));
    const double aUMin = std::atan2(aY, aX);
    const double aUMax = aUMin + M_PI;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP1     = Value1(aUMin, aVCylClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aUMin, aVCylClamped, theU2, theV2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const gp_Pnt aP1     = Value1(aUMax, aVCylClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aUMax, aVCylClamped, theU2, theV2, aSqDist, false, theTol);
    }
  }

  //! Add an extremum to the result, avoiding duplicates.
  void addExtremum(double theU1,
                   double theV1,
                   double theU2,
                   double theV2,
                   double theSqDist,
                   bool   theIsMin,
                   double theTol) const
  {
    const double aU1 = ExtremaSS::NormalizeAngle(theU1);
    const double aU2 = ExtremaSS::NormalizeAngle(theU2);

    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      if (std::abs(anExisting.SquareDistance - theSqDist) < aTolSq)
      {
        const double aDU1 = std::abs(anExisting.U1 - aU1);
        const double aDU2 = std::abs(anExisting.U2 - aU2);
        const double aDV1 = std::abs(anExisting.V1 - theV1);
        const double aDV2 = std::abs(anExisting.V2 - theV2);

        if ((aDU1 < theTol || std::abs(aDU1 - 2 * M_PI) < theTol)
            && (aDU2 < theTol || std::abs(aDU2 - 2 * M_PI) < theTol) && aDV1 < theTol
            && aDV2 < theTol)
        {
          return;
        }
      }
    }

    ExtremaSS::ExtremumResult aResult;

    if (mySwapped)
    {
      aResult.U1     = aU2;
      aResult.V1     = theV2;
      aResult.U2     = aU1;
      aResult.V2     = theV1;
      aResult.Point1 = Value2(aU2, theV2);
      aResult.Point2 = Value1(aU1, theV1);
    }
    else
    {
      aResult.U1     = aU1;
      aResult.V1     = theV1;
      aResult.U2     = aU2;
      aResult.V2     = theV2;
      aResult.Point1 = Value1(aU1, theV1);
      aResult.Point2 = Value2(aU2, theV2);
    }

    aResult.SquareDistance = theSqDist;
    aResult.IsMinimum      = theIsMin;

    myResult.Extrema.Append(aResult);

    if (myResult.Status != ExtremaSS::Status::InfiniteSolutions)
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
  }

private:
  gp_Cylinder myCylinder; //!< The cylinder
  gp_Cone     myCone;     //!< The cone

  std::optional<ExtremaSS::Domain4D> myDomain;  //!< Optional bounded domain
  bool                               mySwapped; //!< True if cone was provided first

  // Cached cylinder geometry
  gp_Pnt myCylCenter; //!< Cylinder center
  gp_Dir myCylAxis;   //!< Cylinder axis
  gp_Dir myCylXDir;   //!< Cylinder X direction
  gp_Dir myCylYDir;   //!< Cylinder Y direction
  double myCylRadius; //!< Cylinder radius

  // Cached cone geometry
  gp_Pnt myConeApex;      //!< Cone apex
  gp_Dir myConeAxis;      //!< Cone axis
  gp_Dir myConeXDir;      //!< Cone X direction
  gp_Dir myConeYDir;      //!< Cone Y direction
  double mySemiAngle;     //!< Cone semi-angle
  double mySinSemiAngle;  //!< sin(semi-angle)
  double myCosSemiAngle;  //!< cos(semi-angle)
  double myConeRefRadius; //!< Cone reference radius

  // Cached relationship
  gp_Dir myCrossProduct; //!< Cross product of axes
  gp_Vec myDeltaCenter;  //!< Vector from cylinder center to cone apex
  bool   myAxesParallel; //!< True if axes are parallel

  mutable ExtremaSS::Result myResult; //!< Computation result
};

#endif // _ExtremaSS_CylinderCone_HeaderFile
