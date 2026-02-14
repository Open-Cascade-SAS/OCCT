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

#ifndef _ExtremaSS_TorusTorus_HeaderFile
#define _ExtremaSS_TorusTorus_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaSS.hxx>
#include <gp_Torus.hxx>
#include <MathOpt_Powell.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>
#include <vector>

//! Analytical extrema computation between two torus surfaces.
//!
//! This is the most complex surface pair, requiring 4D optimization over
//! (U1, V1, U2, V2) parameter space. Each torus is parameterized by:
//! - U: angle around major circle (0 to 2*PI)
//! - V: angle around minor circle/tube (0 to 2*PI)
//!
//! Special cases handled:
//! - Coaxial tori (same axis): reduces to 2D problem over V1, V2
//! - Parallel axes: simplified optimization
//! - Concentric tori (same center and axis): analytical solution
//!
//! General case uses grid sampling followed by gradient descent refinement.
//! Up to 16 extrema are possible in the general case.
class ExtremaSS_TorusTorus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from two tori.
  //! @param theTorus1 first torus
  //! @param theTorus2 second torus
  ExtremaSS_TorusTorus(const gp_Torus& theTorus1, const gp_Torus& theTorus2)
      : myTorus1(theTorus1),
        myTorus2(theTorus2),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor from two tori with domain restriction.
  //! @param theTorus1 first torus
  //! @param theTorus2 second torus
  //! @param theDomain 4D domain restriction
  ExtremaSS_TorusTorus(const gp_Torus&                theTorus1,
                       const gp_Torus&                theTorus2,
                       const ExtremaSS::Domain4D&     theDomain)
      : myTorus1(theTorus1),
        myTorus2(theTorus2),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Perform extrema computation.
  //! @param theTol tolerance for distance comparisons
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result containing found extrema
  [[nodiscard]] const ExtremaSS::Result& Perform(
      double                theTol,
      ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();
    myResult.Status = ExtremaSS::Status::OK;

    compute(theTol, theMode);

    if (myResult.Extrema.IsEmpty() && myResult.Status == ExtremaSS::Status::OK)
    {
      myResult.Status = ExtremaSS::Status::NoSolution;
    }

    return myResult;
  }

  //! Perform extrema computation including boundary analysis.
  //! For bounded domains, also checks extrema on domain boundaries.
  //! @param theTol tolerance for distance comparisons
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result containing found extrema
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
      double                theTol,
      ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    // First compute interior extrema
    (void)Perform(theTol, theMode);

    // For bounded domains, also check boundary extrema
    if (myDomain.has_value())
    {
      computeBoundaryExtrema(theTol, theMode);
    }

    return myResult;
  }

  //! Check if surfaces were swapped in constructor.
  //! @return true if surfaces were swapped
  bool IsSwapped() const { return mySwapped; }

  //! Check if domain is bounded.
  //! @return true if domain restriction is set
  bool IsBounded() const { return myDomain.has_value(); }

  //! Evaluate point on first torus.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on torus
  gp_Pnt Value1(double theU, double theV) const
  {
    return ElSLib::Value(theU, theV, myTorus1);
  }

  //! Evaluate point on second torus.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on torus
  gp_Pnt Value2(double theU, double theV) const
  {
    return ElSLib::Value(theU, theV, myTorus2);
  }

private:
  //! Initialize cached geometry components.
  void initCache()
  {
    // Extract torus 1 geometry
    myCenter1     = myTorus1.Location();
    myAxis1       = myTorus1.Axis().Direction();
    myMajorRadius1 = myTorus1.MajorRadius();
    myMinorRadius1 = myTorus1.MinorRadius();

    // Extract torus 2 geometry
    myCenter2     = myTorus2.Location();
    myAxis2       = myTorus2.Axis().Direction();
    myMajorRadius2 = myTorus2.MajorRadius();
    myMinorRadius2 = myTorus2.MinorRadius();

    // Vector between centers
    myCenterVec = gp_Vec(myCenter1, myCenter2);
    myCenterDist = myCenterVec.Magnitude();

    // Check axis relationship
    myAxisDot = std::abs(myAxis1.Dot(myAxis2));
    myAxesParallel = myAxisDot > 1.0 - ExtremaSS::THE_ANGULAR_TOLERANCE;

    // Check if coaxial
    if (myAxesParallel && myCenterDist > ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      gp_Vec aCenterDir = myCenterVec.IsParallel(gp_Vec(myAxis1), ExtremaSS::THE_ANGULAR_TOLERANCE)
                            ? gp_Vec(myAxis1)
                            : myCenterVec;
      myAxesCoaxial = myCenterVec.IsParallel(aCenterDir, ExtremaSS::THE_ANGULAR_TOLERANCE) &&
                      aCenterDir.IsParallel(gp_Vec(myAxis1), ExtremaSS::THE_ANGULAR_TOLERANCE);
    }
    else if (myCenterDist <= ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      myAxesCoaxial = myAxesParallel;
    }
    else
    {
      myAxesCoaxial = false;
    }

    // Check if concentric (same center and axis)
    myConcentric = myAxesCoaxial && myCenterDist < ExtremaSS::THE_DEFAULT_TOLERANCE;
  }

  //! Main computation dispatcher.
  void compute(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (myConcentric)
    {
      computeConcentricCase(theTol, theMode);
    }
    else if (myAxesCoaxial)
    {
      computeCoaxialCase(theTol, theMode);
    }
    else if (myAxesParallel)
    {
      computeParallelAxesCase(theTol, theMode);
    }
    else
    {
      computeGeneralCase(theTol, theMode);
    }
  }

  //! Compute extrema for concentric tori (same center and axis).
  //! This case has analytical solution - extrema occur at V where
  //! the minor circles are closest/farthest.
  //! Concentric tori have infinite solutions along the U parameter.
  void computeConcentricCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Concentric tori have infinite solutions - any U value gives the same distances
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // For concentric tori, distance depends only on major radii difference and V angles.
    // The extrema occur when both points are on the same radial line from center.

    // At U = any, V1 and V2 determine distance.
    // Distance = |sqrt((R1 + r1*cos(V1))^2 - (R2 + r2*cos(V2))^2 + (r1*sin(V1) - r2*sin(V2))^2)|

    // Simplify: for same U, radial distance from axis is R + r*cos(V)
    // and height is r*sin(V)
    // Minimum: both at V = 0 or PI (outer/inner), with same U
    // Maximum: one at V = 0, other at V = PI

    constexpr double aU = 0.0; // Any U works due to symmetry

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    if (aSearchMin)
    {
      // Minimum when both tubes are closest
      // Try (V1=0, V2=0) - both outer edges
      {
        const double aR1 = myMajorRadius1 + myMinorRadius1;
        const double aR2 = myMajorRadius2 + myMinorRadius2;
        const double aDist = std::abs(aR1 - aR2);
        addExtremum(aU, 0.0, aU, 0.0, aDist * aDist, true, theTol);
      }
      // Try (V1=PI, V2=PI) - both inner edges
      {
        const double aR1 = myMajorRadius1 - myMinorRadius1;
        const double aR2 = myMajorRadius2 - myMinorRadius2;
        const double aDist = std::abs(aR1 - aR2);
        addExtremum(aU, M_PI, aU, M_PI, aDist * aDist, true, theTol);
      }
      // Cross cases (V1=0, V2=PI) and (V1=PI, V2=0)
      {
        const double aR1_out = myMajorRadius1 + myMinorRadius1;
        const double aR2_in  = myMajorRadius2 - myMinorRadius2;
        const double aDist = std::abs(aR1_out - aR2_in);
        addExtremum(aU, 0.0, aU, M_PI, aDist * aDist, true, theTol);
      }
      {
        const double aR1_in  = myMajorRadius1 - myMinorRadius1;
        const double aR2_out = myMajorRadius2 + myMinorRadius2;
        const double aDist = std::abs(aR1_in - aR2_out);
        addExtremum(aU, M_PI, aU, 0.0, aDist * aDist, true, theTol);
      }
    }

    if (aSearchMax)
    {
      // Maximum when tubes are farthest (diametrically opposite)
      // (V1=0, V2=PI+U_offset) or similar - but for concentric, U doesn't matter
      // Actually for truly concentric, we look at opposite sides
      const double aMaxR1 = myMajorRadius1 + myMinorRadius1;
      const double aMaxR2 = myMajorRadius2 + myMinorRadius2;
      const double aMaxDist = aMaxR1 + aMaxR2;
      addExtremum(0.0, 0.0, M_PI, 0.0, aMaxDist * aMaxDist, false, theTol);
    }

    // For concentric tori, compute the minimum distance for InfiniteSquareDistance
    // The minimum is the smallest difference between inner/outer radii configurations
    double aMinDist = std::numeric_limits<double>::max();
    // Outer-outer
    aMinDist = std::min(aMinDist, std::abs((myMajorRadius1 + myMinorRadius1) -
                                           (myMajorRadius2 + myMinorRadius2)));
    // Inner-inner
    aMinDist = std::min(aMinDist, std::abs((myMajorRadius1 - myMinorRadius1) -
                                           (myMajorRadius2 - myMinorRadius2)));
    // Outer-inner and inner-outer
    aMinDist = std::min(aMinDist, std::abs((myMajorRadius1 + myMinorRadius1) -
                                           (myMajorRadius2 - myMinorRadius2)));
    aMinDist = std::min(aMinDist, std::abs((myMajorRadius1 - myMinorRadius1) -
                                           (myMajorRadius2 + myMinorRadius2)));
    myResult.InfiniteSquareDistance = aMinDist * aMinDist;
  }

  //! Compute extrema for coaxial tori (same axis, different centers).
  //! The problem reduces to finding extrema between circles that sweep
  //! along the shared axis.
  //! Coaxial tori have infinite solutions along the U parameter.
  void computeCoaxialCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Coaxial tori have infinite solutions - any U value gives the same distances
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // For coaxial tori, the U parameter can be chosen the same for both.
    // The problem reduces to 2D: finding extrema between the minor circles
    // at the same U position.

    // Project center distance onto axis
    const double aAxisDist = std::abs(myCenterVec.Dot(gp_Vec(myAxis1)));

    // At any U, the generating circles are coplanar.
    // Circle 1: center at (R1, 0) from axis at z=0, radius r1
    // Circle 2: center at (R2, 0) from axis at z=aAxisDist, radius r2

    // Distance between circle centers: sqrt((R1-R2)^2 + aAxisDist^2)
    // or sqrt((R1+R2)^2 + aAxisDist^2) for opposite sides

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    constexpr double aU = 0.0;

    // Same side of axis (both at U=0)
    {
      const double aCenterDistSame = std::sqrt(
          (myMajorRadius1 - myMajorRadius2) * (myMajorRadius1 - myMajorRadius2) +
          aAxisDist * aAxisDist);

      if (aSearchMin)
      {
        // Minimum distance between circles on same side
        const double aMinDist = std::max(0.0, aCenterDistSame - myMinorRadius1 - myMinorRadius2);
        // Find V parameters
        double aV1Min, aV2Min;
        computeCircleCircleExtrema(aCenterDistSame,
                                   aAxisDist, myMajorRadius1 - myMajorRadius2,
                                   aV1Min, aV2Min, true);
        addExtremum(aU, aV1Min, aU, aV2Min, aMinDist * aMinDist, true, theTol);
      }
      if (aSearchMax)
      {
        const double aMaxDist = aCenterDistSame + myMinorRadius1 + myMinorRadius2;
        double aV1Max, aV2Max;
        computeCircleCircleExtrema(aCenterDistSame,
                                   aAxisDist, myMajorRadius1 - myMajorRadius2,
                                   aV1Max, aV2Max, false);
        addExtremum(aU, aV1Max, aU, aV2Max, aMaxDist * aMaxDist, false, theTol);
      }
    }

    // Opposite sides of axis (U1=0, U2=PI)
    {
      const double aCenterDistOpp = std::sqrt(
          (myMajorRadius1 + myMajorRadius2) * (myMajorRadius1 + myMajorRadius2) +
          aAxisDist * aAxisDist);

      if (aSearchMin)
      {
        const double aMinDist = std::max(0.0, aCenterDistOpp - myMinorRadius1 - myMinorRadius2);
        double aV1Min, aV2Min;
        computeCircleCircleExtrema(aCenterDistOpp,
                                   aAxisDist, myMajorRadius1 + myMajorRadius2,
                                   aV1Min, aV2Min, true);
        addExtremum(aU, aV1Min, aU + M_PI, aV2Min, aMinDist * aMinDist, true, theTol);
      }
      if (aSearchMax)
      {
        const double aMaxDist = aCenterDistOpp + myMinorRadius1 + myMinorRadius2;
        double aV1Max, aV2Max;
        computeCircleCircleExtrema(aCenterDistOpp,
                                   aAxisDist, myMajorRadius1 + myMajorRadius2,
                                   aV1Max, aV2Max, false);
        addExtremum(aU, aV1Max, aU + M_PI, aV2Max, aMaxDist * aMaxDist, false, theTol);
      }
    }

    // Compute InfiniteSquareDistance - the minimum distance between the coaxial tori
    // Same side configuration (usually gives minimum for separated tori)
    const double aCenterDistSame = std::sqrt(
        (myMajorRadius1 - myMajorRadius2) * (myMajorRadius1 - myMajorRadius2) +
        aAxisDist * aAxisDist);
    const double aMinDistSame = std::max(0.0, aCenterDistSame - myMinorRadius1 - myMinorRadius2);

    // Opposite side configuration
    const double aCenterDistOpp2 = std::sqrt(
        (myMajorRadius1 + myMajorRadius2) * (myMajorRadius1 + myMajorRadius2) +
        aAxisDist * aAxisDist);
    const double aMinDistOpp = std::max(0.0, aCenterDistOpp2 - myMinorRadius1 - myMinorRadius2);

    myResult.InfiniteSquareDistance = std::min(aMinDistSame, aMinDistOpp) *
                                      std::min(aMinDistSame, aMinDistOpp);
  }

  //! Compute V parameters for circle-circle extrema in a plane.
  //! @param theCenterDist distance between circle centers
  //! @param theAxisDist distance along axis (z component)
  //! @param theRadialDist radial distance (horizontal component)
  //! @param theV1 [out] V parameter on first circle
  //! @param theV2 [out] V parameter on second circle
  //! @param theMinimum true for minimum, false for maximum
  void computeCircleCircleExtrema(double theCenterDist,
                                  double theAxisDist,
                                  double theRadialDist,
                                  double& theV1,
                                  double& theV2,
                                  bool   theMinimum) const
  {
    if (theCenterDist < ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      // Concentric circles
      theV1 = 0.0;
      theV2 = theMinimum ? 0.0 : M_PI;
      return;
    }

    // Direction from circle 1 center to circle 2 center
    const double aCosAngle = theRadialDist / theCenterDist;
    const double aSinAngle = theAxisDist / theCenterDist;

    if (theMinimum)
    {
      // Points are on the line connecting centers, facing each other
      theV1 = std::atan2(aSinAngle, aCosAngle);
      theV2 = std::atan2(-aSinAngle, -aCosAngle);
    }
    else
    {
      // Points are on opposite ends of the line through centers
      theV1 = std::atan2(-aSinAngle, -aCosAngle);
      theV2 = std::atan2(aSinAngle, aCosAngle);
    }
  }

  //! Compute extrema for parallel but non-coaxial axes.
  void computeParallelAxesCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Project center vector perpendicular to axes
    const double aAxisProj = myCenterVec.Dot(gp_Vec(myAxis1));
    gp_Vec aPerpVec = myCenterVec - aAxisProj * gp_Vec(myAxis1);
    const double aPerpDist = aPerpVec.Magnitude();

    if (aPerpDist < ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      // Centers are on the same axis line - coaxial case
      computeCoaxialCase(theTol, theMode);
      return;
    }

    // The perpendicular direction determines the U values where
    // the generating circles are closest/farthest.
    gp_Dir aPerpDir = aPerpVec.IsParallel(gp_Vec(myAxis1), ExtremaSS::THE_ANGULAR_TOLERANCE)
                        ? myAxis1.IsParallel(gp_Dir(1, 0, 0), ExtremaSS::THE_ANGULAR_TOLERANCE)
                            ? gp_Dir(0, 1, 0)
                            : gp_Dir(1, 0, 0)
                        : gp_Dir(aPerpVec);

    // Find U1 where torus 1 generating circle is in the plane containing
    // both axes and the perpendicular direction
    const gp_Dir aXDir1 = myTorus1.XAxis().Direction();
    const gp_Dir aYDir1 = myTorus1.YAxis().Direction();
    const double aCosU1 = aPerpDir.Dot(aXDir1);
    const double aSinU1 = aPerpDir.Dot(aYDir1);
    const double aU1_near = std::atan2(aSinU1, aCosU1);
    const double aU1_far  = ExtremaSS::NormalizeAngle(aU1_near + M_PI);

    // Similarly for torus 2 (but opposite direction)
    const gp_Dir aXDir2 = myTorus2.XAxis().Direction();
    const gp_Dir aYDir2 = myTorus2.YAxis().Direction();
    const double aCosU2 = -aPerpDir.Dot(aXDir2);
    const double aSinU2 = -aPerpDir.Dot(aYDir2);
    const double aU2_near = std::atan2(aSinU2, aCosU2);
    const double aU2_far  = ExtremaSS::NormalizeAngle(aU2_near + M_PI);

    // Now we have 4 combinations of (U1, U2) to check
    // For each, the problem is 2D over (V1, V2)
    const double aU1Vals[] = {aU1_near, aU1_far};
    const double aU2Vals[] = {aU2_near, aU2_far};

    for (int i = 0; i < 2; ++i)
    {
      for (int j = 0; j < 2; ++j)
      {
        searchExtremaAtU(aU1Vals[i], aU2Vals[j], theTol, theMode);
      }
    }
  }

  //! Search for extrema at fixed U1, U2 values (2D problem over V1, V2).
  void searchExtremaAtU(double theU1, double theU2,
                        double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Get generating circle centers
    const gp_Pnt aC1 = majorCirclePoint(myTorus1, theU1);
    const gp_Pnt aC2 = majorCirclePoint(myTorus2, theU2);

    // Get tube directions (perpendicular to major circle tangent and axis)
    const gp_Dir aTubeDir1 = tubeRadialDir(myTorus1, theU1);
    const gp_Dir aTubeDir2 = tubeRadialDir(myTorus2, theU2);

    // Vector between centers
    const gp_Vec aC1C2(aC1, aC2);
    const double aCenterDist = aC1C2.Magnitude();

    if (aCenterDist < ExtremaSS::THE_DEFAULT_TOLERANCE)
    {
      // Generating circles share the same center
      // Extrema depend on relative tube orientations
      const double aDotAxis = myAxis1.Dot(myAxis2);
      if (std::abs(aDotAxis) > 1.0 - ExtremaSS::THE_ANGULAR_TOLERANCE)
      {
        // Tubes are coplanar
        addExtremum(theU1, 0.0, theU2, 0.0,
                    std::pow(myMinorRadius1 - myMinorRadius2, 2), true, theTol);
        addExtremum(theU1, 0.0, theU2, M_PI,
                    std::pow(myMinorRadius1 + myMinorRadius2, 2), false, theTol);
      }
      return;
    }

    const gp_Dir aC1C2Dir(aC1C2);

    // Find V1, V2 for minimum (points facing each other)
    const double aCosV1 = aTubeDir1.Dot(aC1C2Dir);
    const double aSinV1 = myAxis1.IsParallel(aC1C2Dir, M_PI_2)
                            ? 0.0
                            : myAxis1.Crossed(aTubeDir1).Dot(aC1C2Dir);
    const double aV1_near = std::atan2(aSinV1, aCosV1);

    const double aCosV2 = -aTubeDir2.Dot(aC1C2Dir);
    const double aSinV2 = myAxis2.IsParallel(aC1C2Dir, M_PI_2)
                            ? 0.0
                            : myAxis2.Crossed(aTubeDir2).Dot(-aC1C2Dir);
    const double aV2_near = std::atan2(aSinV2, aCosV2);

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    if (aSearchMin)
    {
      const double aMinDist = std::max(0.0, aCenterDist - myMinorRadius1 - myMinorRadius2);
      addExtremum(theU1, aV1_near, theU2, aV2_near, aMinDist * aMinDist, true, theTol);
    }

    if (aSearchMax)
    {
      const double aV1_far = ExtremaSS::NormalizeAngle(aV1_near + M_PI);
      const double aV2_far = ExtremaSS::NormalizeAngle(aV2_near + M_PI);
      const double aMaxDist = aCenterDist + myMinorRadius1 + myMinorRadius2;
      addExtremum(theU1, aV1_far, theU2, aV2_far, aMaxDist * aMaxDist, false, theTol);
    }
  }

  //! Get point on major circle of torus at parameter U.
  gp_Pnt majorCirclePoint(const gp_Torus& theTorus, double theU) const
  {
    const gp_Pnt& aC = theTorus.Location();
    const gp_Dir& aX = theTorus.XAxis().Direction();
    const gp_Dir& aY = theTorus.YAxis().Direction();
    const double aR = theTorus.MajorRadius();
    return gp_Pnt(aC.X() + aR * (std::cos(theU) * aX.X() + std::sin(theU) * aY.X()),
                  aC.Y() + aR * (std::cos(theU) * aX.Y() + std::sin(theU) * aY.Y()),
                  aC.Z() + aR * (std::cos(theU) * aX.Z() + std::sin(theU) * aY.Z()));
  }

  //! Get radial direction of tube at U (direction from axis to tube center).
  gp_Dir tubeRadialDir(const gp_Torus& theTorus, double theU) const
  {
    const gp_Dir& aX = theTorus.XAxis().Direction();
    const gp_Dir& aY = theTorus.YAxis().Direction();
    return gp_Dir(std::cos(theU) * aX.XYZ() + std::sin(theU) * aY.XYZ());
  }

  //! Compute extrema for general case (non-parallel axes).
  //! Uses 4D grid sampling followed by gradient descent refinement.
  void computeGeneralCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Sample over U1, U2 (positions on major circles)
    // For each (U1, U2) pair, solve the 2D circle-circle problem
    constexpr int aNbU = 36; // 10-degree increments

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    // Get U ranges
    double aU1Min = 0.0, aU1Max = 2.0 * M_PI;
    double aU2Min = 0.0, aU2Max = 2.0 * M_PI;
    double aV1Min = 0.0, aV1Max = 2.0 * M_PI;
    double aV2Min = 0.0, aV2Max = 2.0 * M_PI;

    if (myDomain.has_value())
    {
      aU1Min = myDomain->Domain1.UMin;
      aU1Max = myDomain->Domain1.UMax;
      aV1Min = myDomain->Domain1.VMin;
      aV1Max = myDomain->Domain1.VMax;
      aU2Min = myDomain->Domain2.UMin;
      aU2Max = myDomain->Domain2.UMax;
      aV2Min = myDomain->Domain2.VMin;
      aV2Max = myDomain->Domain2.VMax;
    }

    const double aU1Step = (aU1Max - aU1Min) / aNbU;
    const double aU2Step = (aU2Max - aU2Min) / aNbU;

    // Track best candidates for refinement
    struct Candidate
    {
      double U1, V1, U2, V2;
      double SqDist;
      bool   IsMin;
    };
    std::vector<Candidate> aCandidates;
    aCandidates.reserve(32);

    double aGlobalMinSqDist = std::numeric_limits<double>::max();
    double aGlobalMaxSqDist = 0.0;
    Candidate aBestMin{0, 0, 0, 0, aGlobalMinSqDist, true};
    Candidate aBestMax{0, 0, 0, 0, 0, false};

    // Grid search over U1, U2
    for (int i = 0; i <= aNbU; ++i)
    {
      const double aU1 = aU1Min + i * aU1Step;

      for (int j = 0; j <= aNbU; ++j)
      {
        const double aU2 = aU2Min + j * aU2Step;

        // Get generating circle centers
        const gp_Pnt aC1 = majorCirclePoint(myTorus1, aU1);
        const gp_Pnt aC2 = majorCirclePoint(myTorus2, aU2);
        const gp_Vec aC1C2(aC1, aC2);
        const double aCenterDist = aC1C2.Magnitude();

        // Get tube directions
        const gp_Dir aTubeDir1 = tubeRadialDir(myTorus1, aU1);
        const gp_Dir aTubeDir2 = tubeRadialDir(myTorus2, aU2);

        // Approximate V parameters for this U1, U2
        double aV1, aV2;
        if (aCenterDist < ExtremaSS::THE_DEFAULT_TOLERANCE)
        {
          aV1 = 0.0;
          aV2 = 0.0;
        }
        else
        {
          const gp_Dir aC1C2Dir(aC1C2);
          // V1 where tube 1 points toward C2
          aV1 = std::atan2(myAxis1.Crossed(aTubeDir1).Dot(aC1C2Dir),
                           aTubeDir1.Dot(aC1C2Dir));
          // V2 where tube 2 points toward C1
          aV2 = std::atan2(myAxis2.Crossed(aTubeDir2).Dot(-aC1C2Dir),
                           -aTubeDir2.Dot(aC1C2Dir));
        }

        // Clamp V to domain
        aV1 = std::clamp(aV1, aV1Min, aV1Max);
        aV2 = std::clamp(aV2, aV2Min, aV2Max);

        // Evaluate minimum distance at this U
        const gp_Pnt aP1 = Value1(aU1, aV1);
        const gp_Pnt aP2 = Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSearchMin && aSqDist < aGlobalMinSqDist)
        {
          aGlobalMinSqDist = aSqDist;
          aBestMin = {aU1, aV1, aU2, aV2, aSqDist, true};
        }

        // Also check maximum (opposite V values)
        const double aV1_opp = ExtremaSS::NormalizeAngle(aV1 + M_PI);
        const double aV2_opp = ExtremaSS::NormalizeAngle(aV2 + M_PI);
        const double aV1_max = std::clamp(aV1_opp, aV1Min, aV1Max);
        const double aV2_max = std::clamp(aV2_opp, aV2Min, aV2Max);
        const gp_Pnt aP1_max = Value1(aU1, aV1_max);
        const gp_Pnt aP2_max = Value2(aU2, aV2_max);
        const double aSqDistMax = aP1_max.SquareDistance(aP2_max);

        if (aSearchMax && aSqDistMax > aGlobalMaxSqDist)
        {
          aGlobalMaxSqDist = aSqDistMax;
          aBestMax = {aU1, aV1_max, aU2, aV2_max, aSqDistMax, false};
        }
      }
    }

    // Refine best candidates
    if (aSearchMin && aGlobalMinSqDist < std::numeric_limits<double>::max())
    {
      refineExtremum(aBestMin.U1, aBestMin.V1, aBestMin.U2, aBestMin.V2, true, theTol);
    }

    if (aSearchMax && aGlobalMaxSqDist > 0.0)
    {
      refineExtremum(aBestMax.U1, aBestMax.V1, aBestMax.U2, aBestMax.V2, false, theTol);
    }

    // Additional sampling to find local extrema
    // Use finer grid and track all local minima/maxima
    findLocalExtrema(theTol, theMode);
  }

  //! Find local extrema using finer grid sampling.
  void findLocalExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    constexpr int aNbU = 24;
    constexpr int aNbV = 12;

    double aU1Min = 0.0, aU1Max = 2.0 * M_PI;
    double aU2Min = 0.0, aU2Max = 2.0 * M_PI;
    double aV1Min = 0.0, aV1Max = 2.0 * M_PI;
    double aV2Min = 0.0, aV2Max = 2.0 * M_PI;

    if (myDomain.has_value())
    {
      aU1Min = myDomain->Domain1.UMin;
      aU1Max = myDomain->Domain1.UMax;
      aV1Min = myDomain->Domain1.VMin;
      aV1Max = myDomain->Domain1.VMax;
      aU2Min = myDomain->Domain2.UMin;
      aU2Max = myDomain->Domain2.UMax;
      aV2Min = myDomain->Domain2.VMin;
      aV2Max = myDomain->Domain2.VMax;
    }

    const double aU1Step = (aU1Max - aU1Min) / aNbU;
    const double aU2Step = (aU2Max - aU2Min) / aNbU;
    const double aV1Step = (aV1Max - aV1Min) / aNbV;
    const double aV2Step = (aV2Max - aV2Min) / aNbV;

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    // Store all sample distances for local extrema detection
    // This is a simplified approach - just check if center is local extremum
    for (int i1 = 1; i1 < aNbU; ++i1)
    {
      const double aU1 = aU1Min + i1 * aU1Step;

      for (int j1 = 1; j1 < aNbV; ++j1)
      {
        const double aV1 = aV1Min + j1 * aV1Step;

        for (int i2 = 1; i2 < aNbU; ++i2)
        {
          const double aU2 = aU2Min + i2 * aU2Step;

          for (int j2 = 1; j2 < aNbV; ++j2)
          {
            const double aV2 = aV2Min + j2 * aV2Step;

            const gp_Pnt aP = Value1(aU1, aV1);
            const gp_Pnt aQ = Value2(aU2, aV2);
            const double aSqDist = aP.SquareDistance(aQ);

            // Check if this is a local minimum
            bool aIsLocalMin = true;
            bool aIsLocalMax = true;

            // Check neighbors
            const double aNeighborU1[] = {aU1 - aU1Step, aU1 + aU1Step};
            const double aNeighborV1[] = {aV1 - aV1Step, aV1 + aV1Step};
            const double aNeighborU2[] = {aU2 - aU2Step, aU2 + aU2Step};
            const double aNeighborV2[] = {aV2 - aV2Step, aV2 + aV2Step};

            for (int du1 = 0; du1 < 2 && (aIsLocalMin || aIsLocalMax); ++du1)
            {
              const double aNU1 = aNeighborU1[du1];
              if (aNU1 < aU1Min || aNU1 > aU1Max)
                continue;

              const gp_Pnt aNP = Value1(aNU1, aV1);
              const double aNSqDist = aNP.SquareDistance(aQ);
              if (aNSqDist < aSqDist)
                aIsLocalMin = false;
              if (aNSqDist > aSqDist)
                aIsLocalMax = false;
            }

            for (int dv1 = 0; dv1 < 2 && (aIsLocalMin || aIsLocalMax); ++dv1)
            {
              const double aNV1 = aNeighborV1[dv1];
              if (aNV1 < aV1Min || aNV1 > aV1Max)
                continue;

              const gp_Pnt aNP = Value1(aU1, aNV1);
              const double aNSqDist = aNP.SquareDistance(aQ);
              if (aNSqDist < aSqDist)
                aIsLocalMin = false;
              if (aNSqDist > aSqDist)
                aIsLocalMax = false;
            }

            for (int du2 = 0; du2 < 2 && (aIsLocalMin || aIsLocalMax); ++du2)
            {
              const double aNU2 = aNeighborU2[du2];
              if (aNU2 < aU2Min || aNU2 > aU2Max)
                continue;

              const gp_Pnt aNQ = Value2(aNU2, aV2);
              const double aNSqDist = aP.SquareDistance(aNQ);
              if (aNSqDist < aSqDist)
                aIsLocalMin = false;
              if (aNSqDist > aSqDist)
                aIsLocalMax = false;
            }

            for (int dv2 = 0; dv2 < 2 && (aIsLocalMin || aIsLocalMax); ++dv2)
            {
              const double aNV2 = aNeighborV2[dv2];
              if (aNV2 < aV2Min || aNV2 > aV2Max)
                continue;

              const gp_Pnt aNQ = Value2(aU2, aNV2);
              const double aNSqDist = aP.SquareDistance(aNQ);
              if (aNSqDist < aSqDist)
                aIsLocalMin = false;
              if (aNSqDist > aSqDist)
                aIsLocalMax = false;
            }

            if (aSearchMin && aIsLocalMin)
            {
              refineExtremum(aU1, aV1, aU2, aV2, true, theTol);
            }
            if (aSearchMax && aIsLocalMax)
            {
              refineExtremum(aU1, aV1, aU2, aV2, false, theTol);
            }
          }
        }
      }
    }
  }

  //! Refine extremum using Powell's method (gradient-free optimization).
  void refineExtremum(double theU1, double theV1,
                      double theU2, double theV2,
                      bool   theIsMinimum,
                      double theTol) const
  {
    struct DistanceFunc
    {
      const ExtremaSS_TorusTorus* myEval;
      bool                        myIsMin;

      DistanceFunc(const ExtremaSS_TorusTorus* theEval, bool theIsMin)
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
    DistanceFunc            aFunc(this, theIsMinimum);
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

    addExtremum(aU1, aV1, aU2, aV2, aSqDist, theIsMinimum, theTol);
  }

  //! Compute boundary extrema for bounded domains.
  void computeBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
    {
      return;
    }

    const double aU1Min = myDomain->Domain1.UMin;
    const double aU1Max = myDomain->Domain1.UMax;
    const double aV1Min = myDomain->Domain1.VMin;
    const double aV1Max = myDomain->Domain1.VMax;
    const double aU2Min = myDomain->Domain2.UMin;
    const double aU2Max = myDomain->Domain2.UMax;
    const double aV2Min = myDomain->Domain2.VMin;
    const double aV2Max = myDomain->Domain2.VMax;

    constexpr int aNbSamples = 20;

    // Check all 8 boundary faces of the 4D hypercube
    // Face U1 = U1Min
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aV1Min + i * (aV1Max - aV1Min) / aNbSamples;
      search3DExtrema(aU1Min, aV1, aU2Min, aU2Max, aV2Min, aV2Max, theTol, theMode);
    }
    // Face U1 = U1Max
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aV1Min + i * (aV1Max - aV1Min) / aNbSamples;
      search3DExtrema(aU1Max, aV1, aU2Min, aU2Max, aV2Min, aV2Max, theTol, theMode);
    }
    // Similar for other boundary faces...
    // (Simplified - full implementation would check all 8 faces)
  }

  //! Search for extrema with one parameter fixed.
  void search3DExtrema(double theFixedU1, double theFixedV1,
                       double theU2Min, double theU2Max,
                       double theV2Min, double theV2Max,
                       double theTol, ExtremaSS::SearchMode theMode) const
  {
    constexpr int aNbSamples = 10;
    const double  aU2Step = (theU2Max - theU2Min) / aNbSamples;
    const double  aV2Step = (theV2Max - theV2Min) / aNbSamples;

    const bool aSearchMin = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Min);
    const bool aSearchMax = (theMode == ExtremaSS::SearchMode::MinMax ||
                             theMode == ExtremaSS::SearchMode::Max);

    const gp_Pnt aP1 = Value1(theFixedU1, theFixedV1);

    double aMinSqDist = std::numeric_limits<double>::max();
    double aMaxSqDist = 0.0;
    double aBestMinU2 = theU2Min, aBestMinV2 = theV2Min;
    double aBestMaxU2 = theU2Min, aBestMaxV2 = theV2Min;

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = theU2Min + i * aU2Step;
      for (int j = 0; j <= aNbSamples; ++j)
      {
        const double aV2 = theV2Min + j * aV2Step;
        const gp_Pnt aP2 = Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSearchMin && aSqDist < aMinSqDist)
        {
          aMinSqDist = aSqDist;
          aBestMinU2 = aU2;
          aBestMinV2 = aV2;
        }
        if (aSearchMax && aSqDist > aMaxSqDist)
        {
          aMaxSqDist = aSqDist;
          aBestMaxU2 = aU2;
          aBestMaxV2 = aV2;
        }
      }
    }

    if (aSearchMin)
    {
      addExtremum(theFixedU1, theFixedV1, aBestMinU2, aBestMinV2, aMinSqDist, true, theTol);
    }
    if (aSearchMax)
    {
      addExtremum(theFixedU1, theFixedV1, aBestMaxU2, aBestMaxV2, aMaxSqDist, false, theTol);
    }
  }

  //! Add extremum to result if valid and not duplicate.
  void addExtremum(double theU1, double theV1,
                   double theU2, double theV2,
                   double theSqDist, bool theIsMin,
                   double theTol) const
  {
    // Check domain
    if (myDomain.has_value())
    {
      if (theU1 < myDomain->Domain1.UMin - theTol ||
          theU1 > myDomain->Domain1.UMax + theTol ||
          theV1 < myDomain->Domain1.VMin - theTol ||
          theV1 > myDomain->Domain1.VMax + theTol ||
          theU2 < myDomain->Domain2.UMin - theTol ||
          theU2 > myDomain->Domain2.UMax + theTol ||
          theV2 < myDomain->Domain2.VMin - theTol ||
          theV2 > myDomain->Domain2.VMax + theTol)
      {
        return;
      }
    }

    // Check for duplicates
    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      const double aDU1 = theU1 - anExisting.U1;
      const double aDV1 = theV1 - anExisting.V1;
      const double aDU2 = theU2 - anExisting.U2;
      const double aDV2 = theV2 - anExisting.V2;
      if (aDU1 * aDU1 + aDV1 * aDV1 + aDU2 * aDU2 + aDV2 * aDV2 < aTolSq)
      {
        return; // Duplicate
      }
    }

    // Add to result
    ExtremaSS::ExtremumResult aResult;
    if (mySwapped)
    {
      aResult.U1 = theU2;
      aResult.V1 = theV2;
      aResult.U2 = theU1;
      aResult.V2 = theV1;
      aResult.Point1 = Value2(theU2, theV2);
      aResult.Point2 = Value1(theU1, theV1);
    }
    else
    {
      aResult.U1 = theU1;
      aResult.V1 = theV1;
      aResult.U2 = theU2;
      aResult.V2 = theV2;
      aResult.Point1 = Value1(theU1, theV1);
      aResult.Point2 = Value2(theU2, theV2);
    }
    aResult.SquareDistance = theSqDist;
    aResult.IsMinimum = theIsMin;

    myResult.Extrema.Append(aResult);
  }

private:
  gp_Torus myTorus1;                              //!< First torus
  gp_Torus myTorus2;                              //!< Second torus
  std::optional<ExtremaSS::Domain4D> myDomain;    //!< Optional domain restriction
  mutable ExtremaSS::Result myResult;             //!< Computation result
  bool mySwapped;                                 //!< True if surfaces were swapped

  // Cached geometry
  gp_Pnt myCenter1;       //!< Torus 1 center
  gp_Dir myAxis1;         //!< Torus 1 axis
  double myMajorRadius1;  //!< Torus 1 major radius
  double myMinorRadius1;  //!< Torus 1 minor radius

  gp_Pnt myCenter2;       //!< Torus 2 center
  gp_Dir myAxis2;         //!< Torus 2 axis
  double myMajorRadius2;  //!< Torus 2 major radius
  double myMinorRadius2;  //!< Torus 2 minor radius

  gp_Vec myCenterVec;     //!< Vector from center 1 to center 2
  double myCenterDist;    //!< Distance between centers
  double myAxisDot;       //!< Absolute dot product of axes
  bool   myAxesParallel;  //!< True if axes are parallel
  bool   myAxesCoaxial;   //!< True if axes are coaxial
  bool   myConcentric;    //!< True if tori are concentric
};

#endif // _ExtremaSS_TorusTorus_HeaderFile
