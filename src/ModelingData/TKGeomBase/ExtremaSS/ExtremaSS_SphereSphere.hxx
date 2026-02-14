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

#ifndef _ExtremaSS_SphereSphere_HeaderFile
#define _ExtremaSS_SphereSphere_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Sphere-Sphere extrema computation.
//!
//! Computes the extrema between two spherical surfaces.
//!
//! Algorithm:
//! 1. Compute distance d between sphere centers
//! 2. If d = 0 (concentric spheres): infinite solutions at distance |R1 - R2|
//! 3. Otherwise:
//!    - Minimum distance = |d - R1 - R2| (if d > R1 + R2, otherwise spheres intersect)
//!    - Maximum distance = d + R1 + R2
//!    - Both extrema are on the line connecting centers
//!
//! @note For concentric spheres (same center), there are infinite solutions
//!       at all (U, V) pairs where inner sphere touches outer - returns InfiniteSolutions.
//!
//! @note This class handles both (Sphere1, Sphere2) and (Sphere2, Sphere1) orderings.
class ExtremaSS_SphereSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two sphere geometries (uses full sphere domains).
  //! @param[in] theSphere1 first sphere
  //! @param[in] theSphere2 second sphere
  ExtremaSS_SphereSphere(const gp_Sphere& theSphere1, const gp_Sphere& theSphere2)
      : mySphere1(theSphere1),
        mySphere2(theSphere2),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with two sphere geometries and parameter domains.
  //! @param[in] theSphere1 first sphere
  //! @param[in] theSphere2 second sphere
  //! @param[in] theDomain parameter domains for both spheres
  ExtremaSS_SphereSphere(const gp_Sphere&             theSphere1,
                         const gp_Sphere&             theSphere2,
                         const ExtremaSS::Domain4D& theDomain)
      : mySphere1(theSphere1),
        mySphere2(theSphere2),
        myDomain(theDomain)
  {
    initCache();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache sphere 1 components
    const gp_Ax3& aPos1    = mySphere1.Position();
    const gp_Pnt& aCenter1 = aPos1.Location();
    myCenter1X             = aCenter1.X();
    myCenter1Y             = aCenter1.Y();
    myCenter1Z             = aCenter1.Z();

    const gp_Dir& aAxis1 = aPos1.Direction();
    myAxis1X             = aAxis1.X();
    myAxis1Y             = aAxis1.Y();
    myAxis1Z             = aAxis1.Z();

    const gp_Dir& aXDir1 = aPos1.XDirection();
    myXDir1X             = aXDir1.X();
    myXDir1Y             = aXDir1.Y();
    myXDir1Z             = aXDir1.Z();

    const gp_Dir& aYDir1 = aPos1.YDirection();
    myYDir1X             = aYDir1.X();
    myYDir1Y             = aYDir1.Y();
    myYDir1Z             = aYDir1.Z();

    myRadius1 = mySphere1.Radius();

    // Cache sphere 2 components
    const gp_Ax3& aPos2    = mySphere2.Position();
    const gp_Pnt& aCenter2 = aPos2.Location();
    myCenter2X             = aCenter2.X();
    myCenter2Y             = aCenter2.Y();
    myCenter2Z             = aCenter2.Z();

    const gp_Dir& aAxis2 = aPos2.Direction();
    myAxis2X             = aAxis2.X();
    myAxis2Y             = aAxis2.Y();
    myAxis2Z             = aAxis2.Z();

    const gp_Dir& aXDir2 = aPos2.XDirection();
    myXDir2X             = aXDir2.X();
    myXDir2Y             = aXDir2.Y();
    myXDir2Z             = aXDir2.Z();

    const gp_Dir& aYDir2 = aPos2.YDirection();
    myYDir2X             = aYDir2.X();
    myYDir2Y             = aYDir2.Y();
    myYDir2Z             = aYDir2.Z();

    myRadius2 = mySphere2.Radius();

    // Precompute center-to-center vector
    myCenterDx  = myCenter2X - myCenter1X;
    myCenterDy  = myCenter2Y - myCenter1Y;
    myCenterDz  = myCenter2Z - myCenter1Z;
    myCenterDistSq = myCenterDx * myCenterDx + myCenterDy * myCenterDy + myCenterDz * myCenterDz;
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface 1 point at given parameters.
  //! @param theU U parameter (azimuth angle)
  //! @param theV V parameter (elevation angle, -PI/2 to PI/2)
  //! @return point on sphere 1 at (U, V)
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU  = std::cos(theU);
    const double aSinU  = std::sin(theU);
    const double aCosV  = std::cos(theV);
    const double aSinV  = std::sin(theV);
    const double aRcosV = myRadius1 * aCosV;
    return gp_Pnt(
      myCenter1X + aRcosV * (aCosU * myXDir1X + aSinU * myYDir1X) + myRadius1 * aSinV * myAxis1X,
      myCenter1Y + aRcosV * (aCosU * myXDir1Y + aSinU * myYDir1Y) + myRadius1 * aSinV * myAxis1Y,
      myCenter1Z + aRcosV * (aCosU * myXDir1Z + aSinU * myYDir1Z) + myRadius1 * aSinV * myAxis1Z);
  }

  //! Evaluate surface 2 point at given parameters.
  //! @param theU U parameter (azimuth angle)
  //! @param theV V parameter (elevation angle, -PI/2 to PI/2)
  //! @return point on sphere 2 at (U, V)
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU  = std::cos(theU);
    const double aSinU  = std::sin(theU);
    const double aCosV  = std::cos(theV);
    const double aSinV  = std::sin(theV);
    const double aRcosV = myRadius2 * aCosV;
    return gp_Pnt(
      myCenter2X + aRcosV * (aCosU * myXDir2X + aSinU * myYDir2X) + myRadius2 * aSinV * myAxis2X,
      myCenter2Y + aRcosV * (aCosU * myXDir2Y + aSinU * myYDir2Y) + myRadius2 * aSinV * myAxis2Y,
      myCenter2Z + aRcosV * (aCosU * myXDir2Z + aSinU * myYDir2Z) + myRadius2 * aSinV * myAxis2Z);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only (min and max between spheres).
  //!
  //! For two spheres, there are exactly two interior extrema:
  //! - Minimum: closest points on line connecting centers
  //! - Maximum: farthest points on line connecting centers (antipodal)
  //!
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Check for concentric spheres (centers coincide)
    if (myCenterDistSq < theTol * theTol)
    {
      // Concentric spheres - infinite solutions
      const double aRadDiff = std::abs(myRadius1 - myRadius2);
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aRadDiff * aRadDiff;
      return myResult;
    }

    const double aCenterDist    = std::sqrt(myCenterDistSq);
    const double aInvCenterDist = 1.0 / aCenterDist;

    // Normalized direction from center1 to center2
    const double aDirX = myCenterDx * aInvCenterDist;
    const double aDirY = myCenterDy * aInvCenterDist;
    const double aDirZ = myCenterDz * aInvCenterDist;

    // Compute UV parameters for extremum points
    // Point on sphere 1 toward sphere 2: center1 + R1 * dir
    // Point on sphere 2 toward sphere 1: center2 - R2 * dir

    // For sphere 1: compute UV from direction (center1 -> center2)
    double aU1Min, aV1Min;
    computeUVFromDirection(aDirX, aDirY, aDirZ, myAxis1X, myAxis1Y, myAxis1Z, myXDir1X, myXDir1Y,
                           myXDir1Z, myYDir1X, myYDir1Y, myYDir1Z, theTol, aU1Min, aV1Min);

    // For sphere 2: compute UV from direction (center2 -> center1), which is -dir
    double aU2Min, aV2Min;
    computeUVFromDirection(-aDirX, -aDirY, -aDirZ, myAxis2X, myAxis2Y, myAxis2Z, myXDir2X, myXDir2Y,
                           myXDir2Z, myYDir2X, myYDir2Y, myYDir2Z, theTol, aU2Min, aV2Min);

    // Minimum extremum
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aPt1(myCenter1X + myRadius1 * aDirX, myCenter1Y + myRadius1 * aDirY,
                        myCenter1Z + myRadius1 * aDirZ);
      const gp_Pnt aPt2(myCenter2X - myRadius2 * aDirX, myCenter2Y - myRadius2 * aDirY,
                        myCenter2Z - myRadius2 * aDirZ);

      const double aMinDist = aCenterDist - myRadius1 - myRadius2;
      const double aMinSqDist =
        (aMinDist > 0.0) ? aMinDist * aMinDist : 0.0; // Spheres may intersect

      ExtremaSS::AddExtremum(myResult, aU1Min, aV1Min, aU2Min, aV2Min, aPt1, aPt2, aMinSqDist, true,
                             theTol);
    }

    // Maximum extremum (antipodal points)
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Point on sphere 1 away from sphere 2: center1 - R1 * dir
      // Point on sphere 2 away from sphere 1: center2 + R2 * dir
      const gp_Pnt aPt1(myCenter1X - myRadius1 * aDirX, myCenter1Y - myRadius1 * aDirY,
                        myCenter1Z - myRadius1 * aDirZ);
      const gp_Pnt aPt2(myCenter2X + myRadius2 * aDirX, myCenter2Y + myRadius2 * aDirY,
                        myCenter2Z + myRadius2 * aDirZ);

      const double aMaxDist   = aCenterDist + myRadius1 + myRadius2;
      const double aMaxSqDist = aMaxDist * aMaxDist;

      // Compute UV for antipodal points
      double aU1Max, aV1Max;
      computeUVFromDirection(-aDirX, -aDirY, -aDirZ, myAxis1X, myAxis1Y, myAxis1Z, myXDir1X,
                             myXDir1Y, myXDir1Z, myYDir1X, myYDir1Y, myYDir1Z, theTol, aU1Max,
                             aV1Max);

      double aU2Max, aV2Max;
      computeUVFromDirection(aDirX, aDirY, aDirZ, myAxis2X, myAxis2Y, myAxis2Z, myXDir2X, myXDir2Y,
                             myXDir2Z, myYDir2X, myYDir2Y, myYDir2Z, theTol, aU2Max, aV2Max);

      ExtremaSS::AddExtremum(myResult, aU1Max, aV1Max, aU2Max, aV2Max, aPt1, aPt2, aMaxSqDist, false,
                             theTol);
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution
                                                 : ExtremaSS::Status::OK;
    return myResult;
  }

  //! Find extrema including boundary edges and corners.
  //!
  //! For bounded sphere domains, adds boundary extrema.
  //!
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    (void)Perform(theTol, theMode);

    // If infinite solutions or no bounded domain, return immediately
    if (myResult.IsInfinite() || !myDomain.has_value())
    {
      return myResult;
    }

    // Add boundary extrema
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

  //! @}

  //! Returns the first sphere geometry.
  const gp_Sphere& Sphere1() const { return mySphere1; }

  //! Returns the second sphere geometry.
  const gp_Sphere& Sphere2() const { return mySphere2; }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaSS::Domain4D& Domain() const { return *myDomain; }

private:
  //! Compute UV parameters from a direction vector on a sphere.
  //! @param theDirX, theDirY, theDirZ - direction from center
  //! @param theAxisX, theAxisY, theAxisZ - sphere axis (Z direction)
  //! @param theXDirX, theXDirY, theXDirZ - sphere X direction
  //! @param theYDirX, theYDirY, theYDirZ - sphere Y direction
  //! @param theTol - tolerance for pole detection
  //! @param theU, theV - output UV parameters
  static void computeUVFromDirection(double theDirX,
                                     double theDirY,
                                     double theDirZ,
                                     double theAxisX,
                                     double theAxisY,
                                     double theAxisZ,
                                     double theXDirX,
                                     double theXDirY,
                                     double theXDirZ,
                                     double theYDirX,
                                     double theYDirY,
                                     double theYDirZ,
                                     double theTol,
                                     double& theU,
                                     double& theV)
  {
    // V (elevation) from direction dot axis
    double aSinV = theDirX * theAxisX + theDirY * theAxisY + theDirZ * theAxisZ;
    aSinV        = aSinV < -1.0 ? -1.0 : (aSinV > 1.0 ? 1.0 : aSinV);
    theV         = std::asin(aSinV);

    // U (azimuth) from direction
    const double aCosV = std::cos(theV);
    theU               = 0.0;

    if (std::abs(aCosV) > theTol)
    {
      const double aInvCosV = 1.0 / aCosV;
      const double aCosU = (theDirX * theXDirX + theDirY * theXDirY + theDirZ * theXDirZ) * aInvCosV;
      const double aSinU = (theDirX * theYDirX + theDirY * theYDirY + theDirZ * theYDirZ) * aInvCosV;
      theU               = std::atan2(aSinU, aCosU);
      if (theU < 0.0)
        theU += ExtremaSS::THE_TWO_PI;
    }
  }

  //! Check boundary extrema for bounded domains.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
    {
      return;
    }

    const ExtremaSS::Domain4D&   aDom  = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    // Sample boundary edges
    constexpr int aNbSamples = 20;

    // Check boundaries of domain 1 against interior of domain 2
    // V1 = VMin1 and V1 = VMax1 edges
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkPointAgainstSphere2(aU1, aDom1.VMin, theTol, theMode);
      checkPointAgainstSphere2(aU1, aDom1.VMax, theTol, theMode);
    }

    // U1 = UMin1 and U1 = UMax1 edges
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkPointAgainstSphere2(aDom1.UMin, aV1, theTol, theMode);
      checkPointAgainstSphere2(aDom1.UMax, aV1, theTol, theMode);
    }

    // Similarly for domain 2 boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkPointAgainstSphere1(aU2, aDom2.VMin, theTol, theMode);
      checkPointAgainstSphere1(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkPointAgainstSphere1(aDom2.UMin, aV2, theTol, theMode);
      checkPointAgainstSphere1(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on sphere 1 against sphere 2 for potential extrema.
  void checkPointAgainstSphere2(double theU1, double theV1, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Direction from sphere 2 center to P1
    const double aDx = aP1.X() - myCenter2X;
    const double aDy = aP1.Y() - myCenter2Y;
    const double aDz = aP1.Z() - myCenter2Z;
    const double aDist = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDist < theTol)
    {
      // P1 is at center of sphere 2, any point on sphere 2 is equidistant
      return;
    }

    // Normalize direction
    const double aInvDist = 1.0 / aDist;
    const double aNx = aDx * aInvDist;
    const double aNy = aDy * aInvDist;
    const double aNz = aDz * aInvDist;

    // Compute UV on sphere 2 for closest point (toward P1)
    double aU2Min = 0.0, aV2Min = 0.0;
    computeUVFromDirection(aNx, aNy, aNz,
                           myAxis2X, myAxis2Y, myAxis2Z,
                           myXDir2X, myXDir2Y, myXDir2Z,
                           myYDir2X, myYDir2Y, myYDir2Z,
                           theTol, aU2Min, aV2Min);

    // Check if UV is within domain
    if (myDomain.has_value())
    {
      const MathUtils::Domain2D& aDom2 = myDomain->Domain2;
      if (aU2Min < aDom2.UMin - theTol || aU2Min > aDom2.UMax + theTol ||
          aV2Min < aDom2.VMin - theTol || aV2Min > aDom2.VMax + theTol)
      {
        // Clamp to boundary and use that point instead
        aU2Min = std::clamp(aU2Min, aDom2.UMin, aDom2.UMax);
        aV2Min = std::clamp(aV2Min, aDom2.VMin, aDom2.VMax);
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP2 = Value2(aU2Min, aV2Min);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, theU1, theV1, aU2Min, aV2Min, aP1, aP2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Farthest point is in opposite direction
      double aU2Max = 0.0, aV2Max = 0.0;
      computeUVFromDirection(-aNx, -aNy, -aNz,
                             myAxis2X, myAxis2Y, myAxis2Z,
                             myXDir2X, myXDir2Y, myXDir2Z,
                             myYDir2X, myYDir2Y, myYDir2Z,
                             theTol, aU2Max, aV2Max);

      if (myDomain.has_value())
      {
        const MathUtils::Domain2D& aDom2 = myDomain->Domain2;
        aU2Max = std::clamp(aU2Max, aDom2.UMin, aDom2.UMax);
        aV2Max = std::clamp(aV2Max, aDom2.VMin, aDom2.VMax);
      }

      const gp_Pnt aP2 = Value2(aU2Max, aV2Max);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, theU1, theV1, aU2Max, aV2Max, aP1, aP2, aSqDist, false, theTol);
    }
  }

  //! Check a point on sphere 2 against sphere 1 for potential extrema.
  void checkPointAgainstSphere1(double theU2, double theV2, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    // Direction from sphere 1 center to P2
    const double aDx = aP2.X() - myCenter1X;
    const double aDy = aP2.Y() - myCenter1Y;
    const double aDz = aP2.Z() - myCenter1Z;
    const double aDist = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDist < theTol)
    {
      // P2 is at center of sphere 1
      return;
    }

    // Normalize direction
    const double aInvDist = 1.0 / aDist;
    const double aNx = aDx * aInvDist;
    const double aNy = aDy * aInvDist;
    const double aNz = aDz * aInvDist;

    // Compute UV on sphere 1 for closest point (toward P2)
    double aU1Min = 0.0, aV1Min = 0.0;
    computeUVFromDirection(aNx, aNy, aNz,
                           myAxis1X, myAxis1Y, myAxis1Z,
                           myXDir1X, myXDir1Y, myXDir1Z,
                           myYDir1X, myYDir1Y, myYDir1Z,
                           theTol, aU1Min, aV1Min);

    // Check if UV is within domain
    if (myDomain.has_value())
    {
      const MathUtils::Domain2D& aDom1 = myDomain->Domain1;
      if (aU1Min < aDom1.UMin - theTol || aU1Min > aDom1.UMax + theTol ||
          aV1Min < aDom1.VMin - theTol || aV1Min > aDom1.VMax + theTol)
      {
        aU1Min = std::clamp(aU1Min, aDom1.UMin, aDom1.UMax);
        aV1Min = std::clamp(aV1Min, aDom1.VMin, aDom1.VMax);
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP1 = Value1(aU1Min, aV1Min);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, aU1Min, aV1Min, theU2, theV2, aP1, aP2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Farthest point is in opposite direction
      double aU1Max = 0.0, aV1Max = 0.0;
      computeUVFromDirection(-aNx, -aNy, -aNz,
                             myAxis1X, myAxis1Y, myAxis1Z,
                             myXDir1X, myXDir1Y, myXDir1Z,
                             myYDir1X, myYDir1Y, myYDir1Z,
                             theTol, aU1Max, aV1Max);

      if (myDomain.has_value())
      {
        const MathUtils::Domain2D& aDom1 = myDomain->Domain1;
        aU1Max = std::clamp(aU1Max, aDom1.UMin, aDom1.UMax);
        aV1Max = std::clamp(aV1Max, aDom1.VMin, aDom1.VMax);
      }

      const gp_Pnt aP1 = Value1(aU1Max, aV1Max);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, aU1Max, aV1Max, theU2, theV2, aP1, aP2, aSqDist, false, theTol);
    }
  }

private:
  gp_Sphere                            mySphere1; //!< First sphere geometry
  gp_Sphere                            mySphere2; //!< Second sphere geometry
  std::optional<ExtremaSS::Domain4D> myDomain; //!< Parameter domain (nullopt for natural)
  mutable ExtremaSS::Result            myResult;  //!< Reusable result storage

  // Cached components for sphere 1
  double myCenter1X, myCenter1Y, myCenter1Z; //!< Sphere 1 center
  double myAxis1X, myAxis1Y, myAxis1Z;       //!< Sphere 1 axis (Z direction)
  double myXDir1X, myXDir1Y, myXDir1Z;       //!< Sphere 1 X direction
  double myYDir1X, myYDir1Y, myYDir1Z;       //!< Sphere 1 Y direction
  double myRadius1;                          //!< Sphere 1 radius

  // Cached components for sphere 2
  double myCenter2X, myCenter2Y, myCenter2Z; //!< Sphere 2 center
  double myAxis2X, myAxis2Y, myAxis2Z;       //!< Sphere 2 axis (Z direction)
  double myXDir2X, myXDir2Y, myXDir2Z;       //!< Sphere 2 X direction
  double myYDir2X, myYDir2Y, myYDir2Z;       //!< Sphere 2 Y direction
  double myRadius2;                          //!< Sphere 2 radius

  // Cached center-to-center data
  double myCenterDx, myCenterDy, myCenterDz; //!< Vector from center1 to center2
  double myCenterDistSq;                      //!< Squared distance between centers
};

#endif // _ExtremaSS_SphereSphere_HeaderFile
