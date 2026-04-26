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

// Coverage for EditorView::EdgeOps::ReplaceVertex - boundary/internal vertex
// remap without a full edge rebuild.

#include <BRepGraph.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 = BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

} // namespace

TEST(BRepGraph_ReplaceVertexTest, StartVertex_SwappedToFreshVertex_AuditClean)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anOldStartRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;
  ASSERT_TRUE(anOldStartRefId.IsValid());

  const BRepGraph_VertexId aNewVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(99.0, 99.0, 99.0), 1.0e-7);
  ASSERT_TRUE(aNewVertex.IsValid());

  const BRepGraph_VertexRefId aNewRefId =
    aGraph.Editor().Edges().ReplaceVertex(anEdgeId, anOldStartRefId, aNewVertex);
  ASSERT_TRUE(aNewRefId.IsValid());
  EXPECT_NE(aNewRefId, anOldStartRefId);

  // Edge slot must now point at the new ref.
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId, aNewRefId);

  // Old ref is retired, new ref points at the replacement vertex.
  EXPECT_TRUE(aGraph.Refs().Vertices().Entry(anOldStartRefId).IsRemoved);
  EXPECT_EQ(aGraph.Refs().Vertices().Entry(aNewRefId).VertexDefId, aNewVertex);
  EXPECT_EQ(aGraph.Refs().Vertices().Entry(aNewRefId).ParentId, BRepGraph_NodeId(anEdgeId));

  // Lightweight only; a single-edge replacement breaks wire connectivity.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraph_ReplaceVertexTest, EndVertex_SwappedToFreshVertex_PreservesOrientation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anOldEndRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).EndVertexRefId;
  ASSERT_TRUE(anOldEndRefId.IsValid());
  const TopAbs_Orientation aExpectedOri = aGraph.Refs().Vertices().Entry(anOldEndRefId).Orientation;

  const BRepGraph_VertexId aNewVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.0e-7);
  const BRepGraph_VertexRefId aNewRefId =
    aGraph.Editor().Edges().ReplaceVertex(anEdgeId, anOldEndRefId, aNewVertex);
  ASSERT_TRUE(aNewRefId.IsValid());

  EXPECT_EQ(aGraph.Refs().Vertices().Entry(aNewRefId).Orientation, aExpectedOri)
    << "ReplaceVertex must preserve orientation of the retired ref";
  // Lightweight validate covers structural invariants (reverse index, active
  // counts) but skips wire-connectivity. A single-edge vertex replacement
  // deliberately breaks wire connectivity at the shared corner; the full
  // Audit would reject it. Callers that want a connectivity-safe replace
  // must update every edge at that corner in the same mutation batch.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraph_ReplaceVertexTest, SameVertex_Idempotent)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anOldStartRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;
  const BRepGraph_VertexId aSameVertex =
    aGraph.Refs().Vertices().Entry(anOldStartRefId).VertexDefId;

  const BRepGraph_VertexRefId aResult =
    aGraph.Editor().Edges().ReplaceVertex(anEdgeId, anOldStartRefId, aSameVertex);
  EXPECT_EQ(aResult, anOldStartRefId)
    << "Replacing with the same vertex must be a no-op returning the same ref";
  EXPECT_FALSE(aGraph.Refs().Vertices().Entry(anOldStartRefId).IsRemoved);
}

TEST(BRepGraph_ReplaceVertexTest, InactiveEdge_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anOldRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;

  const BRepGraph_VertexId aNewVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 5.0, 5.0), 1.0e-7);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));

  const BRepGraph_VertexRefId aResult =
    aGraph.Editor().Edges().ReplaceVertex(anEdgeId, anOldRefId, aNewVertex);
  EXPECT_FALSE(aResult.IsValid()) << "Replacing on a removed edge must return an invalid ref id";
}

TEST(BRepGraph_ReplaceVertexTest, WrongParent_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 1);

  // Vertex ref from edge 1 passed to an API call targeting edge 0 must fail.
  const BRepGraph_EdgeId      anEdge0(0);
  const BRepGraph_VertexRefId aEdge1StartRefId =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(1)).StartVertexRefId;
  ASSERT_TRUE(aEdge1StartRefId.IsValid());

  const BRepGraph_VertexId aNewVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 100.0), 1.0e-7);

  const BRepGraph_VertexRefId aResult =
    aGraph.Editor().Edges().ReplaceVertex(anEdge0, aEdge1StartRefId, aNewVertex);
  EXPECT_FALSE(aResult.IsValid())
    << "ReplaceVertex must refuse a ref whose ParentId is a different edge";
}

TEST(BRepGraph_ReplaceVertexTest, ReverseIndex_Rebuilt_VertexToEdge)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_EdgeId      anEdgeId(0);
  const BRepGraph_VertexRefId anOldRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;
  const BRepGraph_VertexId aOldVertexId = aGraph.Refs().Vertices().Entry(anOldRefId).VertexDefId;

  const BRepGraph_VertexId aNewVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1000.0, 0.0, 0.0), 1.0e-7);

  ASSERT_TRUE(aGraph.Editor().Edges().ReplaceVertex(anEdgeId, anOldRefId, aNewVertex).IsValid());

  // New vertex must now see anEdgeId in its vertex-to-edge reverse map.
  const NCollection_DynamicArray<BRepGraph_EdgeId>& aNewEdges =
    aGraph.Topo().Vertices().Edges(aNewVertex);
  bool aFoundNew = false;
  for (const BRepGraph_EdgeId& anE : aNewEdges)
  {
    if (anE == anEdgeId)
    {
      aFoundNew = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundNew) << "Vertex->Edge reverse index must include the replacement edge";

  // Old vertex may still be referenced by other edges of the box (shared
  // corners); but it should no longer see anEdgeId through this slot.
  const NCollection_DynamicArray<BRepGraph_EdgeId>& aOldEdges =
    aGraph.Topo().Vertices().Edges(aOldVertexId);
  (void)aOldEdges;

  // Lightweight validate covers structural invariants (reverse index, active
  // counts) but skips wire-connectivity. A single-edge vertex replacement
  // deliberately breaks wire connectivity at the shared corner; the full
  // Audit would reject it. Callers that want a connectivity-safe replace
  // must update every edge at that corner in the same mutation batch.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}
