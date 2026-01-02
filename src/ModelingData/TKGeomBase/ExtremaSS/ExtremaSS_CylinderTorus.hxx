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

#ifndef _ExtremaSS_CylinderTorus_HeaderFile
#define _ExtremaSS_CylinderTorus_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Torus.hxx>
#include <MathOpt_Powell.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! Analytical computation of extrema between a cylinder and a torus.
//!
//! The algorithm considers the torus as a family of generating circles
//! parameterized by U. For each U, we have a circle-cylinder problem.
//!
//! **Case 1: Cylinder axis parallel to torus axis**
//! The problem has rotational symmetry about the common axis direction.
//! Extrema occur at specific U angles where the generating circle is
//! closest/farthest from the cylinder axis.
//!
//! **Case 2: Cylinder axis perpendicular to torus axis**
//! Need to search over torus U to find optimal generating circles.
//!
//! **Case 3: General skew axes**
//! Full 2D optimization over torus U and implicitly cylinder V.
//!
//! **Parameterization:**
//! - Cylinder: P(U, V) = Center + R*(cos(U)*XDir + sin(U)*YDir) + V*Axis
//! - Torus: P(U, V) = Center + (R + r*cos(V))*(cos(U)*XDir + sin(U)*YDir) + r*sin(V)*Axis
//!   where R is major radius, r is minor radius
class ExtremaSS_CylinderTorus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from cylinder and torus.
  //! @param theCylinder The cylinder
  //! @param theTorus The torus
  ExtremaSS_CylinderTorus(const gp_Cylinder& theCylinder, const gp_Torus& theTorus)
      : myCylinder(theCylinder),
        myTorus(theTorus),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor from torus and cylinder (swapped order).
  //! @param theTorus The torus
  //! @param theCylinder The cylinder
  ExtremaSS_CylinderTorus(const gp_Torus& theTorus, const gp_Cylinder& theCylinder)
      : myCylinder(theCylinder),
        myTorus(theTorus),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with bounded domain.
  //! @param theCylinder The cylinder
  //! @param theTorus The torus
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_CylinderTorus(const gp_Cylinder&         theCylinder,
                          const gp_Torus&            theTorus,
                          const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        myTorus(theTorus),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with bounded domain (swapped order).
  //! @param theTorus The torus
  //! @param theCylinder The cylinder
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_CylinderTorus(const gp_Torus&            theTorus,
                          const gp_Cylinder&         theCylinder,
                          const ExtremaSS::Domain4D& theDomain)
      : myCylinder(theCylinder),
        myTorus(theTorus),
        myDomain(ExtremaSS::Domain4D{theDomain.Domain2, theDomain.Domain1}),
        mySwapped(true)
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

    if (myDomain.has_value())
    {
      return PerformWithBoundary(theTol, theMode);
    }

    if (myAxesParallel)
    {
      computeParallelCase(theTol, theMode);
    }
    else
    {
      computeGeneralCase(theTol, theMode);
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

    if (myAxesParallel)
    {
      computeParallelCase(theTol, theMode);
    }
    else
    {
      computeGeneralCase(theTol, theMode);
    }

    if (myDomain.has_value())
    {
      checkBoundaryExtrema(theTol, theMode);
    }

    return myResult;
  }

  //! Evaluate point on cylinder (surface 1).
  //! @param theU U parameter (angle)
  //! @param theV V parameter (height)
  //! @return Point on cylinder
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    return gp_Pnt(myCylCenter.X() + myCylRadius * (aCosU * myCylXDir.X() + aSinU * myCylYDir.X())
                    + theV * myCylAxis.X(),
                  myCylCenter.Y() + myCylRadius * (aCosU * myCylXDir.Y() + aSinU * myCylYDir.Y())
                    + theV * myCylAxis.Y(),
                  myCylCenter.Z() + myCylRadius * (aCosU * myCylXDir.Z() + aSinU * myCylYDir.Z())
                    + theV * myCylAxis.Z());
  }

  //! Evaluate point on torus (surface 2).
  //! @param theU U parameter (angle around major circle)
  //! @param theV V parameter (angle around minor circle)
  //! @return Point on torus
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aCosV   = std::cos(theV);
    const double aSinV   = std::sin(theV);
    const double aRadius = myMajorRadius + myMinorRadius * aCosV;
    return gp_Pnt(myTorusCenter.X() + aRadius * (aCosU * myTorusXDir.X() + aSinU * myTorusYDir.X())
                    + myMinorRadius * aSinV * myTorusAxis.X(),
                  myTorusCenter.Y() + aRadius * (aCosU * myTorusXDir.Y() + aSinU * myTorusYDir.Y())
                    + myMinorRadius * aSinV * myTorusAxis.Y(),
                  myTorusCenter.Z() + aRadius * (aCosU * myTorusXDir.Z() + aSinU * myTorusYDir.Z())
                    + myMinorRadius * aSinV * myTorusAxis.Z());
  }

  //! Check if surfaces were swapped.
  //! @return True if torus was provided first
  bool IsSwapped() const { return mySwapped; }

  //! Check if domain is bounded.
  //! @return True if bounded domain was provided
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Initialize cached values from geometry.
  void initCache()
  {
    // Extract cylinder data
    const gp_Ax3& aCylAx = myCylinder.Position();
    myCylCenter          = aCylAx.Location();
    myCylAxis            = aCylAx.Direction();
    myCylXDir            = aCylAx.XDirection();
    myCylYDir            = aCylAx.YDirection();
    myCylRadius          = myCylinder.Radius();

    // Extract torus data
    const gp_Ax3& aTorusAx = myTorus.Position();
    myTorusCenter          = aTorusAx.Location();
    myTorusAxis            = aTorusAx.Direction();
    myTorusXDir            = aTorusAx.XDirection();
    myTorusYDir            = aTorusAx.YDirection();
    myMajorRadius          = myTorus.MajorRadius();
    myMinorRadius          = myTorus.MinorRadius();

    // Check if axes are parallel using gp_Vec to avoid gp_Dir exception for parallel vectors
    const gp_Vec aCrossVec = gp_Vec(myCylAxis).Crossed(gp_Vec(myTorusAxis));
    const double aCrossMag = aCrossVec.Magnitude();
    myAxesParallel         = (aCrossMag < ExtremaSS::THE_ANGULAR_TOLERANCE);

    if (!myAxesParallel)
    {
      myCrossProduct =
        gp_Dir(aCrossVec.X() / aCrossMag, aCrossVec.Y() / aCrossMag, aCrossVec.Z() / aCrossMag);
    }

    // Vector from cylinder center to torus center
    myDeltaCenter = gp_Vec(myTorusCenter.X() - myCylCenter.X(),
                           myTorusCenter.Y() - myCylCenter.Y(),
                           myTorusCenter.Z() - myCylCenter.Z());
  }

  //! Compute extrema for parallel axes case.
  void computeParallelCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Project torus center onto plane perpendicular to cylinder axis through cylinder center
    const double aDotAxis = myDeltaCenter.Dot(gp_Vec(myCylAxis));
    const gp_Vec aPerp(myDeltaCenter.X() - aDotAxis * myCylAxis.X(),
                       myDeltaCenter.Y() - aDotAxis * myCylAxis.Y(),
                       myDeltaCenter.Z() - aDotAxis * myCylAxis.Z());

    const double aAxisDist = aPerp.Magnitude();

    myResult.Status = ExtremaSS::Status::OK;

    if (aAxisDist < theTol)
    {
      // Coaxial case
      computeCoaxialCase(theTol, theMode);
      return;
    }

    // Direction from cylinder axis toward torus center
    const gp_Dir aDirToTorus(aPerp.X() / aAxisDist, aPerp.Y() / aAxisDist, aPerp.Z() / aAxisDist);

    // Find U_cyl where cylinder points toward torus
    const double aCylDotX = aDirToTorus.X() * myCylXDir.X() + aDirToTorus.Y() * myCylXDir.Y()
                            + aDirToTorus.Z() * myCylXDir.Z();
    const double aCylDotY = aDirToTorus.X() * myCylYDir.X() + aDirToTorus.Y() * myCylYDir.Y()
                            + aDirToTorus.Z() * myCylYDir.Z();
    const double aUCylToward = std::atan2(aCylDotY, aCylDotX);
    const double aUCylAway   = aUCylToward + M_PI;

    // Find U_torus where torus points toward/away from cylinder
    const gp_Dir aDirFromTorus(-aDirToTorus.X(), -aDirToTorus.Y(), -aDirToTorus.Z());
    const double aTorusDotX = aDirFromTorus.X() * myTorusXDir.X()
                              + aDirFromTorus.Y() * myTorusXDir.Y()
                              + aDirFromTorus.Z() * myTorusXDir.Z();
    const double aTorusDotY = aDirFromTorus.X() * myTorusYDir.X()
                              + aDirFromTorus.Y() * myTorusYDir.Y()
                              + aDirFromTorus.Z() * myTorusYDir.Z();
    const double aUTorusToward = std::atan2(aTorusDotY, aTorusDotX);
    const double aUTorusAway   = aUTorusToward + M_PI;

    // V_cyl at torus center height
    const double aVCyl = aDotAxis;

    // For parallel axes, extrema occur at specific U_torus values
    // The generating circle at U_torus closest to cylinder axis

    // Minimum distance scenarios:
    // 1. Generating circle toward cylinder: distance = aAxisDist - R - r - R_cyl (innermost)
    // 2. Generating circle away from cylinder: distance = aAxisDist + R + r - R_cyl (outermost)

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Check generating circle toward cylinder (U_torus = aUTorusToward)
      // Center of this circle is at distance (aAxisDist - R) from cylinder axis
      const double aCircleCenterDist = aAxisDist - myMajorRadius;

      // For this circle, find the closest point to the cylinder
      // The circle has center at distance aCircleCenterDist from cylinder axis
      // with radius myMinorRadius in the plane perpendicular to torus axis

      // V_torus = 0 (pointing toward cylinder axis)
      const double aMinDist1 = std::abs(aCircleCenterDist - myCylRadius) - myMinorRadius;
      addExtremum(aUCylToward, aVCyl, aUTorusToward, 0.0, aMinDist1 * aMinDist1, true, theTol);

      // Also check the generating circle away from cylinder
      const double aCircleCenterDistAway = aAxisDist + myMajorRadius;
      const double aMinDistAway          = aCircleCenterDistAway - myCylRadius - myMinorRadius;
      addExtremum(aUCylToward, aVCyl, aUTorusAway, 0.0, aMinDistAway * aMinDistAway, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum distance: generating circle away, V_torus = 0 (outward)
      const double aMaxDist = aAxisDist + myMajorRadius + myMinorRadius + myCylRadius;
      addExtremum(aUCylAway, aVCyl, aUTorusAway, 0.0, aMaxDist * aMaxDist, false, theTol);
    }
  }

  //! Compute extrema when axes are coaxial.
  void computeCoaxialCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // V_cyl at torus center height
    const double aDotAxis = myDeltaCenter.Dot(gp_Vec(myCylAxis));
    const double aVCyl    = aDotAxis;

    // For coaxial surfaces, any U_cyl and U_torus give the same radial relationship
    // The distance depends only on V_torus

    // The torus outer radius is R + r, inner radius is R - r (if R > r)
    // Cylinder has radius R_cyl

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      // Minimum at V_torus where torus is closest to cylinder
      // If R_cyl is between R-r and R+r, minimum is 0
      // Otherwise minimum is |R_cyl - (R+r)| or |R_cyl - (R-r)|

      const double aOuterRadius = myMajorRadius + myMinorRadius;
      const double aInnerRadius = myMajorRadius - myMinorRadius;

      double aMinDist = 0.0;
      double aVTorus  = 0.0;

      if (myCylRadius >= aOuterRadius)
      {
        aMinDist = myCylRadius - aOuterRadius;
        aVTorus  = 0.0; // Outer point of generating circle
      }
      else if (myCylRadius <= aInnerRadius && aInnerRadius > 0)
      {
        aMinDist = aInnerRadius - myCylRadius;
        aVTorus  = M_PI; // Inner point of generating circle
      }
      else
      {
        // Cylinder radius is within torus annulus
        aMinDist = 0.0;
        // V_torus where R + r*cos(V) = R_cyl
        const double aCosV = (myCylRadius - myMajorRadius) / myMinorRadius;
        aVTorus            = ExtremaSS::SafeAcos(aCosV);
      }

      addExtremum(0.0, aVCyl, 0.0, aVTorus, aMinDist * aMinDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      // Maximum at V_torus = 0 (outer) or V_torus = pi (inner) depending on cylinder radius
      const double aMaxDist1 = myCylRadius + myMajorRadius + myMinorRadius;

      addExtremum(0.0, aVCyl, M_PI, 0.0, aMaxDist1 * aMaxDist1, false, theTol);
    }

    // Compute InfiniteSquareDistance - minimum distance for coaxial cylinder-torus
    const double aOuterRadius = myMajorRadius + myMinorRadius;
    const double aInnerRadius = myMajorRadius - myMinorRadius;
    double       aMinDist     = 0.0;

    if (myCylRadius >= aOuterRadius)
    {
      aMinDist = myCylRadius - aOuterRadius;
    }
    else if (myCylRadius <= aInnerRadius && aInnerRadius > 0)
    {
      aMinDist = aInnerRadius - myCylRadius;
    }
    // else cylinder is within torus annulus, distance is 0

    myResult.InfiniteSquareDistance = aMinDist * aMinDist;
  }

  //! Compute extrema for general (non-parallel) axes case.
  void computeGeneralCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::OK;

    // For non-parallel axes, we need to search over torus U parameter
    // For each U_torus, the generating circle has a center and we find
    // extrema to the cylinder

    constexpr int aNbSamples = 72; // 5 degree increments

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 0, aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU1 = 0, aBestMaxV1 = 0, aBestMaxU2 = 0, aBestMaxV2 = 0;

    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aUTorus = 2.0 * M_PI * i / aNbSamples;

      // Generating circle center
      const double aCosU = std::cos(aUTorus);
      const double aSinU = std::sin(aUTorus);
      const gp_Pnt aCircleCenter(
        myTorusCenter.X() + myMajorRadius * (aCosU * myTorusXDir.X() + aSinU * myTorusYDir.X()),
        myTorusCenter.Y() + myMajorRadius * (aCosU * myTorusXDir.Y() + aSinU * myTorusYDir.Y()),
        myTorusCenter.Z() + myMajorRadius * (aCosU * myTorusXDir.Z() + aSinU * myTorusYDir.Z()));

      // Direction from torus center to circle center (radial direction)
      const gp_Dir aRadialDir(aCosU * myTorusXDir.X() + aSinU * myTorusYDir.X(),
                              aCosU * myTorusXDir.Y() + aSinU * myTorusYDir.Y(),
                              aCosU * myTorusXDir.Z() + aSinU * myTorusYDir.Z());

      // Find closest point on cylinder axis to circle center
      const gp_Vec aVecToCircle(myCylCenter, aCircleCenter);
      const double aVCyl = aVecToCircle.Dot(gp_Vec(myCylAxis));

      const gp_Pnt aPOnCylAxis(myCylCenter.X() + aVCyl * myCylAxis.X(),
                               myCylCenter.Y() + aVCyl * myCylAxis.Y(),
                               myCylCenter.Z() + aVCyl * myCylAxis.Z());

      // Vector from cylinder axis to circle center
      gp_Vec aDirToCircle(aPOnCylAxis, aCircleCenter);

      // For the generating circle, find extrema to cylinder
      // The circle is in a plane with normal = aRadialDir
      // Extrema occur where the line from cylinder axis to circle center
      // intersects the circle, considering the minor radius

      // Sample V_torus (angle on generating circle)
      for (int j = 0; j < 36; ++j)
      {
        const double aVTorus = 2.0 * M_PI * j / 36;

        const gp_Pnt aPTorus = Value2(aUTorus, aVTorus);

        // Project onto cylinder axis
        const gp_Vec aVecFromCylCenter(myCylCenter, aPTorus);
        const double aVCylPt = aVecFromCylCenter.Dot(gp_Vec(myCylAxis));

        const gp_Pnt aPOnCylAxisPt(myCylCenter.X() + aVCylPt * myCylAxis.X(),
                                   myCylCenter.Y() + aVCylPt * myCylAxis.Y(),
                                   myCylCenter.Z() + aVCylPt * myCylAxis.Z());

        // Direction from axis to torus point
        gp_Vec aDirToTorusPt(aPOnCylAxisPt, aPTorus);
        aDirToTorusPt = aDirToTorusPt - gp_Vec(myCylAxis) * aDirToTorusPt.Dot(gp_Vec(myCylAxis));
        const double aDistToAxis = aDirToTorusPt.Magnitude();

        if (aDistToAxis < theTol)
          continue;

        aDirToTorusPt.Divide(aDistToAxis);

        // U_cyl for minimum (pointing toward torus point)
        const double aUCylX   = aDirToTorusPt.Dot(gp_Vec(myCylXDir));
        const double aUCylY   = aDirToTorusPt.Dot(gp_Vec(myCylYDir));
        const double aUCylMin = std::atan2(aUCylY, aUCylX);
        const double aUCylMax = aUCylMin + M_PI;

        const gp_Pnt aPCylMin = Value1(aUCylMin, aVCylPt);
        const gp_Pnt aPCylMax = Value1(aUCylMax, aVCylPt);

        const double aSqDistMin = aPCylMin.SquareDistance(aPTorus);
        const double aSqDistMax = aPCylMax.SquareDistance(aPTorus);

        if (aSqDistMin < aBestMinSqDist)
        {
          aBestMinSqDist = aSqDistMin;
          aBestMinU1     = aUCylMin;
          aBestMinV1     = aVCylPt;
          aBestMinU2     = aUTorus;
          aBestMinV2     = aVTorus;
        }

        if (aSqDistMax > aBestMaxSqDist)
        {
          aBestMaxSqDist = aSqDistMax;
          aBestMaxU1     = aUCylMax;
          aBestMaxV1     = aVCylPt;
          aBestMaxU2     = aUTorus;
          aBestMaxV2     = aVTorus;
        }
      }
    }

    // Refine and add extrema
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      refineExtremum(aBestMinU1, aBestMinV1, aBestMinU2, aBestMinV2, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      refineExtremum(aBestMaxU1, aBestMaxV1, aBestMaxU2, aBestMaxV2, false, theTol);
    }
  }

  //! Refine an extremum using Powell's method (gradient-free optimization).
  void refineExtremum(double theU1,
                      double theV1,
                      double theU2,
                      double theV2,
                      bool   theIsMin,
                      double theTol) const
  {
    struct DistanceFunc
    {
      const ExtremaSS_CylinderTorus* myEval;
      bool                           myIsMin;

      DistanceFunc(const ExtremaSS_CylinderTorus* theEval, bool theIsMin)
          : myEval(theEval),
            myIsMin(theIsMin)
      {
      }

      bool Value(const math_Vector& theX, double& theF) const
      {
        const gp_Pnt aP1     = myEval->Value1(theX(1), theX(2));
        const gp_Pnt aP2     = myEval->Value2(theX(3), theX(4));
        const double aSqDist = aP1.SquareDistance(aP2);
        theF                 = myIsMin ? aSqDist : -aSqDist;
        return true;
      }
    };

    math_Vector aStartPt(1, 4);
    aStartPt(1) = theU1;
    aStartPt(2) = theV1;
    aStartPt(3) = theU2;
    aStartPt(4) = theV2;

    MathUtils::Config       aConfig(theTol, 50);
    DistanceFunc            aFunc(this, theIsMin);
    MathUtils::VectorResult aOptResult = MathOpt::Powell(aFunc, aStartPt, aConfig);

    double aU1 = theU1, aV1 = theV1, aU2 = theU2, aV2 = theV2;

    if (aOptResult.IsDone() && aOptResult.Solution.has_value())
    {
      const math_Vector& aSol = *aOptResult.Solution;
      aU1                     = aSol(1);
      aV1                     = aSol(2);
      aU2                     = aSol(3);
      aV2                     = aSol(4);
    }

    const gp_Pnt aP1     = Value1(aU1, aV1);
    const gp_Pnt aP2     = Value2(aU2, aV2);
    const double aSqDist = aP1.SquareDistance(aP2);

    addExtremum(aU1, aV1, aU2, aV2, aSqDist, theIsMin, theTol);
  }

  //! Check boundary extrema for bounded domains.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
      return;

    const ExtremaSS::Domain4D& aDom  = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    constexpr int aNbSamples = 20;

    // Check cylinder boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkPointAgainstTorus(aU1, aDom1.VMin, theTol, theMode);
      checkPointAgainstTorus(aU1, aDom1.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkPointAgainstTorus(aDom1.UMin, aV1, theTol, theMode);
      checkPointAgainstTorus(aDom1.UMax, aV1, theTol, theMode);
    }

    // Check torus boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkPointAgainstCylinder(aU2, aDom2.VMin, theTol, theMode);
      checkPointAgainstCylinder(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkPointAgainstCylinder(aDom2.UMin, aV2, theTol, theMode);
      checkPointAgainstCylinder(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on cylinder against torus.
  void checkPointAgainstTorus(double                theU1,
                              double                theV1,
                              double                theTol,
                              ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Find closest/farthest point on torus to aP1
    // This requires searching over U_torus and V_torus

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU2 = 0, aBestMaxV2 = 0;

    constexpr int aNbSamples = 36;
    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aU2 = 2.0 * M_PI * i / aNbSamples;
      for (int j = 0; j < aNbSamples; ++j)
      {
        const double aV2     = 2.0 * M_PI * j / aNbSamples;
        const gp_Pnt aP2     = Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSqDist < aBestMinSqDist)
        {
          aBestMinSqDist = aSqDist;
          aBestMinU2     = aU2;
          aBestMinV2     = aV2;
        }
        if (aSqDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aSqDist;
          aBestMaxU2     = aU2;
          aBestMaxV2     = aV2;
        }
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(theU1, theV1, aBestMinU2, aBestMinV2, aBestMinSqDist, true, theTol);
    }
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(theU1, theV1, aBestMaxU2, aBestMaxV2, aBestMaxSqDist, false, theTol);
    }
  }

  //! Check a point on torus against cylinder.
  void checkPointAgainstCylinder(double                theU2,
                                 double                theV2,
                                 double                theTol,
                                 ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    // Project onto cylinder axis
    const gp_Vec aVecFromCenter(myCylCenter, aP2);
    const double aVCyl = aVecFromCenter.Dot(gp_Vec(myCylAxis));

    double aVCylClamped = aVCyl;
    if (myDomain.has_value())
    {
      aVCylClamped = std::clamp(aVCyl, myDomain->Domain1.VMin, myDomain->Domain1.VMax);
    }

    const gp_Pnt aPOnAxis(myCylCenter.X() + aVCylClamped * myCylAxis.X(),
                          myCylCenter.Y() + aVCylClamped * myCylAxis.Y(),
                          myCylCenter.Z() + aVCylClamped * myCylAxis.Z());

    gp_Vec aDirToP2(aPOnAxis, aP2);
    aDirToP2                 = aDirToP2 - gp_Vec(myCylAxis) * aDirToP2.Dot(gp_Vec(myCylAxis));
    const double aDistToAxis = aDirToP2.Magnitude();

    if (aDistToAxis < theTol)
      return;

    aDirToP2.Divide(aDistToAxis);

    const double aX    = aDirToP2.Dot(gp_Vec(myCylXDir));
    const double aY    = aDirToP2.Dot(gp_Vec(myCylYDir));
    const double aUMin = std::atan2(aY, aX);
    const double aUMax = aUMin + M_PI;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      const gp_Pnt aP1     = Value1(aUMin, aVCylClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aUMin, aVCylClamped, theU2, theV2, aSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      const gp_Pnt aP1     = Value1(aUMax, aVCylClamped);
      const double aSqDist = aP1.SquareDistance(aP2);
      addExtremum(aUMax, aVCylClamped, theU2, theV2, aSqDist, false, theTol);
    }
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
    const double aU1 = ExtremaSS::NormalizeAngle(theU1);
    const double aU2 = ExtremaSS::NormalizeAngle(theU2);
    const double aV2 = ExtremaSS::NormalizeAngle(theV2);

    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      if (std::abs(anExisting.SquareDistance - theSqDist) < aTolSq)
      {
        const double aDU1 = std::abs(anExisting.U1 - aU1);
        const double aDU2 = std::abs(anExisting.U2 - aU2);
        const double aDV1 = std::abs(anExisting.V1 - theV1);
        const double aDV2 = std::abs(anExisting.V2 - aV2);

        if ((aDU1 < theTol || std::abs(aDU1 - 2 * M_PI) < theTol)
            && (aDU2 < theTol || std::abs(aDU2 - 2 * M_PI) < theTol) && aDV1 < theTol
            && (aDV2 < theTol || std::abs(aDV2 - 2 * M_PI) < theTol))
        {
          return;
        }
      }
    }

    ExtremaSS::ExtremumResult aResult;

    if (mySwapped)
    {
      aResult.U1     = aU2;
      aResult.V1     = aV2;
      aResult.U2     = aU1;
      aResult.V2     = theV1;
      aResult.Point1 = Value2(aU2, aV2);
      aResult.Point2 = Value1(aU1, theV1);
    }
    else
    {
      aResult.U1     = aU1;
      aResult.V1     = theV1;
      aResult.U2     = aU2;
      aResult.V2     = aV2;
      aResult.Point1 = Value1(aU1, theV1);
      aResult.Point2 = Value2(aU2, aV2);
    }

    aResult.SquareDistance = theSqDist;
    aResult.IsMinimum      = theIsMin;

    myResult.Extrema.Append(aResult);

    if (myResult.Status != ExtremaSS::Status::InfiniteSolutions)
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
  }

private:
  gp_Cylinder myCylinder; //!< The cylinder
  gp_Torus    myTorus;    //!< The torus

  std::optional<ExtremaSS::Domain4D> myDomain;  //!< Optional bounded domain
  bool                               mySwapped; //!< True if torus was provided first

  // Cached cylinder geometry
  gp_Pnt myCylCenter; //!< Cylinder center
  gp_Dir myCylAxis;   //!< Cylinder axis
  gp_Dir myCylXDir;   //!< Cylinder X direction
  gp_Dir myCylYDir;   //!< Cylinder Y direction
  double myCylRadius; //!< Cylinder radius

  // Cached torus geometry
  gp_Pnt myTorusCenter; //!< Torus center
  gp_Dir myTorusAxis;   //!< Torus axis
  gp_Dir myTorusXDir;   //!< Torus X direction
  gp_Dir myTorusYDir;   //!< Torus Y direction
  double myMajorRadius; //!< Torus major radius
  double myMinorRadius; //!< Torus minor radius

  // Cached relationship
  gp_Dir myCrossProduct; //!< Cross product of axes
  gp_Vec myDeltaCenter;  //!< Vector from cylinder center to torus center
  bool   myAxesParallel; //!< True if axes are parallel

  mutable ExtremaSS::Result myResult; //!< Computation result
};

#endif // _ExtremaSS_CylinderTorus_HeaderFile
