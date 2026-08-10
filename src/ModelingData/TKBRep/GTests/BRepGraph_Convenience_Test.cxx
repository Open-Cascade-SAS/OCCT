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
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
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
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = myGraph.Shapes().Add(aBox);
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
  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId aStart = BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId);
  EXPECT_TRUE(aStart.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_EndVertex_Valid)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anEnd = BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdgeId);
  EXPECT_TRUE(anEnd.IsValid());
}

TEST_F(BRepGraph_ConvenienceTest, EdgeDef_StartEnd_DifferForNonClosed)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId anEdgeId(0);
  if (!BRepGraph_Tool::Edge::IsClosed(myGraph, anEdgeId))
  {
    const BRepGraph_VertexRefId aStartId = BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId);
    const BRepGraph_VertexRefId anEndId  = BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdgeId);
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

TEST_F(BRepGraph_ConvenienceTest, EdgeOps_FindByVertices_FindsDirectedEdge)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId aStartRef    = BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId);
  const BRepGraph_VertexRefId anEndRef     = BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdgeId);
  const BRepGraph_Tool::VertexUsage aStart = BRepGraph_Tool::Vertex::Usage(myGraph, aStartRef);
  const BRepGraph_Tool::VertexUsage anEnd  = BRepGraph_Tool::Vertex::Usage(myGraph, anEndRef);
  ASSERT_TRUE(aStart.IsValid());
  ASSERT_TRUE(anEnd.IsValid());

  EXPECT_EQ(BRepGraph_Tool::Edge::FindByVertices(myGraph, aStart.DefId, anEnd.DefId), anEdgeId);
}

TEST_F(BRepGraph_ConvenienceTest, EdgeOps_FindByVertices_ReverseRequiresExplicitFlag)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId aStartRef    = BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId);
  const BRepGraph_VertexRefId anEndRef     = BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdgeId);
  const BRepGraph_Tool::VertexUsage aStart = BRepGraph_Tool::Vertex::Usage(myGraph, aStartRef);
  const BRepGraph_Tool::VertexUsage anEnd  = BRepGraph_Tool::Vertex::Usage(myGraph, anEndRef);
  ASSERT_TRUE(aStart.IsValid());
  ASSERT_TRUE(anEnd.IsValid());
  ASSERT_NE(aStart.DefId, anEnd.DefId);

  EXPECT_FALSE(BRepGraph_Tool::Edge::FindByVertices(myGraph, anEnd.DefId, aStart.DefId).IsValid());
  EXPECT_EQ(BRepGraph_Tool::Edge::FindByVertices(myGraph, anEnd.DefId, aStart.DefId, true),
            anEdgeId);
}

// ---------- Part D: FaceDef::Surface ----------

TEST_F(BRepGraph_ConvenienceTest, FaceSurface_Valid)
{
  const BRepGraph::TopoView& aDefs = myGraph.Topo();
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

// ---------- Part E: DefsView::FindPCurveCoEdgeId ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurveCoEdgeId_ValidPair)
{
  // Find an edge/face pair that has a PCurve.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();

    for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aPCurveId =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph, anEdgeIt.CurrentId(), aFaceId);
      if (aPCurveId.IsValid())
      {
        const BRepGraphInc::CoEdgeDef& aPCurve = myGraph.Topo().CoEdges().Definition(aPCurveId);
        EXPECT_TRUE(aPCurve.Curve2DRepId.IsValid());
        return;
      }
    }
  }
}

TEST_F(BRepGraph_ConvenienceTest, FindPCurveCoEdgeId_InvalidPair_ReturnsNull)
{
  EXPECT_FALSE(BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph,
                                                        BRepGraph_EdgeId::Start(),
                                                        BRepGraph_FaceId(9999))
                 .IsValid());
}

// ---------- Part F: RefsView::FaceRefIdsOf ----------

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_Box_SixFaces)
{
  const BRepGraph::RefsView& aRefs = myGraph.Refs();
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aRefs.Faces().IdsOf(BRepGraph_ShellId::Start()).Size(), 6);
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_AllValid)
{
  const BRepGraph::RefsView&                           aRefs = myGraph.Refs();
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefIds =
    aRefs.Faces().IdsOf(BRepGraph_ShellId::Start());
  for (size_t aFaceIter = 0; aFaceIter < aFaceRefIds.Size(); ++aFaceIter)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aRefs.Faces().Entry(aFaceRefIds.Value(aFaceIter));
    EXPECT_TRUE(aFaceRef.ChildFaceId.IsValid()) << "Shell face ref " << aFaceIter;
  }
}

TEST_F(BRepGraph_ConvenienceTest, ShellFaceRefs_InvalidShell_Empty)
{
  const BRepGraph::RefsView& aRefs = myGraph.Refs();
  EXPECT_EQ(aRefs.Faces().IdsOf(BRepGraph_ShellId(100)).Size(), 0);
}

TEST_F(BRepGraph_ConvenienceTest, ShapesView_RemoveShape_RemovesFoundNode)
{
  ASSERT_GT(myGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const TopoDS_Shape     aFace   = myGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(aFace.IsNull());
  ASSERT_TRUE(myGraph.Shapes().FindNode(aFace).IsValid());

  EXPECT_TRUE(myGraph.Shapes().RemoveShape(aFace));
  EXPECT_FALSE(myGraph.Shapes().FindNode(aFace).IsValid());
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceId));
  EXPECT_FALSE(myGraph.Shapes().RemoveShape(aFace));
}

// ---------- Integration: Cylinder with seam edge ----------

TEST_F(BRepGraph_ConvenienceTest, FindPCurveCoEdgeId_WithOrientation_SeamEdge)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCyl);
  ASSERT_TRUE(aBuildRes2.IsOk());

  const BRepGraph::TopoView& aDefs = aGraph.Topo();

  // Look for seam edges via the connectivity-derived IsSeam query.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aDefs.Edges().CoEdges(anEdgeIt.CurrentId());

    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aDefs.CoEdges().Definition(aCoEdgeId);
      if (!BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCoEdgeId))
      {
        continue;
      }

      // Found seam edge - verify FindPCurveCoEdgeId returns distinct entries for each orientation.
      const BRepGraph_FaceId   aFaceId = aCE.FaceId;
      const BRepGraph_CoEdgeId aPCF    = BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph,
                                                                               anEdgeIt.CurrentId(),
                                                                               aFaceId,
                                                                               TopAbs_FORWARD);
      const BRepGraph_CoEdgeId aPCR    = BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph,
                                                                               anEdgeIt.CurrentId(),
                                                                               aFaceId,
                                                                               TopAbs_REVERSED);
      EXPECT_TRUE(aPCF.IsValid());
      EXPECT_TRUE(aPCR.IsValid());
      if (aPCF.IsValid() && aPCR.IsValid())
      {
        EXPECT_NE(aPCF, aPCR);
      }
      return;
    }
  }
}
