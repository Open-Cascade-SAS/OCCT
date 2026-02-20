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

#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_Pnt2dTest, DefaultConstructor)
{
  gp_Pnt2d aPnt;
  EXPECT_DOUBLE_EQ(aPnt.X(), 0.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 0.0);
}

TEST(gp_Pnt2dTest, CoordinateConstructor)
{
  gp_Pnt2d aPnt(1.0, 2.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 1.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 2.0);
}

TEST(gp_Pnt2dTest, XYConstructor)
{
  gp_XY aXY(3.0, 4.0);
  gp_Pnt2d aPnt(aXY);
  EXPECT_DOUBLE_EQ(aPnt.X(), 3.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 4.0);
}

TEST(gp_Pnt2dTest, SetCoord)
{
  gp_Pnt2d aPnt;
  aPnt.SetX(10.0);
  aPnt.SetY(20.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 10.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 20.0);

  aPnt.SetCoord(7.0, 8.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 7.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 8.0);
}

TEST(gp_Pnt2dTest, Distance)
{
  gp_Pnt2d aP1(0.0, 0.0);
  gp_Pnt2d aP2(3.0, 4.0);
  EXPECT_NEAR(aP1.Distance(aP2), 5.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, SquareDistance)
{
  gp_Pnt2d aP1(1.0, 2.0);
  gp_Pnt2d aP2(4.0, 6.0);
  // (4-1)^2 + (6-2)^2 = 9 + 16 = 25
  EXPECT_NEAR(aP1.SquareDistance(aP2), 25.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, IsEqual)
{
  gp_Pnt2d aP1(1.0, 2.0);
  gp_Pnt2d aP2(1.0, 2.0);
  gp_Pnt2d aP3(1.0, 3.0);
  EXPECT_TRUE(aP1.IsEqual(aP2, Precision::Confusion()));
  EXPECT_FALSE(aP1.IsEqual(aP3, Precision::Confusion()));
}

TEST(gp_Pnt2dTest, Translate_ByVec2d)
{
  gp_Pnt2d aPnt(1.0, 2.0);
  gp_Vec2d aVec(10.0, 20.0);
  gp_Pnt2d aResult = aPnt.Translated(aVec);
  EXPECT_NEAR(aResult.X(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 22.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, Scale)
{
  gp_Pnt2d aPnt(2.0, 4.0);
  gp_Pnt2d aCenter(0.0, 0.0);
  gp_Pnt2d aResult = aPnt.Scaled(aCenter, 2.0);
  EXPECT_NEAR(aResult.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 8.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, Rotate)
{
  gp_Pnt2d aPnt(1.0, 0.0);
  gp_Pnt2d aCenter(0.0, 0.0);
  gp_Pnt2d aResult = aPnt.Rotated(aCenter, M_PI_2);
  EXPECT_NEAR(aResult.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 1.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, Mirror_Point)
{
  gp_Pnt2d aPnt(1.0, 0.0);
  gp_Pnt2d aCenter(0.0, 0.0);
  gp_Pnt2d aResult = aPnt.Mirrored(aCenter);
  EXPECT_NEAR(aResult.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 0.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, Mirror_Axis)
{
  gp_Pnt2d aPnt(1.0, 1.0);
  // Mirror through the X axis
  gp_Ax2d anAxis(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  gp_Pnt2d aResult = aPnt.Mirrored(anAxis);
  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), -1.0, Precision::Confusion());
}

TEST(gp_Pnt2dTest, Transformed)
{
  gp_Pnt2d  aPnt(1.0, 0.0);
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(0.0, 5.0));
  gp_Pnt2d aResult = aPnt.Transformed(aTrsf);
  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 5.0, Precision::Confusion());
}
