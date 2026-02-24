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
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class Adaptor2d_Line2dTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Line through origin along direction (1, 0) with bounds [0, 10]
    myLine = new Adaptor2d_Line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), 0.0, 10.0);

    // Line through (1, 2) along direction (3, 4)/5 with bounds [-5, 5]
    myDiagLine =
      new Adaptor2d_Line2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(3.0 / 5.0, 4.0 / 5.0), -5.0, 5.0);
  }

  occ::handle<Adaptor2d_Line2d> myLine;
  occ::handle<Adaptor2d_Line2d> myDiagLine;
};

TEST_F(Adaptor2d_Line2dTest, DefaultConstructor)
{
  Adaptor2d_Line2d aLine;
  EXPECT_EQ(aLine.GetType(), GeomAbs_Line);
}

TEST_F(Adaptor2d_Line2dTest, ParameterBounds)
{
  EXPECT_DOUBLE_EQ(myLine->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(myLine->LastParameter(), 10.0);
  EXPECT_DOUBLE_EQ(myDiagLine->FirstParameter(), -5.0);
  EXPECT_DOUBLE_EQ(myDiagLine->LastParameter(), 5.0);
}

TEST_F(Adaptor2d_Line2dTest, Continuity)
{
  EXPECT_EQ(myLine->Continuity(), GeomAbs_CN);
}

TEST_F(Adaptor2d_Line2dTest, NbIntervals)
{
  EXPECT_EQ(myLine->NbIntervals(GeomAbs_C0), 1);
  EXPECT_EQ(myLine->NbIntervals(GeomAbs_CN), 1);
}

TEST_F(Adaptor2d_Line2dTest, Intervals)
{
  NCollection_Array1<double> anIntervals(1, 2);
  myLine->Intervals(anIntervals, GeomAbs_CN);
  EXPECT_DOUBLE_EQ(anIntervals(1), 0.0);
  EXPECT_DOUBLE_EQ(anIntervals(2), 10.0);
}

TEST_F(Adaptor2d_Line2dTest, IsNotClosed)
{
  EXPECT_FALSE(myLine->IsClosed());
}

TEST_F(Adaptor2d_Line2dTest, IsNotPeriodic)
{
  EXPECT_FALSE(myLine->IsPeriodic());
}

TEST_F(Adaptor2d_Line2dTest, GetType)
{
  EXPECT_EQ(myLine->GetType(), GeomAbs_Line);
}

TEST_F(Adaptor2d_Line2dTest, Value_AtOrigin)
{
  const gp_Pnt2d aPnt = myLine->Value(0.0);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, Value_AtParameter)
{
  const gp_Pnt2d aPnt = myLine->Value(5.0);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, Value_DiagonalLine)
{
  // At U=0, point should be origin (1, 2)
  const gp_Pnt2d aPnt0 = myDiagLine->Value(0.0);
  EXPECT_NEAR(aPnt0.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt0.Y(), 2.0, Precision::Confusion());

  // At U=5, point should be (1 + 3, 2 + 4) = (4, 6)
  const gp_Pnt2d aPnt5 = myDiagLine->Value(5.0);
  EXPECT_NEAR(aPnt5.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aPnt5.Y(), 6.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, D0)
{
  gp_Pnt2d aPnt;
  myLine->D0(3.0, aPnt);
  EXPECT_NEAR(aPnt.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, D1_FirstDerivativeIsDirection)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aVec;
  myLine->D1(5.0, aPnt, aVec);

  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aVec.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVec.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, D2_SecondDerivativeIsZero)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1, aV2;
  myLine->D2(5.0, aPnt, aV1, aV2);

  EXPECT_NEAR(aV2.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, D3_ThirdDerivativeIsZero)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1, aV2, aV3;
  myLine->D3(5.0, aPnt, aV1, aV2, aV3);

  EXPECT_NEAR(aV3.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, DN_FirstOrderMatchesD1)
{
  const gp_Vec2d aDN1 = myLine->DN(5.0, 1);
  EXPECT_NEAR(aDN1.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDN1.Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, DN_HigherOrderIsZero)
{
  const gp_Vec2d aDN2 = myLine->DN(5.0, 2);
  EXPECT_NEAR(aDN2.Magnitude(), 0.0, Precision::Confusion());

  const gp_Vec2d aDN3 = myLine->DN(5.0, 3);
  EXPECT_NEAR(aDN3.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, Resolution)
{
  // For a unit-speed line, resolution should equal the input tolerance
  const double aResolution = myLine->Resolution(0.001);
  EXPECT_NEAR(aResolution, 0.001, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, LineGeometry)
{
  const gp_Lin2d aLin = myLine->Line();
  EXPECT_NEAR(aLin.Direction().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aLin.Direction().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aLin.Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aLin.Location().Y(), 0.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, Load_WithLine)
{
  Adaptor2d_Line2d aLine;
  gp_Lin2d         aLin(gp_Pnt2d(1.0, 1.0), gp_Dir2d(0.0, 1.0));
  aLine.Load(aLin);

  const gp_Pnt2d aPnt = aLine.Value(3.0);
  EXPECT_NEAR(aPnt.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 4.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, Load_WithBounds)
{
  Adaptor2d_Line2d aLine;
  gp_Lin2d         aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  aLine.Load(aLin, 2.0, 8.0);

  EXPECT_DOUBLE_EQ(aLine.FirstParameter(), 2.0);
  EXPECT_DOUBLE_EQ(aLine.LastParameter(), 8.0);
}

TEST_F(Adaptor2d_Line2dTest, Trim)
{
  occ::handle<Adaptor2d_Curve2d> aTrimmed = myLine->Trim(2.0, 7.0, Precision::Confusion());
  EXPECT_DOUBLE_EQ(aTrimmed->FirstParameter(), 2.0);
  EXPECT_DOUBLE_EQ(aTrimmed->LastParameter(), 7.0);

  const gp_Pnt2d aPnt = aTrimmed->Value(2.0);
  EXPECT_NEAR(aPnt.X(), 2.0, Precision::Confusion());
}

TEST_F(Adaptor2d_Line2dTest, ShallowCopy)
{
  occ::handle<Adaptor2d_Curve2d> aCopy = myLine->ShallowCopy();
  EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aCopy->LastParameter(), 10.0);
  EXPECT_EQ(aCopy->GetType(), GeomAbs_Line);

  const gp_Pnt2d aPnt = aCopy->Value(5.0);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
}
