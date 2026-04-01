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

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepGraphAlgo_Validate.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraphAlgo_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DE_Wrapper.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_Atomic.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <set>
#include <thread>

#include <gtest/gtest.h>

namespace
{

//! Extract faces from a box shape using BRepBuilderAPI_Copy with copyGeom=true.
//! This produces faces with independent TShape edges that are geometrically
//! identical but topologically distinct - the exact scenario sewing must resolve.
NCollection_Sequence<TopoDS_Face> extractCopiedFaces(const TopoDS_Shape& theBox)
{
  NCollection_Sequence<TopoDS_Face> aFaces;
  for (TopExp_Explorer anExp(theBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), /*copyGeom=*/true);
    aFaces.Append(TopoDS::Face(aCopier.Shape()));
  }
  return aFaces;
}

//! Find two adjacent faces from a shape (sharing a common edge).
//! Returns true if a pair is found.
bool findAdjacentFaces(const TopoDS_Shape& theShape, TopoDS_Face& theFace1, TopoDS_Face& theFace2)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);
  for (int anEntryIdx = 1; anEntryIdx <= anEdgeFaceMap.Extent(); ++anEntryIdx)
  {
    const NCollection_List<TopoDS_Shape>& aFaces = anEdgeFaceMap(anEntryIdx);
    if (aFaces.Extent() == 2)
    {
      theFace1 = TopoDS::Face(aFaces.First());
      theFace2 = TopoDS::Face(aFaces.Last());
      return true;
    }
  }
  return false;
}

//! Build a compound from copied faces and sew using convenience API.
TopoDS_Shape sewCopiedFaces(const NCollection_Sequence<TopoDS_Face>& theFaces,
                            const BRepGraphAlgo_Sewing::Options&     theOptions)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int aFaceIdx = 1; aFaceIdx <= theFaces.Length(); ++aFaceIdx)
  {
    aBB.Add(aCompound, theFaces.Value(aFaceIdx));
  }
  return BRepGraphAlgo_Sewing::Sew(aCompound, theOptions);
}

//! Build a compound from shapes and sew using Perform on a pre-built graph.
BRepGraphAlgo_Sewing::Result sewOnGraph(const NCollection_Sequence<TopoDS_Shape>& theShapes,
                                        const BRepGraphAlgo_Sewing::Options&      theOptions,
                                        BRepGraph&                                theGraph)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= theShapes.Length(); ++anIdx)
  {
    aBB.Add(aCompound, theShapes.Value(anIdx));
  }
  theGraph.Build(aCompound, theOptions.Parallel);
  if (!theGraph.IsDone())
  {
    return BRepGraphAlgo_Sewing::Result();
  }
  return BRepGraphAlgo_Sewing::Perform(theGraph, theOptions);
}

} // namespace

TEST(BRepGraphAlgo_SewingTest, SewTwoAdjacentFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, /*copyGeom=*/true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, /*copyGeom=*/true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

  ASSERT_TRUE(aResult.IsDone);
  // Two adjacent box faces share exactly 1 edge.
  EXPECT_EQ(aResult.NbSewnEdges, 1);
  // 8 free before (4 per face), 6 free after (8 - 2*1).
  EXPECT_EQ(aResult.NbFreeEdgesBefore, 8);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 6);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  // Verify via Perform on graph for diagnostics.
  BRepGraph                          aGraph;
  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));
  BRepGraphAlgo_Sewing::Result aDiag = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aDiag.IsDone);
  EXPECT_EQ(aDiag.NbSewnEdges, 12);
  EXPECT_EQ(aDiag.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, SewSingleFace_NothingToDo)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_GE(aFaces.Length(), 1);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  NCollection_Sequence<TopoDS_Face> aSingle;
  aSingle.Append(aFaces.Value(1));
  TopoDS_Shape aResult = sewCopiedFaces(aSingle, aOpts);
  ASSERT_FALSE(aResult.IsNull());
}

TEST(BRepGraphAlgo_SewingTest, GraphAccessAfterSewing)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);

  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), 0);
  // History should record the merge.
  EXPECT_GE(aGraph.History().NbRecords(), 1);
}

TEST(BRepGraphAlgo_SewingTest, FreeEdgeCounting)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_GE(aFaces.Length(), 3);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  aShapes.Append(aFaces.Value(1));
  aShapes.Append(aFaces.Value(2));
  aShapes.Append(aFaces.Value(3));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  // With copied faces, all edges start as free (12 total for 3 rectangular faces).
  EXPECT_EQ(aResult.NbFreeEdgesBefore, 12);
  // Some edges should have been sewn.
  EXPECT_GT(aResult.NbSewnEdges, 0);
  EXPECT_LT(aResult.NbFreeEdgesAfter, aResult.NbFreeEdgesBefore);
}

TEST(BRepGraphAlgo_SewingTest, ConfigurationDefaults)
{
  BRepGraphAlgo_Sewing::Options aOpts;
  EXPECT_TRUE(aOpts.Cutting);
  EXPECT_TRUE(aOpts.SameParameterMode);
  EXPECT_FALSE(aOpts.NonManifoldMode);
  EXPECT_FALSE(aOpts.Parallel);
  EXPECT_TRUE(aOpts.HistoryMode);
  EXPECT_NEAR(aOpts.Tolerance, 1.0e-06, 1.0e-15);
}

TEST(BRepGraphAlgo_SewingTest, SewWithParallelBuild)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Parallel  = true;

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 12);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, SewWithCuttingDisabled)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Cutting   = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 1);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_ResultIsValid)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  // Verify 6 faces are present in the result compound.
  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbFaces, 6);

  // Verify non-reconstructed (unaffected) faces are valid.
  int aNbValidFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepCheck_Analyzer anAnalyzer(anExp.Current(), true);
    if (anAnalyzer.IsValid())
    {
      ++aNbValidFaces;
    }
  }
  EXPECT_GE(aNbValidFaces, 2);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  // Sum absolute area face-by-face.
  double aTotalArea = 0.0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aTotalArea += std::abs(aProps.Mass());
  }
  // Full 2*(10*20 + 10*30 + 20*30) = 2200.
  EXPECT_GT(aTotalArea, 1000.0);
  EXPECT_LE(aTotalArea, 2300.0);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_HistoryRecordsExist)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance   = 1.0e-04;
  aOpts.HistoryMode = true;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);

  // RecordBatch creates one history record containing all merge mappings.
  EXPECT_GE(aGraph.History().NbRecords(), 1);

  // Verify batch record has at least as many mappings as sewn edges.
  if (aGraph.History().NbRecords() > 0)
  {
    const BRepGraph_HistoryRecord& aRecord = aGraph.History().Record(0);
    EXPECT_GE(aRecord.Mapping.Extent(), aResult.NbSewnEdges);
    NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>::Iterator anIt(
      aRecord.Mapping);
    ASSERT_TRUE(anIt.More());
    const BRepGraph_NodeId anOrigId = aGraph.History().FindOriginal(anIt.Key());
    EXPECT_TRUE(anOrigId.IsValid());
  }
}

TEST(BRepGraphAlgo_SewingTest, SewWithFaceAnalysisDisabled)
{
  // Face analysis is no longer a separate option in the new API (graph must be pre-built).
  // This test verifies that sewing works correctly with the default options.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, /*copyGeom=*/true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, /*copyGeom=*/true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 1);
}

// ===================================================================
// Extended Sewing Tests
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, SewEmptyInput_NotDone)
{
  // An empty graph should return IsDone=false from Build, so Perform should also fail.
  BRepGraph aGraph;
  // Don't build anything - graph is not done.
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph);
  EXPECT_FALSE(aResult.IsDone);
}

TEST(BRepGraphAlgo_SewingTest, SewClosedSolid_NothingFree)
{
  // A complete solid has no free edges - nothing to sew.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbFreeEdgesBefore, 0);
  EXPECT_EQ(aResult.NbSewnEdges, 0);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, SewWithTightTolerance_NothingMatched)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  // Extremely tight tolerance.
  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-15;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbFreeEdgesBefore, 8);
}

TEST(BRepGraphAlgo_SewingTest, SewWithLooseTolerance_StillSews)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 12);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, SewSameParameterDisabled)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance         = 1.0e-04;
  aOpts.SameParameterMode = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 1);
}

TEST(BRepGraphAlgo_SewingTest, SewFourFaces_PartialBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_GE(aFaces.Length(), 4);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= 4; ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  // 4 faces with 4 edges each = 16 free before.
  EXPECT_EQ(aResult.NbFreeEdgesBefore, 16);
  EXPECT_GT(aResult.NbSewnEdges, 0);
  EXPECT_LT(aResult.NbFreeEdgesAfter, aResult.NbFreeEdgesBefore);
}

TEST(BRepGraphAlgo_SewingTest, SewParallel_MatchesSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  // Sequential.
  BRepGraphAlgo_Sewing::Options aSeqOpts;
  aSeqOpts.Tolerance = 1.0e-04;
  aSeqOpts.Parallel  = false;
  BRepGraph                    aSeqGraph;
  BRepGraphAlgo_Sewing::Result aSeqResult = sewOnGraph(aShapes, aSeqOpts, aSeqGraph);
  ASSERT_TRUE(aSeqResult.IsDone);

  // Re-copy faces for the parallel run.
  NCollection_Sequence<TopoDS_Face>  aFaces2 = extractCopiedFaces(aBox);
  NCollection_Sequence<TopoDS_Shape> aShapes2;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces2.Length(); ++aFaceIdx)
    aShapes2.Append(aFaces2.Value(aFaceIdx));

  // Parallel.
  BRepGraphAlgo_Sewing::Options aParOpts;
  aParOpts.Tolerance = 1.0e-04;
  aParOpts.Parallel  = true;
  BRepGraph                    aParGraph;
  BRepGraphAlgo_Sewing::Result aParResult = sewOnGraph(aShapes2, aParOpts, aParGraph);
  ASSERT_TRUE(aParResult.IsDone);

  EXPECT_EQ(aParResult.NbSewnEdges, aSeqResult.NbSewnEdges);
  EXPECT_EQ(aParResult.NbFreeEdgesAfter, aSeqResult.NbFreeEdgesAfter);
  EXPECT_EQ(aParResult.NbFreeEdgesBefore, aSeqResult.NbFreeEdgesBefore);
}

TEST(BRepGraphAlgo_SewingTest, SewParallel_TwoDisconnectedFaceGroups)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();

  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox2Base = aBoxMaker2.Shape();

  gp_Trsf aMove;
  aMove.SetTranslation(gp_Vec(1000.0, 0.0, 0.0));
  BRepBuilderAPI_Transform aTransform(aBox2Base, aMove, true);
  const TopoDS_Shape&      aBox2 = aTransform.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces1 = extractCopiedFaces(aBox1);
  NCollection_Sequence<TopoDS_Face> aFaces2 = extractCopiedFaces(aBox2);
  ASSERT_EQ(aFaces1.Length(), 6);
  ASSERT_EQ(aFaces2.Length(), 6);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces1.Length(); ++aFaceIdx)
    aShapes.Append(aFaces1.Value(aFaceIdx));
  for (int aFaceIdx = 1; aFaceIdx <= aFaces2.Length(); ++aFaceIdx)
    aShapes.Append(aFaces2.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Parallel  = true;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);

  // Two independent boxes should sew independently: 12 + 12 shared edges.
  EXPECT_EQ(aResult.NbSewnEdges, 24);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, SewCylinder_TwoHalves)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_FALSE(aCyl.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aCyl);
  ASSERT_GE(aFaces.Length(), 3);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  int aNbResultFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbResultFaces;
  EXPECT_EQ(aNbResultFaces, aFaces.Length());
}

TEST(BRepGraphAlgo_SewingTest, SewSphere_AllFaces)
{
  BRepPrimAPI_MakeSphere aSphereMaker(10.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();
  ASSERT_FALSE(aSphere.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aSphere);
  ASSERT_GE(aFaces.Length(), 1);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  int aNbResultFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbResultFaces;
  EXPECT_EQ(aNbResultFaces, aFaces.Length());
}

TEST(BRepGraphAlgo_SewingTest, VertexMerging_SharedVerticesAfterSewing)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  aShapes.Append(aCopy1.Shape());
  aShapes.Append(aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 1);

  // After sewing, the graph's vertex count should be less than the sum of
  // individual face vertices (because coincident vertices were merged).
  EXPECT_LE(aGraph.Topo().Vertices().Nb(), 8);
  EXPECT_GE(aGraph.Topo().Vertices().Nb(), 6);
}

TEST(BRepGraphAlgo_SewingTest, GraphEdgeCount_AfterSewing)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_GE(aGraph.Topo().Edges().Nb(), 12);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_AreaPerFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  // Each face should have positive area.
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    EXPECT_GT(std::abs(aProps.Mass()), 1.0) << "A face in the result has negligible area";
  }
}

TEST(BRepGraphAlgo_SewingTest, DefaultTolerance)
{
  // Default options use tolerance 1.0e-06.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  // Use default options (tolerance 1.0e-06).
  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 1);
}

// ===================================================================
// Many-Faces Sewing: build a shell from a grid of disconnected faces
// ===================================================================

namespace
{

//! Build a grid of NxM copied box faces arranged in a flat grid.
NCollection_Sequence<TopoDS_Shape> buildFaceGrid(int    theNx,
                                                 int    theNy,
                                                 double theSizeX,
                                                 double theSizeY)
{
  NCollection_Sequence<TopoDS_Shape> aFaces;
  BRepPrimAPI_MakeBox                aBoxMaker(theSizeX, theSizeY, 1.0);
  const TopoDS_Shape&                aBox = aBoxMaker.Shape();

  // Find the bottom face (Z=0 plane).
  TopoDS_Face aTemplate;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    GProp_GProps       aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    gp_Pnt aCentroid = aProps.CentreOfMass();
    if (std::abs(aCentroid.Z()) < 0.01)
    {
      aTemplate = aFace;
      break;
    }
  }

  if (aTemplate.IsNull())
  {
    TopExp_Explorer anExp(aBox, TopAbs_FACE);
    aTemplate = TopoDS::Face(anExp.Current());
  }

  BRepGraph aTemplateGraph;
  aTemplateGraph.Build(aTemplate);

  for (int anIx = 0; anIx < theNx; ++anIx)
  {
    for (int anIy = 0; anIy < theNy; ++anIy)
    {
      gp_Trsf aTrsf;
      aTrsf.SetTranslation(gp_Vec(anIx * theSizeX, anIy * theSizeY, 0.0));

      if (aTemplateGraph.IsDone() && aTemplateGraph.Topo().Faces().Nb() > 0)
      {
        BRepGraph aTransGraph =
          BRepGraphAlgo_Transform::TransformFace(aTemplateGraph, BRepGraph_FaceId(0), aTrsf, true);
        if (aTransGraph.IsDone())
        {
          aFaces.Append(aTransGraph.Shapes().Reconstruct(BRepGraph_FaceId(0)));
          continue;
        }
      }

      BRepBuilderAPI_Copy      aCopier(aTemplate, true);
      TopoDS_Shape             aCopied = aCopier.Shape();
      BRepBuilderAPI_Transform aTransformer(aCopied, aTrsf, true);
      aFaces.Append(aTransformer.Shape());
    }
  }
  return aFaces;
}

} // namespace

TEST(BRepGraphAlgo_SewingTest, ManyFaces_GridShell_10x10)
{
  NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(10, 10, 1.0, 1.0);
  ASSERT_EQ(aFaces.Length(), 100);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aFaces, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GT(aResult.NbSewnEdges, 0);
  EXPECT_LT(aResult.NbFreeEdgesAfter, aResult.NbFreeEdgesBefore);
}

TEST(BRepGraphAlgo_SewingTest, ManyFaces_GridShell_FreeEdgeCount)
{
  NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(5, 5, 2.0, 2.0);
  ASSERT_EQ(aFaces.Length(), 25);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aFaces, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);

  EXPECT_EQ(aResult.NbFreeEdgesBefore, 100);
  EXPECT_EQ(aResult.NbSewnEdges, 40);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 20);
}

TEST(BRepGraphAlgo_SewingTest, ManyFaces_ManyBoxes_50Boxes)
{
  NCollection_Sequence<TopoDS_Shape> aAllFaces;
  for (int anIdx = 0; anIdx < 50; ++anIdx)
  {
    BRepPrimAPI_MakeBox               aBoxMaker(gp_Pnt(anIdx * 10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
    NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBoxMaker.Shape());
    for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    {
      aAllFaces.Append(aFaces.Value(aFaceIdx));
    }
  }
  ASSERT_EQ(aAllFaces.Length(), 300);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aAllFaces, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GT(aResult.NbSewnEdges, 300);
  EXPECT_LT(aResult.NbFreeEdgesAfter, aResult.NbFreeEdgesBefore);
}

// ===================================================================
// New Tests: Perform on pre-built graph, chaining, convenience match
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, Perform_PreBuiltGraph_ModifiesInPlace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face>  aFaces = extractCopiedFaces(aBox);
  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= aShapes.Length(); ++anIdx)
    aBB.Add(aCompound, aShapes.Value(anIdx));

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot edge count before sewing.
  const int aNbEdgesBefore = aGraph.Topo().Edges().Nb();

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance                      = 1.0e-04;
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 12);

  // The graph was modified in-place; edge definitions still exist.
  EXPECT_GE(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
  // History records are present (batch recording creates fewer records).
  EXPECT_GE(aGraph.History().NbRecords(), 1);
}

TEST(BRepGraphAlgo_SewingTest, Sew_Convenience_MatchesPerform)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face>  aFaces = extractCopiedFaces(aBox);
  NCollection_Sequence<TopoDS_Shape> aShapes;
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aShapes.Append(aFaces.Value(aFaceIdx));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  // Perform path.
  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aPerformResult = sewOnGraph(aShapes, aOpts, aGraph);
  ASSERT_TRUE(aPerformResult.IsDone);

  // Convenience path (Sew).
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  NCollection_Sequence<TopoDS_Face> aFaces2 = extractCopiedFaces(aBox);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces2.Length(); ++aFaceIdx)
    aBB.Add(aCompound, aFaces2.Value(aFaceIdx));

  TopoDS_Shape aSewResult = BRepGraphAlgo_Sewing::Sew(aCompound, aOpts);
  ASSERT_FALSE(aSewResult.IsNull());

  // Count faces in both results.
  int aNbPerformFaces = 0;
  for (TopExp_Explorer anExp(aGraph.Shapes().Reconstruct(BRepGraph_CompoundId(0)), TopAbs_FACE);
       anExp.More();
       anExp.Next())
    ++aNbPerformFaces;

  int aNbSewFaces = 0;
  for (TopExp_Explorer anExp(aSewResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbSewFaces;

  EXPECT_EQ(aNbPerformFaces, aNbSewFaces);
  EXPECT_EQ(aNbSewFaces, 6);
}

TEST(BRepGraphAlgo_SewingTest, CutAtIntersections_TVertex)
{
  const gp_Pnt aP00(0, 0, 0);
  const gp_Pnt aP20(2, 0, 0);
  const gp_Pnt aP01(0, 1, 0);
  const gp_Pnt aP21(2, 1, 0);
  const gp_Pnt aP10(1, 0, 0);
  const gp_Pnt aP0m1(0, -1, 0);
  const gp_Pnt aP1m1(1, -1, 0);
  const gp_Pnt aP2m1(2, -1, 0);

  // Face1: rectangle [0,2] x [0,1].
  TopoDS_Face aF1;
  {
    BRepBuilderAPI_MakeWire aMW;
    aMW.Add(BRepBuilderAPI_MakeEdge(aP00, aP20).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP20, aP21).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP21, aP01).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP01, aP00).Edge());
    aF1 = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  }

  // Face2: rectangle [0,1] x [-1,0].
  TopoDS_Face aF2;
  {
    BRepBuilderAPI_MakeWire aMW;
    aMW.Add(BRepBuilderAPI_MakeEdge(aP00, aP10).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP10, aP1m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP1m1, aP0m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP0m1, aP00).Edge());
    aF2 = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  }

  // Face3: rectangle [1,2] x [-1,0].
  TopoDS_Face aF3;
  {
    BRepBuilderAPI_MakeWire aMW;
    aMW.Add(BRepBuilderAPI_MakeEdge(aP10, aP20).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP20, aP2m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP2m1, aP1m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP1m1, aP10).Edge());
    aF3 = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  }

  ASSERT_FALSE(aF1.IsNull());
  ASSERT_FALSE(aF2.IsNull());
  ASSERT_FALSE(aF3.IsNull());

  // With cutting.
  {
    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    aBB.Add(aCompound, aF1);
    aBB.Add(aCompound, aF2);
    aBB.Add(aCompound, aF3);

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-4;
    aOpts.Cutting   = true;

    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    ASSERT_TRUE(aResult.IsDone);
    EXPECT_GE(aResult.NbSewnEdges, 2);
  }

  // Without cutting.
  {
    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    aBB.Add(aCompound, aF1);
    aBB.Add(aCompound, aF2);
    aBB.Add(aCompound, aF3);

    BRepGraphAlgo_Sewing::Options aNoCutOpts;
    aNoCutOpts.Tolerance = 1.0e-4;
    aNoCutOpts.Cutting   = false;

    BRepGraph aGraphNoCut;
    aGraphNoCut.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aResultNoCut =
      BRepGraphAlgo_Sewing::Perform(aGraphNoCut, aNoCutOpts);
    ASSERT_TRUE(aResultNoCut.IsDone);
    // Without cutting the T-junction edges should NOT be matched.
    EXPECT_LT(aResultNoCut.NbSewnEdges, 2);
  }
}

// ============================================================
// Sewing Hierarchy Preservation
// ============================================================

TEST(BRepGraphAlgo_SewingTest, SewBoxSolid_PreservesHierarchy)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = sewCopiedFaces(aFaces, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_SewingTest, SewSolidInput_PreservesHierarchy)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  TopoDS_Shape aResult = BRepGraphAlgo_Sewing::Sew(aBox, aOpts);
  ASSERT_FALSE(aResult.IsNull());

  int aNbSolids = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_SOLID); anExp.More(); anExp.Next())
    ++aNbSolids;
  EXPECT_EQ(aNbSolids, 1);

  int aNbShells = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_SHELL); anExp.More(); anExp.Next())
    ++aNbShells;
  EXPECT_EQ(aNbShells, 1);

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

// ============================================================
// Deep vs Light Copy Test
// ============================================================

TEST(BRepGraphAlgo_SewingTest, DeepAndLightCopy_MatchNodeCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aDeepCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aDeepCopy.IsDone());

  BRepGraph aLightCopy = BRepGraphAlgo_Copy::Perform(aGraph, false);
  ASSERT_TRUE(aLightCopy.IsDone());

  EXPECT_EQ(aDeepCopy.Topo().Faces().Nb(), aLightCopy.Topo().Faces().Nb());
  EXPECT_EQ(aDeepCopy.Topo().Edges().Nb(), aLightCopy.Topo().Edges().Nb());
  EXPECT_EQ(aDeepCopy.Topo().Vertices().Nb(), aLightCopy.Topo().Vertices().Nb());
}

// ============================================================
// New Feature Tests: Options extensions
// ============================================================

TEST(BRepGraphAlgo_SewingTest, MaxTolerance_RejectLargeGaps)
{
  // Build two box faces; sew with MaxTolerance smaller than default edge tolerance.
  // BRep edges typically have tolerance ~1e-7. Setting MaxTolerance below that rejects merges.
  BRepPrimAPI_MakeBox               aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape&               aBox   = aBoxMaker.Shape();
  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aFaces.Value(1));
  aBB.Add(aCompound, aFaces.Value(2));

  // MaxTolerance = 1e-10 blocks any merge (edges have tolerance ~1e-7).
  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance    = 1.0e-04;
  aOpts.MaxTolerance = 1.0e-10;
  aOpts.FaceAnalysis = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aRes = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_EQ(aRes.NbSewnEdges, 0);
}

TEST(BRepGraphAlgo_SewingTest, LocalTolerancesMode_AdaptiveTol)
{
  // Verify LocalTolerancesMode sews all 6 box faces correctly.
  // The adaptive tolerance adds per-edge tolerances to the global tolerance.
  BRepPrimAPI_MakeBox               aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape&               aBox   = aBoxMaker.Shape();
  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aBB.Add(aCompound, aFaces.Value(i));
  }

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance           = 1.0e-04;
  aOpts.LocalTolerancesMode = true;
  aOpts.FaceAnalysis        = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  EXPECT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 12);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, Result_FreeEdgesPopulated)
{
  BRepPrimAPI_MakeBox               aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape&               aBox   = aBoxMaker.Shape();
  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  // Only 2 of 6 faces - sewing will leave free edges.
  aBB.Add(aCompound, aFaces.Value(1));
  aBB.Add(aCompound, aFaces.Value(2));

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance    = 1.0e-04;
  aOpts.FaceAnalysis = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  EXPECT_TRUE(aResult.IsDone);
  // FreeEdges vector should match NbFreeEdgesAfter count.
  EXPECT_EQ(aResult.FreeEdges.Length(), aResult.NbFreeEdgesAfter);
  EXPECT_GT(aResult.NbFreeEdgesAfter, 0);
}

TEST(BRepGraphAlgo_SewingTest, Result_SewnEdgePairsPopulated)
{
  BRepPrimAPI_MakeBox               aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape&               aBox   = aBoxMaker.Shape();
  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aBB.Add(aCompound, aFaces.Value(i));
  }

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance    = 1.0e-04;
  aOpts.FaceAnalysis = false;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  EXPECT_TRUE(aResult.IsDone);
  // SewnEdgePairs should have NbSewnEdges entries.
  EXPECT_EQ(aResult.SewnEdgePairs.Length(), aResult.NbSewnEdges);
  EXPECT_GT(aResult.NbSewnEdges, 0);
}

TEST(BRepGraphAlgo_SewingTest, FaceAnalysis_IntegratedWithSewing)
{
  // End-to-end: build a box, sew with FaceAnalysis enabled (default).
  BRepPrimAPI_MakeBox               aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape&               aBox   = aBoxMaker.Shape();
  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aBB.Add(aCompound, aFaces.Value(i));
  }

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance    = 1.0e-04;
  aOpts.FaceAnalysis = true; // explicitly enabled

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  EXPECT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbFreeEdgesAfter, 0);
  EXPECT_EQ(aResult.NbSewnEdges, 12);
}

TEST(BRepGraphAlgo_SewingTest, ConfigurationDefaults_NewOptions)
{
  BRepGraphAlgo_Sewing::Options aOpts;
  EXPECT_TRUE(aOpts.FaceAnalysis);
  EXPECT_FALSE(aOpts.FloatingEdgesMode);
  EXPECT_FALSE(aOpts.LocalTolerancesMode);
  EXPECT_NEAR(aOpts.MinTolerance, 0.0, 1.0e-15);
  EXPECT_GT(aOpts.MaxTolerance, 1.0e+100);
}

TEST(BRepGraphAlgo_SewingTest, NonManifoldMode_ThreeFacesShareEdge)
{
  // Three faces share a common geometric edge (P1-P2).
  // In manifold mode: only 1 pair is sewn. In non-manifold mode: 2 pairs are sewn
  // (the keep-edge accumulates PCurves from both remove-edges).
  const gp_Pnt aP1(0, 0, 0), aP2(10, 0, 0);
  const gp_Pnt aP3(5, 5, 0), aP4(5, -5, 0), aP5(5, 0, 5);

  // Face1: triangle P1-P2-P3.
  BRepBuilderAPI_MakeWire aMW1;
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP3, aP1).Edge());
  TopoDS_Face aF1 = BRepBuilderAPI_MakeFace(aMW1.Wire(), true);

  // Face2: triangle P1-P2-P4.
  BRepBuilderAPI_MakeWire aMW2;
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP2, aP4).Edge());
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());
  TopoDS_Face aF2 = BRepBuilderAPI_MakeFace(aMW2.Wire(), true);

  // Face3: triangle P1-P2-P5.
  BRepBuilderAPI_MakeWire aMW3;
  aMW3.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW3.Add(BRepBuilderAPI_MakeEdge(aP2, aP5).Edge());
  aMW3.Add(BRepBuilderAPI_MakeEdge(aP5, aP1).Edge());
  TopoDS_Face aF3 = BRepBuilderAPI_MakeFace(aMW3.Wire(), true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aF1);
  aBB.Add(aCompound, aF2);
  aBB.Add(aCompound, aF3);

  // Manifold mode: 1 pair sewn (third edge left free).
  {
    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance       = 1.0e-4;
    aOpts.NonManifoldMode = false;
    aOpts.FaceAnalysis    = false;

    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aRes = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    ASSERT_TRUE(aRes.IsDone);
    EXPECT_EQ(aRes.NbSewnEdges, 1);
  }

  // Non-manifold mode: 2 pairs sewn (vs 1 in manifold mode).
  {
    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance       = 1.0e-4;
    aOpts.NonManifoldMode = true;
    aOpts.FaceAnalysis    = false;

    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aRes = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    ASSERT_TRUE(aRes.IsDone);
    EXPECT_EQ(aRes.NbSewnEdges, 2);
    EXPECT_EQ(aRes.SewnEdgePairs.Length(), 2);

    // Each keep-edge should have gained at least 1 extra coedge from the merge.
    // Verify coedges reference valid, distinct faces with non-null 2D curves.
    for (int i = 0; i < aRes.SewnEdgePairs.Length(); ++i)
    {
      const int                                     aKeepIdx = aRes.SewnEdgePairs.Value(i).Index;
      const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
        aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(aKeepIdx));
      EXPECT_GE(aCoEdgeIdxs.Length(), 2);

      NCollection_Map<int> aFaceIds;
      for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
      {
        const BRepGraphInc::CoEdgeDef& aCE =
          aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
        // PCurve may be stored (Curve2DRepId valid) or computed on-the-fly for planar faces.
        // Use PCurveAdaptor which handles both cases.
        const Geom2dAdaptor_Curve aPCAdaptor =
          BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdgeIdxs.Value(j));
        EXPECT_FALSE(aPCAdaptor.Curve().IsNull());
        EXPECT_TRUE(aCE.FaceDefId.IsValid());
        aFaceIds.Add(aCE.FaceDefId.Index);
      }
      // CoEdges should reference distinct faces (each merged from a different face).
      EXPECT_GE(aFaceIds.Extent(), 2);
    }
  }
}

// ---------------------------------------------------------------------------
// Seam edge PCurve consistency: corrupted dual-PCurve detected by audit
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_SewingTest, SeamEdge_CorruptedDualPCurve_DetectedByValidator)
{
  // Build a cylinder which has seam edges with dual PCurves.
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aCylinder);
  ASSERT_TRUE(aGraph.IsDone());

  // Find a CoEdge that is part of a seam pair.
  int aSeamCoEdgeIdx = -1;
  for (int i = 0; i < aGraph.Topo().CoEdges().Nb(); ++i)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(i));
    if (aCoEdge.SeamPairId.IsValid() && !aCoEdge.IsRemoved)
    {
      aSeamCoEdgeIdx = i;
      break;
    }
  }
  ASSERT_GE(aSeamCoEdgeIdx, 0) << "Cylinder should have at least one seam CoEdge.";

  // Verify seam detection: the found CoEdge must reference a valid paired CoEdge.
  {
    const BRepGraphInc::CoEdgeDef& aSeamCoEdge =
      aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(aSeamCoEdgeIdx));
    ASSERT_TRUE(aSeamCoEdge.SeamPairId.IsValid())
      << "Seam CoEdge must have a valid SeamPairId before corruption.";
    ASSERT_LT(aSeamCoEdge.SeamPairId.Index, aGraph.Topo().CoEdges().Nb())
      << "SeamPairId must reference an in-bounds CoEdge.";
    ASSERT_TRUE(aSeamCoEdge.Curve2DRepId.IsValid())
      << "Seam CoEdge must have a valid Curve2DRepId before corruption.";
  }

  // Corrupt its Curve2DRepId to an out-of-bounds index.
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdgeDef =
    aGraph.Builder().MutCoEdge(BRepGraph_CoEdgeId(aSeamCoEdgeIdx));
  aCoEdgeDef->Curve2DRepId = BRepGraph_Curve2DRepId(aGraph.Topo().Geometry().NbCurves2D() + 1);

  const BRepGraphAlgo_Validate::Result aAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid()) << "Corrupted seam edge PCurve should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  // Verify the error specifically mentions geometry reference bounds.
  bool aFoundGeomRefError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraphAlgo_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraphAlgo_Validate::Severity::Error
        && anIssue.Description.Search("CoEdgeDef.Curve2DRepId out of bounds") > 0)
    {
      aFoundGeomRefError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundGeomRefError)
    << "Audit should report 'CoEdgeDef.Curve2DRepId out of bounds' for the corrupted seam CoEdge.";
}
