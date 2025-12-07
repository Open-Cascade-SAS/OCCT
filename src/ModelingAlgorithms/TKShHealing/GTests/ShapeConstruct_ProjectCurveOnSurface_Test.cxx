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

#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <GC_MakeSegment.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <TColgp_Array1OfPnt.hxx>

//==================================================================================================
// Test fixture for ShapeConstruct_ProjectCurveOnSurface
//==================================================================================================

class ShapeConstruct_ProjectCurveOnSurfaceTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  //! Helper to create a simple B-spline curve lying on Z=0 plane
  Handle(Geom_BSplineCurve) createPlanarBSpline()
  {
    TColgp_Array1OfPnt aPoles(1, 5);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(2, 3, 0);
    aPoles(3) = gp_Pnt(5, 4, 0);
    aPoles(4) = gp_Pnt(8, 2, 0);
    aPoles(5) = gp_Pnt(10, 0, 0);

    GeomAPI_PointsToBSpline aBuilder(aPoles);
    return aBuilder.Curve();
  }

  //! Helper to create a helical B-spline on a cylinder
  Handle(Geom_BSplineCurve) createHelicalBSpline(const double theRadius, const int theNbPoints)
  {
    TColgp_Array1OfPnt aPoles(1, theNbPoints);
    for (int i = 1; i <= theNbPoints; ++i)
    {
      const double anAngle = (i - 1) * M_PI / 4.0;
      const double aZ      = (i - 1) * 1.0;
      aPoles(i)            = gp_Pnt(theRadius * cos(anAngle), theRadius * sin(anAngle), aZ);
    }
    GeomAPI_PointsToBSpline aBuilder(aPoles);
    return aBuilder.Curve();
  }

  //! Verifies that pcurve correctly maps to 3D points on surface
  void verifyProjection(const Handle(Geom_Curve)&            theCurve,
                        const Handle(Geom2d_Curve)&          thePCurve,
                        const Handle(ShapeAnalysis_Surface)& theSurface,
                        const double                         theTolerance)
  {
    const double aFirst = theCurve->FirstParameter();
    const double aLast  = theCurve->LastParameter();

    for (double t = 0.0; t <= 1.0; t += 0.25)
    {
      const double aParam = aFirst + t * (aLast - aFirst);
      gp_Pnt       a3DPnt;
      theCurve->D0(aParam, a3DPnt);

      const gp_Pnt2d a2DPnt   = thePCurve->Value(aParam);
      gp_Pnt         aSurfPnt = theSurface->Value(a2DPnt);

      EXPECT_NEAR(a3DPnt.Distance(aSurfPnt), 0.0, theTolerance) << "Point mismatch at t=" << t;
    }
  }

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  double                                       myTolerance;
};

//==================================================================================================
// Basic projection tests - lines and circles on analytical surfaces
//==================================================================================================

// Test: Project diagonal line on horizontal plane, verify 2D coordinates match X,Y of 3D points
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, LineOnPlane_A1)
{
  Handle(Geom_Plane)        aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_TrimmedCurve) aLine  = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const double           aFirst  = aLine->FirstParameter();
  const double           aLast   = aLine->LastParameter();
  const Standard_Boolean aResult = myProjector->Perform(aLine, aFirst, aLast, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Verify start point maps to (0,0) and end point maps to (10,10)
  const gp_Pnt2d aStart = aPCurve->Value(aFirst);
  const gp_Pnt2d aEnd   = aPCurve->Value(aLast);
  EXPECT_NEAR(aStart.X(), 0.0, 0.01);
  EXPECT_NEAR(aStart.Y(), 0.0, 0.01);
  EXPECT_NEAR(aEnd.X(), 10.0, 0.01);
  EXPECT_NEAR(aEnd.Y(), 10.0, 0.01);
}

// Test: Project circle lying on cylinder surface, V-coordinate should be constant
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, CircleOnCylinder_A2)
{
  const double                    aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Circle at height Z=3 on the cylinder
  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, 3), gp_Dir(0, 0, 1)), aRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aCylinder), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // On cylinder parametrization, V = Z, so circle at Z=3 should have V=3
  const gp_Pnt2d aMid = aPCurve->Value(M_PI);
  EXPECT_NEAR(aMid.Y(), 3.0, 0.01) << "V coordinate should be 3";
}

// Test: Project vertical line on cylinder (isoparametric case), U should be constant
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, IsoparametricLineOnCylinder_A3)
{
  const double                    aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Line along cylinder axis at X=radius, Y=0 (U=0 on cylinder)
  Handle(Geom_TrimmedCurve) aLine =
    GC_MakeSegment(gp_Pnt(aRadius, 0, 0), gp_Pnt(aRadius, 0, 10)).Value();

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aCylinder);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const double           aFirst  = aLine->FirstParameter();
  const double           aLast   = aLine->LastParameter();
  const Standard_Boolean aResult = myProjector->Perform(aLine, aFirst, aLast, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // U should be constant (isoparametric curve)
  const gp_Pnt2d aStart = aPCurve->Value(aFirst);
  const gp_Pnt2d aEnd   = aPCurve->Value(aLast);
  EXPECT_NEAR(aStart.X(), aEnd.X(), 0.01) << "U should be constant for isoparametric curve";
}

//==================================================================================================
// B-Spline curve projection tests
//==================================================================================================

// Test: Project planar B-spline on plane, endpoints should match original curve
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, BSplineOnPlane_B1)
{
  Handle(Geom_Plane)        aPlane   = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_BSplineCurve) aBSpline = createPlanarBSpline();
  ASSERT_FALSE(aBSpline.IsNull());

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aBSpline->FirstParameter(), aBSpline->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Verify endpoints
  const gp_Pnt2d aStart = aPCurve->Value(aBSpline->FirstParameter());
  const gp_Pnt2d aEnd   = aPCurve->Value(aBSpline->LastParameter());
  EXPECT_NEAR(aStart.X(), 0.0, 0.1);
  EXPECT_NEAR(aStart.Y(), 0.0, 0.1);
  EXPECT_NEAR(aEnd.X(), 10.0, 0.1);
  EXPECT_NEAR(aEnd.Y(), 0.0, 0.1);
}

// Test: Project helical B-spline on cylinder, verify all sample points lie on surface
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, HelicalBSplineOnCylinder_B2)
{
  const double                    aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  Handle(Geom_BSplineCurve) aBSpline = createHelicalBSpline(aRadius, 9);
  ASSERT_FALSE(aBSpline.IsNull());

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aCylinder);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aBSpline->FirstParameter(), aBSpline->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  verifyProjection(aBSpline, aPCurve, aSAS, 0.1);
}

// Test: Project high-degree B-spline with many control points
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, HighDegreeBSpline_B3)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create B-spline with 50 control points (sinusoidal shape)
  const int          aNbPoles = 50;
  TColgp_Array1OfPnt aPoles(1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    const double aX = (i - 1) * 0.5;
    const double aY = 5.0 * sin(aX * M_PI / 10.0);
    aPoles(i)       = gp_Pnt(aX, aY, 0);
  }

  GeomAPI_PointsToBSpline   aBuilder(aPoles, 3, 8);
  Handle(Geom_BSplineCurve) aBSpline = aBuilder.Curve();
  ASSERT_FALSE(aBSpline.IsNull());

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aBSpline->FirstParameter(), aBSpline->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  verifyProjection(aBSpline, aPCurve, aSAS, 0.5);
}

//==================================================================================================
// Periodic surface tests - sphere, torus
//==================================================================================================

// Test: Project equator circle on sphere
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, EquatorOnSphere_C1)
{
  const double                  aRadius = 10.0;
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aSphere), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

// Test: Project latitude circle on sphere at 45 degrees, V should be constant
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, LatitudeOnSphere_C2)
{
  const double                  aRadius = 10.0;
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Latitude at 45 degrees
  const double aLatitude   = M_PI / 4.0;
  const double aZ          = aRadius * sin(aLatitude);
  const double aCircRadius = aRadius * cos(aLatitude);

  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), aCircRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aSphere), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // V should be constant for latitude circle
  const gp_Pnt2d aStart = aPCurve->Value(0.0);
  const gp_Pnt2d aMid   = aPCurve->Value(M_PI);
  EXPECT_NEAR(aStart.Y(), aMid.Y(), 0.01) << "V should be constant for latitude circle";
}

// Test: Project on toroidal surface (both U and V periodic)
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, CircleOnTorus_C3)
{
  const double                 aMajorRadius = 10.0;
  const double                 aMinorRadius = 2.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajorRadius, aMinorRadius);

  // Circle in the minor cross-section plane at U=0
  gp_Circ aCirc(gp_Ax2(gp_Pnt(aMajorRadius + aMinorRadius, 0, 0), gp_Dir(0, 1, 0)), aMinorRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aTorus), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

//==================================================================================================
// Conical surface tests
//==================================================================================================

// Test: Project circle on cone at specific height
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, CircleOnCone_D1)
{
  const double                aRadius    = 5.0;
  const double                aSemiAngle = M_PI / 6.0; // 30 degrees
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aSemiAngle, aRadius);

  // Circle at height Z=5, radius increases with height
  const double        aZ         = 5.0;
  const double        aRadiusAtZ = aRadius + aZ * tan(aSemiAngle);
  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), aRadiusAtZ);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aCone), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

//==================================================================================================
// Trimmed curve tests
//==================================================================================================

// Test: Project 90-degree arc on plane
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, ArcOnPlane_E1)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  gp_Circ                   aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_Circle)       aFullCircle = new Geom_Circle(aCirc);
  Handle(Geom_TrimmedCurve) aArc        = new Geom_TrimmedCurve(aFullCircle, 0.0, M_PI / 2.0);

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const double           aFirst  = aArc->FirstParameter();
  const double           aLast   = aArc->LastParameter();
  const Standard_Boolean aResult = myProjector->Perform(aArc, aFirst, aLast, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Arc from (5,0) to (0,5)
  const gp_Pnt2d aStart = aPCurve->Value(aFirst);
  const gp_Pnt2d aEnd   = aPCurve->Value(aLast);
  EXPECT_NEAR(aStart.X(), 5.0, 0.01);
  EXPECT_NEAR(aStart.Y(), 0.0, 0.01);
  EXPECT_NEAR(aEnd.X(), 0.0, 0.01);
  EXPECT_NEAR(aEnd.Y(), 5.0, 0.01);
}

// Test: Project doubly-trimmed curve (tests recursive unwrapping of trimmed curves)
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, DoublyTrimmedCurve_E2)
{
  Handle(Geom_Plane)        aPlane   = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_BSplineCurve) aBSpline = createPlanarBSpline();
  ASSERT_FALSE(aBSpline.IsNull());

  // First trim: take first half
  const double              aFirst1 = aBSpline->FirstParameter();
  const double              aLast1  = aBSpline->LastParameter();
  const double              aMid    = (aFirst1 + aLast1) / 2.0;
  Handle(Geom_TrimmedCurve) aTrim1  = new Geom_TrimmedCurve(aBSpline, aFirst1, aMid);

  // Second trim (nested): take first quarter of the first trim
  const double              aFirst2 = aTrim1->FirstParameter();
  const double              aLast2  = aTrim1->LastParameter();
  const double              aQuart  = aFirst2 + (aLast2 - aFirst2) / 4.0;
  Handle(Geom_TrimmedCurve) aTrim2  = new Geom_TrimmedCurve(aTrim1, aFirst2, aQuart);

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aTrim2, aTrim2->FirstParameter(), aTrim2->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Projection of doubly-trimmed curve should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

//==================================================================================================
// Ellipse projection tests
//==================================================================================================

// Test: Project ellipse on plane, verify quadrant points
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, EllipseOnPlane_F1)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Ellipse with major radius 10, minor radius 5
  gp_Elips             anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(anElips);

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(anEllipse, 0.0, 2.0 * M_PI, aPCurve);

  ASSERT_TRUE(aResult) << "Projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // At parameter 0, ellipse is at (10, 0)
  gp_Pnt2d aP0 = aPCurve->Value(0.0);
  EXPECT_NEAR(aP0.X(), 10.0, 0.01);
  EXPECT_NEAR(aP0.Y(), 0.0, 0.01);

  // At parameter PI/2, ellipse is at (0, 5)
  gp_Pnt2d aP1 = aPCurve->Value(M_PI / 2.0);
  EXPECT_NEAR(aP1.X(), 0.0, 0.01);
  EXPECT_NEAR(aP1.Y(), 5.0, 0.01);
}

//==================================================================================================
// API method tests - Init, SetSurface, SetPrecision
//==================================================================================================

// Test: Init with Geom_Surface directly (not wrapped in ShapeAnalysis_Surface)
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, InitWithGeomSurface_G1)
{
  Handle(Geom_Plane)        aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_TrimmedCurve) aLine  = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  myProjector->Init(aPlane, Precision::Confusion());

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Init with Geom_Surface should work";
  ASSERT_FALSE(aPCurve.IsNull());
}

// Test: SetSurface + SetPrecision separately
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, SetSurfaceAndPrecision_G2)
{
  Handle(Geom_Plane)        aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_TrimmedCurve) aLine  = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  myProjector->SetSurface(aPlane);
  myProjector->SetPrecision(Precision::Confusion());

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "SetSurface + SetPrecision should work";
  ASSERT_FALSE(aPCurve.IsNull());
}

// Test: AdjustOverDegenMode accessor returns modifiable reference
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, AdjustOverDegenModeAccessor_G3)
{
  // Default value should be 1
  EXPECT_EQ(myProjector->AdjustOverDegenMode(), 1);

  // Modify through reference
  myProjector->AdjustOverDegenMode() = 0;
  EXPECT_EQ(myProjector->AdjustOverDegenMode(), 0);

  // Restore
  myProjector->AdjustOverDegenMode() = 1;
  EXPECT_EQ(myProjector->AdjustOverDegenMode(), 1);
}

// Test: Status method returns meaningful status after projection
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, StatusMethod_G4)
{
  Handle(Geom_Plane)        aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_TrimmedCurve) aLine  = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  Handle(Geom2d_Curve) aPCurve;
  myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  // Status should indicate OK or DONE
  EXPECT_TRUE(myProjector->Status(ShapeExtend_OK) || myProjector->Status(ShapeExtend_DONE))
    << "Status should indicate OK or DONE";
}

//==================================================================================================
// Tolerance and precision tests
//==================================================================================================

// Test: Projection with very tight tolerance on exact curve
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, TightTolerance_H1)
{
  Handle(Geom_Plane)        aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_TrimmedCurve) aLine  = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1e-15);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Tight tolerance should work for exact curve";
  ASSERT_FALSE(aPCurve.IsNull());
}

// Test: Projection with different endpoint tolerances
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, DifferentEndpointTolerances_H2)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Curve slightly off plane at end
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(5, 5, 0.05);
  aPoles(3) = gp_Pnt(10, 10, 0.1);

  GeomAPI_PointsToBSpline   aBuilder(aPoles, 2, 2);
  Handle(Geom_BSplineCurve) aBSpline = aBuilder.Curve();
  ASSERT_FALSE(aBSpline.IsNull());

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const double           aTolFirst = 0.01;
  const double           aTolLast  = 0.2;
  const Standard_Boolean aResult   = myProjector->Perform(aBSpline,
                                                        aBSpline->FirstParameter(),
                                                        aBSpline->LastParameter(),
                                                        aPCurve,
                                                        aTolFirst,
                                                        aTolLast);

  ASSERT_TRUE(aResult) << "Projection with different endpoint tolerances should succeed";
  ASSERT_FALSE(aPCurve.IsNull());
}

//==================================================================================================
// Partial curve projection tests
//==================================================================================================

// Test: Project only first quarter of full circle
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, PartialCircle_I1)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  // Project only 0 to PI/2 (first quadrant)
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, M_PI / 2.0, aPCurve);

  ASSERT_TRUE(aResult) << "Partial projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull());

  // Verify endpoints: (5,0) to (0,5)
  gp_Pnt2d aStart = aPCurve->Value(0.0);
  gp_Pnt2d aEnd   = aPCurve->Value(M_PI / 2.0);
  EXPECT_NEAR(aStart.X(), 5.0, 0.01);
  EXPECT_NEAR(aStart.Y(), 0.0, 0.01);
  EXPECT_NEAR(aEnd.X(), 0.0, 0.01);
  EXPECT_NEAR(aEnd.Y(), 5.0, 0.01);
}

// Test: Project middle portion of B-spline
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, MiddlePortionBSpline_I2)
{
  Handle(Geom_Plane)        aPlane   = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(Geom_BSplineCurve) aBSpline = createPlanarBSpline();
  ASSERT_FALSE(aBSpline.IsNull());

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  const double aFirst = aBSpline->FirstParameter();
  const double aLast  = aBSpline->LastParameter();
  const double aRange = aLast - aFirst;

  // Project middle 50%
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aFirst + 0.25 * aRange, aFirst + 0.75 * aRange, aPCurve);

  ASSERT_TRUE(aResult) << "Middle portion projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull());
}

//==================================================================================================
// Multiple projection and reinitialization tests
//==================================================================================================

// Test: Multiple projections on same surface reuse setup correctly
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, MultipleProjections_J1)
{
  Handle(Geom_Plane)            aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  Handle(ShapeAnalysis_Surface) aSAS   = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  // Project 5 different curves
  for (int i = 0; i < 5; ++i)
  {
    const double              aOffset = i * 10.0;
    Handle(Geom_TrimmedCurve) aLine =
      GC_MakeSegment(gp_Pnt(aOffset, 0, 0), gp_Pnt(aOffset + 5, 5, 0)).Value();

    Handle(Geom2d_Curve)   aPCurve;
    const Standard_Boolean aResult =
      myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

    EXPECT_TRUE(aResult) << "Projection " << i << " should succeed";
    EXPECT_FALSE(aPCurve.IsNull()) << "PCurve " << i << " should not be null";
  }
}

// Test: Surface reinitialization between projections
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, SurfaceReinitialization_J2)
{
  // First surface
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  myProjector->Init(new ShapeAnalysis_Surface(aPlane1), myTolerance);

  Handle(Geom_TrimmedCurve) aLine1 = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();
  Handle(Geom2d_Curve)      aPCurve1;
  myProjector->Perform(aLine1, aLine1->FirstParameter(), aLine1->LastParameter(), aPCurve1);
  ASSERT_FALSE(aPCurve1.IsNull());

  // Second surface (different plane)
  Handle(Geom_Plane) aPlane2 = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)));
  myProjector->Init(new ShapeAnalysis_Surface(aPlane2), myTolerance);

  Handle(Geom_TrimmedCurve) aLine2 = GC_MakeSegment(gp_Pnt(0, 0, 5), gp_Pnt(10, 10, 5)).Value();
  Handle(Geom2d_Curve)      aPCurve2;
  myProjector->Perform(aLine2, aLine2->FirstParameter(), aLine2->LastParameter(), aPCurve2);
  ASSERT_FALSE(aPCurve2.IsNull());
}

//==================================================================================================
// Near-singularity tests (poles, degenerate regions)
//==================================================================================================

// Test: Project small circle near north pole of sphere
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, NearPoleOnSphere_K1)
{
  const double                  aRadius = 10.0;
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Small circle very close to north pole (latitude = 89.4 degrees)
  const double aLatitude   = M_PI / 2.0 - 0.01;
  const double aZ          = aRadius * sin(aLatitude);
  const double aCircRadius = aRadius * cos(aLatitude);

  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), aCircRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  myProjector->Init(new ShapeAnalysis_Surface(aSphere), myTolerance);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  // Should succeed even near pole
  ASSERT_TRUE(aResult) << "Projection near pole should succeed";
  ASSERT_FALSE(aPCurve.IsNull());
}

//==================================================================================================
// Edge case tests
//==================================================================================================

// Test: Very short curve projection should not crash
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, VeryShortCurve_L1)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Very short line segment (1e-6 length)
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(1e-6, 0, 0)).Value();

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1e-4);

  Handle(Geom2d_Curve) aPCurve;
  // Result may vary but should not crash
  (void)myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  SUCCEED() << "Short curve projection completed without crash";
}

// Test: Curve far from surface still projects
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, CurveFarFromSurface_L2)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Line at Z=100, far from plane at Z=0
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 100), gp_Pnt(10, 10, 100)).Value();

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  Handle(Geom2d_Curve) aPCurve;
  (void)myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  // Projection should still produce result (it projects onto the surface)
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should be created even for distant curve";
}

// Test: Large tolerance projection
TEST_F(ShapeConstruct_ProjectCurveOnSurfaceTest, LargeTolerance_L3)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Line slightly off plane
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0.1), gp_Pnt(10, 10, 0.1)).Value();

  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1.0);

  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aLine, aLine->FirstParameter(), aLine->LastParameter(), aPCurve);

  ASSERT_TRUE(aResult) << "Large tolerance projection should succeed";
  ASSERT_FALSE(aPCurve.IsNull());
}
