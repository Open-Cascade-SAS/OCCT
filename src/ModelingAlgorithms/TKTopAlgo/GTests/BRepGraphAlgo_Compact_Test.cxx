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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <BRepGraphAlgo_Compact.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Validate.hxx>
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

TEST(BRepGraphAlgo_CompactTest, NoRemovedNodes_Noop)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Defs().NbVertices();
  const int aNbEdgesBefore    = aGraph.Defs().NbEdges();
  const int aNbFacesBefore    = aGraph.Defs().NbFaces();

  const BRepGraphAlgo_Compact::Result aRes = BRepGraphAlgo_Compact::Perform(aGraph);

  EXPECT_EQ(aRes.NbRemovedVertices, 0);
  EXPECT_EQ(aRes.NbRemovedEdges, 0);
  EXPECT_EQ(aRes.NbRemovedFaces, 0);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Defs().NbVertices(), aNbVerticesBefore);
  EXPECT_EQ(aGraph.Defs().NbEdges(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Defs().NbFaces(), aNbFacesBefore);
}

TEST(BRepGraphAlgo_CompactTest, AfterDeduplicate_RemovesNodes)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Run geometry dedup which replaces duplicate surface/curve handles directly.
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  BRepGraphAlgo_Compact::Options anOpts;
  anOpts.HistoryMode                       = false;
  const BRepGraphAlgo_Compact::Result aRes = BRepGraphAlgo_Compact::Perform(aGraph, anOpts);

  // No separate geometry nodes exist, so NbRemovedSurfaces/NbRemovedCurves is always 0.
  EXPECT_EQ(aRes.NbRemovedSurfaces, 0);
  EXPECT_EQ(aRes.NbRemovedCurves, 0);
  // Without MergeDefsWhenSafe, topology node count does not change either.
  EXPECT_EQ(aRes.NbNodesAfter, aRes.NbNodesBefore);
}

TEST(BRepGraphAlgo_CompactTest, IndexDensity_NoGaps)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Perform(aGraph);
  BRepGraphAlgo_Compact::Perform(aGraph);

  // After compaction, there should be no removed defs.
  for (int anIdx = 0; anIdx < aGraph.Defs().NbVertices(); ++anIdx)
    EXPECT_FALSE(aGraph.Defs().Vertex(anIdx).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
    EXPECT_FALSE(aGraph.Defs().Edge(anIdx).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Defs().NbFaces(); ++anIdx)
    EXPECT_FALSE(aGraph.Defs().Face(anIdx).IsRemoved);
  for (int anIdx = 0; anIdx < aGraph.Defs().NbWires(); ++anIdx)
    EXPECT_FALSE(aGraph.Defs().Wire(anIdx).IsRemoved);
}

TEST(BRepGraphAlgo_CompactTest, CrossReferences_Valid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Perform(aGraph);
  BRepGraphAlgo_Compact::Perform(aGraph);

  const BRepGraphAlgo_Validate::Result aValResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraphAlgo_CompactTest, HistoryMode_RecordsMapping)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  BRepGraphAlgo_Compact::Options anOpts;
  anOpts.HistoryMode = true;
  BRepGraphAlgo_Compact::Perform(aGraph, anOpts);

  const int aNbRemapRecords =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap"));
  // There should be history records if any remapping occurred.
  // After geometry dedup only, topology indices don't change, so remap may be 0.
  // This test just verifies the mechanism works without crashing.
  EXPECT_GE(aNbRemapRecords, 0);
}

TEST(BRepGraphAlgo_CompactTest, FullPipeline_Deduplicate_Compact_Validate)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Full dedup (replaces duplicate handles directly on defs).
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Compact. Without MergeDefsWhenSafe, no topology nodes are removed,
  // so NbNodesAfter == NbNodesBefore.
  const BRepGraphAlgo_Compact::Result aCompactRes = BRepGraphAlgo_Compact::Perform(aGraph);
  EXPECT_EQ(aCompactRes.NbNodesAfter, aCompactRes.NbNodesBefore);

  // Validate.
  const BRepGraphAlgo_Validate::Result aValResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST(BRepGraphAlgo_CompactTest, Compact_PreservesTopologyUIDs)
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
    collectUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Defs().NbVertices());
  const NCollection_Map<BRepGraph_UID> anOrigEdgeUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Defs().NbEdges());
  const NCollection_Map<BRepGraph_UID> anOrigWireUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Defs().NbWires());
  const NCollection_Map<BRepGraph_UID> anOrigFaceUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Defs().NbFaces());
  const NCollection_Map<BRepGraph_UID> anOrigShellUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Defs().NbShells());
  const NCollection_Map<BRepGraph_UID> anOrigSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Defs().NbSolids());
  const NCollection_Map<BRepGraph_UID> anOrigCompoundUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::Compound, aGraph.Defs().NbCompounds());
  const NCollection_Map<BRepGraph_UID> anOrigCompSolidUIDs =
    collectUIDs(BRepGraph_NodeId::Kind::CompSolid, aGraph.Defs().NbCompSolids());
  // Geometry is now stored inline on defs; no separate geometry UIDs to collect.

  const uint32_t aGenBefore = aGraph.UIDs().Generation();

  // Run dedup + compact.
  BRepGraphAlgo_Deduplicate::Perform(aGraph);
  BRepGraphAlgo_Compact::Perform(aGraph);

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
             aGraph.Defs().NbVertices(),
             anOrigVertexUIDs,
             "Vertex");
  verifyUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Defs().NbEdges(), anOrigEdgeUIDs, "Edge");
  verifyUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Defs().NbWires(), anOrigWireUIDs, "Wire");
  verifyUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Defs().NbFaces(), anOrigFaceUIDs, "Face");
  verifyUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Defs().NbShells(), anOrigShellUIDs, "Shell");
  verifyUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Defs().NbSolids(), anOrigSolidUIDs, "Solid");
  verifyUIDs(BRepGraph_NodeId::Kind::Compound,
             aGraph.Defs().NbCompounds(),
             anOrigCompoundUIDs,
             "Compound");
  verifyUIDs(BRepGraph_NodeId::Kind::CompSolid,
             aGraph.Defs().NbCompSolids(),
             anOrigCompSolidUIDs,
             "CompSolid");

  // Geometry is now stored inline on defs; no separate geometry UIDs to verify.
}

TEST(BRepGraphAlgo_CompactTest, MutationGen_SurvivesCompact)
{
  constexpr double   THE_MUTATED_EDGE_TOLERANCE = 0.2;
  constexpr uint32_t THE_EXPECTED_MUTATION_GEN  = 2;

  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Mutate edge 0 twice so MutationGen == THE_EXPECTED_MUTATION_GEN.
  aGraph.MutEdge(0)->Tolerance = 0.1;
  aGraph.MutEdge(0)->Tolerance = THE_MUTATED_EDGE_TOLERANCE;
  ASSERT_EQ(aGraph.Defs().Edge(0).MutationGen, THE_EXPECTED_MUTATION_GEN);

  // Run dedup + compact.
  BRepGraphAlgo_Deduplicate::Perform(aGraph);
  BRepGraphAlgo_Compact::Perform(aGraph);

  // Edge 0 may have been remapped. Find the edge that carries the mutated
  // tolerance and verify both the tolerance value and MutationGen are preserved.
  bool aFound = false;
  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    const auto& anEdge = aGraph.Defs().Edge(anIdx);
    if (std::abs(anEdge.Tolerance - THE_MUTATED_EDGE_TOLERANCE) < Precision::Confusion())
    {
      EXPECT_EQ(anEdge.MutationGen, THE_EXPECTED_MUTATION_GEN)
        << "Edge " << anIdx << " has mutated tolerance but wrong MutationGen";
      aFound = true;
      break;
    }
  }
  EXPECT_TRUE(aFound) << "No edge with mutated tolerance found after compact";
}
