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

#include <GeomEval_AHTBezierSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{

constexpr double THE_FD_TOL_D1 = 1e-5;
constexpr double THE_FD_TOL_D2 = 1e-4;

double vecDiff(const gp_Vec& theV1, const gp_Vec& theV2)
{
  return (theV1 - theV2).Magnitude();
}

// Helper to create a pure polynomial AHT-Bezier surface.
// Basis: {1, t, t^2} in each direction => 3x3 poles, algDegree=2, alpha=0, beta=0
GeomEval_AHTBezierSurface createPolynomialSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(double(i - 1), double(j - 1),
                                    double((i - 1) * (j - 1)) * 0.1));
    }
  }
  return GeomEval_AHTBezierSurface(aPoles, 2, 2, 0.0, 0.0, 0.0, 0.0);
}

GeomEval_AHTBezierSurface createPolynomialSurfaceRationalUniformWeight(const double theWeight)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(double(i - 1), double(j - 1),
                                    double((i - 1) * (j - 1)) * 0.1));
      aWeights.SetValue(i, j, theWeight);
    }
  }
  return GeomEval_AHTBezierSurface(aPoles, aWeights, 2, 2, 0.0, 0.0, 0.0, 0.0);
}

} // namespace

// Test construction with valid parameters
TEST(GeomEval_AHTBezierSurfaceTest, Construction_ValidParams)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  EXPECT_EQ(aSurf.NbPolesU(), 3);
  EXPECT_EQ(aSurf.NbPolesV(), 3);
  EXPECT_EQ(aSurf.AlgDegreeU(), 2);
  EXPECT_EQ(aSurf.AlgDegreeV(), 2);
  EXPECT_NEAR(aSurf.AlphaU(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.AlphaV(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.BetaU(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.BetaV(), 0.0, Precision::Confusion());
}

// Test parameter range is [0, 1] x [0, 1]
TEST(GeomEval_AHTBezierSurfaceTest, ParameterRange)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aU2, 1.0, Precision::Confusion());
  EXPECT_NEAR(aV1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aV2, 1.0, Precision::Confusion());
}

// Test EvalD0 at corners
TEST(GeomEval_AHTBezierSurfaceTest, EvalD0_Corners)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();

  gp_Pnt aP00 = aSurf.EvalD0(0.0, 0.0);
  gp_Pnt aP10 = aSurf.EvalD0(1.0, 0.0);
  gp_Pnt aP01 = aSurf.EvalD0(0.0, 1.0);
  gp_Pnt aP11 = aSurf.EvalD0(1.0, 1.0);

  // Verify corners are distinct (non-degenerate surface)
  EXPECT_TRUE(aP00.Distance(aP10) > Precision::Confusion());
  EXPECT_TRUE(aP00.Distance(aP01) > Precision::Confusion());
  EXPECT_TRUE(aP00.Distance(aP11) > Precision::Confusion());
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_AHTBezierSurfaceTest, EvalD1_ConsistentWithD0)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  const double aU = 0.3, aV = 0.7;

  Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);
  gp_Pnt aPu1 = aSurf.EvalD0(aU + Precision::Confusion(), aV);
  gp_Pnt aPu2 = aSurf.EvalD0(aU - Precision::Confusion(), aV);
  gp_Pnt aPv1 = aSurf.EvalD0(aU, aV + Precision::Confusion());
  gp_Pnt aPv2 = aSurf.EvalD0(aU, aV - Precision::Confusion());

  gp_Vec aFDU((aPu1.XYZ() - aPu2.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFDV((aPv1.XYZ() - aPv2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1U.X(), aFDU.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1U.Y(), aFDU.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1U.Z(), aFDU.Z(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.X(), aFDV.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.Y(), aFDV.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.Z(), aFDV.Z(), THE_FD_TOL_D1);
}

// Test periodicity
TEST(GeomEval_AHTBezierSurfaceTest, Periodicity)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  EXPECT_FALSE(aSurf.IsUPeriodic());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_FALSE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVClosed());
}

TEST(GeomEval_AHTBezierSurfaceTest, Reverse_NotImplemented)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  EXPECT_THROW(aSurf.UReverse(), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VReverse(), Standard_NotImplemented);
  EXPECT_THROW(aSurf.UReversedParameter(0.5), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VReversedParameter(0.5), Standard_NotImplemented);
}

TEST(GeomEval_AHTBezierSurfaceTest, Iso_NotImplemented)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  EXPECT_THROW(aSurf.UIso(0.5), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VIso(0.5), Standard_NotImplemented);
}

// Test Transform preserves evaluation
TEST(GeomEval_AHTBezierSurfaceTest, Transform_PreservesEvaluation)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  const double aU = 0.5, aV = 0.5;
  gp_Pnt aPBefore = aSurf.EvalD0(aU, aV);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(aU, aV);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_AHTBezierSurfaceTest, Copy_Independent)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_AHTBezierSurface* aCopySurf =
    dynamic_cast<const GeomEval_AHTBezierSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_EQ(aCopySurf->NbPolesU(), 3);
  EXPECT_EQ(aCopySurf->NbPolesV(), 3);
  EXPECT_EQ(aCopySurf->AlgDegreeU(), 2);
}

// Test DumpJson does not crash
TEST(GeomEval_AHTBezierSurfaceTest, DumpJson_NoCrash)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// Test EvalD2 components match finite differences of D1
TEST(GeomEval_AHTBezierSurfaceTest, EvalD2_ConsistentWithD1)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  const double aU = 0.3, aV = 0.4;

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);

  // D2U: finite difference of D1U w.r.t. U
  Geom_Surface::ResD1 aD1Uplus = aSurf.EvalD1(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD1 aD1Uminus = aSurf.EvalD1(aU - Precision::Confusion(), aV);
  gp_Vec aFD_D2U((aD1Uplus.D1U.XYZ() - aD1Uminus.D1U.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2U.X(), aFD_D2U.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2U.Y(), aFD_D2U.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2U.Z(), aFD_D2U.Z(), THE_FD_TOL_D2);

  // D2V: finite difference of D1V w.r.t. V
  Geom_Surface::ResD1 aD1Vplus = aSurf.EvalD1(aU, aV + Precision::Confusion());
  Geom_Surface::ResD1 aD1Vminus = aSurf.EvalD1(aU, aV - Precision::Confusion());
  gp_Vec aFD_D2V((aD1Vplus.D1V.XYZ() - aD1Vminus.D1V.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2V.X(), aFD_D2V.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2V.Y(), aFD_D2V.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2V.Z(), aFD_D2V.Z(), THE_FD_TOL_D2);

  // D2UV: finite difference of D1U w.r.t. V
  gp_Vec aFD_D2UV((aD1Vplus.D1U.XYZ() - aD1Vminus.D1U.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2UV.X(), aFD_D2UV.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2UV.Y(), aFD_D2UV.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD2.D2UV.Z(), aFD_D2UV.Z(), THE_FD_TOL_D2);
}

// Test EvalD3 components match finite differences of D2
TEST(GeomEval_AHTBezierSurfaceTest, EvalD3_ConsistentWithD2)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  const double aU = 0.4, aV = 0.3;

  Geom_Surface::ResD3 aD3 = aSurf.EvalD3(aU, aV);

  // D3U: finite difference of D2U w.r.t. U
  Geom_Surface::ResD2 aD2Uplus = aSurf.EvalD2(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD2 aD2Uminus = aSurf.EvalD2(aU - Precision::Confusion(), aV);
  gp_Vec aFD_D3U((aD2Uplus.D2U.XYZ() - aD2Uminus.D2U.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3U.X(), aFD_D3U.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3U.Y(), aFD_D3U.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3U.Z(), aFD_D3U.Z(), THE_FD_TOL_D2);

  // D3V: finite difference of D2V w.r.t. V
  Geom_Surface::ResD2 aD2Vplus = aSurf.EvalD2(aU, aV + Precision::Confusion());
  Geom_Surface::ResD2 aD2Vminus = aSurf.EvalD2(aU, aV - Precision::Confusion());
  gp_Vec aFD_D3V((aD2Vplus.D2V.XYZ() - aD2Vminus.D2V.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3V.X(), aFD_D3V.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3V.Y(), aFD_D3V.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3V.Z(), aFD_D3V.Z(), THE_FD_TOL_D2);

  // D3UUV: finite difference of D2U w.r.t. V
  gp_Vec aFD_D3UUV((aD2Vplus.D2U.XYZ() - aD2Vminus.D2U.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3UUV.X(), aFD_D3UUV.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3UUV.Y(), aFD_D3UUV.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3UUV.Z(), aFD_D3UUV.Z(), THE_FD_TOL_D2);

  // D3UVV: finite difference of D2V w.r.t. U
  gp_Vec aFD_D3UVV((aD2Uplus.D2V.XYZ() - aD2Uminus.D2V.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3UVV.X(), aFD_D3UVV.X(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3UVV.Y(), aFD_D3UVV.Y(), THE_FD_TOL_D2);
  EXPECT_NEAR(aD3.D3UVV.Z(), aFD_D3UVV.Z(), THE_FD_TOL_D2);
}

// Test EvalDN(u,v,1,0) and EvalDN(u,v,0,1) match D1U and D1V from EvalD1
TEST(GeomEval_AHTBezierSurfaceTest, EvalDN_ConsistentWithD1)
{
  GeomEval_AHTBezierSurface aSurf = createPolynomialSurface();
  const double aU = 0.5, aV = 0.5;

  Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);
  gp_Vec aDN_U = aSurf.EvalDN(aU, aV, 1, 0);
  gp_Vec aDN_V = aSurf.EvalDN(aU, aV, 0, 1);

  EXPECT_NEAR(aD1.D1U.X(), aDN_U.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1U.Y(), aDN_U.Y(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1U.Z(), aDN_U.Z(), Precision::Confusion());

  EXPECT_NEAR(aD1.D1V.X(), aDN_V.X(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1V.Y(), aDN_V.Y(), Precision::Confusion());
  EXPECT_NEAR(aD1.D1V.Z(), aDN_V.Z(), Precision::Confusion());
}

TEST(GeomEval_AHTBezierSurfaceTest, RationalUniformScale_Invariant)
{
  GeomEval_AHTBezierSurface aRatW1 = createPolynomialSurfaceRationalUniformWeight(1.0);
  GeomEval_AHTBezierSurface aRatW5 = createPolynomialSurfaceRationalUniformWeight(5.0);
  const double              aU     = 0.37;
  const double              aV     = 0.62;

  EXPECT_NEAR(aRatW1.EvalD0(aU, aV).Distance(aRatW5.EvalD0(aU, aV)), 0.0, 1e-12);

  const Geom_Surface::ResD1 aD1W1 = aRatW1.EvalD1(aU, aV);
  const Geom_Surface::ResD1 aD1W5 = aRatW5.EvalD1(aU, aV);
  EXPECT_NEAR(vecDiff(aD1W1.D1U, aD1W5.D1U), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD1W1.D1V, aD1W5.D1V), 0.0, 1e-12);

  const Geom_Surface::ResD2 aD2W1 = aRatW1.EvalD2(aU, aV);
  const Geom_Surface::ResD2 aD2W5 = aRatW5.EvalD2(aU, aV);
  EXPECT_NEAR(vecDiff(aD2W1.D2U, aD2W5.D2U), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD2W1.D2V, aD2W5.D2V), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD2W1.D2UV, aD2W5.D2UV), 0.0, 1e-12);

  const Geom_Surface::ResD3 aD3W1 = aRatW1.EvalD3(aU, aV);
  const Geom_Surface::ResD3 aD3W5 = aRatW5.EvalD3(aU, aV);
  EXPECT_NEAR(vecDiff(aD3W1.D3U, aD3W5.D3U), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD3W1.D3V, aD3W5.D3V), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD3W1.D3UUV, aD3W5.D3UUV), 0.0, 1e-12);
  EXPECT_NEAR(vecDiff(aD3W1.D3UVV, aD3W5.D3UVV), 0.0, 1e-12);

  EXPECT_NEAR(vecDiff(aRatW1.EvalDN(aU, aV, 2, 1), aRatW5.EvalDN(aU, aV, 2, 1)), 0.0, 1e-12);
}
