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

// Unit tests for GeomProp_Surface with Plane geometry.

#include <Geom_Plane.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
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

TEST(GeomProp_PlaneTest, Normal_IsConstant)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aNorm.Direction.Z()), 1.0, THE_DIR_TOL);
}

TEST(GeomProp_PlaneTest, Normal_AllParams)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceNormalResult aRef = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (int i = -5; i <= 5; ++i)
  {
    for (int j = -5; j <= 5; ++j)
    {
      const double                        aU    = i * 2.0;
      const double                        aV    = j * 2.0;
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
      const double aDot = aNorm.Direction.Dot(aRef.Direction);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal differs at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_PlaneTest, Curvatures_AllZero)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_PlaneTest, Curvatures_IsUmbilic)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  // On a plane, min = max = 0, so the point is trivially umbilic.
  for (int i = -3; i <= 3; ++i)
  {
    for (int j = -3; j <= 3; ++j)
    {
      const double                           aU    = i * 1.5;
      const double                           aV    = j * 1.5;
      const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aCurv.IsDefined);
      EXPECT_NEAR(aCurv.MinCurvature, aCurv.MaxCurvature, THE_CURV_TOL)
        << "Not umbilic at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_PlaneTest, MeanGaussian_AllZero)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_PlaneTest, GetType_IsPlane)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Plane);
}

TEST(GeomProp_PlaneTest, Normal_TiltedPlane)
{
  const gp_Dir            aNormalDir(1.0, 1.0, 1.0);
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), aNormalDir);
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  const double aDot = aNorm.Direction.Dot(aNormalDir);
  EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
}

TEST(GeomProp_PlaneTest, Curvatures_TiltedPlane)
{
  const gp_Dir            aNormalDir(1.0, 1.0, 1.0);
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), aNormalDir);
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(3.0, 4.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_PlaneTest, MeanGaussian_TiltedPlane)
{
  const gp_Dir            aNormalDir(1.0, 1.0, 1.0);
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), aNormalDir);
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(3.0, 4.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_PlaneTest, Normal_OffCenter)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(10, 20, 30), gp_Dir(0, 0, 1));
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(5.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(std::abs(aNorm.Direction.Z()), 1.0, THE_DIR_TOL);
}

TEST(GeomProp_PlaneTest, VsSLProps_Normal)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  const double            aStep  = 2.0;
  for (int i = 0; i <= 5; ++i)
  {
    for (int j = 0; j <= 5; ++j)
    {
      compareNormal(aPlane, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_PlaneTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  const double            aStep  = 2.0;
  for (int i = 0; i <= 5; ++i)
  {
    for (int j = 0; j <= 5; ++j)
    {
      compareCurvatures(aPlane, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_PlaneTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  const double            aStep  = 2.0;
  for (int i = 0; i <= 5; ++i)
  {
    for (int j = 0; j <= 5; ++j)
    {
      compareMeanGaussian(aPlane, i * aStep, j * aStep);
    }
  }
}

TEST(GeomProp_PlaneTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  compareAllSurface(aPlane, -10.0, 10.0, -10.0, 10.0, 8, 8);
}

TEST(GeomProp_PlaneTest, VsSLProps_TiltedPlane)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  compareAllSurface(aPlane, -5.0, 5.0, -5.0, 5.0);
}

TEST(GeomProp_PlaneTest, VsSLProps_OffCenter)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(10, 20, 30), gp_Dir(0, 0, 1));
  compareAllSurface(aPlane, -5.0, 5.0, -5.0, 5.0);
}

TEST(GeomProp_PlaneTest, VsSLProps_LargeParams)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  compareAllSurface(aPlane, 900.0, 1100.0, 900.0, 1100.0);
}

TEST(GeomProp_PlaneTest, Normal_ConsistentOrientation)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceNormalResult aRef = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  // Check that the normal is the same at widely separated points.
  const double aParams[] = {-100.0, -10.0, 0.0, 10.0, 100.0};
  for (double aU : aParams)
  {
    for (double aV : aParams)
    {
      const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aNorm.IsDefined);
      const double aDot = aNorm.Direction.Dot(aRef.Direction);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal orientation inconsistent at (" << aU << "," << aV << ")";
    }
  }
}

TEST(GeomProp_PlaneTest, Curvatures_ExactlyZero)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(5.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_EQ(aCurv.MinCurvature, 0.0);
  EXPECT_EQ(aCurv.MaxCurvature, 0.0);
}

TEST(GeomProp_PlaneTest, MeanGaussian_ExactlyZero)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(5.0, 5.0, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_EQ(aMG.MeanCurvature, 0.0);
  EXPECT_EQ(aMG.GaussianCurvature, 0.0);
}

TEST(GeomProp_PlaneTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_Plane> aPlane       = new Geom_Plane(gp_Ax3());
  const double            aParams[][2] = {{0.0, 0.0},
                                          {1.0e-10, 1.0e-10},
                                          {-1.0e-10, -1.0e-10},
                                          {1.0e6, 1.0e6},
                                          {-1.0e6, -1.0e6},
                                          {1.0e-10, 1.0e6},
                                          {0.5, 0.5},
                                          {-100.0, 100.0}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aPlane, aUV[0], aUV[1]);
    compareCurvatures(aPlane, aUV[0], aUV[1]);
    compareMeanGaussian(aPlane, aUV[0], aUV[1]);
  }
}
