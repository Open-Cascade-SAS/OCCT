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

#include <Geom2dEval_CircleInvoluteCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(Geom2dEval_CircleInvoluteCurveTest, Construction_ValidParams)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);
  EXPECT_NEAR(aCurve.Radius(), 1.0, Precision::Confusion());
}

TEST(Geom2dEval_CircleInvoluteCurveTest, Construction_InvalidRadius_Throws)
{
  gp_Ax2d anAx2d;
  EXPECT_THROW(Geom2dEval_CircleInvoluteCurve(anAx2d, 0.0), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_CircleInvoluteCurve(anAx2d, -1.0), Standard_ConstructionError);
}

TEST(Geom2dEval_CircleInvoluteCurveTest, EvalD0_AtT0_OnBaseCircle)
{
  gp_Ax2d anAx2d;
  const double aR = 1.0;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, aR);

  // C(0) = R*(cos(0)+0, sin(0)-0) = (R, 0)
  gp_Pnt2d aP0 = aCurve.EvalD0(0.0);
  EXPECT_NEAR(aP0.X(), aR, Precision::Confusion());
  EXPECT_NEAR(aP0.Y(), 0.0, Precision::Confusion());
}

TEST(Geom2dEval_CircleInvoluteCurveTest, D1_AtT0_IsZero_Cusp)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);

  // D1(0) = R*0*(cos(0), sin(0)) = (0, 0)
  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(0.0);
  EXPECT_NEAR(aD1.D1.Magnitude(), 0.0, Precision::Confusion());
}

TEST(Geom2dEval_CircleInvoluteCurveTest, D1_SpeedLinearInParameter)
{
  gp_Ax2d anAx2d;
  const double aR = 2.0;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, aR);

  // |D1(t)| = R*t
  const double aParams[] = {0.5, 1.0, 2.0, M_PI, 5.0};
  for (double aT : aParams)
  {
    Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
    EXPECT_NEAR(aD1.D1.Magnitude(), aR * aT, Precision::Confusion());
  }
}

TEST(Geom2dEval_CircleInvoluteCurveTest, EvalD1_ConsistentWithD0)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);
  const double aT = 2.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
  gp_Pnt2d aP1 = aCurve.EvalD0(aT + Precision::Confusion());
  gp_Pnt2d aP2 = aCurve.EvalD0(aT - Precision::Confusion());
  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL);
}

TEST(Geom2dEval_CircleInvoluteCurveTest, EvalD2_ConsistentWithD1)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);
  const double aT = 2.0;

  Geom2d_Curve::ResD2 aD2 = aCurve.EvalD2(aT);
  Geom2d_Curve::ResD1 aD1p = aCurve.EvalD1(aT + Precision::Confusion());
  Geom2d_Curve::ResD1 aD1m = aCurve.EvalD1(aT - Precision::Confusion());
  gp_Vec2d aFD((aD1p.D1.XY() - aD1m.D1.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2.Y(), aFD.Y(), THE_FD_TOL);
}

TEST(Geom2dEval_CircleInvoluteCurveTest, Properties)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);
  EXPECT_FALSE(aCurve.IsClosed());
  EXPECT_FALSE(aCurve.IsPeriodic());
  EXPECT_EQ(aCurve.Continuity(), GeomAbs_CN);
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_TRUE(Precision::IsInfinite(aCurve.LastParameter()));
}

TEST(Geom2dEval_CircleInvoluteCurveTest, Copy_Independent)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 3.0);
  occ::handle<Geom2d_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const Geom2dEval_CircleInvoluteCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_CircleInvoluteCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_NEAR(aCopyCurve->Radius(), 3.0, Precision::Confusion());
}

TEST(Geom2dEval_CircleInvoluteCurveTest, DumpJson_NoCrash)
{
  gp_Ax2d anAx2d;
  Geom2dEval_CircleInvoluteCurve aCurve(anAx2d, 1.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}
