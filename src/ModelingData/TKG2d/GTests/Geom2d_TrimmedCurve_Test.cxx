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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_TrimmedCurveTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Line through origin along X
    myLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));

    // Circle centered at origin, radius 5
    gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
    myCircle = new Geom2d_Circle(aCirc);
  }

  occ::handle<Geom2d_Line>   myLine;
  occ::handle<Geom2d_Circle> myCircle;
};

TEST_F(Geom2d_TrimmedCurveTest, ConstructFromLine)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);

  EXPECT_DOUBLE_EQ(aTrim->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aTrim->LastParameter(), 10.0);
}

TEST_F(Geom2d_TrimmedCurveTest, ConstructFromCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);

  EXPECT_DOUBLE_EQ(aTrim->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aTrim->LastParameter(), M_PI);
}

TEST_F(Geom2d_TrimmedCurveTest, StartAndEndPoints)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 2.0, 8.0);

  const gp_Pnt2d aStart = aTrim->StartPoint();
  const gp_Pnt2d anEnd  = aTrim->EndPoint();

  EXPECT_NEAR(aStart.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(anEnd.X(), 8.0, Precision::Confusion());
}

TEST_F(Geom2d_TrimmedCurveTest, BasisCurve)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);

  occ::handle<Geom2d_Curve> aBasis = aTrim->BasisCurve();
  EXPECT_FALSE(aBasis.IsNull());
}

TEST_F(Geom2d_TrimmedCurveTest, EvalD0_MatchesBasisCurve)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);

  for (double u = 0.0; u <= M_PI; u += M_PI / 4.0)
  {
    const gp_Pnt2d aTrimPnt = aTrim->EvalD0(u);
    const gp_Pnt2d aBasePnt = myCircle->EvalD0(u);
    EXPECT_TRUE(aTrimPnt.IsEqual(aBasePnt, Precision::Confusion()));
  }
}

TEST_F(Geom2d_TrimmedCurveTest, EvalD1_MatchesBasisCurve)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);

  const auto [aTrimPnt, aTrimD1] = aTrim->EvalD1(5.0);
  const auto [aBasePnt, aBaseD1] = myLine->EvalD1(5.0);

  EXPECT_TRUE(aTrimPnt.IsEqual(aBasePnt, Precision::Confusion()));
  EXPECT_NEAR(aTrimD1.X(), aBaseD1.X(), Precision::Confusion());
  EXPECT_NEAR(aTrimD1.Y(), aBaseD1.Y(), Precision::Confusion());
}

TEST_F(Geom2d_TrimmedCurveTest, Continuity)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);
  EXPECT_EQ(aTrim->Continuity(), GeomAbs_CN);
}

TEST_F(Geom2d_TrimmedCurveTest, IsCN)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);
  EXPECT_TRUE(aTrim->IsCN(10));
}

TEST_F(Geom2d_TrimmedCurveTest, IsClosed_OpenArc)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);
  EXPECT_FALSE(aTrim->IsClosed());
}

TEST_F(Geom2d_TrimmedCurveTest, IsClosed_FullCircle)
{
  // Full-period trim on a periodic curve should be reported as closed
  occ::handle<Geom2d_TrimmedCurve> aTrim =
    new Geom2d_TrimmedCurve(myCircle, 0.0, 2.0 * M_PI);

  EXPECT_TRUE(aTrim->IsClosed());
}

TEST_F(Geom2d_TrimmedCurveTest, IsPeriodic_PartialArc)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);
  EXPECT_FALSE(aTrim->IsPeriodic());
}

TEST_F(Geom2d_TrimmedCurveTest, IsPeriodic_FullPeriod)
{
  // Full-period trim on a periodic curve should be reported as periodic
  occ::handle<Geom2d_TrimmedCurve> aTrim =
    new Geom2d_TrimmedCurve(myCircle, 0.0, 2.0 * M_PI);
  EXPECT_TRUE(aTrim->IsPeriodic());
}

TEST_F(Geom2d_TrimmedCurveTest, IsPeriodic_Line)
{
  // Line is not periodic, so trimmed line should not be periodic
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);
  EXPECT_FALSE(aTrim->IsPeriodic());
}

TEST_F(Geom2d_TrimmedCurveTest, SetTrim)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);
  aTrim->SetTrim(3.0, 7.0);

  EXPECT_DOUBLE_EQ(aTrim->FirstParameter(), 3.0);
  EXPECT_DOUBLE_EQ(aTrim->LastParameter(), 7.0);
}

TEST_F(Geom2d_TrimmedCurveTest, Copy)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI);
  occ::handle<Geom2d_Geometry>     aCopyGeom = aTrim->Copy();
  occ::handle<Geom2d_TrimmedCurve> aCopy = occ::down_cast<Geom2d_TrimmedCurve>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aCopy->LastParameter(), M_PI);
}

TEST_F(Geom2d_TrimmedCurveTest, Transform_Translation)
{
  occ::handle<Geom2d_TrimmedCurve> aTrim = new Geom2d_TrimmedCurve(myLine, 0.0, 10.0);

  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(5.0, 5.0));
  aTrim->Transform(aTrsf);

  const gp_Pnt2d aStart = aTrim->StartPoint();
  EXPECT_NEAR(aStart.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aStart.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_TrimmedCurveTest, CircleArc_StartEndPoints)
{
  // Quarter circle arc
  occ::handle<Geom2d_TrimmedCurve> aTrim =
    new Geom2d_TrimmedCurve(myCircle, 0.0, M_PI / 2.0);

  const gp_Pnt2d aStart = aTrim->StartPoint();
  const gp_Pnt2d anEnd  = aTrim->EndPoint();

  EXPECT_NEAR(aStart.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aStart.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anEnd.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anEnd.Y(), 5.0, Precision::Confusion());
}
