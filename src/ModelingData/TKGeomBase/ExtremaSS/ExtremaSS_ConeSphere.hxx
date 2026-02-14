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

#ifndef _ExtremaSS_ConeSphere_HeaderFile
#define _ExtremaSS_ConeSphere_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Cone-Sphere extrema computation.
//!
//! Computes the extrema between a conical surface and a sphere.
//! This class handles both (Cone, Sphere) and (Sphere, Cone) orderings.
//!
//! Algorithm:
//! 1. Find distance from sphere center to cone surface
//! 2. If center is on cone axis: infinite solutions on a circle
//! 3. Otherwise: find closest/farthest points on cone to center
//! 4. Adjust by sphere radius for surface-to-surface distance
class ExtremaSS_ConeSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with cone and sphere (canonical order).
  ExtremaSS_ConeSphere(const gp_Cone& theCone, const gp_Sphere& theSphere)
      : myCone(theCone),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere and cone (swapped order).
  ExtremaSS_ConeSphere(const gp_Sphere& theSphere, const gp_Cone& theCone)
      : myCone(theCone),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with cone, sphere, and domain.
  ExtremaSS_ConeSphere(const gp_Cone&             theCone,
                       const gp_Sphere&           theSphere,
                       const ExtremaSS::Domain4D& theDomain)
      : myCone(theCone),
        mySphere(theSphere),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere, cone, and domain (swapped order).
  ExtremaSS_ConeSphere(const gp_Sphere&           theSphere,
                       const gp_Cone&             theCone,
                       const ExtremaSS::Domain4D& theDomain)
      : myCone(theCone),
        mySphere(theSphere),
        myDomain(ExtremaSS::Domain4D(theDomain.Domain2, theDomain.Domain1)),
        mySwapped(true)
  {
    initCache();
  }

private:
  void initCache()
  {
    // Cache cone components
    const gp_Ax3& aConePos = myCone.Position();
    myConeOrig             = aConePos.Location();
    myConeOrigX            = myConeOrig.X();
    myConeOrigY            = myConeOrig.Y();
    myConeOrigZ            = myConeOrig.Z();

    const gp_Dir& aConeAxis = aConePos.Direction();
    myConeAxisX             = aConeAxis.X();
    myConeAxisY             = aConeAxis.Y();
    myConeAxisZ             = aConeAxis.Z();

    const gp_Dir& aConeXDir = aConePos.XDirection();
    myConeXDirX             = aConeXDir.X();
    myConeXDirY             = aConeXDir.Y();
    myConeXDirZ             = aConeXDir.Z();

    const gp_Dir& aConeYDir = aConePos.YDirection();
    myConeYDirX             = aConeYDir.X();
    myConeYDirY             = aConeYDir.Y();
    myConeYDirZ             = aConeYDir.Z();

    myApex         = myCone.Apex();
    mySemiAngle    = myCone.SemiAngle();
    myCosSemiAngle = std::cos(mySemiAngle);
    mySinSemiAngle = std::sin(mySemiAngle);
    myTanSemiAngle = std::tan(mySemiAngle);
    myRefRadius    = myCone.RefRadius();

    // Cache sphere components
    const gp_Ax3& aSpherePos = mySphere.Position();
    mySphereCenter           = aSpherePos.Location();
    mySphereCenterX          = mySphereCenter.X();
    mySphereCenterY          = mySphereCenter.Y();
    mySphereCenterZ          = mySphereCenter.Z();
    mySphereRadius           = mySphere.Radius();

    const gp_Dir& aSphereXDir = aSpherePos.XDirection();
    mySphereXDirX             = aSphereXDir.X();
    mySphereXDirY             = aSphereXDir.Y();
    mySphereXDirZ             = aSphereXDir.Z();

    const gp_Dir& aSphereYDir = aSpherePos.YDirection();
    mySphereYDirX             = aSphereYDir.X();
    mySphereYDirY             = aSphereYDir.Y();
    mySphereYDirZ             = aSphereYDir.Z();

    const gp_Dir& aSphereZDir = aSpherePos.Direction();
    mySphereZDirX             = aSphereZDir.X();
    mySphereZDirY             = aSphereZDir.Y();
    mySphereZDirZ             = aSphereZDir.Z();

    // Vector from apex to sphere center
    myApexToCenterX = mySphereCenterX - myApex.X();
    myApexToCenterY = mySphereCenterY - myApex.Y();
    myApexToCenterZ = mySphereCenterZ - myApex.Z();

    // Project center onto cone axis
    myCenterAlongAxis =
      myApexToCenterX * myConeAxisX + myApexToCenterY * myConeAxisY + myApexToCenterZ * myConeAxisZ;

    // Radial distance from axis
    const double aRadialX = myApexToCenterX - myCenterAlongAxis * myConeAxisX;
    const double aRadialY = myApexToCenterY - myCenterAlongAxis * myConeAxisY;
    const double aRadialZ = myApexToCenterZ - myCenterAlongAxis * myConeAxisZ;
    myCenterRadialDist    = std::sqrt(aRadialX * aRadialX + aRadialY * aRadialY + aRadialZ * aRadialZ);

    // Normalized radial direction (from axis toward center)
    if (myCenterRadialDist > ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      myRadialDirX       = aRadialX / myCenterRadialDist;
      myRadialDirY       = aRadialY / myCenterRadialDist;
      myRadialDirZ       = aRadialZ / myCenterRadialDist;
      myCenterOnAxis     = false;
    }
    else
    {
      myRadialDirX       = myConeXDirX;
      myRadialDirY       = myConeXDirY;
      myRadialDirZ       = myConeXDirZ;
      myCenterOnAxis     = true;
    }
  }

public:
  gp_Pnt Value1(double theU, double theV) const
  {
    // Cone point at (U, V)
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius + theV * myTanSemiAngle;
    return gp_Pnt(myConeOrigX + theV * myConeAxisX + aRadius * (aCosU * myConeXDirX + aSinU * myConeYDirX),
                  myConeOrigY + theV * myConeAxisY + aRadius * (aCosU * myConeXDirY + aSinU * myConeYDirY),
                  myConeOrigZ + theV * myConeAxisZ + aRadius * (aCosU * myConeXDirZ + aSinU * myConeYDirZ));
  }

  gp_Pnt Value2(double theU, double theV) const
  {
    // Sphere point at (U, V)
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    const double aCosV = std::cos(theV);
    const double aSinV = std::sin(theV);
    return gp_Pnt(
      mySphereCenterX + mySphereRadius * (aCosV * (aCosU * mySphereXDirX + aSinU * mySphereYDirX)
                                        + aSinV * mySphereZDirX),
      mySphereCenterY + mySphereRadius * (aCosV * (aCosU * mySphereXDirY + aSinU * mySphereYDirY)
                                        + aSinV * mySphereZDirY),
      mySphereCenterZ + mySphereRadius * (aCosV * (aCosU * mySphereXDirZ + aSinU * mySphereYDirZ)
                                        + aSinV * mySphereZDirZ));
  }

  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Special case: sphere center on cone axis
    if (myCenterOnAxis)
    {
      return performCenterOnAxis(theTol, theMode);
    }

    // General case: find closest/farthest points on cone to sphere center
    return performGeneral(theTol, theMode);
  }

private:
  const ExtremaSS::Result& performCenterOnAxis(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Center on cone axis - extrema form circles
    // Distance from center to any point on cone at height h:
    // d(h) = sqrt((h - centerAlongAxis)^2 + (h * tan(alpha))^2) for cone from apex
    // This is minimized when d(d^2)/dh = 0

    // Let h be distance from apex along axis, r(h) = h * tan(alpha) is radius at h
    // Point on cone: apex + h*axis + r(h)*(cos(u)*X + sin(u)*Y)
    // Distance^2 to center = (h - centerAlongAxis)^2 + r(h)^2

    // d(D^2)/dh = 2(h - centerAlongAxis) + 2*h*tan^2(alpha) = 0
    // h(1 + tan^2(alpha)) = centerAlongAxis
    // h = centerAlongAxis * cos^2(alpha)

    const double aHMin = myCenterAlongAxis * myCosSemiAngle * myCosSemiAngle;

    // Check if this height is valid (h > 0 for standard cone)
    if (aHMin <= 0)
    {
      // Closest point is at apex
      const double aDistToApex = std::sqrt(myApexToCenterX * myApexToCenterX + myApexToCenterY * myApexToCenterY
                                         + myApexToCenterZ * myApexToCenterZ);
      const double aMinDist = std::abs(aDistToApex - mySphereRadius);

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        // Infinite solutions on a circle around axis on sphere
        myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = aMinDist * aMinDist;
        return myResult;
      }
    }
    else
    {
      // Compute distance at optimal height
      const double aRadiusAtH = aHMin * myTanSemiAngle;
      const double aDeltaH    = aHMin - myCenterAlongAxis;
      const double aDistToCone =
        std::sqrt(aDeltaH * aDeltaH + aRadiusAtH * aRadiusAtH); // This simplifies to |centerAlongAxis|*sin(alpha)
      const double aMinDist = std::abs(aDistToCone - mySphereRadius);

      if (aMinDist < theTol)
      {
        // Sphere intersects cone - infinite solutions on intersection circle
        myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = 0.0;
        return myResult;
      }

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = aMinDist * aMinDist;
        return myResult;
      }
    }

    // Maximum: for unbounded cone and sphere on axis, no finite maximum
    myResult.Status = ExtremaSS::Status::NoSolution;
    return myResult;
  }

  const ExtremaSS::Result& performGeneral(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // First, check if sphere touches or contains the apex
    const double aDistToApex =
      std::sqrt(myApexToCenterX * myApexToCenterX + myApexToCenterY * myApexToCenterY + myApexToCenterZ * myApexToCenterZ);
    const double aApexDistFromSurface = aDistToApex - mySphereRadius;

    if (theMode != ExtremaSS::SearchMode::Max && std::abs(aApexDistFromSurface) < theTol)
    {
      // Sphere touches apex - minimum distance is 0
      const double aVApex = -myRefRadius / myTanSemiAngle;
      double       aSphereU, aSphereV;
      computeSphereParams(myApex, aSphereU, aSphereV);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, 0.0, aVApex, myApex, myApex, 0.0, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, 0.0, aVApex, aSphereU, aSphereV, myApex, myApex, 0.0, true, theTol);
      }
      myResult.Status = ExtremaSS::Status::OK;
      return myResult;
    }

    // Sphere center not on axis
    // The extrema lie in the plane containing the axis and the center
    // This plane intersects the cone in two lines (generators)

    // In 2D (axis, radial direction):
    // Cone generators: points at (h, h*tan(alpha)) and (h, -h*tan(alpha))
    // Center at (centerAlongAxis, centerRadialDist)

    // Distance from center to generator line through apex at angle alpha from axis:
    // Line 1 (toward center): (t*cos(alpha), t*sin(alpha)) for t >= 0
    // Line 2 (away from center): (t*cos(alpha), -t*sin(alpha)) for t >= 0

    // Distance from point (a, r) to line through origin at angle theta:
    // d = |a*sin(theta) - r*cos(theta)|

    // For line 1 (angle = alpha): d1 = |centerAlongAxis*sin(alpha) - centerRadialDist*cos(alpha)|
    // For line 2 (angle = -alpha): d2 = |centerAlongAxis*sin(alpha) + centerRadialDist*cos(alpha)|

    const double aDist1 =
      std::abs(myCenterAlongAxis * mySinSemiAngle - myCenterRadialDist * myCosSemiAngle);
    const double aDist2 =
      std::abs(myCenterAlongAxis * mySinSemiAngle + myCenterRadialDist * myCosSemiAngle);

    // Determine which generator is closer/farther
    const bool aGen1IsCloser = (aDist1 <= aDist2);

    // Find foot of perpendicular from center to the closer generator
    // For the closer generator, we need to find the point on cone
    // Parametric: apex + t*(cos(alpha)*axis + sin(alpha)*radialDir)
    // Project center onto this line

    // Direction of closer generator in 3D
    double aGenDirX, aGenDirY, aGenDirZ;
    double aOtherGenDirX, aOtherGenDirY, aOtherGenDirZ;

    if (aGen1IsCloser)
    {
      // Generator toward radial direction
      aGenDirX      = myCosSemiAngle * myConeAxisX + mySinSemiAngle * myRadialDirX;
      aGenDirY      = myCosSemiAngle * myConeAxisY + mySinSemiAngle * myRadialDirY;
      aGenDirZ      = myCosSemiAngle * myConeAxisZ + mySinSemiAngle * myRadialDirZ;
      aOtherGenDirX = myCosSemiAngle * myConeAxisX - mySinSemiAngle * myRadialDirX;
      aOtherGenDirY = myCosSemiAngle * myConeAxisY - mySinSemiAngle * myRadialDirY;
      aOtherGenDirZ = myCosSemiAngle * myConeAxisZ - mySinSemiAngle * myRadialDirZ;
    }
    else
    {
      // Generator away from radial direction
      aGenDirX      = myCosSemiAngle * myConeAxisX - mySinSemiAngle * myRadialDirX;
      aGenDirY      = myCosSemiAngle * myConeAxisY - mySinSemiAngle * myRadialDirY;
      aGenDirZ      = myCosSemiAngle * myConeAxisZ - mySinSemiAngle * myRadialDirZ;
      aOtherGenDirX = myCosSemiAngle * myConeAxisX + mySinSemiAngle * myRadialDirX;
      aOtherGenDirY = myCosSemiAngle * myConeAxisY + mySinSemiAngle * myRadialDirY;
      aOtherGenDirZ = myCosSemiAngle * myConeAxisZ + mySinSemiAngle * myRadialDirZ;
    }

    // Project apex-to-center vector onto generator direction
    const double aT = myApexToCenterX * aGenDirX + myApexToCenterY * aGenDirY + myApexToCenterZ * aGenDirZ;

    // Minimum distance calculation
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      double aMinDistToCone;
      gp_Pnt aMinConePoint;
      double aMinConeU, aMinConeV;

      if (aT <= 0)
      {
        // Closest point is at apex
        aMinDistToCone = std::sqrt(myApexToCenterX * myApexToCenterX + myApexToCenterY * myApexToCenterY
                                 + myApexToCenterZ * myApexToCenterZ);
        aMinConePoint  = myApex;
        aMinConeU      = 0.0;
        aMinConeV = -myRefRadius / myTanSemiAngle; // V at apex
      }
      else
      {
        // Foot of perpendicular on generator
        const double aConeX  = myApex.X() + aT * aGenDirX;
        const double aConeY  = myApex.Y() + aT * aGenDirY;
        const double aConeZ  = myApex.Z() + aT * aGenDirZ;
        aMinConePoint        = gp_Pnt(aConeX, aConeY, aConeZ);
        aMinDistToCone       = aGen1IsCloser ? aDist1 : aDist2;

        // Compute cone parameters
        const double aH      = aT * myCosSemiAngle; // Height from apex
        aMinConeV            = aH - myRefRadius / myTanSemiAngle; // V from cone origin
        const double aSign   = aGen1IsCloser ? 1.0 : -1.0;
        aMinConeU            = std::atan2(aSign * (myRadialDirX * myConeYDirX + myRadialDirY * myConeYDirY
                                                 + myRadialDirZ * myConeYDirZ),
                                          aSign * (myRadialDirX * myConeXDirX + myRadialDirY * myConeXDirY
                                                 + myRadialDirZ * myConeXDirZ));
        if (aMinConeU < 0)
          aMinConeU += ExtremaSS::THE_TWO_PI;
      }

      const double aMinSurfDist = aMinDistToCone - mySphereRadius;
      if (aMinSurfDist < -theTol)
      {
        // Sphere intersects cone
        addIntersectionExtremum(aMinConePoint, aMinConeU, aMinConeV, theTol, true);
      }
      else
      {
        const double aMinSqDist = aMinSurfDist * aMinSurfDist;

        // Sphere point: on line from center toward cone point
        const double aDx = aMinConePoint.X() - mySphereCenterX;
        const double aDy = aMinConePoint.Y() - mySphereCenterY;
        const double aDz = aMinConePoint.Z() - mySphereCenterZ;
        const double aLen =
          std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

        if (aLen > theTol)
        {
          const double aSphereX = mySphereCenterX + mySphereRadius * aDx / aLen;
          const double aSphereY = mySphereCenterY + mySphereRadius * aDy / aLen;
          const double aSphereZ = mySphereCenterZ + mySphereRadius * aDz / aLen;
          const gp_Pnt aSpherePt(aSphereX, aSphereY, aSphereZ);

          // Compute sphere parameters
          double aSphereU, aSphereV;
          computeSphereParams(aSpherePt, aSphereU, aSphereV);

          if (mySwapped)
          {
            ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, aMinConeU, aMinConeV, aSpherePt,
                                   aMinConePoint, aMinSqDist, true, theTol);
          }
          else
          {
            ExtremaSS::AddExtremum(myResult, aMinConeU, aMinConeV, aSphereU, aSphereV, aMinConePoint,
                                   aSpherePt, aMinSqDist, true, theTol);
          }
        }
      }
    }

    // Maximum distance
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum is on the farther generator
      const double aTOther = myApexToCenterX * aOtherGenDirX + myApexToCenterY * aOtherGenDirY
                           + myApexToCenterZ * aOtherGenDirZ;

      if (aTOther > 0)
      {
        // Use the other generator for maximum
        const double aMaxDistToCone = aGen1IsCloser ? aDist2 : aDist1;
        const double aMaxSurfDist   = aMaxDistToCone + mySphereRadius;
        const double aMaxSqDist     = aMaxSurfDist * aMaxSurfDist;

        const double aConeX = myApex.X() + aTOther * aOtherGenDirX;
        const double aConeY = myApex.Y() + aTOther * aOtherGenDirY;
        const double aConeZ = myApex.Z() + aTOther * aOtherGenDirZ;
        const gp_Pnt aMaxConePoint(aConeX, aConeY, aConeZ);

        const double aH      = aTOther * myCosSemiAngle;
        const double aMaxConeV = aH - myRefRadius / myTanSemiAngle;
        const double aSign   = aGen1IsCloser ? -1.0 : 1.0;
        double       aMaxConeU =
          std::atan2(aSign * (myRadialDirX * myConeYDirX + myRadialDirY * myConeYDirY + myRadialDirZ * myConeYDirZ),
                     aSign * (myRadialDirX * myConeXDirX + myRadialDirY * myConeXDirY + myRadialDirZ * myConeXDirZ));
        if (aMaxConeU < 0)
          aMaxConeU += ExtremaSS::THE_TWO_PI;

        // Sphere point: opposite direction from cone
        const double aDx  = mySphereCenterX - aConeX;
        const double aDy  = mySphereCenterY - aConeY;
        const double aDz  = mySphereCenterZ - aConeZ;
        const double aLen = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

        if (aLen > theTol)
        {
          const double aSphereX = mySphereCenterX + mySphereRadius * aDx / aLen;
          const double aSphereY = mySphereCenterY + mySphereRadius * aDy / aLen;
          const double aSphereZ = mySphereCenterZ + mySphereRadius * aDz / aLen;
          const gp_Pnt aSpherePt(aSphereX, aSphereY, aSphereZ);

          double aSphereU, aSphereV;
          computeSphereParams(aSpherePt, aSphereU, aSphereV);

          if (mySwapped)
          {
            ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, aMaxConeU, aMaxConeV, aSpherePt,
                                   aMaxConePoint, aMaxSqDist, false, theTol);
          }
          else
          {
            ExtremaSS::AddExtremum(myResult, aMaxConeU, aMaxConeV, aSphereU, aSphereV, aMaxConePoint,
                                   aSpherePt, aMaxSqDist, false, theTol);
          }
        }
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  void addIntersectionExtremum(const gp_Pnt& theConePoint,
                               double        theConeU,
                               double        theConeV,
                               double        theTol,
                               bool          theIsMin) const
  {
    // For intersection, the cone point is inside the sphere
    // The closest sphere point is in direction from center to cone point
    const double aDx  = theConePoint.X() - mySphereCenterX;
    const double aDy  = theConePoint.Y() - mySphereCenterY;
    const double aDz  = theConePoint.Z() - mySphereCenterZ;
    const double aLen = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    gp_Pnt aSpherePt;
    double aSphereU, aSphereV;

    if (aLen > theTol)
    {
      // Normal case: cone point is not at sphere center
      const double aSphereX = mySphereCenterX + mySphereRadius * aDx / aLen;
      const double aSphereY = mySphereCenterY + mySphereRadius * aDy / aLen;
      const double aSphereZ = mySphereCenterZ + mySphereRadius * aDz / aLen;
      aSpherePt             = gp_Pnt(aSphereX, aSphereY, aSphereZ);
      computeSphereParams(aSpherePt, aSphereU, aSphereV);
    }
    else
    {
      // Cone point is at sphere center - sphere center is exactly on cone surface
      // Use the cone's radial direction to determine sphere point
      const double aSphereX = mySphereCenterX + mySphereRadius * myRadialDirX;
      const double aSphereY = mySphereCenterY + mySphereRadius * myRadialDirY;
      const double aSphereZ = mySphereCenterZ + mySphereRadius * myRadialDirZ;
      aSpherePt             = gp_Pnt(aSphereX, aSphereY, aSphereZ);
      computeSphereParams(aSpherePt, aSphereU, aSphereV);
    }

    // The actual distance is 0 since they intersect - find actual intersection point
    // The intersection is where the sphere surface meets the cone surface
    // For the minimum, we report distance 0 at the intersection
    const double aSqDist = 0.0;

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, theConeU, theConeV, aSpherePt, theConePoint,
                             aSqDist, theIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, theConeU, theConeV, aSphereU, aSphereV, theConePoint, aSpherePt,
                             aSqDist, theIsMin, theTol);
    }
  }

  void computeSphereParams(const gp_Pnt& thePoint, double& theU, double& theV) const
  {
    // Convert point on sphere to (U, V) parameters
    const double aDx = thePoint.X() - mySphereCenterX;
    const double aDy = thePoint.Y() - mySphereCenterY;
    const double aDz = thePoint.Z() - mySphereCenterZ;

    // Project onto sphere coordinate system
    const double aX = aDx * mySphereXDirX + aDy * mySphereXDirY + aDz * mySphereXDirZ;
    const double aY = aDx * mySphereYDirX + aDy * mySphereYDirY + aDz * mySphereYDirZ;
    const double aZ = aDx * mySphereZDirX + aDy * mySphereZDirY + aDz * mySphereZDirZ;

    theV = std::asin(std::clamp(aZ / mySphereRadius, -1.0, 1.0));
    theU = std::atan2(aY, aX);
    if (theU < 0)
      theU += ExtremaSS::THE_TWO_PI;
  }

public:
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);
    return myResult;
  }

  const gp_Cone&   Cone() const { return myCone; }
  const gp_Sphere& Sphere() const { return mySphere; }
  bool             IsSwapped() const { return mySwapped; }
  bool             IsBounded() const { return myDomain.has_value(); }

private:
  gp_Cone                            myCone;
  gp_Sphere                          mySphere;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Cached cone components
  gp_Pnt myConeOrig;
  double myConeOrigX, myConeOrigY, myConeOrigZ;
  double myConeAxisX, myConeAxisY, myConeAxisZ;
  double myConeXDirX, myConeXDirY, myConeXDirZ;
  double myConeYDirX, myConeYDirY, myConeYDirZ;
  gp_Pnt myApex;
  double mySemiAngle, myCosSemiAngle, mySinSemiAngle, myTanSemiAngle;
  double myRefRadius;

  // Cached sphere components
  gp_Pnt mySphereCenter;
  double mySphereCenterX, mySphereCenterY, mySphereCenterZ;
  double mySphereRadius;
  double mySphereXDirX, mySphereXDirY, mySphereXDirZ;
  double mySphereYDirX, mySphereYDirY, mySphereYDirZ;
  double mySphereZDirX, mySphereZDirY, mySphereZDirZ;

  // Cached geometric relationships
  double myApexToCenterX, myApexToCenterY, myApexToCenterZ;
  double myCenterAlongAxis;
  double myCenterRadialDist;
  double myRadialDirX, myRadialDirY, myRadialDirZ;
  bool   myCenterOnAxis;
};

#endif // _ExtremaSS_ConeSphere_HeaderFile
