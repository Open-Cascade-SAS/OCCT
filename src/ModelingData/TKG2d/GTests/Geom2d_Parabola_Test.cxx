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

#include <Geom2d_Parabola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class Geom2d_ParabolaTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Parabola centered at origin with focal distance 4
    gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 4.0);
    myParabola = new Geom2d_Parabola(aParab);
  }

  occ::handle<Geom2d_Parabola> myParabola;
};

TEST_F(Geom2d_ParabolaTest, Focal)
{
  EXPECT_NEAR(myParabola->Focal(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, IsNotClosedNotPeriodic)
{
  EXPECT_FALSE(myParabola->IsClosed());
  EXPECT_FALSE(myParabola->IsPeriodic());
}

TEST_F(Geom2d_ParabolaTest, Eccentricity)
{
  EXPECT_NEAR(myParabola->Eccentricity(), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, Focus)
{
  const gp_Pnt2d aFocus = myParabola->Focus();
  EXPECT_NEAR(aFocus.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aFocus.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, Parameter)
{
  // p = 2*Focal = 8
  EXPECT_NEAR(myParabola->Parameter(), 8.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, EvalD0_AtZero)
{
  // At U=0: vertex
  const gp_Pnt2d aPnt = myParabola->EvalD0(0.0);
  EXPECT_NEAR(aPnt.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, EvalD0_Symmetric)
{
  // Parabola is symmetric: P(U) and P(-U) should have same X, opposite Y
  const double   aU      = 3.0;
  const gp_Pnt2d aPntPos = myParabola->EvalD0(aU);
  const gp_Pnt2d aPntNeg = myParabola->EvalD0(-aU);

  EXPECT_NEAR(aPntPos.X(), aPntNeg.X(), Precision::Confusion());
  EXPECT_NEAR(aPntPos.Y(), -aPntNeg.Y(), Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, EvalD1_AtZero)
{
  const auto [aPnt, aD1] = myParabola->EvalD1(0.0);
  // At vertex: tangent is along Y axis
  EXPECT_NEAR(aD1.X(), 0.0, Precision::Confusion());
  EXPECT_GT(aD1.Magnitude(), Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, EvalD2_AtZero)
{
  const auto [aPnt, aD1, aD2] = myParabola->EvalD2(0.0);
  // Second derivative at vertex should be along X (opening direction)
  EXPECT_GT(aD2.X(), 0.0);
}

TEST_F(Geom2d_ParabolaTest, PointOnParabola_FocusDirectrixProperty)
{
  // For any point P on parabola: distance(P, Focus) = distance(P, Directrix)
  const gp_Pnt2d aFocus     = myParabola->Focus();
  const gp_Ax2d  aDirectrix = myParabola->Directrix();
  const gp_Lin2d aDirLine(aDirectrix);

  for (double u = -5.0; u <= 5.0; u += 1.0)
  {
    const gp_Pnt2d aPnt       = myParabola->EvalD0(u);
    const double   aDistFocus = aPnt.Distance(aFocus);
    const double   aDistDir   = aDirLine.Distance(aPnt);
    EXPECT_NEAR(aDistFocus, aDistDir, 1e-6);
  }
}

TEST_F(Geom2d_ParabolaTest, SetFocal)
{
  myParabola->SetFocal(10.0);
  EXPECT_NEAR(myParabola->Focal(), 10.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, Copy)
{
  occ::handle<Geom2d_Geometry> aCopyGeom = myParabola->Copy();
  occ::handle<Geom2d_Parabola> aCopy     = occ::down_cast<Geom2d_Parabola>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->Focal(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_ParabolaTest, Transform_Translation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(5.0, 10.0));
  myParabola->Transform(aTrsf);

  EXPECT_NEAR(myParabola->Location().X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(myParabola->Location().Y(), 10.0, Precision::Confusion());
}
