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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom_Plane.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

// Test for bug 0032395: BRepMesh_Delaun::isVertexInsidePolygon
// The method uses winding number algorithm to check if a point is inside a polygon.
// For CCW polygons, the cumulative angle is -2*PI, but the original code only
// checked for +2*PI. This test verifies that both orientations are handled correctly.

TEST(BRepMesh_DelaunTest, Vec2dAngleSignConvention)
{
  // Verify gp_Vec2d::Angle sign convention used in isVertexInsidePolygon
  // The angle from aPrev to aCur is computed as aCur.Angle(aPrev)

  // Test CCW rotation: from (1,0) to (0,1) is 90 degrees CCW
  gp_Vec2d aVecRight(1.0, 0.0);
  gp_Vec2d aVecUp(0.0, 1.0);

  // aCur.Angle(aPrev) where aCur=up, aPrev=right
  // Going from right to up is CCW, angle should be negative in OCCT convention
  const double anAngleCCW = aVecUp.Angle(aVecRight);
  EXPECT_LT(anAngleCCW, 0.0) << "CCW rotation should give negative angle in OCCT convention";
  EXPECT_NEAR(anAngleCCW, -M_PI_2, Precision::Angular());

  // Test CW rotation: from (0,1) to (1,0) is 90 degrees CW
  const double anAngleCW = aVecRight.Angle(aVecUp);
  EXPECT_GT(anAngleCW, 0.0) << "CW rotation should give positive angle in OCCT convention";
  EXPECT_NEAR(anAngleCW, M_PI_2, Precision::Angular());
}

TEST(BRepMesh_DelaunTest, WindingAngleCCWPolygon)
{
  // Simulate the winding angle calculation for a CCW square polygon
  // with a point at the center
  // Vertices: (1,0), (0,1), (-1,0), (0,-1) - CCW order
  // Center point: (0,0)

  gp_XY aCenter(0.0, 0.0);
  gp_XY aVertices[5] = {
    gp_XY(1.0, 0.0),
    gp_XY(0.0, 1.0),
    gp_XY(-1.0, 0.0),
    gp_XY(0.0, -1.0),
    gp_XY(1.0, 0.0) // Close the polygon
  };

  gp_Vec2d aPrevDir(aVertices[0] - aCenter);
  double   aTotalAngle = 0.0;

  for (int i = 1; i < 5; ++i)
  {
    gp_Vec2d aCurDir(aVertices[i] - aCenter);
    aTotalAngle += aCurDir.Angle(aPrevDir);
    aPrevDir = aCurDir;
  }

  // For CCW polygon with point inside, total angle should be -2*PI
  EXPECT_NEAR(std::abs(aTotalAngle), 2.0 * M_PI, Precision::Angular())
    << "Winding angle magnitude should be 2*PI for point inside polygon";

  // The fix checks std::abs(std::abs(aTotalAng) - Angle2PI), so both +2PI and -2PI work
  const double Angle2PI       = 2.0 * M_PI;
  const bool   isInsideOldFix = std::abs(Angle2PI - aTotalAngle) <= Precision::Angular();
  const bool   isInsideNewFix = std::abs(std::abs(aTotalAngle) - Angle2PI) <= Precision::Angular();

  EXPECT_FALSE(isInsideOldFix) << "Old check fails for CCW polygon (angle is negative)";
  EXPECT_TRUE(isInsideNewFix) << "New check handles both CCW and CW polygons";
}

TEST(BRepMesh_DelaunTest, WindingAngleCWPolygon)
{
  // Simulate the winding angle calculation for a CW square polygon
  // Vertices in CW order: (1,0), (0,-1), (-1,0), (0,1)
  // Center point: (0,0)

  gp_XY aCenter(0.0, 0.0);
  gp_XY aVertices[5] = {
    gp_XY(1.0, 0.0),
    gp_XY(0.0, -1.0),
    gp_XY(-1.0, 0.0),
    gp_XY(0.0, 1.0),
    gp_XY(1.0, 0.0) // Close the polygon
  };

  gp_Vec2d aPrevDir(aVertices[0] - aCenter);
  double   aTotalAngle = 0.0;

  for (int i = 1; i < 5; ++i)
  {
    gp_Vec2d aCurDir(aVertices[i] - aCenter);
    aTotalAngle += aCurDir.Angle(aPrevDir);
    aPrevDir = aCurDir;
  }

  // For CW polygon with point inside, total angle should be +2*PI
  EXPECT_NEAR(aTotalAngle, 2.0 * M_PI, Precision::Angular())
    << "Winding angle should be +2*PI for CW polygon";

  const double Angle2PI       = 2.0 * M_PI;
  const bool   isInsideOldFix = std::abs(Angle2PI - aTotalAngle) <= Precision::Angular();
  const bool   isInsideNewFix = std::abs(std::abs(aTotalAngle) - Angle2PI) <= Precision::Angular();

  EXPECT_TRUE(isInsideOldFix) << "Old check works for CW polygon";
  EXPECT_TRUE(isInsideNewFix) << "New check also works for CW polygon";
}

TEST(BRepMesh_DelaunTest, MeshPlanarFaceWithHole)
{
  // Create a planar face with a hole - this exercises the Delaunay triangulation
  // code path that uses isVertexInsidePolygon during mesh refinement

  // Outer wire: square 10x10 centered at origin
  gp_Pnt aP1(-5.0, -5.0, 0.0);
  gp_Pnt aP2(5.0, -5.0, 0.0);
  gp_Pnt aP3(5.0, 5.0, 0.0);
  gp_Pnt aP4(-5.0, 5.0, 0.0);

  BRepBuilderAPI_MakeWire anOuterWireMaker;
  anOuterWireMaker.Add(BRepBuilderAPI_MakeEdge(aP1, aP2));
  anOuterWireMaker.Add(BRepBuilderAPI_MakeEdge(aP2, aP3));
  anOuterWireMaker.Add(BRepBuilderAPI_MakeEdge(aP3, aP4));
  anOuterWireMaker.Add(BRepBuilderAPI_MakeEdge(aP4, aP1));
  ASSERT_TRUE(anOuterWireMaker.IsDone());

  // Inner wire (hole): smaller square 2x2 centered at origin
  gp_Pnt aH1(-1.0, -1.0, 0.0);
  gp_Pnt aH2(1.0, -1.0, 0.0);
  gp_Pnt aH3(1.0, 1.0, 0.0);
  gp_Pnt aH4(-1.0, 1.0, 0.0);

  BRepBuilderAPI_MakeWire anInnerWireMaker;
  anInnerWireMaker.Add(BRepBuilderAPI_MakeEdge(aH1, aH2));
  anInnerWireMaker.Add(BRepBuilderAPI_MakeEdge(aH2, aH3));
  anInnerWireMaker.Add(BRepBuilderAPI_MakeEdge(aH3, aH4));
  anInnerWireMaker.Add(BRepBuilderAPI_MakeEdge(aH4, aH1));
  ASSERT_TRUE(anInnerWireMaker.IsDone());

  // Create face with hole
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ()));
  BRepBuilderAPI_MakeFace aFaceMaker(aPlane, anOuterWireMaker.Wire());
  aFaceMaker.Add(anInnerWireMaker.Wire());
  ASSERT_TRUE(aFaceMaker.IsDone());

  TopoDS_Face aFace = aFaceMaker.Face();

  // Mesh the face
  BRepMesh_IncrementalMesh aMesher(aFace, 0.1);
  EXPECT_TRUE(aMesher.IsDone()) << "Meshing should succeed";

  // Verify triangulation exists
  TopLoc_Location                       aLoc;
  const occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aLoc);
  ASSERT_FALSE(aTri.IsNull()) << "Triangulation should be created";
  EXPECT_GT(aTri->NbTriangles(), 0) << "Should have triangles";
  EXPECT_GT(aTri->NbNodes(), 0) << "Should have nodes";
}

TEST(BRepMesh_DelaunTest, MeshBoxAllFaces)
{
  // Create and mesh a box - tests triangulation on faces with different orientations
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);
  EXPECT_TRUE(aMesher.IsDone()) << "Meshing should succeed";

  // Verify all faces are triangulated
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face&                    aFace = TopoDS::Face(anExp.Current());
    TopLoc_Location                       aLoc;
    const occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aLoc);

    EXPECT_FALSE(aTri.IsNull()) << "Face " << aFaceCount << " should have triangulation";
    if (!aTri.IsNull())
    {
      EXPECT_GT(aTri->NbTriangles(), 0) << "Face " << aFaceCount << " should have triangles";
    }
    ++aFaceCount;
  }

  EXPECT_EQ(aFaceCount, 6) << "Box should have 6 faces";
}

TEST(BRepMesh_DelaunTest, MeshCylinderCurvedFaces)
{
  // Create and mesh a cylinder - curved faces may have different polygon orientations
  TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape();

  BRepMesh_IncrementalMesh aMesher(aCylinder, 0.5);
  EXPECT_TRUE(aMesher.IsDone()) << "Meshing should succeed";

  // Verify faces are triangulated
  int aTotalTriangles = 0;
  for (TopExp_Explorer anExp(aCylinder, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face&                    aFace = TopoDS::Face(anExp.Current());
    TopLoc_Location                       aLoc;
    const occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aLoc);

    EXPECT_FALSE(aTri.IsNull()) << "Face should have triangulation";
    if (!aTri.IsNull())
    {
      aTotalTriangles += aTri->NbTriangles();
    }
  }

  EXPECT_GT(aTotalTriangles, 0) << "Cylinder should have triangles";
}
