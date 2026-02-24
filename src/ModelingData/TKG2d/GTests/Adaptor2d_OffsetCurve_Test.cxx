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

#include <Adaptor2d_Line2d.hxx>
#include <Adaptor2d_OffsetCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class Adaptor2d_OffsetCurveTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Horizontal line through origin, bounds [0, 10]
    myBaseLine = new Adaptor2d_Line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), 0.0, 10.0);
  }

  occ::handle<Adaptor2d_Line2d> myBaseLine;
};

TEST_F(Adaptor2d_OffsetCurveTest, DefaultConstructor)
{
  Adaptor2d_OffsetCurve anOffset;
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 0.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, ConstructWithCurve_OffsetIsZero)
{
  // Constructor with only curve sets offset to 0 and bounds to 0,0
  Adaptor2d_OffsetCurve anOffset(myBaseLine);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 0.0);
  EXPECT_DOUBLE_EQ(anOffset.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anOffset.LastParameter(), 0.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, ConstructWithOffset)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 3.0);
  EXPECT_DOUBLE_EQ(anOffset.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anOffset.LastParameter(), 10.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, ConstructWithOffsetAndBounds)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 2.0, 1.0, 8.0);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 2.0);
  EXPECT_DOUBLE_EQ(anOffset.FirstParameter(), 1.0);
  EXPECT_DOUBLE_EQ(anOffset.LastParameter(), 8.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, Value_PositiveOffset)
{
  // Offsetting a horizontal +X line by +3 shifts Y by -3
  // (OCCT 2D offset uses right-hand normal: Z cross tangent)
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  const gp_Pnt2d        aPnt = anOffset.Value(5.0);

  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), -3.0, Precision::Confusion());
}

TEST_F(Adaptor2d_OffsetCurveTest, Value_NegativeOffset)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, -2.0);
  const gp_Pnt2d        aPnt = anOffset.Value(5.0);

  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 2.0, Precision::Confusion());
}

TEST_F(Adaptor2d_OffsetCurveTest, Value_ZeroOffset_MatchesBaseCurve)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 0.0);

  for (double u = 0.0; u <= 10.0; u += 2.0)
  {
    const gp_Pnt2d aOffPnt  = anOffset.Value(u);
    const gp_Pnt2d aBasePnt = myBaseLine->Value(u);
    EXPECT_TRUE(aOffPnt.IsEqual(aBasePnt, Precision::Confusion()));
  }
}

TEST_F(Adaptor2d_OffsetCurveTest, D1_OffsetLine)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);

  gp_Pnt2d aPnt;
  gp_Vec2d aVec;
  anOffset.D1(5.0, aPnt, aVec);

  // First derivative of an offset line is same as the base line direction
  EXPECT_NEAR(aVec.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVec.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_OffsetCurveTest, Continuity)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  // Offset of CN line is CN
  EXPECT_EQ(anOffset.Continuity(), GeomAbs_CN);
}

TEST_F(Adaptor2d_OffsetCurveTest, NbIntervals)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  EXPECT_EQ(anOffset.NbIntervals(GeomAbs_CN), 1);
}

TEST_F(Adaptor2d_OffsetCurveTest, GetType_Line)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  EXPECT_EQ(anOffset.GetType(), GeomAbs_Line);
}

TEST_F(Adaptor2d_OffsetCurveTest, IsNotClosed)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  EXPECT_FALSE(anOffset.IsClosed());
}

TEST_F(Adaptor2d_OffsetCurveTest, IsNotPeriodic)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  EXPECT_FALSE(anOffset.IsPeriodic());
}

TEST_F(Adaptor2d_OffsetCurveTest, LoadCurve_ResetsOffset)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 5.0);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 5.0);

  // Load a new curve resets offset to 0
  occ::handle<Adaptor2d_Line2d> aNewLine =
    new Adaptor2d_Line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0), 0.0, 5.0);
  anOffset.Load(aNewLine);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 0.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, LoadOffset)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 0.0);

  anOffset.Load(7.5);
  EXPECT_DOUBLE_EQ(anOffset.Offset(), 7.5);
}

TEST_F(Adaptor2d_OffsetCurveTest, LoadOffsetWithBounds)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine);
  anOffset.Load(4.0, 2.0, 6.0);

  EXPECT_DOUBLE_EQ(anOffset.Offset(), 4.0);
  EXPECT_DOUBLE_EQ(anOffset.FirstParameter(), 2.0);
  EXPECT_DOUBLE_EQ(anOffset.LastParameter(), 6.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, Curve_ReturnsBaseCurve)
{
  Adaptor2d_OffsetCurve                 anOffset(myBaseLine, 3.0);
  const occ::handle<Adaptor2d_Curve2d>& aCurve = anOffset.Curve();
  EXPECT_FALSE(aCurve.IsNull());
  EXPECT_EQ(aCurve->GetType(), GeomAbs_Line);
}

TEST_F(Adaptor2d_OffsetCurveTest, Trim)
{
  Adaptor2d_OffsetCurve          anOffset(myBaseLine, 3.0);
  occ::handle<Adaptor2d_Curve2d> aTrimmed = anOffset.Trim(2.0, 7.0, Precision::Confusion());

  EXPECT_DOUBLE_EQ(aTrimmed->FirstParameter(), 2.0);
  EXPECT_DOUBLE_EQ(aTrimmed->LastParameter(), 7.0);
}

TEST_F(Adaptor2d_OffsetCurveTest, ShallowCopy)
{
  Adaptor2d_OffsetCurve          anOffset(myBaseLine, 3.0, 1.0, 9.0);
  occ::handle<Adaptor2d_Curve2d> aCopy = anOffset.ShallowCopy();

  EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), 1.0);
  EXPECT_DOUBLE_EQ(aCopy->LastParameter(), 9.0);

  const gp_Pnt2d aPntOrig = anOffset.Value(5.0);
  const gp_Pnt2d aPntCopy = aCopy->Value(5.0);
  EXPECT_TRUE(aPntOrig.IsEqual(aPntCopy, Precision::Confusion()));
}

TEST_F(Adaptor2d_OffsetCurveTest, Resolution)
{
  Adaptor2d_OffsetCurve anOffset(myBaseLine, 3.0);
  const double          aRes = anOffset.Resolution(0.001);
  EXPECT_GT(aRes, 0.0);
}
