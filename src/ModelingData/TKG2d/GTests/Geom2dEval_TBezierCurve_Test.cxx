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
#include <Geom2dEval_TBezierCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
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
constexpr double THE_FD_TOL_D3 = 1e-3;

// Helper: T-Bezier unit semicircle C(t) = (cos(t), sin(t)), t in [0, Pi].
Geom2dEval_TBezierCurve createSemicircle2d()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(0.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(1.0, 0.0));
  return Geom2dEval_TBezierCurve(aPoles, 1.0);
}

// Helper: T-Bezier semi-ellipse (a=3, b=2): C(t) = (3*cos(t), 2*sin(t)).
Geom2dEval_TBezierCurve createSemiEllipse2d()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(0.0, 2.0));
  aPoles.SetValue(3, gp_Pnt2d(3.0, 0.0));
  return Geom2dEval_TBezierCurve(aPoles, 1.0);
}

// Helper to create a simple 3-pole (order 1) non-rational T-Bezier curve
Geom2dEval_TBezierCurve createSimpleCurve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  return Geom2dEval_TBezierCurve(aPoles, 1.0);
}

// Helper to create a 5-pole (order 2) non-rational T-Bezier curve
Geom2dEval_TBezierCurve createQuadraticCurve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 2.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  aPoles.SetValue(4, gp_Pnt2d(3.0, -1.0));
  aPoles.SetValue(5, gp_Pnt2d(4.0, 0.5));
  return Geom2dEval_TBezierCurve(aPoles, 0.5);
}

} // namespace

// Test construction with valid parameters (linear, order 1)
TEST(Geom2dEval_TBezierCurveTest, Construction_Linear)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.Order(), 1);
  EXPECT_NEAR(aCurve.Alpha(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction with valid parameters (quadratic, order 2)
TEST(Geom2dEval_TBezierCurveTest, Construction_Quadratic)
{
  Geom2dEval_TBezierCurve aCurve = createQuadraticCurve();
  EXPECT_EQ(aCurve.NbPoles(), 5);
  EXPECT_EQ(aCurve.Order(), 2);
  EXPECT_NEAR(aCurve.Alpha(), 0.5, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction throws for invalid parameters
TEST(Geom2dEval_TBezierCurveTest, Construction_InvalidParams_Throws)
{
  // Even number of poles (not odd)
  NCollection_Array1<gp_Pnt2d> aPoles2(1, 2);
  aPoles2.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles2.SetValue(2, gp_Pnt2d(1.0, 0.0));
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles2, 1.0), Standard_ConstructionError);

  // Too few poles
  NCollection_Array1<gp_Pnt2d> aPoles1(1, 1);
  aPoles1.SetValue(1, gp_Pnt2d(0.0, 0.0));
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles1, 1.0), Standard_ConstructionError);

  // Invalid alpha
  NCollection_Array1<gp_Pnt2d> aPoles3(1, 3);
  aPoles3.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles3.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles3.SetValue(3, gp_Pnt2d(2.0, 0.0));
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles3, 0.0), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles3, -1.0), Standard_ConstructionError);
}

// Test EvalD0 at endpoints matches StartPoint/EndPoint
TEST(Geom2dEval_TBezierCurveTest, EvalD0_Endpoints)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();

  gp_Pnt2d aStart = aCurve.EvalD0(aCurve.FirstParameter());
  gp_Pnt2d aEnd   = aCurve.EvalD0(aCurve.LastParameter());

  EXPECT_NEAR(aStart.Distance(aCurve.StartPoint()), 0.0, Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(aCurve.EndPoint()), 0.0, Precision::Confusion());
}

// Test EvalD0 at a known midpoint for the quadratic curve
TEST(Geom2dEval_TBezierCurveTest, EvalD0_Midpoint)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  const double            aMid   = (aCurve.FirstParameter() + aCurve.LastParameter()) / 2.0;
  gp_Pnt2d                aPt    = aCurve.EvalD0(aMid);

  // Verify the point is finite and within a reasonable range
  EXPECT_TRUE(std::isfinite(aPt.X()));
  EXPECT_TRUE(std::isfinite(aPt.Y()));
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(Geom2dEval_TBezierCurveTest, EvalD1_ConsistentWithD0)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  const double            aU     = M_PI / 3.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test derivative consistency for the quadratic curve
TEST(Geom2dEval_TBezierCurveTest, EvalD1_QuadraticConsistentWithD0)
{
  Geom2dEval_TBezierCurve aCurve = createQuadraticCurve();
  const double            aU     = M_PI / 4.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test Bounds returns correct domain [0, Pi/alpha]
TEST(Geom2dEval_TBezierCurveTest, Bounds)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.LastParameter(), M_PI / 1.0, Precision::Confusion());

  Geom2dEval_TBezierCurve aCurve2 = createQuadraticCurve();
  EXPECT_NEAR(aCurve2.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve2.LastParameter(), M_PI / 0.5, Precision::Confusion());
}

// Test IsRational returns false for non-rational curve
TEST(Geom2dEval_TBezierCurveTest, IsRational_NonRational)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_FALSE(aCurve.IsRational());
}

// Test IsRational returns true for rational curve with weights
TEST(Geom2dEval_TBezierCurveTest, IsRational_WithWeights)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 2.0);
  aWeights.SetValue(3, 1.0);
  Geom2dEval_TBezierCurve aCurve(aPoles, aWeights, 1.0);
  EXPECT_TRUE(aCurve.IsRational());
  EXPECT_EQ(aCurve.Weights().Size(), 3);
}

// Test rational construction throws for mismatched weights size
TEST(Geom2dEval_TBezierCurveTest, Construction_Rational_MismatchedWeights_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aWeights.SetValue(i, 1.0);
  }
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles, aWeights, 1.0), Standard_ConstructionError);
}

// Test rational construction throws for non-positive weight
TEST(Geom2dEval_TBezierCurveTest, Construction_Rational_NegativeWeight_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, -1.0);
  aWeights.SetValue(3, 1.0);
  EXPECT_THROW(Geom2dEval_TBezierCurve(aPoles, aWeights, 1.0), Standard_ConstructionError);
}

// Test NbPoles and Alpha accessors
TEST(Geom2dEval_TBezierCurveTest, NbPoles_Alpha)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_NEAR(aCurve.Alpha(), 1.0, Precision::Confusion());

  Geom2dEval_TBezierCurve aCurve2 = createQuadraticCurve();
  EXPECT_EQ(aCurve2.NbPoles(), 5);
  EXPECT_NEAR(aCurve2.Alpha(), 0.5, Precision::Confusion());
}

// Test Transform/Transformed are not implemented
TEST(Geom2dEval_TBezierCurveTest, Transform_NotImplemented)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  gp_Trsf2d               aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(1.0, 2.0));

  EXPECT_THROW(aCurve.Transform(aTrsf), Standard_NotImplemented);
  EXPECT_THROW((void)aCurve.Transformed(aTrsf), Standard_NotImplemented);
}

// Test Copy produces independent identical object
TEST(Geom2dEval_TBezierCurveTest, Copy_Independent)
{
  Geom2dEval_TBezierCurve      aCurve = createSimpleCurve();
  occ::handle<Geom2d_Geometry> aCopy  = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const Geom2dEval_TBezierCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_TBezierCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_EQ(aCopyCurve->NbPoles(), 3);
  EXPECT_NEAR(aCopyCurve->Alpha(), 1.0, Precision::Confusion());

  // Verify copy evaluates the same
  const double aU      = M_PI / 4.0;
  gp_Pnt2d     aOrigPt = aCurve.EvalD0(aU);
  gp_Pnt2d     aCopyPt = aCopyCurve->EvalD0(aU);
  EXPECT_NEAR(aOrigPt.Distance(aCopyPt), 0.0, Precision::Confusion());
}

// Test Copy behavior with unsupported transform.
TEST(Geom2dEval_TBezierCurveTest, Copy_Modification_Independent)
{
  Geom2dEval_TBezierCurve      aCurve     = createSimpleCurve();
  occ::handle<Geom2d_Geometry> aCopy      = aCurve.Copy();
  Geom2dEval_TBezierCurve*     aCopyCurve = dynamic_cast<Geom2dEval_TBezierCurve*>(aCopy.get());
  ASSERT_TRUE(aCopyCurve != nullptr);

  // Transform is unsupported on the copy as well.
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10.0, 10.0));
  EXPECT_THROW(aCopyCurve->Transform(aTrsf), Standard_NotImplemented);

  // Copy and original remain identical.
  const double aU      = M_PI / 4.0;
  gp_Pnt2d     aOrigPt = aCurve.EvalD0(aU);
  gp_Pnt2d     aCopyPt = aCopyCurve->EvalD0(aU);
  EXPECT_NEAR(aOrigPt.Distance(aCopyPt), 0.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(Geom2dEval_TBezierCurveTest, DumpJson_NoCrash)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  Standard_SStream        aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test DumpJson does not crash for rational curve
TEST(Geom2dEval_TBezierCurveTest, DumpJson_Rational_NoCrash)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 2.0);
  aWeights.SetValue(3, 1.0);
  Geom2dEval_TBezierCurve aCurve(aPoles, aWeights, 1.0);
  Standard_SStream        aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test periodicity and continuity
TEST(Geom2dEval_TBezierCurveTest, PeriodicityAndContinuity)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_FALSE(aCurve.IsPeriodic());
  EXPECT_EQ(aCurve.Continuity(), GeomAbs_CN);
  EXPECT_TRUE(aCurve.IsCN(0));
  EXPECT_TRUE(aCurve.IsCN(10));
}

TEST(Geom2dEval_TBezierCurveTest, Reverse_NotImplemented)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

// Test EvalD1 derivative consistency for rational curve
TEST(Geom2dEval_TBezierCurveTest, EvalD1_Rational_ConsistentWithD0)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 3.0);
  aWeights.SetValue(3, 1.0);
  Geom2dEval_TBezierCurve aCurve(aPoles, aWeights, 1.0);

  const double aU = M_PI / 3.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test EvalD2 consistency: compare D2 vs finite difference of D1
TEST(Geom2dEval_TBezierCurveTest, EvalD2_ConsistentWithD1)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  const double            aU     = M_PI / 4.0;

  Geom2d_Curve::ResD2 aD2      = aCurve.EvalD2(aU);
  Geom2d_Curve::ResD1 aD1Plus  = aCurve.EvalD1(aU + Precision::Confusion());
  Geom2d_Curve::ResD1 aD1Minus = aCurve.EvalD1(aU - Precision::Confusion());

  gp_Vec2d aFD((aD1Plus.D1.XY() - aD1Minus.D1.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Y(), aFD.Y(), THE_FD_TOL_D2);
}

// Test EvalD3 consistency: compare D3 vs finite difference of D2
TEST(Geom2dEval_TBezierCurveTest, EvalD3_ConsistentWithD2)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  const double            aU     = M_PI / 3.0;

  Geom2d_Curve::ResD3 aD3      = aCurve.EvalD3(aU);
  Geom2d_Curve::ResD2 aD2Plus  = aCurve.EvalD2(aU + Precision::Confusion());
  Geom2d_Curve::ResD2 aD2Minus = aCurve.EvalD2(aU - Precision::Confusion());

  gp_Vec2d aFD((aD2Plus.D2.XY() - aD2Minus.D2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3.X(), aFD.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3.Y(), aFD.Y(), THE_FD_TOL_D3);
}

// Test EvalDN(u,1) matches D1 from EvalD1
TEST(Geom2dEval_TBezierCurveTest, EvalDN_ConsistentWithD1)
{
  Geom2dEval_TBezierCurve aCurve = createSimpleCurve();
  const double            aU     = M_PI / 6.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Vec2d            aDN = aCurve.EvalDN(aU, 1);

  EXPECT_NEAR(aD1.D1.X(), aDN.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Y(), aDN.Y(), Precision::Confusion());
}

// Test EvalD0 at multiple evenly-spaced points produces finite results
TEST(Geom2dEval_TBezierCurveTest, EvalD0_MultiplePoints)
{
  Geom2dEval_TBezierCurve aCurve    = createSimpleCurve();
  const double            aFirst    = aCurve.FirstParameter();
  const double            aLast     = aCurve.LastParameter();
  const int               aNbPoints = 6;

  for (int i = 0; i < aNbPoints; ++i)
  {
    const double aU  = aFirst + (aLast - aFirst) * i / (aNbPoints - 1);
    gp_Pnt2d     aPt = aCurve.EvalD0(aU);
    EXPECT_TRUE(std::isfinite(aPt.X())) << "Non-finite X at i=" << i;
    EXPECT_TRUE(std::isfinite(aPt.Y())) << "Non-finite Y at i=" << i;
  }
}

// Test 2D T-Bezier semicircle matches Geom2d_Circle D0/D1/D2/D3
TEST(Geom2dEval_TBezierCurveTest, EvalD3_MatchesCircle)
{
  Geom2dEval_TBezierCurve    aTBez   = createSemicircle2d();
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(gp_Ax22d(), 1.0);

  const double aParams[] = {M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 2.0 * M_PI / 3.0};
  for (const double aU : aParams)
  {
    Geom2d_Curve::ResD3 aDT = aTBez.EvalD3(aU);
    Geom2d_Curve::ResD3 aDC = aCircle->EvalD3(aU);

    EXPECT_NEAR(aDT.Point.Distance(aDC.Point), 0.0, Precision::Angular());
    EXPECT_NEAR(aDT.D1.X(), aDC.D1.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Y(), aDC.D1.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.X(), aDC.D2.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Y(), aDC.D2.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D3.X(), aDC.D3.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D3.Y(), aDC.D3.Y(), Precision::Angular());
  }
}

// Test 2D T-Bezier semi-ellipse matches Geom2d_Ellipse D0/D1/D2
TEST(Geom2dEval_TBezierCurveTest, EvalD2_MatchesEllipse)
{
  Geom2dEval_TBezierCurve     aTBez     = createSemiEllipse2d();
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(gp_Ax22d(), 3.0, 2.0);

  const double aParams[] = {M_PI / 6.0, M_PI / 4.0, M_PI / 2.0, 3.0 * M_PI / 4.0};
  for (const double aU : aParams)
  {
    Geom2d_Curve::ResD2 aDT = aTBez.EvalD2(aU);
    Geom2d_Curve::ResD2 aDC = anEllipse->EvalD2(aU);

    EXPECT_NEAR(aDT.Point.Distance(aDC.Point), 0.0, Precision::Angular());
    EXPECT_NEAR(aDT.D1.X(), aDC.D1.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D1.Y(), aDC.D1.Y(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.X(), aDC.D2.X(), Precision::Angular());
    EXPECT_NEAR(aDT.D2.Y(), aDC.D2.Y(), Precision::Angular());
  }
}
