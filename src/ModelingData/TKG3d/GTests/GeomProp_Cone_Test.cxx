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

// Unit tests for GeomProp_Surface with ConicalSurface geometry.

#include <Geom_ConicalSurface.hxx>
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
    ASSERT_TRUE(aNew.IsDefined) << "New MeanGaussian undefined at (" << theU << "," << theV << ")";
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

TEST(GeomProp_ConeTest, Normal_NotParallelToAxis)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // Normal should not be parallel to Z axis for a cone.
  EXPECT_LT(std::abs(aNorm.Direction.Z()), 1.0 - THE_DIR_TOL);
}

TEST(GeomProp_ConeTest, Normal_Grid)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      const double                        aU    = i * M_PI / 4.0;
      const double                        aV    = j * 2.0;
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_ConeTest, Curvatures_OneZero)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(1.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // MaxCurvature is zero (along the ruling), MinCurvature is negative (concave).
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
  EXPECT_LT(aCurv.MinCurvature, -THE_CURV_TOL);
}

TEST(GeomProp_ConeTest, Curvatures_VaryWithV)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::SurfaceCurvatureResult aCurv1 = aProp.Curvatures(0.0, 1.0, THE_LIN_TOL);
  const GeomProp::SurfaceCurvatureResult aCurv2 = aProp.Curvatures(0.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv1.IsDefined);
  ASSERT_TRUE(aCurv2.IsDefined);
  // The non-zero curvature is MinCurvature (negative, concave), varies with V.
  EXPECT_NE(aCurv1.MinCurvature, aCurv2.MinCurvature);
}

TEST(GeomProp_ConeTest, Curvatures_DecreasesWithV)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  // As v increases, the cross-section radius grows and curvature magnitude decreases.
  const GeomProp::SurfaceCurvatureResult aCurv1 = aProp.Curvatures(0.0, 1.0, THE_LIN_TOL);
  const GeomProp::SurfaceCurvatureResult aCurv2 = aProp.Curvatures(0.0, 5.0, THE_LIN_TOL);
  const GeomProp::SurfaceCurvatureResult aCurv3 = aProp.Curvatures(0.0, 10.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv1.IsDefined);
  ASSERT_TRUE(aCurv2.IsDefined);
  ASSERT_TRUE(aCurv3.IsDefined);
  // The non-zero curvature is MinCurvature (negative); magnitude decreases with V.
  EXPECT_GT(std::abs(aCurv1.MinCurvature), std::abs(aCurv2.MinCurvature));
  EXPECT_GT(std::abs(aCurv2.MinCurvature), std::abs(aCurv3.MinCurvature));
}

TEST(GeomProp_ConeTest, MeanGaussian_GaussianZero)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(1.0, 3.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_ConeTest, MeanGaussian_MeanVaries)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::MeanGaussianResult aMG1 = aProp.MeanGaussian(0.0, 1.0, THE_LIN_TOL);
  const GeomProp::MeanGaussianResult aMG2 = aProp.MeanGaussian(0.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG1.IsDefined);
  ASSERT_TRUE(aMG2.IsDefined);
  EXPECT_NE(aMG1.MeanCurvature, aMG2.MeanCurvature);
}

TEST(GeomProp_ConeTest, GetType_IsCone)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  GeomProp_Surface                 aProp(aCone);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Cone);
}

TEST(GeomProp_ConeTest, Curvatures_AwayFromApex)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  // At v=0 (reference circle). MaxCurvature is 0 (along ruling).
  const GeomProp::SurfaceCurvatureResult aCurvAtV0 = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvAtV0.IsDefined);
  EXPECT_NEAR(aCurvAtV0.MaxCurvature, 0.0, THE_CURV_TOL);

  // At v=10 (farther from apex). MaxCurvature is still 0 (along ruling).
  const GeomProp::SurfaceCurvatureResult aCurvAtV10 = aProp.Curvatures(0.0, 10.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvAtV10.IsDefined);
  EXPECT_NEAR(aCurvAtV10.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_ConeTest, Normal_Symmetric)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  // At u and u+PI, the normals should be related by reflection through the axis.
  const double                        aV     = 3.0;
  const GeomProp::SurfaceNormalResult aNorm1 = aProp.Normal(0.0, aV, THE_LIN_TOL);
  const GeomProp::SurfaceNormalResult aNorm2 = aProp.Normal(M_PI, aV, THE_LIN_TOL);
  ASSERT_TRUE(aNorm1.IsDefined);
  ASSERT_TRUE(aNorm2.IsDefined);
  // Z components should be the same.
  EXPECT_NEAR(aNorm1.Direction.Z(), aNorm2.Direction.Z(), THE_DIR_TOL);
}

// ============================================================================
// Cross-validation tests vs GeomLProp_SLProps
// ============================================================================

TEST(GeomProp_ConeTest, VsSLProps_Normal)
{
  occ::handle<Geom_ConicalSurface> aCone  = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  const double                     aUStep = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      compareNormal(aCone, i * aUStep, j * 2.0);
    }
  }
}

TEST(GeomProp_ConeTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_ConicalSurface> aCone  = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  const double                     aUStep = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      compareCurvatures(aCone, i * aUStep, j * 2.0);
    }
  }
}

TEST(GeomProp_ConeTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_ConicalSurface> aCone  = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  const double                     aUStep = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      compareMeanGaussian(aCone, i * aUStep, j * 2.0);
    }
  }
}

TEST(GeomProp_ConeTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 1.0, 10.0, 8, 8);
}

TEST(GeomProp_ConeTest, VsSLProps_SmallAngle)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 12.0, 5.0);
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 1.0, 10.0);
}

TEST(GeomProp_ConeTest, VsSLProps_LargeAngle)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 3.0, 5.0);
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 1.0, 10.0);
}

TEST(GeomProp_ConeTest, VsSLProps_LargeRadius)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 100.0);
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 1.0, 20.0);
}

TEST(GeomProp_ConeTest, VsSLProps_SmallRadius)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 0.5);
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 1.0, 10.0);
}

TEST(GeomProp_ConeTest, Curvatures_CompareV0_V5)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  const GeomProp::SurfaceCurvatureResult aCurvV0 = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  const GeomProp::SurfaceCurvatureResult aCurvV5 = aProp.Curvatures(0.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvV0.IsDefined);
  ASSERT_TRUE(aCurvV5.IsDefined);
  // At v=0 the cross-section radius is smaller, so |MinCurvature| is larger.
  // MinCurvature is the non-zero (negative) curvature; MaxCurvature is 0.
  EXPECT_GT(std::abs(aCurvV0.MinCurvature), std::abs(aCurvV5.MinCurvature));
}

TEST(GeomProp_ConeTest, MeanGaussian_Consistent)
{
  const double                     aHalfAngle = M_PI / 6.0;
  const double                     aRefRadius = 5.0;
  occ::handle<Geom_ConicalSurface> aCone =
    new Geom_ConicalSurface(gp_Ax3(), aHalfAngle, aRefRadius);
  GeomProp_Surface aProp(aCone);

  // Verify H = (k1+k2)/2 and K = k1*k2 = 0.
  for (int j = 1; j <= 5; ++j)
  {
    const double                           aV    = j * 2.0;
    const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, aV, THE_LIN_TOL);
    const GeomProp::MeanGaussianResult     aMG   = aProp.MeanGaussian(0.0, aV, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    ASSERT_TRUE(aMG.IsDefined);
    const double aExpectedMean  = (aCurv.MinCurvature + aCurv.MaxCurvature) / 2.0;
    const double aExpectedGauss = aCurv.MinCurvature * aCurv.MaxCurvature;
    EXPECT_NEAR(aMG.MeanCurvature, aExpectedMean, THE_CURV_TOL) << "H != (k1+k2)/2 at v=" << aV;
    EXPECT_NEAR(aMG.GaussianCurvature, aExpectedGauss, THE_CURV_TOL) << "K != k1*k2 at v=" << aV;
  }
}

TEST(GeomProp_ConeTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  // Critical: near apex (small v), seam, various angles
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0e-10, 0.0},
                               {0.0, 1.0e-10},
                               {0.0, 0.1},
                               {0.0, 1.0},
                               {0.0, 10.0},
                               {0.0, 100.0},
                               {M_PI / 2.0, 0.5},
                               {M_PI, 1.0},
                               {3.0 * M_PI / 2.0, 2.0},
                               {2.0 * M_PI - 1.0e-10, 0.5},
                               {M_PI / 4.0, 1.0e-6}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aCone, aUV[0], aUV[1]);
    compareCurvatures(aCone, aUV[0], aUV[1]);
    compareMeanGaussian(aCone, aUV[0], aUV[1]);
  }
}
