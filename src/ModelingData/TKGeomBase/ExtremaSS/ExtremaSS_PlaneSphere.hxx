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

#ifndef _ExtremaSS_PlaneSphere_HeaderFile
#define _ExtremaSS_PlaneSphere_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Plane-Sphere extrema computation.
//!
//! Computes the extrema between a plane and a spherical surface.
//! This class handles both (Plane, Sphere) and (Sphere, Plane) orderings.
//!
//! Algorithm:
//! 1. Compute signed distance d from sphere center to plane
//! 2. Minimum distance = |d| - R (if |d| > R, otherwise sphere intersects plane)
//! 3. Maximum distance = |d| + R
//! 4. Both extrema are on the normal line through sphere center
//!
//! @note The extrema points are:
//!       - On sphere: center + R * (sign(d) * normal) for min, center - R * (sign(d) * normal) for max
//!       - On plane: projection of sphere extremum point onto plane
class ExtremaSS_PlaneSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane and sphere (canonical order).
  //! @param[in] thePlane the plane
  //! @param[in] theSphere the sphere
  ExtremaSS_PlaneSphere(const gp_Pln& thePlane, const gp_Sphere& theSphere)
      : myPlane(thePlane),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere and plane (swapped order).
  //! @param[in] theSphere the sphere
  //! @param[in] thePlane the plane
  ExtremaSS_PlaneSphere(const gp_Sphere& theSphere, const gp_Pln& thePlane)
      : myPlane(thePlane),
        mySphere(theSphere),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with plane, sphere, and domain.
  //! @param[in] thePlane the plane
  //! @param[in] theSphere the sphere
  //! @param[in] theDomain parameter domains
  ExtremaSS_PlaneSphere(const gp_Pln&              thePlane,
                        const gp_Sphere&           theSphere,
                        const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        mySphere(theSphere),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with sphere, plane, and domain (swapped order).
  //! @param[in] theSphere the sphere
  //! @param[in] thePlane the plane
  //! @param[in] theDomain parameter domains (will be swapped internally)
  ExtremaSS_PlaneSphere(const gp_Sphere&           theSphere,
                        const gp_Pln&              thePlane,
                        const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
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
    // Cache plane components
    const gp_Ax3& aPlanePos = myPlane.Position();
    const gp_Pnt& aPlaneOrig = aPlanePos.Location();
    myPlaneOrigX = aPlaneOrig.X();
    myPlaneOrigY = aPlaneOrig.Y();
    myPlaneOrigZ = aPlaneOrig.Z();

    const gp_Dir& aPlaneNorm = aPlanePos.Direction();
    myPlaneNormX = aPlaneNorm.X();
    myPlaneNormY = aPlaneNorm.Y();
    myPlaneNormZ = aPlaneNorm.Z();

    const gp_Dir& aPlaneXDir = aPlanePos.XDirection();
    myPlaneXDirX = aPlaneXDir.X();
    myPlaneXDirY = aPlaneXDir.Y();
    myPlaneXDirZ = aPlaneXDir.Z();

    const gp_Dir& aPlaneYDir = aPlanePos.YDirection();
    myPlaneYDirX = aPlaneYDir.X();
    myPlaneYDirY = aPlaneYDir.Y();
    myPlaneYDirZ = aPlaneYDir.Z();

    // Cache sphere components
    const gp_Ax3& aSpherePos = mySphere.Position();
    const gp_Pnt& aSphereCenter = aSpherePos.Location();
    mySphereCenterX = aSphereCenter.X();
    mySphereCenterY = aSphereCenter.Y();
    mySphereCenterZ = aSphereCenter.Z();

    const gp_Dir& aSphereAxis = aSpherePos.Direction();
    mySphereAxisX = aSphereAxis.X();
    mySphereAxisY = aSphereAxis.Y();
    mySphereAxisZ = aSphereAxis.Z();

    const gp_Dir& aSphereXDir = aSpherePos.XDirection();
    mySphereXDirX = aSphereXDir.X();
    mySphereXDirY = aSphereXDir.Y();
    mySphereXDirZ = aSphereXDir.Z();

    const gp_Dir& aSphereYDir = aSpherePos.YDirection();
    mySphereYDirX = aSphereYDir.X();
    mySphereYDirY = aSphereYDir.Y();
    mySphereYDirZ = aSphereYDir.Z();

    mySphereRadius = mySphere.Radius();

    // Compute signed distance from sphere center to plane
    // d = (center - planeOrigin) . planeNormal
    const double aDx = mySphereCenterX - myPlaneOrigX;
    const double aDy = mySphereCenterY - myPlaneOrigY;
    const double aDz = mySphereCenterZ - myPlaneOrigZ;
    mySignedDistance = aDx * myPlaneNormX + aDy * myPlaneNormY + aDz * myPlaneNormZ;
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate plane point at given parameters.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on plane at (U, V)
  gp_Pnt Value1(double theU, double theV) const
  {
    return gp_Pnt(myPlaneOrigX + theU * myPlaneXDirX + theV * myPlaneYDirX,
                  myPlaneOrigY + theU * myPlaneXDirY + theV * myPlaneYDirY,
                  myPlaneOrigZ + theU * myPlaneXDirZ + theV * myPlaneYDirZ);
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
    const double aRcosV = mySphereRadius * aCosV;
    return gp_Pnt(mySphereCenterX + aRcosV * (aCosU * mySphereXDirX + aSinU * mySphereYDirX)
                    + mySphereRadius * aSinV * mySphereAxisX,
                  mySphereCenterY + aRcosV * (aCosU * mySphereXDirY + aSinU * mySphereYDirY)
                    + mySphereRadius * aSinV * mySphereAxisY,
                  mySphereCenterZ + aRcosV * (aCosU * mySphereXDirZ + aSinU * mySphereYDirZ)
                    + mySphereRadius * aSinV * mySphereAxisZ);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //!
  //! For plane-sphere, there are exactly two interior extrema:
  //! - Minimum: closest point on sphere toward plane
  //! - Maximum: farthest point on sphere away from plane
  //!
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Direction from plane to sphere center (along normal)
    const double aSign = (mySignedDistance >= 0.0) ? 1.0 : -1.0;
    const double aAbsDist = std::abs(mySignedDistance);

    // Compute UV on sphere from plane normal direction
    // For minimum: sphere point toward plane = center - R * sign * normal
    // For maximum: sphere point away from plane = center + R * sign * normal
    double aSphereU_Min, aSphereV_Min;
    double aSphereU_Max, aSphereV_Max;

    // Direction toward plane: -sign * normal
    computeSphereUVFromDirection(-aSign * myPlaneNormX, -aSign * myPlaneNormY, -aSign * myPlaneNormZ,
                                 theTol, aSphereU_Min, aSphereV_Min);

    // Direction away from plane: +sign * normal
    computeSphereUVFromDirection(aSign * myPlaneNormX, aSign * myPlaneNormY, aSign * myPlaneNormZ,
                                 theTol, aSphereU_Max, aSphereV_Max);

    // Minimum extremum
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Point on sphere closest to plane
      const gp_Pnt aSphPt(mySphereCenterX - mySphereRadius * aSign * myPlaneNormX,
                          mySphereCenterY - mySphereRadius * aSign * myPlaneNormY,
                          mySphereCenterZ - mySphereRadius * aSign * myPlaneNormZ);

      // Project onto plane to get plane point
      const double aPlaneU = (aSphPt.X() - myPlaneOrigX) * myPlaneXDirX
                           + (aSphPt.Y() - myPlaneOrigY) * myPlaneXDirY
                           + (aSphPt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
      const double aPlaneV = (aSphPt.X() - myPlaneOrigX) * myPlaneYDirX
                           + (aSphPt.Y() - myPlaneOrigY) * myPlaneYDirY
                           + (aSphPt.Z() - myPlaneOrigZ) * myPlaneYDirZ;
      const gp_Pnt aPlanePt = Value1(aPlaneU, aPlaneV);

      const double aMinDist   = aAbsDist - mySphereRadius;
      const double aMinSqDist = (aMinDist > 0.0) ? aMinDist * aMinDist : 0.0;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aSphereU_Min, aSphereV_Min, aPlaneU, aPlaneV, aSphPt,
                               aPlanePt, aMinSqDist, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, aSphereU_Min, aSphereV_Min, aPlanePt,
                               aSphPt, aMinSqDist, true, theTol);
      }
    }

    // Maximum extremum
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Point on sphere farthest from plane
      const gp_Pnt aSphPt(mySphereCenterX + mySphereRadius * aSign * myPlaneNormX,
                          mySphereCenterY + mySphereRadius * aSign * myPlaneNormY,
                          mySphereCenterZ + mySphereRadius * aSign * myPlaneNormZ);

      // Project onto plane to get plane point
      const double aPlaneU = (aSphPt.X() - myPlaneOrigX) * myPlaneXDirX
                           + (aSphPt.Y() - myPlaneOrigY) * myPlaneXDirY
                           + (aSphPt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
      const double aPlaneV = (aSphPt.X() - myPlaneOrigX) * myPlaneYDirX
                           + (aSphPt.Y() - myPlaneOrigY) * myPlaneYDirY
                           + (aSphPt.Z() - myPlaneOrigZ) * myPlaneYDirZ;
      const gp_Pnt aPlanePt = Value1(aPlaneU, aPlaneV);

      const double aMaxDist   = aAbsDist + mySphereRadius;
      const double aMaxSqDist = aMaxDist * aMaxDist;

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aSphereU_Max, aSphereV_Max, aPlaneU, aPlaneV, aSphPt,
                               aPlanePt, aMaxSqDist, false, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, aSphereU_Max, aSphereV_Max, aPlanePt,
                               aSphPt, aMaxSqDist, false, theTol);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution
                                                 : ExtremaSS::Status::OK;
    return myResult;
  }

  //! Find extrema including boundary.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);

    if (myResult.IsInfinite() || !myDomain.has_value())
    {
      return myResult;
    }

    // TODO: Add boundary extrema handling for bounded domains

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

  //! Returns the plane geometry.
  const gp_Pln& Plane() const { return myPlane; }

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
    double aSinV = theDirX * mySphereAxisX + theDirY * mySphereAxisY + theDirZ * mySphereAxisZ;
    aSinV        = aSinV < -1.0 ? -1.0 : (aSinV > 1.0 ? 1.0 : aSinV);
    theV         = std::asin(aSinV);

    // U (azimuth) from direction
    const double aCosV = std::cos(theV);
    theU               = 0.0;

    if (std::abs(aCosV) > theTol)
    {
      const double aInvCosV = 1.0 / aCosV;
      const double aCosU =
        (theDirX * mySphereXDirX + theDirY * mySphereXDirY + theDirZ * mySphereXDirZ) * aInvCosV;
      const double aSinU =
        (theDirX * mySphereYDirX + theDirY * mySphereYDirY + theDirZ * mySphereYDirZ) * aInvCosV;
      theU = std::atan2(aSinU, aCosU);
      if (theU < 0.0)
        theU += ExtremaSS::THE_TWO_PI;
    }
  }

private:
  gp_Pln                               myPlane;   //!< Plane geometry
  gp_Sphere                            mySphere;  //!< Sphere geometry
  std::optional<ExtremaSS::Domain4D> myDomain; //!< Parameter domain
  mutable ExtremaSS::Result            myResult;  //!< Reusable result storage
  bool                                 mySwapped; //!< True if surfaces were swapped

  // Cached plane components
  double myPlaneOrigX, myPlaneOrigY, myPlaneOrigZ;
  double myPlaneNormX, myPlaneNormY, myPlaneNormZ;
  double myPlaneXDirX, myPlaneXDirY, myPlaneXDirZ;
  double myPlaneYDirX, myPlaneYDirY, myPlaneYDirZ;

  // Cached sphere components
  double mySphereCenterX, mySphereCenterY, mySphereCenterZ;
  double mySphereAxisX, mySphereAxisY, mySphereAxisZ;
  double mySphereXDirX, mySphereXDirY, mySphereXDirZ;
  double mySphereYDirX, mySphereYDirY, mySphereYDirZ;
  double mySphereRadius;

  // Precomputed distance
  double mySignedDistance; //!< Signed distance from sphere center to plane
};

#endif // _ExtremaSS_PlaneSphere_HeaderFile
