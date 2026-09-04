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

#include <BRepAlgoAPI_Cut.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Bnd_Box.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_LayerDeferred.hxx>
#include <BRepGraph_LayerLock.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_RevisionHash.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <tuple>

#include <gtest/gtest.h>

namespace
{

TopoDS_Compound makeTwoCopiedFaces()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());
  return aCompound;
}

TopoDS_Compound makeBoxWithLooseEdge()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(100.0, 0.0, 0.0), gp_Pnt(120.0, 0.0, 0.0));

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, anEdgeMaker.Edge());
  return aCompound;
}

int countHistoryRecordsByOp(const BRepGraph& theGraph, const TCollection_AsciiString& theOp)
{
  const BRepGraph_LayerHistory* aHistory =
    theGraph.LayerRegistry().Find<BRepGraph_LayerHistory>().get();
  if (aHistory == nullptr)
  {
    return 0;
  }

  int aCount = 0;
  for (size_t aRecIdx = 0; aRecIdx < aHistory->NbRecords(); ++aRecIdx)
  {
    if (aHistory->Record(aRecIdx).OperationName == theOp)
    {
      ++aCount;
    }
  }
  return aCount;
}

void addGraphBounds(const BRepGraph& theGraph, Bnd_Box& theBox)
{
  for (BRepGraph_VertexIterator aVertIt(theGraph); aVertIt.More(); aVertIt.Next())
  {
    theBox.Add(theGraph.Topo().Vertices().Definition(aVertIt.CurrentId()).Point);
  }
}

void expectBoxNear(const Bnd_Box& theLeft, const Bnd_Box& theRight, const double theTol)
{
  ASSERT_FALSE(theLeft.IsVoid());
  ASSERT_FALSE(theRight.IsVoid());

  double aLeftMinX, aLeftMinY, aLeftMinZ, aLeftMaxX, aLeftMaxY, aLeftMaxZ;
  double aRightMinX, aRightMinY, aRightMinZ, aRightMaxX, aRightMaxY, aRightMaxZ;
  theLeft.Get(aLeftMinX, aLeftMinY, aLeftMinZ, aLeftMaxX, aLeftMaxY, aLeftMaxZ);
  theRight.Get(aRightMinX, aRightMinY, aRightMinZ, aRightMaxX, aRightMaxY, aRightMaxZ);

  EXPECT_NEAR(aLeftMinX, aRightMinX, theTol);
  EXPECT_NEAR(aLeftMinY, aRightMinY, theTol);
  EXPECT_NEAR(aLeftMinZ, aRightMinZ, theTol);
  EXPECT_NEAR(aLeftMaxX, aRightMaxX, theTol);
  EXPECT_NEAR(aLeftMaxY, aRightMaxY, theTol);
  EXPECT_NEAR(aLeftMaxZ, aRightMaxZ, theTol);
}

uint32_t countFaceBoundPCurveCoEdges(const BRepGraph& theGraph)
{
  uint32_t aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    if (aCoEdge.FaceId.IsValid() && !BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId).IsNull())
    {
      ++aCount;
    }
  }
  return aCount;
}

} // namespace

TEST(BRepGraph_CompactTest, NoRemovedNodes_Noop)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const uint32_t aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbFacesBefore    = aGraph.Topo().Faces().Nb();

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedVertices, 0);
  EXPECT_EQ(aRes.NbRemovedEdges, 0);
  EXPECT_EQ(aRes.NbRemovedFaces, 0);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore);
}

TEST(BRepGraph_CompactTest, RemovedTailVertex_CompactsStorage)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());
  ASSERT_EQ(aGraph.LayerRegistry().NbLayers(), 1u);
  ASSERT_FALSE(aGraph.LayerRegistry().Find<BRepGraph_LayerHistory>().IsNull());

  const BRepGraph_UID aV0Uid = aGraph.UIDs().Of(aV0);
  ASSERT_TRUE(aV0Uid.IsValid());

  aGraph.Editor().Gen().RemoveNode(aV1);
  ASSERT_EQ(aGraph.Topo().Vertices().Nb(), 2u);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedVertices, 1u);
  EXPECT_EQ(aRes.NbNodesBefore, 2u);
  EXPECT_EQ(aRes.NbNodesAfter, 1u);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 1u);
  EXPECT_FALSE(BRepGraph_VertexId(1).IsValid(aGraph.Topo().Vertices().Nb()));
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aV0Uid), BRepGraph_NodeId(aV0));
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, AuditValidationFailureKeepsSourceGraphUnchanged)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aProductA = aGraph.Editor().Products().Add();
  const BRepGraph_ProductId aProductB = aGraph.Editor().Products().Add();
  ASSERT_TRUE(aProductA.IsValid());
  ASSERT_TRUE(aProductB.IsValid());
  aGraph.Editor().Products().AppendDocumentRoot(aProductA);
  ASSERT_TRUE(aGraph.Editor().Products().Append(aProductA, aProductB, TopLoc_Location()).IsValid());
  ASSERT_TRUE(aGraph.Editor().Products().Append(aProductB, aProductA, TopLoc_Location()).IsValid());
  aGraph.Editor().Products().AppendDocumentRoot(aProductA);

  const BRepGraph_VertexId aRemovedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(100.0, 0.0, 0.0), Precision::Confusion());
  ASSERT_TRUE(aRemovedVertex.IsValid());
  aGraph.Editor().Gen().RemoveNode(aRemovedVertex);

  ASSERT_FALSE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());

  const uint32_t      aNbNodesBefore       = aGraph.Topo().Gen().NbNodes();
  const uint32_t      aNbVerticesBefore    = aGraph.Topo().Vertices().Nb();
  const uint32_t      aNbProductsBefore    = aGraph.Topo().Products().Nb();
  const uint32_t      aNbOccurrencesBefore = aGraph.Topo().Occurrences().Nb();
  const BRepGraph_UID aFirstVertexUid      = aGraph.UIDs().Of(BRepGraph_VertexId::Start());
  ASSERT_TRUE(aFirstVertexUid.IsValid());

  BRepGraph_Compact::Options aOptions;
  aOptions.ValidationMode              = BRepGraph_Compact::Options::ValidationPolicy::Audit;
  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph, aOptions);

  EXPECT_EQ(aRes.NbRemovedVertices, 1u);
  EXPECT_EQ(aRes.NbNodesBefore, aNbNodesBefore);
  EXPECT_EQ(aRes.NbNodesAfter, aNbNodesBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
  EXPECT_EQ(aGraph.Topo().Products().Nb(), aNbProductsBefore);
  EXPECT_EQ(aGraph.Topo().Occurrences().Nb(), aNbOccurrencesBefore);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aRemovedVertex));
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aFirstVertexUid),
            BRepGraph_NodeId(BRepGraph_VertexId::Start()));
  EXPECT_FALSE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

TEST(BRepGraph_CompactTest, RemovedTailEdge_CompactsVertexRefs)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId   anE0 =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(3.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId   anE1 =
    aGraph.Editor().Edges().Add(aV2, aV3, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(anE0.IsValid());
  ASSERT_TRUE(anE1.IsValid());
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 2u);
  ASSERT_EQ(aGraph.Refs().Vertices().Nb(), 4u);

  const BRepGraph_UID anE0Uid = aGraph.UIDs().Of(anE0);
  ASSERT_TRUE(anE0Uid.IsValid());

  aGraph.Editor().Gen().RemoveNode(anE1);
  const BRepGraph_RevisionHash aSemanticHash = BRepGraph_RevisionHash::Hasher::Semantic(aGraph);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedEdges, 1u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 1u);
  EXPECT_EQ(aGraph.Refs().Vertices().Nb(), 2u);
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(anE0Uid), BRepGraph_NodeId(anE0));
  EXPECT_TRUE(
    BRepGraph_Tool::Edge::StartVertexId(aGraph, anE0).IsValid(aGraph.Refs().Vertices().Nb()));
  EXPECT_TRUE(
    BRepGraph_Tool::Edge::EndVertexId(aGraph, anE0).IsValid(aGraph.Refs().Vertices().Nb()));
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aSemanticHash);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, InteriorRemovedVertex_CompactsStorage)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());
  ASSERT_TRUE(aV2.IsValid());
  ASSERT_EQ(aGraph.LayerRegistry().NbLayers(), 1u);
  ASSERT_FALSE(aGraph.LayerRegistry().Find<BRepGraph_LayerHistory>().IsNull());

  aGraph.Editor().Gen().RemoveNode(aV1);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.StatusCode, BRepGraph_Compact::Status::Done);
  EXPECT_EQ(aRes.NbRemovedVertices, 1u);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 2u);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, RemovedCoEdgeOnly_CompactsStorage)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  const BRepGraph_CoEdgeId aCoEdge = aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge.IsValid());
  ASSERT_EQ(aGraph.Topo().CoEdges().Nb(), 1u);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aCoEdge));

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedCoEdges, 1u);
  EXPECT_GT(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().CoEdges().Nb(), 0u);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, RemovedFaceRefOnly_CompactsStorage)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Refs().Faces().Nb(), 0u);
  ASSERT_GT(aGraph.Topo().Shells().Nb(), 0u);

  BRepGraph_FaceRefId aFaceRefId;
  for (BRepGraph_RefsFaceOfShell aRefIt(aGraph, BRepGraph_ShellId::Start()); aRefIt.More();)
  {
    aFaceRefId = aRefIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aFaceRefId.IsValid());

  const uint32_t aNbFaceRefsBefore = aGraph.Refs().Faces().Nb();
  ASSERT_TRUE(aGraph.Editor().Gen().RemoveRef(aFaceRefId));
  const BRepGraph_RevisionHash aSemanticHash = BRepGraph_RevisionHash::Hasher::Semantic(aGraph);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedRefs, 1u);
  EXPECT_EQ(aGraph.Refs().Faces().Nb(), aNbFaceRefsBefore - 1u);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aSemanticHash);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, RemovedCurveRepOnly_CompactsStorage)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId      aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId      aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Lin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)));
  const BRepGraph_EdgeId anEdge = aGraph.Editor().Edges().Add(aV0, aV1, aCurve, 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(anEdge.IsValid());
  ASSERT_EQ(aGraph.Topo().Geometry().NbEdgeCurves3D(), 1u);
  ASSERT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), 1u);

  aGraph.Editor().Edges().ClearCurve(anEdge);
  ASSERT_EQ(aGraph.Topo().Geometry().NbEdgeCurves3D(), 1u);
  ASSERT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), 0u);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedReps, 1u);
  EXPECT_EQ(aGraph.Topo().Geometry().NbEdgeCurves3D(), 0u);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), 0u);
  EXPECT_FALSE(aGraph.Topo().Edges().Definition(anEdge).Curve3DRepId.IsValid());
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

TEST(BRepGraph_CompactTest, AfterDeduplicate_RemovesNodes)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Run geometry dedup which replaces duplicate surface/curve handles directly.
  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);

  // No separate geometry nodes exist, so NbRemovedSurfaces/NbRemovedCurves is always 0.
  EXPECT_EQ(aRes.NbRemovedSurfaces, 0);
  EXPECT_EQ(aRes.NbRemovedCurves, 0);
  // Without MergeEntitiesWhenSafe, topology node count does not change either.
  EXPECT_EQ(aRes.NbNodesAfter, aRes.NbNodesBefore);
}

TEST(BRepGraph_CompactTest, IndexDensity_NoGaps)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // After compaction, there should be no removed defs.
  for (BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
       aVertexId.IsValid(aGraph.Topo().Vertices().Nb());
       ++aVertexId)
  {
    EXPECT_FALSE(aVertexId.IsRemoved(aGraph));
  }
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
       anEdgeId.IsValid(aGraph.Topo().Edges().Nb());
       ++anEdgeId)
  {
    EXPECT_FALSE(anEdgeId.IsRemoved(aGraph));
  }
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(aGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    EXPECT_FALSE(aFaceId.IsRemoved(aGraph));
  }
  for (BRepGraph_WireId aWireId = BRepGraph_WireId::Start();
       aWireId.IsValid(aGraph.Topo().Wires().Nb());
       ++aWireId)
  {
    EXPECT_FALSE(aWireId.IsRemoved(aGraph));
  }
}

TEST(BRepGraph_CompactTest, CrossReferences_Valid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, Compact_DoesNotRecordRemapHistory)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Use full entity merge so that duplicate topology nodes are actually removed.
  // This ensures Compact must remap surviving indices and produces >= 1 record.
  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const int aNbRemapRecords =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap"));
  EXPECT_EQ(aNbRemapRecords, 0);
}

TEST(BRepGraph_CompactTest, FullPipeline_Deduplicate_Compact_Validate)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Full dedup (replaces duplicate handles directly on defs).
  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);

  // Compact. Without MergeEntitiesWhenSafe, no topology nodes are removed,
  // so NbNodesAfter == NbNodesBefore.
  const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aCompactRes.NbNodesAfter, aCompactRes.NbNodesBefore);

  // Validate.
  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, RemovalCompact_PreservesBounds_AndDoesNotGrowTopology)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0);
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 2);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId(2));

  const uint32_t aCoEdgesBeforeCompact = aGraph.Topo().CoEdges().Nb();

  Bnd_Box aBoxBeforeCompact;
  addGraphBounds(aGraph, aBoxBeforeCompact);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GE(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_LE(aGraph.Topo().CoEdges().Nb(), aCoEdgesBeforeCompact);

  Bnd_Box aBoxAfterCompact;
  addGraphBounds(aGraph, aBoxAfterCompact);
  expectBoxNear(aBoxAfterCompact, aBoxBeforeCompact, Precision::Confusion());

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, RemovalCompact_NoRootProducts_PreservesTopology)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;
  const BRepGraph::ShapesView::Result aBuildResult =
    aGraph.Shapes().Add(aBoxMaker.Shape(), anOptions);
  ASSERT_TRUE(aBuildResult.TopologyRoot.IsValid());
  ASSERT_TRUE(aGraph.RootProductIds().IsEmpty());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 2u);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId(2));

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GT(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aRes.NbRemovedFaces, 1u);
  EXPECT_EQ(aRes.NbUnmappedActiveDefs, 0u);
  EXPECT_TRUE(aGraph.RootProductIds().IsEmpty());

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, RemovalCompact_PreservesClosedTopologyAndValidShape)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(makeBoxWithLooseEdge());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId aLooseEdge;
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!aGraph.Topo().Edges().FacesOf(anEdgeIt.CurrentId()).More())
    {
      aLooseEdge = anEdgeIt.CurrentId();
      break;
    }
  }
  ASSERT_TRUE(aLooseEdge.IsValid());

  aGraph.Editor().Gen().RemoveNode(aLooseEdge);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GT(aRes.NbNodesBefore, aRes.NbNodesAfter);

  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 1);

  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(aGraph); aWireIt.More(); aWireIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Wire::IsClosed(aGraph, aWireIt.CurrentId()));
  }
  EXPECT_TRUE(BRepGraph_Tool::Shell::IsClosed(aGraph, BRepGraph_ShellId::Start()));

  const TopoDS_Shape aRootShape =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aGraph.RootProductIds().Value(0)));
  ASSERT_FALSE(aRootShape.IsNull());

  BRepCheck_Analyzer anAnalyzer(aRootShape);
  EXPECT_TRUE(anAnalyzer.IsValid());

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, AuditMode_PassesAfterDedupCompact)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());

  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());
}

TEST(BRepGraph_CompactTest, AuditMode_PassesAfterRemovalCompact)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(makeBoxWithLooseEdge());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId aLooseEdge;
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!aGraph.Topo().Edges().FacesOf(anEdgeIt.CurrentId()).More())
    {
      aLooseEdge = anEdgeIt.CurrentId();
      break;
    }
  }
  ASSERT_TRUE(aLooseEdge.IsValid());

  aGraph.Editor().Gen().RemoveNode(aLooseEdge);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());
}

TEST(BRepGraph_CompactTest, Compact_PreservesTopologyUIDs)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Collect the set of all original topology UIDs before dedup+compact.
  // Helper: record UIDs for all defs of a given kind.
  auto collectUIDs = [&](BRepGraph_NodeId::Kind theKind, uint32_t theCount) {
    NCollection_FlatMap<BRepGraph_UID> aMap;
    for (uint32_t anIdx = 0; anIdx < theCount; ++anIdx)
    {
      const BRepGraph_UID aUID = aGraph.UIDs().Of(BRepGraph_NodeId(theKind, anIdx));
      EXPECT_TRUE(aUID.IsValid());
      aMap.Add(aUID);
    }
    return aMap;
  };

  const NCollection_FlatMap<BRepGraph_UID> anOrigVertexUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Topo().Vertices().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigEdgeUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().Edges().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigWireUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().Wires().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigFaceUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().Faces().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigShellUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().Shells().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigCompoundUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Compound, aGraph.Topo().Compounds().Nb());
  const NCollection_FlatMap<BRepGraph_UID> anOrigCompSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::CompSolid, aGraph.Topo().CompSolids().Nb());
  // Geometry is now stored inline on defs; no separate geometry UIDs to collect.

  const uint32_t aGenBefore = aGraph.UIDs().Generation();

  // Run dedup + compact.
  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // Generation must be preserved across compact.
  EXPECT_EQ(aGraph.UIDs().Generation(), aGenBefore);

  // Helper: verify surviving defs of a kind retain original UIDs.
  auto verifyUIDs = [&](BRepGraph_NodeId::Kind                    theKind,
                        uint32_t                                  theCount,
                        const NCollection_FlatMap<BRepGraph_UID>& theOriginals,
                        const char*                               theLabel) {
    for (uint32_t anIdx = 0; anIdx < theCount; ++anIdx)
    {
      const BRepGraph_NodeId aNewId(theKind, anIdx);
      const BRepGraph_UID    aNewUID = aGraph.UIDs().Of(aNewId);
      EXPECT_TRUE(aNewUID.IsValid()) << theLabel << " " << anIdx << " lost UID after compact";
      EXPECT_TRUE(theOriginals.Contains(aNewUID))
        << theLabel << " " << anIdx << " has UID not present before compact";
      EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aNewUID), aNewId);
      EXPECT_TRUE(aGraph.UIDs().Has(aNewUID));
    }
  };

  verifyUIDs(BRepGraph_NodeId::Kind::Vertex,
             aGraph.Topo().Vertices().Nb(),
             anOrigVertexUIDs,
             "Vertex");
  verifyUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().Edges().Nb(), anOrigEdgeUIDs, "Edge");
  verifyUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().Wires().Nb(), anOrigWireUIDs, "Wire");
  verifyUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().Faces().Nb(), anOrigFaceUIDs, "Face");
  verifyUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().Shells().Nb(), anOrigShellUIDs, "Shell");
  verifyUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb(), anOrigSolidUIDs, "Solid");
  verifyUIDs(BRepGraph_NodeId::Kind::Compound,
             aGraph.Topo().Compounds().Nb(),
             anOrigCompoundUIDs,
             "Compound");
  verifyUIDs(BRepGraph_NodeId::Kind::CompSolid,
             aGraph.Topo().CompSolids().Nb(),
             anOrigCompSolidUIDs,
             "CompSolid");

  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
}

TEST(BRepGraph_CompactTest, Compact_PreservesRepresentationUIDs)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceSurfaceRepId aSurfaceRepId =
    aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId;
  ASSERT_TRUE(aSurfaceRepId.IsValid());

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_FaceSurfaceRepId aCompactedRepId =
    aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId;
  EXPECT_TRUE(aCompactedRepId.IsValid());
  EXPECT_EQ(aCompactedRepId, aSurfaceRepId);
}

TEST(BRepGraph_CompactTest, OwnGen_SurvivesCompact)
{
  constexpr double   THE_MUTATED_EDGE_TOLERANCE = 0.2;
  constexpr uint32_t THE_EXPECTED_OWN_GEN       = 2;

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Mutate edge 0 twice so OwnGen == THE_EXPECTED_OWN_GEN.
  aGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.1);
  aGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), THE_MUTATED_EDGE_TOLERANCE);
  ASSERT_EQ(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen,
            THE_EXPECTED_OWN_GEN);

  // Run dedup + compact.
  std::ignore = BRepGraph_Deduplicate::Perform(aGraph);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // Edge 0 may have been remapped. Find the edge that carries the mutated
  // tolerance and verify both the tolerance value and OwnGen are preserved.
  bool aFound = false;
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
       anEdgeId.IsValid(aGraph.Topo().Edges().Nb());
       ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = aGraph.Topo().Edges().Definition(anEdgeId);
    if (std::abs(anEdge.Tolerance - THE_MUTATED_EDGE_TOLERANCE) < Precision::Confusion())
    {
      EXPECT_EQ(anEdge.OwnGen, THE_EXPECTED_OWN_GEN)
        << "Edge " << anEdgeId.Index << " has mutated tolerance but wrong OwnGen";
      aFound = true;
      break;
    }
  }
  EXPECT_TRUE(aFound) << "No edge with mutated tolerance found after compact";
}

TEST(BRepGraph_CompactTest, UIDRoundTrip_AfterCompaction)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_GE(aGraph.Topo().Edges().Nb(), 3);

  // Record UIDs for a few face and edge nodes.
  const BRepGraph_UID aFaceUID0  = aGraph.UIDs().Of(BRepGraph_FaceId::Start());
  const BRepGraph_UID aFaceUID1  = aGraph.UIDs().Of(BRepGraph_FaceId(1));
  const BRepGraph_UID aFaceUID2  = aGraph.UIDs().Of(BRepGraph_FaceId(2));
  const BRepGraph_UID anEdgeUID0 = aGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  const BRepGraph_UID anEdgeUID1 = aGraph.UIDs().Of(BRepGraph_EdgeId(1));
  ASSERT_TRUE(aFaceUID0.IsValid());
  ASSERT_TRUE(aFaceUID1.IsValid());
  ASSERT_TRUE(aFaceUID2.IsValid());
  ASSERT_TRUE(anEdgeUID0.IsValid());
  ASSERT_TRUE(anEdgeUID1.IsValid());

  // Record the UID of the face to be removed.
  const BRepGraph_UID aRemovedFaceUID = aGraph.UIDs().Of(BRepGraph_FaceId(2));

  // Remove one face.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId(2));

  // Run compaction.
  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GE(aRes.NbRemovedFaces, 1);

  // Surviving UIDs should resolve to valid NodeIds.
  const BRepGraph_NodeId aFace0After  = aGraph.UIDs().NodeIdFrom(aFaceUID0);
  const BRepGraph_NodeId aFace1After  = aGraph.UIDs().NodeIdFrom(aFaceUID1);
  const BRepGraph_NodeId anEdge0After = aGraph.UIDs().NodeIdFrom(anEdgeUID0);
  const BRepGraph_NodeId anEdge1After = aGraph.UIDs().NodeIdFrom(anEdgeUID1);
  EXPECT_TRUE(aFace0After.IsValid()) << "Face 0 UID lost after compaction";
  EXPECT_TRUE(aFace1After.IsValid()) << "Face 1 UID lost after compaction";
  EXPECT_TRUE(anEdge0After.IsValid()) << "Edge 0 UID lost after compaction";
  EXPECT_TRUE(anEdge1After.IsValid()) << "Edge 1 UID lost after compaction";

  // The removed face's UID should no longer resolve to a valid NodeId.
  const BRepGraph_NodeId aRemovedAfter = aGraph.UIDs().NodeIdFrom(aRemovedFaceUID);
  EXPECT_FALSE(aRemovedAfter.IsValid()) << "Removed face UID still resolves after compaction";
}

TEST(BRepGraph_CompactTest, CoEdgeUID_AfterCompaction)
{
  // Verify that CoEdge entity UIDs survive compaction (previously missing from transferUIDs).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Wires().Nb(), 1);

  // Capture a CoEdge from Face 1 (Face 0 will be removed before compact).
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2);
  BRepGraph_CoEdgeId aCoEdgeId;
  {
    const BRepGraphInc::FaceRelations& aFace1Relations =
      aGraph.Topo().Faces().Relations(BRepGraph_FaceId(1));
    ASSERT_FALSE(aFace1Relations.WireRefIds.IsEmpty());

    const BRepGraphInc::WireRef& aWireRef =
      aGraph.Refs().Wires().Entry(aFace1Relations.WireRefIds.First());
    const BRepGraphInc::WireRelations& aWireRelations =
      aGraph.Topo().Wires().Relations(aWireRef.ChildWireId);
    ASSERT_FALSE(aWireRelations.CoEdgeIds.IsEmpty());
    aCoEdgeId = aWireRelations.CoEdgeIds.First();
  }
  ASSERT_TRUE(aCoEdgeId.IsValid()) << "No surviving CoEdge found in the graph";

  const BRepGraph_UID aCoEdgeUID = aGraph.UIDs().Of(aCoEdgeId);
  ASSERT_TRUE(aCoEdgeUID.IsValid()) << "CoEdge has no valid UID before compact";

  // Remove one face and compact to trigger index remapping.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());
  const BRepGraph_Compact::Result aCompactResult = BRepGraph_Compact::Perform(aGraph);
  ASSERT_GT(aCompactResult.NbNodesBefore, aCompactResult.NbNodesAfter);

  // CoEdge UID must resolve to a valid CoEdgeId after compact.
  const BRepGraph_NodeId aResolved = aGraph.UIDs().NodeIdFrom(aCoEdgeUID);
  EXPECT_TRUE(aResolved.IsValid()) << "CoEdge UID lost after compaction";
  EXPECT_EQ(aResolved.NodeKind, BRepGraph_NodeId::Kind::CoEdge)
    << "CoEdge UID resolved to wrong kind";
}

TEST(BRepGraph_CompactTest, Compact_PreservesManyFaceBoundFreePCurves)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aRemovedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(-10.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aRemovedVertex.IsValid());

  const BRepGraph_FaceId aFace = aGraph.Editor().Faces().Add(occ::handle<Geom_Surface>(),
                                                             BRepGraph_WireId(),
                                                             NCollection_Array1<BRepGraph_WireId>(),
                                                             1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  constexpr uint32_t THE_NB_FREE_PCURVES = 160;
  for (uint32_t anIdx = 0; anIdx < THE_NB_FREE_PCURVES; ++anIdx)
  {
    const double             anX    = static_cast<double>(anIdx);
    const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(anX, 0.0, 0.0), 1.e-7);
    const BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(anX, 1.0, 0.0), 1.e-7);
    ASSERT_TRUE(aStart.IsValid());
    ASSERT_TRUE(anEnd.IsValid());

    const BRepGraph_EdgeId anEdge =
      aGraph.Editor().Edges().Add(aStart, anEnd, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
    ASSERT_TRUE(anEdge.IsValid());

    const occ::handle<Geom2d_Line> aPCurve =
      new Geom2d_Line(gp_Pnt2d(0.0, anX), gp_Dir2d(1.0, 0.0));
    const BRepGraph_CoEdgeId aCoEdge =
      aGraph.Editor().CoEdges().Add(anEdge, aFace, aPCurve, 0.0, 1.0, TopAbs_FORWARD);
    ASSERT_TRUE(aCoEdge.IsValid());
  }
  ASSERT_EQ(countFaceBoundPCurveCoEdges(aGraph), THE_NB_FREE_PCURVES);

  aGraph.Editor().Gen().RemoveNode(aRemovedVertex);

  const BRepGraph_Compact::Result aCompactResult = BRepGraph_Compact::Perform(aGraph);
  ASSERT_GT(aCompactResult.NbNodesBefore, aCompactResult.NbNodesAfter);
  EXPECT_EQ(aCompactResult.NbRemovedVertices, 1u);
  EXPECT_EQ(countFaceBoundPCurveCoEdges(aGraph), THE_NB_FREE_PCURVES);
}

TEST(BRepGraph_CompactTest, Compact_PreservesDistinctFreeCoEdgesOnSameEdgeAndFace)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aRemoved = aGraph.Editor().Vertices().Add(gp_Pnt(-1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aStart   = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd    = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  const BRepGraph_FaceId aFace = aGraph.Editor().Faces().Add(occ::handle<Geom_Surface>(),
                                                             BRepGraph_WireId(),
                                                             NCollection_Array1<BRepGraph_WireId>(),
                                                             1.e-7);
  ASSERT_TRUE(aRemoved.IsValid());
  ASSERT_TRUE(anEdge.IsValid());
  ASSERT_TRUE(aFace.IsValid());

  const occ::handle<Geom2d_Line> aForwardCurve =
    new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  const occ::handle<Geom2d_Line> aReversedCurve =
    new Geom2d_Line(gp_Pnt2d(0.0, 1.0), gp_Dir2d(1.0, 0.0));
  const BRepGraph_CoEdgeId aForward =
    aGraph.Editor().CoEdges().Add(anEdge, aFace, aForwardCurve, 0.0, 1.0, TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aReversed =
    aGraph.Editor().CoEdges().Add(anEdge, aFace, aReversedCurve, 0.0, 1.0, TopAbs_REVERSED);
  ASSERT_TRUE(aForward.IsValid());
  ASSERT_TRUE(aReversed.IsValid());
  ASSERT_NE(aForward, aReversed);
  const BRepGraph_UID aForwardUID  = aGraph.UIDs().Of(aForward);
  const BRepGraph_UID aReversedUID = aGraph.UIDs().Of(aReversed);

  aGraph.Editor().Gen().RemoveNode(aRemoved);
  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);
  ASSERT_EQ(aResult.NbRemovedVertices, 1u);

  const BRepGraph_CoEdgeId aCompactedForward =
    BRepGraph_CoEdgeId::FromNodeId(aGraph.UIDs().NodeIdFrom(aForwardUID));
  const BRepGraph_CoEdgeId aCompactedReversed =
    BRepGraph_CoEdgeId::FromNodeId(aGraph.UIDs().NodeIdFrom(aReversedUID));
  ASSERT_TRUE(aCompactedForward.IsValid());
  ASSERT_TRUE(aCompactedReversed.IsValid());
  EXPECT_NE(aCompactedForward, aCompactedReversed);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCompactedForward).Orientation, TopAbs_FORWARD);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCompactedReversed).Orientation, TopAbs_REVERSED);
}

TEST(BRepGraph_CompactTest, Compact_PreservesActiveDefinitionOutsideDocumentRoots)
{
  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aBox =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  ASSERT_TRUE(aBox.IsOk());
  ASSERT_FALSE(aGraph.RootProductIds().IsEmpty());

  const BRepGraph_VertexId anOrphan =
    aGraph.Editor().Vertices().Add(gp_Pnt(10.0, 20.0, 30.0), 1.e-7);
  const BRepGraph_VertexId aRemoved =
    aGraph.Editor().Vertices().Add(gp_Pnt(-10.0, -20.0, -30.0), 1.e-7);
  ASSERT_TRUE(anOrphan.IsValid());
  ASSERT_TRUE(aRemoved.IsValid());
  const BRepGraph_UID anOrphanUID = aGraph.UIDs().Of(anOrphan);
  const size_t        aRootCount  = aGraph.RootProductIds().Size();

  aGraph.Editor().Gen().RemoveNode(aRemoved);
  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);
  ASSERT_EQ(aResult.NbRemovedVertices, 1u);

  const BRepGraph_NodeId aCompactedOrphan = aGraph.UIDs().NodeIdFrom(anOrphanUID);
  ASSERT_TRUE(aCompactedOrphan.IsValid());
  EXPECT_NEAR(aGraph.Topo()
                .Vertices()
                .Definition(BRepGraph_VertexId(aCompactedOrphan))
                .Point.Distance(gp_Pnt(10.0, 20.0, 30.0)),
              0.0,
              Precision::Confusion());
  EXPECT_EQ(aGraph.RootProductIds().Size(), aRootCount);
}

TEST(BRepGraph_CompactTest, UIDRoundTrip_RefUIDs_AfterCompaction)
{
  // Verify that all transferred RefUID kinds survive compaction.
  // Checks VertexRef, WireRef, FaceRef, ShellRef plus direct CoEdge node identity.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // VertexRef - from Edge 0 start vertex ref.
  BRepGraph_VertexRefId aVertexRefId;
  {
    const BRepGraphInc::EdgeDef& anEdge =
      aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start());
    if (anEdge.StartVertexRefId.IsValid())
    {
      aVertexRefId = anEdge.StartVertexRefId;
    }
  }

  // CoEdge - from Face 1 wire (Face 0 will be removed before compact).
  BRepGraph_CoEdgeId aCoEdgeId;
  {
    const BRepGraphInc::FaceRelations& aFaceRelations =
      aGraph.Topo().Faces().Relations(BRepGraph_FaceId(1));
    if (!aFaceRelations.WireRefIds.IsEmpty())
    {
      const BRepGraphInc::WireRef& aWireRef =
        aGraph.Refs().Wires().Entry(aFaceRelations.WireRefIds.First());
      const BRepGraphInc::WireRelations& aWireRelations =
        aGraph.Topo().Wires().Relations(aWireRef.ChildWireId);
      if (!aWireRelations.CoEdgeIds.IsEmpty())
      {
        aCoEdgeId = aWireRelations.CoEdgeIds.First();
      }
    }
  }

  // WireRef - from Face 1 first wire ref (we will remove Face 0, so use Face 1).
  BRepGraph_WireRefId aWireRefId;
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2);
  {
    const BRepGraphInc::FaceRelations& aFaceRelations =
      aGraph.Topo().Faces().Relations(BRepGraph_FaceId(1));
    if (!aFaceRelations.WireRefIds.IsEmpty())
    {
      aWireRefId = aFaceRelations.WireRefIds.First();
    }
  }

  // FaceRef - from Shell, pointing to Face 1 (skip Face 0 whose ref will be removed).
  BRepGraph_FaceRefId aFaceRefId;
  {
    for (BRepGraph_Iterator<BRepGraphInc::ShellDef> anIt(aGraph); anIt.More(); anIt.Next())
    {
      for (BRepGraph_RefsFaceOfShell aRefIt(aGraph, anIt.CurrentId()); aRefIt.More(); aRefIt.Next())
      {
        const BRepGraphInc::FaceRef& aFR = aGraph.Refs().Faces().Entry(aRefIt.CurrentId());
        // Skip Face 0 ref - that face will be removed before compact.
        if (aFR.ChildFaceId.Index != 0)
        {
          aFaceRefId = aRefIt.CurrentId();
          break;
        }
      }
      if (aFaceRefId.IsValid())
      {
        break;
      }
    }
  }

  // ShellRef - from Solid 0 via refs iterator.
  BRepGraph_ShellRefId aShellRefId;
  {
    for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(aGraph); anIt.More(); anIt.Next())
    {
      for (BRepGraph_RefsShellOfSolid aRefIt(aGraph, anIt.CurrentId()); aRefIt.More();)
      {
        aShellRefId = aRefIt.CurrentId();
        break;
      }
      if (aShellRefId.IsValid())
      {
        break;
      }
    }
  }

  // Capture RefUIDs before compact.
  BRepGraph_RefUID aVertexRefUID, aWireRefUID, aFaceRefUID, aShellRefUID;
  BRepGraph_UID    aCoEdgeUID;
  if (aVertexRefId.IsValid())
  {
    aVertexRefUID = aGraph.UIDs().Of(aVertexRefId);
  }
  if (aCoEdgeId.IsValid())
  {
    aCoEdgeUID = aGraph.UIDs().Of(aCoEdgeId);
  }
  if (aWireRefId.IsValid())
  {
    aWireRefUID = aGraph.UIDs().Of(aWireRefId);
  }
  if (aFaceRefId.IsValid())
  {
    aFaceRefUID = aGraph.UIDs().Of(aFaceRefId);
  }
  if (aShellRefId.IsValid())
  {
    aShellRefUID = aGraph.UIDs().Of(aShellRefId);
  }

  // Remove one face to trigger compaction.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // Each surviving RefUID must resolve to a valid RefId of the correct kind.
  if (aVertexRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aVertexRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "VertexRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::Vertex)
      << "VertexRef UID resolved to wrong kind";
  }
  if (aCoEdgeUID.IsValid())
  {
    const BRepGraph_NodeId aResolved = aGraph.UIDs().NodeIdFrom(aCoEdgeUID);
    EXPECT_TRUE(aResolved.IsValid()) << "CoEdge UID lost after compaction";
    EXPECT_EQ(aResolved.NodeKind, BRepGraph_NodeId::Kind::CoEdge)
      << "CoEdge UID resolved to wrong kind";
  }
  if (aWireRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aWireRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "WireRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::Wire)
      << "WireRef UID resolved to wrong kind";
  }
  if (aFaceRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aFaceRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "FaceRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::Face)
      << "FaceRef UID resolved to wrong kind";
  }
  if (aShellRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aShellRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "ShellRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::Shell)
      << "ShellRef UID resolved to wrong kind";
  }
}

//=================================================================================================

TEST(BRepGraph_CompactTest, FindNodeStillWorksAfterCompact)
{
  // Regression for Bug B2: BRepGraph_Compact must preserve the TShape-to-NodeId
  // bindings so that BRepGraph::Shapes().FindNode() / HasNode() still resolves
  // original BRepGraph::ShapesView::Add()-time shapes after compaction.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Pick one face from the original build input.
  TopoDS_Shape aFace;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More();)
  {
    aFace = anExp.Current();
    break;
  }
  ASSERT_FALSE(aFace.IsNull());
  ASSERT_TRUE(aGraph.Shapes().HasNode(aFace)) << "HasNode returned false before compact";
  const BRepGraph_NodeId aNodeIdBefore = aGraph.Shapes().FindNode(aFace);
  ASSERT_TRUE(aNodeIdBefore.IsValid()) << "FindNode returned invalid node before compact";

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // After compact the TShape binding must survive so the original face is still locatable.
  EXPECT_TRUE(aGraph.Shapes().HasNode(aFace))
    << "HasNode returned false after compact - TShape bindings were lost";
  const BRepGraph_NodeId aNodeIdAfter = aGraph.Shapes().FindNode(aFace);
  EXPECT_TRUE(aNodeIdAfter.IsValid())
    << "FindNode returned invalid node after compact - TShape bindings were lost";
}

TEST(BRepGraph_CompactTest, OwnGen_PreservedForAllTopologyKindsAfterCompact)
{
  constexpr uint32_t THE_EXPECTED_GEN = 3;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Mutate one entity of each topology kind to set a known OwnGen value.
  // After compact, the OwnGen should survive on the corresponding remapped entity.

  // Vertex: set point
  aGraph.Editor().Vertices().SetPoint(BRepGraph_VertexId::Start(), gp_Pnt(11.0, 21.0, 31.0));
  aGraph.Editor().Vertices().SetPoint(BRepGraph_VertexId::Start(), gp_Pnt(12.0, 22.0, 32.0));
  aGraph.Editor().Vertices().SetPoint(BRepGraph_VertexId::Start(), gp_Pnt(13.0, 23.0, 33.0));

  // Edge: set tolerance
  aGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.1);
  aGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.2);
  aGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.3);

  // Wire: MarkDirty directly
  aGraph.Editor().Wires().Mut(BRepGraph_WireId::Start()).MarkDirty();
  aGraph.Editor().Wires().Mut(BRepGraph_WireId::Start()).MarkDirty();
  aGraph.Editor().Wires().Mut(BRepGraph_WireId::Start()).MarkDirty();

  // Face: set tolerance
  aGraph.Editor().Faces().SetTolerance(BRepGraph_FaceId::Start(), 0.01);
  aGraph.Editor().Faces().SetTolerance(BRepGraph_FaceId::Start(), 0.02);
  aGraph.Editor().Faces().SetTolerance(BRepGraph_FaceId::Start(), 0.03);

  // Shell: MarkDirty
  aGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start()).MarkDirty();
  aGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start()).MarkDirty();
  aGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start()).MarkDirty();

  // Solid: MarkDirty
  aGraph.Editor().Solids().Mut(BRepGraph_SolidId::Start()).MarkDirty();
  aGraph.Editor().Solids().Mut(BRepGraph_SolidId::Start()).MarkDirty();
  aGraph.Editor().Solids().Mut(BRepGraph_SolidId::Start()).MarkDirty();

  // Verify OwnGen before compact.
  EXPECT_EQ(aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen,
            THE_EXPECTED_GEN);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, THE_EXPECTED_GEN);
  EXPECT_EQ(aGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).OwnGen, THE_EXPECTED_GEN);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).OwnGen, THE_EXPECTED_GEN);
  EXPECT_EQ(aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).OwnGen, THE_EXPECTED_GEN);
  EXPECT_EQ(aGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).OwnGen, THE_EXPECTED_GEN);

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // After compact (no removals, just rebuild), all OwnGen values should survive.
  // Since nothing was removed, the indices should remain at Start().
  EXPECT_EQ(aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen,
            THE_EXPECTED_GEN)
    << "Vertex OwnGen not preserved after compact";
  EXPECT_EQ(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, THE_EXPECTED_GEN)
    << "Edge OwnGen not preserved after compact";
  EXPECT_EQ(aGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).OwnGen, THE_EXPECTED_GEN)
    << "Wire OwnGen not preserved after compact";
  EXPECT_EQ(aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).OwnGen, THE_EXPECTED_GEN)
    << "Face OwnGen not preserved after compact";
  EXPECT_EQ(aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).OwnGen, THE_EXPECTED_GEN)
    << "Shell OwnGen not preserved after compact";
  EXPECT_EQ(aGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).OwnGen, THE_EXPECTED_GEN)
    << "Solid OwnGen not preserved after compact";
}

TEST(BRepGraph_CompactTest, Compact_PreservesDeletedItemUidHistory)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  occ::handle<BRepGraph_LayerHistory> aHistory =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const BRepGraph_FaceId  aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraph_ItemUID aFaceUid = aGraph.UIDs().Of(BRepGraph_ItemId(aFaceId));
  ASSERT_TRUE(aFaceUid.IsValid());

  aGraph.Editor().Gen().RemoveNode(aFaceId);
  ASSERT_TRUE(aHistory->IsDeleted(aFaceUid));
  ASSERT_TRUE(aHistory->HasKnownInput(aFaceUid));

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(aHistory->IsDeleted(aFaceUid));
  EXPECT_TRUE(aHistory->HasKnownInput(aFaceUid));
  EXPECT_TRUE(aHistory->DeletedItemUids().Contains(aFaceUid));
}

TEST(BRepGraph_CompactTest, Compact_PreservesItemUidHistoryMappings)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2u);

  occ::handle<BRepGraph_LayerHistory> aHistory =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const BRepGraph_FaceId  aOriginalFace = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId  aReplacementFace(1);
  const BRepGraph_ItemUID anOriginalUid   = aGraph.UIDs().Of(BRepGraph_ItemId(aOriginalFace));
  const BRepGraph_ItemUID aReplacementUid = aGraph.UIDs().Of(BRepGraph_ItemId(aReplacementFace));
  ASSERT_TRUE(anOriginalUid.IsValid());
  ASSERT_TRUE(aReplacementUid.IsValid());

  NCollection_LinearVector<BRepGraph_ItemUID> aReplacements;
  aReplacements.Append(aReplacementUid);
  aHistory->RecordItemUid("CompactItemHistory", anOriginalUid, aReplacements.ToArray1());
  ASSERT_TRUE(aHistory->HasKnownInput(anOriginalUid));

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const NCollection_LinearVector<BRepGraph_ItemUID>* aModified =
    aHistory->FindModified(anOriginalUid);
  ASSERT_NE(aModified, nullptr);
  ASSERT_EQ(aModified->Size(), size_t(1));
  EXPECT_EQ(aModified->Value(0), aReplacementUid);
  EXPECT_TRUE(aHistory->HasKnownInput(anOriginalUid));
}

TEST(BRepGraph_CompactTest, Compact_AfterDedupMerge_NoBoundsErrors)
{
  // Build 3 identical boxes via deep copy so dedup can merge topology nodes.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepBuilderAPI_Copy aCopy1(aBoxMaker.Shape(), true);
  BRepBuilderAPI_Copy aCopy2(aBoxMaker.Shape(), true);
  BRepBuilderAPI_Copy aCopy3(aBoxMaker.Shape(), true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());
  aBuilder.Add(aCompound, aCopy3.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_NE(aRes.NbNodesAfter, 0u);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12u);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6u);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, Compact_FaceWithInnerWires_WireCountPreserved)
{
  // Build a box with a cylindrical hole so that some faces have inner wires.
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);

  gp_Ax2                   anAxes(gp_Pnt(10.0, 10.0, 0.0), gp::DZ());
  BRepPrimAPI_MakeCylinder aCylMaker(anAxes, 4.0, 25.0);

  BRepAlgoAPI_Cut aCut(aBoxMaker.Shape(), aCylMaker.Shape());
  ASSERT_TRUE(aCut.IsDone());

  BRepBuilderAPI_Copy aCopy1(aCut.Shape(), true);
  BRepBuilderAPI_Copy aCopy2(aCut.Shape(), true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Verify before dedup that the shape produces faces with inner wires (WireRefIds > 1).
  bool aHasInnerWires = false;
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(aGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    if (aGraph.Topo().Faces().Relations(aFaceId).WireRefIds.Size() > 1)
    {
      aHasInnerWires = true;
      break;
    }
  }
  ASSERT_TRUE(aHasInnerWires);

  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_NE(aRes.NbNodesAfter, 0u);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, HistorySurvivesCompactWithRemappedIds)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBoxMaker1.Shape());
  aBuilder.Add(aCompound, aBoxMaker2.Shape());

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(true);
  const BRepGraph_NodeId                     aVertexNode = BRepGraph_VertexId::Start();
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(aVertexNode);
  aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
    TCollection_AsciiString("Test:Modify"),
    aVertexNode,
    aRepl.ToArray1());
  const size_t aNbBefore = aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  ASSERT_GT(aNbBefore, 0u);

  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  std::ignore = BRepGraph_Compact::Perform(aGraph);

  EXPECT_GE(aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore);
  EXPECT_EQ(countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap")), 0);
}

TEST(BRepGraph_CompactTest, PreservesPersistentMeshAndSemanticState)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aTombstone =
    aGraph.Editor().Vertices().Add(gp_Pnt(-1.0, -1.0, -1.0), Precision::Confusion());
  ASSERT_TRUE(aTombstone.IsValid());
  aGraph.Editor().Gen().RemoveNode(aTombstone);
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape()).IsOk());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  const occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);
  const occ::handle<Poly_Polygon3D>     aPolygon3D     = new Poly_Polygon3D(2, false);
  const occ::handle<Poly_Polygon2D>     aPolygon2D     = new Poly_Polygon2D(2);
  const occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  const BRepGraph_UID             aFaceUID      = aGraph.UIDs().Of(aFaceId);
  const BRepGraph_UID             anEdgeUID     = aGraph.UIDs().Of(anEdgeId);
  const BRepGraph_UID             aCoEdgeUID    = aGraph.UIDs().Of(aCoEdgeId);
  const BRepGraph_RevisionHash    aSemanticHash = BRepGraph_RevisionHash::Hasher::Semantic(aGraph);
  const BRepGraph_Compact::Result aResult       = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aResult.NbRemovedVertices, 1u);

  const BRepGraph_FaceId   aNewFace(aGraph.UIDs().NodeIdFrom(aFaceUID));
  const BRepGraph_EdgeId   aNewEdge(aGraph.UIDs().NodeIdFrom(anEdgeUID));
  const BRepGraph_CoEdgeId aNewCoEdge(aGraph.UIDs().NodeIdFrom(aCoEdgeUID));
  ASSERT_TRUE(aNewFace.IsValid());
  ASSERT_TRUE(aNewEdge.IsValid());
  ASSERT_TRUE(aNewCoEdge.IsValid());
  EXPECT_EQ(aGraph.Mesh().Persistent().Faces().Triangulation(aNewFace), aTriangulation);
  EXPECT_EQ(aGraph.Mesh().Persistent().Edges().Polygon3D(aNewEdge), aPolygon3D);
  EXPECT_EQ(aGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(aNewCoEdge), aPolygon2D);
  EXPECT_EQ(aGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aNewCoEdge), aPolygonOnTri);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aSemanticHash);
}

TEST(BRepGraph_CompactTest, DoesNotReuseRemovedHighUID)
{
  BRepGraph aGraph;
  std::ignore = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_VertexId aRemoved =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_UID aRemovedUID = aGraph.UIDs().Of(aRemoved);
  aGraph.Editor().Gen().RemoveNode(aRemoved);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_VertexId aNext =
    aGraph.Editor().Vertices().Add(gp_Pnt(2.0, 0.0, 0.0), Precision::Confusion());
  EXPECT_GT(aGraph.UIDs().Of(aNext).Counter, aRemovedUID.Counter);
}

namespace
{
class StubMeshDriver : public BRepGraph_CacheMesh::Driver
{
public:
  DEFINE_STANDARD_RTTI_INLINE(StubMeshDriver, BRepGraph_CacheMesh::Driver)

  explicit StubMeshDriver(size_t theHash)
      : myHash(theHash)
  {
  }

  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee");
    return THE_ID;
  }

  size_t RecipeHash() const override { return myHash; }

  bool Fill(BRepGraph&,
            BRepGraph_CacheMesh::SlotId,
            const BRepGraph_CacheMesh::DirtySet&,
            const Message_ProgressRange&) override
  {
    return true;
  }

  occ::handle<BRepGraph_CacheMesh::Driver> Copy(const BRepGraph_CopyRemap& theCopy) const override
  {
    if (theCopy.IsCompact())
    {
      return occ::handle<BRepGraph_CacheMesh::Driver>(const_cast<StubMeshDriver*>(this));
    }
    return new StubMeshDriver(myHash);
  }

private:
  size_t myHash;
};
} // namespace

TEST(BRepGraph_CompactTest, CacheMesh_DriverSurvivesCompact)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Register CacheMesh, register a driver, and populate a face entry.
  occ::handle<BRepGraph_CacheMesh> aCache = aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCache.IsNull());

  constexpr size_t THE_RECIPE_HASH = 42;
  auto             aDriver         = new StubMeshDriver(THE_RECIPE_HASH);
  aCache->RegisterDriver(BRepGraph_CacheMesh::DefaultDisplaySlot, aDriver);

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTri->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTri->SetNode(3, gp_Pnt(0.0, 1.0, 0.0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);
  ASSERT_NE(aCache->FindFaceMesh(aFaceId), nullptr);

  // Verify driver is registered before compact.
  ASSERT_FALSE(aCache->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot).IsNull());
  ASSERT_TRUE(aCache->State(BRepGraph_CacheMesh::DefaultDisplaySlot).HasDriver);

  // Compact.
  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbRemovedVertices, 0u);

  // Cache object must survive compaction.
  occ::handle<BRepGraph_CacheMesh> aCacheAfter = aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCacheAfter.IsNull());
  EXPECT_EQ(aCache.get(), aCacheAfter.get()) << "Cache handle must be the same object";

  // Driver must survive compaction.
  const occ::handle<BRepGraph_CacheMesh::Driver>& aDriverAfter =
    aCacheAfter->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot);
  ASSERT_FALSE(aDriverAfter.IsNull());
  EXPECT_EQ(aDriverAfter->RecipeHash(), THE_RECIPE_HASH);
  EXPECT_TRUE(aCacheAfter->State(BRepGraph_CacheMesh::DefaultDisplaySlot).HasDriver);

  // No removed nodes - compact short-circuits, cache representation is untouched.
  EXPECT_NE(aCacheAfter->FindFaceMesh(aFaceId), nullptr)
    << "Cache entry must survive when compact short-circuits (no removed nodes)";
}

TEST(BRepGraph_CompactTest, CacheMesh_DriverSurvivesCompactWithDedup)
{
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Register CacheMesh with driver and populate entries for all faces.
  occ::handle<BRepGraph_CacheMesh> aCache = aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCache.IsNull());

  constexpr size_t THE_RECIPE_HASH = 99;
  auto             aDriver         = new StubMeshDriver(THE_RECIPE_HASH);
  aCache->RegisterDriver(BRepGraph_CacheMesh::DefaultDisplaySlot, aDriver);

  const occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTri->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTri->SetNode(3, gp_Pnt(0.0, 1.0, 0.0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));

  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  ASSERT_GT(aNbFaces, 0u);
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start(); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);
  }

  // Deduplicate + Compact.
  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GT(aRes.NbRemovedVertices + aRes.NbRemovedEdges + aRes.NbRemovedFaces, 0u)
    << "Deduplicate should have removed some nodes";

  // Cache object and driver must survive.
  occ::handle<BRepGraph_CacheMesh> aCacheAfter = aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCacheAfter.IsNull());
  EXPECT_NE(aCache.get(), aCacheAfter.get());

  const occ::handle<BRepGraph_CacheMesh::Driver>& aDriverAfter =
    aCacheAfter->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot);
  ASSERT_FALSE(aDriverAfter.IsNull());
  EXPECT_EQ(aDriverAfter->RecipeHash(), THE_RECIPE_HASH);

  // Cache representation is cleared - no entries should remain.
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(aGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    EXPECT_EQ(aCacheAfter->FindFaceMesh(aFaceId), nullptr)
      << "Cache representation must be cleared after compact";
  }
}

TEST(BRepGraph_CompactTest, CacheMesh_CanCopyFreshEntriesThroughCompactAndPreserveDriver)
{
  BRepGraph aGraph;
  std::ignore = aGraph.Shapes().Add(makeTwoCopiedFaces());
  ASSERT_FALSE(aGraph.IsEmpty());

  occ::handle<BRepGraph_CacheMesh> aCache = aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCache.IsNull());

  constexpr size_t THE_RECIPE_HASH = 77;
  auto             aDriver         = new StubMeshDriver(THE_RECIPE_HASH);
  aCache->RegisterDriver(BRepGraph_CacheMesh::DefaultDisplaySlot, aDriver);

  const occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTri->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTri->SetNode(3, gp_Pnt(0.0, 1.0, 0.0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));

  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  ASSERT_GT(aNbFaces, 0u);
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start(); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    if (aFaceId.IsRemoved(aGraph))
    {
      continue;
    }
    aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);
    ASSERT_NE(aCache->FindFaceMesh(aFaceId), nullptr);
  }

  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_GT(aRes.NbRemovedVertices + aRes.NbRemovedEdges + aRes.NbRemovedFaces, 0u);

  occ::handle<BRepGraph_CacheMesh> aCacheAfter = aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCacheAfter.IsNull());
  EXPECT_NE(aCacheAfter.get(), aCache.get());
  const occ::handle<BRepGraph_CacheMesh::Driver> aDriverAfter =
    aCacheAfter->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot);
  ASSERT_FALSE(aDriverAfter.IsNull());
  EXPECT_EQ(aDriverAfter.get(), aCache->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot).get());
  EXPECT_EQ(aDriverAfter->RecipeHash(), THE_RECIPE_HASH);
  const BRepGraph_CacheMesh::SlotState aState =
    aCacheAfter->State(BRepGraph_CacheMesh::DefaultDisplaySlot);
  EXPECT_TRUE(aState.HasDriver);
  EXPECT_EQ(aState.RecipeHash, THE_RECIPE_HASH);

  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(aGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    const BRepGraph_CacheMesh::FaceMeshEntry* anEntry = aCacheAfter->FindFaceMesh(aFaceId);
    ASSERT_NE(anEntry, nullptr);
    EXPECT_EQ(anEntry->Triangulation.get(), aTri.get());
  }
}
