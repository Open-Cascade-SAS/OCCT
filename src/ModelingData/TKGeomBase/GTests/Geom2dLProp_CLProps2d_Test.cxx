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
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <LProp_NotDefined.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2dLProp_CLProps2dTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Circle centered at origin, radius 5
    gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
    myCircle = new Geom2d_Circle(aCirc);

    // Line through origin along X
    gp_Lin2d aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
    myLine = new Geom2d_Line(aLin);

    // Ellipse centered at origin, major radius 10, minor radius 5
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
    myEllipse = new Geom2d_Ellipse(anElips);
  }

  occ::handle<Geom2d_Circle>  myCircle;
  occ::handle<Geom2d_Line>    myLine;
  occ::handle<Geom2d_Ellipse> myEllipse;
};

TEST_F(Geom2dLProp_CLProps2dTest, Circle_Value)
{
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());

  const gp_Pnt2d& aPnt = aProps.Value();
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_SetParameter)
{
  Geom2dLProp_CLProps2d aProps(myCircle, 2, Precision::Confusion());

  aProps.SetParameter(M_PI / 2.0);
  const gp_Pnt2d& aPnt = aProps.Value();
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_TangentDefined)
{
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());
  EXPECT_TRUE(aProps.IsTangentDefined());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_TangentAtZero)
{
  // At U=0 on a circle, the tangent should point in +Y direction
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());

  gp_Dir2d aTangent;
  aProps.Tangent(aTangent);
  EXPECT_NEAR(aTangent.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTangent.Y(), 1.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_Curvature)
{
  // Curvature of a circle of radius R is 1/R
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());

  const double aCurvature = aProps.Curvature();
  EXPECT_NEAR(aCurvature, 1.0 / 5.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_CurvatureConstant)
{
  // Curvature on a circle should be the same at every point
  Geom2dLProp_CLProps2d aProps(myCircle, 2, Precision::Confusion());

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    aProps.SetParameter(u);
    EXPECT_NEAR(aProps.Curvature(), 1.0 / 5.0, Precision::Confusion());
  }
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_Normal)
{
  // Normal at U=0 on a circle centered at origin should point inward (-X)
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());

  gp_Dir2d aNormal;
  aProps.Normal(aNormal);
  // Normal should be perpendicular to tangent
  gp_Dir2d aTangent;
  aProps.Tangent(aTangent);
  EXPECT_NEAR(std::abs(aTangent.X() * aNormal.X() + aTangent.Y() * aNormal.Y()),
              0.0,
              Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_CentreOfCurvature)
{
  // Centre of curvature of a circle is its center
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());

  gp_Pnt2d aCenter;
  aProps.CentreOfCurvature(aCenter);
  EXPECT_NEAR(aCenter.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Circle_CentreOfCurvature_AtPiHalf)
{
  Geom2dLProp_CLProps2d aProps(myCircle, M_PI / 2.0, 2, Precision::Confusion());

  gp_Pnt2d aCenter;
  aProps.CentreOfCurvature(aCenter);
  EXPECT_NEAR(aCenter.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Line_TangentDefined)
{
  Geom2dLProp_CLProps2d aProps(myLine, 0.0, 2, Precision::Confusion());
  EXPECT_TRUE(aProps.IsTangentDefined());
}

TEST_F(Geom2dLProp_CLProps2dTest, Line_Tangent)
{
  Geom2dLProp_CLProps2d aProps(myLine, 5.0, 2, Precision::Confusion());

  gp_Dir2d aTangent;
  aProps.Tangent(aTangent);
  EXPECT_NEAR(aTangent.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTangent.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Line_CurvatureIsZero)
{
  Geom2dLProp_CLProps2d aProps(myLine, 5.0, 2, Precision::Confusion());
  const double          aCurvature = aProps.Curvature();
  EXPECT_NEAR(aCurvature, 0.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, Line_CentreOfCurvature_Throws)
{
  // Centre of curvature is not defined for a line (zero curvature)
  Geom2dLProp_CLProps2d aProps(myLine, 5.0, 2, Precision::Confusion());
  gp_Pnt2d              aCenter;
  EXPECT_THROW(aProps.CentreOfCurvature(aCenter), LProp_NotDefined);
}

TEST_F(Geom2dLProp_CLProps2dTest, Ellipse_CurvatureAtMajorVertex)
{
  // At U=0, ellipse is at major vertex. Curvature = b^2/a^3 for a=10, b=5
  // but actually for parametric ellipse, curvature at end of major axis = b^2/(a^2) * (1/a)
  // = b^2/a^2 * 1/a ... let's compute it properly:
  // Curvature at parameter 0 for ellipse(a,b) = a/b^2
  // Actually: radius of curvature at major vertex = b^2/a, so curvature = a/b^2
  Geom2dLProp_CLProps2d aProps(myEllipse, 0.0, 2, Precision::Confusion());

  const double aCurvature = aProps.Curvature();
  const double aExpected  = 10.0 / (5.0 * 5.0); // a/b^2 = 10/25 = 0.4
  EXPECT_NEAR(aCurvature, aExpected, 1e-6);
}

TEST_F(Geom2dLProp_CLProps2dTest, Ellipse_CurvatureAtMinorVertex)
{
  // At U=PI/2, ellipse is at minor vertex. Curvature = b/a^2
  Geom2dLProp_CLProps2d aProps(myEllipse, M_PI / 2.0, 2, Precision::Confusion());

  const double aCurvature = aProps.Curvature();
  const double aExpected  = 5.0 / (10.0 * 10.0); // b/a^2 = 5/100 = 0.05
  EXPECT_NEAR(aCurvature, aExpected, 1e-6);
}

TEST_F(Geom2dLProp_CLProps2dTest, SetCurve)
{
  Geom2dLProp_CLProps2d<> aProps(myEllipse, 0.0, 2, Precision::Confusion());
  aProps.SetCurve(myCircle);
  aProps.SetParameter(0.0);

  const gp_Pnt2d& aPnt = aProps.Value();
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, D1_Circle)
{
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());
  const gp_Vec2d&       aD1 = aProps.D1();

  // First derivative at U=0 on circle(R=5) is (0, 5)
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2dLProp_CLProps2dTest, D2_Circle)
{
  Geom2dLProp_CLProps2d aProps(myCircle, 0.0, 2, Precision::Confusion());
  const gp_Vec2d&       aD2 = aProps.D2();

  // Second derivative at U=0 on circle(R=5) is (-5, 0)
  EXPECT_NEAR(aD2.X(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aD2.Y(), 0.0, Precision::Confusion());
}
