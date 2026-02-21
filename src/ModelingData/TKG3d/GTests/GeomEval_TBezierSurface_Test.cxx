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

#include <Geom_SphericalSurface.hxx>
#include <GeomEval_TBezierSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{

constexpr double THE_FD_TOL_D1 = 1e-5;
constexpr double THE_FD_TOL_D2 = 1e-4;

// Helper to create a T-Bezier surface representing a unit sphere patch.
// Basis: {1, sin(u), cos(u)} x {1, sin(v), cos(v)} with alpha=1.
// S(u,v) = cos(u)*cos(v)*X + sin(u)*cos(v)*Y + sin(v)*Z, domain [0, Pi]x[0, Pi].
GeomEval_TBezierSurface createSpherePatch()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  // All poles default to origin (0,0,0)
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(0.0, 0.0, 0.0));
  // P(1,2) = (0,0,1) corresponds to 1*sin(v) -> Z component
  aPoles.SetValue(1, 2, gp_Pnt(0.0, 0.0, 1.0));
  // P(2,3) = (0,1,0) corresponds to sin(u)*cos(v) -> Y component
  aPoles.SetValue(2, 3, gp_Pnt(0.0, 1.0, 0.0));
  // P(3,3) = (1,0,0) corresponds to cos(u)*cos(v) -> X component
  aPoles.SetValue(3, 3, gp_Pnt(1.0, 0.0, 0.0));
  return GeomEval_TBezierSurface(aPoles, 1.0, 1.0);
}

// Helper to create a simple 3x3 (order 1 in both directions) T-Bezier surface
GeomEval_TBezierSurface createSimpleSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(double(i - 1), double(j - 1), 0.0));
    }
  }
  return GeomEval_TBezierSurface(aPoles, 1.0, 1.0);
}

} // namespace

// Test construction with valid parameters
TEST(GeomEval_TBezierSurfaceTest, Construction_ValidParams)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  EXPECT_EQ(aSurf.NbUPoles(), 3);
  EXPECT_EQ(aSurf.NbVPoles(), 3);
  EXPECT_EQ(aSurf.OrderU(), 1);
  EXPECT_EQ(aSurf.OrderV(), 1);
  EXPECT_NEAR(aSurf.AlphaU(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.AlphaV(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aSurf.IsRational());
}

// Test construction throws for invalid parameters
TEST(GeomEval_TBezierSurfaceTest, Construction_InvalidParams_Throws)
{
  // Even number of poles in U (not odd)
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
  for (int i = 1; i <= 2; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(double(i), double(j), 0.0));
  EXPECT_THROW(GeomEval_TBezierSurface(aPoles, 1.0, 1.0), Standard_ConstructionError);
}

// Test parameter range
TEST(GeomEval_TBezierSurfaceTest, ParameterRange)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aU2, M_PI, Precision::Confusion()); // Pi/alphaU = Pi/1
  EXPECT_NEAR(aV1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aV2, M_PI, Precision::Confusion()); // Pi/alphaV = Pi/1
}

// Test EvalD0 at corners
TEST(GeomEval_TBezierSurfaceTest, EvalD0_Corners)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);

  // The corner evaluations should be well-defined points
  gp_Pnt aP00 = aSurf.EvalD0(aU1, aV1);
  gp_Pnt aP10 = aSurf.EvalD0(aU2, aV1);
  gp_Pnt aP01 = aSurf.EvalD0(aU1, aV2);
  gp_Pnt aP11 = aSurf.EvalD0(aU2, aV2);

  // Verify points are distinct (surface is not degenerate)
  EXPECT_TRUE(aP00.Distance(aP10) > Precision::Confusion());
  EXPECT_TRUE(aP00.Distance(aP01) > Precision::Confusion());
  EXPECT_TRUE(aP00.Distance(aP11) > Precision::Confusion());
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_TBezierSurfaceTest, EvalD1_ConsistentWithD0)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  const double aU = M_PI / 3.0, aV = M_PI / 4.0;

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
TEST(GeomEval_TBezierSurfaceTest, Periodicity)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  EXPECT_FALSE(aSurf.IsUPeriodic());
  EXPECT_FALSE(aSurf.IsVPeriodic());
}

// Test Transform preserves evaluation
TEST(GeomEval_TBezierSurfaceTest, Transform_PreservesEvaluation)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  const double aU = M_PI / 4.0, aV = M_PI / 4.0;
  gp_Pnt aPBefore = aSurf.EvalD0(aU, aV);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(aU, aV);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_TBezierSurfaceTest, Copy_Independent)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_TBezierSurface* aCopySurf =
    dynamic_cast<const GeomEval_TBezierSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_EQ(aCopySurf->NbUPoles(), 3);
  EXPECT_EQ(aCopySurf->NbVPoles(), 3);
}

// Test DumpJson does not crash
TEST(GeomEval_TBezierSurfaceTest, DumpJson_NoCrash)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// Test EvalD2 components match finite differences of D1
TEST(GeomEval_TBezierSurfaceTest, EvalD2_ConsistentWithD1)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  const double aU = M_PI / 4.0, aV = M_PI / 4.0;

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
TEST(GeomEval_TBezierSurfaceTest, EvalD3_ConsistentWithD2)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  const double aU = M_PI / 3.0, aV = M_PI / 3.0;

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
TEST(GeomEval_TBezierSurfaceTest, EvalDN_ConsistentWithD1)
{
  GeomEval_TBezierSurface aSurf = createSimpleSurface();
  const double aU = M_PI / 4.0, aV = M_PI / 3.0;

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

// Test T-Bezier sphere patch matches Geom_SphericalSurface D0
TEST(GeomEval_TBezierSurfaceTest, EvalD0_MatchesSphere)
{
  GeomEval_TBezierSurface aTBez = createSpherePatch();
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp_Ax3(), 1.0);

  const double aUParams[] = {0.1, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0,
                              M_PI / 2.0, 2.0 * M_PI / 3.0, 2.5};
  const double aVParams[] = {0.1, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, 1.2};
  for (const double aU : aUParams)
  {
    for (const double aV : aVParams)
    {
      gp_Pnt aPT = aTBez.EvalD0(aU, aV);
      gp_Pnt aPS = aSphere->EvalD0(aU, aV);
      EXPECT_NEAR(aPT.Distance(aPS), 0.0, Precision::Angular())
        << "D0 mismatch at u=" << aU << " v=" << aV;
    }
  }
}

// Test T-Bezier sphere patch matches Geom_SphericalSurface D1
TEST(GeomEval_TBezierSurfaceTest, EvalD1_MatchesSphere)
{
  GeomEval_TBezierSurface aTBez = createSpherePatch();
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp_Ax3(), 1.0);

  const double aUParams[] = {M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 2.0 * M_PI / 3.0};
  const double aVParams[] = {0.2, M_PI / 4.0, M_PI / 3.0, 1.0};
  for (const double aU : aUParams)
  {
    for (const double aV : aVParams)
    {
      Geom_Surface::ResD1 aDT = aTBez.EvalD1(aU, aV);
      Geom_Surface::ResD1 aDS = aSphere->EvalD1(aU, aV);

      EXPECT_NEAR(aDT.Point.Distance(aDS.Point), 0.0, Precision::Angular());
      EXPECT_NEAR(aDT.D1U.X(), aDS.D1U.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D1U.Y(), aDS.D1U.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D1U.Z(), aDS.D1U.Z(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.X(), aDS.D1V.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.Y(), aDS.D1V.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.Z(), aDS.D1V.Z(), Precision::Angular());
    }
  }
}

// Test T-Bezier sphere patch matches Geom_SphericalSurface D2
TEST(GeomEval_TBezierSurfaceTest, EvalD2_MatchesSphere)
{
  GeomEval_TBezierSurface aTBez = createSpherePatch();
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp_Ax3(), 1.0);

  const double aUParams[] = {M_PI / 6.0, M_PI / 3.0, M_PI / 2.0};
  const double aVParams[] = {0.3, M_PI / 4.0, 1.0};
  for (const double aU : aUParams)
  {
    for (const double aV : aVParams)
    {
      Geom_Surface::ResD2 aDT = aTBez.EvalD2(aU, aV);
      Geom_Surface::ResD2 aDS = aSphere->EvalD2(aU, aV);

      EXPECT_NEAR(aDT.Point.Distance(aDS.Point), 0.0, Precision::Angular());
      EXPECT_NEAR(aDT.D1U.X(), aDS.D1U.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D1U.Y(), aDS.D1U.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D1U.Z(), aDS.D1U.Z(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.X(), aDS.D1V.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.Y(), aDS.D1V.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D1V.Z(), aDS.D1V.Z(), Precision::Angular());
      EXPECT_NEAR(aDT.D2U.X(), aDS.D2U.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D2U.Y(), aDS.D2U.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D2U.Z(), aDS.D2U.Z(), Precision::Angular());
      EXPECT_NEAR(aDT.D2V.X(), aDS.D2V.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D2V.Y(), aDS.D2V.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D2V.Z(), aDS.D2V.Z(), Precision::Angular());
      EXPECT_NEAR(aDT.D2UV.X(), aDS.D2UV.X(), Precision::Angular());
      EXPECT_NEAR(aDT.D2UV.Y(), aDS.D2UV.Y(), Precision::Angular());
      EXPECT_NEAR(aDT.D2UV.Z(), aDS.D2UV.Z(), Precision::Angular());
    }
  }
}
