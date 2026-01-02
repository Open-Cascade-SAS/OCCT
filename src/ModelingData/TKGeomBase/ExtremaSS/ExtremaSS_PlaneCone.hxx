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

#ifndef _ExtremaSS_PlaneCone_HeaderFile
#define _ExtremaSS_PlaneCone_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Plane-Cone extrema computation.
//!
//! Computes the extrema between a plane and a conical surface.
//! This class handles both (Plane, Cone) and (Cone, Plane) orderings.
//!
//! Algorithm:
//! For a cone with apex A, axis direction D, semi-angle alpha, and local X/Y directions:
//! Point(U, V) = A + V * [cos(alpha) * D + sin(alpha) * (cos(U) * X + sin(U) * Y)]
//!
//! Distance from point to plane: d(U, V) = d0 + V * (c1 + c2*cos(U) + c3*sin(U))
//! where:
//!   d0 = signed distance from apex to plane
//!   c1 = cos(alpha) * (D . N)
//!   c2 = sin(alpha) * (X . N)
//!   c3 = sin(alpha) * (Y . N)
//!
//! Extrema occur at U = atan2(c3, c2) and U = atan2(c3, c2) + pi
//! giving d(V) = d0 + V * (c1 +/- sqrt(c2^2 + c3^2))
class ExtremaSS_PlaneCone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane and cone (canonical order).
  ExtremaSS_PlaneCone(const gp_Pln& thePlane, const gp_Cone& theCone)
      : myPlane(thePlane),
        myCone(theCone),
        myDomain(std::nullopt),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with cone and plane (swapped order).
  ExtremaSS_PlaneCone(const gp_Cone& theCone, const gp_Pln& thePlane)
      : myPlane(thePlane),
        myCone(theCone),
        myDomain(std::nullopt),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with plane, cone, and domain.
  ExtremaSS_PlaneCone(const gp_Pln&              thePlane,
                      const gp_Cone&             theCone,
                      const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myCone(theCone),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with cone, plane, and domain (swapped order).
  ExtremaSS_PlaneCone(const gp_Cone&             theCone,
                      const gp_Pln&              thePlane,
                      const ExtremaSS::Domain4D& theDomain)
      : myPlane(thePlane),
        myCone(theCone),
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

    // Cache cone components
    const gp_Ax3& aConePos = myCone.Position();
    myApex                 = myCone.Apex();
    myApexX                = myApex.X();
    myApexY                = myApex.Y();
    myApexZ                = myApex.Z();

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

    mySemiAngle    = myCone.SemiAngle();
    myCosSemiAngle = std::cos(mySemiAngle);
    mySinSemiAngle = std::sin(mySemiAngle);
    myRefRadius    = myCone.RefRadius();

    // Compute key dot products
    myAxisDotNorm = myConeAxisX * myPlaneNormX + myConeAxisY * myPlaneNormY + myConeAxisZ * myPlaneNormZ;
    myXDirDotNorm = myConeXDirX * myPlaneNormX + myConeXDirY * myPlaneNormY + myConeXDirZ * myPlaneNormZ;
    myYDirDotNorm = myConeYDirX * myPlaneNormX + myConeYDirY * myPlaneNormY + myConeYDirZ * myPlaneNormZ;

    // Signed distance from apex to plane
    myApexDistToPlane = (myApexX - myPlaneOrigX) * myPlaneNormX + (myApexY - myPlaneOrigY) * myPlaneNormY
                      + (myApexZ - myPlaneOrigZ) * myPlaneNormZ;

    // Coefficients for distance formula: d(U, V) = d0 + V * (c1 + c2*cos(U) + c3*sin(U))
    myC1         = myCosSemiAngle * myAxisDotNorm;
    myC2         = mySinSemiAngle * myXDirDotNorm;
    myC3         = mySinSemiAngle * myYDirDotNorm;
    myC2C3Norm   = std::sqrt(myC2 * myC2 + myC3 * myC3);
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
    // Cone point at (U, V)
    // The cone in OCCT is defined with axis at reference radius from apex
    // Position = ConeOrigin + V*Axis + (RefRadius + V*tan(SemiAngle)) * (cos(U)*XDir + sin(U)*YDir)
    // where ConeOrigin is at distance RefRadius/tan(SemiAngle) from apex along axis
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius + theV * std::tan(mySemiAngle);
    const gp_Pnt aConeOrig = myCone.Location();
    return gp_Pnt(aConeOrig.X() + theV * myConeAxisX + aRadius * (aCosU * myConeXDirX + aSinU * myConeYDirX),
                  aConeOrig.Y() + theV * myConeAxisY + aRadius * (aCosU * myConeXDirY + aSinU * myConeYDirY),
                  aConeOrig.Z() + theV * myConeAxisZ + aRadius * (aCosU * myConeXDirZ + aSinU * myConeYDirZ));
  }

  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    const double aAngleTol = ExtremaSS::THE_ANGULAR_TOLERANCE;

    // Special case: axis perpendicular to plane (axis parallel to normal)
    const double aAbsAxisDotNorm = std::abs(myAxisDotNorm);
    if (aAbsAxisDotNorm > 1.0 - aAngleTol)
    {
      // Cone axis is perpendicular to plane
      // All points at same V have same distance to plane
      // Extrema is at apex if cone opens away from plane, or infinite solutions if it touches
      return performAxisPerpendicular(theTol, theMode);
    }

    // Special case: axis parallel to plane (axis perpendicular to normal)
    if (aAbsAxisDotNorm < aAngleTol)
    {
      return performAxisParallel(theTol, theMode);
    }

    // General case
    return performGeneral(theTol, theMode);
  }

private:
  const ExtremaSS::Result& performAxisPerpendicular(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Axis perpendicular to plane means c1 = cos(alpha) * (axis . norm) = +/- cos(alpha)
    // and c2, c3 are small. The cone opens perpendicular to the plane.
    // Distance from cone to plane varies with radius at each V.

    // The minimum distance from cone surface to plane depends on whether
    // the cone opens toward or away from the plane.

    // For a point on cone at V, the distance to plane is:
    // d(V) = apexDist + V * cos(alpha) * sign(axis.norm)
    // The radius at V is: R(V) = refRadius + V * tan(alpha)

    // The closest point on the cone to plane is where the surface is tangent to plane-parallel planes.
    // For axis perpendicular to plane, the cone surface at each height V is a circle
    // at constant distance from the plane.

    // If apex is on one side of plane and cone opens toward plane, it may intersect.
    const double aSign        = myAxisDotNorm > 0 ? 1.0 : -1.0;
    const double aApexAbsDist = std::abs(myApexDistToPlane);

    // Check if cone intersects plane
    // The cone intersects plane if the apex is on one side and the opening direction
    // would bring points to the other side.
    // This happens if apexDist and (cos(alpha) * axis.norm) have opposite signs
    // and the cone extends far enough.

    const double aDistRate = myCosSemiAngle * aSign; // how fast distance changes with V

    if (myApexDistToPlane * aDistRate < 0)
    {
      // Cone opens toward the plane, will eventually intersect
      // For OCCT parameterization: d(V) = locDist + V * (Axis.N)
      // where locDist = (Location - PlaneOrig).N
      // For axis perpendicular, X.N = Y.N = 0, so the formula simplifies
      const gp_Pnt& aLoc = myCone.Location();
      const double aLocDistToPlane = (aLoc.X() - myPlaneOrigX) * myPlaneNormX
                                   + (aLoc.Y() - myPlaneOrigY) * myPlaneNormY
                                   + (aLoc.Z() - myPlaneOrigZ) * myPlaneNormZ;
      const double aVIntersect = -aLocDistToPlane / myAxisDotNorm;

      // V can be negative (toward apex) or positive (away from apex)
      // Check if the intersection point is on the valid part of the cone
      // For refRadius > 0, V can be negative down to -refRadius/tan(alpha)
      const double aVApex = (std::abs(mySinSemiAngle) > theTol) ? -myRefRadius / std::tan(mySemiAngle) : -1e10;
      const bool aValidIntersection = (aVIntersect > aVApex) || std::abs(aLocDistToPlane) < theTol;

      if (aValidIntersection)
      {
        // Intersection exists - add representative point with distance 0
        if (theMode != ExtremaSS::SearchMode::Max)
        {
          if (std::abs(aLocDistToPlane) < theTol)
          {
            // Location is on plane - reference circle touches plane
            addExtremumAtUV(0.0, 0.0, theTol, true);
          }
          else
          {
            addExtremumAtUV(0.0, aVIntersect, theTol, true);
          }
        }
        myResult.Status = ExtremaSS::Status::OK;
        return myResult;
      }
    }

    // No intersection - apex provides one extremum
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Apex is minimum if cone opens away from plane, or if apex is the closest point
      if (myApexDistToPlane * aDistRate >= 0 || aApexAbsDist < theTol)
      {
        addApexExtremum(theTol, true);
      }
    }

    // For unbounded cone, no maximum (distance grows to infinity)
    // Maximum only exists for bounded domain
    if (theMode != ExtremaSS::SearchMode::Min && myDomain.has_value())
    {
      // Check V bounds for maximum
      const double aVMax = mySwapped ? myDomain->Domain1.VMax : myDomain->Domain2.VMax;
      if (aVMax < std::numeric_limits<double>::max())
      {
        // Maximum is at V = VMax along the appropriate U
        addExtremumAtV(aVMax, theTol, false);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  const ExtremaSS::Result& performAxisParallel(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Axis parallel to plane means axis.norm = 0, so c1 = 0
    // Distance: d(U, V) = apexDist + V * (c2*cos(U) + c3*sin(U))
    //                   = apexDist + V * c2c3Norm * cos(U - phi)
    // where phi = atan2(c3, c2)

    // Extremal U values: U_min = phi + pi, U_max = phi
    // At U_max: d(V) = apexDist + V * c2c3Norm
    // At U_min: d(V) = apexDist - V * c2c3Norm

    if (myC2C3Norm < ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      // Degenerate case: XDir and YDir both perpendicular to plane normal
      // All points on cone are at same distance from plane
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = myApexDistToPlane * myApexDistToPlane;
      return myResult;
    }

    const double aPhi = std::atan2(myC3, myC2);

    // Check if cone intersects plane
    // At U_min: d(V) = apexDist - V * c2c3Norm = 0 when V = apexDist / c2c3Norm
    // At U_max: d(V) = apexDist + V * c2c3Norm = 0 when V = -apexDist / c2c3Norm

    bool aHasIntersection = false;
    if (myApexDistToPlane > 0)
    {
      // Apex above plane, intersection at U_min for V > 0
      const double aVInt = myApexDistToPlane / myC2C3Norm;
      if (aVInt > 0)
        aHasIntersection = true;
    }
    else if (myApexDistToPlane < 0)
    {
      // Apex below plane, intersection at U_max for V > 0
      const double aVInt = -myApexDistToPlane / myC2C3Norm;
      if (aVInt > 0)
        aHasIntersection = true;
    }
    else
    {
      // Apex is on plane
      aHasIntersection = true;
    }

    if (aHasIntersection)
    {
      // Minimum distance is 0 - add representative intersection point
      if (theMode != ExtremaSS::SearchMode::Max)
      {
        if (std::abs(myApexDistToPlane) < theTol)
        {
          // Apex is on plane
          addApexExtremum(theTol, true);
        }
        else
        {
          // Find intersection point
          const double aUInt = myApexDistToPlane > 0 ? aPhi + M_PI : aPhi;
          const double aVInt = std::abs(myApexDistToPlane) / myC2C3Norm;
          addExtremumAtUV(aUInt < 0 ? aUInt + ExtremaSS::THE_TWO_PI : aUInt, aVInt, theTol, true);
        }
      }
      myResult.Status = ExtremaSS::Status::OK;
      return myResult;
    }

    // Minimum: apex (V=0) if no intersection
    if (theMode != ExtremaSS::SearchMode::Max && !aHasIntersection)
    {
      addApexExtremum(theTol, true);
    }

    // Maximum: at boundary V if domain is bounded
    if (theMode != ExtremaSS::SearchMode::Min && myDomain.has_value())
    {
      const double aVMax = mySwapped ? myDomain->Domain1.VMax : myDomain->Domain2.VMax;
      if (aVMax < std::numeric_limits<double>::max())
      {
        // Maximum is along the generator that goes away from plane
        double aUMax = myApexDistToPlane >= 0 ? aPhi : aPhi + M_PI;
        if (aUMax < 0)
          aUMax += ExtremaSS::THE_TWO_PI;
        if (aUMax > ExtremaSS::THE_TWO_PI)
          aUMax -= ExtremaSS::THE_TWO_PI;

        addExtremumAtUV(aUMax, aVMax, theTol, false);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  const ExtremaSS::Result& performGeneral(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // General case: axis at angle to plane
    // d(U, V) = d0 + V * (c1 + c2*cos(U) + c3*sin(U))
    // where c1 = cos(alpha) * axis.norm

    // Extremal generators are at U = atan2(c3, c2) and U = atan2(c3, c2) + pi
    // Along these generators:
    //   d+(V) = d0 + V * (c1 + c2c3Norm)  [k+ = c1 + c2c3Norm]
    //   d-(V) = d0 + V * (c1 - c2c3Norm)  [k- = c1 - c2c3Norm]

    const double aKPlus  = myC1 + myC2C3Norm;
    const double aKMinus = myC1 - myC2C3Norm;
    const double aPhi    = std::atan2(myC3, myC2);

    // U values for the two extremal generators
    double aUPlus = aPhi;
    if (aUPlus < 0)
      aUPlus += ExtremaSS::THE_TWO_PI;
    double aUMinus = aPhi + M_PI;
    if (aUMinus > ExtremaSS::THE_TWO_PI)
      aUMinus -= ExtremaSS::THE_TWO_PI;

    // Check for intersection with plane
    // d+(V) = 0 => V = -d0 / k+
    // d-(V) = 0 => V = -d0 / k-

    double aVIntPlus  = (std::abs(aKPlus) > theTol) ? -myApexDistToPlane / aKPlus : -1.0;
    double aVIntMinus = (std::abs(aKMinus) > theTol) ? -myApexDistToPlane / aKMinus : -1.0;

    // Valid intersection requires V > 0 for standard cone orientation
    const bool aIntersectsPlus  = (aVIntPlus > theTol);
    const bool aIntersectsMinus = (aVIntMinus > theTol);

    if (aIntersectsPlus || aIntersectsMinus || std::abs(myApexDistToPlane) < theTol)
    {
      // Cone intersects plane - minimum distance is 0
      if (theMode != ExtremaSS::SearchMode::Max)
      {
        // Add intersection point(s) as minimum
        if (std::abs(myApexDistToPlane) < theTol)
        {
          // Apex is on plane
          addApexExtremum(theTol, true);
        }
        else if (aIntersectsPlus)
        {
          addExtremumAtUV(aUPlus, aVIntPlus, theTol, true);
        }
        else if (aIntersectsMinus)
        {
          addExtremumAtUV(aUMinus, aVIntMinus, theTol, true);
        }
      }
    }
    else
    {
      // No intersection - apex is on one side of plane
      // Minimum is at apex
      if (theMode != ExtremaSS::SearchMode::Max)
      {
        addApexExtremum(theTol, true);
      }
    }

    // Maximum: for unbounded cone, no finite maximum
    // For bounded cone, check boundary V
    if (theMode != ExtremaSS::SearchMode::Min && myDomain.has_value())
    {
      const double aVMax = mySwapped ? myDomain->Domain1.VMax : myDomain->Domain2.VMax;
      if (aVMax < std::numeric_limits<double>::max())
      {
        // Find which generator gives maximum distance at VMax
        const double aDistPlus  = std::abs(myApexDistToPlane + aVMax * aKPlus);
        const double aDistMinus = std::abs(myApexDistToPlane + aVMax * aKMinus);

        if (aDistPlus >= aDistMinus)
        {
          addExtremumAtUV(aUPlus, aVMax, theTol, false);
        }
        else
        {
          addExtremumAtUV(aUMinus, aVMax, theTol, false);
        }
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaSS::Status::NoSolution : ExtremaSS::Status::OK;
    return myResult;
  }

  void addApexExtremum(double theTol, bool theIsMin) const
  {
    // Project apex onto plane
    const gp_Pnt aPlanePt(myApexX - myApexDistToPlane * myPlaneNormX,
                          myApexY - myApexDistToPlane * myPlaneNormY,
                          myApexZ - myApexDistToPlane * myPlaneNormZ);

    // Compute plane UV parameters
    const double aPlaneU = (aPlanePt.X() - myPlaneOrigX) * myPlaneXDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneXDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
    const double aPlaneV = (aPlanePt.X() - myPlaneOrigX) * myPlaneYDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneYDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

    // Cone parameters at apex: U is arbitrary (0), V such that we're at apex
    // For OCCT cone, V=0 is at the reference circle, not at apex
    // V at apex = -RefRadius / tan(SemiAngle) (if semi-angle != 0)
    double aConeV = 0.0;
    if (std::abs(mySinSemiAngle) > ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      aConeV = -myRefRadius / std::tan(mySemiAngle);
    }
    const double aConeU = 0.0; // U is arbitrary at apex

    const double aSqDist = myApexDistToPlane * myApexDistToPlane;

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, aConeU, aConeV, aPlaneU, aPlaneV, myApex, aPlanePt, aSqDist,
                             theIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, aConeU, aConeV, aPlanePt, myApex, aSqDist,
                             theIsMin, theTol);
    }
  }

  void addExtremumAtV(double theV, double theTol, bool theIsMin) const
  {
    // At given V, find U that gives extremum
    // d(U, V) = d0 + V * (c1 + c2*cos(U) + c3*sin(U))
    // Extremum at U = atan2(c3, c2) for max, U = atan2(c3, c2) + pi for min

    const double aPhi = std::atan2(myC3, myC2);
    double       aU   = theIsMin ? aPhi + M_PI : aPhi;
    if (aU < 0)
      aU += ExtremaSS::THE_TWO_PI;
    if (aU > ExtremaSS::THE_TWO_PI)
      aU -= ExtremaSS::THE_TWO_PI;

    addExtremumAtUV(aU, theV, theTol, theIsMin);
  }

  void addExtremumAtUV(double theU, double theV, double theTol, bool theIsMin) const
  {
    // Compute point on cone at (U, V)
    const gp_Pnt aConePt = Value2(theU, theV);

    // Compute distance to plane
    const double aDistToPlane = (aConePt.X() - myPlaneOrigX) * myPlaneNormX
                              + (aConePt.Y() - myPlaneOrigY) * myPlaneNormY
                              + (aConePt.Z() - myPlaneOrigZ) * myPlaneNormZ;

    // Project onto plane
    const gp_Pnt aPlanePt(aConePt.X() - aDistToPlane * myPlaneNormX,
                          aConePt.Y() - aDistToPlane * myPlaneNormY,
                          aConePt.Z() - aDistToPlane * myPlaneNormZ);

    // Compute plane UV parameters
    const double aPlaneU = (aPlanePt.X() - myPlaneOrigX) * myPlaneXDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneXDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneXDirZ;
    const double aPlaneV = (aPlanePt.X() - myPlaneOrigX) * myPlaneYDirX
                         + (aPlanePt.Y() - myPlaneOrigY) * myPlaneYDirY
                         + (aPlanePt.Z() - myPlaneOrigZ) * myPlaneYDirZ;

    const double aSqDist = aDistToPlane * aDistToPlane;

    if (mySwapped)
    {
      ExtremaSS::AddExtremum(myResult, theU, theV, aPlaneU, aPlaneV, aConePt, aPlanePt, aSqDist,
                             theIsMin, theTol);
    }
    else
    {
      ExtremaSS::AddExtremum(myResult, aPlaneU, aPlaneV, theU, theV, aPlanePt, aConePt, aSqDist,
                             theIsMin, theTol);
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
      checkPlanePointAgainstCone(aU1, aDom.Domain1.VMin, theTol, theMode);
      checkPlanePointAgainstCone(aU1, aDom.Domain1.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV1 = aDom.Domain1.VMin + i * aDV1;
      checkPlanePointAgainstCone(aDom.Domain1.UMin, aV1, theTol, theMode);
      checkPlanePointAgainstCone(aDom.Domain1.UMax, aV1, theTol, theMode);
    }

    // Sample cone boundary edges (Domain2)
    const double aDU2 = (aDom.Domain2.UMax - aDom.Domain2.UMin) / aNbSamples;
    const double aDV2 = (aDom.Domain2.VMax - aDom.Domain2.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom.Domain2.UMin + i * aDU2;
      checkConePointAgainstPlane(aU2, aDom.Domain2.VMin, theTol, theMode);
      checkConePointAgainstPlane(aU2, aDom.Domain2.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV2 = aDom.Domain2.VMin + i * aDV2;
      checkConePointAgainstPlane(aDom.Domain2.UMin, aV2, theTol, theMode);
      checkConePointAgainstPlane(aDom.Domain2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a plane boundary point against the cone.
  void checkPlanePointAgainstCone(double thePlaneU, double thePlaneV, double theTol,
                                  ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aPlanePt = Value1(thePlaneU, thePlaneV);

    // Find closest point on cone axis to this plane point
    const gp_Pnt& aConeOrig = myCone.Location();
    const double aDx = aPlanePt.X() - aConeOrig.X();
    const double aDy = aPlanePt.Y() - aConeOrig.Y();
    const double aDz = aPlanePt.Z() - aConeOrig.Z();

    // V parameter along axis (from cone origin)
    const double aConeV = aDx * myConeAxisX + aDy * myConeAxisY + aDz * myConeAxisZ;

    // Point on axis at this V
    const double aAxisPtX = aConeOrig.X() + aConeV * myConeAxisX;
    const double aAxisPtY = aConeOrig.Y() + aConeV * myConeAxisY;
    const double aAxisPtZ = aConeOrig.Z() + aConeV * myConeAxisZ;

    // Radial direction from axis to point
    const double aRadX = aPlanePt.X() - aAxisPtX;
    const double aRadY = aPlanePt.Y() - aAxisPtY;
    const double aRadZ = aPlanePt.Z() - aAxisPtZ;
    const double aRadDist = std::sqrt(aRadX * aRadX + aRadY * aRadY + aRadZ * aRadZ);

    if (aRadDist < theTol)
    {
      // Point is on cone axis
      return;
    }

    // U angle
    const double aRadDirX = aRadX / aRadDist;
    const double aRadDirY = aRadY / aRadDist;
    const double aRadDirZ = aRadZ / aRadDist;

    double aConeU = std::atan2(aRadDirX * myConeYDirX + aRadDirY * myConeYDirY + aRadDirZ * myConeYDirZ,
                               aRadDirX * myConeXDirX + aRadDirY * myConeXDirY + aRadDirZ * myConeXDirZ);
    if (aConeU < 0)
      aConeU += ExtremaSS::THE_TWO_PI;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Check closest point (minimum)
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const double aClampedU = std::clamp(aConeU, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aConeV, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aConePt = Value2(aClampedU, aClampedV);
      const double aSqDist = aPlanePt.SquareDistance(aConePt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, thePlaneU, thePlaneV, aConePt, aPlanePt,
                               aSqDist, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, thePlaneU, thePlaneV, aClampedU, aClampedV, aPlanePt, aConePt,
                               aSqDist, true, theTol);
      }
    }

    // Check farthest point (maximum) - opposite side of cone
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      double aConeUFar = aConeU + M_PI;
      if (aConeUFar > ExtremaSS::THE_TWO_PI)
        aConeUFar -= ExtremaSS::THE_TWO_PI;

      const double aClampedU = std::clamp(aConeUFar, aDom.Domain2.UMin, aDom.Domain2.UMax);
      const double aClampedV = std::clamp(aConeV, aDom.Domain2.VMin, aDom.Domain2.VMax);
      const gp_Pnt aConePt = Value2(aClampedU, aClampedV);
      const double aSqDist = aPlanePt.SquareDistance(aConePt);

      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, thePlaneU, thePlaneV, aConePt, aPlanePt,
                               aSqDist, false, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, thePlaneU, thePlaneV, aClampedU, aClampedV, aPlanePt, aConePt,
                               aSqDist, false, theTol);
      }
    }
  }

  //! Check a cone boundary point against the plane.
  void checkConePointAgainstPlane(double theConeU, double theConeV, double theTol,
                                  ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aConePt = Value2(theConeU, theConeV);

    // Project cone point onto plane
    const double aDistToPlane = (aConePt.X() - myPlaneOrigX) * myPlaneNormX
                              + (aConePt.Y() - myPlaneOrigY) * myPlaneNormY
                              + (aConePt.Z() - myPlaneOrigZ) * myPlaneNormZ;

    const gp_Pnt aProjPt(aConePt.X() - aDistToPlane * myPlaneNormX,
                         aConePt.Y() - aDistToPlane * myPlaneNormY,
                         aConePt.Z() - aDistToPlane * myPlaneNormZ);

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
    const double aSqDist = aConePt.SquareDistance(aPlanePt);

    // Only minimum makes sense for fixed cone point
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      if (mySwapped)
      {
        ExtremaSS::AddExtremum(myResult, theConeU, theConeV, aClampedU, aClampedV, aConePt, aPlanePt,
                               aSqDist, true, theTol);
      }
      else
      {
        ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, theConeU, theConeV, aPlanePt, aConePt,
                               aSqDist, true, theTol);
      }
    }
  }

public:
  const gp_Pln&  Plane() const { return myPlane; }
  const gp_Cone& Cone() const { return myCone; }
  bool           IsSwapped() const { return mySwapped; }
  bool           IsBounded() const { return myDomain.has_value(); }

private:
  gp_Pln                             myPlane;
  gp_Cone                            myCone;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Cached plane components
  double myPlaneOrigX, myPlaneOrigY, myPlaneOrigZ;
  double myPlaneNormX, myPlaneNormY, myPlaneNormZ;
  double myPlaneXDirX, myPlaneXDirY, myPlaneXDirZ;
  double myPlaneYDirX, myPlaneYDirY, myPlaneYDirZ;

  // Cached cone components
  gp_Pnt myApex;
  double myApexX, myApexY, myApexZ;
  double myConeAxisX, myConeAxisY, myConeAxisZ;
  double myConeXDirX, myConeXDirY, myConeXDirZ;
  double myConeYDirX, myConeYDirY, myConeYDirZ;
  double mySemiAngle, myCosSemiAngle, mySinSemiAngle;
  double myRefRadius;

  // Cached dot products and coefficients
  double myAxisDotNorm;
  double myXDirDotNorm;
  double myYDirDotNorm;
  double myApexDistToPlane;
  double myC1, myC2, myC3;
  double myC2C3Norm;
};

#endif // _ExtremaSS_PlaneCone_HeaderFile
