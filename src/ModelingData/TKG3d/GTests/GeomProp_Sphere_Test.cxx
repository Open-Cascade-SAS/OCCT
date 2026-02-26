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

// Unit tests for GeomProp_Surface with SphericalSurface geometry.

#include <Geom_SphericalSurface.hxx>
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

TEST(GeomProp_SphereTest, Normal_PointsOutward)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  // At u=0, v=0 (equator, X axis), normal should point along +X (outward).
  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(std::abs(aNorm.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_SphereTest, Normal_Grid)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  // Avoid poles where D1U degenerates.
  for (int i = 0; i < 12; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      const double                        aU    = i * M_PI / 6.0;
      const double                        aV    = j * M_PI / 9.0; // Range [-PI/3, PI/3]
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_SphereTest, Curvatures_IsUmbilic)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = -2; j <= 2; ++j)
    {
      const double                           aU    = i * M_PI / 4.0;
      const double                           aV    = j * M_PI / 6.0;
      const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aCurv.IsDefined);
      EXPECT_NEAR(aCurv.MinCurvature, aCurv.MaxCurvature, THE_CURV_TOL)
        << "Not umbilic at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_SphereTest, Curvatures_Constant)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = -2; j <= 2; ++j)
    {
      const double                           aU    = i * M_PI / 4.0;
      const double                           aV    = j * M_PI / 6.0;
      const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aCurv.IsDefined);
      EXPECT_NEAR(std::abs(aCurv.MinCurvature), 1.0 / aRadius, THE_CURV_TOL)
        << "Curvature magnitude wrong at (" << aU << "," << aV << ")";
      EXPECT_NEAR(std::abs(aCurv.MaxCurvature), 1.0 / aRadius, THE_CURV_TOL)
        << "Curvature magnitude wrong at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_SphereTest, Curvatures_SmallRadius)
{
  const double                       aRadius = 0.1;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.MaxCurvature), 1.0 / aRadius, THE_CURV_TOL);
}

TEST(GeomProp_SphereTest, Curvatures_LargeRadius)
{
  const double                       aRadius = 1000.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.MaxCurvature), 1.0 / aRadius, THE_CURV_TOL);
}

TEST(GeomProp_SphereTest, MeanGaussian_Gaussian)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.GaussianCurvature), 1.0 / (aRadius * aRadius), THE_CURV_TOL);
}

TEST(GeomProp_SphereTest, MeanGaussian_Mean)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.MeanCurvature), 1.0 / aRadius, THE_CURV_TOL);
}

TEST(GeomProp_SphereTest, MeanGaussian_Constant)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  const GeomProp::MeanGaussianResult aRef = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = -2; j <= 2; ++j)
    {
      const double                       aU  = i * M_PI / 4.0;
      const double                       aV  = j * M_PI / 6.0;
      const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aMG.IsDefined);
      EXPECT_NEAR(aMG.MeanCurvature, aRef.MeanCurvature, THE_CURV_TOL)
        << "Mean curvature not constant at (" << aU << "," << aV << ")";
      EXPECT_NEAR(aMG.GaussianCurvature, aRef.GaussianCurvature, THE_CURV_TOL)
        << "Gaussian curvature not constant at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_SphereTest, GetType_IsSphere)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  GeomProp_Surface                   aProp(aSphere);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Sphere);
}

// ============================================================================
// Cross-validation tests vs GeomLProp_SLProps
// ============================================================================

TEST(GeomProp_SphereTest, VsSLProps_Normal)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  // Avoid poles.
  const double aUStep = M_PI / 3.0;
  const double aVStep = M_PI / 9.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareNormal(aSphere, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_SphereTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  const double                       aUStep  = M_PI / 3.0;
  const double                       aVStep  = M_PI / 9.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareCurvatures(aSphere, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_SphereTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  const double                       aUStep  = M_PI / 3.0;
  const double                       aVStep  = M_PI / 9.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      compareMeanGaussian(aSphere, i * aUStep, j * aVStep);
    }
  }
}

TEST(GeomProp_SphereTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  compareAllSurface(aSphere, 0.0, 2.0 * M_PI, -M_PI / 3.0, M_PI / 3.0, 8, 8);
}

TEST(GeomProp_SphereTest, VsSLProps_SmallRadius)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 0.1);
  compareAllSurface(aSphere, 0.0, 2.0 * M_PI, -M_PI / 3.0, M_PI / 3.0);
}

TEST(GeomProp_SphereTest, VsSLProps_LargeRadius)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 1000.0);
  compareAllSurface(aSphere, 0.0, 2.0 * M_PI, -M_PI / 3.0, M_PI / 3.0);
}

TEST(GeomProp_SphereTest, VsSLProps_OffCenter)
{
  const gp_Ax3                       anAx3(gp_Pnt(5, 10, 15), gp_Dir(0, 0, 1));
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(anAx3, 5.0);
  compareAllSurface(aSphere, 0.0, 2.0 * M_PI, -M_PI / 3.0, M_PI / 3.0);
}

TEST(GeomProp_SphereTest, Normal_AtEquator)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  // At u=PI/2, v=0, the point is (0,R,0), normal should be along +Y.
  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(M_PI / 2.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aNorm.Direction.Y()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_SphereTest, Curvatures_SignConsistency)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  for (int i = 0; i < 8; ++i)
  {
    const double                           aU    = i * M_PI / 4.0;
    const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    // Both curvatures should have the same sign on a sphere.
    EXPECT_TRUE((aCurv.MinCurvature >= 0.0 && aCurv.MaxCurvature >= 0.0)
                || (aCurv.MinCurvature <= 0.0 && aCurv.MaxCurvature <= 0.0))
      << "Curvatures have different signs at u=" << aU;
  }
}

TEST(GeomProp_SphereTest, MeanGaussian_GaussianPositive)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);
  GeomProp_Surface                   aProp(aSphere);

  for (int i = 0; i < 8; ++i)
  {
    for (int j = -2; j <= 2; ++j)
    {
      const double                       aU  = i * M_PI / 4.0;
      const double                       aV  = j * M_PI / 6.0;
      const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aMG.IsDefined);
      EXPECT_GT(aMG.GaussianCurvature, 0.0)
        << "Gaussian curvature not positive at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_SphereTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  // Critical: near poles (v=+-PI/2), equator (v=0), seam (u=0), near-seam
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0e-10, 0.0},
                               {0.0, 1.0e-10},
                               {M_PI / 2.0, 0.0},
                               {M_PI, 0.0},
                               {3.0 * M_PI / 2.0, 0.0},
                               {0.0, M_PI / 2.0 - 1.0e-6},
                               {0.0, -(M_PI / 2.0 - 1.0e-6)},
                               {0.0, M_PI / 4.0},
                               {0.0, -M_PI / 4.0},
                               {2.0 * M_PI - 1.0e-10, 0.0},
                               {M_PI / 4.0, M_PI / 3.0},
                               {M_PI / 4.0, -M_PI / 3.0}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSphere, aUV[0], aUV[1]);
    compareCurvatures(aSphere, aUV[0], aUV[1]);
    compareMeanGaussian(aSphere, aUV[0], aUV[1]);
  }
}
