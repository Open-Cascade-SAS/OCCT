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
#include <Geom_Ellipse.hxx>
#include <GeomEval_TBezierCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{

constexpr double THE_FD_TOL_D1 = 1e-5;
constexpr double THE_FD_TOL_D2 = 1e-4;

// Helper to create a T-Bezier that represents a unit semicircle in XY plane.
// C(t) = P_0 + P_1*sin(t) + P_2*cos(t) = (cos(t), sin(t), 0), t in [0, Pi].
GeomEval_TBezierCurve createSemicircle()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(1.0, 0.0, 0.0));
  return GeomEval_TBezierCurve(aPoles, 1.0);
}

// Helper to create a T-Bezier that represents a semi-ellipse (a=3, b=2) in XY plane.
// C(t) = P_0 + P_1*sin(t) + P_2*cos(t) = (3*cos(t), 2*sin(t), 0), t in [0, Pi].
GeomEval_TBezierCurve createSemiEllipse()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(0.0, 2.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(3.0, 0.0, 0.0));
  return GeomEval_TBezierCurve(aPoles, 1.0);
}

// Helper to create a simple 3-pole (order 1) T-Bezier curve
GeomEval_TBezierCurve createSimpleCurve()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  return GeomEval_TBezierCurve(aPoles, 1.0);
}

} // namespace

// Test construction with valid parameters
TEST(GeomEval_TBezierCurveTest, Construction_ValidParams)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.Order(), 1);
  EXPECT_NEAR(aCurve.Alpha(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction throws for invalid parameters
TEST(GeomEval_TBezierCurveTest, Construction_InvalidParams_Throws)
{
  // Even number of poles (not odd)
  NCollection_Array1<gp_Pnt> aPoles2(1, 2);
  aPoles2.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles2.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  EXPECT_THROW(GeomEval_TBezierCurve(aPoles2, 1.0), Standard_ConstructionError);

  // Invalid alpha
  NCollection_Array1<gp_Pnt> aPoles3(1, 3);
  aPoles3.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles3.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  aPoles3.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  EXPECT_THROW(GeomEval_TBezierCurve(aPoles3, 0.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_TBezierCurve(aPoles3, -1.0), Standard_ConstructionError);
}

// Test EvalD0 at endpoints matches StartPoint/EndPoint
TEST(GeomEval_TBezierCurveTest, EvalD0_Endpoints)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();

  gp_Pnt aStart = aCurve.EvalD0(aCurve.FirstParameter());
  gp_Pnt aEnd   = aCurve.EvalD0(aCurve.LastParameter());

  EXPECT_NEAR(aStart.Distance(aCurve.StartPoint()), 0.0, Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(aCurve.EndPoint()), 0.0, Precision::Confusion());
}

// Test parameter range
TEST(GeomEval_TBezierCurveTest, ParameterRange)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.LastParameter(), M_PI / 1.0, Precision::Confusion()); // Pi/alpha
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_TBezierCurveTest, EvalD1_ConsistentWithD0)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  const double          aU     = M_PI / 3.0;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec aFD((aP1.XYZ() - aP2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Z(), aFD.Z(), THE_FD_TOL_D1);
}

// Test periodicity and closure
TEST(GeomEval_TBezierCurveTest, PeriodicityAndClosure)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_FALSE(aCurve.IsPeriodic());
}

TEST(GeomEval_TBezierCurveTest, Reverse_NotImplemented)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

// Test rational construction
TEST(GeomEval_TBezierCurveTest, Construction_Rational)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 2.0);
  aWeights.SetValue(3, 1.0);
  GeomEval_TBezierCurve aCurve(aPoles, aWeights, 1.0);
  EXPECT_TRUE(aCurve.IsRational());
  EXPECT_EQ(aCurve.Weights().Size(), 3);
}

// Test Transform/Transformed are not implemented.
TEST(GeomEval_TBezierCurveTest, Transform_NotImplemented)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  gp_Trsf               aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  EXPECT_THROW(aCurve.Transform(aTrsf), Standard_NotImplemented);
  EXPECT_THROW((void)aCurve.Transformed(aTrsf), Standard_NotImplemented);
}

// Test Copy produces independent identical object
TEST(GeomEval_TBezierCurveTest, Copy_Independent)
{
  GeomEval_TBezierCurve      aCurve = createSimpleCurve();
  occ::handle<Geom_Geometry> aCopy  = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_TBezierCurve* aCopyCurve = dynamic_cast<const GeomEval_TBezierCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_EQ(aCopyCurve->NbPoles(), 3);
  EXPECT_NEAR(aCopyCurve->Alpha(), 1.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(GeomEval_TBezierCurveTest, DumpJson_NoCrash)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  Standard_SStream      aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test EvalD2 derivative D2 matches finite differences of D1
TEST(GeomEval_TBezierCurveTest, EvalD2_ConsistentWithD1)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  const double          aU     = M_PI / 4.0;

  Geom_Curve::ResD2 aD2      = aCurve.EvalD2(aU);
  Geom_Curve::ResD1 aD1Plus  = aCurve.EvalD1(aU + Precision::Confusion());
  Geom_Curve::ResD1 aD1Minus = aCurve.EvalD1(aU - Precision::Confusion());

  gp_Vec aFD((aD1Plus.D1.XYZ() - aD1Minus.D1.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Y(), aFD.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Z(), aFD.Z(), THE_FD_TOL_D2);
}

// Test EvalD3 derivative D3 matches finite differences of D2
TEST(GeomEval_TBezierCurveTest, EvalD3_ConsistentWithD2)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  const double          aU     = M_PI / 3.0;

  Geom_Curve::ResD3 aD3      = aCurve.EvalD3(aU);
  Geom_Curve::ResD2 aD2Plus  = aCurve.EvalD2(aU + Precision::Confusion());
  Geom_Curve::ResD2 aD2Minus = aCurve.EvalD2(aU - Precision::Confusion());

  gp_Vec aFD((aD2Plus.D2.XYZ() - aD2Minus.D2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3.Y(), aFD.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3.Z(), aFD.Z(), THE_FD_TOL_D2);
}

// Test EvalDN(u,1) matches D1 from EvalD1
TEST(GeomEval_TBezierCurveTest, EvalDN_ConsistentWithD1)
{
  GeomEval_TBezierCurve aCurve = createSimpleCurve();
  const double          aU     = M_PI / 6.0;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Vec            aDN = aCurve.EvalDN(aU, 1);

  EXPECT_NEAR(aD1.D1.X(), aDN.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Y(), aDN.Y(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Z(), aDN.Z(), Precision::Confusion());
}

// Test T-Bezier semicircle matches Geom_Circle D0
TEST(GeomEval_TBezierCurveTest, EvalD0_MatchesCircle)
{
  GeomEval_TBezierCurve aTBez   = createSemicircle();
  Handle(Geom_Circle)   aCircle = new Geom_Circle(gp_Ax2(), 1.0);

  const double aParams[] =
    {0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, 2.0 * M_PI / 3.0, 3.0 * M_PI / 4.0, M_PI};
  for (const double aU : aParams)
  {
    gp_Pnt aPT = aTBez.EvalD0(aU);
    gp_Pnt aPC = aCircle->EvalD0(aU);
    EXPECT_NEAR(aPT.Distance(aPC), 0.0, Precision::Angular()) << "D0 mismatch at u=" << aU;
  }
}

// Test T-Bezier semicircle matches Geom_Circle D1/D2/D3
TEST(GeomEval_TBezierCurveTest, EvalD3_MatchesCircle)
{
  GeomEval_TBezierCurve aTBez   = createSemicircle();
  Handle(Geom_Circle)   aCircle = new Geom_Circle(gp_Ax2(), 1.0);

  const double aParams[] = {M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 2.0 * M_PI / 3.0};
  for (const double aU : aParams)
  {
    Geom_Curve::ResD3 aDT = aTBez.EvalD3(aU);
    Geom_Curve::ResD3 aDC = aCircle->EvalD3(aU);

    EXPECT_NEAR(aDT.Point.Distance(aDC.Point), 0.0, Precision::Angular());
    EXPECT_NEAR(aDT.D1.X(), aDC.D1.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Y(), aDC.D1.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Z(), aDC.D1.Z(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.X(), aDC.D2.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Y(), aDC.D2.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Z(), aDC.D2.Z(), Precision::Angular());
    EXPECT_NEAR(aDT.D3.X(), aDC.D3.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D3.Y(), aDC.D3.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D3.Z(), aDC.D3.Z(), Precision::Angular());
  }
}

// Test T-Bezier semi-ellipse matches Geom_Ellipse D0/D1/D2
TEST(GeomEval_TBezierCurveTest, EvalD2_MatchesEllipse)
{
  GeomEval_TBezierCurve aTBez     = createSemiEllipse();
  Handle(Geom_Ellipse)  anEllipse = new Geom_Ellipse(gp_Ax2(), 3.0, 2.0);

  const double aParams[] = {M_PI / 6.0, M_PI / 4.0, M_PI / 2.0, 3.0 * M_PI / 4.0};
  for (const double aU : aParams)
  {
    Geom_Curve::ResD2 aDT = aTBez.EvalD2(aU);
    Geom_Curve::ResD2 aDC = anEllipse->EvalD2(aU);

    EXPECT_NEAR(aDT.Point.Distance(aDC.Point), 0.0, Precision::Angular());
    EXPECT_NEAR(aDT.D1.X(), aDC.D1.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Y(), aDC.D1.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Z(), aDC.D1.Z(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.X(), aDC.D2.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Y(), aDC.D2.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Z(), aDC.D2.Z(), Precision::Angular());
  }
}

// Test EvalD0 at multiple evenly-spaced points
TEST(GeomEval_TBezierCurveTest, EvalD0_MultiplePoints)
{
  GeomEval_TBezierCurve aCurve     = createSimpleCurve();
  const double          aFirst     = aCurve.FirstParameter();
  const double          aLast      = aCurve.LastParameter();
  const int             aNbSamples = 10;
  const double          aStep      = (aLast - aFirst) / aNbSamples;

  gp_Pnt aPrev = aCurve.EvalD0(aFirst);
  for (int i = 1; i <= aNbSamples; ++i)
  {
    const double aU   = aFirst + aStep * i;
    gp_Pnt       aPnt = aCurve.EvalD0(aU);

    // Verify the point has finite coordinates
    EXPECT_TRUE(std::isfinite(aPnt.X()));
    EXPECT_TRUE(std::isfinite(aPnt.Y()));
    EXPECT_TRUE(std::isfinite(aPnt.Z()));

    // Verify consecutive points don't jump too far (smoothness check)
    const double aDist = aPrev.Distance(aPnt);
    EXPECT_TRUE(aDist < 10.0 * aStep);

    aPrev = aPnt;
  }
}
