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

#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_Vec2dTest, DefaultConstructor)
{
  gp_Vec2d aVec;
  EXPECT_DOUBLE_EQ(aVec.X(), 0.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 0.0);
}

TEST(gp_Vec2dTest, CoordinateConstructor)
{
  gp_Vec2d aVec(1.0, 2.0);
  EXPECT_DOUBLE_EQ(aVec.X(), 1.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 2.0);
}

TEST(gp_Vec2dTest, TwoPointConstructor)
{
  gp_Pnt2d aP1(1.0, 2.0);
  gp_Pnt2d aP2(4.0, 6.0);
  gp_Vec2d aVec(aP1, aP2);
  EXPECT_DOUBLE_EQ(aVec.X(), 3.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 4.0);
}

TEST(gp_Vec2dTest, Dir2dConstructor)
{
  gp_Dir2d aDir(0.0, 1.0);
  gp_Vec2d aVec(aDir);
  EXPECT_NEAR(aVec.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVec.Y(), 1.0, Precision::Confusion());
}

TEST(gp_Vec2dTest, AddSubtract)
{
  gp_Vec2d aV1(1.0, 2.0);
  gp_Vec2d aV2(4.0, 5.0);

  gp_Vec2d aSum = aV1 + aV2;
  EXPECT_DOUBLE_EQ(aSum.X(), 5.0);
  EXPECT_DOUBLE_EQ(aSum.Y(), 7.0);

  gp_Vec2d aDiff = aV2 - aV1;
  EXPECT_DOUBLE_EQ(aDiff.X(), 3.0);
  EXPECT_DOUBLE_EQ(aDiff.Y(), 3.0);
}

TEST(gp_Vec2dTest, MultiplyDivide)
{
  gp_Vec2d aVec(2.0, 4.0);

  gp_Vec2d aScaled = aVec * 3.0;
  EXPECT_DOUBLE_EQ(aScaled.X(), 6.0);
  EXPECT_DOUBLE_EQ(aScaled.Y(), 12.0);

  gp_Vec2d aDivided = aVec / 2.0;
  EXPECT_DOUBLE_EQ(aDivided.X(), 1.0);
  EXPECT_DOUBLE_EQ(aDivided.Y(), 2.0);
}

TEST(gp_Vec2dTest, Dot)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(0.0, 1.0);
  EXPECT_DOUBLE_EQ(aV1.Dot(aV2), 0.0);

  gp_Vec2d aV3(1.0, 2.0);
  gp_Vec2d aV4(4.0, 5.0);
  // 1*4 + 2*5 = 14
  EXPECT_DOUBLE_EQ(aV3.Dot(aV4), 14.0);
}

TEST(gp_Vec2dTest, Crossed)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(0.0, 1.0);
  // V1.X*V2.Y - V1.Y*V2.X = 1*1 - 0*0 = 1
  EXPECT_NEAR(aV1.Crossed(aV2), 1.0, Precision::Confusion());

  gp_Vec2d aV3(3.0, 4.0);
  gp_Vec2d aV4(2.0, 5.0);
  // 3*5 - 4*2 = 15 - 8 = 7
  EXPECT_NEAR(aV3.Crossed(aV4), 7.0, Precision::Confusion());
}

TEST(gp_Vec2dTest, Magnitude)
{
  gp_Vec2d aVec(3.0, 4.0);
  EXPECT_NEAR(aVec.Magnitude(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aVec.SquareMagnitude(), 25.0, Precision::Confusion());
}

TEST(gp_Vec2dTest, Normalize)
{
  gp_Vec2d aVec(3.0, 4.0);
  gp_Vec2d aNormalized = aVec.Normalized();
  EXPECT_NEAR(aNormalized.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aNormalized.X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aNormalized.Y(), 0.8, Precision::Confusion());
}

TEST(gp_Vec2dTest, Angle)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(0.0, 1.0);
  EXPECT_NEAR(aV1.Angle(aV2), M_PI_2, Precision::Angular());
}

TEST(gp_Vec2dTest, IsParallel)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(5.0, 0.0);
  EXPECT_TRUE(aV1.IsParallel(aV2, Precision::Angular()));
}

TEST(gp_Vec2dTest, IsNormal)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(0.0, 1.0);
  EXPECT_TRUE(aV1.IsNormal(aV2, Precision::Angular()));
}

TEST(gp_Vec2dTest, IsOpposite)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(-3.0, 0.0);
  EXPECT_TRUE(aV1.IsOpposite(aV2, Precision::Angular()));
}

TEST(gp_Vec2dTest, Reverse)
{
  gp_Vec2d aVec(1.0, 2.0);
  gp_Vec2d aReversed = aVec.Reversed();
  EXPECT_DOUBLE_EQ(aReversed.X(), -1.0);
  EXPECT_DOUBLE_EQ(aReversed.Y(), -2.0);
}

TEST(gp_Vec2dTest, SetLinearForm)
{
  gp_Vec2d aV1(1.0, 0.0);
  gp_Vec2d aV2(0.0, 1.0);
  gp_Vec2d aResult;
  aResult.SetLinearForm(2.0, aV1, 3.0, aV2);
  EXPECT_NEAR(aResult.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 3.0, Precision::Confusion());
}
