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

#include <IntPolyh_Point.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(IntPolyh_Point, DefaultConstructor_AllZero)
{
  IntPolyh_Point aPt;
  EXPECT_DOUBLE_EQ(aPt.X(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.U(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.V(), 0.0);
}

TEST(IntPolyh_Point, Divide_NormalDivisor_CorrectResult)
{
  IntPolyh_Point aPt(10.0, 20.0, 30.0, 0.5, 0.8);
  IntPolyh_Point aRes = aPt.Divide(2.0);

  EXPECT_DOUBLE_EQ(aRes.X(), 5.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 10.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), 15.0);
  EXPECT_DOUBLE_EQ(aRes.U(), 0.25);
  EXPECT_DOUBLE_EQ(aRes.V(), 0.4);
}

// Regression test for bug #18: Divide by zero must not crash or print to stdout;
// returns default-initialized (zero) point instead.
TEST(IntPolyh_Point, Divide_ZeroDivisor_ReturnsDefaultPoint)
{
  IntPolyh_Point aPt(10.0, 20.0, 30.0, 0.5, 0.8);
  IntPolyh_Point aRes = aPt.Divide(0.0);

  EXPECT_DOUBLE_EQ(aRes.X(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.U(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.V(), 0.0);
}

// Near-zero divisor (below Precision::Computational) returns default point.
TEST(IntPolyh_Point, Divide_NearZeroDivisor_ReturnsDefaultPoint)
{
  IntPolyh_Point aPt(1.0, 2.0, 3.0, 0.1, 0.2);
  IntPolyh_Point aRes = aPt.Divide(1.0e-20);

  EXPECT_DOUBLE_EQ(aRes.X(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), 0.0);
}

// Negative divisor works correctly.
TEST(IntPolyh_Point, Divide_NegativeDivisor_CorrectResult)
{
  IntPolyh_Point aPt(6.0, -9.0, 12.0, 0.3, 0.6);
  IntPolyh_Point aRes = aPt.Divide(-3.0);

  EXPECT_DOUBLE_EQ(aRes.X(), -2.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 3.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), -4.0);
}

TEST(IntPolyh_Point, Add_CorrectResult)
{
  IntPolyh_Point aP1(1.0, 2.0, 3.0, 0.1, 0.2);
  IntPolyh_Point aP2(4.0, 5.0, 6.0, 0.3, 0.4);
  IntPolyh_Point aRes = aP1.Add(aP2);

  EXPECT_DOUBLE_EQ(aRes.X(), 5.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 7.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), 9.0);
}

TEST(IntPolyh_Point, Sub_CorrectResult)
{
  IntPolyh_Point aP1(5.0, 7.0, 9.0, 0.5, 0.8);
  IntPolyh_Point aP2(1.0, 2.0, 3.0, 0.1, 0.2);
  IntPolyh_Point aRes = aP1.Sub(aP2);

  EXPECT_DOUBLE_EQ(aRes.X(), 4.0);
  EXPECT_DOUBLE_EQ(aRes.Y(), 5.0);
  EXPECT_DOUBLE_EQ(aRes.Z(), 6.0);
}

TEST(IntPolyh_Point, SquareModulus_CorrectResult)
{
  IntPolyh_Point aPt(1.0, 2.0, 3.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(aPt.SquareModulus(), 14.0);
}

TEST(IntPolyh_Point, SquareDistance_SamePoint_Zero)
{
  IntPolyh_Point aPt(3.0, 4.0, 5.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(aPt.SquareDistance(aPt), 0.0);
}

TEST(IntPolyh_Point, Dot_OrthogonalVectors_Zero)
{
  IntPolyh_Point aP1(1.0, 0.0, 0.0, 0.0, 0.0);
  IntPolyh_Point aP2(0.0, 1.0, 0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(aP1.Dot(aP2), 0.0);
}
