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

#ifndef _ExtremaSS_SphereTorus_HeaderFile
#define _ExtremaSS_SphereTorus_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Sphere-Torus extrema computation.
//!
//! Computes the extrema between a sphere and a torus.
//! This class handles both (Sphere, Torus) and (Torus, Sphere) orderings.
//!
//! Algorithm:
//! 1. Project sphere center onto torus plane (XY plane of torus)
//! 2. Find angles U where generating circles are closest/farthest to sphere center
//! 3. For each such U, compute distance from sphere center to generating circle
//! 4. Add/subtract sphere radius for final surface-to-surface distance
//!
//! The torus surface: T(u,v) = C + (R + r*cos(v))*(cos(u)*X + sin(u)*Y) + r*sin(v)*Z
//! where R = major radius, r = minor radius
class ExtremaSS_SphereTorus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with sphere and torus (canonical order).
  ExtremaSS_SphereTorus(const gp_Sphere& theSphere, const gp_Torus& theTorus)
      : mySphere(theSphere),
        myTorus(theTorus),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with torus and sphere (swapped order).
  ExtremaSS_SphereTorus(const gp_Torus& theTorus, const gp_Sphere& theSphere)
      : mySphere(theSphere),
        myTorus(theTorus),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with sphere, torus, and domain.
  ExtremaSS_SphereTorus(const gp_Sphere&           theSphere,
                        const gp_Torus&            theTorus,
                        const ExtremaSS::Domain4D& theDomain)
      : mySphere(theSphere),
        myTorus(theTorus),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with torus, sphere, and domain (swapped order).
  ExtremaSS_SphereTorus(const gp_Torus&            theTorus,
                        const gp_Sphere&           theSphere,
                        const ExtremaSS::Domain4D& theDomain)
      : mySphere(theSphere),
        myTorus(theTorus),
        myDomain(ExtremaSS::Domain4D(theDomain.Domain2, theDomain.Domain1)),
        mySwapped(true)
  {
    initCache();
  }

private:
  void initCache()
  {
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

    // Cache torus components
    const gp_Ax3& aTorusPos = myTorus.Position();
    myTorusCenter           = aTorusPos.Location();
    myTorusCenterX          = myTorusCenter.X();
    myTorusCenterY          = myTorusCenter.Y();
    myTorusCenterZ          = myTorusCenter.Z();

    const gp_Dir& aTorusAxis = aTorusPos.Direction();
    myTorusAxisX             = aTorusAxis.X();
    myTorusAxisY             = aTorusAxis.Y();
    myTorusAxisZ             = aTorusAxis.Z();

    const gp_Dir& aTorusXDir = aTorusPos.XDirection();
    myTorusXDirX             = aTorusXDir.X();
    myTorusXDirY             = aTorusXDir.Y();
    myTorusXDirZ             = aTorusXDir.Z();

    const gp_Dir& aTorusYDir = aTorusPos.YDirection();
    myTorusYDirX             = aTorusYDir.X();
    myTorusYDirY             = aTorusYDir.Y();
    myTorusYDirZ             = aTorusYDir.Z();

    myMajorRadius = myTorus.MajorRadius();
    myMinorRadius = myTorus.MinorRadius();

    // Vector from torus center to sphere center
    myCenterToCenterX = mySphereCenterX - myTorusCenterX;
    myCenterToCenterY = mySphereCenterY - myTorusCenterY;
    myCenterToCenterZ = mySphereCenterZ - myTorusCenterZ;

    // Project sphere center onto torus plane
    myHeightAbovePlane = myCenterToCenterX * myTorusAxisX + myCenterToCenterY * myTorusAxisY
                       + myCenterToCenterZ * myTorusAxisZ;

    // Projection in torus XY plane
    const double aProjX = myCenterToCenterX - myHeightAbovePlane * myTorusAxisX;
    const double aProjY = myCenterToCenterY - myHeightAbovePlane * myTorusAxisY;
    const double aProjZ = myCenterToCenterZ - myHeightAbovePlane * myTorusAxisZ;

    myRadialDistFromAxis = std::sqrt(aProjX * aProjX + aProjY * aProjY + aProjZ * aProjZ);

    // Radial direction (from torus axis toward sphere center projection)
    if (myRadialDistFromAxis > ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      myRadialDirX   = aProjX / myRadialDistFromAxis;
      myRadialDirY   = aProjY / myRadialDistFromAxis;
      myRadialDirZ   = aProjZ / myRadialDistFromAxis;
      myCenterOnAxis = false;
    }
    else
    {
      myRadialDirX   = myTorusXDirX;
      myRadialDirY   = myTorusXDirY;
      myRadialDirZ   = myTorusXDirZ;
      myCenterOnAxis = true;
    }
  }

public:
  gp_Pnt Value1(double theU, double theV) const
  {
    // Sphere point at (U, V)
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    const double aCosV = std::cos(theV);
    const double aSinV = std::sin(theV);
    return gp_Pnt(
      mySphereCenterX
        + mySphereRadius * (aCosV * (aCosU * mySphereXDirX + aSinU * mySphereYDirX) + aSinV * mySphereZDirX),
      mySphereCenterY
        + mySphereRadius * (aCosV * (aCosU * mySphereXDirY + aSinU * mySphereYDirY) + aSinV * mySphereZDirY),
      mySphereCenterZ
        + mySphereRadius * (aCosV * (aCosU * mySphereXDirZ + aSinU * mySphereYDirZ) + aSinV * mySphereZDirZ));
  }

  gp_Pnt Value2(double theU, double theV) const
  {
    // Torus point at (U, V)
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aCosV   = std::cos(theV);
    const double aSinV   = std::sin(theV);
    const double aRadius = myMajorRadius + myMinorRadius * aCosV;
    return gp_Pnt(
      myTorusCenterX + aRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX) + myMinorRadius * aSinV * myTorusAxisX,
      myTorusCenterY + aRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY) + myMinorRadius * aSinV * myTorusAxisY,
      myTorusCenterZ + aRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ) + myMinorRadius * aSinV * myTorusAxisZ);
  }

  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Special case: sphere center on torus axis
    if (myCenterOnAxis)
    {
      return performCenterOnAxis(theTol, theMode);
    }

    // General case
    return performGeneral(theTol, theMode);
  }

private:
  const ExtremaSS::Result& performCenterOnAxis(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Sphere center is on torus axis
    // All generating circles are equidistant from sphere center
    // Distance from center to any generating circle center = sqrt(h^2 + R^2)
    // where h = height above plane, R = major radius

    const double aDistToCircleCenter =
      std::sqrt(myHeightAbovePlane * myHeightAbovePlane + myMajorRadius * myMajorRadius);

    // Distance from sphere center to generating circle (closest point)
    const double aDistToTorus = aDistToCircleCenter - myMinorRadius;
    const double aMinDist     = std::abs(aDistToTorus - mySphereRadius);
    const double aMaxDist     = aDistToTorus + mySphereRadius;

    if (aMinDist < theTol)
    {
      // Sphere intersects or touches torus - infinite solutions
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = 0.0;
      return myResult;
    }

    // Infinite solutions at constant distance
    myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
    myResult.InfiniteSquareDistance = (theMode == ExtremaSS::SearchMode::Max) ? aMaxDist * aMaxDist : aMinDist * aMinDist;
    return myResult;
  }

  const ExtremaSS::Result& performGeneral(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Sphere center not on torus axis
    // Extrema occur at U angles where generating circle is closest/farthest
    // U_close: generating circle center closest to sphere center projection
    // U_far: generating circle center farthest from sphere center projection

    // Angle for closest generating circle (toward sphere center)
    double aUClose = std::atan2(myRadialDirX * myTorusYDirX + myRadialDirY * myTorusYDirY + myRadialDirZ * myTorusYDirZ,
                                myRadialDirX * myTorusXDirX + myRadialDirY * myTorusXDirY + myRadialDirZ * myTorusXDirZ);
    if (aUClose < 0)
      aUClose += ExtremaSS::THE_TWO_PI;

    // Angle for farthest generating circle
    double aUFar = aUClose + M_PI;
    if (aUFar > ExtremaSS::THE_TWO_PI)
      aUFar -= ExtremaSS::THE_TWO_PI;

    // Process closest generating circle
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      processGeneratingCircle(aUClose, theTol, true);
    }

    // Process farthest generating circle
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      processGeneratingCircle(aUFar, theTol, false);
    }

    // Also check for additional extrema on the inner/outer surfaces of torus
    // These occur at U = aUClose but on different sides of the generating circle
    if (theMode == ExtremaSS::SearchMode::MinMax || theMode == ExtremaSS::SearchMode::Max)
    {
      // Maximum on outer surface (farthest side of closest generating circle)
      processOuterExtremum(aUClose, theTol);
    }

    if (theMode == ExtremaSS::SearchMode::MinMax || theMode == ExtremaSS::SearchMode::Min)
    {
      // Minimum on inner surface (closest side of farthest generating circle)
      processInnerExtremum(aUFar, theTol);
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  void processGeneratingCircle(double theU, double theTol, bool theIsClosest) const
  {
    // Center of generating circle at angle U
    const double aCosU      = std::cos(theU);
    const double aSinU      = std::sin(theU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    // Vector from generating circle center to sphere center
    const double aDx = mySphereCenterX - aCircCentX;
    const double aDy = mySphereCenterY - aCircCentY;
    const double aDz = mySphereCenterZ - aCircCentZ;

    const double aDistToCircCenter = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDistToCircCenter < theTol)
    {
      // Sphere center is at generating circle center - sphere is inside the tube
      // All torus points on this generating circle are equidistant from sphere center
      // Distance = minorRadius - sphereRadius (if sphere fits inside) or sphere intersects torus
      const double aDistFromSphSurface = myMinorRadius - mySphereRadius;
      if (aDistFromSphSurface < theTol)
      {
        // Sphere intersects or touches torus
        addExtremum(Value2(theU, 0), theU, theTol, true, 0.0);
      }
      else
      {
        // Sphere is inside the tube, minimum distance is minorRadius - sphereRadius
        // Pick any point on generating circle (V=0) as representative
        const gp_Pnt aTorusPt = Value2(theU, 0);
        const double aSqDist = aDistFromSphSurface * aDistFromSphSurface;
        // Sphere point toward torus - use torus XDir as representative direction
        const double aCosU = std::cos(theU);
        const double aSinU = std::sin(theU);
        const double aRadDirX = aCosU * myTorusXDirX + aSinU * myTorusYDirX;
        const double aRadDirY = aCosU * myTorusXDirY + aSinU * myTorusYDirY;
        const double aRadDirZ = aCosU * myTorusXDirZ + aSinU * myTorusYDirZ;
        const gp_Pnt aSpherePt(mySphereCenterX + mySphereRadius * aRadDirX,
                               mySphereCenterY + mySphereRadius * aRadDirY,
                               mySphereCenterZ + mySphereRadius * aRadDirZ);
        addExtremumFull(aSpherePt, aTorusPt, theU, theTol, theIsClosest, aSqDist);
      }
      return;
    }

    // Direction from circle center toward sphere center
    const double aDirX = aDx / aDistToCircCenter;
    const double aDirY = aDy / aDistToCircCenter;
    const double aDirZ = aDz / aDistToCircCenter;

    // Closest point on generating circle to sphere center
    const double aTorusPtX = aCircCentX + myMinorRadius * aDirX;
    const double aTorusPtY = aCircCentY + myMinorRadius * aDirY;
    const double aTorusPtZ = aCircCentZ + myMinorRadius * aDirZ;
    const gp_Pnt aTorusPt(aTorusPtX, aTorusPtY, aTorusPtZ);

    // Distance from torus point to sphere center
    const double aDistToCone = aDistToCircCenter - myMinorRadius;
    const double aSurfDist   = theIsClosest ? (aDistToCone - mySphereRadius) : (aDistToCone + mySphereRadius);

    if (aSurfDist < -theTol && theIsClosest)
    {
      // Surfaces intersect
      addExtremum(aTorusPt, theU, theTol, true, 0.0);
      return;
    }

    const double aSqDist = aSurfDist * aSurfDist;

    // Point on sphere
    gp_Pnt aSpherePt;
    if (theIsClosest)
    {
      // Sphere point toward torus
      aSpherePt = gp_Pnt(mySphereCenterX - mySphereRadius * aDirX,
                         mySphereCenterY - mySphereRadius * aDirY,
                         mySphereCenterZ - mySphereRadius * aDirZ);
    }
    else
    {
      // Sphere point away from torus (for maximum)
      aSpherePt = gp_Pnt(mySphereCenterX + mySphereRadius * aDirX,
                         mySphereCenterY + mySphereRadius * aDirY,
                         mySphereCenterZ + mySphereRadius * aDirZ);
    }

    addExtremumFull(aSpherePt, aTorusPt, theU, theTol, theIsClosest, aSqDist);
  }

  void processOuterExtremum(double theU, double theTol) const
  {
    // Maximum distance: farthest side of the closest generating circle
    const double aCosU      = std::cos(theU);
    const double aSinU      = std::sin(theU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    const double aDx             = mySphereCenterX - aCircCentX;
    const double aDy             = mySphereCenterY - aCircCentY;
    const double aDz             = mySphereCenterZ - aCircCentZ;
    const double aDistToCircCent = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDistToCircCent < theTol)
      return;

    // Farthest point on generating circle (opposite direction)
    const double aDirX     = -aDx / aDistToCircCent;
    const double aDirY     = -aDy / aDistToCircCent;
    const double aDirZ     = -aDz / aDistToCircCent;
    const double aTorusPtX = aCircCentX + myMinorRadius * aDirX;
    const double aTorusPtY = aCircCentY + myMinorRadius * aDirY;
    const double aTorusPtZ = aCircCentZ + myMinorRadius * aDirZ;
    const gp_Pnt aTorusPt(aTorusPtX, aTorusPtY, aTorusPtZ);

    const double aDistToSphCenter = aDistToCircCent + myMinorRadius;
    const double aSurfDist        = aDistToSphCenter + mySphereRadius;
    const double aSqDist          = aSurfDist * aSurfDist;

    const gp_Pnt aSpherePt(mySphereCenterX + mySphereRadius * aDirX,
                           mySphereCenterY + mySphereRadius * aDirY,
                           mySphereCenterZ + mySphereRadius * aDirZ);

    addExtremumFull(aSpherePt, aTorusPt, theU, theTol, false, aSqDist);
  }

  void processInnerExtremum(double theU, double theTol) const
  {
    // Minimum distance: closest side of the farthest generating circle
    const double aCosU      = std::cos(theU);
    const double aSinU      = std::sin(theU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    const double aDx             = mySphereCenterX - aCircCentX;
    const double aDy             = mySphereCenterY - aCircCentY;
    const double aDz             = mySphereCenterZ - aCircCentZ;
    const double aDistToCircCent = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDistToCircCent < theTol)
      return;

    const double aDirX     = aDx / aDistToCircCent;
    const double aDirY     = aDy / aDistToCircCent;
    const double aDirZ     = aDz / aDistToCircCent;
    const double aTorusPtX = aCircCentX + myMinorRadius * aDirX;
    const double aTorusPtY = aCircCentY + myMinorRadius * aDirY;
    const double aTorusPtZ = aCircCentZ + myMinorRadius * aDirZ;
    const gp_Pnt aTorusPt(aTorusPtX, aTorusPtY, aTorusPtZ);

    const double aDistToSphCenter = aDistToCircCent - myMinorRadius;
    const double aSurfDist        = std::abs(aDistToSphCenter - mySphereRadius);
    const double aSqDist          = aSurfDist * aSurfDist;

    const gp_Pnt aSpherePt(mySphereCenterX - mySphereRadius * aDirX,
                           mySphereCenterY - mySphereRadius * aDirY,
                           mySphereCenterZ - mySphereRadius * aDirZ);

    addExtremumFull(aSpherePt, aTorusPt, theU, theTol, true, aSqDist);
  }

  void addExtremum(const gp_Pnt& theTorusPt, double theTorusU, double theTol, bool theIsMin,
                   double theSqDist) const
  {
    // Compute torus V parameter
    const double aCosU      = std::cos(theTorusU);
    const double aSinU      = std::sin(theTorusU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    const double aLocalX = theTorusPt.X() - aCircCentX;
    const double aLocalY = theTorusPt.Y() - aCircCentY;
    const double aLocalZ = theTorusPt.Z() - aCircCentZ;

    // Project onto radial direction (in torus XY plane) and axis direction
    const double aRadialComp = aLocalX * (aCosU * myTorusXDirX + aSinU * myTorusYDirX)
                             + aLocalY * (aCosU * myTorusXDirY + aSinU * myTorusYDirY)
                             + aLocalZ * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);
    const double aAxialComp =
      aLocalX * myTorusAxisX + aLocalY * myTorusAxisY + aLocalZ * myTorusAxisZ;

    double aTorusV = std::atan2(aAxialComp, aRadialComp);
    if (aTorusV < 0)
      aTorusV += ExtremaSS::THE_TWO_PI;

    // Compute sphere point and parameters
    const double aDx  = theTorusPt.X() - mySphereCenterX;
    const double aDy  = theTorusPt.Y() - mySphereCenterY;
    const double aDz  = theTorusPt.Z() - mySphereCenterZ;
    const double aLen = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aLen < theTol)
      return;

    const gp_Pnt aSpherePt(mySphereCenterX + mySphereRadius * aDx / aLen,
                           mySphereCenterY + mySphereRadius * aDy / aLen,
                           mySphereCenterZ + mySphereRadius * aDz / aLen);

    double aSphereU, aSphereV;
    computeSphereParams(aSpherePt, aSphereU, aSphereV);

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, theTorusU, aTorusV, aSphereU, aSphereV, theTorusPt, aSpherePt,
                             theSqDist, theIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, theTorusU, aTorusV, aSpherePt, theTorusPt,
                             theSqDist, theIsMin, theTol);
    }
  }

  void addExtremumFull(const gp_Pnt& theSpherePt, const gp_Pnt& theTorusPt, double theTorusU,
                       double theTol, bool theIsMin, double theSqDist) const
  {
    // Compute torus V parameter
    const double aCosU      = std::cos(theTorusU);
    const double aSinU      = std::sin(theTorusU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    const double aLocalX = theTorusPt.X() - aCircCentX;
    const double aLocalY = theTorusPt.Y() - aCircCentY;
    const double aLocalZ = theTorusPt.Z() - aCircCentZ;

    const double aRadialComp = aLocalX * (aCosU * myTorusXDirX + aSinU * myTorusYDirX)
                             + aLocalY * (aCosU * myTorusXDirY + aSinU * myTorusYDirY)
                             + aLocalZ * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);
    const double aAxialComp =
      aLocalX * myTorusAxisX + aLocalY * myTorusAxisY + aLocalZ * myTorusAxisZ;

    double aTorusV = std::atan2(aAxialComp, aRadialComp);
    if (aTorusV < 0)
      aTorusV += ExtremaSS::THE_TWO_PI;

    // Compute sphere parameters
    double aSphereU, aSphereV;
    computeSphereParams(theSpherePt, aSphereU, aSphereV);

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, theTorusU, aTorusV, aSphereU, aSphereV, theTorusPt, theSpherePt,
                             theSqDist, theIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, aSphereU, aSphereV, theTorusU, aTorusV, theSpherePt, theTorusPt,
                             theSqDist, theIsMin, theTol);
    }
  }

  void computeSphereParams(const gp_Pnt& thePoint, double& theU, double& theV) const
  {
    const double aDx = thePoint.X() - mySphereCenterX;
    const double aDy = thePoint.Y() - mySphereCenterY;
    const double aDz = thePoint.Z() - mySphereCenterZ;

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

  const gp_Sphere& Sphere() const { return mySphere; }
  const gp_Torus&  Torus() const { return myTorus; }
  bool             IsSwapped() const { return mySwapped; }
  bool             IsBounded() const { return myDomain.has_value(); }

private:
  gp_Sphere                          mySphere;
  gp_Torus                           myTorus;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Cached sphere components
  gp_Pnt mySphereCenter;
  double mySphereCenterX, mySphereCenterY, mySphereCenterZ;
  double mySphereRadius;
  double mySphereXDirX, mySphereXDirY, mySphereXDirZ;
  double mySphereYDirX, mySphereYDirY, mySphereYDirZ;
  double mySphereZDirX, mySphereZDirY, mySphereZDirZ;

  // Cached torus components
  gp_Pnt myTorusCenter;
  double myTorusCenterX, myTorusCenterY, myTorusCenterZ;
  double myTorusAxisX, myTorusAxisY, myTorusAxisZ;
  double myTorusXDirX, myTorusXDirY, myTorusXDirZ;
  double myTorusYDirX, myTorusYDirY, myTorusYDirZ;
  double myMajorRadius, myMinorRadius;

  // Cached geometric relationships
  double myCenterToCenterX, myCenterToCenterY, myCenterToCenterZ;
  double myHeightAbovePlane;
  double myRadialDistFromAxis;
  double myRadialDirX, myRadialDirY, myRadialDirZ;
  bool   myCenterOnAxis;
};

#endif // _ExtremaSS_SphereTorus_HeaderFile
