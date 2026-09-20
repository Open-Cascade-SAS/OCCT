// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <ElSLib.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-13;

static gp_Ax3 identityPosition()
{
  return gp_Ax3(gp::Origin(), gp::DZ(), gp::DX());
}

static void expectPointExact(const gp_Pnt& theActual, const gp_Pnt& theExpected)
{
  EXPECT_DOUBLE_EQ(theActual.X(), theExpected.X());
  EXPECT_DOUBLE_EQ(theActual.Y(), theExpected.Y());
  EXPECT_DOUBLE_EQ(theActual.Z(), theExpected.Z());
}

static void expectVectorNear(const gp_Vec& theActual, const gp_Vec& theExpected, const double theTol)
{
  EXPECT_NEAR(theActual.X(), theExpected.X(), theTol);
  EXPECT_NEAR(theActual.Y(), theExpected.Y(), theTol);
  EXPECT_NEAR(theActual.Z(), theExpected.Z(), theTol);
}

static double periodicDifference(const double theLeft, const double theRight)
{
  return std::remainder(theLeft - theRight, 2.0 * M_PI);
}
} // namespace

//=================================================================================================

TEST(ElSLibTest, TorusCanonicalQuadrantsAreExact)
{
  const gp_Ax3 aPosition = identityPosition();

  const gp_Pnt aUQuadrant = ElSLib::TorusValue(M_PI_2, 0.0, aPosition, 8.0, 4.0);
  EXPECT_DOUBLE_EQ(aUQuadrant.X(), 0.0);
  EXPECT_DOUBLE_EQ(aUQuadrant.Y(), 12.0);
  EXPECT_DOUBLE_EQ(aUQuadrant.Z(), 0.0);

  const gp_Pnt aVQuadrant = ElSLib::TorusValue(0.0, M_PI_2, aPosition, 8.0, 4.0);
  EXPECT_DOUBLE_EQ(aVQuadrant.X(), 8.0);
  EXPECT_DOUBLE_EQ(aVQuadrant.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aVQuadrant.Z(), 4.0);

  const gp_Pnt aHalfTurn = ElSLib::TorusValue(M_PI, M_PI, aPosition, 8.0, 4.0);
  EXPECT_DOUBLE_EQ(aHalfTurn.X(), -4.0);
  EXPECT_DOUBLE_EQ(aHalfTurn.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aHalfTurn.Z(), 0.0);

  const gp_Pnt aSeam = ElSLib::TorusValue(2.0 * M_PI, 2.0 * M_PI, aPosition, 8.0, 4.0);
  EXPECT_DOUBLE_EQ(aSeam.X(), 12.0);
  EXPECT_DOUBLE_EQ(aSeam.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aSeam.Z(), 0.0);
}

//=================================================================================================

TEST(ElSLibTest, TorusNearQuadrantIsNotSnapped)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = M_PI_2 + 1.0e-12;
  const gp_Pnt aPoint    = ElSLib::TorusValue(anU, 0.0, aPosition, 8.0, 4.0);

  EXPECT_NE(aPoint.X(), 0.0);
  EXPECT_NEAR(aPoint.X(), 12.0 * std::cos(anU), 1.0e-22);
  EXPECT_NEAR(aPoint.Y(), 12.0 * std::sin(anU), 1.0e-14);
}

//=================================================================================================

TEST(ElSLibTest, TorusHornSingularityIsCanonical)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = 0.731;

  gp_Pnt aPoint;
  gp_Vec aDU;
  gp_Vec aDV;
  ElSLib::TorusD1(anU, M_PI, aPosition, 4.0, 4.0, aPoint, aDU, aDV);

  EXPECT_DOUBLE_EQ(aPoint.X(), 0.0);
  EXPECT_DOUBLE_EQ(aPoint.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aPoint.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.X(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.Z(), 0.0);
}

//=================================================================================================

TEST(ElSLibTest, TorusSpindleSingularityIsStable)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = 1.137;
  const double aV        = std::acos(-3.0 / 5.0);

  gp_Pnt aPoint;
  gp_Vec aDU;
  gp_Vec aDV;
  ElSLib::TorusD1(anU, aV, aPosition, 3.0, 5.0, aPoint, aDU, aDV);

  EXPECT_DOUBLE_EQ(aPoint.X(), 0.0);
  EXPECT_DOUBLE_EQ(aPoint.Y(), 0.0);
  EXPECT_NEAR(aPoint.Z(), 4.0, 1.0e-14);
  EXPECT_DOUBLE_EQ(aDU.X(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.Z(), 0.0);
}

//=================================================================================================

TEST(ElSLibTest, TorusNearSpindleSingularityIsNotCollapsed)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = 1.137;
  const double aV        = std::acos(-3.0 / 5.0) + 1.0e-12;

  gp_Pnt aPoint;
  gp_Vec aDU;
  gp_Vec aDV;
  ElSLib::TorusD1(anU, aV, aPosition, 3.0, 5.0, aPoint, aDU, aDV);

  EXPECT_GT(aDU.Magnitude(), 1.0e-13);
  EXPECT_NE(aPoint.X(), 0.0);
  EXPECT_NE(aPoint.Y(), 0.0);
}

//=================================================================================================

TEST(ElSLibTest, TorusDerivativeEvaluatorsAreConsistent)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = 0.371;
  const double aV        = 1.213;
  const double aMajor    = 8.0;
  const double aMinor    = 4.0;

  const gp_Pnt aValue = ElSLib::TorusValue(anU, aV, aPosition, aMajor, aMinor);
  gp_Pnt       aP0;
  ElSLib::TorusD0(anU, aV, aPosition, aMajor, aMinor, aP0);

  gp_Pnt aP1;
  gp_Vec aDU1;
  gp_Vec aDV1;
  ElSLib::TorusD1(anU, aV, aPosition, aMajor, aMinor, aP1, aDU1, aDV1);

  gp_Pnt aP2;
  gp_Vec aDU2;
  gp_Vec aDV2;
  gp_Vec aDUU2;
  gp_Vec aDVV2;
  gp_Vec aDUV2;
  ElSLib::TorusD2(anU, aV, aPosition, aMajor, aMinor, aP2, aDU2, aDV2, aDUU2, aDVV2, aDUV2);

  gp_Pnt aP3;
  gp_Vec aDU3;
  gp_Vec aDV3;
  gp_Vec aDUU3;
  gp_Vec aDVV3;
  gp_Vec aDUV3;
  gp_Vec aDUUU3;
  gp_Vec aDVVV3;
  gp_Vec aDUUV3;
  gp_Vec aDUVV3;
  ElSLib::TorusD3(anU,
                  aV,
                  aPosition,
                  aMajor,
                  aMinor,
                  aP3,
                  aDU3,
                  aDV3,
                  aDUU3,
                  aDVV3,
                  aDUV3,
                  aDUUU3,
                  aDVVV3,
                  aDUUV3,
                  aDUVV3);

  expectPointExact(aValue, aP0);
  expectPointExact(aValue, aP1);
  expectPointExact(aValue, aP2);
  expectPointExact(aValue, aP3);
  expectVectorNear(aDU1, aDU2, THE_TOLERANCE);
  expectVectorNear(aDU1, aDU3, THE_TOLERANCE);
  expectVectorNear(aDV1, aDV2, THE_TOLERANCE);
  expectVectorNear(aDV1, aDV3, THE_TOLERANCE);
  expectVectorNear(aDUU2, aDUU3, THE_TOLERANCE);
  expectVectorNear(aDVV2, aDVV3, THE_TOLERANCE);
  expectVectorNear(aDUV2, aDUV3, THE_TOLERANCE);

  expectVectorNear(aDU1, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 1, 0), THE_TOLERANCE);
  expectVectorNear(aDV1, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 0, 1), THE_TOLERANCE);
  expectVectorNear(aDUU2, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 2, 0), THE_TOLERANCE);
  expectVectorNear(aDVV2, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 0, 2), THE_TOLERANCE);
  expectVectorNear(aDUV2, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 1, 1), THE_TOLERANCE);
  expectVectorNear(aDUUU3, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 3, 0), THE_TOLERANCE);
  expectVectorNear(aDVVV3, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 0, 3), THE_TOLERANCE);
  expectVectorNear(aDUUV3, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 2, 1), THE_TOLERANCE);
  expectVectorNear(aDUVV3, ElSLib::TorusDN(anU, aV, aPosition, aMajor, aMinor, 1, 2), THE_TOLERANCE);
}

//=================================================================================================

TEST(ElSLibTest, TorusParametersRoundTripRingAndSpindle)
{
  const gp_Ax3 aPosition = identityPosition();

  struct Case
  {
    double Major;
    double Minor;
    double U;
    double V;
  };

  const Case aCases[] = {{8.0, 4.0, 0.37, 1.21},
                         {8.0, 4.0, 5.71, 4.02},
                         {3.0, 5.0, 0.70, 0.60},
                         {3.0, 5.0, 0.70, 2.50}};

  for (const Case& aCase : aCases)
  {
    SCOPED_TRACE(::testing::Message() << aCase.Major << ", " << aCase.Minor << ", " << aCase.U
                                      << ", " << aCase.V);
    const gp_Pnt aPoint =
      ElSLib::TorusValue(aCase.U, aCase.V, aPosition, aCase.Major, aCase.Minor);

    double aU = 0.0;
    double aV = 0.0;
    ElSLib::TorusParameters(aPosition, aCase.Major, aCase.Minor, aPoint, aU, aV);

    EXPECT_NEAR(periodicDifference(aU, aCase.U), 0.0, 2.0e-13);
    EXPECT_NEAR(periodicDifference(aV, aCase.V), 0.0, 2.0e-13);
  }
}

//=================================================================================================

TEST(ElSLibTest, TorusParametersRoundTripTransformedFrame)
{
  const gp_Ax3 aPosition(gp_Pnt(3.0, -2.0, 5.0),
                         gp_Dir(0.0, 1.0, 0.0),
                         gp_Dir(1.0, 0.0, 0.0));
  const double anU = 5.17;
  const double aV  = 2.31;

  const gp_Pnt aPoint = ElSLib::TorusValue(anU, aV, aPosition, 8.0, 4.0);
  double       aComputedU = 0.0;
  double       aComputedV = 0.0;
  ElSLib::TorusParameters(aPosition, 8.0, 4.0, aPoint, aComputedU, aComputedV);

  EXPECT_NEAR(periodicDifference(aComputedU, anU), 0.0, 2.0e-13);
  EXPECT_NEAR(periodicDifference(aComputedV, aV), 0.0, 2.0e-13);
}

//=================================================================================================

TEST(ElSLibTest, TorusUniformSmallScaleIsPreserved)
{
  const gp_Ax3 aPosition = identityPosition();
  const double anU       = 0.37;
  const double aV        = 0.91;
  const double aMajor    = 1.0e-100;
  const double aMinor    = 4.0e-101;
  const gp_Pnt aPoint    = ElSLib::TorusValue(anU, aV, aPosition, aMajor, aMinor);

  EXPECT_GT(aPoint.SquareDistance(gp::Origin()), 0.0);
  EXPECT_NE(aPoint.X(), 0.0);
  EXPECT_NE(aPoint.Y(), 0.0);
  EXPECT_NE(aPoint.Z(), 0.0);

  double aComputedU = 0.0;
  double aComputedV = 0.0;
  ElSLib::TorusParameters(aPosition, aMajor, aMinor, aPoint, aComputedU, aComputedV);
  EXPECT_NEAR(periodicDifference(aComputedU, anU), 0.0, 2.0e-13);
  EXPECT_NEAR(periodicDifference(aComputedV, aV), 0.0, 2.0e-13);
}
