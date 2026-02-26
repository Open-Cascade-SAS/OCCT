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

// Unit tests for GeomProp_Surface with ToroidalSurface geometry.

#include <Geom_ToroidalSurface.hxx>
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

TEST(GeomProp_TorusTest, Normal_AtOuterEdge)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // At u=0, v=0 (outer edge), point is (R+r, 0, 0), normal points outward along +X.
  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(std::abs(aNorm.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_TorusTest, Normal_Grid)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  for (int i = 0; i < 12; ++i)
  {
    for (int j = 0; j < 12; ++j)
    {
      const double                        aU    = i * M_PI / 6.0;
      const double                        aV    = j * M_PI / 6.0;
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_TorusTest, Curvatures_AtOuterEdge)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // At outer edge (v=0): both principal curvatures are positive.
  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // Both curvatures should have the same sign (positive for outward normal convention).
  EXPECT_TRUE((aCurv.MinCurvature >= -THE_CURV_TOL && aCurv.MaxCurvature >= -THE_CURV_TOL)
              || (aCurv.MinCurvature <= THE_CURV_TOL && aCurv.MaxCurvature <= THE_CURV_TOL));
}

TEST(GeomProp_TorusTest, Curvatures_AtInnerEdge)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // At inner edge (v=PI): principal curvatures have opposite signs.
  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_LT(aCurv.MinCurvature * aCurv.MaxCurvature, THE_CURV_TOL)
    << "Expected opposite sign curvatures at inner edge";
}

TEST(GeomProp_TorusTest, Curvatures_VaryWithV)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  const GeomProp::SurfaceCurvatureResult aCurvOuter = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  const GeomProp::SurfaceCurvatureResult aCurvInner = aProp.Curvatures(0.0, M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aCurvOuter.IsDefined);
  ASSERT_TRUE(aCurvInner.IsDefined);
  // The circumferential curvature (MaxCurvature) differs between inner and outer edge.
  // The meridional curvature (MinCurvature = -1/r) is constant.
  EXPECT_NE(aCurvOuter.MaxCurvature, aCurvInner.MaxCurvature);
}

TEST(GeomProp_TorusTest, MeanGaussian_GaussianPositiveOuter)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_GT(aMG.GaussianCurvature, 0.0);
}

TEST(GeomProp_TorusTest, MeanGaussian_GaussianNegativeInner)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.0, M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_LT(aMG.GaussianCurvature, 0.0);
}

TEST(GeomProp_TorusTest, MeanGaussian_GaussianZeroTop)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // At v=PI/2, the Gaussian curvature should be approximately zero.
  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.0, M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_TorusTest, GetType_IsTorus)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  GeomProp_Surface                  aProp(aTorus);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Torus);
}

TEST(GeomProp_TorusTest, Curvatures_MinorCurvature)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // The meridional curvature is always -1/r (signed, constant everywhere).
  for (int j = 0; j < 8; ++j)
  {
    const double                           aV    = j * M_PI / 4.0;
    const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, aV, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    // MinCurvature is the meridional curvature = -1/r (constant, negative).
    EXPECT_NEAR(aCurv.MinCurvature, -1.0 / aMinorR, THE_CURV_TOL)
      << "Minor curvature wrong at v=" << aV;
  }
}

// ============================================================================
// Cross-validation tests vs GeomLProp_SLProps
// ============================================================================

TEST(GeomProp_TorusTest, VsSLProps_Normal)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  const double                      aStep  = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 0; j <= 6; ++j)
    {
      compareNormal(aTorus, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_TorusTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  const double                      aStep  = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 0; j <= 6; ++j)
    {
      compareCurvatures(aTorus, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_TorusTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  const double                      aStep  = M_PI / 3.0;
  for (int i = 0; i <= 6; ++i)
  {
    for (int j = 0; j <= 6; ++j)
    {
      compareMeanGaussian(aTorus, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_TorusTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  compareAllSurface(aTorus, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 8, 8);
}

TEST(GeomProp_TorusTest, VsSLProps_LargeRadii)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 100.0, 10.0);
  compareAllSurface(aTorus, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_TorusTest, VsSLProps_SmallRadii)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 2.0, 0.5);
  compareAllSurface(aTorus, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_TorusTest, MeanGaussian_Mean_AtOuterEdge)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // H = (1/r + 1/(R+r)) / 2
  const double                       aExpectedH = (1.0 / aMinorR + 1.0 / (aMajorR + aMinorR)) / 2.0;
  const GeomProp::MeanGaussianResult aMG        = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.MeanCurvature), aExpectedH, THE_CURV_TOL);
}

TEST(GeomProp_TorusTest, MeanGaussian_Gaussian_AtOuterEdge)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // K = 1 / (r * (R + r))
  const double                       aExpectedK = 1.0 / (aMinorR * (aMajorR + aMinorR));
  const GeomProp::MeanGaussianResult aMG        = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.GaussianCurvature), aExpectedK, THE_CURV_TOL);
}

TEST(GeomProp_TorusTest, Curvatures_ConstantAlongU)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // Curvatures depend only on v, not on u (axial symmetry).
  const double                           aV   = 0.5;
  const GeomProp::SurfaceCurvatureResult aRef = aProp.Curvatures(0.0, aV, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (int i = 1; i < 8; ++i)
  {
    const double                           aU    = i * M_PI / 4.0;
    const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.MinCurvature, aRef.MinCurvature, THE_CURV_TOL)
      << "MinCurvature varies with u at u=" << aU;
    EXPECT_NEAR(aCurv.MaxCurvature, aRef.MaxCurvature, THE_CURV_TOL)
      << "MaxCurvature varies with u at u=" << aU;
  }
}

TEST(GeomProp_TorusTest, Normal_ConstantMagnitude)
{
  const double                      aMajorR = 10.0;
  const double                      aMinorR = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus  = new Geom_ToroidalSurface(gp_Ax3(), aMajorR, aMinorR);
  GeomProp_Surface                  aProp(aTorus);

  // gp_Dir is always a unit vector, so the normal magnitude is always 1.
  for (int i = 0; i < 8; ++i)
  {
    for (int j = 0; j < 8; ++j)
    {
      const double                        aU    = i * M_PI / 4.0;
      const double                        aV    = j * M_PI / 4.0;
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined);
      const double aMag = std::sqrt(aNorm.Direction.X() * aNorm.Direction.X()
                                    + aNorm.Direction.Y() * aNorm.Direction.Y()
                                    + aNorm.Direction.Z() * aNorm.Direction.Z());
      EXPECT_NEAR(aMag, 1.0, THE_DIR_TOL) << "Normal not unit at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_TorusTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  // Critical: inner ring (v=PI), outer (v=0), top/bottom (v=+-PI/2), seam
  const double aParams[][2] = {{0.0, 0.0},
                               {0.0, M_PI},
                               {0.0, M_PI / 2.0},
                               {0.0, -M_PI / 2.0},
                               {M_PI / 2.0, 0.0},
                               {M_PI / 2.0, M_PI},
                               {1.0e-10, 0.0},
                               {0.0, 1.0e-10},
                               {0.0, M_PI + 1.0e-6},
                               {0.0, M_PI - 1.0e-6},
                               {2.0 * M_PI - 1.0e-10, M_PI / 4.0},
                               {M_PI / 4.0, 3.0 * M_PI / 4.0}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aTorus, aUV[0], aUV[1]);
    compareCurvatures(aTorus, aUV[0], aUV[1]);
    compareMeanGaussian(aTorus, aUV[0], aUV[1]);
  }
}
