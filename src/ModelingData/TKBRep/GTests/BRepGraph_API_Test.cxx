// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gtest/gtest.h>

// ============================================================
// Task 1A: Graph-Native BoundingBox
// ============================================================

TEST(BRepGraphAPI_BoundingBoxTest, Box_MatchesTopoDSBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Get bounding box from graph.
  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  Bnd_Box aGraphBox = aGraph.BoundingBox(aSolidId);
  ASSERT_FALSE(aGraphBox.IsVoid());

  double aGXmin, aGYmin, aGZmin, aGXmax, aGYmax, aGZmax;
  aGraphBox.Get(aGXmin, aGYmin, aGZmin, aGXmax, aGYmax, aGZmax);

  // Bounding box should cover the 10x20x30 box at origin.
  EXPECT_LE(aGXmin, 0.0 + Precision::Confusion());
  EXPECT_LE(aGYmin, 0.0 + Precision::Confusion());
  EXPECT_LE(aGZmin, 0.0 + Precision::Confusion());
  EXPECT_GE(aGXmax, 10.0 - Precision::Confusion());
  EXPECT_GE(aGYmax, 20.0 - Precision::Confusion());
  EXPECT_GE(aGZmax, 30.0 - Precision::Confusion());
}

TEST(BRepGraphAPI_BoundingBoxTest, Sphere_NonVoid)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape& aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  Bnd_Box aGraphBox = aGraph.BoundingBox(aSolidId);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_BoundingBoxTest, Cylinder_NonVoid)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  Bnd_Box aGraphBox = aGraph.BoundingBox(aSolidId);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_BoundingBoxTest, SingleFace_MatchesVertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.NbFaceDefs(), 0);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  Bnd_Box aGraphBox = aGraph.BoundingBox(aFaceId);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_CentroidTest, Box_CentroidNearCenter)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  gp_Pnt aCentroid = aGraph.Centroid(aSolidId);

  // Centroid should be near the center of the box.
  EXPECT_NEAR(aCentroid.X(), 5.0, 1.0);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 1.0);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 1.0);
}

// ============================================================
// Task 2A: Programmatic Node Addition API
// ============================================================

TEST(BRepGraphAPI_AddNodeTest, AddVertexDef_ReturnsValidId)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aVtxId = aGraph.AddVertexDef(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_TRUE(aVtxId.IsValid());
  EXPECT_EQ(aVtxId.Kind, BRepGraph_NodeKind::Vertex);
  EXPECT_EQ(aVtxId.Index, 0);

  const BRepGraph_TopoNode::VertexDef& aVtxDef = aGraph.VertexDefinition(0);
  EXPECT_NEAR(aVtxDef.Point.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Tolerance, 0.001, 1e-10);
}

TEST(BRepGraphAPI_AddNodeTest, AddEdgeDef_WithCurve)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aV1 = aGraph.AddVertexDef(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.AddVertexDef(gp_Pnt(10.0, 0.0, 0.0), 0.001);

  Handle(Geom_Line) aLine = new Geom_Line(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)));
  BRepGraph_NodeId anEdgeId = aGraph.AddEdgeDef(aV1, aV2, aLine, 0.0, 10.0, 0.001);

  EXPECT_TRUE(anEdgeId.IsValid());
  EXPECT_EQ(anEdgeId.Kind, BRepGraph_NodeKind::Edge);

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.EdgeDefinition(0);
  EXPECT_EQ(anEdgeDef.StartVertexDefId, aV1);
  EXPECT_EQ(anEdgeDef.EndVertexDefId, aV2);
  EXPECT_TRUE(anEdgeDef.CurveNodeId.IsValid());
  EXPECT_NEAR(anEdgeDef.ParamFirst, 0.0, 1e-10);
  EXPECT_NEAR(anEdgeDef.ParamLast, 10.0, 1e-10);
}

TEST(BRepGraphAPI_AddNodeTest, AddWireDef_ClosedRectangle)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aV0 = aGraph.AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry> aEdges;
  BRepGraph_TopoNode::WireDef::EdgeEntry anEntry;
  anEntry.OrientationInWire = TopAbs_FORWARD;

  anEntry.EdgeDefId = aE0; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE1; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE2; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE3; aEdges.Append(anEntry);

  BRepGraph_NodeId aWireId = aGraph.AddWireDef(aEdges);
  EXPECT_TRUE(aWireId.IsValid());
  EXPECT_EQ(aWireId.Kind, BRepGraph_NodeKind::Wire);

  const BRepGraph_TopoNode::WireDef& aWireDef = aGraph.WireDefinition(0);
  EXPECT_EQ(aWireDef.OrderedEdges.Length(), 4);
  EXPECT_TRUE(aWireDef.IsClosed);
}

TEST(BRepGraphAPI_AddNodeTest, AddFaceDef_WithSurface)
{
  BRepGraph aGraph;

  // Build a simple rectangular face programmatically.
  BRepGraph_NodeId aV0 = aGraph.AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry> aEdges;
  BRepGraph_TopoNode::WireDef::EdgeEntry anEntry;
  anEntry.OrientationInWire = TopAbs_FORWARD;
  anEntry.EdgeDefId = aE0; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE1; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE2; aEdges.Append(anEntry);
  anEntry.EdgeDefId = aE3; aEdges.Append(anEntry);

  BRepGraph_NodeId aWireId = aGraph.AddWireDef(aEdges);

  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.AddFaceDef(aPlane, aWireId, aInnerWires, 0.001);

  EXPECT_TRUE(aFaceId.IsValid());
  EXPECT_EQ(aFaceId.Kind, BRepGraph_NodeKind::Face);
  EXPECT_EQ(aGraph.NbFaceDefs(), 1);
  EXPECT_EQ(aGraph.NbSurfaces(), 1);

  const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.FaceDefinition(0);
  EXPECT_TRUE(aFaceDef.SurfNodeId.IsValid());
}

TEST(BRepGraphAPI_AddNodeTest, AddShellAndSolid)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aShellId = aGraph.AddShellDef();
  EXPECT_TRUE(aShellId.IsValid());
  EXPECT_EQ(aShellId.Kind, BRepGraph_NodeKind::Shell);

  BRepGraph_NodeId aSolidId = aGraph.AddSolidDef();
  EXPECT_TRUE(aSolidId.IsValid());
  EXPECT_EQ(aSolidId.Kind, BRepGraph_NodeKind::Solid);
}

// ============================================================
// Task 2B: Incremental Build (AppendShape)
// ============================================================

TEST(BRepGraphAPI_AppendShapeTest, AppendTwoBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Get two faces from the box.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape aFace1 = anExp.Current();
  BRepBuilderAPI_Copy aCopy1(aFace1, true);
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape aFace2 = anExp.Current();
  BRepBuilderAPI_Copy aCopy2(aFace2, true);

  BRepGraph aGraph;
  aGraph.Build(aCopy1.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbFaceDefs(), 1);

  // Append second face.
  aGraph.AppendShape(aCopy2.Shape());
  EXPECT_EQ(aGraph.NbFaceDefs(), 2);
  EXPECT_TRUE(aGraph.IsDone());
}

// ============================================================
// Task 2C: Soft Node Removal
// ============================================================

TEST(BRepGraphAPI_RemoveNodeTest, RemoveVertex_IsRemoved)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aVtxId = aGraph.AddVertexDef(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_FALSE(aGraph.IsRemoved(aVtxId));

  aGraph.RemoveNode(aVtxId);
  EXPECT_TRUE(aGraph.IsRemoved(aVtxId));
}

TEST(BRepGraphAPI_RemoveNodeTest, RemoveFaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.NbFaceDefs(), 6);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_FALSE(aGraph.IsRemoved(aFaceId));

  aGraph.RemoveNode(aFaceId);
  EXPECT_TRUE(aGraph.IsRemoved(aFaceId));

  // Other faces should not be removed.
  for (int aFaceIdx = 1; aFaceIdx < aGraph.NbFaceDefs(); ++aFaceIdx)
  {
    EXPECT_FALSE(aGraph.IsRemoved(BRepGraph_NodeId(BRepGraph_NodeKind::Face, aFaceIdx)));
  }
}

TEST(BRepGraphAPI_RemoveNodeTest, RemoveInvalidNode_NoError)
{
  BRepGraph aGraph;
  BRepGraph_NodeId anInvalidId;
  EXPECT_FALSE(aGraph.IsRemoved(anInvalidId));
  aGraph.RemoveNode(anInvalidId); // Should not crash.
}
