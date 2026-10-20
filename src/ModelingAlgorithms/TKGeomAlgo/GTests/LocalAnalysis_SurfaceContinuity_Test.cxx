// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gtest/gtest.h>

namespace
{
// Quadratic graph z = (theKX * x*x + theKY * y*y) / 2.
occ::handle<Geom_BezierSurface> paraboloid(double theKX, double theKY)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  const double               aSquare[3] = {1.0, -1.0, 1.0};
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) =
        gp_Pnt(i - 2.0, j - 2.0, 0.5 * (theKX * aSquare[i - 1] + theKY * aSquare[j - 1]));
    }
  }
  return new Geom_BezierSurface(aPoles);
}
} // namespace

TEST(LocalAnalysis_SurfaceContinuityTest, ExactOperatorGapAndRigidRotation)
{
  occ::handle<Geom_BezierSurface> aSurface1 = paraboloid(2.0, 1.0);
  occ::handle<Geom_BezierSurface> aSurface2 = paraboloid(2.0, 1.0);
  gp_Trsf                         aTurn;
  aTurn.SetRotation(gp_Ax1(gp::Origin(), gp::DZ()), M_PI / 4.0);
  aSurface2->Transform(aTurn);
  for (int i = 0; i < 3; ++i)
  {
    LocalAnalysis_SurfaceContinuity aCheck(aSurface1, 0.5, 0.5, aSurface2, 0.5, 0.5, GeomAbs_G2);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_TRUE(aCheck.IsG1());
    EXPECT_FALSE(aCheck.IsG2());
    EXPECT_NEAR(aCheck.G2CurvatureGap(), std::sqrt(0.5), 1.e-12);
    aTurn.SetRotation(gp_Ax1(gp::Origin(), gp_Dir(1.0, 2.0, 3.0)), 0.73);
    aSurface1->Transform(aTurn);
    aSurface2->Transform(aTurn);
  }
}

TEST(LocalAnalysis_SurfaceContinuityTest, ReversedParameterization)
{
  occ::handle<Geom_BezierSurface> aSurface1 = paraboloid(2.0, -1.0);
  occ::handle<Geom_BezierSurface> aSurface2 = paraboloid(2.0, -1.0);
  aSurface2->UReverse();
  LocalAnalysis_SurfaceContinuity aCheck(aSurface1, 0.5, 0.5, aSurface2, 0.5, 0.5, GeomAbs_G2);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_TRUE(aCheck.IsG2());
  EXPECT_NEAR(aCheck.G2CurvatureGap(), 0.0, 1.e-12);
}

TEST(LocalAnalysis_SurfaceContinuityTest, OppositeCurvaturesDoNotCancel)
{
  occ::handle<Geom_BezierSurface> aSurface1 = paraboloid(1.0, 1.0);
  occ::handle<Geom_BezierSurface> aSurface2 = paraboloid(-1.0, -1.0);
  LocalAnalysis_SurfaceContinuity aCheck(aSurface1, 0.5, 0.5, aSurface2, 0.5, 0.5, GeomAbs_G2);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_FALSE(aCheck.IsG2());
  EXPECT_NEAR(aCheck.G2CurvatureGap(), 2.0, 1.e-12);
}

TEST(LocalAnalysis_SurfaceContinuityTest, PlanesAndNegativeUmbilics)
{
  for (double aCurvature : {0.0, -1.0, 1.0})
  {
    occ::handle<Geom_BezierSurface> aSurface = paraboloid(aCurvature, aCurvature);
    LocalAnalysis_SurfaceContinuity aCheck(aSurface, 0.5, 0.5, aSurface, 0.5, 0.5, GeomAbs_G2);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_TRUE(aCheck.IsG2());
  }
}

TEST(LocalAnalysis_SurfaceContinuityTest, ReuseAfterFailureAndUnsupportedOrder)
{
  occ::handle<Geom_Surface>       aSphere = new Geom_SphericalSurface(gp_Ax3(), 1.0);
  occ::handle<Geom_Surface>       aPlane  = new Geom_Plane(gp_Ax3());
  GeomLProp_SLProps               aSingular(aSphere, 0.0, M_PI / 2.0, 2, Precision::Confusion());
  GeomLProp_SLProps               aRegular(aPlane, 0.0, 0.0, 2, Precision::Confusion());
  LocalAnalysis_SurfaceContinuity aCheck;
  EXPECT_FALSE(aCheck.IsDone());
  aCheck.ComputeAnalysis(aSingular, aRegular, GeomAbs_G2);
  EXPECT_FALSE(aCheck.IsDone());
  aCheck.ComputeAnalysis(aRegular, aRegular, GeomAbs_G2);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_EQ(aCheck.StatusError(), LocalAnalysis_NoError);
  EXPECT_TRUE(aCheck.IsG2());
  aCheck.ComputeAnalysis(aRegular, aRegular, GeomAbs_CN);
  EXPECT_FALSE(aCheck.IsDone());
  EXPECT_EQ(aCheck.StatusError(), LocalAnalysis_InvalidInput);
}

TEST(LocalAnalysis_SurfaceContinuityTest, RelativeCurvatureTolerance)
{
  occ::handle<Geom_BezierSurface> aSurface1 = paraboloid(1.0, 1.0);
  for (double aDifference : {0.005, 0.1})
  {
    occ::handle<Geom_BezierSurface> aSurface2 = paraboloid(1.0 + aDifference, 1.0 + aDifference);
    LocalAnalysis_SurfaceContinuity aCheck(aSurface1, 0.5, 0.5, aSurface2, 0.5, 0.5, GeomAbs_G2);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_EQ(aCheck.IsG2(), aDifference == 0.005);
    EXPECT_NEAR(aCheck.G2CurvatureGap(), aDifference, 1.e-12);
  }
}

TEST(LocalAnalysis_SurfaceContinuityTest, MissingSurface)
{
  LocalAnalysis_SurfaceContinuity
    aCheck(occ::handle<Geom_Surface>(), 0.0, 0.0, paraboloid(0.0, 0.0), 0.5, 0.5, GeomAbs_G1);
  EXPECT_FALSE(aCheck.IsDone());
  EXPECT_EQ(aCheck.StatusError(), LocalAnalysis_InvalidInput);
}

TEST(LocalAnalysis_SurfaceContinuityTest, TransportBetweenDifferentTangentPlanes)
{
  // A shortest rotation about an in-plane axis transports the whole curvature
  // operator without changing its eigenvalues or principal-frame alignment.
  occ::handle<Geom_BezierSurface> aSurface1 = paraboloid(2.0, -1.0);
  for (double anAngle : {0.0, 1.e-8, 0.05, 0.5, 1.5})
  {
    occ::handle<Geom_BezierSurface> aSurface2 = paraboloid(2.0, -1.0);
    gp_Trsf                         aRotation;
    aRotation.SetRotation(gp_Ax1(gp::Origin(), gp_Dir(1, 2, 0)), anAngle);
    aSurface2->Transform(aRotation);
    for (bool isReversed : {false, true})
    {
      if (isReversed)
      {
        aSurface2->UReverse();
      }
      LocalAnalysis_SurfaceContinuity aCheck(aSurface1, 0.5, 0.5, aSurface2, 0.5, 0.5, GeomAbs_G2);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.G2CurvatureGap(), 0.0, 1.e-12);
      LocalAnalysis_SurfaceContinuity
        aSwapped(aSurface2, 0.5, 0.5, aSurface1, 0.5, 0.5, GeomAbs_G2);
      ASSERT_TRUE(aSwapped.IsDone());
      EXPECT_NEAR(aSwapped.G2CurvatureGap(), aCheck.G2CurvatureGap(), 1.e-12);
    }
  }
}
