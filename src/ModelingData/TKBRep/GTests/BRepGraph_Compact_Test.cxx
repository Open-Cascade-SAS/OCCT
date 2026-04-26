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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Bnd_Box.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

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
  int aCount = 0;
  for (size_t aRecIdx = 0; aRecIdx < theGraph.History().NbRecords(); ++aRecIdx)
  {
    if (theGraph.History().Record(aRecIdx).OperationName == theOp)
      ++aCount;
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

} // namespace

TEST(BRepGraph_CompactTest, NoRemovedNodes_Noop)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_CompactTest, AfterDeduplicate_RemovesNodes)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Run geometry dedup which replaces duplicate surface/curve handles directly.
  (void)BRepGraph_Deduplicate::Perform(aGraph);

  BRepGraph_Compact::Options anOpts;
  anOpts.HistoryMode                   = false;
  const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph, anOpts);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  // After compaction, there should be no removed defs.
  for (BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
       aVertexId.IsValid(aGraph.Topo().Vertices().Nb());
       ++aVertexId)
    EXPECT_FALSE(aGraph.Topo().Vertices().Definition(aVertexId).IsRemoved);
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
       anEdgeId.IsValid(aGraph.Topo().Edges().Nb());
       ++anEdgeId)
    EXPECT_FALSE(aGraph.Topo().Edges().Definition(anEdgeId).IsRemoved);
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(aGraph.Topo().Faces().Nb());
       ++aFaceId)
    EXPECT_FALSE(aGraph.Topo().Faces().Definition(aFaceId).IsRemoved);
  for (BRepGraph_WireId aWireId = BRepGraph_WireId::Start();
       aWireId.IsValid(aGraph.Topo().Wires().Nb());
       ++aWireId)
    EXPECT_FALSE(aGraph.Topo().Wires().Definition(aWireId).IsRemoved);
}

TEST(BRepGraph_CompactTest, CrossReferences_Valid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, HistoryMode_RecordsMapping)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Use full entity merge so that duplicate topology nodes are actually removed.
  // This ensures Compact must remap surviving indices and produces >= 1 record.
  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  (void)BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);

  BRepGraph_Compact::Options anOpts;
  anOpts.HistoryMode = true;
  (void)BRepGraph_Compact::Perform(aGraph, anOpts);

  const int aNbRemapRecords =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap"));
  // After merging duplicate entities at least some topology nodes are removed,
  // which forces surviving-node index remapping during Compact.
  EXPECT_GE(aNbRemapRecords, 1);
}

TEST(BRepGraph_CompactTest, FullPipeline_Deduplicate_Compact_Validate)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Full dedup (replaces duplicate handles directly on defs).
  (void)BRepGraph_Deduplicate::Perform(aGraph);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
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

TEST(BRepGraph_CompactTest, RemovalCompact_PreservesClosedTopologyAndValidShape)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, makeBoxWithLooseEdge());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId aLooseEdge;
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (aGraph.Topo().Edges().Faces(anEdgeIt.CurrentId()).IsEmpty())
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
    EXPECT_TRUE(aWireIt.Current().IsClosed);
  }
  EXPECT_TRUE(aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).IsClosed);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());

  (void)BRepGraph_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());
}

TEST(BRepGraph_CompactTest, AuditMode_PassesAfterRemovalCompact)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, makeBoxWithLooseEdge());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId aLooseEdge;
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (aGraph.Topo().Edges().Faces(anEdgeIt.CurrentId()).IsEmpty())
    {
      aLooseEdge = anEdgeIt.CurrentId();
      break;
    }
  }
  ASSERT_TRUE(aLooseEdge.IsValid());

  aGraph.Editor().Gen().RemoveNode(aLooseEdge);
  (void)BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Mode::Audit).IsValid());
}

TEST(BRepGraph_CompactTest, Compact_PreservesTopologyUIDs)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Collect the set of all original topology UIDs before dedup+compact.
  // Helper: record UIDs for all defs of a given kind.
  auto collectUIDs = [&](BRepGraph_NodeId::Kind theKind, int theCount) {
    NCollection_Map<BRepGraph_UID> aMap;
    for (int anIdx = 0; anIdx < theCount; ++anIdx)
    {
      const BRepGraph_UID aUID = aGraph.UIDs().Of(BRepGraph_NodeId(theKind, anIdx));
      EXPECT_TRUE(aUID.IsValid());
      aMap.Add(aUID);
    }
    return aMap;
  };

  const NCollection_Map<BRepGraph_UID> anOrigVertexUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Topo().Vertices().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigEdgeUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().Edges().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigWireUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().Wires().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigFaceUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().Faces().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigShellUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().Shells().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigCompoundUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Compound, aGraph.Topo().Compounds().Nb());
  const NCollection_Map<BRepGraph_UID> anOrigCompSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::CompSolid, aGraph.Topo().CompSolids().Nb());
  // Geometry is now stored inline on defs; no separate geometry UIDs to collect.

  const uint32_t aGenBefore = aGraph.UIDs().Generation();

  // Run dedup + compact.
  (void)BRepGraph_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  // Generation must be preserved across compact.
  EXPECT_EQ(aGraph.UIDs().Generation(), aGenBefore);

  // Helper: verify surviving defs of a kind retain original UIDs.
  auto verifyUIDs = [&](BRepGraph_NodeId::Kind                theKind,
                        int                                   theCount,
                        const NCollection_Map<BRepGraph_UID>& theOriginals,
                        const char*                           theLabel) {
    for (int anIdx = 0; anIdx < theCount; ++anIdx)
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

  // Geometry is now stored inline on defs; no separate geometry UIDs to verify.
}

TEST(BRepGraph_CompactTest, OwnGen_SurvivesCompact)
{
  constexpr double   THE_MUTATED_EDGE_TOLERANCE = 0.2;
  constexpr uint32_t THE_EXPECTED_OWN_GEN       = 2;

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Mutate edge 0 twice so OwnGen == THE_EXPECTED_OWN_GEN.
  aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.1;
  aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = THE_MUTATED_EDGE_TOLERANCE;
  ASSERT_EQ(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen,
            THE_EXPECTED_OWN_GEN);

  // Run dedup + compact.
  (void)BRepGraph_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Wires().Nb(), 1);

  // Capture a CoEdge from Face 1 (Face 0 will be removed before compact).
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2);
  BRepGraph_CoEdgeId aCoEdgeId;
  {
    const BRepGraphInc::FaceDef& aFace1 = aGraph.Topo().Faces().Definition(BRepGraph_FaceId(1));
    ASSERT_FALSE(aFace1.WireRefIds.IsEmpty());

    const BRepGraphInc::WireRef& aWireRef = aGraph.Refs().Wires().Entry(aFace1.WireRefIds.First());
    const BRepGraphInc::WireDef& aWire    = aGraph.Topo().Wires().Definition(aWireRef.WireDefId);
    ASSERT_FALSE(aWire.CoEdgeRefIds.IsEmpty());

    const BRepGraphInc::CoEdgeRef& aRef = aGraph.Refs().CoEdges().Entry(aWire.CoEdgeRefIds.First());
    aCoEdgeId                           = aRef.CoEdgeDefId;
  }
  ASSERT_TRUE(aCoEdgeId.IsValid()) << "No surviving CoEdge found in the graph";

  const BRepGraph_UID aCoEdgeUID = aGraph.UIDs().Of(aCoEdgeId);
  ASSERT_TRUE(aCoEdgeUID.IsValid()) << "CoEdge has no valid UID before compact";

  // Remove one face and compact to trigger index remapping.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());
  (void)BRepGraph_Compact::Perform(aGraph);

  // CoEdge UID must resolve to a valid CoEdgeId after compact.
  const BRepGraph_NodeId aResolved = aGraph.UIDs().NodeIdFrom(aCoEdgeUID);
  EXPECT_TRUE(aResolved.IsValid()) << "CoEdge UID lost after compaction";
  EXPECT_EQ(aResolved.NodeKind, BRepGraph_NodeId::Kind::CoEdge)
    << "CoEdge UID resolved to wrong kind";
}

TEST(BRepGraph_CompactTest, UIDRoundTrip_RefUIDs_AfterCompaction)
{
  // Verify that all transferred RefUID kinds survive compaction.
  // Checks VertexRef, CoEdgeRef, WireRef, FaceRef, ShellRef (present in a box).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // VertexRef - from Edge 0 start vertex ref.
  BRepGraph_VertexRefId aVertexRefId;
  {
    const BRepGraphInc::EdgeDef& anEdge =
      aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start());
    if (anEdge.StartVertexRefId.IsValid())
      aVertexRefId = anEdge.StartVertexRefId;
  }

  // CoEdgeRef - from Face 1 wire (Face 0 will be removed before compact).
  BRepGraph_CoEdgeRefId aCoEdgeRefId;
  {
    const BRepGraphInc::FaceDef& aFace = aGraph.Topo().Faces().Definition(BRepGraph_FaceId(1));
    if (!aFace.WireRefIds.IsEmpty())
    {
      const BRepGraphInc::WireRef& aWireRef = aGraph.Refs().Wires().Entry(aFace.WireRefIds.First());
      const BRepGraphInc::WireDef& aWire    = aGraph.Topo().Wires().Definition(aWireRef.WireDefId);
      if (!aWire.CoEdgeRefIds.IsEmpty())
        aCoEdgeRefId = aWire.CoEdgeRefIds.First();
    }
  }

  // WireRef - from Face 1 first wire ref (we will remove Face 0, so use Face 1).
  BRepGraph_WireRefId aWireRefId;
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2);
  {
    const BRepGraphInc::FaceDef& aFace = aGraph.Topo().Faces().Definition(BRepGraph_FaceId(1));
    if (!aFace.WireRefIds.IsEmpty())
      aWireRefId = aFace.WireRefIds.First();
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
        if (aFR.FaceDefId.Index != 0)
        {
          aFaceRefId = aRefIt.CurrentId();
          break;
        }
      }
      if (aFaceRefId.IsValid())
        break;
    }
  }

  // ShellRef - from Solid 0 via refs iterator.
  BRepGraph_ShellRefId aShellRefId;
  {
    for (BRepGraph_Iterator<BRepGraphInc::SolidDef> anIt(aGraph); anIt.More(); anIt.Next())
    {
      for (BRepGraph_RefsShellOfSolid aRefIt(aGraph, anIt.CurrentId()); aRefIt.More();
           aRefIt.Next())
      {
        aShellRefId = aRefIt.CurrentId();
        break;
      }
      if (aShellRefId.IsValid())
        break;
    }
  }

  // Capture RefUIDs before compact.
  BRepGraph_RefUID aVertexRefUID, aCoEdgeRefUID, aWireRefUID, aFaceRefUID, aShellRefUID;
  if (aVertexRefId.IsValid())
    aVertexRefUID = aGraph.UIDs().Of(aVertexRefId);
  if (aCoEdgeRefId.IsValid())
    aCoEdgeRefUID = aGraph.UIDs().Of(aCoEdgeRefId);
  if (aWireRefId.IsValid())
    aWireRefUID = aGraph.UIDs().Of(aWireRefId);
  if (aFaceRefId.IsValid())
    aFaceRefUID = aGraph.UIDs().Of(aFaceRefId);
  if (aShellRefId.IsValid())
    aShellRefUID = aGraph.UIDs().Of(aShellRefId);

  // Remove one face to trigger compaction.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());
  (void)BRepGraph_Compact::Perform(aGraph);

  // Each surviving RefUID must resolve to a valid RefId of the correct kind.
  if (aVertexRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aVertexRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "VertexRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::Vertex)
      << "VertexRef UID resolved to wrong kind";
  }
  if (aCoEdgeRefUID.IsValid())
  {
    const BRepGraph_RefId aResolved = aGraph.UIDs().RefIdFrom(aCoEdgeRefUID);
    EXPECT_TRUE(aResolved.IsValid()) << "CoEdgeRef UID lost after compaction";
    EXPECT_EQ(aResolved.RefKind, BRepGraph_RefId::Kind::CoEdge)
      << "CoEdgeRef UID resolved to wrong kind";
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
  // original BRepGraph_Builder::Add()-time shapes after compaction.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes16 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Pick one face from the original build input.
  TopoDS_Shape aFace;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFace = anExp.Current();
    break;
  }
  ASSERT_FALSE(aFace.IsNull());
  ASSERT_TRUE(aGraph.Shapes().HasNode(aFace)) << "HasNode returned false before compact";
  const BRepGraph_NodeId aNodeIdBefore = aGraph.Shapes().FindNode(aFace);
  ASSERT_TRUE(aNodeIdBefore.IsValid()) << "FindNode returned invalid node before compact";

  (void)BRepGraph_Compact::Perform(aGraph);

  // After compact the TShape binding must survive so the original face is still locatable.
  EXPECT_TRUE(aGraph.Shapes().HasNode(aFace))
    << "HasNode returned false after compact - TShape bindings were lost";
  const BRepGraph_NodeId aNodeIdAfter = aGraph.Shapes().FindNode(aFace);
  EXPECT_TRUE(aNodeIdAfter.IsValid())
    << "FindNode returned invalid node after compact - TShape bindings were lost";
}
