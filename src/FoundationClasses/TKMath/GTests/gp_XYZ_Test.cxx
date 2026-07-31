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

#include <gp_XYZ.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_XYZTest, DefaultConstructor)
{
  gp_XYZ aXYZ;
  EXPECT_DOUBLE_EQ(aXYZ.X(), 0.0);
  EXPECT_DOUBLE_EQ(aXYZ.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aXYZ.Z(), 0.0);
}

TEST(gp_XYZTest, CoordinateConstructor)
{
  gp_XYZ aXYZ(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(aXYZ.X(), 1.0);
  EXPECT_DOUBLE_EQ(aXYZ.Y(), 2.0);
  EXPECT_DOUBLE_EQ(aXYZ.Z(), 3.0);
}

TEST(gp_XYZTest, SetCoord)
{
  gp_XYZ aXYZ;
  aXYZ.SetCoord(7.0, 8.0, 9.0);
  EXPECT_DOUBLE_EQ(aXYZ.X(), 7.0);
  EXPECT_DOUBLE_EQ(aXYZ.Y(), 8.0);
  EXPECT_DOUBLE_EQ(aXYZ.Z(), 9.0);
}

TEST(gp_XYZTest, Accessors)
{
  gp_XYZ aXYZ(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(aXYZ.Coord(1), 1.0);
  EXPECT_DOUBLE_EQ(aXYZ.Coord(2), 2.0);
  EXPECT_DOUBLE_EQ(aXYZ.Coord(3), 3.0);
}

TEST(gp_XYZTest, Add)
{
  gp_XYZ aA(1.0, 2.0, 3.0);
  gp_XYZ aB(4.0, 5.0, 6.0);
  gp_XYZ aSum = aA + aB;
  EXPECT_DOUBLE_EQ(aSum.X(), 5.0);
  EXPECT_DOUBLE_EQ(aSum.Y(), 7.0);
  EXPECT_DOUBLE_EQ(aSum.Z(), 9.0);
}

TEST(gp_XYZTest, Subtract)
{
  gp_XYZ aA(4.0, 5.0, 6.0);
  gp_XYZ aB(1.0, 2.0, 3.0);
  gp_XYZ aDiff = aA - aB;
  EXPECT_DOUBLE_EQ(aDiff.X(), 3.0);
  EXPECT_DOUBLE_EQ(aDiff.Y(), 3.0);
  EXPECT_DOUBLE_EQ(aDiff.Z(), 3.0);
}

TEST(gp_XYZTest, Multiply)
{
  gp_XYZ aXYZ(1.0, 2.0, 3.0);
  gp_XYZ aScaled = aXYZ * 2.0;
  EXPECT_DOUBLE_EQ(aScaled.X(), 2.0);
  EXPECT_DOUBLE_EQ(aScaled.Y(), 4.0);
  EXPECT_DOUBLE_EQ(aScaled.Z(), 6.0);
}

TEST(gp_XYZTest, Divide)
{
  gp_XYZ aXYZ(4.0, 6.0, 8.0);
  gp_XYZ aDivided = aXYZ / 2.0;
  EXPECT_DOUBLE_EQ(aDivided.X(), 2.0);
  EXPECT_DOUBLE_EQ(aDivided.Y(), 3.0);
  EXPECT_DOUBLE_EQ(aDivided.Z(), 4.0);
}

TEST(gp_XYZTest, Cross)
{
  gp_XYZ aA(1.0, 0.0, 0.0);
  gp_XYZ aB(0.0, 1.0, 0.0);
  gp_XYZ aCross = aA.Crossed(aB);
  EXPECT_NEAR(aCross.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Z(), 1.0, Precision::Confusion());
}

TEST(gp_XYZTest, Dot)
{
  gp_XYZ aA(1.0, 2.0, 3.0);
  gp_XYZ aB(4.0, 5.0, 6.0);
  // 1*4 + 2*5 + 3*6 = 32
  EXPECT_DOUBLE_EQ(aA.Dot(aB), 32.0);
}

TEST(gp_XYZTest, CrossMagnitude)
{
  gp_XYZ aA(1.0, 0.0, 0.0);
  gp_XYZ aB(0.0, 1.0, 0.0);
  EXPECT_NEAR(aA.CrossMagnitude(aB), 1.0, Precision::Confusion());
}

TEST(gp_XYZTest, CrossSquareMagnitude)
{
  gp_XYZ aA(1.0, 0.0, 0.0);
  gp_XYZ aB(0.0, 1.0, 0.0);
  EXPECT_NEAR(aA.CrossSquareMagnitude(aB), 1.0, Precision::Confusion());
}

TEST(gp_XYZTest, DotCross)
{
  gp_XYZ aA(1.0, 0.0, 0.0);
  gp_XYZ aB(0.0, 1.0, 0.0);
  gp_XYZ aC(0.0, 0.0, 1.0);
  EXPECT_NEAR(aA.DotCross(aB, aC), 1.0, Precision::Confusion());
}

TEST(gp_XYZTest, Modulus)
{
  gp_XYZ aXYZ(3.0, 4.0, 0.0);
  EXPECT_NEAR(aXYZ.Modulus(), 5.0, Precision::Confusion());
}

TEST(gp_XYZTest, SquareModulus)
{
  gp_XYZ aXYZ(3.0, 4.0, 0.0);
  EXPECT_NEAR(aXYZ.SquareModulus(), 25.0, Precision::Confusion());
}

TEST(gp_XYZTest, SetLinearForm)
{
  gp_XYZ aA(1.0, 0.0, 0.0);
  gp_XYZ aB(0.0, 1.0, 0.0);
  gp_XYZ aResult;
  aResult.SetLinearForm(2.0, aA, 3.0, aB);
  EXPECT_NEAR(aResult.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}

TEST(gp_XYZTest, IsEqual)
{
  gp_XYZ aA(1.0, 2.0, 3.0);
  gp_XYZ aB(1.0, 2.0, 3.0);
  gp_XYZ aC(1.0, 2.0, 4.0);
  EXPECT_TRUE(aA.IsEqual(aB, Precision::Confusion()));
  EXPECT_FALSE(aA.IsEqual(aC, Precision::Confusion()));
}
