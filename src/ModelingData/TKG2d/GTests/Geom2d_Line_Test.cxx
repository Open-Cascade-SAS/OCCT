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

#include <Geom2d_Line.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_LineTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Horizontal line through origin
    myLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  }

  occ::handle<Geom2d_Line> myLine;
};

TEST_F(Geom2d_LineTest, ConstructFromPointAndDir)
{
  EXPECT_NEAR(myLine->Direction().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Direction().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Location().Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, ConstructFromLin2d)
{
  gp_Lin2d                  aLin(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  EXPECT_NEAR(aLine->Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aLine->Direction().Y(), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, ConstructFromAxis)
{
  gp_Ax2d                   anAxis(gp_Pnt2d(3.0, 4.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(anAxis);

  EXPECT_NEAR(aLine->Location().X(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, ParameterBounds)
{
  // OCCT uses large finite bounds (not numeric_limits::max)
  EXPECT_LT(myLine->FirstParameter(), -1e+90);
  EXPECT_GT(myLine->LastParameter(), 1e+90);
}

TEST_F(Geom2d_LineTest, IsNotClosedNotPeriodic)
{
  EXPECT_FALSE(myLine->IsClosed());
  EXPECT_FALSE(myLine->IsPeriodic());
}

TEST_F(Geom2d_LineTest, Continuity)
{
  EXPECT_EQ(myLine->Continuity(), GeomAbs_CN);
  EXPECT_TRUE(myLine->IsCN(100));
}

TEST_F(Geom2d_LineTest, EvalD0)
{
  const gp_Pnt2d aPnt = myLine->EvalD0(5.0);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, EvalD1_DirectionIsConstant)
{
  const auto [aPnt, aD1] = myLine->EvalD1(5.0);
  EXPECT_NEAR(aD1.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, EvalD2_IsZero)
{
  const auto [aPnt, aD1, aD2] = myLine->EvalD2(5.0);
  EXPECT_NEAR(aD2.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, EvalD3_IsZero)
{
  const auto [aPnt, aD1, aD2, aD3] = myLine->EvalD3(5.0);
  EXPECT_NEAR(aD3.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, EvalDN_HigherOrder)
{
  const gp_Vec2d aD4 = myLine->EvalDN(0.0, 4);
  EXPECT_NEAR(aD4.Magnitude(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, Distance)
{
  EXPECT_NEAR(myLine->Distance(gp_Pnt2d(5.0, 3.0)), 3.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Distance(gp_Pnt2d(0.0, 0.0)), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, SetDirection)
{
  myLine->SetDirection(gp_Dir2d(0.0, 1.0));
  EXPECT_NEAR(myLine->Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Direction().Y(), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, SetLocation)
{
  myLine->SetLocation(gp_Pnt2d(3.0, 4.0));
  EXPECT_NEAR(myLine->Location().X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Location().Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, Lin2d)
{
  const gp_Lin2d aLin = myLine->Lin2d();
  EXPECT_NEAR(aLin.Direction().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aLin.Location().X(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, Copy)
{
  occ::handle<Geom2d_Geometry> aCopyGeom = myLine->Copy();
  occ::handle<Geom2d_Line>     aCopy     = occ::down_cast<Geom2d_Line>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->Direction().X(), 1.0, Precision::Confusion());

  aCopy->SetDirection(gp_Dir2d(0.0, 1.0));
  EXPECT_NEAR(myLine->Direction().X(), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, Transform_Translation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(5.0, 10.0));
  myLine->Transform(aTrsf);

  EXPECT_NEAR(myLine->Location().X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(myLine->Location().Y(), 10.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, Transform_Rotation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);
  myLine->Transform(aTrsf);

  // Horizontal line rotated 90 degrees becomes vertical
  EXPECT_NEAR(myLine->Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(std::abs(myLine->Direction().Y()), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_LineTest, ReversedParameter)
{
  EXPECT_DOUBLE_EQ(myLine->ReversedParameter(5.0), -5.0);
}

TEST_F(Geom2d_LineTest, DiagonalLine_Value)
{
  occ::handle<Geom2d_Line> aDiag =
    new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));

  const gp_Pnt2d aPnt  = aDiag->EvalD0(std::sqrt(2.0));
  EXPECT_NEAR(aPnt.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 1.0, Precision::Confusion());
}
