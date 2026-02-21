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

#include <Geom2dEval_LogarithmicSpiralCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(Geom2dEval_LogarithmicSpiralCurveTest, Construction_ValidParams)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.2);
  EXPECT_NEAR(aCurve.Scale(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.GrowthExponent(), 0.2, Precision::Confusion());
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, Construction_InvalidParams_Throws)
{
  gp_Ax2d anAx2d;
  EXPECT_THROW(Geom2dEval_LogarithmicSpiralCurve(anAx2d, 0.0, 0.2), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_LogarithmicSpiralCurve(anAx2d, -1.0, 0.2), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_LogarithmicSpiralCurve(anAx2d, 1.0, 0.0), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_LogarithmicSpiralCurve(anAx2d, 1.0, -0.2), Standard_ConstructionError);
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, SelfSimilarity)
{
  gp_Ax2d anAx2d;
  const double aA = 1.0, aB = 0.2;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, aA, aB);

  // C(t+k) should be a scaled version of C(t) by factor e^(b*k)
  const double aK = 2.0;
  const double aScaleFactor = std::exp(aB * aK);
  const gp_Pnt2d anO = anAx2d.Location();

  const double aParams[] = {0.0, 1.0, 2.0, M_PI};
  for (double aT : aParams)
  {
    gp_Pnt2d aP1 = aCurve.EvalD0(aT);
    gp_Pnt2d aP2 = aCurve.EvalD0(aT + aK);

    // |P2 - O| / |P1 - O| should equal e^(b*k)
    const double aDist1 = anO.Distance(aP1);
    const double aDist2 = anO.Distance(aP2);
    if (aDist1 > Precision::Confusion())
    {
      EXPECT_NEAR(aDist2 / aDist1, aScaleFactor, 1e-10);
    }
  }
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, ConstantAngle)
{
  gp_Ax2d anAx2d;
  const double aA = 1.0, aB = 0.2;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, aA, aB);

  // Angle between tangent and radial direction = atan(1/b)
  const double aExpAngle = std::atan(1.0 / aB);
  const gp_Pnt2d anO = anAx2d.Location();

  const double aParams[] = {1.0, 2.0, M_PI, 5.0};
  for (double aT : aParams)
  {
    Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
    gp_Vec2d aRadial(anO, aD1.Point);
    const double aDot = aD1.D1.Dot(aRadial);
    const double aCross = aD1.D1.Crossed(aRadial);
    const double anAngle = std::abs(std::atan2(std::abs(aCross), aDot));
    EXPECT_NEAR(anAngle, aExpAngle, 1e-10);
  }
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, EvalD1_ConsistentWithD0)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.3);
  const double aT = 2.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
  gp_Pnt2d aP1 = aCurve.EvalD0(aT + Precision::Confusion());
  gp_Pnt2d aP2 = aCurve.EvalD0(aT - Precision::Confusion());
  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL);
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, Properties)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.2);
  EXPECT_FALSE(aCurve.IsClosed());
  EXPECT_FALSE(aCurve.IsPeriodic());
  EXPECT_EQ(aCurve.Continuity(), GeomAbs_CN);
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, Reverse_NotImplemented)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.2);
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, Copy_Independent)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.2);
  occ::handle<Geom2d_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const Geom2dEval_LogarithmicSpiralCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_LogarithmicSpiralCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_NEAR(aCopyCurve->Scale(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCopyCurve->GrowthExponent(), 0.2, Precision::Confusion());
}

TEST(Geom2dEval_LogarithmicSpiralCurveTest, DumpJson_NoCrash)
{
  gp_Ax2d anAx2d;
  Geom2dEval_LogarithmicSpiralCurve aCurve(anAx2d, 1.0, 0.2);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}
