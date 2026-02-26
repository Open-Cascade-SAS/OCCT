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

// Tests for GeomProp_Surface with Geom_SurfaceOfLinearExtrusion.
// Validates normal, curvatures, mean/Gaussian curvature for surfaces
// of extrusion obtained by sweeping circles, lines, ellipses, and Bezier
// curves along a direction. Cross-validates against GeomLProp_SLProps.

#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_SLProps.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
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

  //! Extrusion direction: along Z.
  const gp_Dir THE_EXTRUSION_DIR(0.0, 0.0, 1.0);

  //! Create cylinder by extruding a circle along Z.
  //! Circle of radius 5 in XY plane, extruded along Z.
  occ::handle<Geom_SurfaceOfLinearExtrusion> makeExtrudeCircle()
  {
    gp_Ax2                   aCircAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
    occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircAx, 5.0);
    return new Geom_SurfaceOfLinearExtrusion(aCircle, THE_EXTRUSION_DIR);
  }

  //! Create plane by extruding a line along Z.
  occ::handle<Geom_SurfaceOfLinearExtrusion> makeExtrudeLine()
  {
    occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
    return new Geom_SurfaceOfLinearExtrusion(aLine, THE_EXTRUSION_DIR);
  }

  //! Create surface by extruding an ellipse along Z.
  occ::handle<Geom_SurfaceOfLinearExtrusion> makeExtrudeEllipse()
  {
    gp_Ax2                    anEllAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
    occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anEllAx, 6.0, 3.0);
    return new Geom_SurfaceOfLinearExtrusion(anEllipse, THE_EXTRUSION_DIR);
  }

  //! Create surface by extruding a cubic Bezier curve along Z.
  occ::handle<Geom_SurfaceOfLinearExtrusion> makeExtrudeBezier()
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1)                             = gp_Pnt(0.0, 0.0, 0.0);
    aPoles(2)                             = gp_Pnt(1.0, 3.0, 0.0);
    aPoles(3)                             = gp_Pnt(3.0, -1.0, 0.0);
    aPoles(4)                             = gp_Pnt(4.0, 1.0, 0.0);
    occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
    return new Geom_SurfaceOfLinearExtrusion(aBezier, THE_EXTRUSION_DIR);
  }
} // namespace

// Normal on extruded circle (cylinder) points radially outward
TEST(GeomProp_SurfaceOfExtrusionTest, Normal_ExtrudeCircle_IsCylinder)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  GeomProp_Surface                           aProp(aSurf);

  for (double aU = 0.0; aU < 2.0 * M_PI; aU += M_PI / 4.0)
  {
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 1.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;
    // Normal should be perpendicular to Z axis for a cylinder
    EXPECT_NEAR(aResult.Direction.Z(), 0.0, THE_DIR_TOL) << "Normal has Z component at u=" << aU;
  }
}

// Curvatures of extruded circle: one zero, one 1/R
TEST(GeomProp_SurfaceOfExtrusionTest, Curvatures_ExtrudeCircle)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  GeomProp_Surface                           aProp(aSurf);

  const double                           aExpectedK = 1.0 / 5.0;
  const GeomProp::SurfaceCurvatureResult aResult    = aProp.Curvatures(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  // MinCurvature = -1/R (concave, signed), MaxCurvature = 0 (along extrusion).
  EXPECT_NEAR(aResult.MinCurvature, -aExpectedK, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Gaussian curvature of extruded circle is zero (developable)
TEST(GeomProp_SurfaceOfExtrusionTest, MeanGaussian_ExtrudeCircle)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// Normal on extruded line (plane) is constant
TEST(GeomProp_SurfaceOfExtrusionTest, Normal_ExtrudeLine_IsPlane)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeLine();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::SurfaceNormalResult aRef = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRef.IsDefined);

  for (double aU = -5.0; aU <= 5.0; aU += 2.5)
  {
    for (double aV = -5.0; aV <= 5.0; aV += 2.5)
    {
      const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, aV, THE_LIN_TOL);
      ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at (" << aU << "," << aV << ")";
      const double aDot = aResult.Direction.Dot(aRef.Direction);
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal not constant at (" << aU << "," << aV << ")";
    }
  }
}

// Curvatures of extruded line: all zero
TEST(GeomProp_SurfaceOfExtrusionTest, Curvatures_ExtrudeLine)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeLine();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Mean and Gaussian curvature of extruded line: H=K=0
TEST(GeomProp_SurfaceOfExtrusionTest, MeanGaussian_ExtrudeLine)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeLine();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 1.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// GetType returns GeomAbs_SurfaceOfLinearExtrusion
TEST(GeomProp_SurfaceOfExtrusionTest, GetType_IsSurfaceOfExtrusion)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  GeomProp_Surface                           aProp(aSurf);
  EXPECT_EQ(aProp.GetType(), GeomAbs_SurfaceOfExtrusion);
}

// Cross-validate extruded circle vs SLProps
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_ExtrudeCircle)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -5.0, 5.0);
}

// Cross-validate extruded line vs SLProps
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_ExtrudeLine)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeLine();
  compareAllSurface(aSurf, -5.0, 5.0, -5.0, 5.0);
}

// Dense cross-validation for extruded circle
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_AllProperties_Circle)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -3.0, 3.0, 6, 6);
}

// Dense cross-validation for extruded line
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_AllProperties_Line)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeLine();
  compareAllSurface(aSurf, -5.0, 5.0, -5.0, 5.0, 6, 6);
}

// Normal on extruded ellipse: varying normal direction
TEST(GeomProp_SurfaceOfExtrusionTest, Normal_ExtrudeEllipse)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeEllipse();
  GeomProp_Surface                           aProp(aSurf);

  for (double aU = 0.0; aU < 2.0 * M_PI; aU += M_PI / 4.0)
  {
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;
    // Normal should be perpendicular to extrusion direction Z
    EXPECT_NEAR(aResult.Direction.Z(), 0.0, THE_DIR_TOL);
  }
}

// Curvatures on extruded ellipse: varying along U
TEST(GeomProp_SurfaceOfExtrusionTest, Curvatures_ExtrudeEllipse)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeEllipse();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
  // MaxCurvature is zero (along extrusion direction), MinCurvature is from ellipse (negative).
  EXPECT_NEAR(aResult.MaxCurvature, 0.0, THE_CURV_TOL);
}

// Gaussian curvature of extruded ellipse is zero (developable)
TEST(GeomProp_SurfaceOfExtrusionTest, MeanGaussian_ExtrudeEllipse)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeEllipse();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::MeanGaussianResult aResult = aProp.MeanGaussian(1.0, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_NEAR(aResult.GaussianCurvature, 0.0, THE_CURV_TOL);
}

// Cross-validate extruded ellipse vs SLProps
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_ExtrudeEllipse)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeEllipse();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -3.0, 3.0);
}

// Normal on extruded Bezier curve
TEST(GeomProp_SurfaceOfExtrusionTest, Normal_ExtrudeBezier)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeBezier();
  GeomProp_Surface                           aProp(aSurf);

  for (double aU = 0.0; aU <= 1.0; aU += 0.25)
  {
    const GeomProp::SurfaceNormalResult aResult = aProp.Normal(aU, 0.0, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Normal undefined at u=" << aU;
  }
}

// Curvatures on extruded Bezier: varies with U
TEST(GeomProp_SurfaceOfExtrusionTest, Curvatures_ExtrudeBezier)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeBezier();
  GeomProp_Surface                           aProp(aSurf);

  const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(0.5, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aResult.IsDefined);
  EXPECT_LE(aResult.MinCurvature, aResult.MaxCurvature);
}

// Cross-validate extruded Bezier vs SLProps
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_ExtrudeBezier)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeBezier();
  compareAllSurface(aSurf, 0.05, 0.95, -3.0, 3.0);
}

// Curvatures are constant along extrusion direction (V parameter)
TEST(GeomProp_SurfaceOfExtrusionTest, Curvatures_ConstantAlongV)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  GeomProp_Surface                           aProp(aSurf);

  const double                           aU         = 1.0;
  const GeomProp::SurfaceCurvatureResult aRefResult = aProp.Curvatures(aU, 0.0, THE_LIN_TOL);
  ASSERT_TRUE(aRefResult.IsDefined);

  for (double aV = -5.0; aV <= 5.0; aV += 2.5)
  {
    const GeomProp::SurfaceCurvatureResult aResult = aProp.Curvatures(aU, aV, THE_LIN_TOL);
    ASSERT_TRUE(aResult.IsDefined) << "Curvatures undefined at v=" << aV;
    EXPECT_NEAR(aResult.MinCurvature, aRefResult.MinCurvature, THE_CURV_TOL)
      << "MinCurvature varies along V at v=" << aV;
    EXPECT_NEAR(aResult.MaxCurvature, aRefResult.MaxCurvature, THE_CURV_TOL)
      << "MaxCurvature varies along V at v=" << aV;
  }
}

// Dense 8x8 grid cross-validation for extruded circle
TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_DenseGrid_Circle)
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf = makeExtrudeCircle();
  compareAllSurface(aSurf, 0.1, 2.0 * M_PI - 0.1, -3.0, 3.0, 8, 8);
}

TEST(GeomProp_SurfaceOfExtrusionTest, VsSLProps_CriticalPoints)
{
  // Extrude a circle along Z
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> aSurf =
    new Geom_SurfaceOfLinearExtrusion(aCircle, gp_Dir(0, 0, 1));
  // Critical: seam, various angles, near-seam
  const double aParams[][2] = {{0.0, 0.0},
                               {1.0e-10, 0.0},
                               {2.0 * M_PI - 1.0e-10, 0.0},
                               {M_PI / 2.0, 0.0},
                               {M_PI, 0.0},
                               {0.0, 1.0e-10},
                               {0.0, 1.0e6},
                               {0.0, -1.0e6},
                               {M_PI / 4.0, 0.5},
                               {M_PI / 4.0, -0.5}};
  for (const auto& aUV : aParams)
  {
    compareNormal(aSurf, aUV[0], aUV[1]);
    compareCurvatures(aSurf, aUV[0], aUV[1]);
    compareMeanGaussian(aSurf, aUV[0], aUV[1]);
  }
}
