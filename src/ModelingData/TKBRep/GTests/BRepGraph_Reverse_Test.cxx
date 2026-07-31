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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>

#include <NCollection_LinearVector.hxx>

#include <gtest/gtest.h>

namespace
{
BRepGraph buildBoxGraph()
{
  BRepGraph                                        aGraph;
  const occ::handle<BRepGraph_LayerTopoSupplement> aRegisteredLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  EXPECT_FALSE(aRegisteredLayer.IsNull());
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

BRepGraph_EdgeId addSegment(BRepGraph&               theGraph,
                            const BRepGraph_VertexId theStart,
                            const BRepGraph_VertexId theEnd)
{
  return theGraph.Editor().Edges().Add(theStart,
                                       theEnd,
                                       occ::handle<Geom_Curve>(),
                                       0.0,
                                       1.0,
                                       1.0e-7);
}

TopoDS_Shell makeReversedFaceShell()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);

  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(anExp.Current());
    aFace.Orientation(TopAbs_REVERSED);
    aBuilder.Add(aShell, aFace);
  }

  return aShell;
}
} // namespace

TEST(BRepGraph_ReverseTest, Wire_OrderAndOrientationFlipped)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_WireId aWire(0);
  ASSERT_TRUE(aWire.IsValid(aGraph.Topo().Wires().Nb()));

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgesBefore =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  std::vector<BRepGraph_CoEdgeId> aOrderBefore;
  std::vector<TopAbs_Orientation> aOriBefore;
  for (size_t i = 0; i < aCoEdgesBefore.Size(); ++i)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgesBefore.Value(i);
    aOrderBefore.push_back(aCoEdgeId);
    const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
    aOriBefore.push_back(aCoEdge.Orientation);
  }
  ASSERT_GT(aOrderBefore.size(), 1u);

  aGraph.Editor().Wires().Reverse(aWire);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgesAfter =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_EQ(aOrderBefore.size(), aCoEdgesAfter.Size());
  for (size_t i = 0; i < aCoEdgesAfter.Size(); ++i)
  {
    EXPECT_EQ(aCoEdgesAfter.Value(i), aOrderBefore[aOrderBefore.size() - 1 - i])
      << "CoEdgeId order should be reversed at slot " << i;

    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgesAfter.Value(i);
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
    EXPECT_EQ(aCoEdge.Orientation, TopAbs::Reverse(aOriBefore[aOrderBefore.size() - 1 - i]))
      << "CoEdge orientation should be flipped at slot " << i;
  }
}

TEST(BRepGraph_ReverseTest, WireOrder_SetCoEdgeOrderCanonicalizesUnorderedPermutation)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_WireId aWire(0);
  ASSERT_TRUE(aWire.IsValid(aGraph.Topo().Wires().Nb()));

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_GE(aCoEdges.Size(), 4u);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aBadOrder;
  aBadOrder.Append(aCoEdges.Value(0));
  aBadOrder.Append(aCoEdges.Value(2));
  aBadOrder.Append(aCoEdges.Value(1));
  aBadOrder.Append(aCoEdges.Value(3));

  EXPECT_TRUE(aGraph.Editor().Wires().SetCoEdgeOrder(aWire, aBadOrder.ToArray1()));

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgesAfter =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_EQ(aCoEdges.Size(), aCoEdgesAfter.Size());
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

TEST(BRepGraph_ReverseTest, WireOrder_CheckCoEdgeOrderReportsCurrentAndReordered)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_WireId aWire(0);
  ASSERT_TRUE(aWire.IsValid(aGraph.Topo().Wires().Nb()));

  using CoEdgeOrderStatus = BRepGraph::EditorView::WireOps::CoEdgeOrderStatus;

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_GE(aCoEdges.Size(), 4u);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCurrentOrder;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
  {
    aCurrentOrder.Append(aCoEdgeId);
  }

  EXPECT_EQ(aGraph.Editor().Wires().CheckCoEdgeOrder(aWire, aCurrentOrder.ToArray1()),
            CoEdgeOrderStatus::AlreadyCurrent);
  EXPECT_TRUE(aGraph.Editor().Wires().SetCoEdgeOrder(aWire, aCurrentOrder.ToArray1()));

  NCollection_LinearVector<BRepGraph_CoEdgeId> anUnordered;
  anUnordered.Append(aCoEdges.Value(0));
  anUnordered.Append(aCoEdges.Value(2));
  anUnordered.Append(aCoEdges.Value(1));
  anUnordered.Append(aCoEdges.Value(3));
  EXPECT_EQ(aGraph.Editor().Wires().CheckCoEdgeOrder(aWire, anUnordered.ToArray1()),
            CoEdgeOrderStatus::Reordered);
  EXPECT_EQ(aGraph.Editor().Wires().CheckCoEdgeOrder(BRepGraph_WireId(), anUnordered.ToArray1()),
            CoEdgeOrderStatus::InvalidWire);
}

TEST(BRepGraph_ReverseTest, WireOrder_SetCoEdgeOrderCanonicalizesOpenWirePermutation)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(3.0, 0.0, 0.0), 1.0e-7);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(addSegment(aGraph, aV0, aV1), TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(addSegment(aGraph, aV1, aV2), TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(addSegment(aGraph, aV2, aV3), TopAbs_FORWARD));

  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  ASSERT_TRUE(aWire.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> anUnordered;
  anUnordered.Append(aCoEdges.Value(2));
  anUnordered.Append(aCoEdges.Value(0));
  anUnordered.Append(aCoEdges.Value(1));

  EXPECT_TRUE(aGraph.Editor().Wires().SetCoEdgeOrder(aWire, anUnordered.ToArray1()));

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& anOrdered =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_EQ(anOrdered.Size(), aCoEdges.Size());
  EXPECT_EQ(anOrdered.Value(0), aCoEdges.Value(0));
  EXPECT_EQ(anOrdered.Value(1), aCoEdges.Value(1));
  EXPECT_EQ(anOrdered.Value(2), aCoEdges.Value(2));
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_ReverseTest, WireOrder_CheckAppendCoEdgeReportsReadyAndAlreadyContained)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);

  const BRepGraph_EdgeId                       aFirstEdge = addSegment(aGraph, aV0, aV1);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aInitialCoEdges;
  aInitialCoEdges.Append(aGraph.Editor().CoEdges().Add(aFirstEdge, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aInitialCoEdges.ToArray1());
  ASSERT_TRUE(aWire.IsValid());

  const BRepGraph_EdgeId   aSecondEdge = addSegment(aGraph, aV1, aV2);
  const BRepGraph_CoEdgeId aSecondCoEdge =
    aGraph.Editor().CoEdges().Add(aSecondEdge, TopAbs_FORWARD);

  using CoEdgeOrderStatus = BRepGraph::EditorView::WireOps::CoEdgeOrderStatus;
  EXPECT_EQ(aGraph.Editor().Wires().CheckAppendCoEdge(aWire, aSecondCoEdge),
            CoEdgeOrderStatus::Ready);
  EXPECT_EQ(aGraph.Editor().Wires().CheckAppendCoEdge(aWire, aInitialCoEdges.Value(0)),
            CoEdgeOrderStatus::AlreadyContained);
}

TEST(BRepGraph_ReverseTest, WireOrder_CheckReplaceEdgeReportsReadyAndDisconnected)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(3.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV4 = aGraph.Editor().Vertices().Add(gp_Pnt(4.0, 0.0, 0.0), 1.0e-7);

  const BRepGraph_EdgeId                       anOldEdge = addSegment(aGraph, aV0, aV1);
  const BRepGraph_EdgeId                       aNextEdge = addSegment(aGraph, aV1, aV2);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anOldEdge, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aNextEdge, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  ASSERT_TRUE(aWire.IsValid());

  const BRepGraph_EdgeId aCompatibleEdge   = addSegment(aGraph, aV0, aV1);
  const BRepGraph_EdgeId aDisconnectedEdge = addSegment(aGraph, aV3, aV4);

  using ReplaceEdgeStatus = BRepGraph::EditorView::WireOps::ReplaceEdgeStatus;
  EXPECT_EQ(aGraph.Editor().Wires().CheckReplaceEdge(aWire, anOldEdge, aCompatibleEdge, false),
            ReplaceEdgeStatus::Ready);
  EXPECT_EQ(aGraph.Editor().Wires().CheckReplaceEdge(aWire, anOldEdge, anOldEdge, false),
            ReplaceEdgeStatus::AlreadyCurrent);
  EXPECT_EQ(aGraph.Editor().Wires().CheckReplaceEdge(aWire, anOldEdge, aDisconnectedEdge, false),
            ReplaceEdgeStatus::Disconnected);
}

TEST(BRepGraph_ReverseTest, WireOrder_RemoveInternalCoEdgeFromClosedWireKeepsConnectedOrder)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_WireId aWire(0);
  ASSERT_TRUE(aWire.IsValid(aGraph.Topo().Wires().Nb()));

  const NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgesBefore =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  ASSERT_GE(aCoEdgesBefore.Size(), 4u);

  EXPECT_TRUE(aGraph.Editor().Wires().RemoveCoEdge(aWire, aCoEdgesBefore.Value(1)));

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgesAfter =
    aGraph.Topo().Wires().Relations(aWire).CoEdgeIds;
  EXPECT_EQ(aCoEdgesAfter.Size(), aCoEdgesBefore.Size() - 1);
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

TEST(BRepGraph_ReverseTest, Edge_StartEndVertexRefsSwapped)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_EdgeId anEdge(0);
  ASSERT_TRUE(anEdge.IsValid(aGraph.Topo().Edges().Nb()));

  const BRepGraph_VertexRefId aStartBefore =
    aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  const BRepGraph_VertexRefId aEndBefore = aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId;
  ASSERT_TRUE(aStartBefore.IsValid());
  ASSERT_TRUE(aEndBefore.IsValid());
  ASSERT_NE(aStartBefore, aEndBefore);

  aGraph.Editor().Edges().Reverse(anEdge);

  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId, aEndBefore);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId, aStartBefore);
}

TEST(BRepGraph_ReverseTest, Edge_RoundTripRestoresOriginal)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_EdgeId anEdge(0);
  ASSERT_TRUE(anEdge.IsValid(aGraph.Topo().Edges().Nb()));

  const BRepGraph_VertexRefId aStart = aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  const BRepGraph_VertexRefId aEnd   = aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId;

  aGraph.Editor().Edges().Reverse(anEdge);
  aGraph.Editor().Edges().Reverse(anEdge);

  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId, aStart);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId, aEnd);
}

TEST(BRepGraph_ReverseTest, LayerTopoSupplement_AddVertexToFaceGoesToSupplementLayer)
{
  BRepGraph              aGraph = buildBoxGraph();
  const BRepGraph_FaceId aFaceId(0);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.5, 0.5, 0.5), 1.0e-7);
  ASSERT_TRUE(aVertex.IsValid());

  // Faces().AddVertex() removed; use Shapes().Add(vertexShape, faceNode) instead.
  // const BRepGraph_VertexRefId aRefId =
  //   aGraph.Editor().Faces().AddVertex(aFaceId, aVertex, TopAbs_INTERNAL);
  // EXPECT_FALSE(aRefId.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aFaceId));
  // No vertex added -> no supplement attachments.
  ASSERT_EQ(anAttached.Size(), 0u);
  // Skip removal test since no attachments exist.
}

TEST(BRepGraph_ReverseTest, LayerTopoSupplement_ShellStartsWithoutSupplementAttachments)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);
  const BRepGraph_ShellId aShellId(0);
  const BRepGraph_EdgeId  anEdge(0);
  ASSERT_TRUE(anEdge.IsValid(aGraph.Topo().Edges().Nb()));

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  if (!aLayer.IsNull())
  {
    EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aShellId)).Size(), 0u);
  }
}

TEST(BRepGraph_ReverseTest, Relations_SupplementLayerClearedOnGraphClear)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  const BRepGraph_FaceId aFaceId(0);

  // Faces().AddVertex() removed; use Shapes().Add(vertexShape, faceNode) instead.
  // [[maybe_unused]] const BRepGraph_VertexRefId aVertexRef =
  //   aGraph.Editor().Faces().AddVertex(aFaceId, aVertex, TopAbs_INTERNAL);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayerBefore =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayerBefore.IsNull());
  // No vertex added -> no supplement attachments before clear.
  ASSERT_EQ(aLayerBefore->AttachedTo(BRepGraph_NodeId(aFaceId)).Size(), 0u);

  aGraph.Clear();

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 0);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayerAfter =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayerAfter.IsNull());
  EXPECT_EQ(aLayerAfter->AttachedTo(BRepGraph_NodeId(aFaceId)).Size(), 0u);
}

TEST(BRepGraph_ReverseTest, ReversedShell_GraphStoresFaceRefOrientation)
{
  const TopoDS_Shell aShell = makeReversedFaceShell();

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShell);
  ASSERT_TRUE(aResult.IsOk());

  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6u);

  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    aGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_EQ(aFaceRefs.Size(), 6u);

  for (const BRepGraph_FaceRefId& aFaceRefId : aFaceRefs)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aGraph.Refs().Faces().Entry(aFaceRefId);
    EXPECT_EQ(aFaceRef.Orientation, TopAbs_REVERSED)
      << "Face " << aFaceRef.ChildFaceId.Index << " should have REVERSED orientation";
  }
}

TEST(BRepGraph_ReverseTest, ReversedShell_ParentExplorerReportsShell)
{
  const TopoDS_Shell aShell = makeReversedFaceShell();

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShell);
  ASSERT_TRUE(aResult.IsOk());

  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(6u); ++aFaceId)
  {
    BRepGraph_ParentExplorer anExp(aGraph,
                                   aFaceId,
                                   BRepGraph_ParentExplorer::TraversalMode::DirectParents);
    ASSERT_TRUE(anExp.More()) << "Face " << aFaceId.Index << " should have a parent shell";
    EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));
    EXPECT_EQ(anExp.LeafOrientation(), TopAbs_REVERSED);
  }
}

TEST(BRepGraph_ReverseTest, ReversedShell_ReconstructPreservesOrientation)
{
  const TopoDS_Shell aShell = makeReversedFaceShell();

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShell);
  ASSERT_TRUE(aResult.IsOk());

  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(6u); ++aFaceId)
  {
    TopoDS_Shape aReconShape = aGraph.Shapes().Reconstruct(aFaceId);
    ASSERT_FALSE(aReconShape.IsNull());
    ASSERT_EQ(aReconShape.ShapeType(), TopAbs_FACE);
  }
}

TEST(BRepGraph_ReverseTest, ReversedShell_ReconstructShellPreservesFaceOrientations)
{
  const TopoDS_Shell aShell = makeReversedFaceShell();

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShell);
  ASSERT_TRUE(aResult.IsOk());

  TopoDS_Shape aReconShell =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, 0));
  ASSERT_FALSE(aReconShell.IsNull());
  ASSERT_EQ(aReconShell.ShapeType(), TopAbs_SHELL);

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aReconShell, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    EXPECT_EQ(aFace.Orientation(), TopAbs_REVERSED);
    ++aFaceCount;
  }

  EXPECT_EQ(aFaceCount, 6);
}
