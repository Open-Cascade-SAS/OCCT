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

// Tests for GeomProp_Surface with Geom_OffsetSurface.
// Validates normal, curvatures, mean/Gaussian curvature for offset surfaces
// created from spheres, planes, and cylinders with various offset distances.
// Cross-validates against GeomLProp_SLProps.

#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_SLProps.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
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

  //! Compare surface normal from new GeomProp_Surface vs old GeomLProp_SLProps.
  void compareNormal(const occ::handle<Geom_Surface>& theSurf,
                     const double                     theU,
                     const double                     theV)
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

  //! Compare curvatures from new GeomProp_Surface vs old GeomLProp_SLProps.
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

  //! Compare mean and Gaussian curvatures.
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

  //! Run all surface comparisons at a grid of parameter values.
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

  //! Create offset sphere: sphere R=10, offset d=2.
  occ::handle<Geom_OffsetSurface> makeOffsetSphere(const double theOffset = 2.0)
  {
    occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 10.0);
    return new Geom_OffsetSurface(aSphere, theOffset);
  }

  //! Create offset plane: XY plane with offset d=5.
  occ::handle<Geom_OffsetSurface> makeOffsetPlane()
  {
    occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
    return new Geom_OffsetSurface(aPlane, 5.0);
  }

  //! Create offset cylinder: radius 4, offset d=1.
  occ::handle<Geom_OffsetSurface> makeOffsetCylinder()
  {
    occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 4.0);
    return new Geom_OffsetSurface(aCyl, 1.0);
  }
} // namespace

// Normal of offset sphere: same direction as base sphere
TEST(GeomProp_OffsetSurfaceTest, Normal_OffsetSphere)
{
  occ::handle<Geom_OffsetSurface>      aOffSurf = makeOffsetSphere();
  occ::handle<Geom_SphericalSurface> aBaseSurf = new Geom_SphericalSurface(gp_Ax3(), 10.0);
  GeomProp_Surface                     aOffProp(aOffSurf);
  GeomProp_Surface                     aBaseProp(aBaseSurf);

  // Avoid poles
  for (double aU = 0.5; aU < 2.0 * M_PI; aU += M_PI / 3.0)
  {
    for (double aV = -M_PI / 3.0; aV <= M_PI / 3.0; aV += M_PI / 3.0)
    {
      const GeomProp::SurfaceNormalResult aOffNorm  = aOffProp.Normal(aU, aV, THE_LIN_TOL);
      const GeomProp::SurfaceNormalResult aBaseNorm = aBaseProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aOffNorm.IsDefined);
      ASSERT_TRUE(aBaseNorm.IsDefined);
      const double aDot = aOffNorm.Direction.Dot(aBaseNorm.Direction);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal differs from base at (" << aU << "," << aV << ")";
    }
  }
}

// Curvatures of offset sphere: k = 1/(R+d) for both principal curvatures
TEST(GeomProp_OffsetSurfaceTest, Curvatures_OffsetSphere)
{
  const double                     aR      = 10.0;
  const double                     aD      = 2.0;
  occ::handle<Geom_OffsetSurface> aSurf   = makeOffsetSphere(aD);
  GeomProp_Surface                 aProp(aSurf);

  // Avoid poles
  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aExpectedK = 1.0 / (aR + aD);
  EXPECT_NEAR(std::abs(aResult.MinCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(std::abs(aResult.MaxCurvature), aExpectedK, THE_CURV_TOL);
}

// Mean and Gaussian curvature of offset sphere
TEST(GeomProp_OffsetSurfaceTest, MeanGaussian_OffsetSphere)
{
  const double                     aR    = 10.0;
  const double                     aD    = 2.0;
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere(aD);
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aExpectedK  = 1.0 / (aR + aD);
  const double aExpectedGK = aExpectedK * aExpectedK;
  EXPECT_NEAR(std::abs(aResult.MeanCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.GaussianCurvature, aExpectedGK, THE_CURV_TOL);
}

// Normal of offset plane: same as base plane
TEST(GeomProp_OffsetSurfaceTest, Normal_OffsetPlane)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetPlane();
  GeomProp_Surface                aProp(aSurf);

  const gp_Dir aExpected(0.0, 0.0, 1.0);
  for (double aU = -5.0; aU <= 5.0; aU += 2.5)
  {
    for (double aV = -5.0; aV <= 5.0; aV += 2.5)
    {
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined);
      const double aDot = aResult.Direction.Dot(aExpected);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
    }
  }
}

// Curvatures of offset plane: still zero
TEST(GeomProp_OffsetSurfaceTest, Curvatures_OffsetPlane)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetPlane();
  GeomProp_Surface                aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Mean and Gaussian curvature of offset plane: H=K=0
TEST(GeomProp_OffsetSurfaceTest, MeanGaussian_OffsetPlane)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetPlane();
  GeomProp_Surface                aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// GetType returns GeomAbs_OffsetSurface
TEST(GeomProp_OffsetSurfaceTest, GetType_IsOffsetSurface)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere();
  GeomProp_Surface                aProp(aSurf);
  EXPECT_EQ(aProp.GetType(), GeomAbs_OffsetSurface);
}

// Cross-validate offset sphere vs SLProps
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_OffsetSphere)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -M_PI / 3.0, M_PI / 3.0);
}

// Cross-validate offset plane vs SLProps
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_OffsetPlane)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetPlane();
  compareAllSurface(aSurf, -5.0, 5.0, -5.0, 5.0);
}

// Dense cross-validation for offset sphere
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_AllProperties_Sphere)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -M_PI / 3.0, M_PI / 3.0, 6, 6);
}

// Dense cross-validation for offset plane
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_AllProperties_Plane)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetPlane();
  compareAllSurface(aSurf, -5.0, 5.0, -5.0, 5.0, 6, 6);
}

// Normal of offset cylinder: same as base cylinder
TEST(GeomProp_OffsetSurfaceTest, Normal_OffsetCylinder)
{
  occ::handle<Geom_OffsetSurface>        aOffSurf = makeOffsetCylinder();
  occ::handle<Geom_CylindricalSurface> aBaseSurf = new Geom_CylindricalSurface(gp_Ax3(), 4.0);
  GeomProp_Surface                       aOffProp(aOffSurf);
  GeomProp_Surface                       aBaseProp(aBaseSurf);

  for (double aU = 0.5; aU < 2.0 * M_PI; aU += M_PI / 3.0)
  {
    const GeomProp::SurfaceNormalResult aOffNorm  = aOffProp.Normal(aU, 0.0, THE_LIN_TOL);
    const GeomProp::SurfaceNormalResult aBaseNorm = aBaseProp.Normal(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aOffNorm.IsDefined);
    ASSERT_TRUE(aBaseNorm.IsDefined);
    const double aDot = aOffNorm.Direction.Dot(aBaseNorm.Direction);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
  }
}

// Curvatures of offset cylinder: modified radius R+d
TEST(GeomProp_OffsetSurfaceTest, Curvatures_OffsetCylinder)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetCylinder();
  GeomProp_Surface                aProp(aSurf);

  const double                         aExpectedK = 1.0 / (4.0 + 1.0); // 1/(R+d)
  const GeomProp::SurfaceCurvatureResult aResult  = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // MinCurvature = -1/(R+d) (concave, signed), MaxCurvature = 0 (along axis).
  EXPECT_NEAR(aResult.MinCurvature, -aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Mean and Gaussian curvature of offset cylinder
TEST(GeomProp_OffsetSurfaceTest, MeanGaussian_OffsetCylinder)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetCylinder();
  GeomProp_Surface                aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // Gaussian curvature of cylinder-like surface should be zero
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// Cross-validate offset cylinder vs SLProps
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_OffsetCylinder)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetCylinder();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -5.0, 5.0);
}

// Large offset on sphere (d=100)
TEST(GeomProp_OffsetSurfaceTest, Curvatures_OffsetSphere_LargeOffset)
{
  const double                     aD    = 100.0;
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere(aD);
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aExpectedK = 1.0 / (10.0 + aD);
  EXPECT_NEAR(std::abs(aResult.MinCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(std::abs(aResult.MaxCurvature), aExpectedK, THE_CURV_TOL);
}

// Small offset on sphere (d=0.01)
TEST(GeomProp_OffsetSurfaceTest, Curvatures_OffsetSphere_SmallOffset)
{
  const double                     aD    = 0.01;
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere(aD);
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aExpectedK = 1.0 / (10.0 + aD);
  EXPECT_NEAR(std::abs(aResult.MinCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(std::abs(aResult.MaxCurvature), aExpectedK, THE_CURV_TOL);
}

// Cross-validate large offset sphere
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_LargeOffset)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere(100.0);
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -M_PI / 3.0, M_PI / 3.0);
}

// Cross-validate small offset sphere
TEST(GeomProp_OffsetSurfaceTest, VsSLProps_SmallOffset)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere(0.01);
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -M_PI / 3.0, M_PI / 3.0);
}

// Consistency check: H = (k1+k2)/2 and K = k1*k2
TEST(GeomProp_OffsetSurfaceTest, MeanGaussian_Consistent)
{
  occ::handle<Geom_OffsetSurface> aSurf = makeOffsetSphere();
  GeomProp_Surface                aProp(aSurf);

  for (double aU = 0.5; aU < 2.0 * M_PI; aU += M_PI / 3.0)
  {
    for (double aV = -M_PI / 4.0; aV <= M_PI / 4.0; aV += M_PI / 4.0)
    {
      const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      const GeomProp::MeanGaussianResult     aMG   = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
      if (aCurv.IsDefined && aMG.IsDefined)
      {
        const double aExpectedH = (aCurv.MinCurvature + aCurv.MaxCurvature) / 2.0;
        const double aExpectedK = aCurv.MinCurvature * aCurv.MaxCurvature;
        EXPECT_NEAR(aMG.MeanCurvature, aExpectedH, THE_CURV_TOL)
          << "H != (k1+k2)/2 at (" << aU << "," << aV << ")";
        EXPECT_NEAR(aMG.GaussianCurvature, aExpectedK, THE_CURV_TOL)
          << "K != k1*k2 at (" << aU << "," << aV << ")";
      }
    }
  }
}

TEST(GeomProp_OffsetSurfaceTest, VsSLProps_CriticalPoints)
{
  // Offset of a sphere
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  occ::handle<Geom_OffsetSurface> aSurf = new Geom_OffsetSurface(aSphere, 2.0);
  // Critical: near poles, equator, seam
  const double aParams[][2] = {{0.0, 0.0}, {1.0e-10, 0.0}, {0.0, 1.0e-10},
                                {M_PI / 2.0, 0.0}, {M_PI, 0.0},
                                {0.0, M_PI / 4.0}, {0.0, -M_PI / 4.0},
                                {0.0, M_PI / 2.0 - 1.0e-6}, {0.0, -(M_PI / 2.0 - 1.0e-6)},
                                {2.0 * M_PI - 1.0e-10, 0.0}, {M_PI / 4.0, M_PI / 3.0}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSurf, aUV[0], aUV[1]);
    compareCurvatures(aSurf, aUV[0], aUV[1]);
    compareMeanGaussian(aSurf, aUV[0], aUV[1]);
  }
}
