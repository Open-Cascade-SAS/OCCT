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
  // sqrt(((-14.55) - (-23.45))^2 + ((-36.34) - (-18.81))^2) = sqrt(8.9^2 + 17.53^2) ~ 19.66
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

//! Test the opposite half of the cylinder (U in [PI, 2*PI]) with the same geometry.
//! The opposite half bulges toward the plane center (+Z direction) but is still outside.
//! For cylinder axis along Y with default XDirection=(1,0,0):
//! - U in [0, PI] bulges in -Z direction (away from plane)
//! - U in [PI, 2*PI] bulges in +Z direction (toward plane)
//! Even the closest point of the opposite half (~17.7 units) is still outside the circle
//! (radius 13.335).
TEST_F(IntTools_FaceFaceTest, ReportedBugGeometry_OppositeHalf_NoIntersection)
{
  const gp_Pnt aPlaneCenter(-23.45, -24.0, -18.81);
  const gp_Dir aPlaneNormal(0.0, -1.0, 0.0);
  const double aPlaneRadius = 13.335;

  const gp_Pnt aCylCenter(-14.55, -24.0, -36.34);
  const gp_Dir aCylAxis(0.0, 1.0, 0.0);
  const double aCylRadius = 2.2225;
  const double aVMin      = -0.5;
  const double aVMax      = 0.0;

  // Create the OPPOSITE half of the cylinder: U in [PI, 2*PI]
  // This half bulges toward the plane center (+Z direction)
  TopoDS_Face aCircularPlane = CreateCircularPlaneFace(aPlaneCenter, aPlaneNormal, aPlaneRadius);
  TopoDS_Face aHalfCylinder =
    CreateHalfCylinderFace(aCylCenter, aCylAxis, aCylRadius, aVMin, aVMax, M_PI, 2.0 * M_PI);

  ASSERT_FALSE(aCircularPlane.IsNull()) << "Failed to create circular plane face";
  ASSERT_FALSE(aHalfCylinder.IsNull()) << "Failed to create half-cylinder face";

  // Perform face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-6);
  aFF.Perform(aCircularPlane, aHalfCylinder);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  // Even the opposite half should have no intersection
  // The closest point at U=3*PI/2 is at (-14.55, -24, -34.12)
  // Distance to plane center: sqrt(8.9^2 + 15.31^2) ~ 17.7 > 13.335
  const int aNbCurves = aFF.Lines().Length();
  const int aNbPoints = aFF.Points().Length();

  EXPECT_EQ(aNbCurves, 0)
    << "Expected no intersection curves for opposite half. "
    << "Even the closest point (~17.7 units) is outside the circle (radius 13.335).";
  EXPECT_EQ(aNbPoints, 0) << "Expected no intersection points for opposite half.";
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

//! Test face-face intersection using BREP files from disk.
//! This test reads the actual geometry that triggered the original bug.
TEST_F(IntTools_FaceFaceTest, BrepFilesIntersection)
{
  // Path to the BREP files - adjust as needed
  const char* aFace1Path = "/Users/dpasukhi/work/OCCT/FF_0_Face1.brep";
  const char* aFace2Path = "/Users/dpasukhi/work/OCCT/FF_0_Face2.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape aShape1, aShape2;

  if (!BRepTools::Read(aShape1, aFace1Path, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << aFace1Path;
  }
  if (!BRepTools::Read(aShape2, aFace2Path, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << aFace2Path;
  }

  // Extract faces from shapes
  TopoDS_Face aFace1, aFace2;
  for (TopExp_Explorer anExp(aShape1, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFace1 = TopoDS::Face(anExp.Current());
    break;
  }
  for (TopExp_Explorer anExp(aShape2, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFace2 = TopoDS::Face(anExp.Current());
    break;
  }

  ASSERT_FALSE(aFace1.IsNull()) << "No face found in " << aFace1Path;
  ASSERT_FALSE(aFace2.IsNull()) << "No face found in " << aFace2Path;

  // Test face-face intersection
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, 1.0e-7);
  aFF.Perform(aFace1, aFace2);

  ASSERT_TRUE(aFF.IsDone()) << "IntTools_FaceFace::Perform() failed";

  const int aNbCurves = aFF.Lines().Length();
  const int aNbPoints = aFF.Points().Length();

  std::cout << "Face-Face intersection result:" << std::endl;
  std::cout << "  Number of curves: " << aNbCurves << std::endl;
  std::cout << "  Number of points: " << aNbPoints << std::endl;

  // Based on our geometry analysis, these faces should NOT intersect
  // because the cylinder is outside the circular plane boundary
  EXPECT_EQ(aNbCurves, 0) << "Expected no intersection curves";
  EXPECT_EQ(aNbPoints, 0) << "Expected no intersection points";
}

//! Test BRepAlgoAPI_Common using solid shapes (pill/capsule shapes) from BREP files.
//! This reproduces the reported issue where Common produces incorrect results.
TEST_F(IntTools_FaceFaceTest, BrepFilesBooleanCommon_Solids)
{
  // Path to the BREP files - pill-shaped solids
  const char* anArgPath = "/Users/dpasukhi/work/OCCT/arg1.brep";
  const char* aToolPath = "/Users/dpasukhi/work/OCCT/tool1.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape anArg, aTool;

  if (!BRepTools::Read(anArg, anArgPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << anArgPath;
  }
  if (!BRepTools::Read(aTool, aToolPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << aToolPath;
  }

  // Count input faces and edges
  int aNbArgFaces = 0, aNbToolFaces = 0;
  int aNbArgEdges = 0, aNbToolEdges = 0;
  for (TopExp_Explorer anExp(anArg, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbArgFaces;
  for (TopExp_Explorer anExp(aTool, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbToolFaces;
  for (TopExp_Explorer anExp(anArg, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aNbArgEdges;
  for (TopExp_Explorer anExp(aTool, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aNbToolEdges;

  std::cout << "Input solid shapes:" << std::endl;
  std::cout << "  Argument (small pill): " << aNbArgFaces << " faces, " << aNbArgEdges << " edges"
            << std::endl;
  std::cout << "  Tool (large pill): " << aNbToolFaces << " faces, " << aNbToolEdges << " edges"
            << std::endl;

  // Perform Boolean Common with detailed debugging
  BRepAlgoAPI_Common aCommon(anArg, aTool);
  aCommon.SetRunParallel(false);
  aCommon.SetFuzzyValue(1.0e-6);
  aCommon.Build();

  ASSERT_TRUE(aCommon.IsDone()) << "BRepAlgoAPI_Common failed";

  // Check for warnings
  if (aCommon.HasWarnings())
  {
    std::cout << "WARNING: BRepAlgoAPI_Common has warnings" << std::endl;
  }
  if (aCommon.HasErrors())
  {
    std::cout << "ERROR: BRepAlgoAPI_Common has errors" << std::endl;
  }

  const TopoDS_Shape& aResult = aCommon.Shape();

  // Count faces, edges, and solids in the result
  int aNbResultFaces = 0, aNbResultSolids = 0, aNbResultEdges = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbResultFaces;
  for (TopExp_Explorer anExp(aResult, TopAbs_SOLID); anExp.More(); anExp.Next())
    ++aNbResultSolids;
  for (TopExp_Explorer anExp(aResult, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aNbResultEdges;

  std::cout << "\nBoolean Common result:" << std::endl;
  std::cout << "  Result: " << aNbResultFaces << " faces, " << aNbResultEdges << " edges, "
            << aNbResultSolids << " solids" << std::endl;
  std::cout << "  Result is null: " << aResult.IsNull() << std::endl;

  EXPECT_GT(aNbResultFaces, 0) << "Expected result to have faces";
  EXPECT_GT(aNbResultSolids, 0) << "Expected result to have solids";

  // Write result and individual components for inspection
  BRepTools::Write(aResult, "/Users/dpasukhi/work/OCCT/common_test_result.brep");

  // Write each result face separately for analysis
  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next(), ++aFaceIdx)
  {
    std::string aFaceFile =
      "/Users/dpasukhi/work/OCCT/common_result_face_" + std::to_string(aFaceIdx) + ".brep";
    BRepTools::Write(anExp.Current(), aFaceFile.c_str());
  }

  std::cout << "Result written to common_test_result.brep" << std::endl;
  std::cout << "Individual faces written to common_result_face_*.brep" << std::endl;

  // The expected result for Common should be SMALLER than both inputs
  // since it's only the intersection volume
  std::cout << "\nAnalysis:" << std::endl;
  std::cout
    << "  If result has more faces than expected, something is wrong with face splitting/selection"
    << std::endl;
}

//! Diagnostic test to check face classification in boolean Common.
//! Analyzes which face parts are classified as IN vs OUT.
TEST_F(IntTools_FaceFaceTest, BrepFilesFaceClassification_Diagnostics)
{
  const char* anArgPath = "/Users/dpasukhi/work/OCCT/arg1.brep";
  const char* aToolPath = "/Users/dpasukhi/work/OCCT/tool1.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape anArg, aTool;

  if (!BRepTools::Read(anArg, anArgPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << anArgPath;
  }
  if (!BRepTools::Read(aTool, aToolPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << aToolPath;
  }

  // Get solids
  TopoDS_Solid aToolSolid, anArgSolid;
  for (TopExp_Explorer anExp(aTool, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    aToolSolid = TopoDS::Solid(anExp.Current());
    break;
  }
  for (TopExp_Explorer anExp(anArg, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    anArgSolid = TopoDS::Solid(anExp.Current());
    break;
  }

  std::cout << "\n=== Face Classification Analysis ===" << std::endl;

  // First run the Boolean Common to get the split faces
  BRepAlgoAPI_Common aCommon(anArg, aTool);
  aCommon.SetRunParallel(false);
  aCommon.SetFuzzyValue(1.0e-6);
  aCommon.Build();

  const TopoDS_Shape& aResult = aCommon.Shape();

  std::cout << "Classifying RESULT faces:" << std::endl;
  std::cout << "  (Each result face relative to both solids)" << std::endl;

  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next(), ++aFaceIdx)
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

    // Get a point inside the face
    BRepGProp_Face aProp(aFace);
    double         aU1, aU2, aV1, aV2;
    aProp.Bounds(aU1, aU2, aV1, aV2);
    double aUMid = (aU1 + aU2) / 2.0;
    double aVMid = (aV1 + aV2) / 2.0;
    gp_Pnt aPnt;
    gp_Vec aNormal;
    aProp.Normal(aUMid, aVMid, aPnt, aNormal);

    // Classify relative to tool
    BRepClass3d_SolidClassifier aToolClassifier(aToolSolid, aPnt, 1e-7);
    TopAbs_State                aToolState = aToolClassifier.State();

    // Classify relative to argument
    BRepClass3d_SolidClassifier anArgClassifier(anArgSolid, aPnt, 1e-7);
    TopAbs_State                anArgState = anArgClassifier.State();

    std::cout << "  ResultFace[" << aFaceIdx << "]: ";
    std::cout << "Point(" << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << ")" << std::endl;
    std::cout << "      vs Tool: ";
    switch (aToolState)
    {
      case TopAbs_IN:
        std::cout << "IN";
        break;
      case TopAbs_OUT:
        std::cout << "OUT";
        break;
      case TopAbs_ON:
        std::cout << "ON";
        break;
      default:
        std::cout << "UNKNOWN";
        break;
    }
    std::cout << ", vs Arg: ";
    switch (anArgState)
    {
      case TopAbs_IN:
        std::cout << "IN";
        break;
      case TopAbs_OUT:
        std::cout << "OUT";
        break;
      case TopAbs_ON:
        std::cout << "ON";
        break;
      default:
        std::cout << "UNKNOWN";
        break;
    }
    // For Common, face should be IN both solids or ON boundary
    bool bCorrect = (aToolState == TopAbs_IN || aToolState == TopAbs_ON)
                    && (anArgState == TopAbs_IN || anArgState == TopAbs_ON);
    std::cout << " -> " << (bCorrect ? "CORRECT" : "*** WRONG ***") << std::endl;

    // Write face for inspection
    std::string aFaceFile =
      "/Users/dpasukhi/work/OCCT/result_face_" + std::to_string(aFaceIdx) + ".brep";
    BRepTools::Write(aFace, aFaceFile.c_str());
  }
}

//! Detailed diagnostic test to analyze face-face intersections between two pill solids.
//! Uses direct IntTools_FaceFace to examine intersection details.
TEST_F(IntTools_FaceFaceTest, BrepFilesPaveFiller_Diagnostics)
{
  const char* anArgPath = "/Users/dpasukhi/work/OCCT/arg1.brep";
  const char* aToolPath = "/Users/dpasukhi/work/OCCT/tool1.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape anArg, aTool;

  if (!BRepTools::Read(anArg, anArgPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << anArgPath;
  }
  if (!BRepTools::Read(aTool, aToolPath, aBuilder))
  {
    GTEST_SKIP() << "Could not read " << aToolPath;
  }

  // Collect faces for direct analysis
  std::vector<TopoDS_Face> anArgFaces, aToolFaces;
  for (TopExp_Explorer anExp(anArg, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    anArgFaces.push_back(TopoDS::Face(anExp.Current()));
  }
  for (TopExp_Explorer anExp(aTool, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aToolFaces.push_back(TopoDS::Face(anExp.Current()));
  }

  std::cout << "\n=== Direct Face-Face Intersection Analysis ===" << std::endl;
  std::cout << "Argument faces: " << anArgFaces.size() << std::endl;
  std::cout << "Tool faces: " << aToolFaces.size() << std::endl;

  // Test all face pairs
  int aTotalIntersections = 0;
  for (size_t i = 0; i < anArgFaces.size(); ++i)
  {
    for (size_t j = 0; j < aToolFaces.size(); ++j)
    {
      IntTools_FaceFace aFF;
      aFF.SetParameters(true, true, true, 1.0e-6);
      aFF.Perform(anArgFaces[i], aToolFaces[j]);

      if (aFF.IsDone())
      {
        const NCollection_Sequence<IntTools_Curve>& aCurves   = aFF.Lines();
        const int                                   aNbCurves = aCurves.Length();
        const int                                   aNbPoints = aFF.Points().Length();

        if (aNbCurves > 0 || aNbPoints > 0)
        {
          ++aTotalIntersections;
          std::cout << "\nIntersection found: ArgFace[" << i << "] x ToolFace[" << j << "]"
                    << std::endl;
          std::cout << "  Curves: " << aNbCurves << ", Points: " << aNbPoints << std::endl;

          // Print curve details
          for (int k = 1; k <= aNbCurves; ++k)
          {
            const IntTools_Curve&     aCurve = aCurves(k);
            const Handle(Geom_Curve)& aC3D   = aCurve.Curve();
            if (!aC3D.IsNull())
            {
              const double aFirst = aC3D->FirstParameter();
              const double aLast  = aC3D->LastParameter();
              const gp_Pnt aP1    = aC3D->Value(aFirst);
              const gp_Pnt aP2    = aC3D->Value(aLast);
              std::cout << "  Curve " << k << ": params [" << aFirst << ", " << aLast << "]"
                        << std::endl;
              std::cout << "    Start: (" << aP1.X() << ", " << aP1.Y() << ", " << aP1.Z() << ")"
                        << std::endl;
              std::cout << "    End:   (" << aP2.X() << ", " << aP2.Y() << ", " << aP2.Z() << ")"
                        << std::endl;

              // Write curve as edge
              std::string aCurveFile = "/Users/dpasukhi/work/OCCT/diag_ArgFace_" + std::to_string(i)
                                       + "_ToolFace_" + std::to_string(j) + "_Curve_"
                                       + std::to_string(k) + ".brep";
              BRepBuilderAPI_MakeEdge aME(aC3D, aFirst, aLast);
              if (aME.IsDone())
              {
                BRepTools::Write(aME.Edge(), aCurveFile.c_str());
                std::cout << "    Written to: " << aCurveFile << std::endl;
              }
            }
          }

          // Write intersecting face pair for inspection
          std::string aFace1File = "/Users/dpasukhi/work/OCCT/diag_ArgFace_" + std::to_string(i)
                                   + "_ToolFace_" + std::to_string(j) + "_arg.brep";
          std::string aFace2File = "/Users/dpasukhi/work/OCCT/diag_ArgFace_" + std::to_string(i)
                                   + "_ToolFace_" + std::to_string(j) + "_tool.brep";
          BRepTools::Write(anArgFaces[i], aFace1File.c_str());
          BRepTools::Write(aToolFaces[j], aFace2File.c_str());
        }
      }
    }
  }

  std::cout << "\nTotal face-face intersections: " << aTotalIntersections << std::endl;
}

TEST_F(IntTools_FaceFaceTest, NewTest)
{
  std::string aFolderPath = "/Users/dpasukhi/work/OCCT/NotDetected/";
  // other path: "/Users/dpasukhi/work/OCCT/Detected/";
  std::string anArgPath   = aFolderPath + "Argument.brep";
  std::string aToolPath   = aFolderPath + "Tool.brep";
  std::string aCommonPath = aFolderPath + "Common.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape anArgShape;
  BRepTools::Read(anArgShape, anArgPath.c_str(), aBuilder);
  TopoDS_Shape aToolShape;
  BRepTools::Read(aToolShape, aToolPath.c_str(), aBuilder);

  BRepAlgoAPI_Common aCommon;
  aCommon.SetRunParallel(false);
  aCommon.SetFuzzyValue(1.0e-6);
  NCollection_List<TopoDS_Shape> anArgShapes;
  anArgShapes.Append(anArgShape);
  aCommon.SetArguments(anArgShapes);
  NCollection_List<TopoDS_Shape> aToolShapes;
  aToolShapes.Append(aToolShape);
  aCommon.SetTools(aToolShapes);
  aCommon.Build();
  if (!aCommon.IsDone())
  {
    std::cout << "Common is not done\n";
    return;
  }
  // ==========================================
  // Write common shape
  // ==========================================
  TopoDS_Shape aCommonShape = aCommon.Shape();
  if (aCommonShape.IsNull())
  {
    std::cout << "Common shape is null\n";
    return;
  }
  BRepTools::Write(aCommonShape, aCommonPath.c_str());
  ;
}

// Helper function for bug investigation tests
static void runBooleanCommonInvestigation(const std::string& theFolderPath,
                                          const std::string& theTestName,
                                          double             theFuzzyValue = 1.0e-6)
{
  std::cout << "\n========== " << theTestName << " (FuzzyValue=" << theFuzzyValue
            << ") ==========\n";
  std::cout << "Folder: " << theFolderPath << std::endl;

  std::string anArgPath   = theFolderPath + "Argument.brep";
  std::string aToolPath   = theFolderPath + "Tool.brep";
  std::string aCommonPath = theFolderPath + "Common.brep";

  BRep_Builder aBuilder;
  TopoDS_Shape anArgShape;
  if (!BRepTools::Read(anArgShape, anArgPath.c_str(), aBuilder))
  {
    std::cout << "ERROR: Failed to read " << anArgPath << std::endl;
    return;
  }
  std::cout << "Read Argument shape successfully" << std::endl;

  TopoDS_Shape aToolShape;
  if (!BRepTools::Read(aToolShape, aToolPath.c_str(), aBuilder))
  {
    std::cout << "ERROR: Failed to read " << aToolPath << std::endl;
    return;
  }
  std::cout << "Read Tool shape successfully" << std::endl;

  // Count shapes in input
  int anArgFaceCount = 0, anArgEdgeCount = 0;
  for (TopExp_Explorer anExp(anArgShape, TopAbs_FACE); anExp.More(); anExp.Next())
    ++anArgFaceCount;
  for (TopExp_Explorer anExp(anArgShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++anArgEdgeCount;
  std::cout << "Argument: " << anArgFaceCount << " faces, " << anArgEdgeCount << " edges" << std::endl;

  int aToolFaceCount = 0, aToolEdgeCount = 0;
  for (TopExp_Explorer anExp(aToolShape, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aToolFaceCount;
  for (TopExp_Explorer anExp(aToolShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aToolEdgeCount;
  std::cout << "Tool: " << aToolFaceCount << " faces, " << aToolEdgeCount << " edges" << std::endl;

  std::cout << "\n--- Starting Boolean Common operation ---\n";

  BRepAlgoAPI_Common aCommon;
  aCommon.SetRunParallel(false);
  aCommon.SetFuzzyValue(theFuzzyValue);
  NCollection_List<TopoDS_Shape> anArgShapes;
  anArgShapes.Append(anArgShape);
  aCommon.SetArguments(anArgShapes);
  NCollection_List<TopoDS_Shape> aToolShapes;
  aToolShapes.Append(aToolShape);
  aCommon.SetTools(aToolShapes);

  std::cout << "FuzzyValue: " << aCommon.FuzzyValue() << std::endl;

  aCommon.Build();

  std::cout << "\n--- Boolean Common operation completed ---\n";

  if (!aCommon.IsDone())
  {
    std::cout << "RESULT: Common operation NOT DONE" << std::endl;
    return;
  }

  TopoDS_Shape aCommonShape = aCommon.Shape();
  if (aCommonShape.IsNull())
  {
    std::cout << "RESULT: Common shape is NULL" << std::endl;
    return;
  }

  // Count shapes in result
  int aResultFaceCount = 0, aResultEdgeCount = 0, aResultSolidCount = 0;
  for (TopExp_Explorer anExp(aCommonShape, TopAbs_SOLID); anExp.More(); anExp.Next())
    ++aResultSolidCount;
  for (TopExp_Explorer anExp(aCommonShape, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aResultFaceCount;
  for (TopExp_Explorer anExp(aCommonShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aResultEdgeCount;

  std::cout << "RESULT: " << aResultSolidCount << " solids, " << aResultFaceCount << " faces, "
            << aResultEdgeCount << " edges" << std::endl;

  BRepTools::Write(aCommonShape, aCommonPath.c_str());
  std::cout << "Result written to: " << aCommonPath << std::endl;
  std::cout << "========== END " << theTestName << " ==========\n\n";
}

// Test for NotDetected case with FuzzyValue=1e-6 (failing - shapes perfectly aligned)
TEST_F(IntTools_FaceFaceTest, BugInvestigation_NotDetected)
{
  runBooleanCommonInvestigation("/Users/dpasukhi/work/OCCT/NotDetected/", "NotDetected", 1.0e-6);
}

// Test for NotDetected case with FuzzyValue=1e-5 (for comparison - may work)
TEST_F(IntTools_FaceFaceTest, BugInvestigation_NotDetected_Fuzzy1e5)
{
  runBooleanCommonInvestigation("/Users/dpasukhi/work/OCCT/NotDetected/", "NotDetected_Fuzzy1e5", 1.0e-5);
}

// Test for Detected case (working - shapes slightly moved)
TEST_F(IntTools_FaceFaceTest, BugInvestigation_Detected)
{
  runBooleanCommonInvestigation("/Users/dpasukhi/work/OCCT/Detected/", "Detected", 1.0e-6);
}