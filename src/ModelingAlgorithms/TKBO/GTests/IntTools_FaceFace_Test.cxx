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

#include <vector>

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_FaceFace.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp_Explorer.hxx>
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
    gp_Ax3                          anAx3(theCenter, theAxis);
    Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(anAx3, theRadius);

    // Create face with parameter bounds
    BRepBuilderAPI_MakeFace aFaceMaker(aCylSurf, theUMin, theUMax, theVMin, theVMax, 1e-7);
    return aFaceMaker.Face();
  }

  //! Create a full cylinder face with finite V range.
  //! @param theCenter center point on the cylinder axis
  //! @param theAxis direction of the cylinder axis
  //! @param theRadius radius of the cylinder
  //! @param theVMin minimum V parameter (height along axis)
  //! @param theVMax maximum V parameter (height along axis)
  //! @return the cylinder face
  static TopoDS_Face CreateCylinderFace(const gp_Pnt& theCenter,
                                        const gp_Dir& theAxis,
                                        double        theRadius,
                                        double        theVMin,
                                        double        theVMax)
  {
    return CreateHalfCylinderFace(theCenter, theAxis, theRadius, theVMin, theVMax, 0.0, 2.0 * M_PI);
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
    gp_Pln             aPln(theCenter, theNormal);
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

//! Regression: cylinder-cylinder analytical gating must not depend on argument order.
//! Geometry is configured so that only one cylinder contributes a touching V-boundary.
TEST_F(IntTools_FaceFaceTest, PerpendicularCylinderBoundaryTouch_OrderIndependent)
{
  // Cylinder 1: axis along Z, long finite segment.
  const gp_Pnt aCyl1Center(0.0, 0.0, 0.0);
  const gp_Dir aCyl1Axis(0.0, 0.0, 1.0);
  const double aCyl1Radius = 2.0;
  const double aCyl1VMin   = -5.0;
  const double aCyl1VMax   = 5.0;

  // Cylinder 2: axis along X, one V-boundary at X=0 touches cylinder 1.
  // Endpoint (0, 2, 0) lies on cylinder 1 surface, while cylinder 1 V-boundaries
  // stay far from cylinder 2 surface.
  const gp_Pnt aCyl2Center(0.0, 2.0, 0.0);
  const gp_Dir aCyl2Axis(1.0, 0.0, 0.0);
  const double aCyl2Radius = 0.5;
  const double aCyl2VMin   = 0.0;
  const double aCyl2VMax   = 4.0;

  TopoDS_Face aFace1 =
    CreateCylinderFace(aCyl1Center, aCyl1Axis, aCyl1Radius, aCyl1VMin, aCyl1VMax);
  TopoDS_Face aFace2 =
    CreateCylinderFace(aCyl2Center, aCyl2Axis, aCyl2Radius, aCyl2VMin, aCyl2VMax);

  ASSERT_FALSE(aFace1.IsNull()) << "Failed to create first cylinder face";
  ASSERT_FALSE(aFace2.IsNull()) << "Failed to create second cylinder face";

  IntTools_FaceFace aFF12;
  aFF12.SetParameters(true, true, true, 1.0e-7);
  aFF12.Perform(aFace1, aFace2);
  ASSERT_TRUE(aFF12.IsDone()) << "Intersection failed for (face1, face2)";

  IntTools_FaceFace aFF21;
  aFF21.SetParameters(true, true, true, 1.0e-7);
  aFF21.Perform(aFace2, aFace1);
  ASSERT_TRUE(aFF21.IsDone()) << "Intersection failed for (face2, face1)";

  const int aNbCurves12 = aFF12.Lines().Length();
  const int aNbPoints12 = aFF12.Points().Length();
  const int aNbCurves21 = aFF21.Lines().Length();
  const int aNbPoints21 = aFF21.Points().Length();

  EXPECT_EQ(aNbCurves12, aNbCurves21)
    << "Intersection curve count should not depend on argument order";
  EXPECT_EQ(aNbPoints12, aNbPoints21)
    << "Intersection point count should not depend on argument order";
}

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

//! Test geometry where the opposite half of the cylinder IS inside the circle.
//! Position the cylinder closer so that the half bulging toward the plane is inside the circle.
TEST_F(IntTools_FaceFaceTest, OppositeHalfInsideCircle_HasIntersection)
{
  // Plane circle centered at origin with radius 15
  const gp_Pnt aPlaneCenter(0.0, 0.0, 0.0);
  const gp_Dir aPlaneNormal(0.0, 1.0, 0.0);
  const double aPlaneRadius = 15.0;

  // Cylinder at X=10, Z=-5
  // The half with U in [PI, 2*PI] bulges toward +Z (toward origin)
  // At U=3*PI/2, the point is at (10, Y, -5 + 3) = (10, Y, -2)
  // Distance from (10, -2) to origin = sqrt(100 + 4) ~ 10.2 < 15 (inside!)
  const gp_Pnt aCylCenter(10.0, 0.0, -5.0);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 3.0;
  const double aVMin      = -1.0;
  const double aVMax      = 1.0;

  // Create the opposite half: U in [PI, 2*PI] (bulges toward +Z, toward plane center)
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax, M_PI, 2.0 * M_PI);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  // Perform face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-7);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  // This intersection SHOULD exist - the semicircle is partially inside the plane circle
  const int aNbCurves = aFF.Lines().Length();

  EXPECT_GE(aNbCurves, 1)
    << "Expected intersection for opposite half that bulges toward plane center.";
}

//! Test geometry where the cylinder arc partially crosses the circle boundary.
//! With wire-aware BRepTopAdaptor_TopolTool, the intersection curve is properly
//! trimmed to the portion that lies within both face wire boundaries.
TEST_F(IntTools_FaceFaceTest, PartialCrossing_ProperlyTrimmed)
{
  const gp_Pnt aPlaneCenter(0.0, 0.0, 0.0);
  const gp_Dir aPlaneNormal(0.0, 1.0, 0.0);
  const double aPlaneRadius = 15.0;

  // Position cylinder so the arc partially crosses the boundary.
  // First half: U in [0, PI], bulges toward -Z
  // At U=PI/2: (12, Y, -11), distance = sqrt(144 + 121) ~ 16.3 > 15 (outside!)
  // At U=0: (15, Y, -8), distance = sqrt(225 + 64) ~ 17.0 > 15 (outside!)
  // At U=PI: (9, Y, -8), distance = sqrt(81 + 64) ~ 12.0 < 15 (inside!)
  // The arc crosses from outside to inside the circle.
  const gp_Pnt aCylCenter(12.0, 0.0, -8.0);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 3.0;
  const double aVMin      = -1.0;
  const double aVMax      = 1.0;

  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax, 0.0, M_PI);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-7);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  const int aNbCurves = aFF.Lines().Length();

  // With wire-aware BRepTopAdaptor_TopolTool, partial intersections are properly
  // trimmed to the valid region within both face boundaries, not rejected entirely.
  // The portion of the cylinder that's inside the circular plane produces a valid
  // intersection curve segment.
  EXPECT_GE(aNbCurves, 1)
    << "Wire-aware classification should produce trimmed intersection curve for partial overlap";
}

//! Test with cylinder positioned such that both halves are completely outside the circle.
//! This verifies the algorithm correctly handles cases where no part of the cylinder
//! is inside the circular boundary.
TEST_F(IntTools_FaceFaceTest, BothHalvesCompletelyOutside_NoIntersection)
{
  // Plane circle with small radius
  const gp_Pnt aPlaneCenter(0.0, 0.0, 0.0);
  const gp_Dir aPlaneNormal(0.0, 1.0, 0.0);
  const double aPlaneRadius = 5.0;

  // Cylinder far from the plane center
  // Both halves will be completely outside the circle
  const gp_Pnt aCylCenter(20.0, 0.0, 20.0);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 2.0;
  const double aVMin      = -1.0;
  const double aVMax      = 1.0;

  // Test first half
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder1 =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax, 0.0, M_PI);

  ASSERT_FALSE(aCircularPlane.IsNull());
  ASSERT_FALSE(aHalfCylinder1.IsNull());

  IntTools_FaceFace aFF1;
  aFF1.SetParameters(true, true, true, 1.0e-7);
  aFF1.Perform(aCircularPlane, aHalfCylinder1);

  ASSERT_TRUE(aFF1.IsDone());
  EXPECT_EQ(aFF1.Lines().Length(), 0) << "First half should have no intersection";
  EXPECT_EQ(aFF1.Points().Length(), 0);

  // Test opposite half
  TopoDS_Face aHalfCylinder2 =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax, M_PI, 2.0 * M_PI);

  ASSERT_FALSE(aHalfCylinder2.IsNull());

  IntTools_FaceFace aFF2;
  aFF2.SetParameters(true, true, true, 1.0e-7);
  aFF2.Perform(aCircularPlane, aHalfCylinder2);

  ASSERT_TRUE(aFF2.IsDone());
  EXPECT_EQ(aFF2.Lines().Length(), 0) << "Opposite half should also have no intersection";
  EXPECT_EQ(aFF2.Points().Length(), 0);
}

// Test OCC24005: IntTools_FaceFace should complete quickly and correctly when
// intersecting a slightly off-angle plane with a cylinder. Previously took 7+ seconds.
TEST_F(IntTools_FaceFaceTest, OCC24005_PlaneCylinderIntersection)
{
  // Hardcoded geometry from the original regression test
  Handle(Geom_Plane) aPlane = new Geom_Plane(
    gp_Ax3(gp_Pnt(-72.948737453424499, 754.30437716359393, 259.52151854671678),
           gp_Dir(6.2471473085930200e-007, -0.99999999999980493, 0.00000000000000000),
           gp_Dir(0.99999999999980493, 6.2471473085930200e-007, 0.00000000000000000)));

  Handle(Geom_CylindricalSurface) aCylinder = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(-6.4812490053250649, 753.39408794522092, 279.16400974257465),
           gp_Dir(1.0000000000000000, 0.0, 0.00000000000000000),
           gp_Dir(0.0, 1.0000000000000000, 0.00000000000000000)),
    19.712534607908712);

  BRep_Builder aBuilder;
  TopoDS_Face  aFace1, aFace2;
  aBuilder.MakeFace(aFace1, aPlane, Precision::Confusion());
  aBuilder.MakeFace(aFace2, aCylinder, Precision::Confusion());

  IntTools_FaceFace anInters;
  anInters.SetParameters(false, true, true, Precision::Confusion());
  anInters.Perform(aFace1, aFace2);

  ASSERT_TRUE(anInters.IsDone()) << "IntTools_FaceFace::Perform did not complete";

  // The original test verified that intersection completes without hanging.
  // Check that we get at least one intersection curve.
  const NCollection_Sequence<IntTools_Curve>& aCurves = anInters.Lines();
  EXPECT_GE(aCurves.Length() + anInters.Points().Length(), 1)
    << "Expected at least one intersection result (curve or point)";
}
