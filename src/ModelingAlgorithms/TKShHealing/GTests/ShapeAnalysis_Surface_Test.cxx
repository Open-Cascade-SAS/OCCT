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

#include <gtest/gtest.h>

#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <NCollection_Array2.hxx>

//! Test fixture for ShapeAnalysis_Surface::ValueOfUV tests
class ShapeAnalysis_SurfaceTest : public testing::Test
{
protected:
  //! Verify that ValueOfUV finds correct parameters for a point on surface
  void VerifyValueOfUV(const Handle(Geom_Surface)& theSurf,
                       const gp_Pnt&               thePoint,
                       double                      thePrecision,
                       double                      theExpectedU,
                       double                      theExpectedV,
                       double                      theParamTol = 1e-4)
  {
    Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(theSurf);
    gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(thePoint, thePrecision);

    // Check that distance is within precision
    gp_Pnt aSurfPnt = theSurf->Value(aResult.X(), aResult.Y());
    double aDist    = thePoint.Distance(aSurfPnt);
    EXPECT_LE(aDist, thePrecision * 10)
      << "Distance " << aDist << " exceeds precision " << thePrecision;

    // Check parameters are close to expected (if point is on surface)
    if (aDist < thePrecision)
    {
      // For periodic surfaces, parameters may differ by period
      double dU = std::abs(aResult.X() - theExpectedU);
      double dV = std::abs(aResult.Y() - theExpectedV);

      // Handle U periodicity (2*PI)
      if (dU > M_PI)
        dU = 2 * M_PI - dU;

      EXPECT_LT(dU, theParamTol) << "U parameter mismatch: got " << aResult.X() << ", expected "
                                 << theExpectedU;
      EXPECT_LT(dV, theParamTol) << "V parameter mismatch: got " << aResult.Y() << ", expected "
                                 << theExpectedV;
    }
  }
};

//==================================================================================================
// Plane Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, Plane_PointOnSurface_A1)
{
  gp_Pln                  aPln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aSurf = new Geom_Plane(aPln);

  gp_Pnt aPoint(5.0, 3.0, 0.0);
  VerifyValueOfUV(aSurf, aPoint, 0.001, 5.0, 3.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Plane_PointAboveSurface_A2)
{
  gp_Pln                  aPln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aSurf = new Geom_Plane(aPln);

  // Point above plane - projection should find (5, 3)
  gp_Pnt aPoint(5.0, 3.0, 10.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  EXPECT_NEAR(aResult.X(), 5.0, 1e-6);
  EXPECT_NEAR(aResult.Y(), 3.0, 1e-6);
}

//==================================================================================================
// Cylinder Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, Cylinder_PointOnSurface_B1)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  // Point at U=0 (X direction), V=3
  gp_Pnt aPoint(5.0, 0.0, 3.0);
  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, 3.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Cylinder_PointAtPiHalf_B2)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  // Point at U=PI/2 (Y direction), V=7
  gp_Pnt aPoint(0.0, 5.0, 7.0);
  VerifyValueOfUV(aSurf, aPoint, 0.001, M_PI / 2, 7.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Cylinder_PointNearSurface_B3)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  // Point slightly outside cylinder
  gp_Pnt aPoint(5.1, 0.0, 3.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  EXPECT_NEAR(aResult.X(), 0.0, 0.01);
  EXPECT_NEAR(aResult.Y(), 3.0, 1e-6);
}

//==================================================================================================
// Cone Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, Cone_PointOnSurface_C1)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface(anAxis, M_PI / 6, 5.0); // 30 degrees

  // At V=0, radius = 5
  gp_Pnt aPoint(5.0, 0.0, 0.0);
  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, 0.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Cone_PointAtPositiveV_C2)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface(anAxis, M_PI / 6, 5.0); // 30 degrees

  // At V, radius = 5 + V*sin(30) = 5 + V*0.5
  // At V, Z = V*cos(30) = V*0.866
  // For V=5.7735: radius = 5 + 2.887 = 7.887, Z = 5
  double aV      = 5.7735;
  double aRadius = 5.0 + aV * std::sin(M_PI / 6);
  double aZ      = aV * std::cos(M_PI / 6);
  gp_Pnt aPoint(aRadius, 0.0, aZ);

  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, aV, 0.01);
}

TEST_F(ShapeAnalysis_SurfaceTest, Cone_PointAtAngle_C3)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface(anAxis, M_PI / 6, 5.0);

  // At U=PI/4, V=2
  double aU      = M_PI / 4;
  double aV      = 2.0;
  double aRadius = 5.0 + aV * std::sin(M_PI / 6);
  double aZ      = aV * std::cos(M_PI / 6);
  gp_Pnt aPoint(aRadius * std::cos(aU), aRadius * std::sin(aU), aZ);

  VerifyValueOfUV(aSurf, aPoint, 0.001, aU, aV, 0.01);
}

TEST_F(ShapeAnalysis_SurfaceTest, Cone_NearApex_C4)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface(anAxis, M_PI / 6, 5.0);

  // Apex is at V = -RefRadius/sin(alpha) = -5/0.5 = -10
  // Axial position of apex = V*cos(alpha) = -10*0.866 = -8.66
  double aApexV = -5.0 / std::sin(M_PI / 6);
  double aApexZ = aApexV * std::cos(M_PI / 6);

  // Point near apex
  gp_Pnt aPoint(0.1, 0.0, aApexZ + 0.1);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  // Just verify we get a reasonable result near apex
  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 1.0); // Should be reasonably close
}

//==================================================================================================
// Sphere Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, Sphere_PointOnEquator_D1)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSurf = new Geom_SphericalSurface(anAxis, 5.0);

  // Point on equator (V=0) at U=0
  gp_Pnt aPoint(5.0, 0.0, 0.0);
  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, 0.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Sphere_PointAtLatitude_D2)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSurf = new Geom_SphericalSurface(anAxis, 5.0);

  // Point at U=PI/4, V=PI/6 (latitude 30 degrees)
  double aU = M_PI / 4;
  double aV = M_PI / 6;
  gp_Pnt aPoint = aSurf->Value(aU, aV);

  VerifyValueOfUV(aSurf, aPoint, 0.001, aU, aV, 0.01);
}

TEST_F(ShapeAnalysis_SurfaceTest, Sphere_PointNearPole_D3)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSurf = new Geom_SphericalSurface(anAxis, 5.0);

  // Point near north pole (V close to PI/2)
  gp_Pnt aPoint(0.0, 0.0, 4.99);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 0.02);
}

//==================================================================================================
// Torus Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, Torus_PointOnSurface_E1)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);

  // Point at U=0, V=0 (outer edge in X direction)
  gp_Pnt aPoint(13.0, 0.0, 0.0); // R + r = 10 + 3 = 13
  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, 0.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, Torus_PointAtAngles_E2)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);

  // Point at U=PI/3, V=PI/4
  double aU = M_PI / 3;
  double aV = M_PI / 4;
  gp_Pnt aPoint = aSurf->Value(aU, aV);

  VerifyValueOfUV(aSurf, aPoint, 0.001, aU, aV, 0.01);
}

TEST_F(ShapeAnalysis_SurfaceTest, Torus_InnerPoint_E3)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);

  // Point at inner edge (V=PI)
  gp_Pnt aPoint(7.0, 0.0, 0.0); // R - r = 10 - 3 = 7
  VerifyValueOfUV(aSurf, aPoint, 0.001, 0.0, M_PI, 0.01);
}

//==================================================================================================
// BSpline Surface Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, BSpline_FlatSurface_F1)
{
  // Create a simple flat BSpline surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 5.0, (j - 1) * 5.0, 0.0);
    }
  }

  GeomAPI_PointsToBSplineSurface aBuilder(aPoles, 2, 2, GeomAbs_C1, 1e-6);
  Handle(Geom_BSplineSurface)    aSurf = aBuilder.Surface();

  // Point on surface
  gp_Pnt aPoint(5.0, 5.0, 0.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 0.01);
}

TEST_F(ShapeAnalysis_SurfaceTest, BSpline_CurvedSurface_F2)
{
  // Create a curved BSpline surface (dome shape)
  NCollection_Array2<gp_Pnt> aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double x = (i - 3) * 2.0;
      double y = (j - 3) * 2.0;
      double z = 10.0 - 0.5 * (x * x + y * y);
      aPoles(i, j) = gp_Pnt(x, y, z);
    }
  }

  GeomAPI_PointsToBSplineSurface aBuilder(aPoles, 3, 3, GeomAbs_C2, 1e-6);
  Handle(Geom_BSplineSurface)    aSurf = aBuilder.Surface();

  // Point at center
  gp_Pnt aPoint(0.0, 0.0, 10.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.01);

  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 0.1);
}

//==================================================================================================
// Edge Case Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, EdgeCase_VerySmallPrecision_G1)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  gp_Pnt aPoint(5.0, 0.0, 3.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 1e-10);

  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 1e-6);
}

TEST_F(ShapeAnalysis_SurfaceTest, EdgeCase_LargePrecision_G2)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  gp_Pnt aPoint(5.0, 0.0, 3.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 1.0);

  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  double aDist    = aPoint.Distance(aSurfPnt);
  EXPECT_LT(aDist, 1.0);
}

TEST_F(ShapeAnalysis_SurfaceTest, EdgeCase_PointFarFromSurface_G3)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSurf = new Geom_SphericalSurface(anAxis, 5.0);

  // Point very far from sphere
  gp_Pnt aPoint(100.0, 0.0, 0.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);
  gp_Pnt2d                      aResult    = anAnalyzer->ValueOfUV(aPoint, 0.001);

  // Should find point on sphere closest to query point (at U=0, V=0)
  gp_Pnt aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
  EXPECT_NEAR(aSurfPnt.X(), 5.0, 0.1);
  EXPECT_NEAR(aSurfPnt.Y(), 0.0, 0.1);
  EXPECT_NEAR(aSurfPnt.Z(), 0.0, 0.1);
}

TEST_F(ShapeAnalysis_SurfaceTest, EdgeCase_MultipleQueries_G4)
{
  gp_Ax3                           anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);

  // Multiple queries should all work correctly
  for (int i = 0; i < 10; ++i)
  {
    double aU     = i * M_PI / 5;
    double aV     = i * 2.0;
    gp_Pnt aPoint = aSurf->Value(aU, aV);

    gp_Pnt2d aResult  = anAnalyzer->ValueOfUV(aPoint, 0.001);
    gp_Pnt   aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
    double   aDist    = aPoint.Distance(aSurfPnt);

    EXPECT_LT(aDist, 0.001) << "Failed at iteration " << i;
  }
}

//==================================================================================================
// Stress Tests
//==================================================================================================

TEST_F(ShapeAnalysis_SurfaceTest, StressTest_Cone_H1)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface(anAxis, M_PI / 6, 5.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);

  // Test many points on the cone surface
  for (int iU = 0; iU < 20; ++iU)
  {
    for (int iV = -5; iV < 10; ++iV)
    {
      double aU     = iU * M_PI / 10;
      double aV     = iV * 1.0;
      gp_Pnt aPoint = aSurf->Value(aU, aV);

      gp_Pnt2d aResult  = anAnalyzer->ValueOfUV(aPoint, 0.001);
      gp_Pnt   aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
      double   aDist    = aPoint.Distance(aSurfPnt);

      EXPECT_LT(aDist, 0.01) << "Failed at U=" << aU << ", V=" << aV;
    }
  }
}

TEST_F(ShapeAnalysis_SurfaceTest, StressTest_Sphere_H2)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSurf = new Geom_SphericalSurface(anAxis, 5.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);

  // Test many points on the sphere surface
  for (int iU = 0; iU < 20; ++iU)
  {
    for (int iV = -8; iV < 9; ++iV)
    {
      double aU     = iU * M_PI / 10;
      double aV     = iV * M_PI / 20;
      gp_Pnt aPoint = aSurf->Value(aU, aV);

      gp_Pnt2d aResult  = anAnalyzer->ValueOfUV(aPoint, 0.001);
      gp_Pnt   aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
      double   aDist    = aPoint.Distance(aSurfPnt);

      EXPECT_LT(aDist, 0.01) << "Failed at U=" << aU << ", V=" << aV;
    }
  }
}

TEST_F(ShapeAnalysis_SurfaceTest, StressTest_Torus_H3)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);

  Handle(ShapeAnalysis_Surface) anAnalyzer = new ShapeAnalysis_Surface(aSurf);

  // Test many points on the torus surface
  for (int iU = 0; iU < 20; ++iU)
  {
    for (int iV = 0; iV < 20; ++iV)
    {
      double aU     = iU * M_PI / 10;
      double aV     = iV * M_PI / 10;
      gp_Pnt aPoint = aSurf->Value(aU, aV);

      gp_Pnt2d aResult  = anAnalyzer->ValueOfUV(aPoint, 0.001);
      gp_Pnt   aSurfPnt = aSurf->Value(aResult.X(), aResult.Y());
      double   aDist    = aPoint.Distance(aSurfPnt);

      EXPECT_LT(aDist, 0.01) << "Failed at U=" << aU << ", V=" << aV;
    }
  }
}
