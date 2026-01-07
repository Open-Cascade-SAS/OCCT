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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <IntTools_FaceFace.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
// IntTools_FaceFace Tests - Tests for face-face intersection
//==================================================================================================

class IntTools_FaceFaceTest : public ::testing::Test
{
protected:
  //! Create a half-cylinder face.
  //! @param theCenter center point on the cylinder axis
  //! @param theAxis direction of the cylinder axis
  //! @param theRadius radius of the cylinder
  //! @param theVMin minimum V parameter (height along axis)
  //! @param theVMax maximum V parameter (height along axis)
  //! @param theUMin minimum U parameter (angle, default 0)
  //! @param theUMax maximum U parameter (angle, default PI for half-cylinder)
  //! @return the half-cylinder face
  static TopoDS_Face CreateHalfCylinderFace(const gp_Pnt& theCenter,
                                            const gp_Dir& theAxis,
                                            double        theRadius,
                                            double        theVMin,
                                            double        theVMax,
                                            double        theUMin = 0.0,
                                            double        theUMax = M_PI)
  {
    // Create cylinder surface
    gp_Ax3                              anAx3(theCenter, theAxis);
    Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(anAx3, theRadius);

    // Create face with parameter bounds
    BRepBuilderAPI_MakeFace aFaceMaker(aCylSurf, theUMin, theUMax, theVMin, theVMax, 1e-7);
    return aFaceMaker.Face();
  }

  //! Create a circular planar face.
  //! @param theCenter center of the circle on the plane
  //! @param theNormal normal direction of the plane
  //! @param theRadius radius of the circular boundary
  //! @return the circular planar face
  static TopoDS_Face CreateCircularPlaneFace(const gp_Pnt& theCenter,
                                             const gp_Dir& theNormal,
                                             double        theRadius)
  {
    // Create plane
    gp_Pln                 aPln(theCenter, theNormal);
    Handle(Geom_Plane) aPlane = new Geom_Plane(aPln);

    // Create circular edge on the plane
    gp_Ax2  aCircAx(theCenter, theNormal);
    gp_Circ aCirc(aCircAx, theRadius);

    BRepBuilderAPI_MakeEdge anEdgeMaker(aCirc);
    TopoDS_Edge             aCircEdge = anEdgeMaker.Edge();

    // Create wire from circular edge
    BRepBuilderAPI_MakeWire aWireMaker(aCircEdge);
    TopoDS_Wire             aWire = aWireMaker.Wire();

    // Create face from plane with circular wire
    BRepBuilderAPI_MakeFace aFaceMaker(aPlane, aWire, true);
    return aFaceMaker.Face();
  }
};

//! Test that a half-cylinder face and a circular plane face that don't actually
//! intersect (cylinder is outside the circle) correctly return no intersection.
//! This tests the fix for wire-based boundary validation in IntTools_FaceFace.
TEST_F(IntTools_FaceFaceTest, HalfCylinderOutsideCircularPlane_NoIntersection)
{
  // Create geometry similar to the reported bug:
  // - Half cylinder at position where it would intersect the infinite plane
  // - But the circular boundary of the plane doesn't reach the cylinder

  // Plane at Y = 0, with circular boundary of radius 10, centered at origin
  const gp_Pnt aPlaneCenter(0.0, 0.0, 0.0);
  const gp_Dir aPlaneNormal(0.0, 1.0, 0.0);
  const double aPlaneRadius = 10.0;

  // Half cylinder with axis along Y, positioned far from plane center
  // The cylinder is at X=20, which is outside the plane's circular boundary (radius 10)
  const gp_Pnt aCylCenter(20.0, 0.0, 0.0);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 2.0;
  const double aVMin      = -1.0;
  const double aVMax      = 1.0;

  // Create faces
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  // Perform face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-7);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  // The intersection should be empty because the cylinder is outside the circular boundary
  const int aNbCurves = aFF.Lines().Length();
  const int aNbPoints = aFF.Points().Length();

  EXPECT_EQ(aNbCurves, 0) << "Expected no intersection curves, but found " << aNbCurves
                          << ". The cylinder at X=20 is outside the circular plane (radius 10).";
  EXPECT_EQ(aNbPoints, 0) << "Expected no intersection points, but found " << aNbPoints;
}

//! Test that a half-cylinder face and a circular plane face that do intersect
//! correctly return intersection curves.
TEST_F(IntTools_FaceFaceTest, HalfCylinderInsideCircularPlane_HasIntersection)
{
  // Create geometry where intersection should exist:
  // - Half cylinder positioned inside the circular plane boundary

  // Plane at Y = 0, with circular boundary of radius 20, centered at origin
  const gp_Pnt aPlaneCenter(0.0, 0.0, 0.0);
  const gp_Dir aPlaneNormal(0.0, 1.0, 0.0);
  const double aPlaneRadius = 20.0;

  // Half cylinder with axis along Y, positioned at origin (inside plane circle)
  const gp_Pnt aCylCenter(0.0, 0.0, 0.0);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 5.0;
  const double aVMin      = -1.0;
  const double aVMax      = 1.0;

  // Create faces
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  // Perform face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-7);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  // The intersection should exist - a semicircular arc
  const int aNbCurves = aFF.Lines().Length();

  EXPECT_GE(aNbCurves, 1)
    << "Expected at least one intersection curve (semicircular arc), but found " << aNbCurves;
}

//! Test with the exact geometry from the reported bug.
//! Half-cylinder and circular plane where cylinder is outside circle boundary.
TEST_F(IntTools_FaceFaceTest, ReportedBugGeometry_NoIntersection)
{
  // Exact geometry from the bug report:
  // Cylinder: center (-14.55, -24, -36.34), radius 2.2225, axis Y, V range [-0.5, 0]
  // Plane: center (-23.45, -24, -18.81), normal Y, circular radius 13.335

  // Distance from plane center to cylinder axis in XZ plane:
  // sqrt(((-14.55) - (-23.45))^2 + ((-36.34) - (-18.81))^2) = sqrt(8.9^2 + 17.53^2) ≈ 19.66
  // This is greater than the plane circle radius (13.335), so no intersection should occur.

  const gp_Pnt aPlaneCenter(-23.45, -24.0, -18.81);
  const gp_Dir aPlaneNormal(0.0, -1.0, 0.0);
  const double aPlaneRadius = 13.335;

  const gp_Pnt aCylCenter(-14.55, -24.0, -36.34);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 2.2225;
  const double aVMin      = -0.5;
  const double aVMax      = 0.0;

  // Create faces
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  // Perform face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-6);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  // The intersection should be empty because the cylinder is outside the circular boundary
  const int aNbCurves = aFF.Lines().Length();
  const int aNbPoints = aFF.Points().Length();

  EXPECT_EQ(aNbCurves, 0)
    << "Expected no intersection curves. "
    << "The cylinder center is ~19.66 units from plane center, but plane radius is only 13.335.";
  EXPECT_EQ(aNbPoints, 0) << "Expected no intersection points.";
}
