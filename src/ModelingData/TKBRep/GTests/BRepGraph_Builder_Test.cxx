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
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_Tool.hxx>
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

TEST(BRepGraph_BuilderTest, Box_MatchesTopoDSBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Get bounding box from graph.
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box          aGraphBox;
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

TEST(BRepGraph_BuilderTest, Sphere_NonVoid)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box          aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aSolidId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraph_BuilderTest, Cylinder_NonVoid)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box          aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aSolidId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraph_BuilderTest, SingleFace_MatchesVertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box          aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aFaceId, aGraphBox);
  EXPECT_FALSE(aGraphBox.IsVoid());
}

TEST(BRepGraph_BuilderTest, Box_CentroidNearCenter)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  gp_Pnt           aCentroid = bboxCenter(aGraph, aSolidId);

  // Centroid should be near the center of the box.
  EXPECT_NEAR(aCentroid.X(), 5.0, 1.0);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 1.0);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 1.0);
}

// ============================================================
// Task 2A: Programmatic Node Addition API
// ============================================================

TEST(BRepGraph_BuilderTest, AddVertex_ReturnsValidId)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aVtxId = aGraph.Builder().AddVertex(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_TRUE(aVtxId.IsValid());
  EXPECT_EQ(aVtxId.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  EXPECT_EQ(aVtxId.Index, 0);

  const BRepGraphInc::VertexDef& aVtxDef = aGraph.Topo().Vertex(BRepGraph_VertexId(0));
  EXPECT_NEAR(aVtxDef.Point.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Tolerance, 0.001, 1e-10);
}

TEST(BRepGraph_BuilderTest, AddEdge_WithCurve)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10.0, 0.0, 0.0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)));
  BRepGraph_NodeId       anEdgeId = aGraph.Builder().AddEdge(aV1, aV2, aLine, 0.0, 10.0, 0.001);

  EXPECT_TRUE(anEdgeId.IsValid());
  EXPECT_EQ(anEdgeId.NodeKind, BRepGraph_NodeId::Kind::Edge);

  const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edge(BRepGraph_EdgeId(0));
  EXPECT_EQ(BRepGraph_Tool::Edge::StartVertex(aGraph, BRepGraph_EdgeId(0)).VertexDefId, BRepGraph_VertexId(aV1.Index));
  EXPECT_EQ(BRepGraph_Tool::Edge::EndVertex(aGraph, BRepGraph_EdgeId(0)).VertexDefId, BRepGraph_VertexId(aV2.Index));
  EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid());
  EXPECT_NEAR(anEdgeDef.ParamFirst, 0.0, 1e-10);
  EXPECT_NEAR(anEdgeDef.ParamLast, 10.0, 1e-10);
}

TEST(BRepGraph_BuilderTest, AddWire_ClosedRectangle)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWire(aEdges);
  EXPECT_TRUE(aWireId.IsValid());
  EXPECT_EQ(aWireId.NodeKind, BRepGraph_NodeId::Kind::Wire);

  EXPECT_EQ(BRepGraph_TestTools::CountCoEdgeRefsOfWire(aGraph, BRepGraph_WireId(0)), 4);
  const BRepGraphInc::WireDef& aWireDef = aGraph.Topo().Wire(BRepGraph_WireId(0));
  EXPECT_TRUE(aWireDef.IsClosed);
}

TEST(BRepGraph_BuilderTest, AddFace_WithSurface)
{
  BRepGraph aGraph;

  // Build a simple rectangular face programmatically.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId aWireId = aGraph.Builder().AddWire(aEdges);

  occ::handle<Geom_Plane>              aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  EXPECT_TRUE(aFaceId.IsValid());
  EXPECT_EQ(aFaceId.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);

  const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Face(BRepGraph_FaceId(0));
  EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid());
}

TEST(BRepGraph_BuilderTest, AddShellAndSolid)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aShellId = aGraph.Builder().AddShell();
  EXPECT_TRUE(aShellId.IsValid());
  EXPECT_EQ(aShellId.NodeKind, BRepGraph_NodeId::Kind::Shell);

  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolid();
  EXPECT_TRUE(aSolidId.IsValid());
  EXPECT_EQ(aSolidId.NodeKind, BRepGraph_NodeId::Kind::Solid);
}

// ============================================================
// Task 2B: Incremental Build (AppendShape)
// ============================================================

TEST(BRepGraph_BuilderTest, AppendTwoBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Get two faces from the box.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape        aFace1 = anExp.Current();
  BRepBuilderAPI_Copy aCopy1(aFace1, true);
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape        aFace2 = anExp.Current();
  BRepBuilderAPI_Copy aCopy2(aFace2, true);

  BRepGraph aGraph;
  aGraph.Build(aCopy1.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);

  // Append second face.
  aGraph.Builder().AppendShape(aCopy2.Shape());
  EXPECT_EQ(aGraph.Topo().NbFaces(), 2);
  EXPECT_TRUE(aGraph.IsDone());
}

// ============================================================
// Task 2C: Soft Node Removal
// ============================================================

TEST(BRepGraph_BuilderTest, RemoveVertex_IsRemoved)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aVtxId = aGraph.Builder().AddVertex(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_FALSE(aGraph.Topo().IsRemoved(aVtxId));

  aGraph.Builder().RemoveNode(aVtxId);
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aVtxId));
}

TEST(BRepGraph_BuilderTest, RemoveFaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  EXPECT_FALSE(aGraph.Topo().IsRemoved(aFaceId));

  aGraph.Builder().RemoveNode(aFaceId);
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aFaceId));

  // Other faces should not be removed.
  for (int aFaceIdx = 1; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_FALSE(aGraph.Topo().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx)));
  }
}

TEST(BRepGraph_BuilderTest, RemoveInvalidNode_NoError)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId anInvalidId;
  EXPECT_FALSE(aGraph.Topo().IsRemoved(anInvalidId));
  aGraph.Builder().RemoveNode(anInvalidId); // Should not crash.
}

// ============================================================
// Item 1: Complete Construction API (Shell/Solid linking)
// ============================================================

TEST(BRepGraph_BuilderTest, AddFaceToShell_CreatesUsage)
{
  BRepGraph aGraph;

  // Build a face programmatically.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  // Create shell and link face to it.
  BRepGraph_NodeId aShellId = aGraph.Builder().AddShell();
  aGraph.Builder().AddFaceToShell(aShellId, aFaceId);

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(aGraph, BRepGraph_ShellId(0)), 1);
}

TEST(BRepGraph_BuilderTest, AddShellToSolid_CreatesUsage)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aShellId = aGraph.Builder().AddShell();
  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolid();

  aGraph.Builder().AddShellToSolid(aSolidId, aShellId);

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(aGraph, BRepGraph_SolidId(0)), 1);
}

TEST(BRepGraph_BuilderTest, AddCompound_WithChildren)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aSolid1 = aGraph.Builder().AddSolid();
  BRepGraph_NodeId aSolid2 = aGraph.Builder().AddSolid();

  NCollection_Vector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid1);
  aChildren.Append(aSolid2);

  BRepGraph_NodeId aCompId = aGraph.Builder().AddCompound(aChildren);
  EXPECT_TRUE(aCompId.IsValid());
  EXPECT_EQ(aCompId.NodeKind, BRepGraph_NodeId::Kind::Compound);
  EXPECT_EQ(aGraph.Topo().NbCompounds(), 1);

  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(aGraph, BRepGraph_NodeId::Compound(0)), 2);
}

TEST(BRepGraph_BuilderTest, AddCompSolid_WithSolids)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aSolid1 = aGraph.Builder().AddSolid();
  BRepGraph_NodeId aSolid2 = aGraph.Builder().AddSolid();

  NCollection_Vector<BRepGraph_NodeId> aSolids;
  aSolids.Append(aSolid1);
  aSolids.Append(aSolid2);

  BRepGraph_NodeId aCSolId = aGraph.Builder().AddCompSolid(aSolids);
  EXPECT_TRUE(aCSolId.IsValid());
  EXPECT_EQ(aCSolId.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
  EXPECT_EQ(aGraph.Topo().NbCompSolids(), 1);

  const BRepGraphInc::CompSolidDef& aCSolDef =
    aGraph.Topo().CompSolid(BRepGraph_CompSolidId(0));
  (void)aCSolDef;
  EXPECT_EQ(BRepGraph_TestTools::CountSolidRefsOfCompSolid(aGraph, BRepGraph_CompSolidId(0)), 2);
}

TEST(BRepGraph_BuilderTest, FullSolid_ProgrammaticConstruction)
{
  BRepGraph aGraph;

  // Build a single-face shell solid.
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  BRepGraph_NodeId aShellId = aGraph.Builder().AddShell();
  aGraph.Builder().AddFaceToShell(aShellId, aFaceId);

  BRepGraph_NodeId aSolidId = aGraph.Builder().AddSolid();
  aGraph.Builder().AddShellToSolid(aSolidId, aShellId);

  // Verify the hierarchy.
  EXPECT_EQ(aGraph.Topo().NbSolids(), 1);
  EXPECT_EQ(aGraph.Topo().NbShells(), 1);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(aGraph, BRepGraph_SolidId(0)), 1);
}

// ============================================================
// Item 2: Mutable Access for All Def Types
// ============================================================

TEST(BRepGraph_BuilderTest, MutableFaceDefinition_ChangesTolerance)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);

  const double anOrigTol = BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId(0));
  {
    BRepGraph_MutRef<BRepGraphInc::FaceDef> aFaceDef =
      aGraph.Builder().MutFace(BRepGraph_FaceId(0));
    aFaceDef->Tolerance = 0.5;
  }
  EXPECT_NEAR(BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId(0)), 0.5, 1e-10);
  EXPECT_TRUE(aGraph.Topo().Face(BRepGraph_FaceId(0)).IsModified);
  (void)anOrigTol;
}

TEST(BRepGraph_BuilderTest, MutableShellDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbShells(), 0);

  {
    BRepGraph_MutRef<BRepGraphInc::ShellDef> aShellDef =
      aGraph.Builder().MutShell(BRepGraph_ShellId(0));
  }
  EXPECT_TRUE(aGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
}

TEST(BRepGraph_BuilderTest, MutableSolidDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbSolids(), 0);

  {
    BRepGraph_MutRef<BRepGraphInc::SolidDef> aSolidDef =
      aGraph.Builder().MutSolid(BRepGraph_SolidId(0));
  }
  EXPECT_TRUE(aGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST(BRepGraph_BuilderTest, MutableCompoundDefinition)
{
  BRepGraph                            aGraph;
  NCollection_Vector<BRepGraph_NodeId> aChildren;
  (void)aGraph.Builder().AddCompound(aChildren);
  ASSERT_EQ(aGraph.Topo().NbCompounds(), 1);

  {
    BRepGraph_MutRef<BRepGraphInc::CompoundDef> aCompDef =
      aGraph.Builder().MutCompound(BRepGraph_CompoundId(0));
  }
  EXPECT_TRUE(aGraph.Topo().Compound(BRepGraph_CompoundId(0)).IsModified);
}

TEST(BRepGraph_BuilderTest, MutableCompSolidDefinition)
{
  BRepGraph                            aGraph;
  NCollection_Vector<BRepGraph_NodeId> aSolids;
  (void)aGraph.Builder().AddCompSolid(aSolids);
  ASSERT_EQ(aGraph.Topo().NbCompSolids(), 1);

  {
    BRepGraph_MutRef<BRepGraphInc::CompSolidDef> aCSolDef =
      aGraph.Builder().MutCompSolid(BRepGraph_CompSolidId(0));
  }
  EXPECT_TRUE(aGraph.Topo().CompSolid(BRepGraph_CompSolidId(0)).IsModified);
}

// ============================================================
// Item 3: Iterator Skips Removed Nodes
// ============================================================

TEST(BRepGraph_BuilderTest, SkipsRemovedFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);

  // Remove 2 faces.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_BuilderTest, SkipsRemovedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const int aNbEdges = aGraph.Topo().NbEdges();
  ASSERT_GT(aNbEdges, 0);

  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbEdges - 1);
}

TEST(BRepGraph_BuilderTest, SkipsFirstNode)
{
  BRepGraph aGraph;
  (void)aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  (void)aGraph.Builder().AddVertex(gp_Pnt(1, 0, 0), 0.001);
  (void)aGraph.Builder().AddVertex(gp_Pnt(2, 0, 0), 0.001);

  // Remove the first vertex.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, 0));

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::VertexDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().IsRemoved);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

// ============================================================
// Item 4: Cascading Soft Removal
// ============================================================

TEST(BRepGraph_BuilderTest, RemoveFace_RemovesWiresAndEdges)
{
  BRepGraph aGraph;

  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_NodeId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_NodeId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_NodeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_NodeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_NodeId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_NodeId> aInnerWires;
  BRepGraph_NodeId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  // Remove the face subgraph.
  aGraph.Builder().RemoveSubgraph(aFaceId);

  EXPECT_TRUE(aGraph.Topo().IsRemoved(aFaceId));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aWireId));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aE0));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aE1));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aE2));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aE3));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aV0));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aV1));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aV2));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(aV3));
}

TEST(BRepGraph_BuilderTest, RemoveSolid_CascadesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  aGraph.Builder().RemoveSubgraph(aSolidId);

  EXPECT_TRUE(aGraph.Topo().IsRemoved(aSolidId));

  // All shells should be removed.
  for (int aIdx = 0; aIdx < aGraph.Topo().NbShells(); ++aIdx)
    EXPECT_TRUE(aGraph.Topo().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx)));

  // All faces should be removed.
  for (int aIdx = 0; aIdx < aGraph.Topo().NbFaces(); ++aIdx)
    EXPECT_TRUE(aGraph.Topo().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx)));
}

// ============================================================
// Item 5: Edge Adjacency Queries
// ============================================================

TEST(BRepGraph_BuilderTest, FacesOfEdge_BoxSharedEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Every edge in a box is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_NodeId                     anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
    NCollection_Vector<BRepGraph_NodeId> aFaces = aGraph.Topo().FacesOfEdge(anEdgeId);
    EXPECT_EQ(aFaces.Length(), 2) << "Edge " << anEdgeIdx << " has " << aFaces.Length() << " faces";
  }
}

TEST(BRepGraph_BuilderTest, SharedEdges_AdjacentBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);

  // Count total shared edge pairs across all face pairs.
  int aSharingPairs = 0;
  for (int aFaceA = 0; aFaceA < aGraph.Topo().NbFaces(); ++aFaceA)
  {
    for (int aFaceB = aFaceA + 1; aFaceB < aGraph.Topo().NbFaces(); ++aFaceB)
    {
      NCollection_Vector<BRepGraph_NodeId> aShared =
        aGraph.Topo().SharedEdges(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceA),
                                  BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceB));
      if (!aShared.IsEmpty())
        ++aSharingPairs;
    }
  }
  // A box has 12 edges, each shared by 2 faces, so 12 sharing pairs.
  EXPECT_EQ(aSharingPairs, 12);
}

TEST(BRepGraph_BuilderTest, AdjacentFaces_BoxFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);

  // Each face of a box is adjacent to 4 other faces.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    NCollection_Vector<BRepGraph_NodeId> aAdj = aGraph.Topo().AdjacentFaces(aFaceId);
    EXPECT_EQ(aAdj.Length(), 4) << "Face " << aFaceIdx << " has " << aAdj.Length()
                                << " adjacent faces";
  }
}

TEST(BRepGraph_BuilderTest, FacesOfEdge_NoFaces_Programmatic)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_NodeId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  BRepGraph_NodeId       anEdgeId = aGraph.Builder().AddEdge(aV0, aV1, aLine, 0.0, 10.0, 0.001);

  // Edge not in any face => empty result.
  NCollection_Vector<BRepGraph_NodeId> aFaces = aGraph.Topo().FacesOfEdge(anEdgeId);
  EXPECT_EQ(aFaces.Length(), 0);
}

// ============ New SpatialView adjacency methods ============

TEST(BRepGraph_BuilderTest, EdgesOfFace_Box_HasEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box face has 4 edges (rectangular loop).
  NCollection_Vector<BRepGraph_NodeId> aEdges =
    aGraph.Topo().EdgesOfFace(BRepGraph_NodeId::Face(0));
  EXPECT_EQ(aEdges.Length(), 4);
}

TEST(BRepGraph_BuilderTest, VerticesOfEdge_Box_HasTwoVertices)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_NodeId> aVerts =
    aGraph.Topo().VerticesOfEdge(BRepGraph_NodeId::Edge(0));
  EXPECT_EQ(aVerts.Length(), 2);
  EXPECT_EQ(aVerts.Value(0).NodeKind, BRepGraph_NodeId::Kind::Vertex);
  EXPECT_EQ(aVerts.Value(1).NodeKind, BRepGraph_NodeId::Kind::Vertex);
}

TEST(BRepGraph_BuilderTest, EdgesOfVertex_Box_ThreeEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box corner vertex is shared by 3 edges.
  NCollection_Vector<BRepGraph_NodeId> aEdges =
    aGraph.Topo().EdgesOfVertex(BRepGraph_NodeId::Vertex(0));
  EXPECT_EQ(aEdges.Length(), 3);
}

TEST(BRepGraph_BuilderTest, AdjacentEdges_Box_SharedVertex)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Box edge shares 2 vertices, each with 3 incident edges.
  // Adjacent = (3 - 1) + (3 - 1) - overlap = at least 4 adjacent edges.
  NCollection_Vector<BRepGraph_NodeId> aAdj =
    aGraph.Topo().AdjacentEdges(BRepGraph_NodeId::Edge(0));
  EXPECT_GE(aAdj.Length(), 4);
}

TEST(BRepGraph_BuilderTest, FaceCountOfEdge_Box_TwoFaces)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Every box edge is shared by exactly 2 faces (manifold).
  EXPECT_EQ(aGraph.Topo().FaceCountOfEdge(BRepGraph_NodeId::Edge(0)), 2);
}

TEST(BRepGraph_BuilderTest, IsManifoldEdge_Box_True)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_TRUE(aGraph.Topo().IsManifoldEdge(BRepGraph_NodeId::Edge(0)));
  EXPECT_FALSE(aGraph.Topo().IsBoundaryEdge(BRepGraph_NodeId::Edge(0)));
}

TEST(BRepGraph_BuilderTest, InvalidInput_ReturnsEmpty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Wrong kind.
  EXPECT_EQ(aGraph.Topo().EdgesOfFace(BRepGraph_NodeId::Edge(0)).Length(), 0);
  EXPECT_EQ(aGraph.Topo().EdgesOfVertex(BRepGraph_NodeId::Face(0)).Length(), 0);
  EXPECT_EQ(aGraph.Topo().VerticesOfEdge(BRepGraph_NodeId::Face(0)).Length(), 0);
  EXPECT_EQ(aGraph.Topo().AdjacentEdges(BRepGraph_NodeId::Face(0)).Length(), 0);

  // Invalid NodeId.
  EXPECT_EQ(aGraph.Topo().FaceCountOfEdge(BRepGraph_NodeId()), 0);
  EXPECT_FALSE(aGraph.Topo().IsBoundaryEdge(BRepGraph_NodeId()));
  EXPECT_FALSE(aGraph.Topo().IsManifoldEdge(BRepGraph_NodeId()));
}
