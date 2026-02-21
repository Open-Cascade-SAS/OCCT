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

#include <GeomEval_CircularHelicoidSurface.hxx>
#include <GeomEval_CircularHelixCurve.hxx>
#include <gp_Ax2.hxx>
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
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(GeomEval_CircularHelicoidSurfaceTest, Construction_ValidParams)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  EXPECT_NEAR(aSurf.Pitch(), 10.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelicoidSurfaceTest, Construction_ZeroPitch_Throws)
{
  gp_Ax3 anAx3;
  EXPECT_THROW(GeomEval_CircularHelicoidSurface(anAx3, 0.0), Standard_ConstructionError);
}

TEST(GeomEval_CircularHelicoidSurfaceTest, Construction_NegativePitch_NoThrow)
{
  gp_Ax3 anAx3;
  EXPECT_NO_THROW(GeomEval_CircularHelicoidSurface(anAx3, -5.0));
}

TEST(GeomEval_CircularHelicoidSurfaceTest, EvalD0_AtKnownPoints)
{
  gp_Ax3 anAx3;
  const double aP = 10.0;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, aP);

  // S(0, 1) = (1, 0, 0)
  gp_Pnt aP1 = aSurf.EvalD0(0.0, 1.0);
  EXPECT_NEAR(aP1.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP1.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP1.Z(), 0.0, Precision::Confusion());

  // S(Pi/2, 1) = (0, 1, P/4)
  gp_Pnt aP2 = aSurf.EvalD0(M_PI / 2.0, 1.0);
  EXPECT_NEAR(aP2.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP2.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP2.Z(), aP / 4.0, Precision::Confusion());

  // S(u, 0) = (0, 0, P*u/(2*Pi)) -- on the axis
  gp_Pnt aP3 = aSurf.EvalD0(1.0, 0.0);
  EXPECT_NEAR(aP3.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP3.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP3.Z(), aP / (2.0 * M_PI), Precision::Confusion());
}

TEST(GeomEval_CircularHelicoidSurfaceTest, D2V_IsZero_RuledSurface)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);

  const double aParams[][2] = {{0.0, 1.0}, {1.0, 2.0}, {M_PI, -1.0}};
  for (const auto& aUV : aParams)
  {
    Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aUV[0], aUV[1]);
    EXPECT_NEAR(aD2.D2V.Magnitude(), 0.0, Precision::Confusion());
  }
}

TEST(GeomEval_CircularHelicoidSurfaceTest, D1V_UnitRadial)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);

  // |dS/dv| = |cos(u)*XDir + sin(u)*YDir| = 1
  const double aParams[] = {0.0, 1.0, M_PI / 2.0, M_PI, 3.5};
  for (double aU : aParams)
  {
    Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, 2.0);
    EXPECT_NEAR(aD1.D1V.Magnitude(), 1.0, Precision::Confusion());
  }
}

TEST(GeomEval_CircularHelicoidSurfaceTest, ComparisonWithHelix_ConstantV)
{
  gp_Ax3 anAx3;
  gp_Ax2 anAx2(anAx3.Location(), anAx3.Direction(), anAx3.XDirection());
  const double aP = 10.0, aR = 3.0;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, aP);
  GeomEval_CircularHelixCurve aHelix(anAx2, aR, aP);

  // S(u, R) should trace the same path as the helix C(u)
  const double aParams[] = {0.0, 0.5, 1.0, M_PI, 2.0 * M_PI};
  for (double aU : aParams)
  {
    gp_Pnt aPS = aSurf.EvalD0(aU, aR);
    gp_Pnt aPH = aHelix.EvalD0(aU);
    EXPECT_NEAR(aPS.Distance(aPH), 0.0, Precision::Confusion());
  }
}

TEST(GeomEval_CircularHelicoidSurfaceTest, EvalD1_ConsistentWithD0)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  const double aU = 1.0, aV = 2.0;

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

TEST(GeomEval_CircularHelicoidSurfaceTest, Bounds_Infinite)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  double aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_TRUE(Precision::IsInfinite(aU1));
  EXPECT_TRUE(Precision::IsInfinite(aU2));
  EXPECT_TRUE(Precision::IsInfinite(aV1));
  EXPECT_TRUE(Precision::IsInfinite(aV2));
}

TEST(GeomEval_CircularHelicoidSurfaceTest, Properties)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  EXPECT_FALSE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVClosed());
  EXPECT_FALSE(aSurf.IsUPeriodic());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_THROW(aSurf.UIso(0.0), Standard_NotImplemented);
  EXPECT_THROW(aSurf.VIso(0.0), Standard_NotImplemented);
}

TEST(GeomEval_CircularHelicoidSurfaceTest, Transform_PreservesEvaluation)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  gp_Pnt aPBefore = aSurf.EvalD0(1.0, 2.0);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(1.0, 2.0);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelicoidSurfaceTest, Copy_Independent)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_CircularHelicoidSurface* aCopySurf =
    dynamic_cast<const GeomEval_CircularHelicoidSurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_NEAR(aCopySurf->Pitch(), 10.0, Precision::Confusion());
}

TEST(GeomEval_CircularHelicoidSurfaceTest, DumpJson_NoCrash)
{
  gp_Ax3 anAx3;
  GeomEval_CircularHelicoidSurface aSurf(anAx3, 10.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}
