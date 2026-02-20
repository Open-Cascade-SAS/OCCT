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

#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(Geom_CircleTest, Constructor)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 5.0);
  ASSERT_FALSE(aCircle.IsNull());
  EXPECT_NEAR(aCircle->Radius(), 5.0, Precision::Confusion());
}

TEST(Geom_CircleTest, Center)
{
  gp_Pnt                   aCenter(1.0, 2.0, 3.0);
  gp_Ax2                   anAxis(aCenter, gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 5.0);
  EXPECT_TRUE(aCircle->Circ().Location().IsEqual(aCenter, Precision::Confusion()));
}

TEST(Geom_CircleTest, D0Evaluation)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 1.0);

  gp_Pnt aPnt;

  // At parameter 0: point should be (1,0,0) for unit circle in XY plane
  aCircle->D0(0.0, aPnt);
  EXPECT_NEAR(aPnt.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Z(), 0.0, Precision::Confusion());

  // At parameter PI/2: point should be (0,1,0)
  aCircle->D0(M_PI_2, aPnt);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Z(), 0.0, Precision::Confusion());

  // At parameter PI: point should be (-1,0,0)
  aCircle->D0(M_PI, aPnt);
  EXPECT_NEAR(aPnt.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());

  // At parameter 3*PI/2: point should be (0,-1,0)
  aCircle->D0(3.0 * M_PI_2, aPnt);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), -1.0, Precision::Confusion());
}

TEST(Geom_CircleTest, D1Evaluation)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 1.0);

  gp_Pnt aPnt;
  gp_Vec aV1;

  // At parameter 0: tangent should be (0,1,0) for unit circle
  aCircle->D1(0.0, aPnt, aV1);
  EXPECT_NEAR(aV1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aV1.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aV1.Z(), 0.0, Precision::Confusion());
}

TEST(Geom_CircleTest, D2Evaluation)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 1.0);

  gp_Pnt aPnt;
  gp_Vec aV1, aV2;

  // At parameter 0: second derivative should be (-1,0,0) pointing toward center
  aCircle->D2(0.0, aPnt, aV1, aV2);
  EXPECT_NEAR(aV2.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aV2.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aV2.Z(), 0.0, Precision::Confusion());
}

TEST(Geom_CircleTest, Reverse)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 1.0);

  double aParam         = M_PI_4;
  double aReversedParam = aCircle->ReversedParameter(aParam);
  // For a circle: reversed parameter = 2*PI - U
  EXPECT_NEAR(aReversedParam, 2.0 * M_PI - aParam, Precision::Confusion());
}

TEST(Geom_CircleTest, Transform)
{
  gp_Ax2                   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 5.0);

  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0.0, 0.0, 0.0), 2.0);
  aCircle->Transform(aTrsf);
  EXPECT_NEAR(aCircle->Radius(), 10.0, Precision::Confusion());
}

TEST(Geom_CircleTest, Copy)
{
  gp_Ax2                   anAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anAxis, 7.0);

  occ::handle<Geom_Geometry> aCopy = aCircle->Copy();
  ASSERT_FALSE(aCopy.IsNull());
  occ::handle<Geom_Circle> aCopyCircle = occ::down_cast<Geom_Circle>(aCopy);
  ASSERT_FALSE(aCopyCircle.IsNull());
  EXPECT_NEAR(aCopyCircle->Radius(), 7.0, Precision::Confusion());
  EXPECT_TRUE(
    aCopyCircle->Circ().Location().IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
}
