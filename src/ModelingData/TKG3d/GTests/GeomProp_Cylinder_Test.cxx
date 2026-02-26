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

// Unit tests for GeomProp_Surface with CylindricalSurface geometry.

#include <Geom_CylindricalSurface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <cmath>

#include <gtest/gtest.h>

  namespace
{
  constexpr double THE_LIN_TOL  = Precision::PConfusion();
  constexpr double THE_CURV_TOL = 1.0e-6;
  constexpr double THE_DIR_TOL  = 1.0e-4;

  void compareNormal(const occ::handle<Geom_Surface>& theSurf, const double theU, const double theV)
  {
    GeomProp_Surface                    aProp(theSurf);
    const GeomProp::SurfaceNormalResult aNew = aProp.Normal(theU, theV, THE_LIN_TOL);

    GeomLProp_SLProps anOld(theSurf, theU, theV, 2, THE_LIN_TOL);
    if (anOld.IsNormalDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "New normal undefined at (" << theU << "," << theV << ")";
      const double aDot = aNew.Direction.Dot(anOld.Normal());
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal mismatch at (" << theU << "," << theV << ")";
    }
  }

  void compareCurvatures(const occ::handle<Geom_Surface>& theSurf,
                         const double                     theU,
                         const double                     theV)
  {
    GeomProp_Surface                       aProp(theSurf);
    const GeomProp::SurfaceCurvatureResult aNew = aProp.Curvatures(theU, theV, THE_LIN_TOL);

    GeomLProp_SLProps anOld(theSurf, theU, theV, 2, THE_LIN_TOL);
    if (anOld.IsCurvatureDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "New curvatures undefined at (" << theU << "," << theV << ")";
      EXPECT_NEAR(aNew.MinCurvature, anOld.MinCurvature(), THE_CURV_TOL)
        << "MinCurvature mismatch at (" << theU << "," << theV << ")";
      EXPECT_NEAR(aNew.MaxCurvature, anOld.MaxCurvature(), THE_CURV_TOL)
        << "MaxCurvature mismatch at (" << theU << "," << theV << ")";
    }
  }

  void compareMeanGaussian(const occ::handle<Geom_Surface>& theSurf,
                           const double                     theU,
                           const double                     theV)
  {
    GeomProp_Surface                   aProp(theSurf);
    const GeomProp::MeanGaussianResult aNew = aProp.MeanGaussian(theU, theV, THE_LIN_TOL);

    GeomLProp_SLProps anOld(theSurf, theU, theV, 2, THE_LIN_TOL);
    if (anOld.IsCurvatureDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "New MeanGaussian undefined at (" << theU << "," << theV
                                  << ")";
      EXPECT_NEAR(aNew.MeanCurvature, anOld.MeanCurvature(), THE_CURV_TOL)
        << "Mean curvature mismatch at (" << theU << "," << theV << ")";
      EXPECT_NEAR(aNew.GaussianCurvature, anOld.GaussianCurvature(), THE_CURV_TOL)
        << "Gaussian curvature mismatch at (" << theU << "," << theV << ")";
    }
  }

  void compareAllSurface(const occ::handle<Geom_Surface>& theSurf,
                         const double                     theUMin,
                         const double                     theUMax,
                         const double                     theVMin,
                         const double                     theVMax,
                         const int                        theNbU = 5,
                         const int                        theNbV = 5)
  {
    const double aUStep = (theUMax - theUMin) / theNbU;
    const double aVStep = (theVMax - theVMin) / theNbV;
    for (int i = 0; i <= theNbU; ++i)
    {
      for (int j = 0; j <= theNbV; ++j)
      {
        const double aU = theUMin + i * aUStep;
        const double aV = theVMin + j * aVStep;
        compareNormal(theSurf, aU, aV);
        compareCurvatures(theSurf, aU, aV);
        compareMeanGaussian(theSurf, aU, aV);
      }
    }
  }
} // namespace

// ============================================================================
// Analytical reference tests
// ============================================================================

TEST(GeomProp_CylinderTest, Normal_PointsOutward)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  // At u=0, v=0, the surface point is (R,0,0), normal should point radially outward.
  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(std::abs(aNorm.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_CylinderTest, Normal_AllParams)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  for (int i = 0; i < 12; ++i)
  {
    const double aU = i * M_PI / 6.0;
    for (int j = -3; j <= 3; ++j)
    {
      const double                        aV    = j * 2.0;
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
      // Normal should be perpendicular to the axis (Z component = 0).
      EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL)
        << "Normal not perpendicular to axis at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_CylinderTest, Curvatures_OneZero)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // MinCurvature is -1/R (concave direction), MaxCurvature is 0 (along axis).
  EXPECT_NEAR(aCurv.MinCurvature, -1.0 / aRadius, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, Curvatures_OtherIsInvR)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // The non-zero curvature is MinCurvature = -1/R (signed, concave).
  EXPECT_NEAR(aCurv.MinCurvature, -1.0 / aRadius, THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, Curvatures_Constant)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aRef = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      const double                           aU    = i * M_PI / 4.0;
      const double                           aV    = j * 3.0;
      const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aCurv.IsDefined);
      EXPECT_NEAR(aCurv.MinCurvature, aRef.MinCurvature, THE_CURV_TOL)
        << "MinCurvature not constant at (" << aU << "," << aV << ")";
      EXPECT_NEAR(aCurv.MaxCurvature, aRef.MaxCurvature, THE_CURV_TOL)
        << "MaxCurvature not constant at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_CylinderTest, MeanGaussian_GaussianZero)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, MeanGaussian_MeanHalfInvR)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.MeanCurvature), 1.0 / (2.0 * aRadius), THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, GetType_IsCylinder)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  GeomProp_Surface                     aProp(aCyl);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Cylinder);
}

TEST(GeomProp_CylinderTest, Curvatures_SmallRadius)
{
  const double                         aRadius = 0.1;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // MinCurvature = -1/R (concave), MaxCurvature = 0 (along axis).
  EXPECT_NEAR(aCurv.MinCurvature, -1.0 / aRadius, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, Curvatures_LargeRadius)
{
  const double                         aRadius = 100.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // MinCurvature = -1/R (concave), MaxCurvature = 0 (along axis).
  EXPECT_NEAR(aCurv.MinCurvature, -1.0 / aRadius, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, VsSLProps_Normal)
{
  occ::handle<Geom_CylindricalSurface> aCyl   = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  const double                         aUStep = M_PI / 3.0;
  const double                         aVStep = 2.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareNormal(aCyl, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_CylinderTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_CylindricalSurface> aCyl   = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  const double                         aUStep = M_PI / 3.0;
  const double                         aVStep = 2.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareCurvatures(aCyl, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_CylinderTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_CylindricalSurface> aCyl   = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  const double                         aUStep = M_PI / 3.0;
  const double                         aVStep = 2.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareMeanGaussian(aCyl, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_CylinderTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  compareAllSurface(aCyl, 0.0, 2.0 * M_PI, -10.0, 10.0, 8, 8);
}

TEST(GeomProp_CylinderTest, VsSLProps_SmallRadius)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 0.1);
  compareAllSurface(aCyl, 0.0, 2.0 * M_PI, -5.0, 5.0);
}

TEST(GeomProp_CylinderTest, VsSLProps_LargeRadius)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 100.0);
  compareAllSurface(aCyl, 0.0, 2.0 * M_PI, -5.0, 5.0);
}

TEST(GeomProp_CylinderTest, Normal_ConsistentOrientation)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  // Normal at any (u, v) should be radial and independent of v.
  for (int i = 0; i < 8; ++i)
  {
    const double                        aU     = i * M_PI / 4.0;
    const GeomProp::SurfaceNormalResult aNorm1 = aProp.Normal(aU, 0.0, THE_LIN_TOL);
    const GeomProp::SurfaceNormalResult aNorm2 = aProp.Normal(aU, 10.0, THE_LIN_TOL);
    ASSERT_TRUE(aNorm1.IsDefined);
    ASSERT_TRUE(aNorm2.IsDefined);
    const double aDot = aNorm1.Direction.Dot(aNorm2.Direction);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Normal varies along v at u=" << aU;
  }
}

TEST(GeomProp_CylinderTest, MeanGaussian_Constant)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::MeanGaussianResult aRef = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (int i = 0; i < 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      const double                       aU  = i * M_PI / 3.0;
      const double                       aV  = j * 3.0;
      const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aMG.IsDefined);
      EXPECT_NEAR(aMG.MeanCurvature, aRef.MeanCurvature, THE_CURV_TOL);
      EXPECT_NEAR(aMG.GaussianCurvature, aRef.GaussianCurvature, THE_CURV_TOL);
    }
  }
}

TEST(GeomProp_CylinderTest, Curvatures_IsNotUmbilic)
{
  const double                         aRadius = 5.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                     aProp(aCyl);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // Min and max curvatures differ (0 vs 1/R).
  EXPECT_GT(std::abs(aCurv.MaxCurvature - aCurv.MinCurvature), THE_CURV_TOL);
}

TEST(GeomProp_CylinderTest, VsSLProps_LargeParams)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  compareAllSurface(aCyl, 0.0, 2.0 * M_PI, 990.0, 1010.0);
}

TEST(GeomProp_CylinderTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  // Critical: seam (u=0, near 2PI), axis extremes, various v
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0e-10, 0.0},
                               {2.0 * M_PI - 1.0e-10, 0.0},
                               {M_PI / 2.0, 0.0},
                               {M_PI, 0.0},
                               {3.0 * M_PI / 2.0, 0.0},
                               {0.0, 1.0e-10},
                               {0.0, 1.0e6},
                               {0.0, -1.0e6},
                               {M_PI / 4.0, 100.0},
                               {M_PI / 4.0, -100.0},
                               {M_PI, 1.0e-6}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aCyl, aUV[0], aUV[1]);
    compareCurvatures(aCyl, aUV[0], aUV[1]);
    compareMeanGaussian(aCyl, aUV[0], aUV[1]);
  }
}
