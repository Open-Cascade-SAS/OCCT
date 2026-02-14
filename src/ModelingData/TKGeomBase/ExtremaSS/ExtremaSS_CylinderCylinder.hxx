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

#ifndef _ExtremaSS_CylinderCylinder_HeaderFile
#define _ExtremaSS_CylinderCylinder_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cylinder.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! Analytical computation of extrema between two cylinders.
//!
//! The algorithm distinguishes three cases based on cylinder axis relationship:
//!
//! **Case 1: Parallel axes**
//! The distance between any point on cylinder 1 and the corresponding circle
//! on cylinder 2 is constant along the V (height) parameter. Extrema occur at
//! specific U angles where the radial directions align or oppose.
//! - Minimum: d = |axis_dist - (R1 + R2)| at U1, U2 pointing toward/away from each other
//! - Maximum: d = axis_dist + R1 + R2 at U1, U2 pointing away from each other
//! - If axis_dist < R1 + R2, cylinders may overlap (distance = 0 or negative conceptually)
//!
//! **Case 2: Intersecting axes**
//! Special case of skew axes where the common perpendicular has zero length.
//! The axes intersect at a point, and extrema depend on the dihedral angle.
//!
//! **Case 3: Skew (non-parallel, non-intersecting) axes**
//! The minimum distance occurs along the common perpendicular to both axes.
//! Let N = Axis1 × Axis2 be the direction of the common perpendicular.
//! The distance between the axes is |(P2 - P1) · N| / |N|.
//! Extrema on the surfaces consider radii offsets in directions perpendicular to N.
//!
//! **Parameterization:**
//! - Cylinder: P(U, V) = Center + R*(cos(U)*XDir + sin(U)*YDir) + V*Axis
//! - U ∈ [0, 2π), V ∈ (-∞, +∞) or bounded domain
class ExtremaSS_CylinderCylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from two cylinders.
  //! @param theCyl1 First cylinder
  //! @param theCyl2 Second cylinder
  ExtremaSS_CylinderCylinder(const gp_Cylinder& theCyl1, const gp_Cylinder& theCyl2)
      : myCylinder1(theCyl1),
        myCylinder2(theCyl2)
  {
    initCache();
  }

  //! Constructor with bounded domain.
  //! @param theCyl1 First cylinder
  //! @param theCyl2 Second cylinder
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_CylinderCylinder(const gp_Cylinder& theCyl1,
                             const gp_Cylinder& theCyl2,
                             const ExtremaSS::Domain4D& theDomain)
      : myCylinder1(theCyl1),
        myCylinder2(theCyl2),
        myDomain(theDomain)
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

    // For bounded domain, also check boundary
    if (myDomain.has_value())
    {
      return PerformWithBoundary(theTol, theMode);
    }

    // Check if axes are parallel
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

    // First compute interior extrema
    if (myAxesParallel)
    {
      computeParallelCase(theTol, theMode);
    }
    else
    {
      computeSkewCase(theTol, theMode);
    }

    // For bounded domains, check boundary extrema
    if (myDomain.has_value())
    {
      checkBoundaryExtrema(theTol, theMode);
    }

    return myResult;
  }

  //! Evaluate point on first cylinder.
  //! @param theU U parameter (angle)
  //! @param theV V parameter (height)
  //! @return Point on cylinder 1
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(myCenter1.X() + myRadius1 * (aCosU * myXDir1.X() + aSinU * myYDir1.X())
                      + theV * myAxis1.X(),
                  myCenter1.Y() + myRadius1 * (aCosU * myXDir1.Y() + aSinU * myYDir1.Y())
                      + theV * myAxis1.Y(),
                  myCenter1.Z() + myRadius1 * (aCosU * myXDir1.Z() + aSinU * myYDir1.Z())
                      + theV * myAxis1.Z());
  }

  //! Evaluate point on second cylinder.
  //! @param theU U parameter (angle)
  //! @param theV V parameter (height)
  //! @return Point on cylinder 2
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(myCenter2.X() + myRadius2 * (aCosU * myXDir2.X() + aSinU * myYDir2.X())
                      + theV * myAxis2.X(),
                  myCenter2.Y() + myRadius2 * (aCosU * myXDir2.Y() + aSinU * myYDir2.Y())
                      + theV * myAxis2.Y(),
                  myCenter2.Z() + myRadius2 * (aCosU * myXDir2.Z() + aSinU * myYDir2.Z())
                      + theV * myAxis2.Z());
  }

  //! Check if surfaces were swapped.
  //! @return Always false (symmetric case)
  bool IsSwapped() const { return false; }

  //! Check if domain is bounded.
  //! @return True if bounded domain was provided
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Initialize cached values from cylinder geometry.
  void initCache()
  {
    // Extract cylinder 1 data
    const gp_Ax3& anAx1 = myCylinder1.Position();
    myCenter1           = anAx1.Location();
    myAxis1             = anAx1.Direction();
    myXDir1             = anAx1.XDirection();
    myYDir1             = anAx1.YDirection();
    myRadius1           = myCylinder1.Radius();

    // Extract cylinder 2 data
    const gp_Ax3& anAx2 = myCylinder2.Position();
    myCenter2           = anAx2.Location();
    myAxis2             = anAx2.Direction();
    myXDir2             = anAx2.XDirection();
    myYDir2             = anAx2.YDirection();
    myRadius2           = myCylinder2.Radius();

    // Check if axes are parallel using gp_Vec to avoid gp_Dir exception for parallel vectors
    const gp_Vec aCrossVec = gp_Vec(myAxis1).Crossed(gp_Vec(myAxis2));
    const double aCrossMag = aCrossVec.Magnitude();
    myAxesParallel = (aCrossMag < ExtremaSS::THE_ANGULAR_TOLERANCE);

    if (!myAxesParallel)
    {
      // Normalize the cross product (common perpendicular direction)
      myCrossProduct = gp_Dir(aCrossVec.X() / aCrossMag,
                              aCrossVec.Y() / aCrossMag,
                              aCrossVec.Z() / aCrossMag);
    }

    // Vector from center1 to center2
    myDeltaCenter =
        gp_Vec(myCenter2.X() - myCenter1.X(), myCenter2.Y() - myCenter1.Y(), myCenter2.Z() - myCenter1.Z());
  }

  //! Compute extrema for parallel cylinders.
  void computeParallelCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Project center2 onto the plane perpendicular to axis1 through center1
    const double aDotAxis = myDeltaCenter.Dot(gp_Vec(myAxis1));
    const gp_Vec aPerp(myDeltaCenter.X() - aDotAxis * myAxis1.X(),
                       myDeltaCenter.Y() - aDotAxis * myAxis1.Y(),
                       myDeltaCenter.Z() - aDotAxis * myAxis1.Z());

    const double aAxisDist = aPerp.Magnitude();

    // Check for coaxial cylinders
    if (aAxisDist < theTol)
    {
      // Coaxial cylinders: infinite solutions
      myResult.Status = ExtremaSS::Status::InfiniteSolutions;

      // Report the radial difference as distance
      const double aRadDiff = std::abs(myRadius1 - myRadius2);
      const double aRadSum  = myRadius1 + myRadius2;

      // Set InfiniteSquareDistance based on search mode
      if (theMode == ExtremaSS::SearchMode::Max)
      {
        myResult.InfiniteSquareDistance = aRadSum * aRadSum;
      }
      else
      {
        myResult.InfiniteSquareDistance = aRadDiff * aRadDiff;
      }

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        // Minimum at any U, V=0 (arbitrary)
        addExtremum(0.0, 0.0, 0.0, 0.0, aRadDiff * aRadDiff, true, theTol);
      }
      if (theMode != ExtremaSS::SearchMode::Min)
      {
        // Maximum at opposite U values
        addExtremum(0.0, 0.0, M_PI, 0.0, aRadSum * aRadSum, false, theTol);
      }
      return;
    }

    // Direction from axis1 toward axis2 (in the perpendicular plane)
    const gp_Dir aDirToAxis2(aPerp.X() / aAxisDist, aPerp.Y() / aAxisDist, aPerp.Z() / aAxisDist);

    // Find U1 where cylinder1's radial direction points toward axis2
    // Radial direction at U1 is: cos(U1)*XDir1 + sin(U1)*YDir1 = aDirToAxis2
    const double aDot1X = aDirToAxis2.X() * myXDir1.X() + aDirToAxis2.Y() * myXDir1.Y()
                          + aDirToAxis2.Z() * myXDir1.Z();
    const double aDot1Y = aDirToAxis2.X() * myYDir1.X() + aDirToAxis2.Y() * myYDir1.Y()
                          + aDirToAxis2.Z() * myYDir1.Z();
    const double aU1Toward = std::atan2(aDot1Y, aDot1X);
    const double aU1Away   = aU1Toward + M_PI;

    // Direction from axis2 toward axis1
    const gp_Dir aDirToAxis1(-aDirToAxis2.X(), -aDirToAxis2.Y(), -aDirToAxis2.Z());

    // Find U2 where cylinder2's radial direction points toward axis1
    const double aDot2X = aDirToAxis1.X() * myXDir2.X() + aDirToAxis1.Y() * myXDir2.Y()
                          + aDirToAxis1.Z() * myXDir2.Z();
    const double aDot2Y = aDirToAxis1.X() * myYDir2.X() + aDirToAxis1.Y() * myYDir2.Y()
                          + aDirToAxis1.Z() * myYDir2.Z();
    const double aU2Toward = std::atan2(aDot2Y, aDot2X);
    const double aU2Away   = aU2Toward + M_PI;

    // Parallel axes: infinite solutions along V
    // The minimum/maximum distances are constant along V
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // Minimum distance: both pointing toward each other
    // Distance = aAxisDist - R1 - R2
    const double aMinDist = aAxisDist - myRadius1 - myRadius2;

    // Maximum distance: both pointing away from each other
    // Distance = aAxisDist + R1 + R2
    const double aMaxDist = aAxisDist + myRadius1 + myRadius2;

    // For parallel cylinders, set InfiniteSquareDistance to the minimum
    // This is the constant distance along the entire length
    if (theMode == ExtremaSS::SearchMode::Max)
    {
      myResult.InfiniteSquareDistance = aMaxDist * aMaxDist;
    }
    else
    {
      myResult.InfiniteSquareDistance = std::max(0.0, aMinDist) * std::max(0.0, aMinDist);
    }

    // Also add representative extremum points
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const double aMinSqDist = std::max(0.0, aMinDist) * std::max(0.0, aMinDist);
      addExtremum(aU1Toward, 0.0, aU2Toward, 0.0, aMinSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const double aMaxSqDist = aMaxDist * aMaxDist;
      addExtremum(aU1Away, 0.0, aU2Away, 0.0, aMaxSqDist, false, theTol);
    }
  }

  //! Compute extrema for skew (non-parallel) cylinders.
  void computeSkewCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // For skew lines, find the common perpendicular
    // The common perpendicular direction is myCrossProduct = Axis1 × Axis2 (already normalized)
    // Distance between axes = |(C2 - C1) · N| where N is the common perpendicular

    const double aDotN = myDeltaCenter.Dot(gp_Vec(myCrossProduct));
    const double aAxisDist = std::abs(aDotN);

    // Find the closest points on each axis
    // Parameterize: P1(t) = C1 + t*A1, P2(s) = C2 + s*A2
    // Closest approach: (P2(s) - P1(t)) · A1 = 0 and (P2(s) - P1(t)) · A2 = 0

    const double aA1DotA1 = 1.0; // Axis is unit vector
    const double aA2DotA2 = 1.0;
    const double aA1DotA2 = myAxis1.X() * myAxis2.X() + myAxis1.Y() * myAxis2.Y() + myAxis1.Z() * myAxis2.Z();
    const double aDeltaDotA1 =
        myDeltaCenter.X() * myAxis1.X() + myDeltaCenter.Y() * myAxis1.Y() + myDeltaCenter.Z() * myAxis1.Z();
    const double aDeltaDotA2 =
        myDeltaCenter.X() * myAxis2.X() + myDeltaCenter.Y() * myAxis2.Y() + myDeltaCenter.Z() * myAxis2.Z();

    // Solve: t - s*cos(θ) = (C2-C1)·A1, -t*cos(θ) + s = (C2-C1)·A2
    // where cos(θ) = A1·A2
    const double aDenom = aA1DotA1 * aA2DotA2 - aA1DotA2 * aA1DotA2;

    double aT = 0.0;
    double aS = 0.0;

    if (std::abs(aDenom) > ExtremaSS::THE_ANGULAR_TOLERANCE * ExtremaSS::THE_ANGULAR_TOLERANCE)
    {
      aT = (aDeltaDotA1 * aA2DotA2 - aDeltaDotA2 * aA1DotA2) / aDenom;
      aS = (aDeltaDotA2 * aA1DotA1 - aDeltaDotA1 * aA1DotA2) / aDenom;
    }

    // Points on axes closest to each other
    const gp_Pnt aP1OnAxis(myCenter1.X() + aT * myAxis1.X(),
                           myCenter1.Y() + aT * myAxis1.Y(),
                           myCenter1.Z() + aT * myAxis1.Z());
    const gp_Pnt aP2OnAxis(myCenter2.X() + aS * myAxis2.X(),
                           myCenter2.Y() + aS * myAxis2.Y(),
                           myCenter2.Z() + aS * myAxis2.Z());

    // Direction from P1OnAxis to P2OnAxis
    gp_Vec aDirBetween(aP2OnAxis.X() - aP1OnAxis.X(),
                       aP2OnAxis.Y() - aP1OnAxis.Y(),
                       aP2OnAxis.Z() - aP1OnAxis.Z());

    const double aDist = aDirBetween.Magnitude();

    if (aDist < theTol)
    {
      // Axes intersect: special handling needed
      computeIntersectingAxesCase(aP1OnAxis, theTol, theMode);
      return;
    }

    // Normalize direction
    aDirBetween.Divide(aDist);

    // Project this direction onto the perpendicular plane of each cylinder
    // For cylinder 1: project onto plane perpendicular to Axis1
    gp_Vec aDir1Perp = aDirBetween - gp_Vec(myAxis1) * aDirBetween.Dot(gp_Vec(myAxis1));
    const double aDir1PerpMag = aDir1Perp.Magnitude();

    // For cylinder 2: project onto plane perpendicular to Axis2
    gp_Vec aDir2Perp = aDirBetween - gp_Vec(myAxis2) * aDirBetween.Dot(gp_Vec(myAxis2));
    const double aDir2PerpMag = aDir2Perp.Magnitude();

    myResult.Status = ExtremaSS::Status::OK;

    // For skew cylinders, we need to find the U values that give extrema
    // The problem reduces to finding angles that optimize the distance

    // When the direction between closest axis points has a component in each
    // cylinder's perpendicular plane, the extrema occur when the surface points
    // are aligned along or opposite to these projected directions

    if (aDir1PerpMag < theTol && aDir2PerpMag < theTol)
    {
      // Both projections are negligible - axes nearly intersect perpendicular to the
      // line connecting their closest points. This is a degenerate case.
      // Extrema can occur at any U, so we have infinite solutions
      myResult.Status = ExtremaSS::Status::InfiniteSolutions;

      if (theMode != ExtremaSS::SearchMode::Max)
      {
        const double aMinDist = std::max(0.0, aDist - myRadius1 - myRadius2);
        addExtremum(0.0, aT, 0.0, aS, aMinDist * aMinDist, true, theTol);
      }
      if (theMode != ExtremaSS::SearchMode::Min)
      {
        const double aMaxDist = aDist + myRadius1 + myRadius2;
        addExtremum(0.0, aT, M_PI, aS, aMaxDist * aMaxDist, false, theTol);
      }
      return;
    }

    // Find U1 and U2 angles for extrema
    // For minimum: points on cylinders should point toward each other
    // For maximum: points should point away from each other

    // We need to search for extrema by considering the geometry
    // The problem is more complex for skew cylinders

    // Approach: The distance function is:
    // d²(U1, V1, U2, V2) = |P1(U1,V1) - P2(U2,V2)|²
    // For the closest points along the common perpendicular, V1 ≈ t, V2 ≈ s

    // At these V values, we need to find optimal U1, U2
    // The offset from axis in direction aDirBetween is what matters

    // For cylinder 1 at U1: radial direction is cos(U1)*XDir1 + sin(U1)*YDir1
    // Projection onto aDirBetween: R1 * (cos(U1)*(XDir1·D) + sin(U1)*(YDir1·D))

    const double aX1DotD = myXDir1.X() * aDirBetween.X() + myXDir1.Y() * aDirBetween.Y() + myXDir1.Z() * aDirBetween.Z();
    const double aY1DotD = myYDir1.X() * aDirBetween.X() + myYDir1.Y() * aDirBetween.Y() + myYDir1.Z() * aDirBetween.Z();

    const double aX2DotD = myXDir2.X() * aDirBetween.X() + myXDir2.Y() * aDirBetween.Y() + myXDir2.Z() * aDirBetween.Z();
    const double aY2DotD = myYDir2.X() * aDirBetween.X() + myYDir2.Y() * aDirBetween.Y() + myYDir2.Z() * aDirBetween.Z();

    // Maximum projection for cylinder 1 in direction D: at U1 = atan2(Y1·D, X1·D)
    const double aU1Toward = std::atan2(aY1DotD, aX1DotD);
    const double aU1Away = aU1Toward + M_PI;

    // For cylinder 2, we want projection in -D direction for minimum
    const double aU2Toward = std::atan2(-aY2DotD, -aX2DotD);
    const double aU2Away = aU2Toward + M_PI;

    // Maximum projections
    const double aProj1Max = myRadius1 * std::sqrt(aX1DotD * aX1DotD + aY1DotD * aY1DotD);
    const double aProj2Max = myRadius2 * std::sqrt(aX2DotD * aX2DotD + aY2DotD * aY2DotD);

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Minimum: points pointing toward each other
      // Distance = aDist - proj1 - proj2 (if positive)
      const double aMinDist = std::max(0.0, aDist - aProj1Max - aProj2Max);
      addExtremum(aU1Toward, aT, aU2Toward, aS, aMinDist * aMinDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum: points pointing away from each other
      const double aMaxDist = aDist + aProj1Max + aProj2Max;
      addExtremum(aU1Away, aT, aU2Away, aS, aMaxDist * aMaxDist, false, theTol);
    }
  }

  //! Compute extrema when axes intersect.
  void computeIntersectingAxesCase(const gp_Pnt& theIntersection,
                                   double        theTol,
                                   ExtremaSS::SearchMode theMode) const
  {
    // Axes intersect at theIntersection
    // The dihedral angle between cylinders affects the extrema

    // Find V parameters for intersection point
    const double aV1 = gp_Vec(myCenter1, theIntersection).Dot(gp_Vec(myAxis1));
    const double aV2 = gp_Vec(myCenter2, theIntersection).Dot(gp_Vec(myAxis2));

    // The cross product gives us a direction perpendicular to both axes
    // Points on the cylinders in this direction will be at the dihedral

    // For minimum distance, we need to consider the relative geometry
    // The minimum occurs where the cylinder surfaces come closest

    // Cross product direction
    const gp_Dir& aCrossDir = myCrossProduct;

    // Project cross direction onto each cylinder's perpendicular plane
    // For cylinder 1:
    const double aC1X = aCrossDir.X() * myXDir1.X() + aCrossDir.Y() * myXDir1.Y() + aCrossDir.Z() * myXDir1.Z();
    const double aC1Y = aCrossDir.X() * myYDir1.X() + aCrossDir.Y() * myYDir1.Y() + aCrossDir.Z() * myYDir1.Z();
    const double aU1Cross = std::atan2(aC1Y, aC1X);

    // For cylinder 2:
    const double aC2X = aCrossDir.X() * myXDir2.X() + aCrossDir.Y() * myXDir2.Y() + aCrossDir.Z() * myXDir2.Z();
    const double aC2Y = aCrossDir.X() * myYDir2.X() + aCrossDir.Y() * myYDir2.Y() + aCrossDir.Z() * myYDir2.Z();
    const double aU2Cross = std::atan2(aC2Y, aC2X);

    myResult.Status = ExtremaSS::Status::OK;

    // Four candidate extrema pairs: combinations of ±cross direction for each cylinder
    const double aU1Vals[2] = {aU1Cross, aU1Cross + M_PI};
    const double aU2Vals[2] = {aU2Cross, aU2Cross + M_PI};

    for (int i = 0; i < 2; ++i)
    {
      for (int j = 0; j < 2; ++j)
      {
        const double aU1 = aU1Vals[i];
        const double aU2 = aU2Vals[j];

        const gp_Pnt aP1 = Value1(aU1, aV1);
        const gp_Pnt aP2 = Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        // Determine if this is a minimum or maximum
        const bool aIsMin = (i == j); // Same side = minimum, opposite = maximum

        if ((aIsMin && theMode != ExtremaSS::SearchMode::Max) ||
            (!aIsMin && theMode != ExtremaSS::SearchMode::Min))
        {
          addExtremum(aU1, aV1, aU2, aV2, aSqDist, aIsMin, theTol);
        }
      }
    }
  }

  //! Check boundary extrema for bounded domains.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
    {
      return;
    }

    const ExtremaSS::Domain4D& aDom = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    // Sample boundary edges
    constexpr int aNbSamples = 20;

    // Check boundaries of domain 1 against interior of domain 2
    // V1 = VMin1 and V1 = VMax1 edges
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;

      // V1 = VMin1
      checkPointAgainstSurface2(aU1, aDom1.VMin, theTol, theMode);
      // V1 = VMax1
      checkPointAgainstSurface2(aU1, aDom1.VMax, theTol, theMode);
    }

    // U1 = UMin1 and U1 = UMax1 edges
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;

      checkPointAgainstSurface2(aDom1.UMin, aV1, theTol, theMode);
      checkPointAgainstSurface2(aDom1.UMax, aV1, theTol, theMode);
    }

    // Similarly for domain 2 boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;

      checkPointAgainstSurface1(aU2, aDom2.VMin, theTol, theMode);
      checkPointAgainstSurface1(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;

      checkPointAgainstSurface1(aDom2.UMin, aV2, theTol, theMode);
      checkPointAgainstSurface1(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on surface 1 against surface 2 for potential extrema.
  void checkPointAgainstSurface2(double theU1, double theV1, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Find closest/farthest point on cylinder 2 to aP1
    // Project aP1 onto cylinder 2's axis
    const gp_Vec aVecToP1(myCenter2, aP1);
    const double aV2 = aVecToP1.Dot(gp_Vec(myAxis2));

    // Check if V2 is in domain
    if (myDomain.has_value())
    {
      const double aV2Clamped = std::clamp(aV2, myDomain->Domain2.VMin, myDomain->Domain2.VMax);
      if (std::abs(aV2Clamped - aV2) > theTol)
      {
        // V2 is outside domain, would need boundary handling
        return;
      }
    }

    // Point on axis at V2
    const gp_Pnt aPOnAxis2(myCenter2.X() + aV2 * myAxis2.X(),
                           myCenter2.Y() + aV2 * myAxis2.Y(),
                           myCenter2.Z() + aV2 * myAxis2.Z());

    // Direction from axis to P1, projected onto perpendicular plane
    gp_Vec aDirToP1(aPOnAxis2, aP1);
    aDirToP1 = aDirToP1 - gp_Vec(myAxis2) * aDirToP1.Dot(gp_Vec(myAxis2));
    const double aDistToAxis = aDirToP1.Magnitude();

    if (aDistToAxis < theTol)
    {
      // P1 is on the axis of cylinder 2
      return;
    }

    aDirToP1.Divide(aDistToAxis);

    // U2 for minimum: pointing toward P1
    const double aU2X = aDirToP1.Dot(gp_Vec(myXDir2));
    const double aU2Y = aDirToP1.Dot(gp_Vec(myYDir2));
    const double aU2Min = std::atan2(aU2Y, aU2X);
    const double aU2Max = aU2Min + M_PI;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP2 = Value2(aU2Min, aV2);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(theU1, theV1, aU2Min, aV2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const gp_Pnt aP2 = Value2(aU2Max, aV2);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(theU1, theV1, aU2Max, aV2, aSqDist, false, theTol);
    }
  }

  //! Check a point on surface 2 against surface 1 for potential extrema.
  void checkPointAgainstSurface1(double theU2, double theV2, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    // Find closest/farthest point on cylinder 1 to aP2
    const gp_Vec aVecToP2(myCenter1, aP2);
    const double aV1 = aVecToP2.Dot(gp_Vec(myAxis1));

    if (myDomain.has_value())
    {
      const double aV1Clamped = std::clamp(aV1, myDomain->Domain1.VMin, myDomain->Domain1.VMax);
      if (std::abs(aV1Clamped - aV1) > theTol)
      {
        return;
      }
    }

    const gp_Pnt aPOnAxis1(myCenter1.X() + aV1 * myAxis1.X(),
                           myCenter1.Y() + aV1 * myAxis1.Y(),
                           myCenter1.Z() + aV1 * myAxis1.Z());

    gp_Vec aDirToP2(aPOnAxis1, aP2);
    aDirToP2 = aDirToP2 - gp_Vec(myAxis1) * aDirToP2.Dot(gp_Vec(myAxis1));
    const double aDistToAxis = aDirToP2.Magnitude();

    if (aDistToAxis < theTol)
    {
      return;
    }

    aDirToP2.Divide(aDistToAxis);

    const double aU1X = aDirToP2.Dot(gp_Vec(myXDir1));
    const double aU1Y = aDirToP2.Dot(gp_Vec(myYDir1));
    const double aU1Min = std::atan2(aU1Y, aU1X);
    const double aU1Max = aU1Min + M_PI;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP1 = Value1(aU1Min, aV1);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aU1Min, aV1, theU2, theV2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const gp_Pnt aP1 = Value1(aU1Max, aV1);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aU1Max, aV1, theU2, theV2, aSqDist, false, theTol);
    }
  }

  //! Normalize angle to [0, 2*PI).
  static double normalizeAngle(double theAngle)
  {
    constexpr double aTwoPi = 2.0 * M_PI;
    double aResult = std::fmod(theAngle, aTwoPi);
    if (aResult < 0.0)
    {
      aResult += aTwoPi;
    }
    return aResult;
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
    // Normalize U angles to [0, 2π)
    const double aU1 = normalizeAngle(theU1);
    const double aU2 = normalizeAngle(theU2);

    // Check for duplicates
    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      if (std::abs(anExisting.SquareDistance - theSqDist) < aTolSq)
      {
        // Check parameter proximity
        const double aDU1 = std::abs(anExisting.U1 - aU1);
        const double aDU2 = std::abs(anExisting.U2 - aU2);
        const double aDV1 = std::abs(anExisting.V1 - theV1);
        const double aDV2 = std::abs(anExisting.V2 - theV2);

        if ((aDU1 < theTol || std::abs(aDU1 - 2 * M_PI) < theTol) &&
            (aDU2 < theTol || std::abs(aDU2 - 2 * M_PI) < theTol) &&
            aDV1 < theTol && aDV2 < theTol)
        {
          return; // Duplicate
        }
      }
    }

    ExtremaSS::ExtremumResult aResult;
    aResult.U1             = aU1;
    aResult.V1             = theV1;
    aResult.U2             = aU2;
    aResult.V2             = theV2;
    aResult.Point1         = Value1(aU1, theV1);
    aResult.Point2         = Value2(aU2, theV2);
    aResult.SquareDistance = theSqDist;
    aResult.IsMinimum      = theIsMin;

    myResult.Extrema.Append(aResult);

    if (myResult.Status != ExtremaSS::Status::InfiniteSolutions)
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
  }

private:
  gp_Cylinder myCylinder1; //!< First cylinder
  gp_Cylinder myCylinder2; //!< Second cylinder

  std::optional<ExtremaSS::Domain4D> myDomain; //!< Optional bounded domain

  // Cached geometry for cylinder 1
  gp_Pnt myCenter1; //!< Center of cylinder 1
  gp_Dir myAxis1;   //!< Axis direction of cylinder 1
  gp_Dir myXDir1;   //!< X direction of cylinder 1
  gp_Dir myYDir1;   //!< Y direction of cylinder 1
  double myRadius1; //!< Radius of cylinder 1

  // Cached geometry for cylinder 2
  gp_Pnt myCenter2; //!< Center of cylinder 2
  gp_Dir myAxis2;   //!< Axis direction of cylinder 2
  gp_Dir myXDir2;   //!< X direction of cylinder 2
  gp_Dir myYDir2;   //!< Y direction of cylinder 2
  double myRadius2; //!< Radius of cylinder 2

  // Cached relationship
  gp_Dir myCrossProduct; //!< Cross product of axes (common perpendicular direction)
  gp_Vec myDeltaCenter;  //!< Vector from center1 to center2
  bool   myAxesParallel; //!< True if axes are parallel

  mutable ExtremaSS::Result myResult; //!< Computation result
};

#endif // _ExtremaSS_CylinderCylinder_HeaderFile
