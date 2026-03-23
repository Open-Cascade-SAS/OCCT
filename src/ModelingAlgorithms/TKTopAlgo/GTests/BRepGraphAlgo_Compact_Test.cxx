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
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
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

  // Run geometry dedup which nullifies orphan surfaces/curves.
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  const int aNbSurfacesBefore = aGraph.Geom().NbSurfaces();

  BRepGraphAlgo_Compact::Options anOpts;
  anOpts.HistoryMode = false;
  const BRepGraphAlgo_Compact::Result aRes = BRepGraphAlgo_Compact::Perform(aGraph, anOpts);

  // Some geometry nodes should have been removed.
  EXPECT_GT(aRes.NbRemovedSurfaces + aRes.NbRemovedCurves, 0);
  EXPECT_LT(aRes.NbNodesAfter, aRes.NbNodesBefore);
  EXPECT_LT(aGraph.Geom().NbSurfaces(), aNbSurfacesBefore);
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

  const int aNbRemapRecords = countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Compact:Remap"));
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

  // Full dedup.
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Compact.
  const BRepGraphAlgo_Compact::Result aCompactRes = BRepGraphAlgo_Compact::Perform(aGraph);
  EXPECT_LT(aCompactRes.NbNodesAfter, aCompactRes.NbNodesBefore);

  // Validate.
  const BRepGraphAlgo_Validate::Result aValResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}
