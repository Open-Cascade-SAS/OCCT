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

#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>

#include <gtest/gtest.h>

class BRepGraph_ConvenienceTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
  }

  BRepGraph myGraph;
};

// ---------- Part A: NodeId Static Factories ----------

TEST_F(BRepGraph_ConvenienceTest, NodeId_Factories_CorrectKindAndIndex)
{
  const BRepGraph_NodeId aSolid = BRepGraph_NodeId::Solid(3);
  EXPECT_EQ(aSolid.NodeKind, BRepGraph_NodeId::Kind::Solid);
  EXPECT_EQ(aSolid.Index, 3);

  const BRepGraph_NodeId aShell = BRepGraph_NodeId::Shell(5);
  EXPECT_EQ(aShell.NodeKind, BRepGraph_NodeId::Kind::Shell);
  EXPECT_EQ(aShell.Index, 5);

  const BRepGraph_NodeId aFace = BRepGraph_NodeId::Face(0);
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aFace.Index, 0);

  const BRepGraph_NodeId aWire = BRepGraph_NodeId::Wire(2);
  EXPECT_EQ(aWire.NodeKind, BRepGraph_NodeId::Kind::Wire);
  EXPECT_EQ(aWire.Index, 2);

  const BRepGraph_NodeId anEdge = BRepGraph_NodeId::Edge(7);
  EXPECT_EQ(anEdge.NodeKind, BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anEdge.Index, 7);

  const BRepGraph_NodeId aVertex = BRepGraph_NodeId::Vertex(1);
  EXPECT_EQ(aVertex.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  EXPECT_EQ(aVertex.Index, 1);

  const BRepGraph_NodeId aCompound = BRepGraph_NodeId::Compound(0);
  EXPECT_EQ(aCompound.NodeKind, BRepGraph_NodeId::Kind::Compound);
  EXPECT_EQ(aCompound.Index, 0);

  const BRepGraph_NodeId aCompSolid = BRepGraph_NodeId::CompSolid(0);
  EXPECT_EQ(aCompSolid.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
  EXPECT_EQ(aCompSolid.Index, 0);
}

TEST_F(BRepGraph_ConvenienceTest, NodeId_Factories_EqualToConstructor)
{
  EXPECT_EQ(BRepGraph_NodeId::Face(3), BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));
  EXPECT_EQ(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));
}

// ---------- Part B: EdgeDef Vertex Access via BRepGraph_Tool ----------

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_StartVertex_Valid)
{
  ASSERT_GT(myGraph.Topo().NbEdges(), 0);
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraphInc::VertexRefEntry& aStart =
    BRepGraph_Tool::Edge::StartVertex(myGraph, anEdgeId);
  EXPECT_TRUE(aStart.VertexEntityId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_EndVertex_Valid)
{
  ASSERT_GT(myGraph.Topo().NbEdges(), 0);
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraphInc::VertexRefEntry& anEnd =
    BRepGraph_Tool::Edge::EndVertex(myGraph, anEdgeId);
  EXPECT_TRUE(anEnd.VertexEntityId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_StartEnd_DifferForNonClosed)
{
  ASSERT_GT(myGraph.Topo().NbEdges(), 0);
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdge = myGraph.Topo().Edge(anEdgeId);
  if (!anEdge.IsClosed)
  {
    const BRepGraph_VertexId aStartId =
      BRepGraph_Tool::Edge::StartVertex(myGraph, anEdgeId).VertexEntityId;
    const BRepGraph_VertexId anEndId =
      BRepGraph_Tool::Edge::EndVertex(myGraph, anEdgeId).VertexEntityId;
    EXPECT_NE(aStartId, anEndId);
  }
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_RefIds_AreValid)
{
  ASSERT_GT(myGraph.Topo().NbEdges(), 0);
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdge = myGraph.Topo().Edge(anEdgeId);
  EXPECT_TRUE(anEdge.StartVertexRefId.IsValid());
  EXPECT_TRUE(anEdge.EndVertexRefId.IsValid());
}

// ---------- Part D: FaceDef::Surface ----------

TEST_F(BRepGraph_ConvenienceTest, FaceSurface_Valid)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  ASSERT_GT(aDefs.NbFaces(), 0);
  EXPECT_TRUE(aDefs.Face(BRepGraph_FaceId(0)).SurfaceRepId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, FaceSurface_AllBoxFaces)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  for (int aFaceIter = 0; aFaceIter < aDefs.NbFaces(); ++aFaceIter)
  {
    EXPECT_TRUE(aDefs.Face(BRepGraph_FaceId(aFaceIter)).SurfaceRepId.IsValid())
      << "Face " << aFaceIter << " has no surface";
  }
}

// ---------- Part E: DefsView::FindPCurve ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurve_ValidPair)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();

  // Find an edge/face pair that has a PCurve.
  for (int aFaceIter = 0; aFaceIter < aDefs.NbFaces(); ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceNodeId = BRepGraph_NodeId::Face(aFaceIter);

    for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
    {
      const BRepGraphInc::EdgeDef& anEdgeDef = aDefs.Edge(BRepGraph_EdgeId(anEdgeIter));
      const BRepGraphInc::CoEdgeDef*  aPCurve   = aDefs.FindPCurve(anEdgeDef.Id, aFaceNodeId);
      if (aPCurve != nullptr)
      {
        EXPECT_TRUE(aPCurve->Curve2DRepId.IsValid());
        return;
      }
    }
  }
}

TEST_F(BRepGraph_ConvenienceTest, FindPCurve_InvalidPair_ReturnsNull)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  EXPECT_EQ(aDefs.FindPCurve(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId::Face(9999)), nullptr);
}

// ---------- Part F: DefsView::NbShellFaces / ShellFaceEntity ----------

TEST_F(BRepGraph_ConvenienceTest, NbShellFaces_Box_SixFaces)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  ASSERT_EQ(aDefs.NbShells(), 1);
  EXPECT_EQ(aDefs.NbShellFaces(BRepGraph_ShellId(0)), 6);
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceEntity_AllValid)
{
  const BRepGraph::TopoView aDefs    = myGraph.Topo();
  const int                 aNbFaces = aDefs.NbShellFaces(BRepGraph_ShellId(0));
  for (int aFaceIter = 0; aFaceIter < aNbFaces; ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceEntityId = aDefs.ShellFaceEntity(BRepGraph_ShellId(0), aFaceIter);
    EXPECT_TRUE(aFaceEntityId.IsValid()) << "Shell face " << aFaceIter;
    EXPECT_EQ(aFaceEntityId.NodeKind, BRepGraph_NodeId::Kind::Face);
  }
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceEntity_OutOfRange_Invalid)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  EXPECT_FALSE(aDefs.ShellFaceEntity(BRepGraph_ShellId(0), -1).IsValid());
  EXPECT_FALSE(aDefs.ShellFaceEntity(BRepGraph_ShellId(0), 100).IsValid());
}

// ---------- Integration: Cylinder with seam edge ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurve_WithOrientation_SeamEdge)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::TopoView aDefs = aGraph.Topo();

  // Look for seam edges (coedge with SeamPairId valid).
  for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aDefs.Edge(BRepGraph_EdgeId(anEdgeIter));
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aDefs.CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIter));

    for (int aCEI = 0; aCEI < aCoEdgeIdxs.Length(); ++aCEI)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aDefs.CoEdge(aCoEdgeIdxs.Value(aCEI));
      if (!aCE.SeamPairId.IsValid())
        continue;

      // Found seam edge - verify FindPCurve returns distinct entries for each orientation.
      const BRepGraph_NodeId            aFaceEntityId = aCE.FaceEntityId;
      const BRepGraphInc::CoEdgeDef* aPCF =
        aDefs.FindPCurve(anEdgeDef.Id, aFaceEntityId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeDef* aPCR =
        aDefs.FindPCurve(anEdgeDef.Id, aFaceEntityId, TopAbs_REVERSED);
      EXPECT_NE(aPCF, nullptr);
      EXPECT_NE(aPCR, nullptr);
      if (aPCF != nullptr && aPCR != nullptr)
      {
        EXPECT_NE(aPCF, aPCR);
      }
      return;
    }
  }
}
