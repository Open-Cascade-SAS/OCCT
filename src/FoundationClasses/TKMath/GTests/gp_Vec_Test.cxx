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

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_VecTest, DefaultConstructor)
{
  gp_Vec aVec;
  EXPECT_DOUBLE_EQ(aVec.X(), 0.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aVec.Z(), 0.0);
}

TEST(gp_VecTest, CoordinateConstructor)
{
  gp_Vec aVec(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(aVec.X(), 1.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 2.0);
  EXPECT_DOUBLE_EQ(aVec.Z(), 3.0);
}

TEST(gp_VecTest, TwoPointConstructor)
{
  gp_Pnt aP1(1.0, 2.0, 3.0);
  gp_Pnt aP2(4.0, 6.0, 8.0);
  gp_Vec aVec(aP1, aP2);
  EXPECT_DOUBLE_EQ(aVec.X(), 3.0);
  EXPECT_DOUBLE_EQ(aVec.Y(), 4.0);
  EXPECT_DOUBLE_EQ(aVec.Z(), 5.0);
}

TEST(gp_VecTest, DirConstructor)
{
  gp_Dir aDir(0.0, 0.0, 1.0);
  gp_Vec aVec(aDir);
  EXPECT_NEAR(aVec.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVec.Z(), 1.0, Precision::Confusion());
}

TEST(gp_VecTest, AddSubtract)
{
  gp_Vec aV1(1.0, 2.0, 3.0);
  gp_Vec aV2(4.0, 5.0, 6.0);

  gp_Vec aSum = aV1 + aV2;
  EXPECT_DOUBLE_EQ(aSum.X(), 5.0);
  EXPECT_DOUBLE_EQ(aSum.Y(), 7.0);
  EXPECT_DOUBLE_EQ(aSum.Z(), 9.0);

  gp_Vec aDiff = aV2 - aV1;
  EXPECT_DOUBLE_EQ(aDiff.X(), 3.0);
  EXPECT_DOUBLE_EQ(aDiff.Y(), 3.0);
  EXPECT_DOUBLE_EQ(aDiff.Z(), 3.0);
}

TEST(gp_VecTest, MultiplyDivide)
{
  gp_Vec aVec(2.0, 4.0, 6.0);

  gp_Vec aScaled = aVec * 3.0;
  EXPECT_DOUBLE_EQ(aScaled.X(), 6.0);
  EXPECT_DOUBLE_EQ(aScaled.Y(), 12.0);
  EXPECT_DOUBLE_EQ(aScaled.Z(), 18.0);

  gp_Vec aDivided = aVec / 2.0;
  EXPECT_DOUBLE_EQ(aDivided.X(), 1.0);
  EXPECT_DOUBLE_EQ(aDivided.Y(), 2.0);
  EXPECT_DOUBLE_EQ(aDivided.Z(), 3.0);
}

TEST(gp_VecTest, Dot)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(aV1.Dot(aV2), 0.0);

  gp_Vec aV3(1.0, 2.0, 3.0);
  gp_Vec aV4(4.0, 5.0, 6.0);
  // 1*4 + 2*5 + 3*6 = 32
  EXPECT_DOUBLE_EQ(aV3.Dot(aV4), 32.0);
}

TEST(gp_VecTest, Cross)
{
  gp_Vec aVx(1.0, 0.0, 0.0);
  gp_Vec aVy(0.0, 1.0, 0.0);
  gp_Vec aCross = aVx.Crossed(aVy);
  EXPECT_NEAR(aCross.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Z(), 1.0, Precision::Confusion());
}

TEST(gp_VecTest, CrossMagnitude)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  EXPECT_NEAR(aV1.CrossMagnitude(aV2), 1.0, Precision::Confusion());
}

TEST(gp_VecTest, DotCross_TripleScalarProduct)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  gp_Vec aV3(0.0, 0.0, 1.0);
  // Triple scalar product of orthogonal unit vectors = 1
  EXPECT_NEAR(aV1.DotCross(aV2, aV3), 1.0, Precision::Confusion());
}

TEST(gp_VecTest, Magnitude)
{
  gp_Vec aVec(3.0, 4.0, 0.0);
  EXPECT_NEAR(aVec.Magnitude(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aVec.SquareMagnitude(), 25.0, Precision::Confusion());
}

TEST(gp_VecTest, Normalize)
{
  gp_Vec aVec(3.0, 4.0, 0.0);
  gp_Vec aNormalized = aVec.Normalized();
  EXPECT_NEAR(aNormalized.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aNormalized.X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aNormalized.Y(), 0.8, Precision::Confusion());
}

TEST(gp_VecTest, Angle)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  EXPECT_NEAR(aV1.Angle(aV2), M_PI_2, Precision::Angular());
}

TEST(gp_VecTest, IsParallel)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(5.0, 0.0, 0.0);
  EXPECT_TRUE(aV1.IsParallel(aV2, Precision::Angular()));
}

TEST(gp_VecTest, IsNormal)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  EXPECT_TRUE(aV1.IsNormal(aV2, Precision::Angular()));
}

TEST(gp_VecTest, IsOpposite)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(-3.0, 0.0, 0.0);
  EXPECT_TRUE(aV1.IsOpposite(aV2, Precision::Angular()));
}

TEST(gp_VecTest, Reverse)
{
  gp_Vec aVec(1.0, 2.0, 3.0);
  gp_Vec aReversed = aVec.Reversed();
  EXPECT_DOUBLE_EQ(aReversed.X(), -1.0);
  EXPECT_DOUBLE_EQ(aReversed.Y(), -2.0);
  EXPECT_DOUBLE_EQ(aReversed.Z(), -3.0);
}

TEST(gp_VecTest, SetLinearForm)
{
  gp_Vec aV1(1.0, 0.0, 0.0);
  gp_Vec aV2(0.0, 1.0, 0.0);
  gp_Vec aResult;
  aResult.SetLinearForm(2.0, aV1, 3.0, aV2);
  EXPECT_NEAR(aResult.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}
