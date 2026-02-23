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

#include <Geom2d_Hyperbola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_HyperbolaTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Hyperbola centered at origin, a=5, b=3
    gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
    myHyperbola = new Geom2d_Hyperbola(aHypr);
  }

  occ::handle<Geom2d_Hyperbola> myHyperbola;
};

TEST_F(Geom2d_HyperbolaTest, Radii)
{
  EXPECT_NEAR(myHyperbola->MajorRadius(), 5.0, Precision::Confusion());
  EXPECT_NEAR(myHyperbola->MinorRadius(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, IsNotClosedNotPeriodic)
{
  EXPECT_FALSE(myHyperbola->IsClosed());
  EXPECT_FALSE(myHyperbola->IsPeriodic());
}

TEST_F(Geom2d_HyperbolaTest, Eccentricity)
{
  // e = c/a where c = sqrt(a^2 + b^2) = sqrt(25+9) = sqrt(34)
  const double anExpected = std::sqrt(34.0) / 5.0;
  EXPECT_NEAR(myHyperbola->Eccentricity(), anExpected, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Focal)
{
  // 2c = 2*sqrt(a^2 + b^2)
  const double anExpected = 2.0 * std::sqrt(25.0 + 9.0);
  EXPECT_NEAR(myHyperbola->Focal(), anExpected, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Foci)
{
  const double   c   = std::sqrt(25.0 + 9.0);
  const gp_Pnt2d aF1 = myHyperbola->Focus1();
  const gp_Pnt2d aF2 = myHyperbola->Focus2();

  EXPECT_NEAR(aF1.X(), c, Precision::Confusion());
  EXPECT_NEAR(aF2.X(), -c, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Parameter)
{
  // Semi-latus rectum = b^2/a = 9/5 = 1.8
  EXPECT_NEAR(myHyperbola->Parameter(), 1.8, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, EvalD0_AtZero)
{
  // At U=0: vertex = (a, 0) = (5, 0) since x = a*cosh(0) = a, y = b*sinh(0) = 0
  const gp_Pnt2d aPnt = myHyperbola->EvalD0(0.0);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, EvalD1_AtZero)
{
  const auto [aPnt, aD1] = myHyperbola->EvalD1(0.0);
  // At U=0: dx/du = a*sinh(0) = 0, dy/du = b*cosh(0) = b = 3
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, PointOnHyperbola_FociDistanceDifference)
{
  // For any point on hyperbola: |d(P,F1) - d(P,F2)| = 2a
  const gp_Pnt2d aF1 = myHyperbola->Focus1();
  const gp_Pnt2d aF2 = myHyperbola->Focus2();

  for (double u = -2.0; u <= 2.0; u += 0.5)
  {
    const gp_Pnt2d aPnt  = myHyperbola->EvalD0(u);
    const double   aDiff = std::abs(aPnt.Distance(aF1) - aPnt.Distance(aF2));
    EXPECT_NEAR(aDiff, 2.0 * 5.0, Precision::Confusion());
  }
}

TEST_F(Geom2d_HyperbolaTest, SetMajorRadius)
{
  myHyperbola->SetMajorRadius(8.0);
  EXPECT_NEAR(myHyperbola->MajorRadius(), 8.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, SetMinorRadius)
{
  myHyperbola->SetMinorRadius(4.0);
  EXPECT_NEAR(myHyperbola->MinorRadius(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Copy)
{
  occ::handle<Geom2d_Geometry>  aCopyGeom = myHyperbola->Copy();
  occ::handle<Geom2d_Hyperbola> aCopy     = occ::down_cast<Geom2d_Hyperbola>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->MajorRadius(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCopy->MinorRadius(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Transform_Translation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10.0, 20.0));
  myHyperbola->Transform(aTrsf);

  EXPECT_NEAR(myHyperbola->Location().X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(myHyperbola->Location().Y(), 20.0, Precision::Confusion());
}

TEST_F(Geom2d_HyperbolaTest, Asymptotes)
{
  const gp_Ax2d anAsym1 = myHyperbola->Asymptote1();
  const gp_Ax2d anAsym2 = myHyperbola->Asymptote2();

  // Asymptotes pass through center
  EXPECT_NEAR(anAsym1.Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Location().X(), 0.0, Precision::Confusion());
}
