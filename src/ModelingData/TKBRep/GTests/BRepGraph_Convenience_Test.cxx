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
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>
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
    BRepGraph_Builder::Perform(myGraph, aBox);
  }

  BRepGraph myGraph;
};

// ---------- Part A: NodeId Static Factories ----------

TEST_F(BRepGraph_ConvenienceTest, NodeId_Factories_CorrectKindAndIndex)
{
  const BRepGraph_NodeId aSolid = BRepGraph_SolidId(3);
  EXPECT_EQ(aSolid.NodeKind, BRepGraph_NodeId::Kind::Solid);
  EXPECT_EQ(aSolid.Index, 3);

  const BRepGraph_NodeId aShell = BRepGraph_ShellId(5);
  EXPECT_EQ(aShell.NodeKind, BRepGraph_NodeId::Kind::Shell);
  EXPECT_EQ(aShell.Index, 5);

  const BRepGraph_NodeId aFace = BRepGraph_FaceId::Start();
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aFace.Index, 0);

  const BRepGraph_NodeId aWire = BRepGraph_WireId(2);
  EXPECT_EQ(aWire.NodeKind, BRepGraph_NodeId::Kind::Wire);
  EXPECT_EQ(aWire.Index, 2);

  const BRepGraph_NodeId anEdge = BRepGraph_EdgeId(7);
  EXPECT_EQ(anEdge.NodeKind, BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anEdge.Index, 7);

  const BRepGraph_NodeId aVertex = BRepGraph_VertexId(1);
  EXPECT_EQ(aVertex.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  EXPECT_EQ(aVertex.Index, 1);

  const BRepGraph_NodeId aCompound = BRepGraph_CompoundId::Start();
  EXPECT_EQ(aCompound.NodeKind, BRepGraph_NodeId::Kind::Compound);
  EXPECT_EQ(aCompound.Index, 0);

  const BRepGraph_NodeId aCompSolid = BRepGraph_CompSolidId::Start();
  EXPECT_EQ(aCompSolid.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
  EXPECT_EQ(aCompSolid.Index, 0);
}

TEST_F(BRepGraph_ConvenienceTest, NodeId_Factories_EqualToConstructor)
{
  EXPECT_EQ(BRepGraph_FaceId(3), BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));
  EXPECT_EQ(BRepGraph_EdgeId::Start(), BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));
}

// ---------- Part B: EdgeDef Vertex Access via BRepGraph_Tool ----------

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_StartVertex_Valid)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId         anEdgeId(0);
  const BRepGraphInc::VertexRef& aStart = BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdgeId);
  EXPECT_TRUE(aStart.VertexDefId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_EndVertex_Valid)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId         anEdgeId(0);
  const BRepGraphInc::VertexRef& anEnd = BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdgeId);
  EXPECT_TRUE(anEnd.VertexDefId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_StartEnd_DifferForNonClosed)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdge = myGraph.Topo().Edges().Definition(anEdgeId);
  if (!anEdge.IsClosed)
  {
    const BRepGraph_VertexId aStartId =
      BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdgeId).VertexDefId;
    const BRepGraph_VertexId anEndId =
      BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdgeId).VertexDefId;
    EXPECT_NE(aStartId, anEndId);
  }
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_RefIds_AreValid)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdge = myGraph.Topo().Edges().Definition(anEdgeId);
  EXPECT_TRUE(anEdge.StartVertexRefId.IsValid());
  EXPECT_TRUE(anEdge.EndVertexRefId.IsValid());
}

// ---------- Part D: FaceDef::Surface ----------

TEST_F(BRepGraph_ConvenienceTest, FaceSurface_Valid)
{
  const BRepGraph::TopoView aDefs = myGraph.Topo();
  ASSERT_GT(aDefs.Faces().Nb(), 0);
  EXPECT_TRUE(aDefs.Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, FaceSurface_AllBoxFaces)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(aFaceIt.Current().SurfaceRepId.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has no surface";
  }
}

// ---------- Part E: DefsView::FindPCurve ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurve_ValidPair)
{
  // Find an edge/face pair that has a PCurve.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();

    for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraphInc::CoEdgeDef* aPCurve =
        BRepGraph_Tool::Edge::FindPCurve(myGraph, anEdgeIt.CurrentId(), aFaceId);
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
  EXPECT_EQ(
    BRepGraph_Tool::Edge::FindPCurve(myGraph, BRepGraph_EdgeId::Start(), BRepGraph_FaceId(9999)),
    nullptr);
}

// ---------- Part F: RefsView::FaceRefIdsOf ----------

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_Box_SixFaces)
{
  const BRepGraph::RefsView& aRefs = myGraph.Refs();
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aRefs.Faces().IdsOf(BRepGraph_ShellId::Start()).Length(), 6);
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_AllValid)
{
  const BRepGraph::RefsView&                     aRefs = myGraph.Refs();
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds =
    aRefs.Faces().IdsOf(BRepGraph_ShellId::Start());
  for (int aFaceIter = 0; aFaceIter < aFaceRefIds.Length(); ++aFaceIter)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aRefs.Faces().Entry(aFaceRefIds.Value(aFaceIter));
    EXPECT_TRUE(aFaceRef.FaceDefId.IsValid()) << "Shell face ref " << aFaceIter;
  }
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_InvalidShell_Empty)
{
  const BRepGraph::RefsView& aRefs = myGraph.Refs();
  EXPECT_EQ(aRefs.Faces().IdsOf(BRepGraph_ShellId(100)).Length(), 0);
}

// ---------- Integration: Cylinder with seam edge ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurve_WithOrientation_SeamEdge)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  BRepGraph_Builder::Perform(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::TopoView aDefs = aGraph.Topo();

  // Look for seam edges (coedge with SeamPairId valid).
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aDefs.Edges().CoEdges(anEdgeIt.CurrentId());

    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aDefs.CoEdges().Definition(aCoEdgeId);
      if (!aCE.SeamPairId.IsValid())
        continue;

      // Found seam edge - verify FindPCurve returns distinct entries for each orientation.
      const BRepGraph_FaceId         aFaceDefId = aCE.FaceDefId;
      const BRepGraphInc::CoEdgeDef* aPCF =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, anEdgeIt.CurrentId(), aFaceDefId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeDef* aPCR =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, anEdgeIt.CurrentId(), aFaceDefId, TopAbs_REVERSED);
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
