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
#include <GeomEval_AHTBezierCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{

constexpr double THE_FD_TOL_D1 = 1e-5;
constexpr double THE_FD_TOL_D2 = 1e-4;

// Helper to create a curve with basis {1, t, sinh(alpha*t), cosh(alpha*t), sin(beta*t), cos(beta*t)}
// algDegree=1, alpha=1.0, beta=1.0 => basisDim = 2 + 2 + 2 = 6 poles needed
GeomEval_AHTBezierCurve createFullBasisCurve()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(1.0, 1.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(5, gp_Pnt(0.0, 1.0, 1.0));
  aPoles.SetValue(6, gp_Pnt(1.0, 0.0, 1.0));
  return GeomEval_AHTBezierCurve(aPoles, 1, 1.0, 1.0);
}

// Helper to create a pure trigonometric AHT curve: basis {1, sin(t), cos(t)} => 3 poles
// C(t) = P_0 + P_1*sin(t) + P_2*cos(t) = (cos(t), sin(t), 0), t in [0, 1].
GeomEval_AHTBezierCurve createTrigCircleArc()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(1.0, 0.0, 0.0));
  return GeomEval_AHTBezierCurve(aPoles, 0, 0.0, 1.0);
}

// Helper to create a pure trigonometric AHT ellipse arc: basis {1, sin(t), cos(t)} => 3 poles
// C(t) = (3*cos(t), 2*sin(t), 0), t in [0, 1].
GeomEval_AHTBezierCurve createTrigEllipseArc()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(0.0, 2.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(3.0, 0.0, 0.0));
  return GeomEval_AHTBezierCurve(aPoles, 0, 0.0, 1.0);
}

// Helper to create a pure polynomial curve: basis {1, t, t^2} => 3 poles
GeomEval_AHTBezierCurve createPolynomialCurve()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  return GeomEval_AHTBezierCurve(aPoles, 2, 0.0, 0.0);
}

} // namespace

// Test construction with full AHT basis
TEST(GeomEval_AHTBezierCurveTest, Construction_FullBasis)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_EQ(aCurve.NbPoles(), 6);
  EXPECT_EQ(aCurve.AlgDegree(), 1);
  EXPECT_NEAR(aCurve.Alpha(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction with pure polynomial basis
TEST(GeomEval_AHTBezierCurveTest, Construction_Polynomial)
{
  GeomEval_AHTBezierCurve aCurve = createPolynomialCurve();
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.AlgDegree(), 2);
  EXPECT_NEAR(aCurve.Alpha(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 0.0, Precision::Confusion());
}

// Test parameter range is [0, 1]
TEST(GeomEval_AHTBezierCurveTest, ParameterRange)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.LastParameter(), 1.0, Precision::Confusion());
}

// Test EvalD0 at endpoints matches StartPoint/EndPoint
TEST(GeomEval_AHTBezierCurveTest, EvalD0_Endpoints)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();

  gp_Pnt aStart = aCurve.EvalD0(aCurve.FirstParameter());
  gp_Pnt aEnd   = aCurve.EvalD0(aCurve.LastParameter());

  EXPECT_NEAR(aStart.Distance(aCurve.StartPoint()), 0.0, Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(aCurve.EndPoint()), 0.0, Precision::Confusion());
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_AHTBezierCurveTest, EvalD1_ConsistentWithD0)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double aU = 0.4;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec aFD((aP1.XYZ() - aP2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Z(), aFD.Z(), THE_FD_TOL_D1);
}

// Test periodicity
TEST(GeomEval_AHTBezierCurveTest, Periodicity)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_FALSE(aCurve.IsPeriodic());
}

TEST(GeomEval_AHTBezierCurveTest, Reverse_NotImplemented)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

// Test rational construction
TEST(GeomEval_AHTBezierCurveTest, Construction_Rational)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 2.0);
  aWeights.SetValue(3, 1.0);
  GeomEval_AHTBezierCurve aCurve(aPoles, aWeights, 2, 0.0, 0.0);
  EXPECT_TRUE(aCurve.IsRational());
}

// Test Transform preserves evaluation
TEST(GeomEval_AHTBezierCurveTest, Transform_PreservesEvaluation)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  const double aU = 0.5;
  gp_Pnt aPBefore = aCurve.EvalD0(aU);
  aPBefore.Transform(aTrsf);
  aCurve.Transform(aTrsf);
  gp_Pnt aPAfter = aCurve.EvalD0(aU);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_AHTBezierCurveTest, Copy_Independent)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  occ::handle<Geom_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_AHTBezierCurve* aCopyCurve =
    dynamic_cast<const GeomEval_AHTBezierCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_EQ(aCopyCurve->NbPoles(), 6);
  EXPECT_EQ(aCopyCurve->AlgDegree(), 1);
}

// Test DumpJson does not crash
TEST(GeomEval_AHTBezierCurveTest, DumpJson_NoCrash)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  Standard_SStream aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test EvalD2 derivative D2 matches finite differences of D1
TEST(GeomEval_AHTBezierCurveTest, EvalD2_ConsistentWithD1)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double aU = 0.3;

  Geom_Curve::ResD2 aD2 = aCurve.EvalD2(aU);
  Geom_Curve::ResD1 aD1Plus = aCurve.EvalD1(aU + Precision::Confusion());
  Geom_Curve::ResD1 aD1Minus = aCurve.EvalD1(aU - Precision::Confusion());

  gp_Vec aFD((aD1Plus.D1.XYZ() - aD1Minus.D1.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Y(), aFD.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Z(), aFD.Z(), THE_FD_TOL_D2);
}

// Test EvalD3 derivative D3 matches finite differences of D2
TEST(GeomEval_AHTBezierCurveTest, EvalD3_ConsistentWithD2)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double aU = 0.4;

  Geom_Curve::ResD3 aD3 = aCurve.EvalD3(aU);
  Geom_Curve::ResD2 aD2Plus = aCurve.EvalD2(aU + Precision::Confusion());
  Geom_Curve::ResD2 aD2Minus = aCurve.EvalD2(aU - Precision::Confusion());

  gp_Vec aFD((aD2Plus.D2.XYZ() - aD2Minus.D2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3.Y(), aFD.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3.Z(), aFD.Z(), THE_FD_TOL_D2);
}

// Test EvalDN(u,1) matches D1 from EvalD1
TEST(GeomEval_AHTBezierCurveTest, EvalDN_ConsistentWithD1)
{
  GeomEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double aU = 0.5;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Vec aDN = aCurve.EvalDN(aU, 1);

  EXPECT_NEAR(aD1.D1.X(), aDN.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Y(), aDN.Y(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Z(), aDN.Z(), Precision::Confusion());
}

// Test EvalD0 for pure polynomial curve with known values
// Basis {1, t, t^2} on [0,1], P(t) = P0*1 + P1*t + P2*t^2
TEST(GeomEval_AHTBezierCurveTest, EvalD0_PurePolynomial_KnownValues)
{
  const gp_Pnt aP0(0.0, 0.0, 0.0);
  const gp_Pnt aP1(0.5, 1.0, 0.0);
  const gp_Pnt aP2(1.0, 0.0, 0.0);

  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, aP0);
  aPoles.SetValue(2, aP1);
  aPoles.SetValue(3, aP2);
  GeomEval_AHTBezierCurve aCurve(aPoles, 2, 0.0, 0.0);

  // P(0) = P0
  gp_Pnt aVal0 = aCurve.EvalD0(0.0);
  EXPECT_NEAR(aVal0.Distance(aP0), 0.0, Precision::Confusion());

  // P(0.5) = P0 + 0.5*P1 + 0.25*P2
  gp_Pnt aExpected05(aP0.X() + 0.5 * aP1.X() + 0.25 * aP2.X(),
                     aP0.Y() + 0.5 * aP1.Y() + 0.25 * aP2.Y(),
                     aP0.Z() + 0.5 * aP1.Z() + 0.25 * aP2.Z());
  gp_Pnt aVal05 = aCurve.EvalD0(0.5);
  EXPECT_NEAR(aVal05.Distance(aExpected05), 0.0, Precision::Confusion());

  // P(1) = P0 + P1 + P2
  gp_Pnt aExpected1(aP0.X() + aP1.X() + aP2.X(),
                    aP0.Y() + aP1.Y() + aP2.Y(),
                    aP0.Z() + aP1.Z() + aP2.Z());
  gp_Pnt aVal1 = aCurve.EvalD0(1.0);
  EXPECT_NEAR(aVal1.Distance(aExpected1), 0.0, Precision::Confusion());
}

// Test AHT-Bezier trigonometric circle arc matches Geom_Circle D0
TEST(GeomEval_AHTBezierCurveTest, EvalD0_MatchesCircle)
{
  GeomEval_AHTBezierCurve aAHT = createTrigCircleArc();
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(), 1.0);

  const double aParams[] = {0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0};
  for (const double aU : aParams)
  {
    gp_Pnt aPAHT = aAHT.EvalD0(aU);
    gp_Pnt aPCirc = aCircle->EvalD0(aU);
    EXPECT_NEAR(aPAHT.Distance(aPCirc), 0.0, Precision::Angular())
      << "D0 mismatch at u=" << aU;
  }
}

// Test AHT-Bezier trigonometric circle arc matches Geom_Circle D1/D2/D3
TEST(GeomEval_AHTBezierCurveTest, EvalD3_MatchesCircle)
{
  GeomEval_AHTBezierCurve aAHT = createTrigCircleArc();
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(), 1.0);

  const double aParams[] = {0.1, 0.25, 0.5, 0.75, 0.9};
  for (const double aU : aParams)
  {
    Geom_Curve::ResD3 aDT = aAHT.EvalD3(aU);
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

// Test AHT-Bezier trigonometric ellipse arc matches Geom_Ellipse D0/D1/D2
TEST(GeomEval_AHTBezierCurveTest, EvalD2_MatchesEllipse)
{
  GeomEval_AHTBezierCurve aAHT = createTrigEllipseArc();
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(gp_Ax2(), 3.0, 2.0);

  const double aParams[] = {0.1, 0.25, 0.5, 0.75, 0.9};
  for (const double aU : aParams)
  {
    Geom_Curve::ResD2 aDT = aAHT.EvalD2(aU);
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
