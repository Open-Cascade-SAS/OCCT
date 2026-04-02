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
#include <BRepGraph_Iterator.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <utility>

#include <gtest/gtest.h>

// ============================================================
// Task 2A: Programmatic Node Addition API
// ============================================================

TEST(BRepGraph_BuilderTest, AddVertex_ReturnsValidId)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVtxId = aGraph.Builder().AddVertex(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_TRUE(aVtxId.IsValid());
  EXPECT_EQ(aVtxId.Index, 0);

  const BRepGraphInc::VertexDef& aVtxDef =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(0));
  EXPECT_NEAR(aVtxDef.Point.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Tolerance, 0.001, 1e-10);
}

TEST(BRepGraph_BuilderTest, AddEdge_WithCurve)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10.0, 0.0, 0.0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)));
  BRepGraph_EdgeId       anEdgeId = aGraph.Builder().AddEdge(aV1, aV2, aLine, 0.0, 10.0, 0.001);

  EXPECT_TRUE(anEdgeId.IsValid());

  const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(0));
  EXPECT_EQ(BRepGraph_Tool::Edge::StartVertex(aGraph, BRepGraph_EdgeId(0)).VertexDefId, aV1);
  EXPECT_EQ(BRepGraph_Tool::Edge::EndVertex(aGraph, BRepGraph_EdgeId(0)).VertexDefId, aV2);
  EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid());
  EXPECT_NEAR(anEdgeDef.ParamFirst, 0.0, 1e-10);
  EXPECT_NEAR(anEdgeDef.ParamLast, 10.0, 1e-10);
}

TEST(BRepGraph_BuilderTest, AddWire_ClosedRectangle)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_WireId aWireId = aGraph.Builder().AddWire(aEdges);
  EXPECT_TRUE(aWireId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountCoEdgeRefsOfWire(aGraph, BRepGraph_WireId(0)), 4);
  const BRepGraphInc::WireDef& aWireDef = aGraph.Topo().Wires().Definition(BRepGraph_WireId(0));
  EXPECT_TRUE(aWireDef.IsClosed);
}

TEST(BRepGraph_BuilderTest, AddFace_WithSurface)
{
  BRepGraph aGraph;

  // Build a simple rectangular face programmatically.
  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_WireId aWireId = aGraph.Builder().AddWire(aEdges);

  occ::handle<Geom_Plane>              aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  EXPECT_TRUE(aFaceId.IsValid());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

  const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Faces().Definition(BRepGraph_FaceId(0));
  EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid());
}

TEST(BRepGraph_BuilderTest, AddEdge_InvalidVertex_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVertexId = aGraph.Builder().AddVertex(gp_Pnt(0.0, 0.0, 0.0), 0.001);

  const BRepGraph_EdgeId anEdgeId = aGraph.Builder().AddEdge(aVertexId,
                                                             BRepGraph_VertexId(42),
                                                             occ::handle<Geom_Curve>(),
                                                             0.0,
                                                             1.0,
                                                             0.001);

  EXPECT_FALSE(anEdgeId.IsValid());
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
}

TEST(BRepGraph_BuilderTest, AddWire_InvalidEdge_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph aGraph;

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> anEdges;
  anEdges.Append({BRepGraph_EdgeId(17), TopAbs_FORWARD});

  const BRepGraph_WireId aWireId = aGraph.Builder().AddWire(anEdges);
  EXPECT_FALSE(aWireId.IsValid());
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().CoEdges().Nb(), 0);
}

TEST(BRepGraph_BuilderTest, AddFace_InvalidOuterWire_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph aGraph;

  occ::handle<Geom_Plane>              aPlane = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId               aFaceId =
    aGraph.Builder().AddFace(aPlane, BRepGraph_WireId(9), anInnerWires, 0.001);

  EXPECT_FALSE(aFaceId.IsValid());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
}

TEST(BRepGraph_BuilderTest, AddShellAndSolid)
{
  BRepGraph         aGraph;
  BRepGraph_ShellId aShellId = aGraph.Builder().AddShell();
  EXPECT_TRUE(aShellId.IsValid());

  BRepGraph_SolidId aSolidId = aGraph.Builder().AddSolid();
  EXPECT_TRUE(aSolidId.IsValid());
}

// ============================================================
// Task 2B: Incremental Build (AppendFlattenedShape)
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
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

  // Append second face.
  aGraph.Builder().AppendFlattenedShape(aCopy2.Shape());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_TRUE(aGraph.IsDone());
}

// ============================================================
// Task 2C: Soft Node Removal
// ============================================================

TEST(BRepGraph_BuilderTest, RemoveVertex_IsRemoved)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVtxId = aGraph.Builder().AddVertex(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aVtxId));

  aGraph.Builder().RemoveNode(aVtxId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVtxId));
}

TEST(BRepGraph_BuilderTest, RemoveFaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aFaceId));

  aGraph.Builder().RemoveNode(aFaceId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aFaceId));

  // Other faces should not be removed.
  for (int aFaceIdx = 1; aFaceIdx < aGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    EXPECT_FALSE(
      aGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx)));
  }
}

TEST(BRepGraph_BuilderTest, RemoveInvalidNode_NoError)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId anInvalidId;
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(anInvalidId));
  aGraph.Builder().RemoveNode(anInvalidId); // Should not crash.
}

TEST(BRepGraph_BuilderTest, RemoveAlreadyRemovedNode_NoError)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aVertexId = aGraph.Builder().AddVertex(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  ASSERT_TRUE(aVertexId.IsValid());

  aGraph.Builder().RemoveNode(aVertexId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVertexId));

  EXPECT_NO_THROW(aGraph.Builder().RemoveNode(aVertexId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVertexId));
}

// ============================================================
// Item 1: Complete Construction API (Shell/Solid linking)
// ============================================================

TEST(BRepGraph_BuilderTest, AddFaceToShell_CreatesUsage)
{
  BRepGraph aGraph;

  // Build a face programmatically.
  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_WireId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  // Create shell and link face to it.
  BRepGraph_ShellId         aShellId   = aGraph.Builder().AddShell();
  const BRepGraph_FaceRefId aFaceRefId = aGraph.Builder().AddFaceToShell(aShellId, aFaceId);
  EXPECT_TRUE(aFaceRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(aGraph, BRepGraph_ShellId(0)), 1);
}

TEST(BRepGraph_BuilderTest, AddFaceToShell_InvalidNodes_NoMutation)
{
  BRepGraph aGraph;

  const BRepGraph_ShellId   aShellId = aGraph.Builder().AddShell();
  const BRepGraph_FaceRefId aFaceRefId =
    aGraph.Builder().AddFaceToShell(aShellId, BRepGraph_FaceId(4));
  EXPECT_FALSE(aFaceRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(aGraph, BRepGraph_ShellId(aShellId.Index)),
            0);
}

TEST(BRepGraph_BuilderTest, AddShellToSolid_CreatesUsage)
{
  BRepGraph aGraph;

  BRepGraph_ShellId aShellId = aGraph.Builder().AddShell();
  BRepGraph_SolidId aSolidId = aGraph.Builder().AddSolid();

  const BRepGraph_ShellRefId aShellRefId = aGraph.Builder().AddShellToSolid(aSolidId, aShellId);
  EXPECT_TRUE(aShellRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(aGraph, BRepGraph_SolidId(0)), 1);
}

TEST(BRepGraph_BuilderTest, MutInvalidTopologyDefs_ThrowProgramError)
{
  BRepGraph aGraph;
#if !defined(No_Exception)
  EXPECT_THROW((void)aGraph.Builder().MutVertex(BRepGraph_VertexId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutEdge(BRepGraph_EdgeId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutWire(BRepGraph_WireId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutFace(BRepGraph_FaceId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutShell(BRepGraph_ShellId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutSolid(BRepGraph_SolidId(7)), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Builder().MutCoEdge(BRepGraph_CoEdgeId(7)), Standard_ProgramError);
#endif
}

TEST(BRepGraph_BuilderTest, AddCompound_WithChildren)
{
  BRepGraph aGraph;

  BRepGraph_SolidId aSolid1 = aGraph.Builder().AddSolid();
  BRepGraph_SolidId aSolid2 = aGraph.Builder().AddSolid();

  NCollection_Vector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid1);
  aChildren.Append(aSolid2);

  BRepGraph_CompoundId aCompId = aGraph.Builder().AddCompound(aChildren);
  EXPECT_TRUE(aCompId.IsValid());
  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1);

  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(aGraph, BRepGraph_CompoundId(0)), 2);
}

TEST(BRepGraph_BuilderTest, AddCompSolid_WithSolids)
{
  BRepGraph aGraph;

  BRepGraph_SolidId aSolid1 = aGraph.Builder().AddSolid();
  BRepGraph_SolidId aSolid2 = aGraph.Builder().AddSolid();

  NCollection_Vector<BRepGraph_SolidId> aSolids;
  aSolids.Append(aSolid1);
  aSolids.Append(aSolid2);

  BRepGraph_CompSolidId aCSolId = aGraph.Builder().AddCompSolid(aSolids);
  EXPECT_TRUE(aCSolId.IsValid());
  EXPECT_EQ(aGraph.Topo().CompSolids().Nb(), 1);

  const BRepGraphInc::CompSolidDef& aCSolDef =
    aGraph.Topo().CompSolids().Definition(BRepGraph_CompSolidId(0));
  (void)aCSolDef;
  EXPECT_EQ(BRepGraph_TestTools::CountSolidRefsOfCompSolid(aGraph, BRepGraph_CompSolidId(0)), 2);
}

TEST(BRepGraph_BuilderTest, FullSolid_ProgrammaticConstruction)
{
  BRepGraph aGraph;

  // Build a single-face shell solid.
  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_WireId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  BRepGraph_ShellId aShellId = aGraph.Builder().AddShell();
  aGraph.Builder().AddFaceToShell(aShellId, aFaceId);

  BRepGraph_SolidId aSolidId = aGraph.Builder().AddSolid();
  aGraph.Builder().AddShellToSolid(aSolidId, aShellId);

  // Verify the hierarchy.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

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
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const double anOrigTol = BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId(0));
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef =
      aGraph.Builder().MutFace(BRepGraph_FaceId(0));
    aFaceDef->Tolerance = 0.5;
  }
  EXPECT_NEAR(BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId(0)), 0.5, 1e-10);
  EXPECT_GT(aGraph.Topo().Faces().Definition(BRepGraph_FaceId(0)).OwnGen, 0u);
  (void)anOrigTol;
}

TEST(BRepGraph_BuilderTest, MutableShellDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Shells().Nb(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aShellDef =
      aGraph.Builder().MutShell(BRepGraph_ShellId(0));
  }
  EXPECT_GT(aGraph.Topo().Shells().Definition(BRepGraph_ShellId(0)).OwnGen, 0u);
}

TEST(BRepGraph_BuilderTest, MutableSolidDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Solids().Nb(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::SolidDef> aSolidDef =
      aGraph.Builder().MutSolid(BRepGraph_SolidId(0));
  }
  EXPECT_GT(aGraph.Topo().Solids().Definition(BRepGraph_SolidId(0)).OwnGen, 0u);
}

TEST(BRepGraph_BuilderTest, MutableCompoundDefinition)
{
  BRepGraph                            aGraph;
  NCollection_Vector<BRepGraph_NodeId> aChildren;
  (void)aGraph.Builder().AddCompound(aChildren);
  ASSERT_EQ(aGraph.Topo().Compounds().Nb(), 1);

  {
    BRepGraph_MutGuard<BRepGraphInc::CompoundDef> aCompDef =
      aGraph.Builder().MutCompound(BRepGraph_CompoundId(0));
  }
  EXPECT_GT(aGraph.Topo().Compounds().Definition(BRepGraph_CompoundId(0)).OwnGen, 0u);
}

TEST(BRepGraph_BuilderTest, MutableCompSolidDefinition)
{
  BRepGraph                             aGraph;
  NCollection_Vector<BRepGraph_SolidId> aSolids;
  (void)aGraph.Builder().AddCompSolid(aSolids);
  ASSERT_EQ(aGraph.Topo().CompSolids().Nb(), 1);

  {
    BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> aCSolDef =
      aGraph.Builder().MutCompSolid(BRepGraph_CompSolidId(0));
  }
  EXPECT_GT(aGraph.Topo().CompSolids().Definition(BRepGraph_CompSolidId(0)).OwnGen, 0u);
}

// ============================================================
// Item 3: Definition Traversal Skips Removed Nodes
// ============================================================

TEST(BRepGraph_BuilderTest, SkipsRemovedFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Remove 2 faces.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));

  int aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFaceDef = aFaceIt.Current();
    EXPECT_FALSE(aFaceDef.IsRemoved);
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
  const int aNbEdges = aGraph.Topo().Edges().Nb();
  ASSERT_GT(aNbEdges, 0);

  aGraph.Builder().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));

  int aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = anEdgeIt.Current();
    EXPECT_FALSE(anEdgeDef.IsRemoved);
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
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraphInc::VertexDef& aVertexDef = aVertexIt.Current();
    EXPECT_FALSE(aVertexDef.IsRemoved);
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

  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Builder().AddEdge(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Builder().AddEdge(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Builder().AddEdge(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Builder().AddEdge(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aEdges;
  aEdges.Append({aE0, TopAbs_FORWARD});
  aEdges.Append({aE1, TopAbs_FORWARD});
  aEdges.Append({aE2, TopAbs_FORWARD});
  aEdges.Append({aE3, TopAbs_FORWARD});

  BRepGraph_WireId                     aWireId = aGraph.Builder().AddWire(aEdges);
  occ::handle<Geom_Plane>              aPlane  = new Geom_Plane(gp_Pln());
  NCollection_Vector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId aFaceId = aGraph.Builder().AddFace(aPlane, aWireId, aInnerWires, 0.001);

  // Remove the face subgraph.
  aGraph.Builder().RemoveSubgraph(aFaceId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aFaceId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aWireId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE0));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE1));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE2));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE3));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV0));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV1));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV2));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV3));
}

TEST(BRepGraph_BuilderTest, RemoveSolid_CascadesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_SolidId aSolidId(0);
  aGraph.Builder().RemoveSubgraph(aSolidId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aSolidId));

  // All shells should be removed.
  for (int aIdx = 0; aIdx < aGraph.Topo().Shells().Nb(); ++aIdx)
    EXPECT_TRUE(
      aGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx)));

  // All faces should be removed.
  for (int aIdx = 0; aIdx < aGraph.Topo().Faces().Nb(); ++aIdx)
    EXPECT_TRUE(
      aGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx)));
}

TEST(BRepGraph_BuilderTest, RemoveSubgraph_SharedFace_PreservesSharedEdgesAndVertices)
{
  // Build a box (6 faces). Remove one face. Shared edges and vertices must
  // remain active because other faces still reference them.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbFaces    = aGraph.Topo().Faces().Nb();
  const int aNbEdges    = aGraph.Topo().Edges().Nb();
  const int aNbVertices = aGraph.Topo().Vertices().Nb();
  ASSERT_EQ(aNbFaces, 6);
  ASSERT_EQ(aNbEdges, 12);
  ASSERT_EQ(aNbVertices, 8);

  // Remove face 0.
  const BRepGraph_FaceId aRemovedFace(0);
  aGraph.Builder().RemoveSubgraph(aRemovedFace);

  // The removed face must be removed.
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aRemovedFace));

  // Other faces must remain active.
  for (int aIdx = 1; aIdx < aNbFaces; ++aIdx)
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_FaceId(aIdx)));

  // All 12 edges of a box are shared by exactly 2 faces.
  // Removing 1 face leaves each shared edge with at least 1 active parent face.
  // Therefore all edges must remain active.
  for (int aIdx = 0; aIdx < aNbEdges; ++aIdx)
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_EdgeId(aIdx)));

  // All 8 vertices of a box are shared by 3 edges. All edges are still active,
  // so all vertices must remain active.
  for (int aIdx = 0; aIdx < aNbVertices; ++aIdx)
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_VertexId(aIdx)));
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
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().Edges().Nb(); ++anEdgeIdx)
  {
    BRepGraph_EdgeId                     anEdgeId(anEdgeIdx);
    NCollection_Vector<BRepGraph_FaceId> aFaces = aGraph.Topo().Edges().Faces(anEdgeId);
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
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Count total shared edge pairs across all face pairs.
  int aSharingPairs = 0;
  for (int aFaceA = 0; aFaceA < aGraph.Topo().Faces().Nb(); ++aFaceA)
  {
    for (int aFaceB = aFaceA + 1; aFaceB < aGraph.Topo().Faces().Nb(); ++aFaceB)
    {
      NCollection_Vector<BRepGraph_EdgeId> aShared =
        aGraph.Topo().Faces().SharedEdges(BRepGraph_FaceId(aFaceA),
                                          BRepGraph_FaceId(aFaceB),
                                          aGraph.Allocator());
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
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Each face of a box is adjacent to 4 other faces.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    BRepGraph_FaceId                     aFaceId(aFaceIdx);
    NCollection_Vector<BRepGraph_FaceId> aAdj =
      aGraph.Topo().Faces().Adjacent(aFaceId, aGraph.Allocator());
    EXPECT_EQ(aAdj.Length(), 4) << "Face " << aFaceIdx << " has " << aAdj.Length()
                                << " adjacent faces";
  }
}

TEST(BRepGraph_BuilderTest, FacesOfEdge_NoFaces_Programmatic)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(10, 0, 0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  BRepGraph_EdgeId       anEdgeId = aGraph.Builder().AddEdge(aV0, aV1, aLine, 0.0, 10.0, 0.001);

  // Edge not in any face => empty result.
  const NCollection_Vector<BRepGraph_FaceId>& aFaces = aGraph.Topo().Edges().Faces(anEdgeId);
  EXPECT_EQ(aFaces.Length(), 0);
}

// ============ Topology adjacency methods ============

TEST(BRepGraph_BuilderTest, EdgesOfFace_Box_HasEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box face has 4 edges (rectangular loop).
  int aNbEdges = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Edge);
       anExp.More();
       anExp.Next())
    ++aNbEdges;
  EXPECT_EQ(aNbEdges, 4);
}

TEST(BRepGraph_BuilderTest, VerticesOfEdge_Box_HasTwoVertices)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aNbVertices = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_EdgeId(0), BRepGraph_NodeId::Kind::Vertex);
       anExp.More();
       anExp.Next())
    ++aNbVertices;
  EXPECT_EQ(aNbVertices, 2);
}

TEST(BRepGraph_BuilderTest, EdgesOfVertex_Box_ThreeEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box corner vertex is shared by 3 edges.
  const NCollection_Vector<BRepGraph_EdgeId>& aEdges =
    aGraph.Topo().Vertices().Edges(BRepGraph_VertexId(0));
  EXPECT_EQ(aEdges.Length(), 3);
}

TEST(BRepGraph_BuilderTest, AdjacentEdges_Box_SharedVertex)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Box edge shares 2 vertices, each with 3 incident edges.
  // Adjacent = (3 - 1) + (3 - 1) - overlap = at least 4 adjacent edges.
  NCollection_Vector<BRepGraph_EdgeId> aAdj =
    aGraph.Topo().Edges().Adjacent(BRepGraph_EdgeId(0), aGraph.Allocator());
  EXPECT_GE(aAdj.Length(), 4);
}

TEST(BRepGraph_BuilderTest, NbFacesOfEdge_Box_TwoFaces)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Every box edge is shared by exactly 2 faces (manifold).
  EXPECT_EQ(aGraph.Topo().Edges().NbFaces(BRepGraph_EdgeId(0)), 2);
}

TEST(BRepGraph_BuilderTest, IsManifoldEdge_Box_True)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_TRUE(aGraph.Topo().Edges().IsManifold(BRepGraph_EdgeId(0)));
  EXPECT_FALSE(aGraph.Topo().Edges().IsBoundary(BRepGraph_EdgeId(0)));
}

TEST(BRepGraph_BuilderTest, InvalidInput_ReturnsEmpty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Out-of-range typed ids return empty results.
  EXPECT_EQ(aGraph.Topo().Vertices().Edges(BRepGraph_VertexId(999)).Length(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Adjacent(BRepGraph_EdgeId(999), aGraph.Allocator()).Length(), 0);
}
