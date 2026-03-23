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
#include <BRepGraph_DefsView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>

#include <gtest/gtest.h>

class BRepGraphConvenienceTest : public testing::Test
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

TEST_F(BRepGraphConvenienceTest, NodeId_Factories_CorrectKindAndIndex)
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

TEST_F(BRepGraphConvenienceTest, NodeId_Factories_EqualToConstructor)
{
  EXPECT_EQ(BRepGraph_NodeId::Face(3),
            BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));
  EXPECT_EQ(BRepGraph_NodeId::Edge(0),
            BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));
}

// ---------- Part B: EdgeDef Oriented Vertices ----------

TEST_F(BRepGraphConvenienceTest, EdgeDef_OrientedStartVertex_Forward)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  ASSERT_GT(aDefs.NbEdges(), 0);
  const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(0);
  EXPECT_EQ(anEdge.OrientedStartVertex(TopAbs_FORWARD), anEdge.StartVertexDefId());
}

TEST_F(BRepGraphConvenienceTest, EdgeDef_OrientedStartVertex_Reversed)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(0);
  EXPECT_EQ(anEdge.OrientedStartVertex(TopAbs_REVERSED), anEdge.EndVertexDefId());
}

TEST_F(BRepGraphConvenienceTest, EdgeDef_OrientedEndVertex_Forward)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(0);
  EXPECT_EQ(anEdge.OrientedEndVertex(TopAbs_FORWARD), anEdge.EndVertexDefId());
}

TEST_F(BRepGraphConvenienceTest, EdgeDef_OrientedEndVertex_Reversed)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(0);
  EXPECT_EQ(anEdge.OrientedEndVertex(TopAbs_REVERSED), anEdge.StartVertexDefId());
}

TEST_F(BRepGraphConvenienceTest, EdgeDef_OrientedVertex_Internal_Invalid)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(0);
  EXPECT_FALSE(anEdge.OrientedStartVertex(TopAbs_INTERNAL).IsValid());
  EXPECT_FALSE(anEdge.OrientedEndVertex(TopAbs_INTERNAL).IsValid());
}

// ---------- Part D: FaceDef::Surface ----------

TEST_F(BRepGraphConvenienceTest, FaceSurface_Valid)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  ASSERT_GT(aDefs.NbFaces(), 0);
  EXPECT_FALSE(aDefs.Face(0).Surface.IsNull());
}

TEST_F(BRepGraphConvenienceTest, FaceSurface_AllBoxFaces)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  for (int aFaceIter = 0; aFaceIter < aDefs.NbFaces(); ++aFaceIter)
  {
    EXPECT_FALSE(aDefs.Face(aFaceIter).Surface.IsNull())
      << "Face " << aFaceIter << " has no surface";
  }
}

// ---------- Part E: DefsView::FindPCurve ----------

TEST_F(BRepGraphConvenienceTest, FindPCurve_ValidPair)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();

  // Find an edge/face pair that has a PCurve.
  for (int aFaceIter = 0; aFaceIter < aDefs.NbFaces(); ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceNodeId = BRepGraph_NodeId::Face(aFaceIter);

    for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
    {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeIter);
      const BRepGraphInc::CoEdgeEntity* aPCurve =
        aDefs.FindPCurve(anEdgeDef.Id, aFaceNodeId);
      if (aPCurve != nullptr)
      {
        EXPECT_FALSE(aPCurve->Curve2d.IsNull());
        return;
      }
    }
  }
}

TEST_F(BRepGraphConvenienceTest, FindPCurve_InvalidPair_ReturnsNull)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  EXPECT_EQ(aDefs.FindPCurve(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId::Face(9999)), nullptr);
}

// ---------- Part F: DefsView::NbShellFaces / ShellFaceDef ----------

TEST_F(BRepGraphConvenienceTest, NbShellFaces_Box_SixFaces)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  ASSERT_EQ(aDefs.NbShells(), 1);
  EXPECT_EQ(aDefs.NbShellFaces(0), 6);
}

TEST_F(BRepGraphConvenienceTest, ShellFaceDef_AllValid)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  const int aNbFaces = aDefs.NbShellFaces(0);
  for (int aFaceIter = 0; aFaceIter < aNbFaces; ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceDefId = aDefs.ShellFaceDef(0, aFaceIter);
    EXPECT_TRUE(aFaceDefId.IsValid()) << "Shell face " << aFaceIter;
    EXPECT_EQ(aFaceDefId.NodeKind, BRepGraph_NodeId::Kind::Face);
  }
}

TEST_F(BRepGraphConvenienceTest, ShellFaceDef_OutOfRange_Invalid)
{
  const BRepGraph::DefsView aDefs = myGraph.Defs();
  EXPECT_FALSE(aDefs.ShellFaceDef(0, -1).IsValid());
  EXPECT_FALSE(aDefs.ShellFaceDef(0, 100).IsValid());
}

// ---------- Integration: Cylinder with seam edge ----------

TEST(BRepGraphConvenienceCylinderTest, FindPCurve_WithOrientation_SeamEdge)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::DefsView aDefs = aGraph.Defs();

  // Look for seam edges (coedge with SeamPairIdx >= 0).
  for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeIter);
    const NCollection_Vector<int>& aCoEdgeIdxs = aDefs.CoEdgesOfEdge(anEdgeIter);

    for (int aCEI = 0; aCEI < aCoEdgeIdxs.Length(); ++aCEI)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aDefs.CoEdge(aCoEdgeIdxs.Value(aCEI));
      if (aCE.SeamPairIdx < 0)
        continue;

      // Found seam edge -- verify FindPCurve returns distinct entries for each orientation.
      const BRepGraph_NodeId aFaceDefId = aCE.FaceDefId;
      const BRepGraphInc::CoEdgeEntity* aPCF =
        aDefs.FindPCurve(anEdgeDef.Id, aFaceDefId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeEntity* aPCR =
        aDefs.FindPCurve(anEdgeDef.Id, aFaceDefId, TopAbs_REVERSED);
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
