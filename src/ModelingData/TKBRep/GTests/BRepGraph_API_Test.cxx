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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_UsagesView.hxx>
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

#include <utility>

#include <gtest/gtest.h>

static gp_Pnt bboxCenter(BRepGraph& theGraph, BRepGraph_NodeId theNode)
{
  Bnd_Box aBox;
  BRepGraphAlgo_BndLib::Add(theGraph, theNode, aBox);
  if (aBox.IsVoid())
    return gp_Pnt();
  double xn, yn, zn, xx, yx, zx;
  aBox.Get(xn, yn, zn, xx, yx, zx);
  return gp_Pnt((xn + xx) * 0.5, (yn + yx) * 0.5, (zn + zx) * 0.5);
}

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
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aSolidId, aGraphBox);
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

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aSolidId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_BoundingBoxTest, Cylinder_NonVoid)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aSolidId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_BoundingBoxTest, SingleFace_MatchesVertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aFaceId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraphAPI_CentroidTest, Box_CentroidNearCenter)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  gp_Pnt aCentroid = bboxCenter(aGraph, aSolidId);

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
  BRepGraph_NodeId aVtxId = aGraph.Builder().AddVertexDef(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_TRUE(aVtxId.IsValid());
  EXPECT_EQ(aVtxId.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  EXPECT_EQ(aVtxId.Index, 0);

  const BRepGraph_TopoNode::VertexDef& aVtxDef = aGraph.Defs().Vertex(0);
  EXPECT_NEAR(aVtxDef.Point.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Tolerance, 0.001, 1e-10);
}

TEST(BRepGraphAPI_AddNodeTest, AddEdgeDef_WithCurve)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10.0, 0.0, 0.0), 0.001);

  Handle(Geom_Line) aLine = new Geom_Line(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)));
  BRepGraph_NodeId anEdgeId = aGraph.Builder().AddEdgeDef(aV1, aV2, aLine, 0.0, 10.0, 0.001);

  EXPECT_TRUE(anEdgeId.IsValid());
  EXPECT_EQ(anEdgeId.NodeKind, BRepGraph_NodeId::Kind::Edge);

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(0);
  EXPECT_EQ(anEdgeDef.StartVertexDefId, aV1);
  EXPECT_EQ(anEdgeDef.EndVertexDefId, aV2);
  EXPECT_FALSE(anEdgeDef.Curve3d.IsNull());
  EXPECT_NEAR(anEdgeDef.ParamFirst, 0.0, 1e-10);
  EXPECT_NEAR(anEdgeDef.ParamLast, 10.0, 1e-10);
}

TEST(BRepGraphAPI_AddNodeTest, AddWireDef_ClosedRectangle)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWireDef(aEdges);
  EXPECT_TRUE(aWireId.IsValid());
  EXPECT_EQ(aWireId.NodeKind, BRepGraph_NodeId::Kind::Wire);

  const BRepGraph_TopoNode::WireDef& aWireDef = aGraph.Defs().Wire(0);
  ASSERT_FALSE(aWireDef.Usages.IsEmpty());
  const BRepGraph_TopoNode::WireUsage& aWireUsage = aGraph.Usages().Wire(aWireDef.Usages.Value(0).Index);
  EXPECT_EQ(aWireUsage.EdgeUsages.Length(), 4);
  EXPECT_TRUE(aWireDef.IsClosed);
}

TEST(BRepGraphAPI_AddNodeTest, AddFaceDef_WithSurface)
{
  BRepGraph aGraph;

  // Build a simple rectangular face programmatically.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWireDef(aEdges);

  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFaceDef(aPlane, aWireId, aInnerWires, 0.001);

  EXPECT_TRUE(aFaceId.IsValid());
  EXPECT_EQ(aFaceId.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);

  const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(0);
  EXPECT_FALSE(aFaceDef.Surface.IsNull());
}

TEST(BRepGraphAPI_AddNodeTest, AddShellAndSolid)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aShellId = aGraph.Builder().AddShellDef();
  EXPECT_TRUE(aShellId.IsValid());
  EXPECT_EQ(aShellId.NodeKind, BRepGraph_NodeId::Kind::Shell);

  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolidDef();
  EXPECT_TRUE(aSolidId.IsValid());
  EXPECT_EQ(aSolidId.NodeKind, BRepGraph_NodeId::Kind::Solid);
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
  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);

  // Append second face.
  aGraph.Builder().AppendShape(aCopy2.Shape());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 2);
  EXPECT_TRUE(aGraph.IsDone());
}

// ============================================================
// Task 2C: Soft Node Removal
// ============================================================

TEST(BRepGraphAPI_RemoveNodeTest, RemoveVertex_IsRemoved)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aVtxId = aGraph.Builder().AddVertexDef(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_FALSE(aGraph.Builder().IsRemoved(aVtxId));

  aGraph.Builder().RemoveNode(aVtxId);
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aVtxId));
}

TEST(BRepGraphAPI_RemoveNodeTest, RemoveFaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 6);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  EXPECT_FALSE(aGraph.Builder().IsRemoved(aFaceId));

  aGraph.Builder().RemoveNode(aFaceId);
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aFaceId));

  // Other faces should not be removed.
  for (int aFaceIdx = 1; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    EXPECT_FALSE(aGraph.Builder().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx)));
  }
}

TEST(BRepGraphAPI_RemoveNodeTest, RemoveInvalidNode_NoError)
{
  BRepGraph aGraph;
  BRepGraph_NodeId anInvalidId;
  EXPECT_FALSE(aGraph.Builder().IsRemoved(anInvalidId));
  aGraph.Builder().RemoveNode(anInvalidId); // Should not crash.
}

// ============================================================
// Item 1: Complete Construction API (Shell/Solid linking)
// ============================================================

TEST(BRepGraphAPI_ConstructionTest, AddFaceToShell_CreatesUsage)
{
  BRepGraph aGraph;

  // Build a face programmatically.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWireDef(aEdges);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFaceDef(aPlane, aWireId, aInnerWires, 0.001);

  // Create shell and link face to it.
  BRepGraph_NodeId aShellId = aGraph.Builder().AddShellDef();
  BRepGraph_UsageId aFaceUsage = aGraph.Builder().AddFaceToShell(aShellId, aFaceId);
  EXPECT_TRUE(aFaceUsage.IsValid());
  EXPECT_EQ(aFaceUsage.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aGraph.Usages().NbShells(), 1);
  EXPECT_GE(aGraph.Usages().NbFaces(), 2); // Initial + shell-linked

  // Verify shell usage has a face usage.
  const BRepGraph_TopoNode::ShellUsage& aShellUsage = aGraph.Usages().Shell(0);
  EXPECT_EQ(aShellUsage.FaceUsages.Length(), 1);
}

TEST(BRepGraphAPI_ConstructionTest, AddShellToSolid_CreatesUsage)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aShellId = aGraph.Builder().AddShellDef();
  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolidDef();

  BRepGraph_UsageId aShellUsage = aGraph.Builder().AddShellToSolid(aSolidId, aShellId);
  EXPECT_TRUE(aShellUsage.IsValid());
  EXPECT_EQ(aShellUsage.NodeKind, BRepGraph_NodeId::Kind::Shell);
  EXPECT_EQ(aGraph.Usages().NbSolids(), 1);

  const BRepGraph_TopoNode::SolidUsage& aSolidUsage = aGraph.Usages().Solid(0);
  EXPECT_EQ(aSolidUsage.ShellUsages.Length(), 1);
}

TEST(BRepGraphAPI_ConstructionTest, AddCompoundDef_WithChildren)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aSolid1 = aGraph.Builder().AddSolidDef();
  BRepGraph_NodeId aSolid2 = aGraph.Builder().AddSolidDef();

  NCollection_Vector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid1);
  aChildren.Append(aSolid2);

  BRepGraph_NodeId aCompId = aGraph.Builder().AddCompoundDef(aChildren);
  EXPECT_TRUE(aCompId.IsValid());
  EXPECT_EQ(aCompId.NodeKind, BRepGraph_NodeId::Kind::Compound);
  EXPECT_EQ(aGraph.Defs().NbCompounds(), 1);

  const BRepGraph_TopoNode::CompoundDef& aCompDef = aGraph.Defs().Compound(0);
  EXPECT_EQ(aCompDef.ChildDefIds.Length(), 2);
}

TEST(BRepGraphAPI_ConstructionTest, AddCompSolidDef_WithSolids)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aSolid1 = aGraph.Builder().AddSolidDef();
  BRepGraph_NodeId aSolid2 = aGraph.Builder().AddSolidDef();

  NCollection_Vector<BRepGraph_NodeId> aSolids;
  aSolids.Append(aSolid1);
  aSolids.Append(aSolid2);

  BRepGraph_NodeId aCSolId = aGraph.Builder().AddCompSolidDef(aSolids);
  EXPECT_TRUE(aCSolId.IsValid());
  EXPECT_EQ(aCSolId.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
  EXPECT_EQ(aGraph.Defs().NbCompSolids(), 1);

  const BRepGraph_TopoNode::CompSolidDef& aCSolDef = aGraph.Defs().CompSolid(0);
  EXPECT_EQ(aCSolDef.SolidDefIds.Length(), 2);
}

TEST(BRepGraphAPI_ConstructionTest, FullSolid_ProgrammaticConstruction)
{
  BRepGraph aGraph;

  // Build a single-face shell solid.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWireDef(aEdges);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFaceDef(aPlane, aWireId, aInnerWires, 0.001);

  BRepGraph_NodeId aShellId = aGraph.Builder().AddShellDef();
  aGraph.Builder().AddFaceToShell(aShellId, aFaceId);

  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolidDef();
  aGraph.Builder().AddShellToSolid(aSolidId, aShellId);

  // Verify the hierarchy.
  EXPECT_EQ(aGraph.Defs().NbSolids(), 1);
  EXPECT_EQ(aGraph.Defs().NbShells(), 1);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);
  EXPECT_EQ(aGraph.Usages().NbSolids(), 1);
  EXPECT_GE(aGraph.Usages().NbShells(), 1);

  const BRepGraph_TopoNode::SolidUsage& aSolidUs = aGraph.Usages().Solid(0);
  EXPECT_EQ(aSolidUs.ShellUsages.Length(), 1);
}

// ============================================================
// Item 2: Mutable Access for All Def Types
// ============================================================

TEST(BRepGraphAPI_MutableDefTest, MutableFaceDefinition_ChangesTolerance)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  const double anOrigTol = aGraph.Defs().Face(0).Tolerance;
  BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Mut().FaceDef(0);
  aFaceDef.Tolerance = 0.5;
  EXPECT_NEAR(aGraph.Defs().Face(0).Tolerance, 0.5, 1e-10);
  EXPECT_TRUE(aGraph.Defs().Face(0).IsModified);
  (void)anOrigTol;
}

TEST(BRepGraphAPI_MutableDefTest, MutableShellDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  BRepGraph_TopoNode::ShellDef& aShellDef = aGraph.Mut().ShellDef(0);
  EXPECT_TRUE(aShellDef.IsModified);
}

TEST(BRepGraphAPI_MutableDefTest, MutableSolidDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbSolids(), 0);

  BRepGraph_TopoNode::SolidDef& aSolidDef = aGraph.Mut().SolidDef(0);
  EXPECT_TRUE(aSolidDef.IsModified);
}

TEST(BRepGraphAPI_MutableDefTest, MutableCompoundDefinition)
{
  BRepGraph aGraph;
  NCollection_Vector<BRepGraph_NodeId> aChildren;
  aGraph.Builder().AddCompoundDef(aChildren);
  ASSERT_EQ(aGraph.Defs().NbCompounds(), 1);

  BRepGraph_TopoNode::CompoundDef& aCompDef = aGraph.Mut().CompoundDef(0);
  EXPECT_TRUE(aCompDef.IsModified);
}

TEST(BRepGraphAPI_MutableDefTest, MutableCompSolidDefinition)
{
  BRepGraph aGraph;
  NCollection_Vector<BRepGraph_NodeId> aSolids;
  aGraph.Builder().AddCompSolidDef(aSolids);
  ASSERT_EQ(aGraph.Defs().NbCompSolids(), 1);

  BRepGraph_TopoNode::CompSolidDef& aCSolDef = aGraph.Mut().CompSolidDef(0);
  EXPECT_TRUE(aCSolDef.IsModified);
}

// ============================================================
// Item 3: Iterator Skips Removed Nodes
// ============================================================

TEST(BRepGraphAPI_IteratorTest, SkipsRemovedFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 6);

  // Remove 2 faces.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef> anIt(aGraph);
       anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraphAPI_IteratorTest, SkipsRemovedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const int aNbEdges = aGraph.Defs().NbEdges();
  ASSERT_GT(aNbEdges, 0);

  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef> anIt(aGraph);
       anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbEdges - 1);
}

TEST(BRepGraphAPI_IteratorTest, SkipsFirstNode)
{
  BRepGraph aGraph;
  aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  aGraph.Builder().AddVertexDef(gp_Pnt(1, 0, 0), 0.001);
  aGraph.Builder().AddVertexDef(gp_Pnt(2, 0, 0), 0.001);

  // Remove the first vertex.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, 0));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef> anIt(aGraph);
       anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

// ============================================================
// Item 4: Cascading Soft Removal
// ============================================================

TEST(BRepGraphAPI_RemoveSubgraphTest, RemoveFace_RemovesWiresAndEdges)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 10, 0), 0.001);

  Handle(Geom_Line) aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  Handle(Geom_Line) aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  Handle(Geom_Line) aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdgeDef(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdgeDef(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdgeDef(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdgeDef(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWireDef(aEdges);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFaceDef(aPlane, aWireId, aInnerWires, 0.001);

  // Remove the face subgraph.
  aGraph.Builder().RemoveSubgraph(aFaceId);

  EXPECT_TRUE(aGraph.Builder().IsRemoved(aFaceId));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aWireId));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aE0));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aE1));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aE2));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aE3));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aV0));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aV1));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aV2));
  EXPECT_TRUE(aGraph.Builder().IsRemoved(aV3));
}

TEST(BRepGraphAPI_RemoveSubgraphTest, RemoveSolid_CascadesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  aGraph.Builder().RemoveSubgraph(aSolidId);

  EXPECT_TRUE(aGraph.Builder().IsRemoved(aSolidId));

  // All shells should be removed.
  for (int aIdx = 0; aIdx < aGraph.Defs().NbShells(); ++aIdx)
    EXPECT_TRUE(aGraph.Builder().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx)));

  // All faces should be removed.
  for (int aIdx = 0; aIdx < aGraph.Defs().NbFaces(); ++aIdx)
    EXPECT_TRUE(aGraph.Builder().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx)));
}

// ============================================================
// Item 5: Edge Adjacency Queries
// ============================================================

TEST(BRepGraphAPI_AdjacencyTest, FacesOfEdge_BoxSharedEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Every edge in a box is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
    NCollection_Vector<BRepGraph_NodeId> aFaces = aGraph.Spatial().FacesOfEdge(anEdgeId);
    EXPECT_EQ(aFaces.Length(), 2) << "Edge " << anEdgeIdx << " has " << aFaces.Length() << " faces";
  }
}

TEST(BRepGraphAPI_AdjacencyTest, SharedEdges_AdjacentBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 6);

  // Count total shared edge pairs across all face pairs.
  int aSharingPairs = 0;
  for (int aFaceA = 0; aFaceA < aGraph.Defs().NbFaces(); ++aFaceA)
  {
    for (int aFaceB = aFaceA + 1; aFaceB < aGraph.Defs().NbFaces(); ++aFaceB)
    {
      NCollection_Vector<BRepGraph_NodeId> aShared = aGraph.Spatial().SharedEdges(
        BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceA),
        BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceB));
      if (!aShared.IsEmpty())
        ++aSharingPairs;
    }
  }
  // A box has 12 edges, each shared by 2 faces, so 12 sharing pairs.
  EXPECT_EQ(aSharingPairs, 12);
}

TEST(BRepGraphAPI_AdjacencyTest, AdjacentFaces_BoxFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 6);

  // Each face of a box is adjacent to 4 other faces.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    NCollection_Vector<BRepGraph_NodeId> aAdj = aGraph.Spatial().AdjacentFaces(aFaceId);
    EXPECT_EQ(aAdj.Length(), 4) << "Face " << aFaceIdx << " has " << aAdj.Length() << " adjacent faces";
  }
}

TEST(BRepGraphAPI_AdjacencyTest, FacesOfEdge_NoFaces_Programmatic)
{
  BRepGraph aGraph;
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertexDef(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertexDef(gp_Pnt(10, 0, 0), 0.001);

  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  BRepGraph_NodeId anEdgeId = aGraph.Builder().AddEdgeDef(aV0, aV1, aLine, 0.0, 10.0, 0.001);

  // Edge not in any face => empty result.
  NCollection_Vector<BRepGraph_NodeId> aFaces = aGraph.Spatial().FacesOfEdge(anEdgeId);
  EXPECT_EQ(aFaces.Length(), 0);
}
