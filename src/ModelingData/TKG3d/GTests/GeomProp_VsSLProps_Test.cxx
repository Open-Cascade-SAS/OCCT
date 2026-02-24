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

// Cross-validation tests comparing GeomProp_Surface against GeomLProp_SLProps
// for local surface differential properties (normal, curvatures).

#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array2.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax3.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_LIN_TOL   = Precision::PConfusion();
constexpr double THE_CURV_TOL  = 1.0e-6;
constexpr double THE_DIR_TOL   = 1.0e-4;

//! Compare surface normal from new GeomProp_Surface vs old GeomLProp_SLProps.
void compareNormal(const occ::handle<Geom_Surface>& theSurf,
                   const double theU, const double theV)
{
  GeomProp_Surface aProp;
  aProp.Initialize(theSurf);
  const GeomProp::SurfaceNormalResult aNew = aProp.Normal(theU, theV, THE_LIN_TOL);

  GeomLProp_SLProps anOld(theSurf, theU, theV, 2, THE_LIN_TOL);
  if (anOld.IsNormalDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New normal undefined at (" << theU << "," << theV << ")";
    const gp_Dir anOldNorm = anOld.Normal();
    const double aDot = aNew.Direction.Dot(anOldNorm);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Normal mismatch at (" << theU << "," << theV << ")";
  }
}

//! Compare curvatures from new GeomProp_Surface vs old GeomLProp_SLProps.
void compareCurvatures(const occ::handle<Geom_Surface>& theSurf,
                       const double theU, const double theV)
{
  GeomProp_Surface aProp;
  aProp.Initialize(theSurf);
  const GeomProp::SurfaceCurvatureResult aNew = aProp.Curvatures(theU, theV, THE_LIN_TOL);

  GeomLProp_SLProps anOld(theSurf, theU, theV, 2, THE_LIN_TOL);
  if (anOld.IsCurvatureDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New curvatures undefined at (" << theU << "," << theV << ")";
    EXPECT_NEAR(aNew.MinCurvature, anOld.MinCurvature(), THE_CURV_TOL)
      << "MinCurvature mismatch at (" << theU << "," << theV << ")";
    EXPECT_NEAR(aNew.MaxCurvature, anOld.MaxCurvature(), THE_CURV_TOL)
      << "MaxCurvature mismatch at (" << theU << "," << theV << ")";
    // Note: IsUmbilic comparison intentionally omitted - the flag is tolerance-dependent
    // and may differ between implementations while curvature values agree.
  }
}

//! Compare mean and Gaussian curvatures.
void compareMeanGaussian(const occ::handle<Geom_Surface>& theSurf,
                         const double theU, const double theV)
{
  GeomProp_Surface aProp;
  aProp.Initialize(theSurf);
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
                       const double theUMin, const double theUMax,
                       const double theVMin, const double theVMax,
                       const int theNbU = 5, const int theNbV = 5)
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
// Plane
// ============================================================================

TEST(GeomProp_VsSLPropsTest, Plane)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  compareAllSurface(aPlane, -5.0, 5.0, -5.0, 5.0);
}

// ============================================================================
// Sphere
// ============================================================================

TEST(GeomProp_VsSLPropsTest, Sphere)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  // Avoid poles where D1U degenerates
  compareAllSurface(aSphere, 0.0, 2.0 * M_PI, -M_PI / 3.0, M_PI / 3.0);
}

// ============================================================================
// Cylinder
// ============================================================================

TEST(GeomProp_VsSLPropsTest, Cylinder)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp_Ax3(), 3.0);
  compareAllSurface(aCyl, 0.0, 2.0 * M_PI, -5.0, 5.0);
}

// ============================================================================
// Cone
// ============================================================================

TEST(GeomProp_VsSLPropsTest, Cone)
{
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(gp_Ax3(), M_PI / 6.0, 5.0);
  // Stay away from the apex (at V = -R/sin(alpha) = -10)
  compareAllSurface(aCone, 0.0, 2.0 * M_PI, 0.0, 10.0);
}

// ============================================================================
// Torus
// ============================================================================

TEST(GeomProp_VsSLPropsTest, Torus)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 3.0);
  compareAllSurface(aTorus, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI);
}

// ============================================================================
// BSpline Surface
// ============================================================================

TEST(GeomProp_VsSLPropsTest, BSplineSurface)
{
  // Simple 4x4 bicubic patch
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, 2), aVMults(1, 2);

  aUKnots(1) = 0.0; aUKnots(2) = 1.0;
  aVKnots(1) = 0.0; aVKnots(2) = 1.0;
  aUMults(1) = 4; aUMults(2) = 4;
  aVMults(1) = 4; aVMults(2) = 4;

  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double aX = i - 1;
      const double aY = j - 1;
      const double aZ = std::sin((i - 1) * 0.5) * std::cos((j - 1) * 0.5);
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  occ::handle<Geom_BSplineSurface> aSurf = new Geom_BSplineSurface(
    aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);

  compareAllSurface(aSurf, 0.0, 1.0, 0.0, 1.0, 4, 4);
}
