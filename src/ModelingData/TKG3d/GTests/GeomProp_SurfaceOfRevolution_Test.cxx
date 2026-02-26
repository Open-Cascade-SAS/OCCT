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

// Tests for GeomProp_Surface with Geom_SurfaceOfRevolution.
// Validates normal, curvatures, mean/Gaussian curvature for surfaces
// of revolution obtained by revolving lines, circles, ellipses, and parabolas.
// Cross-validates against GeomLProp_SLProps.

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_SLProps.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
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

  //! Revolution axis: Z axis through origin.
  const gp_Ax1 THE_Z_AXIS(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  //! Create cylinder by revolving a line parallel to Z around Z axis.
  //! Radius = 5.0, line at X=5 parallel to Z.
  occ::handle<Geom_SurfaceOfRevolution> makeRevolveLine()
  {
    occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
    return new Geom_SurfaceOfRevolution(aLine, THE_Z_AXIS);
  }

  //! Create torus by revolving a circle around Z axis.
  //! Major radius = 10.0, minor radius = 3.0.
  occ::handle<Geom_SurfaceOfRevolution> makeRevolveCircle()
  {
    gp_Ax2                   aCircAx(gp_Pnt(10.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
    occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircAx, 3.0);
    return new Geom_SurfaceOfRevolution(aCircle, THE_Z_AXIS);
  }

  //! Create surface by revolving an ellipse around Z axis.
  occ::handle<Geom_SurfaceOfRevolution> makeRevolveEllipse()
  {
    gp_Ax2                    anEllAx(gp_Pnt(8.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
    occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anEllAx, 4.0, 2.0);
    return new Geom_SurfaceOfRevolution(anEllipse, THE_Z_AXIS);
  }

  //! Create surface by revolving a parabola around Z axis.
  occ::handle<Geom_SurfaceOfRevolution> makeRevolveParabola()
  {
    gp_Ax2                     aParabAx(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
    occ::handle<Geom_Parabola> aParab = new Geom_Parabola(aParabAx, 1.0);
    return new Geom_SurfaceOfRevolution(aParab, THE_Z_AXIS);
  }
} // namespace

// Normal on cylinder-like surface (line revolved around Z)
TEST(GeomProp_SurfaceOfRevolutionTest, Normal_RevolveLine_IsCylinder)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  GeomProp_Surface                      aProp(aSurf);

  // Normal should point radially outward from Z axis
  for (double aU = 0.0; aU < 2.0 * M_PI; aU += M_PI / 4.0)
  {
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;
    // Normal Z component should be zero for a cylinder
    EXPECT_NEAR(aResult.Direction.Z(), 0.0, THE_DIR_TOL) << "Normal has Z component at u=" << aU;
  }
}

// Curvatures of revolved line (cylinder): one zero, one 1/R
TEST(GeomProp_SurfaceOfRevolutionTest, Curvatures_RevolveLine_IsCylinder)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  GeomProp_Surface                      aProp(aSurf);

  const double                           aExpectedK = 1.0 / 5.0; // 1/R where R=5
  const GeomProp::SurfaceCurvatureResult aResult    = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // MinCurvature = -1/R (concave, signed), MaxCurvature = 0 (along axis).
  EXPECT_NEAR(aResult.MinCurvature, -aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Gaussian curvature of revolved line is zero (developable surface)
TEST(GeomProp_SurfaceOfRevolutionTest, MeanGaussian_RevolveLine)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// Normal on torus (circle revolved around Z)
TEST(GeomProp_SurfaceOfRevolutionTest, Normal_RevolveCircle_IsTorus)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  GeomProp_Surface                      aProp(aSurf);

  for (int i = 0; i <= 4; ++i)
  {
    for (int j = 0; j <= 4; ++j)
    {
      const double                        aU      = i * M_PI / 2.0;
      const double                        aV      = j * M_PI / 2.0;
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
    }
  }
}

// Curvatures of revolved circle (torus)
TEST(GeomProp_SurfaceOfRevolutionTest, Curvatures_RevolveCircle)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  GeomProp_Surface                      aProp(aSurf);

  // Evaluate away from boundary parameter v=0 to avoid degenerate point.
  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
  // Both curvatures should be non-zero for a torus away from boundary
  EXPECT_GT(std::abs(aResult.MinCurvature), THE_CURV_TOL);
  EXPECT_GT(std::abs(aResult.MaxCurvature), THE_CURV_TOL);
}

// Mean and Gaussian curvature of revolved circle
TEST(GeomProp_SurfaceOfRevolutionTest, MeanGaussian_RevolveCircle)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  GeomProp_Surface                      aProp(aSurf);

  // Evaluate away from boundary parameter v=0 to avoid degenerate point.
  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // On the outer side of the torus, Gaussian curvature should be positive
  EXPECT_GT(aResult.GaussianCurvature, 0.0);
}

// GetType returns GeomAbs_SurfaceOfRevolution
TEST(GeomProp_SurfaceOfRevolutionTest, GetType_IsSurfaceOfRevolution)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  GeomProp_Surface                      aProp(aSurf);
  EXPECT_EQ(aProp.GetType(), GeomAbs_SurfaceOfRevolution);
}

// Cross-validate revolved line vs SLProps
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_RevolveLine)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -5.0, 5.0);
}

// Cross-validate revolved circle vs SLProps
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_RevolveCircle)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, 0.1, 2.0 * M_PI - 0.1);
}

// Dense cross-validation for revolved line
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_AllProperties_Line)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -3.0, 3.0, 6, 6);
}

// Dense cross-validation for revolved circle
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_AllProperties_Circle)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, 0.1, 2.0 * M_PI - 0.1, 6, 6);
}

// Normal on revolved ellipse
TEST(GeomProp_SurfaceOfRevolutionTest, Normal_RevolveEllipse)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveEllipse();
  GeomProp_Surface                      aProp(aSurf);

  for (int i = 0; i <= 4; ++i)
  {
    const double                        aU      = 0.5 + i * M_PI / 2.0;
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;
  }
}

// Curvatures on revolved ellipse: varying curvatures
TEST(GeomProp_SurfaceOfRevolutionTest, Curvatures_RevolveEllipse)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveEllipse();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
}

// Mean and Gaussian curvature on revolved ellipse
TEST(GeomProp_SurfaceOfRevolutionTest, MeanGaussian_RevolveEllipse)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveEllipse();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// Cross-validate revolved ellipse vs SLProps
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_RevolveEllipse)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveEllipse();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, 0.1, 2.0 * M_PI - 0.1);
}

// Normal on revolved parabola
TEST(GeomProp_SurfaceOfRevolutionTest, Normal_RevolveParabola)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveParabola();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::SurfaceNormalResult aResult = aProp.Normal(1.0, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
}

// Curvatures on revolved parabola
TEST(GeomProp_SurfaceOfRevolutionTest, Curvatures_RevolveParabola)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveParabola();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
}

// Cross-validate revolved parabola vs SLProps
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_RevolveParabola)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveParabola();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -2.0, 2.0);
}

// Dense 8x8 grid cross-validation for revolved circle
TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_DenseGrid_Circle)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, 0.1, 2.0 * M_PI - 0.1, 8, 8);
}

// Circumferential curvature is constant along U for revolved line (cylinder)
TEST(GeomProp_SurfaceOfRevolutionTest, Curvatures_ConstantAlongU)
{
  occ::handle<Geom_SurfaceOfRevolution> aSurf = makeRevolveLine();
  GeomProp_Surface                      aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aRefResult = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRefResult.IsDefined);

  for (double aU = 0.0; aU < 2.0 * M_PI; aU += M_PI / 4.0)
  {
    const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Curvatures undefined at u=" << aU;
    EXPECT_NEAR(aResult.MinCurvature, aRefResult.MinCurvature, THE_CURV_TOL)
      << "MinCurvature varies along U at u=" << aU;
    EXPECT_NEAR(aResult.MaxCurvature, aRefResult.MaxCurvature, THE_CURV_TOL)
      << "MaxCurvature varies along U at u=" << aU;
  }
}

TEST(GeomProp_SurfaceOfRevolutionTest, VsSLProps_CriticalPoints)
{
  // Revolve a line to get a cylinder
  occ::handle<Geom_Line>                aLine = new Geom_Line(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aSurf =
    new Geom_SurfaceOfRevolution(aLine, gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  // Critical: seam, various v, near-seam
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0e-10, 0.0},
                               {2.0 * M_PI - 1.0e-10, 0.0},
                               {M_PI / 2.0, 0.0},
                               {M_PI, 0.0},
                               {M_PI / 4.0, 1.0},
                               {M_PI / 4.0, -1.0},
                               {M_PI / 4.0, 100.0},
                               {0.0, 1.0e-10},
                               {M_PI, 1.0e-10}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSurf, aUV[0], aUV[1]);
    compareCurvatures(aSurf, aUV[0], aUV[1]);
    compareMeanGaussian(aSurf, aUV[0], aUV[1]);
  }
}
