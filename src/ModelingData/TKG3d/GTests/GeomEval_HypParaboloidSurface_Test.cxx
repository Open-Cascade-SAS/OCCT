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

#include <Geom_BezierSurface.hxx>
#include <GeomEval_HypParaboloidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_FD_TOL  = 1e-5;
} // namespace

// Test construction with valid parameters
TEST(GeomEval_HypParaboloidSurfaceTest, Construction_ValidParams)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  EXPECT_NEAR(aSurf.SemiAxisA(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aSurf.SemiAxisB(), 3.0, Precision::Confusion());
}

// Test construction throws for invalid semi-axes
TEST(GeomEval_HypParaboloidSurfaceTest, Construction_InvalidAxes_Throws)
{
  gp_Ax3 anAx3;
  EXPECT_THROW(GeomEval_HypParaboloidSurface(anAx3, -1.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_HypParaboloidSurface(anAx3, 1.0, 0.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_HypParaboloidSurface(anAx3, 0.0, 1.0), Standard_ConstructionError);
}

// Test EvalD0 at u=0, v=0: P = O + 0*X + 0*Y + 0*Z = origin
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD0_Origin)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  gp_Pnt aP = aSurf.EvalD0(0.0, 0.0);
  EXPECT_NEAR(aP.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 0.0, Precision::Confusion());
}

// Test EvalD0 at u=1, v=0: P = (1, 0, 1/A^2)
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD0_KnownPoint_U)
{
  gp_Ax3 anAx3;
  const double aA = 2.0, aB = 3.0;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);
  gp_Pnt aP = aSurf.EvalD0(1.0, 0.0);
  EXPECT_NEAR(aP.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 1.0 / (aA * aA), Precision::Confusion()); // u^2/A^2 = 1/4
}

// Test EvalD0 at u=0, v=1: P = (0, 1, -1/B^2)
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD0_KnownPoint_V)
{
  gp_Ax3 anAx3;
  const double aA = 2.0, aB = 3.0;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);
  gp_Pnt aP = aSurf.EvalD0(0.0, 1.0);
  EXPECT_NEAR(aP.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), -1.0 / (aB * aB), Precision::Confusion()); // -v^2/B^2 = -1/9
}

// Test derivative consistency: compare EvalD1 vs finite difference of EvalD0
// Since the surface is polynomial, derivatives should be very accurate
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD1_ConsistentWithD0)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  const double aU = 1.5, aV = 0.7;

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

// Test D2 has constant Z component: d2Z/du2 = 2/A^2, d2Z/dv2 = -2/B^2
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD2_ConstantZComponent)
{
  gp_Ax3 anAx3;
  const double aA = 2.0, aB = 3.0;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);

  // Evaluate at several parameter values: D2 should be constant
  Geom_Surface::ResD2 aD2a = aSurf.EvalD2(0.0, 0.0);
  Geom_Surface::ResD2 aD2b = aSurf.EvalD2(1.0, 2.0);
  Geom_Surface::ResD2 aD2c = aSurf.EvalD2(-3.0, 5.0);

  const double aExpD2U_Z = 2.0 / (aA * aA);
  const double aExpD2V_Z = -2.0 / (aB * aB);

  EXPECT_NEAR(aD2a.D2U.Z(), aExpD2U_Z, Precision::Angular());
  EXPECT_NEAR(aD2b.D2U.Z(), aExpD2U_Z, Precision::Angular());
  EXPECT_NEAR(aD2c.D2U.Z(), aExpD2U_Z, Precision::Angular());

  EXPECT_NEAR(aD2a.D2V.Z(), aExpD2V_Z, Precision::Angular());
  EXPECT_NEAR(aD2b.D2V.Z(), aExpD2V_Z, Precision::Angular());
  EXPECT_NEAR(aD2c.D2V.Z(), aExpD2V_Z, Precision::Angular());
}

// Test Bounds: all infinite, not periodic, not closed
TEST(GeomEval_HypParaboloidSurfaceTest, Bounds_Periodicity)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 1.0, 1.0);
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_TRUE(aU1 < -1e10);
  EXPECT_TRUE(aU2 > 1e10);
  EXPECT_TRUE(aV1 < -1e10);
  EXPECT_TRUE(aV2 > 1e10);
  EXPECT_FALSE(aSurf.IsUPeriodic());
  EXPECT_FALSE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_FALSE(aSurf.IsVClosed());
}

// Test implicit equation at evaluated points
TEST(GeomEval_HypParaboloidSurfaceTest, Coefficients_SatisfiedAtEvalPoints)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  double aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
  aSurf.Coefficients(aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);

  for (double u = -2.0; u <= 2.0; u += 1.0)
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
TEST(GeomEval_HypParaboloidSurfaceTest, Transform_PreservesEvaluation)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  gp_Pnt aPBefore = aSurf.EvalD0(1.0, 1.0);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(1.0, 1.0);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_HypParaboloidSurfaceTest, Copy_Independent)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_HypParaboloidSurface* aCopySurf =
    dynamic_cast<const GeomEval_HypParaboloidSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_NEAR(aCopySurf->SemiAxisA(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCopySurf->SemiAxisB(), 3.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(GeomEval_HypParaboloidSurfaceTest, DumpJson_NoCrash)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// Test D3 components are zero for degree-2 polynomial surface
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD3_Zero)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);

  Geom_Surface::ResD3 aD3 = aSurf.EvalD3(1.0, 1.0);

  EXPECT_TRUE(aD3.D3U.Magnitude() < Precision::SquareConfusion());
  EXPECT_TRUE(aD3.D3V.Magnitude() < Precision::SquareConfusion());
  EXPECT_TRUE(aD3.D3UUV.Magnitude() < Precision::SquareConfusion());
  EXPECT_TRUE(aD3.D3UVV.Magnitude() < Precision::SquareConfusion());
}

// Test D2UV cross derivative is zero since u and v terms are separated
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD2_CrossDerivative)
{
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, 2.0, 3.0);

  const double aTestPoints[][2] = {{0.0, 0.0}, {1.5, -2.0}, {-3.0, 4.5}};
  for (const auto& aPt : aTestPoints)
  {
    Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aPt[0], aPt[1]);
    EXPECT_NEAR(aD2.D2UV.X(), 0.0, Precision::SquareConfusion());
    EXPECT_NEAR(aD2.D2UV.Y(), 0.0, Precision::SquareConfusion());
    EXPECT_NEAR(aD2.D2UV.Z(), 0.0, Precision::SquareConfusion());
  }
}

// Test D2 known values including X,Y components (should be zero)
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD2_KnownValues)
{
  gp_Ax3 anAx3;
  const double aA = 2.0, aB = 3.0;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(1.5, -2.7);

  // D2U = (0, 0, 2/A^2)
  EXPECT_NEAR(aD2.D2U.X(), 0.0, Precision::SquareConfusion());
  EXPECT_NEAR(aD2.D2U.Y(), 0.0, Precision::SquareConfusion());
  EXPECT_NEAR(aD2.D2U.Z(), 2.0 / (aA * aA), Precision::Angular());

  // D2V = (0, 0, -2/B^2)
  EXPECT_NEAR(aD2.D2V.X(), 0.0, Precision::SquareConfusion());
  EXPECT_NEAR(aD2.D2V.Y(), 0.0, Precision::SquareConfusion());
  EXPECT_NEAR(aD2.D2V.Z(), -2.0 / (aB * aB), Precision::Angular());
}

// Test EvalDN consistency with D2 and higher-order zero
TEST(GeomEval_HypParaboloidSurfaceTest, EvalDN_HigherOrder)
{
  gp_Ax3 anAx3;
  const double aA = 2.0, aB = 3.0;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);
  const double aU = 1.0, aV = 1.0;

  Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);

  // EvalDN(u,v,2,0) should match D2U
  gp_Vec aDN20 = aSurf.EvalDN(aU, aV, 2, 0);
  EXPECT_NEAR(aDN20.X(), aD2.D2U.X(), Precision::Confusion());
  EXPECT_NEAR(aDN20.Y(), aD2.D2U.Y(), Precision::Confusion());
  EXPECT_NEAR(aDN20.Z(), aD2.D2U.Z(), Precision::Confusion());

  // EvalDN(u,v,0,2) should match D2V
  gp_Vec aDN02 = aSurf.EvalDN(aU, aV, 0, 2);
  EXPECT_NEAR(aDN02.X(), aD2.D2V.X(), Precision::Confusion());
  EXPECT_NEAR(aDN02.Y(), aD2.D2V.Y(), Precision::Confusion());
  EXPECT_NEAR(aDN02.Z(), aD2.D2V.Z(), Precision::Confusion());

  // EvalDN(u,v,3,0) should be zero vector
  gp_Vec aDN30 = aSurf.EvalDN(aU, aV, 3, 0);
  EXPECT_TRUE(aDN30.Magnitude() < Precision::SquareConfusion());
}

// Test HypParaboloid matches bi-quadratic Bezier surface on [0,1]x[0,1].
// P(u,v) = (u, v, u^2/A^2 - v^2/B^2) is exactly representable as degree-2 Bezier.
TEST(GeomEval_HypParaboloidSurfaceTest, EvalD2_MatchesBezierSurface)
{
  const double aA = 2.0, aB = 3.0;
  gp_Ax3 anAx3;
  GeomEval_HypParaboloidSurface aSurf(anAx3, aA, aB);

  // Build equivalent bi-quadratic Bezier surface.
  // Bernstein coefficients: u -> {0, 0.5, 1}, u^2 -> {0, 0, 1}
  // Z_poles(i,j) = z_u(i) + z_v(j) where z_u = {0, 0, 1/A^2}, z_v = {0, 0, -1/B^2}
  const double aInvA2 = 1.0 / (aA * aA);
  const double aInvB2 = 1.0 / (aB * aB);

  NCollection_Array2<gp_Pnt> aBPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    const double aXi = 0.5 * (i - 1); // {0, 0.5, 1}
    const double aZu = (i == 3) ? aInvA2 : 0.0; // Bernstein coeff of u^2
    for (int j = 1; j <= 3; ++j)
    {
      const double aYj = 0.5 * (j - 1); // {0, 0.5, 1}
      const double aZv = (j == 3) ? -aInvB2 : 0.0; // Bernstein coeff of -v^2
      aBPoles.SetValue(i, j, gp_Pnt(aXi, aYj, aZu + aZv));
    }
  }
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aBPoles);

  const double aParams[][2] = {{0.0, 0.0}, {0.25, 0.25}, {0.5, 0.5},
                                {0.75, 0.25}, {0.3, 0.7}, {1.0, 1.0}};
  for (const auto& aUV : aParams)
  {
    Geom_Surface::ResD2 aD2H = aSurf.EvalD2(aUV[0], aUV[1]);
    Geom_Surface::ResD2 aD2B = aBezier->EvalD2(aUV[0], aUV[1]);

    EXPECT_NEAR(aD2H.Point.Distance(aD2B.Point), 0.0, Precision::Angular())
      << "Point mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD2H.D1U.X(), aD2B.D1U.X(), Precision::Angular());
    EXPECT_NEAR(aD2H.D1U.Y(), aD2B.D1U.Y(), Precision::Angular());
    EXPECT_NEAR(aD2H.D1U.Z(), aD2B.D1U.Z(), Precision::Angular());
    EXPECT_NEAR(aD2H.D1V.X(), aD2B.D1V.X(), Precision::Angular());
    EXPECT_NEAR(aD2H.D1V.Y(), aD2B.D1V.Y(), Precision::Angular());
    EXPECT_NEAR(aD2H.D1V.Z(), aD2B.D1V.Z(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2U.X(), aD2B.D2U.X(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2U.Y(), aD2B.D2U.Y(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2U.Z(), aD2B.D2U.Z(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2V.X(), aD2B.D2V.X(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2V.Y(), aD2B.D2V.Y(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2V.Z(), aD2B.D2V.Z(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2UV.X(), aD2B.D2UV.X(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2UV.Y(), aD2B.D2UV.Y(), Precision::Angular());
    EXPECT_NEAR(aD2H.D2UV.Z(), aD2B.D2UV.Z(), Precision::Angular());
  }
}
