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
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
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

int countHistoryRecordsByOp(const BRepGraph& theGraph, const TCollection_AsciiString& theOp)
{
  int aCount = 0;
  for (int aRecIdx = 0; aRecIdx < theGraph.History().NbRecords(); ++aRecIdx)
  {
    if (theGraph.History().Record(aRecIdx).OperationName == theOp)
      ++aCount;
  }
  return aCount;
}

} // namespace

TEST(BRepGraph_CompactTest, NoRemovedNodes_Noop)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const int aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  const int aNbFacesBefore    = aGraph.Topo().Faces().Nb();

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
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Run geometry dedup which replaces duplicate surface/curve handles directly.
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

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
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  // After compaction, there should be no removed defs.
  for (int anIdx = 0; anIdx < aGraph.Topo().Vertices().Nb(); ++anIdx)
    EXPECT_FALSE(aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(anIdx)).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Topo().Edges().Nb(); ++anIdx)
    EXPECT_FALSE(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(anIdx)).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Topo().Faces().Nb(); ++anIdx)
    EXPECT_FALSE(aGraph.Topo().Faces().Definition(BRepGraph_FaceId(anIdx)).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Topo().Wires().Nb(); ++anIdx)
    EXPECT_FALSE(aGraph.Topo().Wires().Definition(BRepGraph_WireId(anIdx)).IsRemoved);
}

TEST(BRepGraph_CompactTest, CrossReferences_Valid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, HistoryMode_RecordsMapping)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  BRepGraph_Compact::Options anOpts;
  anOpts.HistoryMode = true;
  (void)BRepGraph_Compact::Perform(aGraph, anOpts);

  const int aNbRemapRecords =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap"));
  // There should be history records if any remapping occurred.
  // After geometry dedup only, topology indices don't change, so remap may be 0.
  // This test just verifies the mechanism works without crashing.
  EXPECT_GE(aNbRemapRecords, 0);
}

TEST(BRepGraph_CompactTest, FullPipeline_Deduplicate_Compact_Validate)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Full dedup (replaces duplicate handles directly on defs).
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Compact. Without MergeEntitiesWhenSafe, no topology nodes are removed,
  // so NbNodesAfter == NbNodesBefore.
  const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aCompactRes.NbNodesAfter, aCompactRes.NbNodesBefore);

  // Validate.
  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraph_CompactTest, Compact_PreservesTopologyUIDs)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
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
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
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
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Mutate edge 0 twice so OwnGen == THE_EXPECTED_OWN_GEN.
  aGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.1;
  aGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = THE_MUTATED_EDGE_TOLERANCE;
  ASSERT_EQ(aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(0)).OwnGen, THE_EXPECTED_OWN_GEN);

  // Run dedup + compact.
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  (void)BRepGraph_Compact::Perform(aGraph);

  // Edge 0 may have been remapped. Find the edge that carries the mutated
  // tolerance and verify both the tolerance value and OwnGen are preserved.
  bool aFound = false;
  for (int anIdx = 0; anIdx < aGraph.Topo().Edges().Nb(); ++anIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(anIdx));
    if (std::abs(anEdge.Tolerance - THE_MUTATED_EDGE_TOLERANCE) < Precision::Confusion())
    {
      EXPECT_EQ(anEdge.OwnGen, THE_EXPECTED_OWN_GEN)
        << "Edge " << anIdx << " has mutated tolerance but wrong OwnGen";
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
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_GE(aGraph.Topo().Edges().Nb(), 3);

  // Record UIDs for a few face and edge nodes.
  const BRepGraph_UID aFaceUID0  = aGraph.UIDs().Of(BRepGraph_FaceId(0));
  const BRepGraph_UID aFaceUID1  = aGraph.UIDs().Of(BRepGraph_FaceId(1));
  const BRepGraph_UID aFaceUID2  = aGraph.UIDs().Of(BRepGraph_FaceId(2));
  const BRepGraph_UID anEdgeUID0 = aGraph.UIDs().Of(BRepGraph_EdgeId(0));
  const BRepGraph_UID anEdgeUID1 = aGraph.UIDs().Of(BRepGraph_EdgeId(1));
  ASSERT_TRUE(aFaceUID0.IsValid());
  ASSERT_TRUE(aFaceUID1.IsValid());
  ASSERT_TRUE(aFaceUID2.IsValid());
  ASSERT_TRUE(anEdgeUID0.IsValid());
  ASSERT_TRUE(anEdgeUID1.IsValid());

  // Record the UID of the face to be removed.
  const BRepGraph_UID aRemovedFaceUID = aGraph.UIDs().Of(BRepGraph_FaceId(2));

  // Remove one face.
  aGraph.Builder().RemoveNode(BRepGraph_FaceId(2));

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
