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

#include <Geom2dEval_AHTBezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{

constexpr double THE_FD_TOL_D1 = 1e-5;
constexpr double THE_FD_TOL_D2 = 1e-4;
constexpr double THE_FD_TOL_D3 = 1e-3;

// Helper to create a curve with basis {1, t, sinh(alpha*t), cosh(alpha*t), sin(beta*t),
// cos(beta*t)} algDegree=1, alpha=1.0, beta=1.0 => basisDim = 2 + 2 + 2 = 6 poles needed
Geom2dEval_AHTBezierCurve createFullBasisCurve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(4, gp_Pnt2d(0.0, 1.0));
  aPoles.SetValue(5, gp_Pnt2d(0.0, 1.0));
  aPoles.SetValue(6, gp_Pnt2d(1.0, 0.0));
  return Geom2dEval_AHTBezierCurve(aPoles, 1, 1.0, 1.0);
}

// Helper to create a pure polynomial curve: basis {1, t, t^2} => 3 poles
Geom2dEval_AHTBezierCurve createPolynomialCurve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  return Geom2dEval_AHTBezierCurve(aPoles, 2, 0.0, 0.0);
}

// Helper to create a rational polynomial curve with weights
Geom2dEval_AHTBezierCurve createRationalCurve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 2.0);
  aWeights.SetValue(3, 1.0);
  return Geom2dEval_AHTBezierCurve(aPoles, aWeights, 2, 0.0, 0.0);
}

} // namespace

// Test construction with full AHT basis
TEST(Geom2dEval_AHTBezierCurveTest, Construction_FullBasis)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_EQ(aCurve.NbPoles(), 6);
  EXPECT_EQ(aCurve.AlgDegree(), 1);
  EXPECT_NEAR(aCurve.Alpha(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction with pure polynomial basis
TEST(Geom2dEval_AHTBezierCurveTest, Construction_Polynomial)
{
  Geom2dEval_AHTBezierCurve aCurve = createPolynomialCurve();
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.AlgDegree(), 2);
  EXPECT_NEAR(aCurve.Alpha(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 0.0, Precision::Confusion());
  EXPECT_FALSE(aCurve.IsRational());
}

// Test construction with hyperbolic-only mode: basis {1, sinh(alpha*t), cosh(alpha*t)}
TEST(Geom2dEval_AHTBezierCurveTest, Construction_HyperbolicOnly)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  Geom2dEval_AHTBezierCurve aCurve(aPoles, 0, 2.0, 0.0);
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.AlgDegree(), 0);
  EXPECT_NEAR(aCurve.Alpha(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 0.0, Precision::Confusion());
}

// Test rational construction
TEST(Geom2dEval_AHTBezierCurveTest, Construction_Rational)
{
  Geom2dEval_AHTBezierCurve aCurve = createRationalCurve();
  EXPECT_TRUE(aCurve.IsRational());
  EXPECT_EQ(aCurve.NbPoles(), 3);
  EXPECT_EQ(aCurve.AlgDegree(), 2);
}

// Test non-rational curve reports IsRational() == false
TEST(Geom2dEval_AHTBezierCurveTest, IsRational_NonRational)
{
  Geom2dEval_AHTBezierCurve aCurve = createPolynomialCurve();
  EXPECT_FALSE(aCurve.IsRational());
}

// Test parameter range is [0, 1]
TEST(Geom2dEval_AHTBezierCurveTest, Bounds)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.LastParameter(), 1.0, Precision::Confusion());
}

// Test EvalD0 at endpoints matches StartPoint/EndPoint
TEST(Geom2dEval_AHTBezierCurveTest, EvalD0_Endpoints)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();

  gp_Pnt2d aStart = aCurve.EvalD0(aCurve.FirstParameter());
  gp_Pnt2d aEnd   = aCurve.EvalD0(aCurve.LastParameter());

  EXPECT_NEAR(aStart.Distance(aCurve.StartPoint()), 0.0, Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(aCurve.EndPoint()), 0.0, Precision::Confusion());
}

// Test EvalD0 at a known parameter for the polynomial curve.
// For basis {1, t, t^2} with poles P0=(0,0), P1=(1,1), P2=(2,0):
// C(t) = P0*1 + P1*t + P2*t^2
// C(0.5) = (0,0) + 0.5*(1,1) + 0.25*(2,0) = (0.5+0.5, 0.5) = (1.0, 0.5)
TEST(Geom2dEval_AHTBezierCurveTest, EvalD0_KnownPoint)
{
  Geom2dEval_AHTBezierCurve aCurve = createPolynomialCurve();
  gp_Pnt2d                  aPnt   = aCurve.EvalD0(0.5);
  EXPECT_NEAR(aPnt.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.5, Precision::Confusion());
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(Geom2dEval_AHTBezierCurveTest, EvalD1_ConsistentWithD0)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double              aU     = 0.4;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test derivative consistency for the polynomial curve
TEST(Geom2dEval_AHTBezierCurveTest, EvalD1_Polynomial_ConsistentWithD0)
{
  Geom2dEval_AHTBezierCurve aCurve = createPolynomialCurve();
  const double              aU     = 0.3;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test derivative consistency for rational curve
TEST(Geom2dEval_AHTBezierCurveTest, EvalD1_Rational_ConsistentWithD0)
{
  Geom2dEval_AHTBezierCurve aCurve = createRationalCurve();
  const double              aU     = 0.5;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Pnt2d            aP1 = aCurve.EvalD0(aU + Precision::Confusion());
  gp_Pnt2d            aP2 = aCurve.EvalD0(aU - Precision::Confusion());

  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL_D1);
}

// Test NbPoles(), AlgDegree(), Alpha(), Beta() accessors
TEST(Geom2dEval_AHTBezierCurveTest, Accessors)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 1.0));
  aPoles.SetValue(4, gp_Pnt2d(3.0, 1.0));
  aPoles.SetValue(5, gp_Pnt2d(4.0, 0.0));
  // algDegree=2 => 3 polynomial basis + 2 trig basis = 5 => beta>0
  Geom2dEval_AHTBezierCurve aCurve(aPoles, 2, 0.0, 3.5);
  EXPECT_EQ(aCurve.NbPoles(), 5);
  EXPECT_EQ(aCurve.AlgDegree(), 2);
  EXPECT_NEAR(aCurve.Alpha(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Beta(), 3.5, Precision::Confusion());
}

// Test periodicity
TEST(Geom2dEval_AHTBezierCurveTest, Periodicity)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_FALSE(aCurve.IsPeriodic());
}

// Test continuity
TEST(Geom2dEval_AHTBezierCurveTest, Continuity)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_EQ(aCurve.Continuity(), GeomAbs_CN);
  EXPECT_TRUE(aCurve.IsCN(0));
  EXPECT_TRUE(aCurve.IsCN(5));
  EXPECT_TRUE(aCurve.IsCN(100));
}

// Test Transform/Transformed are not implemented
TEST(Geom2dEval_AHTBezierCurveTest, Transform_NotImplemented)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  gp_Trsf2d                 aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(1.0, 2.0));

  EXPECT_THROW(aCurve.Transform(aTrsf), Standard_NotImplemented);
  EXPECT_THROW((void)aCurve.Transformed(aTrsf), Standard_NotImplemented);
}

// Test Copy produces independent identical object
TEST(Geom2dEval_AHTBezierCurveTest, Copy_Independent)
{
  Geom2dEval_AHTBezierCurve    aCurve = createFullBasisCurve();
  occ::handle<Geom2d_Geometry> aCopy  = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const Geom2dEval_AHTBezierCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_AHTBezierCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_EQ(aCopyCurve->NbPoles(), 6);
  EXPECT_EQ(aCopyCurve->AlgDegree(), 1);

  // Verify the copy evaluates identically at several parameters
  for (double aU = 0.0; aU <= 1.0; aU += 0.25)
  {
    gp_Pnt2d aOrigPnt = aCurve.EvalD0(aU);
    gp_Pnt2d aCopyPnt = aCopyCurve->EvalD0(aU);
    EXPECT_NEAR(aOrigPnt.Distance(aCopyPnt), 0.0, Precision::Confusion());
  }

  // Verify behavior: transformation is not supported on the copy either.
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10.0, 10.0));
  Handle(Geom2dEval_AHTBezierCurve) aCopyHandle =
    Handle(Geom2dEval_AHTBezierCurve)::DownCast(aCopy);
  EXPECT_THROW(aCopyHandle->Transform(aTrsf), Standard_NotImplemented);

  gp_Pnt2d aOrigMid = aCurve.EvalD0(0.5);
  gp_Pnt2d aCopyMid = aCopyHandle->EvalD0(0.5);
  EXPECT_NEAR(aOrigMid.Distance(aCopyMid), 0.0, Precision::Confusion());
}

// Test Copy of rational curve preserves rationality
TEST(Geom2dEval_AHTBezierCurveTest, Copy_Rational)
{
  Geom2dEval_AHTBezierCurve        aCurve = createRationalCurve();
  occ::handle<Geom2d_Geometry>     aCopy  = aCurve.Copy();
  const Geom2dEval_AHTBezierCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_AHTBezierCurve*>(aCopy.get());
  ASSERT_TRUE(aCopyCurve != nullptr);
  EXPECT_TRUE(aCopyCurve->IsRational());
}

// Test DumpJson does not crash
TEST(Geom2dEval_AHTBezierCurveTest, DumpJson_NoCrash)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  Standard_SStream          aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test DumpJson on rational curve does not crash
TEST(Geom2dEval_AHTBezierCurveTest, DumpJson_Rational_NoCrash)
{
  Geom2dEval_AHTBezierCurve aCurve = createRationalCurve();
  Standard_SStream          aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}

// Test invalid construction: pole count mismatch throws
TEST(Geom2dEval_AHTBezierCurveTest, Construction_PoleCountMismatch_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(4, gp_Pnt2d(0.0, 1.0));
  // algDegree=1, alpha=1.0, beta=1.0 => basisDim = 2+2+2 = 6, but only 4 poles
  EXPECT_THROW(Geom2dEval_AHTBezierCurve(aPoles, 1, 1.0, 1.0), Standard_ConstructionError);
}

// Test invalid construction: negative alpha throws
TEST(Geom2dEval_AHTBezierCurveTest, Construction_NegativeAlpha_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  EXPECT_THROW(Geom2dEval_AHTBezierCurve(aPoles, 2, -1.0, 0.0), Standard_ConstructionError);
}

// Test invalid construction: negative beta throws
TEST(Geom2dEval_AHTBezierCurveTest, Construction_NegativeBeta_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  EXPECT_THROW(Geom2dEval_AHTBezierCurve(aPoles, 2, 0.0, -1.0), Standard_ConstructionError);
}

// Test invalid construction: weight count mismatch throws
TEST(Geom2dEval_AHTBezierCurveTest, Construction_WeightCountMismatch_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 2);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0);
  EXPECT_THROW(Geom2dEval_AHTBezierCurve(aPoles, aWeights, 2, 0.0, 0.0),
               Standard_ConstructionError);
}

// Test invalid construction: non-positive weight throws
TEST(Geom2dEval_AHTBezierCurveTest, Construction_NonPositiveWeight_Throws)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aWeights(1, 3);
  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, -1.0);
  aWeights.SetValue(3, 1.0);
  EXPECT_THROW(Geom2dEval_AHTBezierCurve(aPoles, aWeights, 2, 0.0, 0.0),
               Standard_ConstructionError);
}

// Reverse semantics are intentionally unsupported for this evaluator.
TEST(Geom2dEval_AHTBezierCurveTest, Reverse_NotImplemented)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

// Test EvalD2 consistency: compare D2 vs finite difference of D1
TEST(Geom2dEval_AHTBezierCurveTest, EvalD2_ConsistentWithD1)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double              aU     = 0.3;

  Geom2d_Curve::ResD2 aD2      = aCurve.EvalD2(aU);
  Geom2d_Curve::ResD1 aD1Plus  = aCurve.EvalD1(aU + Precision::Confusion());
  Geom2d_Curve::ResD1 aD1Minus = aCurve.EvalD1(aU - Precision::Confusion());

  gp_Vec2d aFD((aD1Plus.D1.XY() - aD1Minus.D1.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2.X(), aFD.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2.Y(), aFD.Y(), THE_FD_TOL_D2);
}

// Test EvalD3 consistency: compare D3 vs finite difference of D2
TEST(Geom2dEval_AHTBezierCurveTest, EvalD3_ConsistentWithD2)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double              aU     = 0.4;

  Geom2d_Curve::ResD3 aD3      = aCurve.EvalD3(aU);
  Geom2d_Curve::ResD2 aD2Plus  = aCurve.EvalD2(aU + Precision::Confusion());
  Geom2d_Curve::ResD2 aD2Minus = aCurve.EvalD2(aU - Precision::Confusion());

  gp_Vec2d aFD((aD2Plus.D2.XY() - aD2Minus.D2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3.X(), aFD.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3.Y(), aFD.Y(), THE_FD_TOL_D3);
}

// Test EvalDN(u,1) matches D1 from EvalD1
TEST(Geom2dEval_AHTBezierCurveTest, EvalDN_ConsistentWithD1)
{
  Geom2dEval_AHTBezierCurve aCurve = createFullBasisCurve();
  const double              aU     = 0.5;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aU);
  gp_Vec2d            aDN = aCurve.EvalDN(aU, 1);

  EXPECT_NEAR(aD1.D1.X(), aDN.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1.Y(), aDN.Y(), Precision::Confusion());
}

// Test EvalD0 for pure polynomial line: alpha=0, beta=0, algDegree=1, basis {1, t}
// P(t) = (0,0)*1 + (1,1)*t => P(0.5) = (0.5, 0.5), P(1) = (1, 1)
TEST(Geom2dEval_AHTBezierCurveTest, EvalD0_PurePolynomial_Line)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, 1.0));
  Geom2dEval_AHTBezierCurve aCurve(aPoles, 1, 0.0, 0.0);

  gp_Pnt2d aPtHalf = aCurve.EvalD0(0.5);
  EXPECT_NEAR(aPtHalf.X(), 0.5, Precision::Confusion());
  EXPECT_NEAR(aPtHalf.Y(), 0.5, Precision::Confusion());

  gp_Pnt2d aPtEnd = aCurve.EvalD0(1.0);
  EXPECT_NEAR(aPtEnd.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPtEnd.Y(), 1.0, Precision::Confusion());
}
