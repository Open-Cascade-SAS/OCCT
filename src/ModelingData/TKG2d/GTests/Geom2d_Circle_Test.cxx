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

#include <Geom2d_Circle.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_CircleTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Circle centered at origin, radius 5
    gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
    myCircle = new Geom2d_Circle(aCirc);
  }

  occ::handle<Geom2d_Circle> myCircle;
};

TEST_F(Geom2d_CircleTest, ConstructFromCirc2d)
{
  EXPECT_NEAR(myCircle->Radius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, ConstructFromAxisAndRadius)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 3.0);

  EXPECT_NEAR(aCircle->Radius(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCircle->Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCircle->Location().Y(), 2.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, ConstructFromAx22d)
{
  gp_Ax22d                   anAxis(gp_Pnt2d(3.0, 4.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0));
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(anAxis, 7.0);

  EXPECT_NEAR(aCircle->Radius(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aCircle->Location().X(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, ParameterBounds)
{
  EXPECT_DOUBLE_EQ(myCircle->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(myCircle->LastParameter(), 2.0 * M_PI);
}

TEST_F(Geom2d_CircleTest, IsClosedAndPeriodic)
{
  EXPECT_TRUE(myCircle->IsClosed());
  EXPECT_TRUE(myCircle->IsPeriodic());
}

TEST_F(Geom2d_CircleTest, Eccentricity)
{
  EXPECT_NEAR(myCircle->Eccentricity(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, SetRadius)
{
  myCircle->SetRadius(10.0);
  EXPECT_NEAR(myCircle->Radius(), 10.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, Circ2d)
{
  const gp_Circ2d aCirc = myCircle->Circ2d();
  EXPECT_NEAR(aCirc.Radius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalD0_AtZero)
{
  const gp_Pnt2d aPnt = myCircle->EvalD0(0.0);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalD0_AtPiHalf)
{
  const gp_Pnt2d aPnt = myCircle->EvalD0(M_PI / 2.0);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalD0_AtPi)
{
  const gp_Pnt2d aPnt = myCircle->EvalD0(M_PI);
  EXPECT_NEAR(aPnt.X(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalD1_AtZero)
{
  const auto [aPnt, aD1] = myCircle->EvalD1(0.0);
  // Tangent at U=0 is (0, R) = (0, 5)
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalD2_AtZero)
{
  const auto [aPnt, aD1, aD2] = myCircle->EvalD2(0.0);
  // Second derivative at U=0 is (-R, 0) = (-5, 0)
  EXPECT_NEAR(aD2.X(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aD2.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, EvalDN_Order1)
{
  const gp_Vec2d aD1 = myCircle->EvalDN(0.0, 1);
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, ReversedParameter)
{
  const double aU   = M_PI / 3.0;
  const double aRev = myCircle->ReversedParameter(aU);
  EXPECT_NEAR(aRev, 2.0 * M_PI - aU, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, Copy)
{
  occ::handle<Geom2d_Geometry> aCopyGeom = myCircle->Copy();
  occ::handle<Geom2d_Circle>   aCopy     = occ::down_cast<Geom2d_Circle>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->Radius(), 5.0, Precision::Confusion());

  // Verify independence
  aCopy->SetRadius(1.0);
  EXPECT_NEAR(myCircle->Radius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, Transform_Translation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10.0, 20.0));
  myCircle->Transform(aTrsf);

  EXPECT_NEAR(myCircle->Location().X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(myCircle->Location().Y(), 20.0, Precision::Confusion());
  EXPECT_NEAR(myCircle->Radius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_CircleTest, PointOnCircle_DistanceFromCenter)
{
  // All points on circle should be at distance R from center
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    const gp_Pnt2d aPnt  = myCircle->EvalD0(u);
    const double   aDist = aPnt.Distance(myCircle->Location());
    EXPECT_NEAR(aDist, 5.0, Precision::Confusion());
  }
}
