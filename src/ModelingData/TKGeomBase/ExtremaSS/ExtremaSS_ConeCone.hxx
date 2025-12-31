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

#ifndef _ExtremaSS_ConeCone_HeaderFile
#define _ExtremaSS_ConeCone_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cone.hxx>
#include <MathSys_Newton4D.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <limits>
#include <optional>

//! Analytical computation of extrema between two cones.
//!
//! Both cones have varying radii along their V parameters, making this
//! a complex optimization problem that generally requires numerical search.
//!
//! **Case 1: Parallel axes**
//! The problem has some structure - for each pair of V1, V2 values,
//! we have a circle-circle problem that can be solved analytically.
//!
//! **Case 2: Coaxial cones**
//! Special case where extrema depend on the radii at various V values.
//!
//! **Case 3: Intersecting axes**
//! The axes meet at a point. Need to search over V1, V2.
//!
//! **Case 4: Skew axes**
//! Most general case requiring 2D optimization over V1, V2.
//!
//! **Parameterization:**
//! - Cone: P(U, V) = Apex + V*cos(α)*Axis + V*sin(α)*(cos(U)*XDir + sin(U)*YDir)
//!   where α is the semi-angle
class ExtremaSS_ConeCone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from two cones.
  //! @param theCone1 First cone
  //! @param theCone2 Second cone
  ExtremaSS_ConeCone(const gp_Cone& theCone1, const gp_Cone& theCone2)
      : myCone1(theCone1),
        myCone2(theCone2)
  {
    initCache();
  }

  //! Constructor with bounded domain.
  //! @param theCone1 First cone
  //! @param theCone2 Second cone
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_ConeCone(const gp_Cone&             theCone1,
                     const gp_Cone&             theCone2,
                     const ExtremaSS::Domain4D& theDomain)
      : myCone1(theCone1),
        myCone2(theCone2),
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

  //! Evaluate point on first cone using OCCT parameterization.
  //! OCCT cone: P(U,V) = Location + V*Direction + (RefRadius + V*tan(SemiAngle))*(cos(U)*XDir + sin(U)*YDir)
  //! @param theU U parameter (angle)
  //! @param theV V parameter (distance along axis from cone location)
  //! @return Point on cone 1
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius1 + theV * myTan1;
    return gp_Pnt(myLocation1.X() + theV * myAxis1.X()
                      + aRadius * (aCosU * myXDir1.X() + aSinU * myYDir1.X()),
                  myLocation1.Y() + theV * myAxis1.Y()
                      + aRadius * (aCosU * myXDir1.Y() + aSinU * myYDir1.Y()),
                  myLocation1.Z() + theV * myAxis1.Z()
                      + aRadius * (aCosU * myXDir1.Z() + aSinU * myYDir1.Z()));
  }

  //! Evaluate point on second cone using OCCT parameterization.
  //! @param theU U parameter (angle)
  //! @param theV V parameter (distance along axis from cone location)
  //! @return Point on cone 2
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius2 + theV * myTan2;
    return gp_Pnt(myLocation2.X() + theV * myAxis2.X()
                      + aRadius * (aCosU * myXDir2.X() + aSinU * myYDir2.X()),
                  myLocation2.Y() + theV * myAxis2.Y()
                      + aRadius * (aCosU * myXDir2.Y() + aSinU * myYDir2.Y()),
                  myLocation2.Z() + theV * myAxis2.Z()
                      + aRadius * (aCosU * myXDir2.Z() + aSinU * myYDir2.Z()));
  }

  //! Check if surfaces were swapped.
  //! @return Always false (symmetric case)
  bool IsSwapped() const { return false; }

  //! Check if domain is bounded.
  //! @return True if bounded domain was provided
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Initialize cached values from geometry.
  void initCache()
  {
    // Extract cone 1 data using OCCT parameterization
    const gp_Ax3& anAx1 = myCone1.Position();
    myLocation1         = anAx1.Location();
    myApex1             = myCone1.Apex();
    myAxis1             = anAx1.Direction();
    myXDir1             = anAx1.XDirection();
    myYDir1             = anAx1.YDirection();
    mySemiAngle1        = myCone1.SemiAngle();
    myRefRadius1        = myCone1.RefRadius();
    myTan1              = std::tan(mySemiAngle1);
    mySin1              = std::sin(mySemiAngle1);
    myCos1              = std::cos(mySemiAngle1);

    // Extract cone 2 data using OCCT parameterization
    const gp_Ax3& anAx2 = myCone2.Position();
    myLocation2         = anAx2.Location();
    myApex2             = myCone2.Apex();
    myAxis2             = anAx2.Direction();
    myXDir2             = anAx2.XDirection();
    myYDir2             = anAx2.YDirection();
    mySemiAngle2        = myCone2.SemiAngle();
    myRefRadius2        = myCone2.RefRadius();
    myTan2              = std::tan(mySemiAngle2);
    mySin2              = std::sin(mySemiAngle2);
    myCos2              = std::cos(mySemiAngle2);

    // Check if axes are parallel using gp_Vec to avoid gp_Dir exception for parallel vectors
    const gp_Vec aCrossVec = gp_Vec(myAxis1).Crossed(gp_Vec(myAxis2));
    const double aCrossMag = aCrossVec.Magnitude();
    myAxesParallel = (aCrossMag < ExtremaSS::THE_ANGULAR_TOLERANCE);

    if (!myAxesParallel)
    {
      myCrossProduct = gp_Dir(aCrossVec.X() / aCrossMag,
                              aCrossVec.Y() / aCrossMag,
                              aCrossVec.Z() / aCrossMag);
    }

    // Vector from location1 to location2 (for parallel case analysis)
    myDeltaLocation = gp_Vec(myLocation2.X() - myLocation1.X(),
                             myLocation2.Y() - myLocation1.Y(),
                             myLocation2.Z() - myLocation1.Z());

    // Vector from apex1 to apex2
    myDeltaApex = gp_Vec(myApex2.X() - myApex1.X(),
                         myApex2.Y() - myApex1.Y(),
                         myApex2.Z() - myApex1.Z());
  }

  //! Compute extrema for parallel axes case.
  void computeParallelCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    // Project location2 onto axis1
    const double aDotAxis = myDeltaLocation.Dot(gp_Vec(myAxis1));
    const gp_Vec aPerp(myDeltaLocation.X() - aDotAxis * myAxis1.X(),
                       myDeltaLocation.Y() - aDotAxis * myAxis1.Y(),
                       myDeltaLocation.Z() - aDotAxis * myAxis1.Z());

    const double aAxisDist = aPerp.Magnitude();

    // Check for coaxial case
    if (aAxisDist < theTol)
    {
      computeCoaxialCase(theTol, theMode);
      return;
    }

    myResult.Status = ExtremaSS::Status::OK;

    // Direction from axis1 toward axis2
    const gp_Dir aDirToAxis2(aPerp.X() / aAxisDist, aPerp.Y() / aAxisDist, aPerp.Z() / aAxisDist);
    const gp_Dir aDirFromAxis2(-aDirToAxis2.X(), -aDirToAxis2.Y(), -aDirToAxis2.Z());

    // Find U1 where cone1 points toward axis2
    const double aDot1X = aDirToAxis2.X() * myXDir1.X() + aDirToAxis2.Y() * myXDir1.Y()
                          + aDirToAxis2.Z() * myXDir1.Z();
    const double aDot1Y = aDirToAxis2.X() * myYDir1.X() + aDirToAxis2.Y() * myYDir1.Y()
                          + aDirToAxis2.Z() * myYDir1.Z();
    const double aU1Toward = std::atan2(aDot1Y, aDot1X);
    const double aU1Away   = aU1Toward + M_PI;

    // Find U2 where cone2 points toward axis1
    const double aDot2X = aDirFromAxis2.X() * myXDir2.X() + aDirFromAxis2.Y() * myXDir2.Y()
                          + aDirFromAxis2.Z() * myXDir2.Z();
    const double aDot2Y = aDirFromAxis2.X() * myYDir2.X() + aDirFromAxis2.Y() * myYDir2.Y()
                          + aDirFromAxis2.Z() * myYDir2.Z();
    const double aU2Toward = std::atan2(aDot2Y, aDot2X);
    const double aU2Away   = aU2Toward + M_PI;

    // For parallel cones with OCCT parameterization:
    // At V1, R1 = RefRadius1 + V1*tan(α1)
    // At V2, R2 = RefRadius2 + V2*tan(α2)
    // The distance between surfaces depends on R1, R2 and axis distance

    // Sample V values to find extrema
    searchParallelCase(aU1Toward, aU1Away, aU2Toward, aU2Away, aAxisDist, theTol, theMode);
  }

  //! Search for extrema in parallel case by sampling V values.
  void searchParallelCase(double theU1Toward, double theU1Away,
                          double theU2Toward, double theU2Away,
                          double theAxisDist,
                          double theTol, ExtremaSS::SearchMode theMode) const
  {
    constexpr int aNbSamples = 50;
    constexpr double aVMin = -50.0;  // V can be negative in OCCT parameterization
    constexpr double aVMax = 100.0;

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinV1 = 0.0, aBestMinV2 = 0.0;
    double aBestMaxV1 = 0.0, aBestMaxV2 = 0.0;

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aVMin + i * (aVMax - aVMin) / aNbSamples;
      // OCCT radius: R = RefRadius + V * tan(SemiAngle)
      const double aR1 = myRefRadius1 + aV1 * myTan1;
      if (aR1 < 0.0) continue;  // Skip invalid radius

      for (int j = 0; j <= aNbSamples; ++j)
      {
        const double aV2 = aVMin + j * (aVMax - aVMin) / aNbSamples;
        const double aR2 = myRefRadius2 + aV2 * myTan2;
        if (aR2 < 0.0) continue;  // Skip invalid radius

        // Minimum: both pointing toward each other
        const double aMinDist = std::abs(theAxisDist - aR1 - aR2);
        if (aMinDist * aMinDist < aBestMinSqDist)
        {
          aBestMinSqDist = aMinDist * aMinDist;
          aBestMinV1     = aV1;
          aBestMinV2     = aV2;
        }

        // Maximum: pointing away
        const double aMaxDist = theAxisDist + aR1 + aR2;
        if (aMaxDist * aMaxDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aMaxDist * aMaxDist;
          aBestMaxV1     = aV1;
          aBestMaxV2     = aV2;
        }
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(theU1Toward, aBestMinV1, theU2Toward, aBestMinV2, aBestMinSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(theU1Away, aBestMaxV1, theU2Away, aBestMaxV2, aBestMaxSqDist, false, theTol);
    }
  }

  //! Compute extrema when axes are coaxial.
  void computeCoaxialCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::InfiniteSolutions;

    // For coaxial cones with OCCT parameterization:
    // At V1, R1 = RefRadius1 + V1*tan(α1)
    // At V2, R2 = RefRadius2 + V2*tan(α2)

    // Sample to find extrema
    constexpr int    aNbSamples = 30;
    constexpr double aVMin      = -50.0;
    constexpr double aVMax      = 50.0;

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinV1 = 0, aBestMinV2 = 0;
    double aBestMaxV1 = 0, aBestMaxV2 = 0;

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aVMin + i * (aVMax - aVMin) / aNbSamples;
      const double aR1 = myRefRadius1 + aV1 * myTan1;
      if (aR1 < 0.0) continue;

      for (int j = 0; j <= aNbSamples; ++j)
      {
        const double aV2 = aVMin + j * (aVMax - aVMin) / aNbSamples;
        const double aR2 = myRefRadius2 + aV2 * myTan2;
        if (aR2 < 0.0) continue;

        // Minimum: same U angle
        const double aMinDist = std::abs(aR1 - aR2);
        if (aMinDist * aMinDist < aBestMinSqDist)
        {
          aBestMinSqDist = aMinDist * aMinDist;
          aBestMinV1     = aV1;
          aBestMinV2     = aV2;
        }

        // Maximum: opposite U angles
        const double aMaxDist = aR1 + aR2;
        if (aMaxDist * aMaxDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aMaxDist * aMaxDist;
          aBestMaxV1     = aV1;
          aBestMaxV2     = aV2;
        }
      }
    }

    myResult.InfiniteSquareDistance = aBestMinSqDist;

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(0.0, aBestMinV1, 0.0, aBestMinV2, aBestMinSqDist, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(0.0, aBestMaxV1, M_PI, aBestMaxV2, aBestMaxSqDist, false, theTol);
    }
  }

  //! Compute extrema for general (non-parallel) axes case.
  void computeGeneralCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::OK;

    // First check apex-to-apex distance (important special case)
    const double aApexDistSq = myApex1.SquareDistance(myApex2);
    if (aApexDistSq < theTol * theTol)
    {
      // Apexes coincide - minimum distance is 0
      // For cones with refRadius=0, the apex is at V where R=0
      const double aV1Apex = -myRefRadius1 / myTan1;
      const double aV2Apex = -myRefRadius2 / myTan2;
      addExtremum(0.0, aV1Apex, 0.0, aV2Apex, 0.0, true, theTol);
    }

    // For skew/intersecting axes, we need to search over V1, V2
    // For each V1, V2, we have circles at different positions and radii
    // Using OCCT parameterization: R = RefRadius + V * tan(SemiAngle)

    constexpr int aNbSamples = 40;
    constexpr double aVMin = -50.0;
    constexpr double aVMax = 50.0;

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 0, aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU1 = 0, aBestMaxV1 = 0, aBestMaxU2 = 0, aBestMaxV2 = 0;

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aVMin + i * (aVMax - aVMin) / aNbSamples;
      // OCCT parameterization: R = RefRadius + V * tan(SemiAngle)
      const double aR1 = myRefRadius1 + aV1 * myTan1;
      if (aR1 < 0.0) continue;

      // Center of circle on cone 1: Location + V * Direction
      const gp_Pnt aCircle1Center(myLocation1.X() + aV1 * myAxis1.X(),
                                  myLocation1.Y() + aV1 * myAxis1.Y(),
                                  myLocation1.Z() + aV1 * myAxis1.Z());

      for (int j = 0; j <= aNbSamples; ++j)
      {
        const double aV2 = aVMin + j * (aVMax - aVMin) / aNbSamples;
        const double aR2 = myRefRadius2 + aV2 * myTan2;
        if (aR2 < 0.0) continue;

        // Center of circle on cone 2
        const gp_Pnt aCircle2Center(myLocation2.X() + aV2 * myAxis2.X(),
                                    myLocation2.Y() + aV2 * myAxis2.Y(),
                                    myLocation2.Z() + aV2 * myAxis2.Z());

        // Direction between circle centers
        gp_Vec aDirBetween(aCircle1Center, aCircle2Center);
        const double aDistBetween = aDirBetween.Magnitude();

        // Handle case where circle centers coincide
        if (aDistBetween < theTol)
        {
          // Same center - minimum is |R1 - R2|, maximum is R1 + R2
          const double aMinDist = std::abs(aR1 - aR2);
          const double aMaxDist = aR1 + aR2;
          if (aMinDist * aMinDist < aBestMinSqDist)
          {
            aBestMinSqDist = aMinDist * aMinDist;
            aBestMinU1 = 0; aBestMinV1 = aV1;
            aBestMinU2 = 0; aBestMinV2 = aV2;
          }
          if (aMaxDist * aMaxDist > aBestMaxSqDist)
          {
            aBestMaxSqDist = aMaxDist * aMaxDist;
            aBestMaxU1 = 0; aBestMaxV1 = aV1;
            aBestMaxU2 = M_PI; aBestMaxV2 = aV2;
          }
          continue;
        }

        aDirBetween.Divide(aDistBetween);

        // Project onto perpendicular planes
        gp_Vec aDir1Perp = aDirBetween - gp_Vec(myAxis1) * aDirBetween.Dot(gp_Vec(myAxis1));
        gp_Vec aDir2Perp = aDirBetween - gp_Vec(myAxis2) * aDirBetween.Dot(gp_Vec(myAxis2));

        const double aMag1 = aDir1Perp.Magnitude();
        const double aMag2 = aDir2Perp.Magnitude();

        double aU1Min = 0, aU1Max = M_PI;
        double aU2Min = 0, aU2Max = M_PI;

        if (aMag1 > theTol)
        {
          aDir1Perp.Divide(aMag1);
          const double aX = aDir1Perp.Dot(gp_Vec(myXDir1));
          const double aY = aDir1Perp.Dot(gp_Vec(myYDir1));
          aU1Min          = std::atan2(aY, aX);
          aU1Max          = aU1Min + M_PI;
        }

        if (aMag2 > theTol)
        {
          aDir2Perp.Divide(aMag2);
          const double aX = aDir2Perp.Dot(gp_Vec(myXDir2));
          const double aY = aDir2Perp.Dot(gp_Vec(myYDir2));
          aU2Min          = std::atan2(aY, aX) + M_PI; // Point toward cone 1
          aU2Max          = aU2Min + M_PI;
        }

        const double aProj1 = aR1 * aMag1;
        const double aProj2 = aR2 * aMag2;

        const double aMinDist = std::max(0.0, aDistBetween - aProj1 - aProj2);
        const double aMaxDist = aDistBetween + aProj1 + aProj2;

        if (aMinDist * aMinDist < aBestMinSqDist)
        {
          aBestMinSqDist = aMinDist * aMinDist;
          aBestMinU1     = aU1Min;
          aBestMinV1     = aV1;
          aBestMinU2     = aU2Min;
          aBestMinV2     = aV2;
        }

        if (aMaxDist * aMaxDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aMaxDist * aMaxDist;
          aBestMaxU1     = aU1Max;
          aBestMaxV1     = aV1;
          aBestMaxU2     = aU2Max;
          aBestMaxV2     = aV2;
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

  //! Compute point and first derivatives on cone 1.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @param theP output point
  //! @param theDU output dP/dU
  //! @param theDV output dP/dV
  void D1_1(double theU, double theV, gp_Pnt& theP, gp_Vec& theDU, gp_Vec& theDV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius1 + theV * myTan1;

    theP = gp_Pnt(myLocation1.X() + theV * myAxis1.X()
                      + aRadius * (aCosU * myXDir1.X() + aSinU * myYDir1.X()),
                  myLocation1.Y() + theV * myAxis1.Y()
                      + aRadius * (aCosU * myXDir1.Y() + aSinU * myYDir1.Y()),
                  myLocation1.Z() + theV * myAxis1.Z()
                      + aRadius * (aCosU * myXDir1.Z() + aSinU * myYDir1.Z()));

    // dP/dU = R * (-sin(U)*XDir + cos(U)*YDir)
    theDU = gp_Vec(aRadius * (-aSinU * myXDir1.X() + aCosU * myYDir1.X()),
                   aRadius * (-aSinU * myXDir1.Y() + aCosU * myYDir1.Y()),
                   aRadius * (-aSinU * myXDir1.Z() + aCosU * myYDir1.Z()));

    // dP/dV = Axis + tan(α) * (cos(U)*XDir + sin(U)*YDir)
    theDV = gp_Vec(myAxis1.X() + myTan1 * (aCosU * myXDir1.X() + aSinU * myYDir1.X()),
                   myAxis1.Y() + myTan1 * (aCosU * myXDir1.Y() + aSinU * myYDir1.Y()),
                   myAxis1.Z() + myTan1 * (aCosU * myXDir1.Z() + aSinU * myYDir1.Z()));
  }

  //! Compute point and first derivatives on cone 2.
  void D1_2(double theU, double theV, gp_Pnt& theP, gp_Vec& theDU, gp_Vec& theDV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius2 + theV * myTan2;

    theP = gp_Pnt(myLocation2.X() + theV * myAxis2.X()
                      + aRadius * (aCosU * myXDir2.X() + aSinU * myYDir2.X()),
                  myLocation2.Y() + theV * myAxis2.Y()
                      + aRadius * (aCosU * myXDir2.Y() + aSinU * myYDir2.Y()),
                  myLocation2.Z() + theV * myAxis2.Z()
                      + aRadius * (aCosU * myXDir2.Z() + aSinU * myYDir2.Z()));

    theDU = gp_Vec(aRadius * (-aSinU * myXDir2.X() + aCosU * myYDir2.X()),
                   aRadius * (-aSinU * myXDir2.Y() + aCosU * myYDir2.Y()),
                   aRadius * (-aSinU * myXDir2.Z() + aCosU * myYDir2.Z()));

    theDV = gp_Vec(myAxis2.X() + myTan2 * (aCosU * myXDir2.X() + aSinU * myYDir2.X()),
                   myAxis2.Y() + myTan2 * (aCosU * myXDir2.Y() + aSinU * myYDir2.Y()),
                   myAxis2.Z() + myTan2 * (aCosU * myXDir2.Z() + aSinU * myYDir2.Z()));
  }

  //! Compute second derivatives on cone 1.
  void D2_1(double theU, double theV, gp_Vec& theDUU, gp_Vec& theDUV, gp_Vec& theDVV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius1 + theV * myTan1;

    // d²P/dU² = R * (-cos(U)*XDir - sin(U)*YDir) = -R * (cos(U)*XDir + sin(U)*YDir)
    theDUU = gp_Vec(-aRadius * (aCosU * myXDir1.X() + aSinU * myYDir1.X()),
                    -aRadius * (aCosU * myXDir1.Y() + aSinU * myYDir1.Y()),
                    -aRadius * (aCosU * myXDir1.Z() + aSinU * myYDir1.Z()));

    // d²P/dUdV = tan(α) * (-sin(U)*XDir + cos(U)*YDir)
    theDUV = gp_Vec(myTan1 * (-aSinU * myXDir1.X() + aCosU * myYDir1.X()),
                    myTan1 * (-aSinU * myXDir1.Y() + aCosU * myYDir1.Y()),
                    myTan1 * (-aSinU * myXDir1.Z() + aCosU * myYDir1.Z()));

    // d²P/dV² = 0
    theDVV = gp_Vec(0, 0, 0);
  }

  //! Compute second derivatives on cone 2.
  void D2_2(double theU, double theV, gp_Vec& theDUU, gp_Vec& theDUV, gp_Vec& theDVV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = myRefRadius2 + theV * myTan2;

    theDUU = gp_Vec(-aRadius * (aCosU * myXDir2.X() + aSinU * myYDir2.X()),
                    -aRadius * (aCosU * myXDir2.Y() + aSinU * myYDir2.Y()),
                    -aRadius * (aCosU * myXDir2.Z() + aSinU * myYDir2.Z()));

    theDUV = gp_Vec(myTan2 * (-aSinU * myXDir2.X() + aCosU * myYDir2.X()),
                    myTan2 * (-aSinU * myXDir2.Y() + aCosU * myYDir2.Y()),
                    myTan2 * (-aSinU * myXDir2.Z() + aCosU * myYDir2.Z()));

    theDVV = gp_Vec(0, 0, 0);
  }

  //! Refine an extremum using Newton4D (4D Newton-Raphson).
  //! This is significantly faster than Powell's gradient-free method.
  void refineExtremum(double theU1,
                      double theV1,
                      double theU2,
                      double theV2,
                      bool   theIsMin,
                      double theTol) const
  {
    // Sign for gradient: +1 for minimum (gradient should be zero), -1 for maximum
    const double aSign = theIsMin ? 1.0 : -1.0;

    // Functor for gradient of D² and its Jacobian (Hessian of D²)
    auto aFunc = [this, aSign](double aU1, double aV1, double aU2, double aV2,
                                double aF[4], double aJ[4][4]) -> bool {
      // Compute points and derivatives on both cones
      gp_Pnt aP1, aP2;
      gp_Vec aDU1, aDV1, aDU2, aDV2;
      D1_1(aU1, aV1, aP1, aDU1, aDV1);
      D1_2(aU2, aV2, aP2, aDU2, aDV2);

      // Delta = P1 - P2
      const gp_Vec aDelta(aP2, aP1);

      // Gradient of D² (to be zeroed):
      // F0 = ∂D²/∂U1 = 2 * Delta · dP1/dU1
      // F1 = ∂D²/∂V1 = 2 * Delta · dP1/dV1
      // F2 = ∂D²/∂U2 = -2 * Delta · dP2/dU2
      // F3 = ∂D²/∂V2 = -2 * Delta · dP2/dV2
      aF[0] = aSign * 2.0 * aDelta.Dot(aDU1);
      aF[1] = aSign * 2.0 * aDelta.Dot(aDV1);
      aF[2] = aSign * (-2.0) * aDelta.Dot(aDU2);
      aF[3] = aSign * (-2.0) * aDelta.Dot(aDV2);

      // Second derivatives for Jacobian (Hessian of D²)
      gp_Vec aDUU1, aDUV1, aDVV1;
      gp_Vec aDUU2, aDUV2, aDVV2;
      D2_1(aU1, aV1, aDUU1, aDUV1, aDVV1);
      D2_2(aU2, aV2, aDUU2, aDUV2, aDVV2);

      // Jacobian J[i][j] = ∂F[i]/∂x[j]
      // J[0][0] = ∂²D²/∂U1² = 2*(dP1/dU1 · dP1/dU1 + Delta · d²P1/dU1²)
      aJ[0][0] = aSign * 2.0 * (aDU1.Dot(aDU1) + aDelta.Dot(aDUU1));
      // J[0][1] = ∂²D²/∂U1∂V1 = 2*(dP1/dU1 · dP1/dV1 + Delta · d²P1/dU1dV1)
      aJ[0][1] = aSign * 2.0 * (aDU1.Dot(aDV1) + aDelta.Dot(aDUV1));
      // J[0][2] = ∂²D²/∂U1∂U2 = -2*(dP1/dU1 · dP2/dU2)
      aJ[0][2] = aSign * (-2.0) * aDU1.Dot(aDU2);
      // J[0][3] = ∂²D²/∂U1∂V2 = -2*(dP1/dU1 · dP2/dV2)
      aJ[0][3] = aSign * (-2.0) * aDU1.Dot(aDV2);

      // J[1][0] = J[0][1] (symmetric)
      aJ[1][0] = aJ[0][1];
      // J[1][1] = ∂²D²/∂V1² = 2*(dP1/dV1 · dP1/dV1 + Delta · d²P1/dV1²)
      aJ[1][1] = aSign * 2.0 * (aDV1.Dot(aDV1) + aDelta.Dot(aDVV1));
      // J[1][2] = ∂²D²/∂V1∂U2 = -2*(dP1/dV1 · dP2/dU2)
      aJ[1][2] = aSign * (-2.0) * aDV1.Dot(aDU2);
      // J[1][3] = ∂²D²/∂V1∂V2 = -2*(dP1/dV1 · dP2/dV2)
      aJ[1][3] = aSign * (-2.0) * aDV1.Dot(aDV2);

      // J[2][0] = J[0][2] (symmetric)
      aJ[2][0] = aJ[0][2];
      // J[2][1] = J[1][2] (symmetric)
      aJ[2][1] = aJ[1][2];
      // J[2][2] = ∂²D²/∂U2² = 2*(dP2/dU2 · dP2/dU2 + (-Delta) · d²P2/dU2²)
      aJ[2][2] = aSign * 2.0 * (aDU2.Dot(aDU2) - aDelta.Dot(aDUU2));
      // J[2][3] = ∂²D²/∂U2∂V2 = 2*(dP2/dU2 · dP2/dV2 + (-Delta) · d²P2/dU2dV2)
      aJ[2][3] = aSign * 2.0 * (aDU2.Dot(aDV2) - aDelta.Dot(aDUV2));

      // J[3][0] = J[0][3] (symmetric)
      aJ[3][0] = aJ[0][3];
      // J[3][1] = J[1][3] (symmetric)
      aJ[3][1] = aJ[1][3];
      // J[3][2] = J[2][3] (symmetric)
      aJ[3][2] = aJ[2][3];
      // J[3][3] = ∂²D²/∂V2² = 2*(dP2/dV2 · dP2/dV2 + (-Delta) · d²P2/dV2²)
      aJ[3][3] = aSign * 2.0 * (aDV2.Dot(aDV2) - aDelta.Dot(aDVV2));

      return true;
    };

    // Bounds for Newton4D (use wide bounds for unbounded case)
    constexpr double aUMin = -2.0 * M_PI;
    constexpr double aUMax = 4.0 * M_PI;
    constexpr double aVMin = -100.0;
    constexpr double aVMax = 200.0;

    // Use Newton4D for fast convergence
    MathSys::Newton4DResult aNewtonRes = MathSys::Newton4D(
      aFunc, theU1, theV1, theU2, theV2,
      aUMin, aUMax, aVMin, aVMax, aUMin, aUMax, aVMin, aVMax,
      theTol * 0.01, 15);

    double aU1 = theU1, aV1 = theV1, aU2 = theU2, aV2 = theV2;

    if (aNewtonRes.IsDone())
    {
      aU1 = aNewtonRes.X1;
      aV1 = aNewtonRes.X2;
      aU2 = aNewtonRes.X3;
      aV2 = aNewtonRes.X4;
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

    const ExtremaSS::Domain4D&  aDom  = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    constexpr int aNbSamples = 20;

    // Check cone 1 boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkPointAgainstCone2(aU1, aDom1.VMin, theTol, theMode);
      checkPointAgainstCone2(aU1, aDom1.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkPointAgainstCone2(aDom1.UMin, aV1, theTol, theMode);
      checkPointAgainstCone2(aDom1.UMax, aV1, theTol, theMode);
    }

    // Check cone 2 boundaries
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkPointAgainstCone1(aU2, aDom2.VMin, theTol, theMode);
      checkPointAgainstCone1(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkPointAgainstCone1(aDom2.UMin, aV2, theTol, theMode);
      checkPointAgainstCone1(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on cone 1 against cone 2.
  void checkPointAgainstCone2(double theU1, double theV1, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    // Search over cone 2 to find closest/farthest
    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU2 = 0, aBestMinV2 = 1;
    double aBestMaxU2 = 0, aBestMaxV2 = 1;

    constexpr int aNbSamples = 36;
    constexpr double aVMin = 0.1, aVMax = 50.0;

    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aU2 = 2.0 * M_PI * i / aNbSamples;
      for (int j = 0; j <= 20; ++j)
      {
        const double aV2     = aVMin + j * (aVMax - aVMin) / 20;
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

  //! Check a point on cone 2 against cone 1.
  void checkPointAgainstCone1(double theU2, double theV2, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 1;
    double aBestMaxU1 = 0, aBestMaxV1 = 1;

    constexpr int aNbSamples = 36;
    constexpr double aVMin = 0.1, aVMax = 50.0;

    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aU1 = 2.0 * M_PI * i / aNbSamples;
      for (int j = 0; j <= 20; ++j)
      {
        const double aV1     = aVMin + j * (aVMax - aVMin) / 20;
        const gp_Pnt aP1     = Value1(aU1, aV1);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSqDist < aBestMinSqDist)
        {
          aBestMinSqDist = aSqDist;
          aBestMinU1     = aU1;
          aBestMinV1     = aV1;
        }
        if (aSqDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aSqDist;
          aBestMaxU1     = aU1;
          aBestMaxV1     = aV1;
        }
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(aBestMinU1, aBestMinV1, theU2, theV2, aBestMinSqDist, true, theTol);
    }
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(aBestMaxU1, aBestMaxV1, theU2, theV2, aBestMaxSqDist, false, theTol);
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

    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      if (std::abs(anExisting.SquareDistance - theSqDist) < aTolSq)
      {
        const double aDU1 = std::abs(anExisting.U1 - aU1);
        const double aDU2 = std::abs(anExisting.U2 - aU2);
        const double aDV1 = std::abs(anExisting.V1 - theV1);
        const double aDV2 = std::abs(anExisting.V2 - theV2);

        if ((aDU1 < theTol || std::abs(aDU1 - 2 * M_PI) < theTol) &&
            (aDU2 < theTol || std::abs(aDU2 - 2 * M_PI) < theTol) && aDV1 < theTol && aDV2 < theTol)
        {
          return;
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
  gp_Cone myCone1; //!< First cone
  gp_Cone myCone2; //!< Second cone

  std::optional<ExtremaSS::Domain4D> myDomain; //!< Optional bounded domain

  // Cached cone 1 geometry (OCCT parameterization)
  gp_Pnt myLocation1;   //!< Location (origin) of cone 1
  gp_Pnt myApex1;       //!< Apex of cone 1
  gp_Dir myAxis1;       //!< Axis of cone 1
  gp_Dir myXDir1;       //!< X direction of cone 1
  gp_Dir myYDir1;       //!< Y direction of cone 1
  double mySemiAngle1;  //!< Semi-angle of cone 1
  double myRefRadius1;  //!< Reference radius of cone 1
  double myTan1;        //!< tan(semi-angle) of cone 1
  double mySin1;        //!< sin(semi-angle) of cone 1
  double myCos1;        //!< cos(semi-angle) of cone 1

  // Cached cone 2 geometry (OCCT parameterization)
  gp_Pnt myLocation2;   //!< Location (origin) of cone 2
  gp_Pnt myApex2;       //!< Apex of cone 2
  gp_Dir myAxis2;       //!< Axis of cone 2
  gp_Dir myXDir2;       //!< X direction of cone 2
  gp_Dir myYDir2;       //!< Y direction of cone 2
  double mySemiAngle2;  //!< Semi-angle of cone 2
  double myRefRadius2;  //!< Reference radius of cone 2
  double myTan2;        //!< tan(semi-angle) of cone 2
  double mySin2;        //!< sin(semi-angle) of cone 2
  double myCos2;        //!< cos(semi-angle) of cone 2

  // Cached relationship
  gp_Dir myCrossProduct;   //!< Cross product of axes
  gp_Vec myDeltaLocation;  //!< Vector from location1 to location2
  gp_Vec myDeltaApex;      //!< Vector from apex1 to apex2
  bool   myAxesParallel;   //!< True if axes are parallel

  mutable ExtremaSS::Result myResult; //!< Computation result
};

#endif // _ExtremaSS_ConeCone_HeaderFile
