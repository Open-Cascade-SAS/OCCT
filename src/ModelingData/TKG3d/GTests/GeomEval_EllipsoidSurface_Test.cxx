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
#include <GeomEval_EllipsoidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL  = 1e-5;
} // namespace

// Test construction with valid parameters
TEST(GeomEval_EllipsoidSurfaceTest, Construction_ValidParams)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  EXPECT_NEAR(aSurf.SemiAxisA(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.SemiAxisB(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.SemiAxisC(), 1.0, Precision::Confusion());
}

// Test construction throws for invalid semi-axes
TEST(GeomEval_EllipsoidSurfaceTest, Construction_InvalidAxes_Throws)
{
  gp_Ax3 anAx3;
  EXPECT_THROW(GeomEval_EllipsoidSurface(anAx3, 0.0, 1.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_EllipsoidSurface(anAx3, 1.0, -1.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_EllipsoidSurface(anAx3, 1.0, 1.0, 0.0), Standard_ConstructionError);
}

// Test EvalD0 at u=0, v=0: P = (A*cos(0)*cos(0), B*cos(0)*sin(0), C*sin(0)) = (A, 0, 0)
TEST(GeomEval_EllipsoidSurfaceTest, EvalD0_XAxis)
{
  gp_Ax3 anAx3;
  const double aA = 3.0, aB = 2.0, aC = 1.0;
  GeomEval_EllipsoidSurface aSurf(anAx3, aA, aB, aC);
  gp_Pnt aP = aSurf.EvalD0(0.0, 0.0);
  EXPECT_NEAR(aP.X(), aA, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 0.0, Precision::Confusion());
}

// Test EvalD0 at u=PI/2, v=0: P = (0, B, 0)
TEST(GeomEval_EllipsoidSurfaceTest, EvalD0_YAxis)
{
  gp_Ax3 anAx3;
  const double aA = 3.0, aB = 2.0, aC = 1.0;
  GeomEval_EllipsoidSurface aSurf(anAx3, aA, aB, aC);
  gp_Pnt aP = aSurf.EvalD0(M_PI / 2.0, 0.0);
  EXPECT_NEAR(aP.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), aB, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 0.0, Precision::Confusion());
}

// Test EvalD0 at u=0, v=PI/2: P = (0, 0, C)
TEST(GeomEval_EllipsoidSurfaceTest, EvalD0_ZAxis)
{
  gp_Ax3 anAx3;
  const double aA = 3.0, aB = 2.0, aC = 1.0;
  GeomEval_EllipsoidSurface aSurf(anAx3, aA, aB, aC);
  gp_Pnt aP = aSurf.EvalD0(0.0, M_PI / 2.0);
  EXPECT_NEAR(aP.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), aC, Precision::Confusion());
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
TEST(GeomEval_EllipsoidSurfaceTest, EvalD1_ConsistentWithD0)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  const double aU = 1.0, aV = 0.5;

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
TEST(GeomEval_EllipsoidSurfaceTest, Bounds_Periodicity)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 1.0, 1.0, 1.0);
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aU2, 2.0 * M_PI, Precision::Confusion());
  EXPECT_NEAR(aV1, -M_PI / 2.0, Precision::Confusion());
  EXPECT_NEAR(aV2, M_PI / 2.0, Precision::Confusion());
  EXPECT_TRUE(aSurf.IsUPeriodic());
  EXPECT_TRUE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_FALSE(aSurf.IsVClosed());
}

TEST(GeomEval_EllipsoidSurfaceTest, Iso_NotImplemented)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  EXPECT_THROW(aSurf.UIso(0.5), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VIso(0.5), Standard_NotImplemented);
}

// Test implicit equation at evaluated points
TEST(GeomEval_EllipsoidSurfaceTest, Coefficients_SatisfiedAtEvalPoints)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.5);
  double aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
  aSurf.Coefficients(aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);

  for (double u = 0.0; u < 6.0; u += 1.0)
  {
    for (double v = -1.0; v <= 1.0; v += 0.5)
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
TEST(GeomEval_EllipsoidSurfaceTest, Transform_PreservesEvaluation)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  gp_Pnt aPBefore = aSurf.EvalD0(1.0, 0.5);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(1.0, 0.5);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_EllipsoidSurfaceTest, Copy_Independent)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_EllipsoidSurface* aCopySurf =
    dynamic_cast<const GeomEval_EllipsoidSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_NEAR(aCopySurf->SemiAxisA(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCopySurf->SemiAxisB(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCopySurf->SemiAxisC(), 1.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(GeomEval_EllipsoidSurfaceTest, DumpJson_NoCrash)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// When A=B=C=R the ellipsoid degenerates to a sphere; EvalD0 must match
TEST(GeomEval_EllipsoidSurfaceTest, EvalD0_MatchesSphere)
{
  gp_Ax3 anAx3;
  const double aR = 5.0;
  GeomEval_EllipsoidSurface anEllipsoid(anAx3, aR, aR, aR);
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAx3, aR);

  const double aParams[][2] = {
    {0.0, 0.0},
    {M_PI / 4.0, M_PI / 4.0},
    {M_PI / 2.0, 0.0},
    {M_PI, -M_PI / 4.0},
    {3.0 * M_PI / 2.0, 0.0},
    {1.0, 0.3}
  };

  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];
    gp_Pnt aPE = anEllipsoid.EvalD0(aU, aV);
    gp_Pnt aPS = aSphere->EvalD0(aU, aV);
    EXPECT_NEAR(aPE.X(), aPS.X(), Precision::Confusion());
    EXPECT_NEAR(aPE.Y(), aPS.Y(), Precision::Confusion());
    EXPECT_NEAR(aPE.Z(), aPS.Z(), Precision::Confusion());
  }
}

// When A=B=C=R, EvalD1 must match the sphere
TEST(GeomEval_EllipsoidSurfaceTest, EvalD1_MatchesSphere)
{
  gp_Ax3 anAx3;
  const double aR = 5.0;
  GeomEval_EllipsoidSurface anEllipsoid(anAx3, aR, aR, aR);
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAx3, aR);

  const double aParams[][2] = {
    {0.0, 0.0},
    {M_PI / 4.0, M_PI / 4.0},
    {1.0, 0.3},
    {M_PI, -M_PI / 4.0}
  };

  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];
    Geom_Surface::ResD1 aDE = anEllipsoid.EvalD1(aU, aV);
    Geom_Surface::ResD1 aDS = aSphere->EvalD1(aU, aV);

    EXPECT_NEAR(aDE.Point.X(), aDS.Point.X(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Y(), aDS.Point.Y(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Z(), aDS.Point.Z(), Precision::Confusion());

    EXPECT_NEAR(aDE.D1U.X(), aDS.D1U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Y(), aDS.D1U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Z(), aDS.D1U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D1V.X(), aDS.D1V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Y(), aDS.D1V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Z(), aDS.D1V.Z(), Precision::Angular());
  }
}

// When A=B=C=R, EvalD2 must match the sphere
TEST(GeomEval_EllipsoidSurfaceTest, EvalD2_MatchesSphere)
{
  gp_Ax3 anAx3;
  const double aR = 5.0;
  GeomEval_EllipsoidSurface anEllipsoid(anAx3, aR, aR, aR);
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAx3, aR);

  const double aParams[][2] = {
    {0.0, 0.0},
    {M_PI / 4.0, M_PI / 4.0},
    {1.0, 0.3}
  };

  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];
    Geom_Surface::ResD2 aDE = anEllipsoid.EvalD2(aU, aV);
    Geom_Surface::ResD2 aDS = aSphere->EvalD2(aU, aV);

    EXPECT_NEAR(aDE.Point.X(), aDS.Point.X(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Y(), aDS.Point.Y(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Z(), aDS.Point.Z(), Precision::Confusion());

    EXPECT_NEAR(aDE.D1U.X(), aDS.D1U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Y(), aDS.D1U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Z(), aDS.D1U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D1V.X(), aDS.D1V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Y(), aDS.D1V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Z(), aDS.D1V.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2U.X(), aDS.D2U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2U.Y(), aDS.D2U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2U.Z(), aDS.D2U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2V.X(), aDS.D2V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2V.Y(), aDS.D2V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2V.Z(), aDS.D2V.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2UV.X(), aDS.D2UV.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2UV.Y(), aDS.D2UV.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2UV.Z(), aDS.D2UV.Z(), Precision::Angular());
  }
}

// When A=B=C=R, EvalD3 must match the sphere
TEST(GeomEval_EllipsoidSurfaceTest, EvalD3_MatchesSphere)
{
  gp_Ax3 anAx3;
  const double aR = 5.0;
  GeomEval_EllipsoidSurface anEllipsoid(anAx3, aR, aR, aR);
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAx3, aR);

  const double aParams[][2] = {
    {M_PI / 4.0, M_PI / 4.0},
    {1.0, 0.3}
  };

  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];
    Geom_Surface::ResD3 aDE = anEllipsoid.EvalD3(aU, aV);
    Geom_Surface::ResD3 aDS = aSphere->EvalD3(aU, aV);

    EXPECT_NEAR(aDE.Point.X(), aDS.Point.X(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Y(), aDS.Point.Y(), Precision::Confusion());
    EXPECT_NEAR(aDE.Point.Z(), aDS.Point.Z(), Precision::Confusion());

    EXPECT_NEAR(aDE.D1U.X(), aDS.D1U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Y(), aDS.D1U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1U.Z(), aDS.D1U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D1V.X(), aDS.D1V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Y(), aDS.D1V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D1V.Z(), aDS.D1V.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2U.X(), aDS.D2U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2U.Y(), aDS.D2U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2U.Z(), aDS.D2U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2V.X(), aDS.D2V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2V.Y(), aDS.D2V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2V.Z(), aDS.D2V.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D2UV.X(), aDS.D2UV.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D2UV.Y(), aDS.D2UV.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D2UV.Z(), aDS.D2UV.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D3U.X(), aDS.D3U.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D3U.Y(), aDS.D3U.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D3U.Z(), aDS.D3U.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D3V.X(), aDS.D3V.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D3V.Y(), aDS.D3V.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D3V.Z(), aDS.D3V.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D3UUV.X(), aDS.D3UUV.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D3UUV.Y(), aDS.D3UUV.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D3UUV.Z(), aDS.D3UUV.Z(), Precision::Angular());

    EXPECT_NEAR(aDE.D3UVV.X(), aDS.D3UVV.X(), Precision::Angular());
    EXPECT_NEAR(aDE.D3UVV.Y(), aDS.D3UVV.Y(), Precision::Angular());
    EXPECT_NEAR(aDE.D3UVV.Z(), aDS.D3UVV.Z(), Precision::Angular());
  }
}

// Verify D2 components are consistent with finite differences of D1
TEST(GeomEval_EllipsoidSurfaceTest, EvalD2_ConsistentWithD1)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  const double aU = 1.0, aV = 0.5;

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);

  // D2U ~ (D1U(u+h,v) - D1U(u-h,v)) / (2h)
  Geom_Surface::ResD1 aD1Up = aSurf.EvalD1(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD1 aD1Um = aSurf.EvalD1(aU - Precision::Confusion(), aV);
  gp_Vec aFD_D2U((aD1Up.D1U.XYZ() - aD1Um.D1U.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD2.D2U.X(), aFD_D2U.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2U.Y(), aFD_D2U.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2U.Z(), aFD_D2U.Z(), THE_FD_TOL);

  // D2V ~ (D1V(u,v+h) - D1V(u,v-h)) / (2h)
  Geom_Surface::ResD1 aD1Vp = aSurf.EvalD1(aU, aV + Precision::Confusion());
  Geom_Surface::ResD1 aD1Vm = aSurf.EvalD1(aU, aV - Precision::Confusion());
  gp_Vec aFD_D2V((aD1Vp.D1V.XYZ() - aD1Vm.D1V.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD2.D2V.X(), aFD_D2V.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2V.Y(), aFD_D2V.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2V.Z(), aFD_D2V.Z(), THE_FD_TOL);

  // D2UV ~ (D1U(u,v+h) - D1U(u,v-h)) / (2h)
  gp_Vec aFD_D2UV((aD1Vp.D1U.XYZ() - aD1Vm.D1U.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD2.D2UV.X(), aFD_D2UV.X(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2UV.Y(), aFD_D2UV.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD2.D2UV.Z(), aFD_D2UV.Z(), THE_FD_TOL);
}

// Verify D3 components are consistent with finite differences of D2
TEST(GeomEval_EllipsoidSurfaceTest, EvalD3_ConsistentWithD2)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  const double aU = 1.0, aV = 0.5;

  Geom_Surface::ResD3 aD3 = aSurf.EvalD3(aU, aV);

  // D3U ~ (D2U(u+h,v) - D2U(u-h,v)) / (2h)
  Geom_Surface::ResD2 aD2Up = aSurf.EvalD2(aU + Precision::Confusion(), aV);
  Geom_Surface::ResD2 aD2Um = aSurf.EvalD2(aU - Precision::Confusion(), aV);
  gp_Vec aFD_D3U((aD2Up.D2U.XYZ() - aD2Um.D2U.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD3.D3U.X(), aFD_D3U.X(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3U.Y(), aFD_D3U.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3U.Z(), aFD_D3U.Z(), THE_FD_TOL);

  // D3V ~ (D2V(u,v+h) - D2V(u,v-h)) / (2h)
  Geom_Surface::ResD2 aD2Vp = aSurf.EvalD2(aU, aV + Precision::Confusion());
  Geom_Surface::ResD2 aD2Vm = aSurf.EvalD2(aU, aV - Precision::Confusion());
  gp_Vec aFD_D3V((aD2Vp.D2V.XYZ() - aD2Vm.D2V.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD3.D3V.X(), aFD_D3V.X(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3V.Y(), aFD_D3V.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3V.Z(), aFD_D3V.Z(), THE_FD_TOL);

  // D3UUV ~ (D2U(u,v+h) - D2U(u,v-h)) / (2h)  i.e. d/dv of D2U
  gp_Vec aFD_D3UUV((aD2Vp.D2U.XYZ() - aD2Vm.D2U.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD3.D3UUV.X(), aFD_D3UUV.X(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3UUV.Y(), aFD_D3UUV.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3UUV.Z(), aFD_D3UUV.Z(), THE_FD_TOL);

  // D3UVV ~ (D2V(u+h,v) - D2V(u-h,v)) / (2h)  i.e. d/du of D2V
  gp_Vec aFD_D3UVV((aD2Up.D2V.XYZ() - aD2Um.D2V.XYZ()) / (2.0 * Precision::Confusion()));
  EXPECT_NEAR(aD3.D3UVV.X(), aFD_D3UVV.X(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3UVV.Y(), aFD_D3UVV.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD3.D3UVV.Z(), aFD_D3UVV.Z(), THE_FD_TOL);
}

// Verify EvalDN is consistent with D1 from EvalD1
TEST(GeomEval_EllipsoidSurfaceTest, EvalDN_ConsistentWithD3)
{
  gp_Ax3 anAx3;
  GeomEval_EllipsoidSurface aSurf(anAx3, 3.0, 2.0, 1.0);
  const double aU = 1.0, aV = 0.5;

  Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);

  // EvalDN(u, v, 1, 0) must match D1U
  gp_Vec aDN10 = aSurf.EvalDN(aU, aV, 1, 0);
  EXPECT_NEAR(aDN10.X(), aD1.D1U.X(), Precision::Angular());
  EXPECT_NEAR(aDN10.Y(), aD1.D1U.Y(), Precision::Angular());
  EXPECT_NEAR(aDN10.Z(), aD1.D1U.Z(), Precision::Angular());

  // EvalDN(u, v, 0, 1) must match D1V
  gp_Vec aDN01 = aSurf.EvalDN(aU, aV, 0, 1);
  EXPECT_NEAR(aDN01.X(), aD1.D1V.X(), Precision::Angular());
  EXPECT_NEAR(aDN01.Y(), aD1.D1V.Y(), Precision::Angular());
  EXPECT_NEAR(aDN01.Z(), aD1.D1V.Z(), Precision::Angular());
}
