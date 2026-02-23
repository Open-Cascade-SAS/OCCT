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

#include <Geom2d_Ellipse.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_EllipseTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Ellipse centered at origin, major radius 10, minor radius 5
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
    myEllipse = new Geom2d_Ellipse(anElips);
  }

  occ::handle<Geom2d_Ellipse> myEllipse;
};

TEST_F(Geom2d_EllipseTest, Radii)
{
  EXPECT_NEAR(myEllipse->MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(myEllipse->MinorRadius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, ParameterBounds)
{
  EXPECT_DOUBLE_EQ(myEllipse->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(myEllipse->LastParameter(), 2.0 * M_PI);
}

TEST_F(Geom2d_EllipseTest, IsClosedAndPeriodic)
{
  EXPECT_TRUE(myEllipse->IsClosed());
  EXPECT_TRUE(myEllipse->IsPeriodic());
}

TEST_F(Geom2d_EllipseTest, Eccentricity)
{
  // e = sqrt(1 - (b/a)^2) = sqrt(1 - 0.25) = sqrt(0.75)
  const double anExpected = std::sqrt(1.0 - (5.0 * 5.0) / (10.0 * 10.0));
  EXPECT_NEAR(myEllipse->Eccentricity(), anExpected, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, Focal)
{
  // Focal distance = sqrt(a^2 - b^2) * 2
  const double anExpected = 2.0 * std::sqrt(10.0 * 10.0 - 5.0 * 5.0);
  EXPECT_NEAR(myEllipse->Focal(), anExpected, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, Foci)
{
  const gp_Pnt2d aFocus1 = myEllipse->Focus1();
  const gp_Pnt2d aFocus2 = myEllipse->Focus2();
  const double   c       = std::sqrt(100.0 - 25.0); // sqrt(a^2 - b^2)

  EXPECT_NEAR(aFocus1.X(), c, Precision::Confusion());
  EXPECT_NEAR(aFocus1.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aFocus2.X(), -c, Precision::Confusion());
  EXPECT_NEAR(aFocus2.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, Parameter)
{
  // Semi-latus rectum = b^2/a = 25/10 = 2.5
  EXPECT_NEAR(myEllipse->Parameter(), 2.5, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, EvalD0_MajorVertex)
{
  // At U=0: point on major axis = (a, 0) = (10, 0)
  const gp_Pnt2d aPnt = myEllipse->EvalD0(0.0);
  EXPECT_NEAR(aPnt.X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, EvalD0_MinorVertex)
{
  // At U=PI/2: point on minor axis = (0, b) = (0, 5)
  const gp_Pnt2d aPnt = myEllipse->EvalD0(M_PI / 2.0);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, EvalD1_AtZero)
{
  const auto [aPnt, aD1] = myEllipse->EvalD1(0.0);
  // At U=0: tangent is (0, b) = (0, 5)
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, EvalD2_AtZero)
{
  const auto [aPnt, aD1, aD2] = myEllipse->EvalD2(0.0);
  // At U=0: second derivative is (-a, 0) = (-10, 0)
  EXPECT_NEAR(aD2.X(), -10.0, Precision::Confusion());
  EXPECT_NEAR(aD2.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, SetMajorRadius)
{
  myEllipse->SetMajorRadius(20.0);
  EXPECT_NEAR(myEllipse->MajorRadius(), 20.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, SetMinorRadius)
{
  myEllipse->SetMinorRadius(3.0);
  EXPECT_NEAR(myEllipse->MinorRadius(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, PointOnEllipse_FociDistanceSum)
{
  // For any point on ellipse: d(P, F1) + d(P, F2) = 2a
  const gp_Pnt2d aF1 = myEllipse->Focus1();
  const gp_Pnt2d aF2 = myEllipse->Focus2();

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    const gp_Pnt2d aPnt  = myEllipse->EvalD0(u);
    const double   aSum  = aPnt.Distance(aF1) + aPnt.Distance(aF2);
    EXPECT_NEAR(aSum, 2.0 * 10.0, Precision::Confusion());
  }
}

TEST_F(Geom2d_EllipseTest, Copy)
{
  occ::handle<Geom2d_Geometry> aCopyGeom = myEllipse->Copy();
  occ::handle<Geom2d_Ellipse>  aCopy     = occ::down_cast<Geom2d_Ellipse>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aCopy->MinorRadius(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, Transform_Translation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(5.0, 10.0));
  myEllipse->Transform(aTrsf);

  EXPECT_NEAR(myEllipse->Location().X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(myEllipse->Location().Y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(myEllipse->MajorRadius(), 10.0, Precision::Confusion());
}

TEST_F(Geom2d_EllipseTest, ReversedParameter)
{
  const double aU = M_PI / 4.0;
  EXPECT_NEAR(myEllipse->ReversedParameter(aU), 2.0 * M_PI - aU, Precision::Confusion());
}
