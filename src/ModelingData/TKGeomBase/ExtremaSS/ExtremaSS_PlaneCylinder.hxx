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

#ifndef _ExtremaSS_PlaneCylinder_HeaderFile
#define _ExtremaSS_PlaneCylinder_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Plane-Cylinder extrema computation.
//!
//! Computes the extrema between a plane and a cylindrical surface.
//! This class handles both (Plane, Cylinder) and (Cylinder, Plane) orderings.
//!
//! Algorithm:
//! 1. Compute angle between cylinder axis and plane normal
//! 2. If axis parallel to plane: min = axis-to-plane distance - R
//! 3. If axis perpendicular to plane: two extrema on circle at intersection height
//! 4. General case: extrema along the generator line closest to plane
class ExtremaSS_PlaneCylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane and cylinder (canonical order).
  ExtremaSS_PlaneCylinder(const gp_Pln& thePlane, const gp_Cylinder& theCylinder)
      : myPlane(thePlane),
        myCylinder(theCylinder),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with cylinder and plane (swapped order).
  ExtremaSS_PlaneCylinder(const gp_Cylinder& theCylinder, const gp_Pln& thePlane)
      : myPlane(thePlane),
        myCylinder(theCylinder),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with plane, cylinder, and domain.
  ExtremaSS_PlaneCylinder(const gp_Pln&              thePlane,
                          const gp_Cylinder&         theCylinder,
                          const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myCylinder(theCylinder),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with cylinder, plane, and domain (swapped order).
  ExtremaSS_PlaneCylinder(const gp_Cylinder&         theCylinder,
                          const gp_Pln&              thePlane,
                          const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myCylinder(theCylinder),
        myDomain(ExtremaSS::Domain4D(theDomain.Domain2, theDomain.Domain1)),
        mySwapped(true)
  {
    initCache();
  }

private:
  void initCache()
  {
    // Cache plane components
    const gp_Ax3& aPlanePos  = myPlane.Position();
    const gp_Pnt& aPlaneOrig = aPlanePos.Location();
    myPlaneOrigX             = aPlaneOrig.X();
    myPlaneOrigY             = aPlaneOrig.Y();
    myPlaneOrigZ             = aPlaneOrig.Z();

    const gp_Dir& aPlaneNorm = aPlanePos.Direction();
    myPlaneNormX             = aPlaneNorm.X();
    myPlaneNormY             = aPlaneNorm.Y();
    myPlaneNormZ             = aPlaneNorm.Z();

    const gp_Dir& aPlaneXDir = aPlanePos.XDirection();
    myPlaneXDirX             = aPlaneXDir.X();
    myPlaneXDirY             = aPlaneXDir.Y();
    myPlaneXDirZ             = aPlaneXDir.Z();

    const gp_Dir& aPlaneYDir = aPlanePos.YDirection();
    myPlaneYDirX             = aPlaneYDir.X();
    myPlaneYDirY             = aPlaneYDir.Y();
    myPlaneYDirZ             = aPlaneYDir.Z();

    // Cache cylinder components
    const gp_Ax3& aCylPos  = myCylinder.Position();
    const gp_Pnt& aCylOrig = aCylPos.Location();
    myCylOrigX             = aCylOrig.X();
    myCylOrigY             = aCylOrig.Y();
    myCylOrigZ             = aCylOrig.Z();

    const gp_Dir& aCylAxis = aCylPos.Direction();
    myCylAxisX             = aCylAxis.X();
    myCylAxisY             = aCylAxis.Y();
    myCylAxisZ             = aCylAxis.Z();

    const gp_Dir& aCylXDir = aCylPos.XDirection();
    myCylXDirX             = aCylXDir.X();
    myCylXDirY             = aCylXDir.Y();
    myCylXDirZ             = aCylXDir.Z();

    const gp_Dir& aCylYDir = aCylPos.YDirection();
    myCylYDirX             = aCylYDir.X();
    myCylYDirY             = aCylYDir.Y();
    myCylYDirZ             = aCylYDir.Z();

    myCylRadius = myCylinder.Radius();

    // Compute axis-normal dot product
    myAxisNormDot =
      myCylAxisX * myPlaneNormX + myCylAxisY * myPlaneNormY + myCylAxisZ * myPlaneNormZ;
  }

public:
  gp_Pnt Value1(double theU, double theV) const
  {
    return gp_Pnt(myPlaneOrigX + theU * myPlaneXDirX + theV * myPlaneYDirX,
                  myPlaneOrigY + theU * myPlaneXDirY + theV * myPlaneYDirY,
                  myPlaneOrigZ + theU * myPlaneXDirZ + theV * myPlaneYDirZ);
  }

  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(
      myCylOrigX + myCylRadius * (aCosU * myCylXDirX + aSinU * myCylYDirX) + theV * myCylAxisX,
      myCylOrigY + myCylRadius * (aCosU * myCylXDirY + aSinU * myCylYDirY) + theV * myCylAxisY,
      myCylOrigZ + myCylRadius * (aCosU * myCylXDirZ + aSinU * myCylYDirZ) + theV * myCylAxisZ);
  }

  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    const double aAbsDot   = std::abs(myAxisNormDot);
    const double aAngleTol = ExtremaSS::THE_ANGULAR_TOLERANCE;

    // Case 1: Axis parallel to plane (axis perpendicular to normal)
    if (aAbsDot < aAngleTol)
    {
      // Cylinder axis is parallel to plane
      // Distance from axis to plane
      const double aDx = myCylOrigX - myPlaneOrigX;
      const double aDy = myCylOrigY - myPlaneOrigY;
      const double aDz = myCylOrigZ - myPlaneOrigZ;
      const double aAxisDist =
        std::abs(aDx * myPlaneNormX + aDy * myPlaneNormY + aDz * myPlaneNormZ);

      // Minimum distance = axis distance - radius
      const double aMinDist = aAxisDist - myCylRadius;

      if (aMinDist <= theTol)
      {
        // Cylinder intersects or touches plane - infinite solutions along intersection
        myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = 0.0;
        return myResult;
      }

      // For parallel axis, infinite extrema along the closest generator
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aMinDist * aMinDist;
      return myResult;
    }

    // Case 2: Axis not parallel to plane
    // Find the point on axis closest to plane and compute extrema at that height

    // Project cylinder origin onto plane normal direction
    const double aDx = myCylOrigX - myPlaneOrigX;
    const double aDy = myCylOrigY - myPlaneOrigY;
    const double aDz = myCylOrigZ - myPlaneOrigZ;

    const double aOrigDistToPlane = aDx * myPlaneNormX + aDy * myPlaneNormY + aDz * myPlaneNormZ;

    // Parameter t along axis where axis intersects plane
    // (cylOrig + t * cylAxis - planeOrig) . planeNorm = 0
    // aOrigDistToPlane + t * myAxisNormDot = 0
    // t = -aOrigDistToPlane / myAxisNormDot
    const double aT = -aOrigDistToPlane / myAxisNormDot;

    // Point on axis at intersection with plane
    const double aAxisPtX = myCylOrigX + aT * myCylAxisX;
    const double aAxisPtY = myCylOrigY + aT * myCylAxisY;
    const double aAxisPtZ = myCylOrigZ + aT * myCylAxisZ;

    // At this height, the cylinder circle intersects the plane
    // Extrema occur at the two points where the radial direction is parallel to plane normal

    // Direction in plane perpendicular to axis: cross(axis, normal) normalized
    double aPerpX = myCylAxisY * myPlaneNormZ - myCylAxisZ * myPlaneNormY;
    double aPerpY = myCylAxisZ * myPlaneNormX - myCylAxisX * myPlaneNormZ;
    double aPerpZ = myCylAxisX * myPlaneNormY - myCylAxisY * myPlaneNormX;

    const double aPerpLen = std::sqrt(aPerpX * aPerpX + aPerpY * aPerpY + aPerpZ * aPerpZ);
    if (aPerpLen < theTol)
    {
      // Axis is perpendicular to plane - all points at same V are equidistant
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = 0.0;
      return myResult;
    }

    // For the general case, the extrema are where the radial direction has
    // maximum/minimum projection onto the plane normal
    // This happens when radial direction is in the plane containing axis and normal

    // Project plane normal onto the plane perpendicular to cylinder axis
    const double aNormProjOnAxis =
      myPlaneNormX * myCylAxisX + myPlaneNormY * myCylAxisY + myPlaneNormZ * myCylAxisZ;
    double aRadialDirX = myPlaneNormX - aNormProjOnAxis * myCylAxisX;
    double aRadialDirY = myPlaneNormY - aNormProjOnAxis * myCylAxisY;
    double aRadialDirZ = myPlaneNormZ - aNormProjOnAxis * myCylAxisZ;

    const double aRadialLen =
      std::sqrt(aRadialDirX * aRadialDirX + aRadialDirY * aRadialDirY + aRadialDirZ * aRadialDirZ);

    if (aRadialLen < theTol)
    {
      // Plane normal is parallel to axis - degenerate
      myResult.Status = ExtremaSS::Status::NoSolution;
      return myResult;
    }

    aRadialDirX /= aRadialLen;
    aRadialDirY /= aRadialLen;
    aRadialDirZ /= aRadialLen;

    // Minimum: cylinder point in direction of -radialDir (toward plane)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const double aCylPtX = aAxisPtX - myCylRadius * aRadialDirX;
      const double aCylPtY = aAxisPtY - myCylRadius * aRadialDirY;
      const double aCylPtZ = aAxisPtZ - myCylRadius * aRadialDirZ;
      const gp_Pnt aCylPt(aCylPtX, aCylPtY, aCylPtZ);

      // Project onto plane
      const double aDistToPlane = (aCylPtX - myPlaneOrigX) * myPlaneNormX
                                  + (aCylPtY - myPlaneOrigY) * myPlaneNormY
                                  + (aCylPtZ - myPlaneOrigZ) * myPlaneNormZ;
      const gp_Pnt aPlanePt(aCylPtX - aDistToPlane * myPlaneNormX,
                            aCylPtY - aDistToPlane * myPlaneNormY,
                            aCylPtZ - aDistToPlane * myPlaneNormZ);

      // Compute UV parameters
      const double aPlaneU = (aPlanePt.X() - myPlaneOrigX) * myPlaneXDirX
                             + (aPlanePt.Y() - myPlaneOrigY) * myPlaneXDirY
                             + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
      const double aPlaneV = (aPlanePt.X() - myPlaneOrigX) * myPlaneYDirX
                             + (aPlanePt.Y() - myPlaneOrigY) * myPlaneYDirY
                             + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

      double aCylU =
        std::atan2(-aRadialDirX * myCylYDirX - aRadialDirY * myCylYDirY - aRadialDirZ * myCylYDirZ,
                   -aRadialDirX * myCylXDirX - aRadialDirY * myCylXDirY - aRadialDirZ * myCylXDirZ);
      if (aCylU < 0.0)
        aCylU += ExtremaSS::THE_TWO_PI;

      const double aMinSqDist = aDistToPlane * aDistToPlane;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult,
                               aCylU,
                               aT,
                               aPlaneU,
                               aPlaneV,
                               aCylPt,
                               aPlanePt,
                               aMinSqDist,
                               true,
                               theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult,
                               aPlaneU,
                               aPlaneV,
                               aCylU,
                               aT,
                               aPlanePt,
                               aCylPt,
                               aMinSqDist,
                               true,
                               theTol);
      }
    }

    // Maximum: cylinder point in direction of +radialDir (away from plane)
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const double aCylPtX = aAxisPtX + myCylRadius * aRadialDirX;
      const double aCylPtY = aAxisPtY + myCylRadius * aRadialDirY;
      const double aCylPtZ = aAxisPtZ + myCylRadius * aRadialDirZ;
      const gp_Pnt aCylPt(aCylPtX, aCylPtY, aCylPtZ);

      // Project onto plane
      const double aDistToPlane = (aCylPtX - myPlaneOrigX) * myPlaneNormX
                                  + (aCylPtY - myPlaneOrigY) * myPlaneNormY
                                  + (aCylPtZ - myPlaneOrigZ) * myPlaneNormZ;
      const gp_Pnt aPlanePt(aCylPtX - aDistToPlane * myPlaneNormX,
                            aCylPtY - aDistToPlane * myPlaneNormY,
                            aCylPtZ - aDistToPlane * myPlaneNormZ);

      const double aPlaneU = (aPlanePt.X() - myPlaneOrigX) * myPlaneXDirX
                             + (aPlanePt.Y() - myPlaneOrigY) * myPlaneXDirY
                             + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
      const double aPlaneV = (aPlanePt.X() - myPlaneOrigX) * myPlaneYDirX
                             + (aPlanePt.Y() - myPlaneOrigY) * myPlaneYDirY
                             + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

      double aCylU =
        std::atan2(aRadialDirX * myCylYDirX + aRadialDirY * myCylYDirY + aRadialDirZ * myCylYDirZ,
                   aRadialDirX * myCylXDirX + aRadialDirY * myCylXDirY + aRadialDirZ * myCylXDirZ);
      if (aCylU < 0.0)
        aCylU += ExtremaSS::THE_TWO_PI;

      const double aMaxSqDist = aDistToPlane * aDistToPlane;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult,
                               aCylU,
                               aT,
                               aPlaneU,
                               aPlaneV,
                               aCylPt,
                               aPlanePt,
                               aMaxSqDist,
                               false,
                               theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult,
                               aPlaneU,
                               aPlaneV,
                               aCylU,
                               aT,
                               aPlanePt,
                               aCylPt,
                               aMaxSqDist,
                               false,
                               theTol);
      }
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);

    // Add boundary extrema if domain is bounded
    if (myResult.IsInfinite() || !myDomain.has_value())
    {
      return myResult;
    }

    checkBoundaryExtrema(theTol, theMode);

    if (myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaSS::Status::NoSolution;
    }
    else
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
    return myResult;
  }

private:
  //! Check extrema on domain boundaries.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
    {
      return;
    }

    const ExtremaSS::Domain4D& aDom       = myDomain.value();
    constexpr int              aNbSamples = 20;

    // Sample plane boundary edges (Domain1)
    const double aDU1 = (aDom.Domain1.UMax - aDom.Domain1.UMin) / aNbSamples;
    const double aDV1 = (aDom.Domain1.VMax - aDom.Domain1.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom.Domain1.UMin + i * aDU1;
      checkPlanePointAgainstCylinder(aU1, aDom.Domain1.VMin, theTol, theMode);
      checkPlanePointAgainstCylinder(aU1, aDom.Domain1.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV1 = aDom.Domain1.VMin + i * aDV1;
      checkPlanePointAgainstCylinder(aDom.Domain1.UMin, aV1, theTol, theMode);
      checkPlanePointAgainstCylinder(aDom.Domain1.UMax, aV1, theTol, theMode);
    }

    // Sample cylinder boundary edges (Domain2)
    const double aDU2 = (aDom.Domain2.UMax - aDom.Domain2.UMin) / aNbSamples;
    const double aDV2 = (aDom.Domain2.VMax - aDom.Domain2.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom.Domain2.UMin + i * aDU2;
      checkCylinderPointAgainstPlane(aU2, aDom.Domain2.VMin, theTol, theMode);
      checkCylinderPointAgainstPlane(aU2, aDom.Domain2.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV2 = aDom.Domain2.VMin + i * aDV2;
      checkCylinderPointAgainstPlane(aDom.Domain2.UMin, aV2, theTol, theMode);
      checkCylinderPointAgainstPlane(aDom.Domain2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a plane boundary point against the cylinder.
  void checkPlanePointAgainstCylinder(double                thePlaneU,
                                      double                thePlaneV,
                                      double                theTol,
                                      ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aPlanePt = Value1(thePlaneU, thePlaneV);

    // Find closest point on cylinder axis to this plane point
    const double aDx = aPlanePt.X() - myCylOrigX;
    const double aDy = aPlanePt.Y() - myCylOrigY;
    const double aDz = aPlanePt.Z() - myCylOrigZ;

    // V parameter along axis
    const double aCylV = aDx * myCylAxisX + aDy * myCylAxisY + aDz * myCylAxisZ;

    // Point on axis
    const double aAxisPtX = myCylOrigX + aCylV * myCylAxisX;
    const double aAxisPtY = myCylOrigY + aCylV * myCylAxisY;
    const double aAxisPtZ = myCylOrigZ + aCylV * myCylAxisZ;

    // Radial direction from axis to point
    const double aRadX    = aPlanePt.X() - aAxisPtX;
    const double aRadY    = aPlanePt.Y() - aAxisPtY;
    const double aRadZ    = aPlanePt.Z() - aAxisPtZ;
    const double aRadDist = std::sqrt(aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ);

    if (aRadDist < theTol)
    {
      // Point is on cylinder axis
      return;
    }

    // U angle
    const double aRadDirX = aRadX / aRadDist;
    const double aRadDirY = aRadY / aRadDist;
    const double aRadDirZ = aRadZ / aRadDist;

    double aCylU =
      std::atan2(aRadDirX * myCylYDirX + aRadDirY * myCylYDirY + aRadDirZ * myCylYDirZ,
                 aRadDirX * myCylXDirX + aRadDirY * myCylXDirY + aRadDirZ * myCylXDirZ);
    if (aCylU < 0)
      aCylU += ExtremaSS::THE_TWO_PI;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Check closest point (minimum)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const double aClampedU = std::clamp(aCylU, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aCylV, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aCylPt    = Value2(aClampedU, aClampedV);
      const double aSqDist   = aPlanePt.SquareDistance(aCylPt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult,
                               aClampedU,
                               aClampedV,
                               thePlaneU,
                               thePlaneV,
                               aCylPt,
                               aPlanePt,
                               aSqDist,
                               true,
                               theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult,
                               thePlaneU,
                               thePlaneV,
                               aClampedU,
                               aClampedV,
                               aPlanePt,
                               aCylPt,
                               aSqDist,
                               true,
                               theTol);
      }
    }

    // Check farthest point (maximum) - opposite side of cylinder
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      double aCylUFar = aCylU + M_PI;
      if (aCylUFar > ExtremaSS::THE_TWO_PI)
        aCylUFar -= ExtremaSS::THE_TWO_PI;

      const double aClampedU = std::clamp(aCylUFar, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aCylV, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aCylPt    = Value2(aClampedU, aClampedV);
      const double aSqDist   = aPlanePt.SquareDistance(aCylPt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult,
                               aClampedU,
                               aClampedV,
                               thePlaneU,
                               thePlaneV,
                               aCylPt,
                               aPlanePt,
                               aSqDist,
                               false,
                               theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult,
                               thePlaneU,
                               thePlaneV,
                               aClampedU,
                               aClampedV,
                               aPlanePt,
                               aCylPt,
                               aSqDist,
                               false,
                               theTol);
      }
    }
  }

  //! Check a cylinder boundary point against the plane.
  void checkCylinderPointAgainstPlane(double                theCylU,
                                      double                theCylV,
                                      double                theTol,
                                      ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aCylPt = Value2(theCylU, theCylV);

    // Project cylinder point onto plane
    const double aDistToPlane = (aCylPt.X() - myPlaneOrigX) * myPlaneNormX
                                + (aCylPt.Y() - myPlaneOrigY) * myPlaneNormY
                                + (aCylPt.Z() - myPlaneOrigZ) * myPlaneNormZ;

    const gp_Pnt aProjPt(aCylPt.X() - aDistToPlane * myPlaneNormX,
                         aCylPt.Y() - aDistToPlane * myPlaneNormY,
                         aCylPt.Z() - aDistToPlane * myPlaneNormZ);

    // Compute plane UV parameters
    double aPlaneU = (aProjPt.X() - myPlaneOrigX) * myPlaneXDirX
                     + (aProjPt.Y() - myPlaneOrigY) * myPlaneXDirY
                     + (aProjPt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
    double aPlaneV = (aProjPt.X() - myPlaneOrigX) * myPlaneYDirX
                     + (aProjPt.Y() - myPlaneOrigY) * myPlaneYDirY
                     + (aProjPt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Clamp to plane domain
    const double aClampedU = std::clamp(aPlaneU, aDom.Domain1.UMin, aDom.Domain1.UMax);
    const double aClampedV = std::clamp(aPlaneV, aDom.Domain1.VMin, aDom.Domain1.VMax);
    const gp_Pnt aPlanePt  = Value1(aClampedU, aClampedV);
    const double aSqDist   = aCylPt.SquareDistance(aPlanePt);

    // Only minimum makes sense for fixed cylinder point
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult,
                               theCylU,
                               theCylV,
                               aClampedU,
                               aClampedV,
                               aCylPt,
                               aPlanePt,
                               aSqDist,
                               true,
                               theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult,
                               aClampedU,
                               aClampedV,
                               theCylU,
                               theCylV,
                               aPlanePt,
                               aCylPt,
                               aSqDist,
                               true,
                               theTol);
      }
    }
  }

public:
  const gp_Pln& Plane() const { return myPlane; }

  const gp_Cylinder& Cylinder() const { return myCylinder; }

  bool IsSwapped() const { return mySwapped; }

  bool IsBounded() const { return myDomain.has_value(); }

private:
  gp_Pln                             myPlane;
  gp_Cylinder                        myCylinder;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Cached plane components
  double myPlaneOrigX, myPlaneOrigY, myPlaneOrigZ;
  double myPlaneNormX, myPlaneNormY, myPlaneNormZ;
  double myPlaneXDirX, myPlaneXDirY, myPlaneXDirZ;
  double myPlaneYDirX, myPlaneYDirY, myPlaneYDirZ;

  // Cached cylinder components
  double myCylOrigX, myCylOrigY, myCylOrigZ;
  double myCylAxisX, myCylAxisY, myCylAxisZ;
  double myCylXDirX, myCylXDirY, myCylXDirZ;
  double myCylYDirX, myCylYDirY, myCylYDirZ;
  double myCylRadius;

  double myAxisNormDot;
};

#endif // _ExtremaSS_PlaneCylinder_HeaderFile
