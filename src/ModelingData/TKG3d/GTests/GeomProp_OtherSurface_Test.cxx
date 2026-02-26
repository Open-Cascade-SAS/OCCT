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

// Tests for GeomProp_Surface with Geom_RectangularTrimmedSurface (OtherSurface).
// Validates normal, curvatures, mean/Gaussian curvature for trimmed surfaces
// created from spheres, cylinders, planes, and tori.
// Cross-validates against GeomLProp_SLProps.

#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
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

//! Compare surface normal from new GeomProp_Surface vs old GeomLProp_SLProps.
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
    ASSERT_TRUE(aNew.IsDefined) << "New MeanGaussian undefined at (" << theU << "," << theV << ")";
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

//! Trimmed sphere: R=8, U in [0.5, 2.5], V in [-0.8, 0.8].
occ::handle<Geom_RectangularTrimmedSurface> makeTrimmedSphere()
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 8.0);
  return new Geom_RectangularTrimmedSurface(aSphere, 0.5, 2.5, -0.8, 0.8);
}

//! Trimmed cylinder: R=5, U in [0.3, 3.0], V in [-4.0, 4.0].
occ::handle<Geom_RectangularTrimmedSurface> makeTrimmedCylinder()
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  return new Geom_RectangularTrimmedSurface(aCyl, 0.3, 3.0, -4.0, 4.0);
}

//! Trimmed plane: U in [-3.0, 3.0], V in [-3.0, 3.0].
occ::handle<Geom_RectangularTrimmedSurface> makeTrimmedPlane()
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  return new Geom_RectangularTrimmedSurface(aPlane, -3.0, 3.0, -3.0, 3.0);
}

//! Trimmed torus: major R=10, minor R=3, U in [0.5, 5.0], V in [0.5, 5.0].
occ::handle<Geom_RectangularTrimmedSurface> makeTrimmedTorus()
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  return new Geom_RectangularTrimmedSurface(aTorus, 0.5, 5.0, 0.5, 5.0);
}
} // namespace

// Normal of trimmed sphere in the trimmed range
TEST(GeomProp_OtherSurfaceTest, Normal_TrimmedSphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::SurfaceNormalResult aResult = aProp.Normal(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// Curvatures of trimmed sphere: k = 1/R
TEST(GeomProp_OtherSurfaceTest, Curvatures_TrimmedSphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  GeomProp_Surface                            aProp(aSurf);

  const double                           aExpectedK = 1.0 / 8.0;
  const GeomProp::SurfaceCurvatureResult aResult    = aProp.Curvatures(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(std::abs(aResult.MinCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(std::abs(aResult.MaxCurvature), aExpectedK, THE_CURV_TOL);
}

// Mean and Gaussian curvature of trimmed sphere
TEST(GeomProp_OtherSurfaceTest, MeanGaussian_TrimmedSphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aExpectedK  = 1.0 / 8.0;
  const double aExpectedGK = aExpectedK * aExpectedK;
  EXPECT_NEAR(std::abs(aResult.MeanCurvature), aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.GaussianCurvature, aExpectedGK, THE_CURV_TOL);
}

// Normal of trimmed cylinder
TEST(GeomProp_OtherSurfaceTest, Normal_TrimmedCylinder)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedCylinder();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::SurfaceNormalResult aResult = aProp.Normal(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // Normal should be perpendicular to Z axis
  EXPECT_NEAR(aResult.Direction.Z(), 0.0, THE_DIR_TOL);
}

// Curvatures of trimmed cylinder
TEST(GeomProp_OtherSurfaceTest, Curvatures_TrimmedCylinder)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedCylinder();
  GeomProp_Surface                            aProp(aSurf);

  const double                           aExpectedK = 1.0 / 5.0;
  const GeomProp::SurfaceCurvatureResult aResult    = aProp.Curvatures(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // MinCurvature = -1/R (concave, signed), MaxCurvature = 0 (along axis).
  EXPECT_NEAR(aResult.MinCurvature, -aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Mean and Gaussian curvature of trimmed cylinder
TEST(GeomProp_OtherSurfaceTest, MeanGaussian_TrimmedCylinder)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedCylinder();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// GetType returns the base surface type because GeomAdaptor unwraps
// RectangularTrimmedSurface to its underlying surface type.
TEST(GeomProp_OtherSurfaceTest, GetType_IsOtherSurface)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  GeomProp_Surface                            aProp(aSurf);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Sphere);
}

// Cross-validate trimmed sphere vs SLProps
TEST(GeomProp_OtherSurfaceTest, VsSLProps_TrimmedSphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  compareAllSurface(aSurf, 0.5, 2.5, -0.8, 0.8);
}

// Cross-validate trimmed cylinder vs SLProps
TEST(GeomProp_OtherSurfaceTest, VsSLProps_TrimmedCylinder)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedCylinder();
  compareAllSurface(aSurf, 0.3, 3.0, -4.0, 4.0);
}

// Dense cross-validation for trimmed sphere
TEST(GeomProp_OtherSurfaceTest, VsSLProps_AllProperties_Sphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  compareAllSurface(aSurf, 0.5, 2.5, -0.8, 0.8, 6, 6);
}

// Dense cross-validation for trimmed cylinder
TEST(GeomProp_OtherSurfaceTest, VsSLProps_AllProperties_Cylinder)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedCylinder();
  compareAllSurface(aSurf, 0.3, 3.0, -4.0, 4.0, 6, 6);
}

// Normal of trimmed plane: constant
TEST(GeomProp_OtherSurfaceTest, Normal_TrimmedPlane)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedPlane();
  GeomProp_Surface                            aProp(aSurf);

  const gp_Dir aExpected(0.0, 0.0, 1.0);
  for (double aU = -3.0; aU <= 3.0; aU += 1.5)
  {
    for (double aV = -3.0; aV <= 3.0; aV += 1.5)
    {
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined);
      const double aDot = aResult.Direction.Dot(aExpected);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
    }
  }
}

// Curvatures of trimmed plane: zeros
TEST(GeomProp_OtherSurfaceTest, Curvatures_TrimmedPlane)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedPlane();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Mean and Gaussian curvature of trimmed plane: zeros
TEST(GeomProp_OtherSurfaceTest, MeanGaussian_TrimmedPlane)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedPlane();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// Cross-validate trimmed plane vs SLProps
TEST(GeomProp_OtherSurfaceTest, VsSLProps_TrimmedPlane)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedPlane();
  compareAllSurface(aSurf, -3.0, 3.0, -3.0, 3.0);
}

// Normal of trimmed torus
TEST(GeomProp_OtherSurfaceTest, Normal_TrimmedTorus)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedTorus();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::SurfaceNormalResult aResult = aProp.Normal(2.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// Curvatures of trimmed torus
TEST(GeomProp_OtherSurfaceTest, Curvatures_TrimmedTorus)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedTorus();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(2.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
  // Both curvatures should be non-zero on a torus
  EXPECT_GT(std::abs(aResult.MinCurvature), THE_CURV_TOL);
  EXPECT_GT(std::abs(aResult.MaxCurvature), THE_CURV_TOL);
}

// Mean and Gaussian curvature of trimmed torus
TEST(GeomProp_OtherSurfaceTest, MeanGaussian_TrimmedTorus)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedTorus();
  GeomProp_Surface                            aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(2.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// Cross-validate trimmed torus vs SLProps
TEST(GeomProp_OtherSurfaceTest, VsSLProps_TrimmedTorus)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedTorus();
  compareAllSurface(aSurf, 0.5, 5.0, 0.5, 5.0);
}

// Dense 8x8 grid cross-validation for trimmed sphere
TEST(GeomProp_OtherSurfaceTest, VsSLProps_DenseGrid_Sphere)
{
  occ::handle<Geom_RectangularTrimmedSurface> aSurf = makeTrimmedSphere();
  compareAllSurface(aSurf, 0.5, 2.5, -0.8, 0.8, 8, 8);
}

TEST(GeomProp_OtherSurfaceTest, VsSLProps_CriticalPoints)
{
  // Trimmed sphere
  occ::handle<Geom_SphericalSurface>          aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  occ::handle<Geom_RectangularTrimmedSurface> aSurf =
    new Geom_RectangularTrimmedSurface(aSphere,
                                       M_PI / 4.0,
                                       3.0 * M_PI / 4.0,
                                       -M_PI / 4.0,
                                       M_PI / 4.0);
  double aUF, aUL, aVF, aVL;
  aSurf->Bounds(aUF, aUL, aVF, aVL);
  const double aUMid = (aUF + aUL) / 2.0;
  const double aVMid = (aVF + aVL) / 2.0;
  // Critical: corners, edge midpoints, center, near-corners
  const double aParams[][2] = {{aUF, aVF},
                               {aUL, aVF},
                               {aUF, aVL},
                               {aUL, aVL},
                               {aUMid, aVF},
                               {aUMid, aVL},
                               {aUF, aVMid},
                               {aUL, aVMid},
                               {aUMid, aVMid},
                               {aUF + 1.0e-10, aVF + 1.0e-10},
                               {aUL - 1.0e-10, aVL - 1.0e-10},
                               {aUMid, aVMid + 1.0e-6}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSurf, aUV[0], aUV[1]);
    compareCurvatures(aSurf, aUV[0], aUV[1]);
    compareMeanGaussian(aSurf, aUV[0], aUV[1]);
  }
}
