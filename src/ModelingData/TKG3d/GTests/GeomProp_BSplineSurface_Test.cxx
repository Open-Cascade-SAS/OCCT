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

// Tests for GeomProp_Surface with Geom_BSplineSurface.
// Validates normal, curvatures, mean/Gaussian curvature for flat and curved
// bicubic BSpline patches. Cross-validates against GeomLProp_SLProps.

#include <Geom_BSplineSurface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
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

  //! Create a flat (planar) 4x4 bicubic BSpline patch.
  occ::handle<Geom_BSplineSurface> makeFlatBSpline()
  {
    NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
    for (int i = 1; i <= 4; ++i)
    {
      for (int j = 1; j <= 4; ++j)
      {
        aPoles.SetValue(i, j, gp_Pnt((i - 1) * 1.0, (j - 1) * 1.0, 0.0));
      }
    }

    NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 2);
    NCollection_Array1<int>    aUMults(1, 2), aVMults(1, 2);
    aUKnots(1) = 0.0;
    aUKnots(2) = 1.0;
    aVKnots(1) = 0.0;
    aVKnots(2) = 1.0;
    aUMults(1) = 4;
    aUMults(2) = 4;
    aVMults(1) = 4;
    aVMults(2) = 4;

    return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
  }

  //! Create a curved 4x4 bicubic BSpline patch with wavy Z variation.
  occ::handle<Geom_BSplineSurface> makeCurvedBSpline()
  {
    NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
    for (int i = 1; i <= 4; ++i)
    {
      for (int j = 1; j <= 4; ++j)
      {
        const double aX = (i - 1) * 1.0;
        const double aY = (j - 1) * 1.0;
        const double aZ = std::sin(aX * 0.5) * std::cos(aY * 0.5);
        aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
      }
    }

    NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 2);
    NCollection_Array1<int>    aUMults(1, 2), aVMults(1, 2);
    aUKnots(1) = 0.0;
    aUKnots(2) = 1.0;
    aVKnots(1) = 0.0;
    aVKnots(2) = 1.0;
    aUMults(1) = 4;
    aUMults(2) = 4;
    aVMults(1) = 4;
    aVMults(2) = 4;

    return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
  }
} // namespace

// Normal at center of curved BSpline patch
TEST(GeomProp_BSplineSurfaceTest, Normal_AtCenter)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::SurfaceNormalResult aResult = aProp.Normal(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_GT(std::abs(aResult.Direction.Z()), 0.1);
}

// Normal at all four corners
TEST(GeomProp_BSplineSurfaceTest, Normal_AtCorners)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const double aCorners[][2] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};
  for (const auto& aUV : aCorners)
  {
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aUV[0], aUV[1], THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at (" << aUV[0] << "," << aUV[1] << ")";
  }
}

// Normal at 5x5 grid
TEST(GeomProp_BSplineSurfaceTest, Normal_Grid)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  for (int i = 0; i <= 4; ++i)
  {
    for (int j = 0; j <= 4; ++j)
    {
      const double                        aU      = i / 4.0;
      const double                        aV      = j / 4.0;
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
    }
  }
}

// Curvatures at center
TEST(GeomProp_BSplineSurfaceTest, Curvatures_AtCenter)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
}

// Curvatures at corners
TEST(GeomProp_BSplineSurfaceTest, Curvatures_AtCorners)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const double aCorners[][2] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};
  for (const auto& aUV : aCorners)
  {
    const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(aUV[0], aUV[1], THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Curvatures undefined at (" << aUV[0] << "," << aUV[1] << ")";
    EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
  }
}

// Mean and Gaussian curvature at center
TEST(GeomProp_BSplineSurfaceTest, MeanGaussian_AtCenter)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// GetType returns GeomAbs_BSplineSurface
TEST(GeomProp_BSplineSurfaceTest, GetType_IsBSplineSurface)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);
  EXPECT_EQ(aProp.GetType(), GeomAbs_BSplineSurface);
}

// Flat BSpline: curvatures are all zero
TEST(GeomProp_BSplineSurfaceTest, Curvatures_FlatPatch)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeFlatBSpline();
  GeomProp_Surface                 aProp(aSurf);

  for (double aU = 0.0; aU <= 1.0; aU += 0.25)
  {
    for (double aV = 0.0; aV <= 1.0; aV += 0.25)
    {
      const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined) << "Curvatures undefined at (" << aU << "," << aV << ")";
      EXPECT_NEAR(aResult.MinCurvature, 0.0, THE_CURV_TOL);
      EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
    }
  }
}

// Flat BSpline: H=K=0
TEST(GeomProp_BSplineSurfaceTest, MeanGaussian_FlatPatch)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeFlatBSpline();
  GeomProp_Surface                 aProp(aSurf);

  for (double aU = 0.0; aU <= 1.0; aU += 0.25)
  {
    for (double aV = 0.0; aV <= 1.0; aV += 0.25)
    {
      const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined);
      EXPECT_NEAR(aResult.MeanCurvature, 0.0, THE_CURV_TOL);
      EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
    }
  }
}

// Cross-validate normal against SLProps
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_Normal)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  for (int i = 0; i <= 4; ++i)
  {
    for (int j = 0; j <= 4; ++j)
    {
      compareNormal(aSurf, i / 4.0, j / 4.0);
    }
  }
}

// Cross-validate curvatures against SLProps
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_Curvatures)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  for (int i = 0; i <= 4; ++i)
  {
    for (int j = 0; j <= 4; ++j)
    {
      compareCurvatures(aSurf, i / 4.0, j / 4.0);
    }
  }
}

// Cross-validate mean/Gaussian against SLProps
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_MeanGaussian)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  for (int i = 0; i <= 4; ++i)
  {
    for (int j = 0; j <= 4; ++j)
    {
      compareMeanGaussian(aSurf, i / 4.0, j / 4.0);
    }
  }
}

// Dense cross-validation of all properties
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_AllProperties)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  compareAllSurface(aSurf, 0.0, 1.0, 0.0, 1.0, 6, 6);
}

// Cross-validate flat patch
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_FlatPatch)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeFlatBSpline();
  compareAllSurface(aSurf, 0.0, 1.0, 0.0, 1.0);
}

// Curved patch has non-zero curvatures
TEST(GeomProp_BSplineSurfaceTest, Curvatures_CurvedPatch)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aMaxAbsK = std::max(std::abs(aResult.MinCurvature), std::abs(aResult.MaxCurvature));
  EXPECT_GT(aMaxAbsK, THE_CURV_TOL);
}

// Curved patch: non-trivial mean and Gaussian curvatures
TEST(GeomProp_BSplineSurfaceTest, MeanGaussian_CurvedPatch)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  const double aAbsH = std::abs(aResult.MeanCurvature);
  const double aAbsK = std::abs(aResult.GaussianCurvature);
  EXPECT_GT(aAbsH + aAbsK, THE_CURV_TOL);
}

// Normal varies smoothly across the patch
TEST(GeomProp_BSplineSurfaceTest, Normal_SmoothVariation)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  constexpr int    aNbSteps = 20;
  constexpr double aStep    = 1.0 / aNbSteps;
  gp_Dir           aPrevNormal;
  bool             aHasPrev = false;

  for (int i = 0; i <= aNbSteps; ++i)
  {
    const double                        aU      = i * aStep;
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 0.5, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;

    if (aHasPrev)
    {
      const double aDot = aResult.Direction.Dot(aPrevNormal);
      EXPECT_GT(aDot, 0.9) << "Normal changed abruptly at u=" << aU;
    }
    aPrevNormal = aResult.Direction;
    aHasPrev    = true;
  }
}

// Dense 8x8 grid cross-validation
TEST(GeomProp_BSplineSurfaceTest, VsSLProps_DenseGrid)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  compareAllSurface(aSurf, 0.0, 1.0, 0.0, 1.0, 8, 8);
}

// Evaluate curvatures at knot values
TEST(GeomProp_BSplineSurfaceTest, Curvatures_AtKnots)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  // Knots are at 0.0 and 1.0 for this simple patch
  const double aKnots[] = {0.0, 1.0};
  for (const double aUKnot : aKnots)
  {
    for (const double aVKnot : aKnots)
    {
      const GeomProp::SurfaceCurvatureResult aResult =
        aProp.Curvatures(aUKnot, aVKnot, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined)
        << "Curvatures undefined at knot (" << aUKnot << "," << aVKnot << ")";
      EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
    }
  }
}

// Normal at knot values
TEST(GeomProp_BSplineSurfaceTest, Normal_AtKnots)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  GeomProp_Surface                 aProp(aSurf);

  const double aKnots[] = {0.0, 1.0};
  for (const double aUKnot : aKnots)
  {
    for (const double aVKnot : aKnots)
    {
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aUKnot, aVKnot, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined)
        << "Normal undefined at knot (" << aUKnot << "," << aVKnot << ")";
    }
  }
}

TEST(GeomProp_BSplineSurfaceTest, VsSLProps_CriticalPoints)
{
  occ::handle<Geom_BSplineSurface> aSurf = makeCurvedBSpline();
  // Critical: corners, edges, center, near-corners
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0, 1.0},
                               {0.0, 1.0},
                               {1.0, 0.0},
                               {0.5, 0.5},
                               {0.5, 0.0},
                               {0.0, 0.5},
                               {1.0, 0.5},
                               {0.5, 1.0},
                               {1.0e-10, 1.0e-10},
                               {1.0 - 1.0e-10, 1.0 - 1.0e-10},
                               {0.25, 0.25},
                               {0.75, 0.75},
                               {0.25, 0.75}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSurf, aUV[0], aUV[1]);
    compareCurvatures(aSurf, aUV[0], aUV[1]);
    compareMeanGaussian(aSurf, aUV[0], aUV[1]);
  }
}
