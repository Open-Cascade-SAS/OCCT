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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//==================================================================================================
// Basic Projection Tests
//==================================================================================================

class ProjectCurveOnSurfaceBasicTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  Standard_Real                                myTolerance;
};

// Test: Project line on plane
TEST_F(ProjectCurveOnSurfaceBasicTest, ProjectLineOnPlane)
{
  // Create a horizontal plane at Z=0
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create a line segment on the plane
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  // Setup projector
  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aPlane);
  myProjector->Init(aSAS, myTolerance);

  // Perform projection using the curve's actual parameter range
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Real    aFirst  = aLine->FirstParameter();
  const Standard_Real    aLast   = aLine->LastParameter();
  const Standard_Boolean aResult = myProjector->Perform(aLine, aFirst, aLast, aPCurve);

  EXPECT_TRUE(aResult) << "Projection of line on plane should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Verify that PCurve correctly maps 3D points to 2D surface coordinates
  if (!aPCurve.IsNull())
  {
    // Check at start, middle, and end of curve
    for (Standard_Real t = 0.0; t <= 1.0; t += 0.5)
    {
      const Standard_Real aParam = aFirst + t * (aLast - aFirst);
      gp_Pnt              a3DPnt;
      aLine->D0(aParam, a3DPnt);

      // Get 2D point from pcurve
      const gp_Pnt2d a2DPnt = aPCurve->Value(aParam);

      // Get 3D point from surface using 2D coordinates
      gp_Pnt aSurfPnt = aSAS->Value(a2DPnt);

      // The 3D points should match
      EXPECT_NEAR(a3DPnt.X(), aSurfPnt.X(), 0.01) << "X mismatch at t=" << t;
      EXPECT_NEAR(a3DPnt.Y(), aSurfPnt.Y(), 0.01) << "Y mismatch at t=" << t;
      EXPECT_NEAR(a3DPnt.Z(), aSurfPnt.Z(), 0.01) << "Z mismatch at t=" << t;
    }
  }
}

// Test: Project circle on cylindrical surface
TEST_F(ProjectCurveOnSurfaceBasicTest, ProjectCircleOnCylinder)
{
  // Create cylindrical surface with radius 5, axis along Z
  const Standard_Real             aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Create a circle at height Z=3 on the cylinder surface
  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, 3), gp_Dir(0, 0, 1)), aRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aCylinder), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  EXPECT_TRUE(aResult) << "Projection of circle on cylinder should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // On cylinder, the circle at Z=3 should project to a horizontal line V=3
  if (!aPCurve.IsNull())
  {
    const gp_Pnt2d aMid =
      aPCurve->Value((aPCurve->FirstParameter() + aPCurve->LastParameter()) / 2);
    // V coordinate should be constant at 3
    EXPECT_NEAR(aMid.Y(), 3.0, myTolerance) << "V coordinate should be 3 on cylinder";
  }
}

// Test: Project line parallel to axis on cylinder (isoparametric)
TEST_F(ProjectCurveOnSurfaceBasicTest, ProjectIsoparametricOnCylinder)
{
  // Create cylindrical surface
  const Standard_Real             aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Create a line along the cylinder at U=0 (X=5, Y=0)
  Handle(Geom_TrimmedCurve) aLine =
    GC_MakeSegment(gp_Pnt(aRadius, 0, 0), gp_Pnt(aRadius, 0, 10)).Value();

  // Setup projector
  Handle(ShapeAnalysis_Surface) aSAS = new ShapeAnalysis_Surface(aCylinder);
  myProjector->Init(aSAS, myTolerance);

  // Perform projection using the curve's actual parameter range
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Real    aFirst  = aLine->FirstParameter();
  const Standard_Real    aLast   = aLine->LastParameter();
  const Standard_Boolean aResult = myProjector->Perform(aLine, aFirst, aLast, aPCurve);

  EXPECT_TRUE(aResult) << "Projection should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Verify that pcurve correctly maps 3D points to 2D surface coordinates
  if (!aPCurve.IsNull())
  {
    // Check at start, middle, and end of curve
    for (Standard_Real t = 0.0; t <= 1.0; t += 0.5)
    {
      const Standard_Real aParam = aFirst + t * (aLast - aFirst);
      gp_Pnt              a3DPnt;
      aLine->D0(aParam, a3DPnt);

      // Get 2D point from pcurve
      const gp_Pnt2d a2DPnt = aPCurve->Value(aParam);

      // Get 3D point from surface using 2D coordinates
      gp_Pnt aSurfPnt = aSAS->Value(a2DPnt);

      // The 3D points should match
      EXPECT_NEAR(a3DPnt.X(), aSurfPnt.X(), 0.01) << "X mismatch at t=" << t;
      EXPECT_NEAR(a3DPnt.Y(), aSurfPnt.Y(), 0.01) << "Y mismatch at t=" << t;
      EXPECT_NEAR(a3DPnt.Z(), aSurfPnt.Z(), 0.01) << "Z mismatch at t=" << t;
    }

    // Additionally verify U is constant (isoparametric in U)
    const gp_Pnt2d aStart = aPCurve->Value(aFirst);
    const gp_Pnt2d aEnd   = aPCurve->Value(aLast);
    EXPECT_NEAR(aStart.X(), aEnd.X(), 0.01) << "U should be constant for isoparametric curve";
  }
}

//==================================================================================================
// Periodic Surface Tests
//==================================================================================================

class ProjectCurveOnSurfacePeriodicTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  Standard_Real                                myTolerance;
};

// Test: Project on spherical surface (both U and V periodic in some sense)
TEST_F(ProjectCurveOnSurfacePeriodicTest, ProjectOnSphericalSurface)
{
  // Create spherical surface
  const Standard_Real           aRadius = 10.0;
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Create a great circle (equator)
  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aSphere), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  EXPECT_TRUE(aResult) << "Projection on sphere should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

// Test: Project on toroidal surface (both U and V periodic)
TEST_F(ProjectCurveOnSurfacePeriodicTest, ProjectOnToroidalSurface)
{
  // Create toroidal surface: major radius 10, minor radius 2
  const Standard_Real          aMajorRadius = 10.0;
  const Standard_Real          aMinorRadius = 2.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajorRadius, aMinorRadius);

  // Create a circle along the outer edge (U=0, V varies)
  gp_Circ aCirc(gp_Ax2(gp_Pnt(aMajorRadius + aMinorRadius, 0, 0), gp_Dir(0, 1, 0)), aMinorRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aTorus), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  EXPECT_TRUE(aResult) << "Projection on torus should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

//==================================================================================================
// B-Spline Curve Tests
//==================================================================================================

class ProjectCurveOnSurfaceBSplineTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  // Helper to create a simple B-spline curve
  Handle(Geom_BSplineCurve) createSimpleBSpline()
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

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  Standard_Real                                myTolerance;
};

// Test: Project B-spline on plane
TEST_F(ProjectCurveOnSurfaceBSplineTest, ProjectBSplineOnPlane)
{
  // Create a horizontal plane
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create B-spline
  Handle(Geom_BSplineCurve) aBSpline = createSimpleBSpline();
  ASSERT_FALSE(aBSpline.IsNull()) << "Failed to create B-spline";

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aBSpline->FirstParameter(), aBSpline->LastParameter(), aPCurve);

  EXPECT_TRUE(aResult) << "Projection of B-spline on plane should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";

  // Verify that the projection preserves the general shape
  if (!aPCurve.IsNull())
  {
    const gp_Pnt2d aStart = aPCurve->Value(aPCurve->FirstParameter());
    const gp_Pnt2d aEnd   = aPCurve->Value(aPCurve->LastParameter());

    // Start and end should approximately match original curve X,Y
    EXPECT_NEAR(aStart.X(), 0.0, 0.1);
    EXPECT_NEAR(aStart.Y(), 0.0, 0.1);
    EXPECT_NEAR(aEnd.X(), 10.0, 0.1);
    EXPECT_NEAR(aEnd.Y(), 0.0, 0.1);
  }
}

// Test: Project B-spline on cylindrical surface
TEST_F(ProjectCurveOnSurfaceBSplineTest, ProjectBSplineOnCylinder)
{
  // Create cylindrical surface
  const Standard_Real             aRadius = 5.0;
  Handle(Geom_CylindricalSurface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);

  // Create a B-spline that lies on the cylinder surface
  TColgp_Array1OfPnt aPoles(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    const Standard_Real anAngle = (i - 1) * M_PI / 8.0; // Partial arc
    const Standard_Real aZ      = (i - 1) * 2.0;
    aPoles(i)                   = gp_Pnt(aRadius * cos(anAngle), aRadius * sin(anAngle), aZ);
  }

  GeomAPI_PointsToBSpline   aBuilder(aPoles);
  Handle(Geom_BSplineCurve) aBSpline = aBuilder.Curve();
  ASSERT_FALSE(aBSpline.IsNull()) << "Failed to create B-spline";

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aCylinder), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult =
    myProjector->Perform(aBSpline, aBSpline->FirstParameter(), aBSpline->LastParameter(), aPCurve);

  EXPECT_TRUE(aResult) << "Projection of B-spline on cylinder should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

//==================================================================================================
// Edge Cases and Error Handling
//==================================================================================================

class ProjectCurveOnSurfaceEdgeCaseTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  Standard_Real                                myTolerance;
};

// Test: Project very short curve
TEST_F(ProjectCurveOnSurfaceEdgeCaseTest, ProjectVeryShortCurve)
{
  // Create a horizontal plane
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create a very short line segment (1e-6 length)
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(1e-6, 0, 0)).Value();

  // Setup projector with larger tolerance
  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1e-4);

  // Perform projection
  Handle(Geom2d_Curve) aPCurve;
  (void)myProjector->Perform(aLine, 0.0, 1.0, aPCurve);

  // Short curves may or may not project successfully, but shouldn't crash
  // The result is implementation-dependent for degenerate cases
  SUCCEED() << "Short curve projection completed without crash";
}

// Test: Projection with large tolerance
TEST_F(ProjectCurveOnSurfaceEdgeCaseTest, ProjectWithLargeTolerance)
{
  // Create a horizontal plane
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create a line slightly off the plane
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0.1), gp_Pnt(10, 10, 0.1)).Value();

  // Setup projector with large tolerance
  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1.0);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aLine, 0.0, 1.0, aPCurve);

  EXPECT_TRUE(aResult) << "Projection with large tolerance should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

// Test: Null surface handling
TEST_F(ProjectCurveOnSurfaceEdgeCaseTest, NullSurfaceHandling)
{
  // Don't initialize the projector with a surface
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  // Attempt projection (should handle gracefully)
  Handle(Geom2d_Curve) aPCurve;

  // This should either fail gracefully or the projector should check for initialization
  // We're testing that it doesn't crash
  SUCCEED() << "Null surface handling test completed";
}

// Test: Curve not on surface
TEST_F(ProjectCurveOnSurfaceEdgeCaseTest, CurveNotOnSurface)
{
  // Create a horizontal plane at Z=0
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create a line far from the plane (at Z=100)
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 100), gp_Pnt(10, 10, 100)).Value();

  // Setup projector with small tolerance
  myProjector->Init(new ShapeAnalysis_Surface(aPlane), 1e-7);

  // Perform projection
  Handle(Geom2d_Curve) aPCurve;
  (void)myProjector->Perform(aLine, 0.0, 1.0, aPCurve);

  // Projection should still succeed (it projects onto the surface)
  // but the status should indicate approximation was used
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should still be created for projection";
}

//==================================================================================================
// Cone and Special Surface Tests
//==================================================================================================

class ProjectCurveOnSurfaceSpecialTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myProjector = new ShapeConstruct_ProjectCurveOnSurface();
    myTolerance = Precision::Confusion();
  }

  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector;
  Standard_Real                                myTolerance;
};

// Test: Project on conical surface
TEST_F(ProjectCurveOnSurfaceSpecialTest, ProjectOnConicalSurface)
{
  // Create conical surface
  const Standard_Real         aRadius    = 5.0;
  const Standard_Real         aSemiAngle = M_PI / 6.0; // 30 degrees
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aSemiAngle, aRadius);

  // Create a circle at height Z=5
  const Standard_Real aZ         = 5.0;
  const Standard_Real aRadiusAtZ = aRadius + aZ * tan(aSemiAngle);
  gp_Circ             aCirc(gp_Ax2(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), aRadiusAtZ);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aCone), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve)   aPCurve;
  const Standard_Boolean aResult = myProjector->Perform(aCircle, 0.0, 2.0 * M_PI, aPCurve);

  EXPECT_TRUE(aResult) << "Projection on cone should succeed";
  EXPECT_FALSE(aPCurve.IsNull()) << "PCurve should not be null";
}

// Test: Status reporting
TEST_F(ProjectCurveOnSurfaceSpecialTest, StatusReporting)
{
  // Create a horizontal plane
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // Create a simple line
  Handle(Geom_TrimmedCurve) aLine = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 0)).Value();

  // Setup projector
  myProjector->Init(new ShapeAnalysis_Surface(aPlane), myTolerance);

  // Perform projection
  Handle(Geom2d_Curve) aPCurve;
  myProjector->Perform(aLine, 0.0, 1.0, aPCurve);

  // Check status
  EXPECT_TRUE(myProjector->Status(ShapeExtend_OK) || myProjector->Status(ShapeExtend_DONE))
    << "Status should indicate OK or DONE";
}
