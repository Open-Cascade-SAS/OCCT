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

#include <GeomEval_ParaboloidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_FD_TOL    = 1e-5;
constexpr double THE_FD_TOL_D3 = 1e-4;
} // namespace

// Test construction with valid parameters
TEST(GeomEval_ParaboloidSurfaceTest, Construction_ValidParams)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  EXPECT_NEAR(aSurf.Focal(), 1.0, Precision::Confusion());
}

// Test construction throws for invalid focal
TEST(GeomEval_ParaboloidSurfaceTest, Construction_InvalidFocal_Throws)
{
  gp_Ax3 anAx3;
  EXPECT_THROW(GeomEval_ParaboloidSurface(anAx3, -1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_ParaboloidSurface(anAx3, 0.0), Standard_ConstructionError);
}

// Test EvalD0 at known point: u=0, v=0 should give origin
TEST(GeomEval_ParaboloidSurfaceTest, EvalD0_Origin)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  gp_Pnt aP = aSurf.EvalD0(0.0, 0.0);
  EXPECT_NEAR(aP.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 0.0, Precision::Confusion());
}

// Test EvalD0 at known point: u=0, v=2, F=1 -> P=(2,0,1)
TEST(GeomEval_ParaboloidSurfaceTest, EvalD0_KnownPoint)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  gp_Pnt aP = aSurf.EvalD0(0.0, 2.0);
  EXPECT_NEAR(aP.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 1.0, Precision::Confusion()); // v^2/(4*F) = 4/4 = 1
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_ParaboloidSurfaceTest, EvalD1_ConsistentWithD0)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 2.0);
  const double aU = 1.0, aV = 1.5;

  Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);
  gp_Pnt aPu1 = aSurf.EvalD0(aU + Precision::Confusion(), aV);
  gp_Pnt aPu2 = aSurf.EvalD0(aU - Precision::Confusion(), aV);
  gp_Pnt aPv1 = aSurf.EvalD0(aU, aV + Precision::Confusion());
  gp_Pnt aPv2 = aSurf.EvalD0(aU, aV - Precision::Confusion());

  gp_Vec aFDU((aPu1.XYZ() - aPu2.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFDV((aPv1.XYZ() - aPv2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1U.X(), aFDU.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1U.Y(), aFDU.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1U.Z(), aFDU.Z(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1V.X(), aFDV.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1V.Y(), aFDV.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1V.Z(), aFDV.Z(), THE_FD_TOL);
}

// Test Bounds and periodicity
TEST(GeomEval_ParaboloidSurfaceTest, Bounds_Periodicity)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aU2, 2.0 * M_PI, Precision::Confusion());
  EXPECT_TRUE(aSurf.IsUPeriodic());
  EXPECT_TRUE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_FALSE(aSurf.IsVClosed());
}

TEST(GeomEval_ParaboloidSurfaceTest, Iso_NotImplemented)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  EXPECT_THROW(aSurf.UIso(0.5), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VIso(0.5), Standard_NotImplemented);
}

TEST(GeomEval_ParaboloidSurfaceTest, Reverse_NotImplemented)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  EXPECT_THROW(aSurf.UReverse(), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VReverse(), Standard_NotImplemented);
  EXPECT_THROW(aSurf.UReversedParameter(0.5), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VReversedParameter(0.5), Standard_NotImplemented);
}

// Test implicit equation at evaluated points
TEST(GeomEval_ParaboloidSurfaceTest, Coefficients_SatisfiedAtEvalPoints)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.5);
  double aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
  aSurf.Coefficients(aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);

  for (double u = 0.0; u < 6.0; u += 1.0)
  {
    for (double v = -2.0; v <= 2.0; v += 1.0)
    {
      gp_Pnt aP = aSurf.EvalD0(u, v);
      double aX = aP.X(), aY = aP.Y(), aZ = aP.Z();
      double aVal = aA1*aX*aX + aA2*aY*aY + aA3*aZ*aZ
                  + 2.0*(aB1*aX*aY + aB2*aX*aZ + aB3*aY*aZ)
                  + 2.0*(aC1*aX + aC2*aY + aC3*aZ) + aD;
      EXPECT_NEAR(aVal, 0.0, Precision::Intersection());
    }
  }
}

// Test Transform preserves evaluation
TEST(GeomEval_ParaboloidSurfaceTest, Transform_PreservesEvaluation)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  gp_Pnt aPBefore = aSurf.EvalD0(1.0, 1.0);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(1.0, 1.0);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_ParaboloidSurfaceTest, Copy_Independent)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 2.0);
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_ParaboloidSurface* aCopySurf =
    dynamic_cast<const GeomEval_ParaboloidSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_NEAR(aCopySurf->Focal(), 2.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(GeomEval_ParaboloidSurfaceTest, DumpJson_NoCrash)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 1.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// Test D2 consistency with finite differences of D1
TEST(GeomEval_ParaboloidSurfaceTest, EvalD2_ConsistentWithD1)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 2.0);
  const double aU = 1.0, aV = 1.5;

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);

  Geom_Surface::ResD1 aD1Uf = aSurf.EvalD1(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD1 aD1Ub = aSurf.EvalD1(aU - Precision::Confusion(), aV);
  Geom_Surface::ResD1 aD1Vf = aSurf.EvalD1(aU, aV + Precision::Confusion());
  Geom_Surface::ResD1 aD1Vb = aSurf.EvalD1(aU, aV - Precision::Confusion());

  gp_Vec aFD_D2U((aD1Uf.D1U.XYZ() - aD1Ub.D1U.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFD_D2V((aD1Vf.D1V.XYZ() - aD1Vb.D1V.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFD_D2UV((aD1Vf.D1U.XYZ() - aD1Vb.D1U.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD2.D2U.X(), aFD_D2U.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2U.Y(), aFD_D2U.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2U.Z(), aFD_D2U.Z(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2V.X(), aFD_D2V.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2V.Y(), aFD_D2V.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2V.Z(), aFD_D2V.Z(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2UV.X(), aFD_D2UV.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2UV.Y(), aFD_D2UV.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2UV.Z(), aFD_D2UV.Z(), THE_FD_TOL);
}

// Test D3 consistency with finite differences of D2
TEST(GeomEval_ParaboloidSurfaceTest, EvalD3_ConsistentWithD2)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 2.0);
  const double aU = 0.7, aV = 1.2;

  Geom_Surface::ResD3 aD3 = aSurf.EvalD3(aU, aV);

  Geom_Surface::ResD2 aD2Uf = aSurf.EvalD2(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD2 aD2Ub = aSurf.EvalD2(aU - Precision::Confusion(), aV);
  Geom_Surface::ResD2 aD2Vf = aSurf.EvalD2(aU, aV + Precision::Confusion());
  Geom_Surface::ResD2 aD2Vb = aSurf.EvalD2(aU, aV - Precision::Confusion());

  gp_Vec aFD_D3U((aD2Uf.D2U.XYZ() - aD2Ub.D2U.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFD_D3V((aD2Vf.D2V.XYZ() - aD2Vb.D2V.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFD_D3UUV((aD2Vf.D2U.XYZ() - aD2Vb.D2U.XYZ()) / (2.0 * Precision::Confusion()));
  gp_Vec aFD_D3UVV((aD2Uf.D2V.XYZ() - aD2Ub.D2V.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD3.D3U.X(), aFD_D3U.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3U.Y(), aFD_D3U.Y(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3U.Z(), aFD_D3U.Z(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3V.X(), aFD_D3V.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3V.Y(), aFD_D3V.Y(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3V.Z(), aFD_D3V.Z(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UUV.X(), aFD_D3UUV.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UUV.Y(), aFD_D3UUV.Y(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UUV.Z(), aFD_D3UUV.Z(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UVV.X(), aFD_D3UVV.X(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UVV.Y(), aFD_D3UVV.Y(), THE_FD_TOL_D3);
  EXPECT_NEAR(aD3.D3UVV.Z(), aFD_D3UVV.Z(), THE_FD_TOL_D3);
}

// Test D2 known analytical values for paraboloid
// P(u,v) = v*cos(u)*X + v*sin(u)*Y + v^2/(4F)*Z
// D2UU = -v*cos(u)*X - v*sin(u)*Y => magnitude = v
TEST(GeomEval_ParaboloidSurfaceTest, EvalD2_KnownAnalytical)
{
  gp_Ax3 anAx3;
  const double aFocal = 2.0;
  GeomEval_ParaboloidSurface aSurf(anAx3, aFocal);
  const double aU = M_PI / 4.0, aV = 2.0;

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);

  // D2UU = (-v*cos(u), -v*sin(u), 0)
  const double aExpX = -aV * std::cos(aU);
  const double aExpY = -aV * std::sin(aU);
  const double aExpZ = 0.0;

  EXPECT_NEAR(aD2.D2U.X(), aExpX, Precision::Angular());
  EXPECT_NEAR(aD2.D2U.Y(), aExpY, Precision::Angular());
  EXPECT_NEAR(aD2.D2U.Z(), aExpZ, Precision::Angular());
  EXPECT_NEAR(aD2.D2U.Magnitude(), aV, Precision::Angular());
}

// Test EvalDN consistency with D1 components
TEST(GeomEval_ParaboloidSurfaceTest, EvalDN_Consistency)
{
  gp_Ax3 anAx3;
  GeomEval_ParaboloidSurface aSurf(anAx3, 2.0);
  const double aU = 1.0, aV = 1.0;

  Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);
  gp_Vec aDN10 = aSurf.EvalDN(aU, aV, 1, 0);
  gp_Vec aDN01 = aSurf.EvalDN(aU, aV, 0, 1);

  EXPECT_NEAR(aDN10.X(), aD1.D1U.X(), Precision::Confusion());
  EXPECT_NEAR(aDN10.Y(), aD1.D1U.Y(), Precision::Confusion());
  EXPECT_NEAR(aDN10.Z(), aD1.D1U.Z(), Precision::Confusion());
  EXPECT_NEAR(aDN01.X(), aD1.D1V.X(), Precision::Confusion());
  EXPECT_NEAR(aDN01.Y(), aD1.D1V.Y(), Precision::Confusion());
  EXPECT_NEAR(aDN01.Z(), aD1.D1V.Z(), Precision::Confusion());
}
