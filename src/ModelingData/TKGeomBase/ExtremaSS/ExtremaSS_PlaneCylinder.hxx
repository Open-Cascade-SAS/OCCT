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
    myAxisNormDot = myCylAxisX * myPlaneNormX + myCylAxisY * myPlaneNormY + myCylAxisZ * myPlaneNormZ;
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
        ExtremaSS::AddExtremum(myResult, aCylU, aT, aPlaneU, aPlaneV, aCylPt, aPlanePt, aMinSqDist,
                               true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, aCylU, aT, aPlanePt, aCylPt, aMinSqDist,
                               true, theTol);
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
        ExtremaSS::AddExtremum(myResult, aCylU, aT, aPlaneU, aPlaneV, aCylPt, aPlanePt, aMaxSqDist,
                               false, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, aCylU, aT, aPlanePt, aCylPt, aMaxSqDist,
                               false, theTol);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution
                                                 : ExtremaSS::Status::OK;
    return myResult;
  }

  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);
    return myResult;
  }

  const gp_Pln&      Plane() const { return myPlane; }
  const gp_Cylinder& Cylinder() const { return myCylinder; }
  bool               IsSwapped() const { return mySwapped; }
  bool               IsBounded() const { return myDomain.has_value(); }

private:
  gp_Pln                               myPlane;
  gp_Cylinder                          myCylinder;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result            myResult;
  bool                                 mySwapped;

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
