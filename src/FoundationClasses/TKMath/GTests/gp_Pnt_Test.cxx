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

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_PntTest, DefaultConstructor)
{
  gp_Pnt aPnt;
  EXPECT_DOUBLE_EQ(aPnt.X(), 0.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aPnt.Z(), 0.0);
}

TEST(gp_PntTest, CoordinateConstructor)
{
  gp_Pnt aPnt(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 1.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 2.0);
  EXPECT_DOUBLE_EQ(aPnt.Z(), 3.0);
}

TEST(gp_PntTest, XYZConstructor)
{
  gp_XYZ aXYZ(4.0, 5.0, 6.0);
  gp_Pnt aPnt(aXYZ);
  EXPECT_DOUBLE_EQ(aPnt.X(), 4.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 5.0);
  EXPECT_DOUBLE_EQ(aPnt.Z(), 6.0);
}

TEST(gp_PntTest, SetCoord)
{
  gp_Pnt aPnt;
  aPnt.SetX(10.0);
  aPnt.SetY(20.0);
  aPnt.SetZ(30.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 10.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 20.0);
  EXPECT_DOUBLE_EQ(aPnt.Z(), 30.0);

  aPnt.SetCoord(7.0, 8.0, 9.0);
  EXPECT_DOUBLE_EQ(aPnt.X(), 7.0);
  EXPECT_DOUBLE_EQ(aPnt.Y(), 8.0);
  EXPECT_DOUBLE_EQ(aPnt.Z(), 9.0);
}

TEST(gp_PntTest, Distance)
{
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(3.0, 4.0, 0.0);
  EXPECT_NEAR(aP1.Distance(aP2), 5.0, Precision::Confusion());
}

TEST(gp_PntTest, SquareDistance)
{
  gp_Pnt aP1(1.0, 2.0, 3.0);
  gp_Pnt aP2(4.0, 6.0, 3.0);
  // (4-1)^2 + (6-2)^2 + (3-3)^2 = 9 + 16 + 0 = 25
  EXPECT_NEAR(aP1.SquareDistance(aP2), 25.0, Precision::Confusion());
}

TEST(gp_PntTest, IsEqual)
{
  gp_Pnt aP1(1.0, 2.0, 3.0);
  gp_Pnt aP2(1.0, 2.0, 3.0);
  gp_Pnt aP3(1.0, 2.0, 4.0);
  EXPECT_TRUE(aP1.IsEqual(aP2, Precision::Confusion()));
  EXPECT_FALSE(aP1.IsEqual(aP3, Precision::Confusion()));
}

TEST(gp_PntTest, BaryCenter)
{
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(10.0, 0.0, 0.0);
  // BaryCenter with equal weights: midpoint
  aP1.BaryCenter(1.0, aP2, 1.0);
  EXPECT_NEAR(aP1.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aP1.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP1.Z(), 0.0, Precision::Confusion());
}

TEST(gp_PntTest, Translate_ByVec)
{
  gp_Pnt aPnt(1.0, 2.0, 3.0);
  gp_Vec aVec(10.0, 20.0, 30.0);
  gp_Pnt aResult = aPnt.Translated(aVec);
  EXPECT_NEAR(aResult.X(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 22.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 33.0, Precision::Confusion());
}

TEST(gp_PntTest, Translate_ByTwoPoints)
{
  gp_Pnt aPnt(1.0, 2.0, 3.0);
  gp_Pnt aFrom(0.0, 0.0, 0.0);
  gp_Pnt aTo(5.0, 5.0, 5.0);
  gp_Pnt aResult = aPnt.Translated(aFrom, aTo);
  EXPECT_NEAR(aResult.X(), 6.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 8.0, Precision::Confusion());
}

TEST(gp_PntTest, Scale)
{
  gp_Pnt aPnt(2.0, 4.0, 6.0);
  gp_Pnt aCenter(0.0, 0.0, 0.0);
  gp_Pnt aResult = aPnt.Scaled(aCenter, 2.0);
  EXPECT_NEAR(aResult.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 8.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 12.0, Precision::Confusion());
}

TEST(gp_PntTest, Rotate)
{
  gp_Pnt aPnt(1.0, 0.0, 0.0);
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt aResult = aPnt.Rotated(anAxis, M_PI_2);
  EXPECT_NEAR(aResult.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}

TEST(gp_PntTest, Mirror_Point)
{
  gp_Pnt aPnt(1.0, 0.0, 0.0);
  gp_Pnt aCenter(0.0, 0.0, 0.0);
  gp_Pnt aResult = aPnt.Mirrored(aCenter);
  EXPECT_NEAR(aResult.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}

TEST(gp_PntTest, Mirror_Axis)
{
  gp_Pnt aPnt(1.0, 1.0, 0.0);
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Pnt aResult = aPnt.Mirrored(anAxis);
  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}

TEST(gp_PntTest, Mirror_Plane)
{
  gp_Pnt aPnt(1.0, 2.0, 3.0);
  // Mirror through XY plane (Z=0)
  gp_Ax2 anAx2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt aResult = aPnt.Mirrored(anAx2);
  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), -3.0, Precision::Confusion());
}

TEST(gp_PntTest, Transformed)
{
  gp_Pnt  aPnt(1.0, 0.0, 0.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 5.0));
  gp_Pnt aResult = aPnt.Transformed(aTrsf);
  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 5.0, Precision::Confusion());
}
