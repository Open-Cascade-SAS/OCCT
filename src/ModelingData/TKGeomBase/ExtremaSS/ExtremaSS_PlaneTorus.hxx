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

#ifndef _ExtremaSS_PlaneTorus_HeaderFile
#define _ExtremaSS_PlaneTorus_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Plane-Torus extrema computation.
//!
//! Computes the extrema between a plane and a torus.
//! This class handles both (Plane, Torus) and (Torus, Plane) orderings.
//!
//! Algorithm:
//! 1. If torus axis is perpendicular to plane: extrema at top/bottom of torus
//! 2. If torus axis is parallel to plane: extrema on outer/inner rim
//! 3. General case: find U angle where generating circle center is closest/farthest,
//!    then find closest point on that generating circle to plane
//!
//! The torus surface: T(u,v) = C + (R + r*cos(v))*(cos(u)*X + sin(u)*Y) + r*sin(v)*Z
class ExtremaSS_PlaneTorus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane and torus (canonical order).
  ExtremaSS_PlaneTorus(const gp_Pln& thePlane, const gp_Torus& theTorus)
      : myPlane(thePlane),
        myTorus(theTorus),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with torus and plane (swapped order).
  ExtremaSS_PlaneTorus(const gp_Torus& theTorus, const gp_Pln& thePlane)
      : myPlane(thePlane),
        myTorus(theTorus),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with plane, torus, and domain.
  ExtremaSS_PlaneTorus(const gp_Pln&              thePlane,
                       const gp_Torus&            theTorus,
                       const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myTorus(theTorus),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with torus, plane, and domain (swapped order).
  ExtremaSS_PlaneTorus(const gp_Torus&            theTorus,
                       const gp_Pln&              thePlane,
                       const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myTorus(theTorus),
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

    // Signed distance from torus center to plane
    myCenterDistToPlane = (myTorusCenterX - myPlaneOrigX) * myPlaneNormX
                        + (myTorusCenterY - myPlaneOrigY) * myPlaneNormY
                        + (myTorusCenterZ - myPlaneOrigZ) * myPlaneNormZ;

    // Dot product of torus axis and plane normal
    myAxisDotNorm = myTorusAxisX * myPlaneNormX + myTorusAxisY * myPlaneNormY + myTorusAxisZ * myPlaneNormZ;

    // Project plane normal onto torus XY plane
    // Component of normal perpendicular to torus axis
    const double aNormPerpX = myPlaneNormX - myAxisDotNorm * myTorusAxisX;
    const double aNormPerpY = myPlaneNormY - myAxisDotNorm * myTorusAxisY;
    const double aNormPerpZ = myPlaneNormZ - myAxisDotNorm * myTorusAxisZ;
    myNormPerpMag = std::sqrt(aNormPerpX * aNormPerpX + aNormPerpY * aNormPerpY + aNormPerpZ * aNormPerpZ);

    if (myNormPerpMag > ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      myNormPerpDirX = aNormPerpX / myNormPerpMag;
      myNormPerpDirY = aNormPerpY / myNormPerpMag;
      myNormPerpDirZ = aNormPerpZ / myNormPerpMag;
    }
    else
    {
      myNormPerpDirX = myTorusXDirX;
      myNormPerpDirY = myTorusXDirY;
      myNormPerpDirZ = myTorusXDirZ;
    }
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

    const double aAbsAxisDotNorm = std::abs(myAxisDotNorm);

    // Case 1: Torus axis perpendicular to plane (axis parallel to plane normal)
    if (aAbsAxisDotNorm > 1.0 - ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      return performAxisPerpendicular(theTol, theMode);
    }

    // Case 2: Torus axis parallel to plane (axis perpendicular to plane normal)
    if (aAbsAxisDotNorm < ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      return performAxisParallel(theTol, theMode);
    }

    // Case 3: General case
    return performGeneral(theTol, theMode);
  }

private:
  const ExtremaSS::Result& performAxisPerpendicular(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Torus axis is perpendicular to plane
    // All generating circles are at the same distance from plane
    // Extrema are at top and bottom of each generating circle (where v = +/-pi/2)

    const double aAbsCenterDist = std::abs(myCenterDistToPlane);

    // Distance from torus center to plane is myCenterDistToPlane
    // Points on torus at v = pi/2 (toward axis direction) are at distance: centerDist + sign*r
    // Points on torus at v = -pi/2 (away from axis direction) are at distance: centerDist - sign*r

    // Minimum distance
    const double aMinDist = aAbsCenterDist - myMinorRadius;

    if (aMinDist < theTol)
    {
      // Torus intersects plane - infinite solutions on intersection curve
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = 0.0;
      return myResult;
    }

    // For axis perpendicular to plane, extrema form circles (all U values at specific V)
    // This is an infinite solutions case, but we add representative points
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;
    // InfiniteSquareDistance should reflect the minimum distance (used for quick checks)
    myResult.InfiniteSquareDistance = aMinDist * aMinDist;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Add a representative minimum point at U=0
      // When center is above plane (myCenterDistToPlane > 0), min is at bottom of torus (v = -pi/2)
      // When center is below plane (myCenterDistToPlane < 0), min is at top of torus (v = pi/2)
      const double aVMin = myCenterDistToPlane > 0 ? -M_PI / 2.0 : M_PI / 2.0;
      processTorusPoint(0.0, aVMin, theTol, true);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Add a representative maximum point at U=0
      const double aVMax = myCenterDistToPlane > 0 ? M_PI / 2.0 : -M_PI / 2.0;
      processTorusPoint(0.0, aVMax, theTol, false);
    }

    return myResult;
  }

  const ExtremaSS::Result& performAxisParallel(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Torus axis is parallel to plane
    // Extrema occur on outer rim (v=0, closest U) and inner rim (v=pi, farthest U)

    // The plane normal lies entirely in the radial plane of the torus
    // Distance from generating circle center to plane varies with U

    // For generating circle at angle U, center is at:
    // torusCenter + R*(cos(U)*X + sin(U)*Y)
    // Distance to plane = centerDist + R*(cos(U)*X + sin(U)*Y) . planeNorm
    //                   = centerDist + R*(cos(U)*(X.N) + sin(U)*(Y.N))

    const double aXDotN = myTorusXDirX * myPlaneNormX + myTorusXDirY * myPlaneNormY + myTorusXDirZ * myPlaneNormZ;
    const double aYDotN = myTorusYDirX * myPlaneNormX + myTorusYDirY * myPlaneNormY + myTorusYDirZ * myPlaneNormZ;

    // Distance at U: d(U) = centerDist + R*(aXDotN*cos(U) + aYDotN*sin(U))
    //                     = centerDist + R*sqrt(aXDotN^2 + aYDotN^2)*cos(U - phi)
    // where phi = atan2(aYDotN, aXDotN)

    const double aPhi = std::atan2(aYDotN, aXDotN);

    // U for minimum distance to plane (generating circle closest to plane)
    double aUClose = aPhi + M_PI; // where cos(U - phi) = -1
    if (aUClose > ExtremaSS::THE_TWO_PI)
      aUClose -= ExtremaSS::THE_TWO_PI;

    // U for maximum distance (generating circle farthest from plane)
    double aUFar = aPhi; // where cos(U - phi) = 1
    if (aUFar < 0)
      aUFar += ExtremaSS::THE_TWO_PI;

    // At closest generating circle, add extrema at v=0 (outer) and v=pi (inner)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Minimum: closest generating circle, closest point on circle (toward plane)
      processTorusPoint(aUClose, 0.0, theTol, true);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum: farthest generating circle, farthest point on circle (away from plane)
      processTorusPoint(aUFar, M_PI, theTol, false);
    }

    // Also check opposite sides
    if (theMode == ExtremaSS::SearchMode::MinMax)
    {
      // Inner rim of closest circle
      processTorusPoint(aUClose, M_PI, theTol, false);
      // Outer rim of farthest circle
      processTorusPoint(aUFar, 0.0, theTol, true);
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  const ExtremaSS::Result& performGeneral(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // General case: torus axis at angle to plane

    // For a point on torus at (U, V):
    // Position = C + (R + r*cos(V))*(cos(U)*X + sin(U)*Y) + r*sin(V)*Z
    // Distance to plane = centerDist + (R + r*cos(V))*(cos(U)*X.N + sin(U)*Y.N) + r*sin(V)*Z.N
    //                   = centerDist + (R + r*cos(V))*normPerp*cos(U-phi) + r*sin(V)*axisDotNorm
    // where normPerp = sqrt((X.N)^2 + (Y.N)^2), phi = atan2(Y.N, X.N)

    const double aXDotN = myTorusXDirX * myPlaneNormX + myTorusXDirY * myPlaneNormY + myTorusXDirZ * myPlaneNormZ;
    const double aYDotN = myTorusYDirX * myPlaneNormX + myTorusYDirY * myPlaneNormY + myTorusYDirZ * myPlaneNormZ;
    const double aPhi   = std::atan2(aYDotN, aXDotN);

    // U values for extrema (where generating circle center is closest/farthest)
    double aUClose = aPhi + M_PI;
    if (aUClose > ExtremaSS::THE_TWO_PI)
      aUClose -= ExtremaSS::THE_TWO_PI;
    double aUFar = aPhi;
    if (aUFar < 0)
      aUFar += ExtremaSS::THE_TWO_PI;

    // For each U, find V that minimizes/maximizes distance
    // d(U,V) = centerDist + (R + r*cos(V))*normPerp*cos(U-phi) + r*sin(V)*axisDotNorm
    // At fixed U, this is: a + b*cos(V) + c*sin(V)
    // where a = centerDist + R*normPerp*cos(U-phi)
    //       b = r*normPerp*cos(U-phi)
    //       c = r*axisDotNorm
    // Extrema at V: -b*sin(V) + c*cos(V) = 0 => tan(V) = c/b => V = atan2(c, b)

    // At U = aUClose (cos(U-phi) = -1):
    {
      const double aB     = -myMinorRadius * myNormPerpMag;
      const double aC     = myMinorRadius * myAxisDotNorm;
      const double aVOpt  = std::atan2(aC, aB);
      const double aVOpt2 = aVOpt + M_PI;

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        processTorusPoint(aUClose, aVOpt, theTol, true);
      }
      if (theMode != ExtremaSS::SearchMode::Min)
      {
        processTorusPoint(aUClose, aVOpt2 > M_PI ? aVOpt2 - ExtremaSS::THE_TWO_PI : aVOpt2, theTol, false);
      }
    }

    // At U = aUFar (cos(U-phi) = 1):
    {
      const double aB     = myMinorRadius * myNormPerpMag;
      const double aC     = myMinorRadius * myAxisDotNorm;
      const double aVOpt  = std::atan2(aC, aB);
      const double aVOpt2 = aVOpt + M_PI;

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        processTorusPoint(aUFar, aVOpt, theTol, true);
      }
      if (theMode != ExtremaSS::SearchMode::Min)
      {
        processTorusPoint(aUFar, aVOpt2 > M_PI ? aVOpt2 - ExtremaSS::THE_TWO_PI : aVOpt2, theTol, false);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  void processTorusPoint(double theU, double theV, double theTol, bool theCheckMin) const
  {
    const gp_Pnt aTorusPt = Value2(theU, theV);

    // Distance to plane
    const double aDistToPlane = (aTorusPt.X() - myPlaneOrigX) * myPlaneNormX
                              + (aTorusPt.Y() - myPlaneOrigY) * myPlaneNormY
                              + (aTorusPt.Z() - myPlaneOrigZ) * myPlaneNormZ;

    // Project onto plane
    const gp_Pnt aPlanePt(aTorusPt.X() - aDistToPlane * myPlaneNormX,
                          aTorusPt.Y() - aDistToPlane * myPlaneNormY,
                          aTorusPt.Z() - aDistToPlane * myPlaneNormZ);

    // Compute plane UV parameters
    const double aPlaneU = (aPlanePt.X() - myPlaneOrigX) * myPlaneXDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneXDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
    const double aPlaneV = (aPlanePt.X() - myPlaneOrigX) * myPlaneYDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneYDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

    const double aSqDist = aDistToPlane * aDistToPlane;

    // Determine if this is a minimum or maximum based on actual distance
    const bool aIsMin = theCheckMin;

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, theU, theV, aPlaneU, aPlaneV, aTorusPt, aPlanePt, aSqDist,
                             aIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, theU, theV, aPlanePt, aTorusPt, aSqDist,
                             aIsMin, theTol);
    }
  }

public:
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

    const ExtremaSS::Domain4D& aDom = myDomain.value();
    constexpr int              aNbSamples = 20;

    // Sample plane boundary edges (Domain1)
    const double aDU1 = (aDom.Domain1.UMax - aDom.Domain1.UMin) / aNbSamples;
    const double aDV1 = (aDom.Domain1.VMax - aDom.Domain1.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom.Domain1.UMin + i * aDU1;
      checkPlanePointAgainstTorus(aU1, aDom.Domain1.VMin, theTol, theMode);
      checkPlanePointAgainstTorus(aU1, aDom.Domain1.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV1 = aDom.Domain1.VMin + i * aDV1;
      checkPlanePointAgainstTorus(aDom.Domain1.UMin, aV1, theTol, theMode);
      checkPlanePointAgainstTorus(aDom.Domain1.UMax, aV1, theTol, theMode);
    }

    // Sample torus boundary edges (Domain2)
    const double aDU2 = (aDom.Domain2.UMax - aDom.Domain2.UMin) / aNbSamples;
    const double aDV2 = (aDom.Domain2.VMax - aDom.Domain2.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom.Domain2.UMin + i * aDU2;
      checkTorusPointAgainstPlane(aU2, aDom.Domain2.VMin, theTol, theMode);
      checkTorusPointAgainstPlane(aU2, aDom.Domain2.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV2 = aDom.Domain2.VMin + i * aDV2;
      checkTorusPointAgainstPlane(aDom.Domain2.UMin, aV2, theTol, theMode);
      checkTorusPointAgainstPlane(aDom.Domain2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a plane boundary point against the torus.
  void checkPlanePointAgainstTorus(double thePlaneU, double thePlaneV, double theTol,
                                   ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aPlanePt = Value1(thePlaneU, thePlaneV);

    // Find closest/farthest point on torus to this plane point
    // Direction from torus center to plane point
    const double aDx = aPlanePt.X() - myTorusCenterX;
    const double aDy = aPlanePt.Y() - myTorusCenterY;
    const double aDz = aPlanePt.Z() - myTorusCenterZ;

    // Project onto torus axis to get height
    const double aHeight = aDx * myTorusAxisX + aDy * myTorusAxisY + aDz * myTorusAxisZ;

    // Projection onto torus XY plane
    const double aProjX = aDx - aHeight * myTorusAxisX;
    const double aProjY = aDy - aHeight * myTorusAxisY;
    const double aProjZ = aDz - aHeight * myTorusAxisZ;
    const double aRadialDist = std::sqrt(aProjX * aProjX + aProjY * aProjY + aProjZ * aProjZ);

    if (aRadialDist < theTol)
    {
      // Point is on torus axis - use default direction
      return;
    }

    // Torus U angle
    const double aRadDirX = aProjX / aRadialDist;
    const double aRadDirY = aProjY / aRadialDist;
    const double aRadDirZ = aProjZ / aRadialDist;

    double aTorusU = std::atan2(aRadDirX * myTorusYDirX + aRadDirY * myTorusYDirY + aRadDirZ * myTorusYDirZ,
                                aRadDirX * myTorusXDirX + aRadDirY * myTorusXDirY + aRadDirZ * myTorusXDirZ);
    if (aTorusU < 0)
      aTorusU += ExtremaSS::THE_TWO_PI;

    // Generating circle center at this U
    const double aCosU = std::cos(aTorusU);
    const double aSinU = std::sin(aTorusU);
    const double aCircCentX = myTorusCenterX + myMajorRadius * (aCosU * myTorusXDirX + aSinU * myTorusYDirX);
    const double aCircCentY = myTorusCenterY + myMajorRadius * (aCosU * myTorusXDirY + aSinU * myTorusYDirY);
    const double aCircCentZ = myTorusCenterZ + myMajorRadius * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);

    // Direction from circle center to plane point
    const double aCdx = aPlanePt.X() - aCircCentX;
    const double aCdy = aPlanePt.Y() - aCircCentY;
    const double aCdz = aPlanePt.Z() - aCircCentZ;
    const double aCdist = std::sqrt(aCdx * aCdx + aCdy * aCdy + aCdz * aCdz);

    if (aCdist < theTol)
      return;

    const double aCdirX = aCdx / aCdist;
    const double aCdirY = aCdy / aCdist;
    const double aCdirZ = aCdz / aCdist;

    // V angle for closest point on generating circle
    const double aRadialComp = aCdirX * (aCosU * myTorusXDirX + aSinU * myTorusYDirX)
                             + aCdirY * (aCosU * myTorusXDirY + aSinU * myTorusYDirY)
                             + aCdirZ * (aCosU * myTorusXDirZ + aSinU * myTorusYDirZ);
    const double aAxialComp = aCdirX * myTorusAxisX + aCdirY * myTorusAxisY + aCdirZ * myTorusAxisZ;

    double aTorusV = std::atan2(aAxialComp, aRadialComp);
    if (aTorusV < 0)
      aTorusV += ExtremaSS::THE_TWO_PI;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Check closest point (minimum)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const double aClampedU = std::clamp(aTorusU, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aTorusV, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aTorusPt = Value2(aClampedU, aClampedV);
      const double aSqDist = aPlanePt.SquareDistance(aTorusPt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, thePlaneU, thePlaneV, aTorusPt, aPlanePt,
                               aSqDist, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, thePlaneU, thePlaneV, aClampedU, aClampedV, aPlanePt, aTorusPt,
                               aSqDist, true, theTol);
      }
    }

    // Check farthest point (maximum) - opposite side of generating circle
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      double aTorusVFar = aTorusV + M_PI;
      if (aTorusVFar > ExtremaSS::THE_TWO_PI)
        aTorusVFar -= ExtremaSS::THE_TWO_PI;

      const double aClampedU = std::clamp(aTorusU, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aTorusVFar, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aTorusPt = Value2(aClampedU, aClampedV);
      const double aSqDist = aPlanePt.SquareDistance(aTorusPt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, thePlaneU, thePlaneV, aTorusPt, aPlanePt,
                               aSqDist, false, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, thePlaneU, thePlaneV, aClampedU, aClampedV, aPlanePt, aTorusPt,
                               aSqDist, false, theTol);
      }
    }
  }

  //! Check a torus boundary point against the plane.
  void checkTorusPointAgainstPlane(double theTorusU, double theTorusV, double theTol,
                                   ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aTorusPt = Value2(theTorusU, theTorusV);

    // Project torus point onto plane
    const double aDistToPlane = (aTorusPt.X() - myPlaneOrigX) * myPlaneNormX
                              + (aTorusPt.Y() - myPlaneOrigY) * myPlaneNormY
                              + (aTorusPt.Z() - myPlaneOrigZ) * myPlaneNormZ;

    const gp_Pnt aProjPt(aTorusPt.X() - aDistToPlane * myPlaneNormX,
                         aTorusPt.Y() - aDistToPlane * myPlaneNormY,
                         aTorusPt.Z() - aDistToPlane * myPlaneNormZ);

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
    const gp_Pnt aPlanePt = Value1(aClampedU, aClampedV);
    const double aSqDist = aTorusPt.SquareDistance(aPlanePt);

    // Both minimum and maximum are the same for a fixed torus point (projection)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, theTorusU, theTorusV, aClampedU, aClampedV, aTorusPt, aPlanePt,
                               aSqDist, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, theTorusU, theTorusV, aPlanePt, aTorusPt,
                               aSqDist, true, theTol);
      }
    }
  }

public:
  const gp_Pln&   Plane() const { return myPlane; }
  const gp_Torus& Torus() const { return myTorus; }
  bool            IsSwapped() const { return mySwapped; }
  bool            IsBounded() const { return myDomain.has_value(); }

private:
  gp_Pln                             myPlane;
  gp_Torus                           myTorus;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Cached plane components
  double myPlaneOrigX, myPlaneOrigY, myPlaneOrigZ;
  double myPlaneNormX, myPlaneNormY, myPlaneNormZ;
  double myPlaneXDirX, myPlaneXDirY, myPlaneXDirZ;
  double myPlaneYDirX, myPlaneYDirY, myPlaneYDirZ;

  // Cached torus components
  gp_Pnt myTorusCenter;
  double myTorusCenterX, myTorusCenterY, myTorusCenterZ;
  double myTorusAxisX, myTorusAxisY, myTorusAxisZ;
  double myTorusXDirX, myTorusXDirY, myTorusXDirZ;
  double myTorusYDirX, myTorusYDirY, myTorusYDirZ;
  double myMajorRadius, myMinorRadius;

  // Cached geometric relationships
  double myCenterDistToPlane;
  double myAxisDotNorm;
  double myNormPerpMag;
  double myNormPerpDirX, myNormPerpDirY, myNormPerpDirZ;
};

#endif // _ExtremaSS_PlaneTorus_HeaderFile
