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

#ifndef _ExtremaSS_CylinderSphere_HeaderFile
#define _ExtremaSS_CylinderSphere_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Cylinder-Sphere extrema computation.
//!
//! Computes the extrema between a cylindrical surface and a spherical surface.
//! This class handles both (Cylinder, Sphere) and (Sphere, Cylinder) orderings.
//!
//! Algorithm:
//! 1. Find the closest point on cylinder axis to sphere center
//! 2. Compute distance d from sphere center to cylinder axis
//! 3. If d = 0 (center on axis): infinite solutions at a ring
//! 4. Otherwise:
//!    - Minimum distance = |d - Rcyl| - Rsph (if |d - Rcyl| > Rsph)
//!    - Maximum distance = d + Rcyl + Rsph
//!    - Extrema are on line from sphere center perpendicular to cylinder axis
//!
//! @note If sphere center lies on cylinder axis, there are infinite
//!       solutions at all U values for the closest V - returns InfiniteSolutions.
class ExtremaSS_CylinderSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with cylinder and sphere (canonical order).
  //! @param[in] theCylinder the cylinder
  //! @param[in] theSphere the sphere
  ExtremaSS_CylinderSphere(const gp_Cylinder& theCylinder, const gp_Sphere& theSphere)
      : myCylinder(theCylinder),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere and cylinder (swapped order).
  //! @param[in] theSphere the sphere
  //! @param[in] theCylinder the cylinder
  ExtremaSS_CylinderSphere(const gp_Sphere& theSphere, const gp_Cylinder& theCylinder)
      : myCylinder(theCylinder),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with cylinder, sphere, and domain.
  //! @param[in] theCylinder the cylinder
  //! @param[in] theSphere the sphere
  //! @param[in] theDomain parameter domains
  ExtremaSS_CylinderSphere(const gp_Cylinder&         theCylinder,
                           const gp_Sphere&           theSphere,
                           const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        mySphere(theSphere),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere, cylinder, and domain (swapped order).
  //! @param[in] theSphere the sphere
  //! @param[in] theCylinder the cylinder
  //! @param[in] theDomain parameter domains (will be swapped internally)
  ExtremaSS_CylinderSphere(const gp_Sphere&           theSphere,
                           const gp_Cylinder&         theCylinder,
                           const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        mySphere(theSphere),
        myDomain(ExtremaSS::Domain4D(theDomain.Domain2, theDomain.Domain1)),
        mySwapped(true)
  {
    initCache();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache cylinder components
    const gp_Ax3& aCylPos = myCylinder.Position();
    const gp_Pnt& aCylOrig = aCylPos.Location();
    myCylOrigX = aCylOrig.X();
    myCylOrigY = aCylOrig.Y();
    myCylOrigZ = aCylOrig.Z();

    const gp_Dir& aCylAxis = aCylPos.Direction();
    myCylAxisX = aCylAxis.X();
    myCylAxisY = aCylAxis.Y();
    myCylAxisZ = aCylAxis.Z();

    const gp_Dir& aCylXDir = aCylPos.XDirection();
    myCylXDirX = aCylXDir.X();
    myCylXDirY = aCylXDir.Y();
    myCylXDirZ = aCylXDir.Z();

    const gp_Dir& aCylYDir = aCylPos.YDirection();
    myCylYDirX = aCylYDir.X();
    myCylYDirY = aCylYDir.Y();
    myCylYDirZ = aCylYDir.Z();

    myCylRadius = myCylinder.Radius();

    // Cache sphere components
    const gp_Ax3& aSphPos = mySphere.Position();
    const gp_Pnt& aSphCenter = aSphPos.Location();
    mySphCenterX = aSphCenter.X();
    mySphCenterY = aSphCenter.Y();
    mySphCenterZ = aSphCenter.Z();

    const gp_Dir& aSphAxis = aSphPos.Direction();
    mySphAxisX = aSphAxis.X();
    mySphAxisY = aSphAxis.Y();
    mySphAxisZ = aSphAxis.Z();

    const gp_Dir& aSphXDir = aSphPos.XDirection();
    mySphXDirX = aSphXDir.X();
    mySphXDirY = aSphXDir.Y();
    mySphXDirZ = aSphXDir.Z();

    const gp_Dir& aSphYDir = aSphPos.YDirection();
    mySphYDirX = aSphYDir.X();
    mySphYDirY = aSphYDir.Y();
    mySphYDirZ = aSphYDir.Z();

    mySphRadius = mySphere.Radius();

    // Compute vector from cylinder origin to sphere center
    const double aDx = mySphCenterX - myCylOrigX;
    const double aDy = mySphCenterY - myCylOrigY;
    const double aDz = mySphCenterZ - myCylOrigZ;

    // Project onto cylinder axis to find closest point
    // t = (sphereCenter - cylOrigin) . cylAxis
    myAxisParam = aDx * myCylAxisX + aDy * myCylAxisY + aDz * myCylAxisZ;

    // Closest point on axis
    myClosestOnAxisX = myCylOrigX + myAxisParam * myCylAxisX;
    myClosestOnAxisY = myCylOrigY + myAxisParam * myCylAxisY;
    myClosestOnAxisZ = myCylOrigZ + myAxisParam * myCylAxisZ;

    // Vector from axis to sphere center (perpendicular to axis)
    myPerpX = mySphCenterX - myClosestOnAxisX;
    myPerpY = mySphCenterY - myClosestOnAxisY;
    myPerpZ = mySphCenterZ - myClosestOnAxisZ;

    // Distance from sphere center to cylinder axis
    myAxisDistSq = myPerpX * myPerpX + myPerpY * myPerpY + myPerpZ * myPerpZ;
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate cylinder point at given parameters.
  //! @param theU U parameter (angle around axis)
  //! @param theV V parameter (height along axis)
  //! @return point on cylinder at (U, V)
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(myCylOrigX + myCylRadius * (aCosU * myCylXDirX + aSinU * myCylYDirX)
                    + theV * myCylAxisX,
                  myCylOrigY + myCylRadius * (aCosU * myCylXDirY + aSinU * myCylYDirY)
                    + theV * myCylAxisY,
                  myCylOrigZ + myCylRadius * (aCosU * myCylXDirZ + aSinU * myCylYDirZ)
                    + theV * myCylAxisZ);
  }

  //! Evaluate sphere point at given parameters.
  //! @param theU U parameter (azimuth)
  //! @param theV V parameter (elevation)
  //! @return point on sphere at (U, V)
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU  = std::cos(theU);
    const double aSinU  = std::sin(theU);
    const double aCosV  = std::cos(theV);
    const double aSinV  = std::sin(theV);
    const double aRcosV = mySphRadius * aCosV;
    return gp_Pnt(mySphCenterX + aRcosV * (aCosU * mySphXDirX + aSinU * mySphYDirX)
                    + mySphRadius * aSinV * mySphAxisX,
                  mySphCenterY + aRcosV * (aCosU * mySphXDirY + aSinU * mySphYDirY)
                    + mySphRadius * aSinV * mySphAxisY,
                  mySphCenterZ + aRcosV * (aCosU * mySphXDirZ + aSinU * mySphYDirZ)
                    + mySphRadius * aSinV * mySphAxisZ);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //!
  //! For cylinder-sphere, there are typically two interior extrema:
  //! - Minimum: closest points (sphere toward cylinder axis, cylinder toward sphere)
  //! - Maximum: farthest points (opposite direction)
  //!
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Check if sphere center is on cylinder axis
    if (myAxisDistSq < theTol * theTol)
    {
      // Sphere center on cylinder axis - infinite solutions at a ring
      const double aRadDiff = std::abs(myCylRadius - mySphRadius);
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aRadDiff * aRadDiff;
      return myResult;
    }

    const double aAxisDist    = std::sqrt(myAxisDistSq);
    const double aInvAxisDist = 1.0 / aAxisDist;

    // Normalized direction from axis to sphere center
    const double aDirX = myPerpX * aInvAxisDist;
    const double aDirY = myPerpY * aInvAxisDist;
    const double aDirZ = myPerpZ * aInvAxisDist;

    // Cylinder V parameter at closest point on axis
    const double aCylV = myAxisParam;

    // Minimum extremum: cylinder point toward sphere, sphere point toward cylinder
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Cylinder point: axis + Rcyl * dir
      const double aCylPtX = myClosestOnAxisX + myCylRadius * aDirX;
      const double aCylPtY = myClosestOnAxisY + myCylRadius * aDirY;
      const double aCylPtZ = myClosestOnAxisZ + myCylRadius * aDirZ;
      const gp_Pnt aCylPt(aCylPtX, aCylPtY, aCylPtZ);

      // Cylinder U parameter
      const double aCylU = std::atan2(aDirX * myCylYDirX + aDirY * myCylYDirY + aDirZ * myCylYDirZ,
                                      aDirX * myCylXDirX + aDirY * myCylXDirY + aDirZ * myCylXDirZ);
      const double aCylUNorm = (aCylU < 0.0) ? aCylU + ExtremaSS::THE_TWO_PI : aCylU;

      // Sphere point: center - Rsph * dir (toward cylinder)
      const double aSphPtX = mySphCenterX - mySphRadius * aDirX;
      const double aSphPtY = mySphCenterY - mySphRadius * aDirY;
      const double aSphPtZ = mySphCenterZ - mySphRadius * aDirZ;
      const gp_Pnt aSphPt(aSphPtX, aSphPtY, aSphPtZ);

      // Sphere UV from direction -dir
      double aSphU, aSphV;
      computeSphereUVFromDirection(-aDirX, -aDirY, -aDirZ, theTol, aSphU, aSphV);

      const double aMinDist = aAxisDist - myCylRadius - mySphRadius;
      const double aMinSqDist = (aMinDist > 0.0) ? aMinDist * aMinDist : 0.0;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aSphU, aSphV, aCylUNorm, aCylV, aSphPt, aCylPt, aMinSqDist,
                               true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aCylUNorm, aCylV, aSphU, aSphV, aCylPt, aSphPt, aMinSqDist,
                               true, theTol);
      }
    }

    // Maximum extremum: cylinder point away from sphere, sphere point away from cylinder
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Cylinder point: axis - Rcyl * dir
      const double aCylPtX = myClosestOnAxisX - myCylRadius * aDirX;
      const double aCylPtY = myClosestOnAxisY - myCylRadius * aDirY;
      const double aCylPtZ = myClosestOnAxisZ - myCylRadius * aDirZ;
      const gp_Pnt aCylPt(aCylPtX, aCylPtY, aCylPtZ);

      // Cylinder U parameter (opposite direction)
      const double aCylU =
        std::atan2(-aDirX * myCylYDirX - aDirY * myCylYDirY - aDirZ * myCylYDirZ,
                   -aDirX * myCylXDirX - aDirY * myCylXDirY - aDirZ * myCylXDirZ);
      const double aCylUNorm = (aCylU < 0.0) ? aCylU + ExtremaSS::THE_TWO_PI : aCylU;

      // Sphere point: center + Rsph * dir (away from cylinder)
      const double aSphPtX = mySphCenterX + mySphRadius * aDirX;
      const double aSphPtY = mySphCenterY + mySphRadius * aDirY;
      const double aSphPtZ = mySphCenterZ + mySphRadius * aDirZ;
      const gp_Pnt aSphPt(aSphPtX, aSphPtY, aSphPtZ);

      // Sphere UV from direction +dir
      double aSphU, aSphV;
      computeSphereUVFromDirection(aDirX, aDirY, aDirZ, theTol, aSphU, aSphV);

      const double aMaxDist   = aAxisDist + myCylRadius + mySphRadius;
      const double aMaxSqDist = aMaxDist * aMaxDist;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aSphU, aSphV, aCylUNorm, aCylV, aSphPt, aCylPt, aMaxSqDist,
                               false, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aCylUNorm, aCylV, aSphU, aSphV, aCylPt, aSphPt, aMaxSqDist,
                               false, theTol);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution
                                                 : ExtremaSS::Status::OK;
    return myResult;
  }

  //! Find extrema including boundary.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);

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

  //! Returns the cylinder geometry.
  const gp_Cylinder& Cylinder() const { return myCylinder; }

  //! Returns the sphere geometry.
  const gp_Sphere& Sphere() const { return mySphere; }

  //! Returns true if surfaces were swapped.
  bool IsSwapped() const { return mySwapped; }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Compute UV on sphere from direction vector.
  void computeSphereUVFromDirection(double theDirX,
                                    double theDirY,
                                    double theDirZ,
                                    double theTol,
                                    double& theU,
                                    double& theV) const
  {
    // V (elevation) from direction dot axis
    double aSinV = theDirX * mySphAxisX + theDirY * mySphAxisY + theDirZ * mySphAxisZ;
    aSinV        = aSinV < -1.0 ? -1.0 : (aSinV > 1.0 ? 1.0 : aSinV);
    theV         = std::asin(aSinV);

    // U (azimuth) from direction
    const double aCosV = std::cos(theV);
    theU               = 0.0;

    if (std::abs(aCosV) > theTol)
    {
      const double aInvCosV = 1.0 / aCosV;
      const double aCosU =
        (theDirX * mySphXDirX + theDirY * mySphXDirY + theDirZ * mySphXDirZ) * aInvCosV;
      const double aSinU =
        (theDirX * mySphYDirX + theDirY * mySphYDirY + theDirZ * mySphYDirZ) * aInvCosV;
      theU = std::atan2(aSinU, aCosU);
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
    const MathUtils::Domain2D& aDom1 = aDom.Domain1; // Cylinder domain
    const MathUtils::Domain2D& aDom2 = aDom.Domain2; // Sphere domain

    constexpr int aNbSamples = 20;

    // Check cylinder boundary against sphere
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkCylinderPointAgainstSphere(aU1, aDom1.VMin, theTol, theMode);
      checkCylinderPointAgainstSphere(aU1, aDom1.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkCylinderPointAgainstSphere(aDom1.UMin, aV1, theTol, theMode);
      checkCylinderPointAgainstSphere(aDom1.UMax, aV1, theTol, theMode);
    }

    // Check sphere boundary against cylinder
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkSpherePointAgainstCylinder(aU2, aDom2.VMin, theTol, theMode);
      checkSpherePointAgainstCylinder(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkSpherePointAgainstCylinder(aDom2.UMin, aV2, theTol, theMode);
      checkSpherePointAgainstCylinder(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on cylinder against sphere for potential extrema.
  void checkCylinderPointAgainstSphere(double theU1, double theV1, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Direction from sphere center to cylinder point
    const double aDx = aP1.X() - mySphCenterX;
    const double aDy = aP1.Y() - mySphCenterY;
    const double aDz = aP1.Z() - mySphCenterZ;
    const double aDist = std::sqrt(aDx * aDx + aDy * aDy + aDz * aDz);

    if (aDist < theTol)
    {
      return;
    }

    const double aInvDist = 1.0 / aDist;
    const double aNx = aDx * aInvDist;
    const double aNy = aDy * aInvDist;
    const double aNz = aDz * aInvDist;

    // Closest point on sphere (toward cylinder point)
    double aU2Min = 0.0, aV2Min = 0.0;
    computeSphereUVFromDirection(aNx, aNy, aNz, theTol, aU2Min, aV2Min);

    if (myDomain.has_value())
    {
      const MathUtils::Domain2D& aDom2 = myDomain->Domain2;
      aU2Min = std::clamp(aU2Min, aDom2.UMin, aDom2.UMax);
      aV2Min = std::clamp(aV2Min, aDom2.VMin, aDom2.VMax);
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP2 = Value2(aU2Min, aV2Min);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, theU1, theV1, aU2Min, aV2Min, aP1, aP2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      double aU2Max = 0.0, aV2Max = 0.0;
      computeSphereUVFromDirection(-aNx, -aNy, -aNz, theTol, aU2Max, aV2Max);

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

  //! Check a point on sphere against cylinder for potential extrema.
  void checkSpherePointAgainstCylinder(double theU2, double theV2, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    // Project sphere point onto cylinder axis
    const double aVecX = aP2.X() - myCylOrigX;
    const double aVecY = aP2.Y() - myCylOrigY;
    const double aVecZ = aP2.Z() - myCylOrigZ;
    double aV1 = aVecX * myCylAxisX + aVecY * myCylAxisY + aVecZ * myCylAxisZ;

    // Clamp V to domain
    if (myDomain.has_value())
    {
      aV1 = std::clamp(aV1, myDomain->Domain1.VMin, myDomain->Domain1.VMax);
    }

    // Point on axis at V1
    const double aAxisPtX = myCylOrigX + aV1 * myCylAxisX;
    const double aAxisPtY = myCylOrigY + aV1 * myCylAxisY;
    const double aAxisPtZ = myCylOrigZ + aV1 * myCylAxisZ;

    // Direction from axis to sphere point (perpendicular to axis)
    double aPerpX = aP2.X() - aAxisPtX;
    double aPerpY = aP2.Y() - aAxisPtY;
    double aPerpZ = aP2.Z() - aAxisPtZ;

    // Remove axis component
    const double aDotAxis = aPerpX * myCylAxisX + aPerpY * myCylAxisY + aPerpZ * myCylAxisZ;
    aPerpX -= aDotAxis * myCylAxisX;
    aPerpY -= aDotAxis * myCylAxisY;
    aPerpZ -= aDotAxis * myCylAxisZ;

    const double aPerpDist = std::sqrt(aPerpX * aPerpX + aPerpY * aPerpY + aPerpZ * aPerpZ);

    double aU1Min = 0.0;
    if (aPerpDist > theTol)
    {
      const double aInvPerpDist = 1.0 / aPerpDist;
      const double aDirX = aPerpX * aInvPerpDist;
      const double aDirY = aPerpY * aInvPerpDist;
      const double aDirZ = aPerpZ * aInvPerpDist;

      // U for closest point (toward sphere point)
      const double aDotX = aDirX * myCylXDirX + aDirY * myCylXDirY + aDirZ * myCylXDirZ;
      const double aDotY = aDirX * myCylYDirX + aDirY * myCylYDirY + aDirZ * myCylYDirZ;
      aU1Min = std::atan2(aDotY, aDotX);
      if (aU1Min < 0.0)
        aU1Min += ExtremaSS::THE_TWO_PI;
    }

    if (myDomain.has_value())
    {
      aU1Min = std::clamp(aU1Min, myDomain->Domain1.UMin, myDomain->Domain1.UMax);
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP1 = Value1(aU1Min, aV1);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, aU1Min, aV1, theU2, theV2, aP1, aP2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      double aU1Max = aU1Min + M_PI;
      if (aU1Max >= ExtremaSS::THE_TWO_PI)
        aU1Max -= ExtremaSS::THE_TWO_PI;

      if (myDomain.has_value())
      {
        aU1Max = std::clamp(aU1Max, myDomain->Domain1.UMin, myDomain->Domain1.UMax);
      }

      const gp_Pnt aP1 = Value1(aU1Max, aV1);
      const double aSqDist = aP1.SquareDistance(aP2);
      ExtremaSS::AddExtremum(myResult, aU1Max, aV1, theU2, theV2, aP1, aP2, aSqDist, false, theTol);
    }
  }

private:
  gp_Cylinder                          myCylinder; //!< Cylinder geometry
  gp_Sphere                            mySphere;   //!< Sphere geometry
  std::optional<ExtremaSS::Domain4D> myDomain;   //!< Parameter domain
  mutable ExtremaSS::Result            myResult;   //!< Reusable result storage
  bool                                 mySwapped;  //!< True if surfaces were swapped

  // Cached cylinder components
  double myCylOrigX, myCylOrigY, myCylOrigZ;
  double myCylAxisX, myCylAxisY, myCylAxisZ;
  double myCylXDirX, myCylXDirY, myCylXDirZ;
  double myCylYDirX, myCylYDirY, myCylYDirZ;
  double myCylRadius;

  // Cached sphere components
  double mySphCenterX, mySphCenterY, mySphCenterZ;
  double mySphAxisX, mySphAxisY, mySphAxisZ;
  double mySphXDirX, mySphXDirY, mySphXDirZ;
  double mySphYDirX, mySphYDirY, mySphYDirZ;
  double mySphRadius;

  // Precomputed geometry
  double myAxisParam;                              //!< Parameter along cylinder axis
  double myClosestOnAxisX, myClosestOnAxisY, myClosestOnAxisZ; //!< Closest point on axis
  double myPerpX, myPerpY, myPerpZ;                //!< Vector from axis to sphere center
  double myAxisDistSq;                              //!< Squared distance from center to axis
};

#endif // _ExtremaSS_CylinderSphere_HeaderFile
