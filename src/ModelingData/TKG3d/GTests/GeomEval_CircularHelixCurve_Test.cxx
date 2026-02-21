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

#include <Geom_Circle.hxx>
#include <GeomEval_CircularHelixCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(GeomEval_CircularHelixCurveTest, Construction_ValidParams)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  EXPECT_NEAR(aCurve.Radius(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Pitch(), 10.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, Construction_InvalidRadius_Throws)
{
  gp_Ax2 anAx2;
  EXPECT_THROW(GeomEval_CircularHelixCurve(anAx2, 0.0, 10.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_CircularHelixCurve(anAx2, -1.0, 10.0), Standard_ConstructionError);
}

TEST(GeomEval_CircularHelixCurveTest, Construction_NegativePitch_NoThrow)
{
  gp_Ax2 anAx2;
  EXPECT_NO_THROW(GeomEval_CircularHelixCurve(anAx2, 5.0, -10.0));
}

TEST(GeomEval_CircularHelixCurveTest, EvalD0_AtKnownPoints)
{
  gp_Ax2 anAx2;
  const double aR = 5.0, aP = 10.0;
  GeomEval_CircularHelixCurve aCurve(anAx2, aR, aP);

  // t=0: C(0) = (R, 0, 0)
  gp_Pnt aP0 = aCurve.EvalD0(0.0);
  EXPECT_NEAR(aP0.X(), aR, Precision::Confusion());
  EXPECT_NEAR(aP0.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP0.Z(), 0.0, Precision::Confusion());

  // t=Pi/2: C = (0, R, P/4)
  gp_Pnt aP1 = aCurve.EvalD0(M_PI / 2.0);
  EXPECT_NEAR(aP1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP1.Y(), aR, Precision::Confusion());
  EXPECT_NEAR(aP1.Z(), aP / 4.0, Precision::Confusion());

  // t=Pi: C = (-R, 0, P/2)
  gp_Pnt aP2 = aCurve.EvalD0(M_PI);
  EXPECT_NEAR(aP2.X(), -aR, Precision::Confusion());
  EXPECT_NEAR(aP2.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP2.Z(), aP / 2.0, Precision::Confusion());

  // t=2*Pi: C = (R, 0, P) -- one full turn
  gp_Pnt aP3 = aCurve.EvalD0(2.0 * M_PI);
  EXPECT_NEAR(aP3.X(), aR, Precision::Confusion());
  EXPECT_NEAR(aP3.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP3.Z(), aP, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, EvalD1_ConstantSpeed)
{
  gp_Ax2 anAx2;
  const double aR = 5.0, aP = 10.0;
  GeomEval_CircularHelixCurve aCurve(anAx2, aR, aP);

  // |D1| should be constant = sqrt(R^2 + (P/(2*Pi))^2)
  const double aZRate = aP / (2.0 * M_PI);
  const double aExpSpeed = std::sqrt(aR * aR + aZRate * aZRate);

  const double aParams[] = {0.0, 0.5, 1.0, M_PI, 2.0 * M_PI};
  for (double aT : aParams)
  {
    Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
    EXPECT_NEAR(aD1.D1.Magnitude(), aExpSpeed, Precision::Confusion());
  }
}

TEST(GeomEval_CircularHelixCurveTest, EvalD1_ConsistentWithD0)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  const double aT = 1.5;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
  gp_Pnt aP1 = aCurve.EvalD0(aT + Precision::Confusion());
  gp_Pnt aP2 = aCurve.EvalD0(aT - Precision::Confusion());
  gp_Vec aFD((aP1.XYZ() - aP2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Z(), aFD.Z(), THE_FD_TOL);
}

TEST(GeomEval_CircularHelixCurveTest, D1_D2_Orthogonal)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);

  // D2 has no Z component, so D1.D2 = R^2*(-sin*(-cos) + cos*(-sin)) = 0
  const double aParams[] = {0.0, 1.0, M_PI, 3.5};
  for (double aT : aParams)
  {
    Geom_Curve::ResD2 aD2 = aCurve.EvalD2(aT);
    // D2 magnitude should be R
    EXPECT_NEAR(aD2.D2.Magnitude(), 5.0, Precision::Confusion());
  }
}

TEST(GeomEval_CircularHelixCurveTest, ComparisonWithCircle_ZeroPitch)
{
  gp_Ax2 anAx2;
  const double aR = 5.0;
  GeomEval_CircularHelixCurve aHelix(anAx2, aR, 0.0);
  Handle(Geom_Circle) aCircle = new Geom_Circle(anAx2, aR);

  const double aParams[] = {0.0, M_PI / 4.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  for (double aT : aParams)
  {
    gp_Pnt aPH = aHelix.EvalD0(aT);
    gp_Pnt aPC = aCircle->EvalD0(aT);
    EXPECT_NEAR(aPH.X(), aPC.X(), Precision::Confusion());
    EXPECT_NEAR(aPH.Y(), aPC.Y(), Precision::Confusion());
    EXPECT_NEAR(aPH.Z(), aPC.Z(), Precision::Confusion());

    Geom_Curve::ResD1 aDH1 = aHelix.EvalD1(aT);
    Geom_Curve::ResD1 aDC1 = aCircle->EvalD1(aT);
    EXPECT_NEAR(aDH1.D1.X(), aDC1.D1.X(), Precision::Angular());
    EXPECT_NEAR(aDH1.D1.Y(), aDC1.D1.Y(), Precision::Angular());
    EXPECT_NEAR(aDH1.D1.Z(), aDC1.D1.Z(), Precision::Angular());

    Geom_Curve::ResD2 aDH2 = aHelix.EvalD2(aT);
    Geom_Curve::ResD2 aDC2 = aCircle->EvalD2(aT);
    EXPECT_NEAR(aDH2.D2.X(), aDC2.D2.X(), Precision::Angular());
    EXPECT_NEAR(aDH2.D2.Y(), aDC2.D2.Y(), Precision::Angular());
    EXPECT_NEAR(aDH2.D2.Z(), aDC2.D2.Z(), Precision::Angular());

    Geom_Curve::ResD3 aDH3 = aHelix.EvalD3(aT);
    Geom_Curve::ResD3 aDC3 = aCircle->EvalD3(aT);
    EXPECT_NEAR(aDH3.D3.X(), aDC3.D3.X(), Precision::Angular());
    EXPECT_NEAR(aDH3.D3.Y(), aDC3.D3.Y(), Precision::Angular());
    EXPECT_NEAR(aDH3.D3.Z(), aDC3.D3.Z(), Precision::Angular());
  }
}

TEST(GeomEval_CircularHelixCurveTest, EvalDN_CyclesPeriod4)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  const double aT = 1.0;

  // DN(t,4) XY-components should equal DN(t,0) XY = D0 XY contribution
  // More specifically: DN cycles with period 4 in XY
  gp_Vec aD1 = aCurve.EvalDN(aT, 1);
  gp_Vec aD5 = aCurve.EvalDN(aT, 5);
  EXPECT_NEAR(aD1.X(), aD5.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.Y(), aD5.Y(), Precision::Confusion());
  // Z: D1 has Z component, D5 does not
  EXPECT_NEAR(aD5.Z(), 0.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, Transform_PreservesEvaluation)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  gp_Pnt aPBefore = aCurve.EvalD0(1.0);
  aPBefore.Transform(aTrsf);
  aCurve.Transform(aTrsf);
  gp_Pnt aPAfter = aCurve.EvalD0(1.0);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, Copy_Independent)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  occ::handle<Geom_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_CircularHelixCurve* aCopyCurve =
    dynamic_cast<const GeomEval_CircularHelixCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_NEAR(aCopyCurve->Radius(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCopyCurve->Pitch(), 10.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, Reverse_NegatesPitch)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  aCurve.Reverse();
  EXPECT_NEAR(aCurve.Pitch(), -10.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.ReversedParameter(3.0), -3.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelixCurveTest, Properties)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  EXPECT_FALSE(aCurve.IsClosed());
  EXPECT_FALSE(aCurve.IsPeriodic());
  EXPECT_EQ(aCurve.Continuity(), GeomAbs_CN);
  EXPECT_TRUE(aCurve.IsCN(100));
}

TEST(GeomEval_CircularHelixCurveTest, DumpJson_NoCrash)
{
  gp_Ax2 anAx2;
  GeomEval_CircularHelixCurve aCurve(anAx2, 5.0, 10.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}
