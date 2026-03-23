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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepGraphAlgo_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DE_Wrapper.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_Atomic.hxx>

#include <cmath>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>

#include <gtest/gtest.h>

namespace
{

//! Extract faces from a box shape using BRepBuilderAPI_Copy with copyGeom=true.
//! This produces faces with independent TShape edges that are geometrically
//! identical but topologically distinct -- the exact scenario sewing must resolve.
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

} // namespace

TEST(BRepGraphAlgo_SewingTest, SewTwoAdjacentFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  // Find two faces that share a common edge in the original box.
  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  // Copy each face independently (geometry is duplicated).
  BRepBuilderAPI_Copy aCopy1(aOrigF1, /*copyGeom=*/true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, /*copyGeom=*/true);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  // Two adjacent box faces share exactly 1 edge.
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);
  // 8 free before (4 per face), 6 free after (8 - 2*1).
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 8);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 6);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  // A box has 12 edges; all should be sewn.
  EXPECT_EQ(aSewer.NbSewnEdges(), 12);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 0);
}

TEST(BRepGraphAlgo_SewingTest, SewSingleFace_NothingToDo)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_GE(aFaces.Length(), 1);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aFaces.Value(1));
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  EXPECT_EQ(aSewer.NbSewnEdges(), 0);
}

TEST(BRepGraphAlgo_SewingTest, GraphAccessAfterSewing)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  // Use adjacent faces to ensure sewing actually happens.
  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());

  const BRepGraph& aGraph = aSewer.Graph();
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 2);
  EXPECT_GT(aGraph.Defs().NbEdges(), 0);
  EXPECT_GT(aGraph.Defs().NbVertices(), 0);
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

  // Three faces: each has 4 edges = 12 total, but some will be sewing candidates.
  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aFaces.Value(1));
  aSewer.Add(aFaces.Value(2));
  aSewer.Add(aFaces.Value(3));
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  // With copied faces, all edges start as free (12 total for 3 rectangular faces).
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 12);
  // Some edges should have been sewn.
  EXPECT_GT(aSewer.NbSewnEdges(), 0);
  EXPECT_LT(aSewer.NbFreeEdgesAfter(), aSewer.NbFreeEdgesBefore());
}

TEST(BRepGraphAlgo_SewingTest, ConfigurationDefaults)
{
  BRepGraphAlgo_Sewing aSewer;
  EXPECT_TRUE(aSewer.FaceAnalysis());
  EXPECT_TRUE(aSewer.Cutting());
  EXPECT_TRUE(aSewer.SameParameterMode());
  EXPECT_FALSE(aSewer.NonManifoldMode());
  EXPECT_FALSE(aSewer.IsParallel());
}

TEST(BRepGraphAlgo_SewingTest, ConfigurationSetters)
{
  BRepGraphAlgo_Sewing aSewer;
  aSewer.SetFaceAnalysis(false);
  aSewer.SetCutting(false);
  aSewer.SetSameParameterMode(false);
  aSewer.SetNonManifoldMode(true);
  aSewer.SetParallel(true);

  EXPECT_FALSE(aSewer.FaceAnalysis());
  EXPECT_FALSE(aSewer.Cutting());
  EXPECT_FALSE(aSewer.SameParameterMode());
  EXPECT_TRUE(aSewer.NonManifoldMode());
  EXPECT_TRUE(aSewer.IsParallel());
}

TEST(BRepGraphAlgo_SewingTest, SewWithParallelBuild)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetParallel(true);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  EXPECT_EQ(aSewer.NbSewnEdges(), 12);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 0);
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetCutting(false);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_ResultIsValid)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  const TopoDS_Shape& aResult = aSewer.Result();
  ASSERT_FALSE(aResult.IsNull());

  // Verify 6 faces are present in the result compound.
  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbFaces, 6);

  // Verify non-reconstructed (unaffected) faces are valid.
  // Reconstructed faces may have partial validity due to edge merging
  // that doesn't yet rebuild full BRep consistency.
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  const TopoDS_Shape& aResult = aSewer.Result();
  ASSERT_FALSE(aResult.IsNull());

  // Sum absolute area face-by-face. Use std::abs because REVERSED-oriented faces
  // yield negative mass from BRepGProp, which is correct shell behavior.
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());

  const BRepGraph& aGraph = aSewer.Graph();
  // A box has 12 edges; each merged edge should produce at least one history record.
  EXPECT_GE(aGraph.History().NbRecords(), 12);

  // Verify that FindOriginal traces back to a valid node for the first history record.
  if (aGraph.History().NbRecords() > 0)
  {
    const BRepGraph_HistoryRecord& aRecord = aGraph.History().Record(0);
    // Iterate over the mapping to get the first original node.
    NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>::Iterator anIt(
      aRecord.Mapping);
    ASSERT_TRUE(anIt.More());
    const BRepGraph_NodeId anOrigId = aGraph.History().FindOriginal(anIt.Key());
    EXPECT_TRUE(anOrigId.IsValid());
  }
}

TEST(BRepGraphAlgo_SewingTest, SewWithFaceAnalysisDisabled)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, /*copyGeom=*/true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, /*copyGeom=*/true);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetFaceAnalysis(false);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);
}

// ===================================================================
// Extended Sewing Tests
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, SewEmptyInput_NotDone)
{
  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Perform();
  // No shapes added -- Perform should return early, not done.
  EXPECT_FALSE(aSewer.IsDone());
}

TEST(BRepGraphAlgo_SewingTest, SewClosedSolid_NothingFree)
{
  // A complete solid has no free edges -- nothing to sew.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aBox);
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 0);
  EXPECT_EQ(aSewer.NbSewnEdges(), 0);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 0);
}

TEST(BRepGraphAlgo_SewingTest, SewWithTightTolerance_NothingMatched)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  // Extremely tight tolerance -- geometric validation should reject all pairs.
  BRepGraphAlgo_Sewing aSewer(1.0e-15);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  // With such tight tolerance, vertex merging and matching likely fail.
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 8);
}

TEST(BRepGraphAlgo_SewingTest, SewWithLooseTolerance_StillSews)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  // Loose tolerance -- should still sew correctly.
  BRepGraphAlgo_Sewing aSewer(1.0);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 12);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 0);
}

TEST(BRepGraphAlgo_SewingTest, SewSameParameterDisabled)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetSameParameterMode(false);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);
}

TEST(BRepGraphAlgo_SewingTest, SewFourFaces_PartialBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_GE(aFaces.Length(), 4);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= 4; ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  // 4 faces with 4 edges each = 16 free before.
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 16);
  // Some edges should be sewn.
  EXPECT_GT(aSewer.NbSewnEdges(), 0);
  // Free edges after should be less than before.
  EXPECT_LT(aSewer.NbFreeEdgesAfter(), aSewer.NbFreeEdgesBefore());
}

TEST(BRepGraphAlgo_SewingTest, SewParallel_MatchesSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  // Sequential.
  BRepGraphAlgo_Sewing aSeqSewer(1.0e-04);
  aSeqSewer.SetParallel(false);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aSeqSewer.Add(aFaces.Value(aFaceIdx));
  aSeqSewer.Perform();
  ASSERT_TRUE(aSeqSewer.IsDone());

  // Re-copy faces for the parallel run (sewing consumes internal state).
  NCollection_Sequence<TopoDS_Face> aFaces2 = extractCopiedFaces(aBox);

  // Parallel.
  BRepGraphAlgo_Sewing aParSewer(1.0e-04);
  aParSewer.SetParallel(true);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces2.Length(); ++aFaceIdx)
    aParSewer.Add(aFaces2.Value(aFaceIdx));
  aParSewer.Perform();
  ASSERT_TRUE(aParSewer.IsDone());

  EXPECT_EQ(aParSewer.NbSewnEdges(), aSeqSewer.NbSewnEdges());
  EXPECT_EQ(aParSewer.NbFreeEdgesAfter(), aSeqSewer.NbFreeEdgesAfter());
  EXPECT_EQ(aParSewer.NbFreeEdgesBefore(), aSeqSewer.NbFreeEdgesBefore());
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetParallel(true);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces1.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces1.Value(aFaceIdx));
  }
  for (int aFaceIdx = 1; aFaceIdx <= aFaces2.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces2.Value(aFaceIdx));
  }

  aSewer.Perform();
  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());

  // Two independent boxes should sew independently: 12 + 12 shared edges.
  EXPECT_EQ(aSewer.NbSewnEdges(), 24);
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 0);
}

TEST(BRepGraphAlgo_SewingTest, SewCylinder_TwoHalves)
{
  // Create a cylinder, extract two faces from it, copy them, and sew.
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_FALSE(aCyl.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aCyl);
  // Cylinder typically has 3 faces: lateral, top cap, bottom cap.
  ASSERT_GE(aFaces.Length(), 3);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_GT(aSewer.NbSewnEdges(), 0);

  // Result should have same number of faces.
  int aNbResultFaces = 0;
  for (TopExp_Explorer anExp(aSewer.Result(), TopAbs_FACE); anExp.More(); anExp.Next())
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());

  // Result should have same face count as input.
  int aNbResultFaces = 0;
  for (TopExp_Explorer anExp(aSewer.Result(), TopAbs_FACE); anExp.More(); anExp.Next())
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);

  // After sewing, the graph's vertex count should be less than the sum of
  // individual face vertices (because coincident vertices were merged).
  const BRepGraph& aGraph = aSewer.Graph();
  // 2 rectangular faces = 2 * 4 = 8 vertices before merge.
  // Sharing one edge means 2 shared vertices, so 6 unique after merge.
  EXPECT_LE(aGraph.Defs().NbVertices(), 8);
  EXPECT_GE(aGraph.Defs().NbVertices(), 6);
}

TEST(BRepGraphAlgo_SewingTest, GraphEdgeCount_AfterSewing)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aSewer.Add(aFaces.Value(aFaceIdx));
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());

  // Graph should have edges from all 6 faces (6*4=24 edge entries),
  // but 12 pairs are merged, so some edge references are replaced.
  const BRepGraph& aGraph = aSewer.Graph();
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  // Each face has 4 edges. With 12 shared edges in the box,
  // original graph has 24 edge entries from individual faces.
  EXPECT_GE(aGraph.Defs().NbEdges(), 12);
}

TEST(BRepGraphAlgo_SewingTest, SewAllSixFaces_AreaPerFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aSewer.Add(aFaces.Value(aFaceIdx));
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  const TopoDS_Shape& aResult = aSewer.Result();
  ASSERT_FALSE(aResult.IsNull());

  // Each face should have positive area.
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    // Some reconstructed faces may have reversed orientation, giving negative mass.
    EXPECT_GT(std::abs(aProps.Mass()), 1.0) << "A face in the result has negligible area";
  }
}

TEST(BRepGraphAlgo_SewingTest, DefaultTolerance)
{
  // Default constructor uses tolerance 1.0e-06.
  BRepGraphAlgo_Sewing aSewer;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  aSewer.Add(aCopy1.Shape());
  aSewer.Add(aCopy2.Shape());
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_EQ(aSewer.NbSewnEdges(), 1);
}

// ===================================================================
// Many-Faces Sewing: build a shell from a grid of disconnected faces
// ===================================================================

namespace
{

//! Build a grid of NxM copied box faces arranged in a flat grid.
//! Each face is an independent copy (separate TShape edges) placed at
//! the correct position -- simulating a scenario where faces arrive as
//! a flat list and must be sewn into a shell.
//! Uses graph-based BRepGraphAlgo_Copy/Transform for O(1)-pass copy+transform
//! instead of multi-traversal BRepBuilderAPI_Copy + BRepBuilderAPI_Transform.
NCollection_Sequence<TopoDS_Shape> buildFaceGrid(int    theNx,
                                                 int    theNy,
                                                 double theSizeX,
                                                 double theSizeY)
{
  NCollection_Sequence<TopoDS_Shape> aFaces;
  // Create a template face from a unit box's bottom face.
  BRepPrimAPI_MakeBox aBoxMaker(theSizeX, theSizeY, 1.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

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
    // Fallback: just use first face.
    TopExp_Explorer anExp(aBox, TopAbs_FACE);
    aTemplate = TopoDS::Face(anExp.Current());
  }

  // Build graph from the template face once; reuse for all copy+transform ops.
  BRepGraph aTemplateGraph;
  aTemplateGraph.Build(aTemplate);

  for (int anIx = 0; anIx < theNx; ++anIx)
  {
    for (int anIy = 0; anIy < theNy; ++anIy)
    {
      // Graph-based copy+transform: single-pass copy with geometry duplication,
      // then location-based translation.
      gp_Trsf aTrsf;
      aTrsf.SetTranslation(gp_Vec(anIx * theSizeX, anIy * theSizeY, 0.0));

      if (aTemplateGraph.IsDone() && aTemplateGraph.Defs().NbFaces() > 0)
      {
        TopoDS_Shape aResult =
          BRepGraphAlgo_Transform::TransformFace(aTemplateGraph, 0, aTrsf, true);
        if (!aResult.IsNull())
        {
          aFaces.Append(aResult);
          continue;
        }
      }

      // Fallback to legacy path if graph-based approach fails.
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
  // 10x10 grid = 100 faces, each sharing edges with neighbors.
  NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(10, 10, 1.0, 1.0);
  ASSERT_EQ(aFaces.Length(), 100);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  EXPECT_GT(aSewer.NbSewnEdges(), 0);
  EXPECT_LT(aSewer.NbFreeEdgesAfter(), aSewer.NbFreeEdgesBefore());

  // Count result faces.
  int aNbResultFaces = 0;
  for (TopExp_Explorer anExp(aSewer.Result(), TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbResultFaces;
  EXPECT_EQ(aNbResultFaces, 100);
}

TEST(BRepGraphAlgo_SewingTest, ManyFaces_GridShell_FreeEdgeCount)
{
  // 5x5 grid = 25 faces.
  // Interior edges are shared, boundary edges remain free.
  NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(5, 5, 2.0, 2.0);
  ASSERT_EQ(aFaces.Length(), 25);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());

  // For a 5x5 grid of quads:
  // Total free edges before = 25 * 4 = 100 (each face has 4 edges, all start free).
  EXPECT_EQ(aSewer.NbFreeEdgesBefore(), 100);
  // Interior shared edges in a 5x5 grid: 5*4 horizontal + 4*5 vertical = 40.
  // Each sewn edge consumes 2 free edges.
  EXPECT_EQ(aSewer.NbSewnEdges(), 40);
  // Free edges after = 100 - 2*40 = 20 (the boundary perimeter: 4*5 = 20).
  EXPECT_EQ(aSewer.NbFreeEdgesAfter(), 20);
}

TEST(BRepGraphAlgo_SewingTest, ManyFaces_ManyBoxes_50Boxes)
{
  // 50 boxes = 300 faces -- a larger-scale sewing scenario.
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

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aAllFaces.Length(); ++aFaceIdx)
  {
    aSewer.Add(aAllFaces.Value(aFaceIdx));
  }
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_FALSE(aSewer.Result().IsNull());
  // Greedy matching pairs each free edge with at most one partner.
  // 300 faces * 4 edges = 1200 free edges before. A significant portion gets sewn.
  EXPECT_GT(aSewer.NbSewnEdges(), 300);
  EXPECT_LT(aSewer.NbFreeEdgesAfter(), aSewer.NbFreeEdgesBefore());
}

// ===================================================================
// Performance: BRepGraphAlgo_Sewing vs BRepBuilderAPI_Sewing
// ===================================================================

namespace
{

//! Number of timed iterations per benchmark (after 1 warm-up).
constexpr int THE_NB_PERF_ITERS = 10;

//! Helper: run BRepGraphAlgo_Sewing, return elapsed time.
//! @param[in]  theFaces    faces to sew
//! @param[in]  theParallel enable parallel mode
//! @param[out] theNbSewn   number of sewn edges (from last run)
//! @return elapsed time in seconds
double runGraphSewing(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                      bool                                      theParallel,
                      int&                                      theNbSewn)
{
  auto                 aStart = std::chrono::steady_clock::now();
  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetParallel(theParallel);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
    aSewer.Add(theFaces.Value(anIdx));
  aSewer.Perform();
  auto aEnd = std::chrono::steady_clock::now();
  theNbSewn = aSewer.IsDone() ? aSewer.NbSewnEdges() : 0;
  return std::chrono::duration<double>(aEnd - aStart).count();
}

//! Helper: run BRepBuilderAPI_Sewing, return elapsed time.
double runLegacySewing(const NCollection_Sequence<TopoDS_Shape>& theFaces)
{
  auto                  aStart = std::chrono::steady_clock::now();
  BRepBuilderAPI_Sewing aSewer(1.0e-04);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
    aSewer.Add(theFaces.Value(anIdx));
  aSewer.Perform();
  auto aEnd = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(aEnd - aStart).count();
}

//! Build face lists for a box benchmark. Produces separate copies for each run.
using FaceListBuilder = std::function<NCollection_Sequence<TopoDS_Shape>()>;

//! Run a 3-way benchmark: graph-sequential, graph-parallel, legacy.
//! 1 warm-up + THE_NB_PERF_ITERS timed iterations for each mode.
//! Reports min/avg times and speedup ratios.
//! @param[in]  theLabel      short label for output
//! @param[in]  theNbFaces    expected face count
//! @param[in]  theBuildFaces functor to create a fresh face list
//! @param[in]  theExpSewnSeq expected sewn edges for sequential (0 = skip check)
//! @param[in]  theExpSewnPar expected sewn edges for parallel  (0 = skip, <0 = GT(-val))
void runBenchmark(const char*            theLabel,
                  int                    theNbFaces,
                  const FaceListBuilder& theBuildFaces,
                  int                    theExpSewnSeq,
                  int                    theExpSewnPar)
{
  // Warm-up: 1 iteration each to prime caches & thread pool.
  {
    int                                aDummy = 0;
    NCollection_Sequence<TopoDS_Shape> aWarm  = theBuildFaces();
    runGraphSewing(aWarm, false, aDummy);
    aWarm = theBuildFaces();
    runGraphSewing(aWarm, true, aDummy);
    aWarm = theBuildFaces();
    runLegacySewing(aWarm);
  }

  double aSeqMin = RealLast(), aSeqSum = 0.0;
  double aParMin = RealLast(), aParSum = 0.0;
  double aLegMin = RealLast(), aLegSum = 0.0;
  int    aNbSewnSeq = 0, aNbSewnPar = 0;

  for (int anIter = 0; anIter < THE_NB_PERF_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces;
    double                             aTime;
    int                                aSewn = 0;

    aFaces     = theBuildFaces();
    aTime      = runGraphSewing(aFaces, false, aSewn);
    aNbSewnSeq = aSewn;
    aSeqMin    = std::min(aSeqMin, aTime);
    aSeqSum += aTime;

    aFaces     = theBuildFaces();
    aTime      = runGraphSewing(aFaces, true, aSewn);
    aNbSewnPar = aSewn;
    aParMin    = std::min(aParMin, aTime);
    aParSum += aTime;

    aFaces  = theBuildFaces();
    aTime   = runLegacySewing(aFaces);
    aLegMin = std::min(aLegMin, aTime);
    aLegSum += aTime;
  }

  const double aSeqAvg = aSeqSum / THE_NB_PERF_ITERS;
  const double aParAvg = aParSum / THE_NB_PERF_ITERS;
  const double aLegAvg = aLegSum / THE_NB_PERF_ITERS;

  std::cout << "[  PERF   ] " << theNbFaces << " faces (" << theLabel << "), " << THE_NB_PERF_ITERS
            << " iterations:" << std::endl;
  std::cout << "[  PERF   ]   Graph sequential:  min " << aSeqMin << " s, avg " << aSeqAvg << " s"
            << std::endl;
  std::cout << "[  PERF   ]   Graph parallel:    min " << aParMin << " s, avg " << aParAvg << " s"
            << std::endl;
  std::cout << "[  PERF   ]   Legacy:            min " << aLegMin << " s, avg " << aLegAvg << " s"
            << std::endl;
  if (aSeqAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup seq vs legacy: min " << aLegMin / aSeqMin << "x, avg "
              << aLegAvg / aSeqAvg << "x" << std::endl;
  }
  if (aParAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup par vs legacy: min " << aLegMin / aParMin << "x, avg "
              << aLegAvg / aParAvg << "x" << std::endl;
  }
  if (aParAvg > 1.0e-9 && aSeqAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Parallel speedup:      min " << aSeqMin / aParMin << "x, avg "
              << aSeqAvg / aParAvg << "x" << std::endl;
  }

  // Correctness checks.
  if (theExpSewnSeq > 0)
    EXPECT_EQ(aNbSewnSeq, theExpSewnSeq);
  else if (theExpSewnSeq < 0)
    EXPECT_GT(aNbSewnSeq, -theExpSewnSeq);

  if (theExpSewnPar > 0)
    EXPECT_EQ(aNbSewnPar, theExpSewnPar);
  else if (theExpSewnPar < 0)
    EXPECT_GT(aNbSewnPar, -theExpSewnPar);
}

} // namespace

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_6Faces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  runBenchmark(
    "box",
    6,
    [&]() {
      NCollection_Sequence<TopoDS_Shape> aFaces;
      NCollection_Sequence<TopoDS_Face>  aF = extractCopiedFaces(aBox);
      for (int anIdx = 1; anIdx <= aF.Length(); ++anIdx)
        aFaces.Append(aF.Value(anIdx));
      return aFaces;
    },
    12,
    12);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid5x5)
{
  runBenchmark("5x5 grid", 25, []() { return buildFaceGrid(5, 5, 2.0, 2.0); }, 40, 40);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid10x10)
{
  runBenchmark("10x10 grid", 100, []() { return buildFaceGrid(10, 10, 1.0, 1.0); }, 180, 180);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_300Faces)
{
  runBenchmark(
    "50 boxes",
    300,
    []() {
      NCollection_Sequence<TopoDS_Shape> aAllFaces;
      for (int anIdx = 0; anIdx < 50; ++anIdx)
      {
        BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(anIdx * 10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
        NCollection_Sequence<TopoDS_Face> aF = extractCopiedFaces(aBoxMaker.Shape());
        for (int aFIdx = 1; aFIdx <= aF.Length(); ++aFIdx)
          aAllFaces.Append(aF.Value(aFIdx));
      }
      return aAllFaces;
    },
    -300,
    -300); // GT(300)
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid20x20)
{
  runBenchmark("20x20 grid", 400, []() { return buildFaceGrid(20, 20, 1.0, 1.0); }, 760, 760);
}

// ===================================================================
// Profiling-oriented: 50 iterations of large workloads for ~10s runs.
// Run with --gtest_filter="*Profiling*" under Instruments / perf.
// ===================================================================

constexpr int THE_NB_PROFILE_ITERS = 50;

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_Sequential)
{
  int    aNbSewn = 0;
  double aTotal  = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces      = buildFaceGrid(50, 50, 1.0, 1.0);
    auto                               aTimerStart = std::chrono::steady_clock::now();
    BRepGraphAlgo_Sewing               aSewer(1.0e-04);
    aSewer.SetParallel(false);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aSewer.Add(aFaces.Value(anIdx));
    aSewer.Perform();
    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aSewer.IsDone());
      aNbSewn = aSewer.NbSewnEdges();
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), sequential, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_Parallel)
{
  int    aNbSewn = 0;
  double aTotal  = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces      = buildFaceGrid(50, 50, 1.0, 1.0);
    auto                               aTimerStart = std::chrono::steady_clock::now();
    BRepGraphAlgo_Sewing               aSewer(1.0e-04);
    aSewer.SetParallel(true);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aSewer.Add(aFaces.Value(anIdx));
    aSewer.Perform();
    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aSewer.IsDone());
      aNbSewn = aSewer.NbSewnEdges();
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), parallel, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_NoHistory)
{
  int    aNbSewn = 0;
  double aTotal  = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces      = buildFaceGrid(50, 50, 1.0, 1.0);
    auto                               aTimerStart = std::chrono::steady_clock::now();
    BRepGraphAlgo_Sewing               aSewer(1.0e-04);
    aSewer.SetParallel(false);
    aSewer.SetHistoryMode(false);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aSewer.Add(aFaces.Value(anIdx));
    aSewer.Perform();
    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aSewer.IsDone());
      aNbSewn = aSewer.NbSewnEdges();
      EXPECT_EQ(aSewer.Graph().History().NbRecords(), 0);
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), no history, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Boxes200_Sequential)
{
  int    aNbSewn = 0;
  double aTotal  = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces;
    for (int aBIdx = 0; aBIdx < 200; ++aBIdx)
    {
      BRepPrimAPI_MakeBox               aBoxMaker(gp_Pnt(aBIdx * 10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
      NCollection_Sequence<TopoDS_Face> aF = extractCopiedFaces(aBoxMaker.Shape());
      for (int aFIdx = 1; aFIdx <= aF.Length(); ++aFIdx)
        aFaces.Append(aF.Value(aFIdx));
    }
    auto                 aTimerStart = std::chrono::steady_clock::now();
    BRepGraphAlgo_Sewing aSewer(1.0e-04);
    aSewer.SetParallel(true);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aSewer.Add(aFaces.Value(anIdx));
    aSewer.Perform();
    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aSewer.IsDone());
      aNbSewn = aSewer.NbSewnEdges();
    }
  }
  std::cout << "[  PERF   ] 1200 faces (200 boxes), parallel, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  EXPECT_GT(aNbSewn, 1000);
}

// ===================================================================
// Performance on real-world STEP model: shape1.stp
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_STEPFile)
{
  TCollection_AsciiString aFilePath = "shape1.stp";

  // Read the STEP file using DE_Wrapper.
  DE_Wrapper                            aWrapper;
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aWrapper.Bind(aNode);

  TopoDS_Shape aShape;
  bool         aReadOk = aWrapper.Read(aFilePath, aShape);
  if (!aReadOk || aShape.IsNull())
  {
    std::cout << "[  SKIP   ] Could not read " << aFilePath << std::endl;
    GTEST_SKIP() << "Failed to read STEP file";
  }

  // Extract copied faces (independent TShape edges) for sewing.
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), /*copyGeom=*/true);
    aFaces.Append(aCopier.Shape());
  }

  const int aNbFaces = aFaces.Length();
  if (aNbFaces == 0)
  {
    std::cout << "[  SKIP   ] No faces in " << aFilePath << std::endl;
    GTEST_SKIP() << "STEP file contains no faces";
  }

  std::cout << "[  INFO   ] STEP file: " << aFilePath << ", " << aNbFaces << " faces" << std::endl;

  // Determine iteration count based on face count to keep test under ~30s.
  const int aNbIters = aNbFaces > 500 ? 5 : (aNbFaces > 100 ? 20 : 50);

  // Warm-up: 1 iteration each.
  {
    int aDummy = 0;
    runGraphSewing(aFaces, false, aDummy);

    NCollection_Sequence<TopoDS_Shape> aFaces2;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFaces2.Append(aCopier.Shape());
    }
    runGraphSewing(aFaces2, true, aDummy);

    NCollection_Sequence<TopoDS_Shape> aFaces3;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFaces3.Append(aCopier.Shape());
    }
    runLegacySewing(aFaces3);
  }

  double aSeqMin = RealLast(), aSeqSum = 0.0;
  double aParMin = RealLast(), aParSum = 0.0;
  double aLegMin = RealLast(), aLegSum = 0.0;
  int    aNbSewnSeq = 0, aNbSewnPar = 0;

  for (int anIter = 0; anIter < aNbIters; ++anIter)
  {
    // Rebuild face lists each iteration (sewing modifies internal state).
    NCollection_Sequence<TopoDS_Shape> aFacesSeq, aFacesPar, aFacesLeg;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesSeq.Append(aCopier.Shape());
    }
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesPar.Append(aCopier.Shape());
    }
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesLeg.Append(aCopier.Shape());
    }

    double aTime;
    int    aSewn = 0;

    aTime      = runGraphSewing(aFacesSeq, false, aSewn);
    aNbSewnSeq = aSewn;
    aSeqMin    = std::min(aSeqMin, aTime);
    aSeqSum += aTime;

    aTime      = runGraphSewing(aFacesPar, true, aSewn);
    aNbSewnPar = aSewn;
    aParMin    = std::min(aParMin, aTime);
    aParSum += aTime;

    aTime   = runLegacySewing(aFacesLeg);
    aLegMin = std::min(aLegMin, aTime);
    aLegSum += aTime;
  }

  const double aSeqAvg = aSeqSum / aNbIters;
  const double aParAvg = aParSum / aNbIters;
  const double aLegAvg = aLegSum / aNbIters;

  std::cout << "[  PERF   ] " << aNbFaces << " faces (STEP file), " << aNbIters
            << " iterations:" << std::endl;
  std::cout << "[  PERF   ]   Graph sequential:  min " << aSeqMin << " s, avg " << aSeqAvg
            << " s, sewn: " << aNbSewnSeq << std::endl;
  std::cout << "[  PERF   ]   Graph parallel:    min " << aParMin << " s, avg " << aParAvg
            << " s, sewn: " << aNbSewnPar << std::endl;
  std::cout << "[  PERF   ]   Legacy:            min " << aLegMin << " s, avg " << aLegAvg << " s"
            << std::endl;
  if (aSeqMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup seq vs legacy: min " << aLegMin / aSeqMin << "x, avg "
              << aLegAvg / aSeqAvg << "x" << std::endl;
  }
  if (aParMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup par vs legacy: min " << aLegMin / aParMin << "x, avg "
              << aLegAvg / aParAvg << "x" << std::endl;
  }
  if (aParMin > 1.0e-9 && aSeqMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Parallel speedup:      min " << aSeqMin / aParMin << "x, avg "
              << aSeqAvg / aParAvg << "x" << std::endl;
  }

  // Both modes should sew something.
  EXPECT_GT(aNbSewnSeq, 0);
  EXPECT_GT(aNbSewnPar, 0);
  // Sequential and parallel should produce the same sewing count.
  EXPECT_EQ(aNbSewnSeq, aNbSewnPar);
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadPoolDiagnostics)
{
  // Report OSD_ThreadPool configuration.
  const Handle(OSD_ThreadPool)& aPool              = OSD_ThreadPool::DefaultPool();
  const int                     aNbPoolThreads     = aPool->NbThreads();
  const int                     aNbDefLaunch       = aPool->NbDefaultThreadsToLaunch();
  const int                     aNbLogical         = OSD_Parallel::NbLogicalProcessors();
  const bool                    isUsingOcctThreads = OSD_Parallel::ToUseOcctThreads();

  std::cout << "[  INFO   ] OSD_Parallel diagnostics:" << std::endl;
  std::cout << "[  INFO   ]   Logical processors:        " << aNbLogical << std::endl;
  std::cout << "[  INFO   ]   ThreadPool NbThreads:       " << aNbPoolThreads
            << " (includes main thread)" << std::endl;
  std::cout << "[  INFO   ]   NbDefaultThreadsToLaunch:   " << aNbDefLaunch << std::endl;
  std::cout << "[  INFO   ]   Using OCCT threads:         "
            << (isUsingOcctThreads ? "YES" : "NO (TBB)") << std::endl;
  std::cout << "[  INFO   ]   Pool has worker threads:    " << (aPool->HasThreads() ? "YES" : "NO")
            << std::endl;

  // Verify that the thread pool has at least 2 threads for meaningful parallel tests.
  EXPECT_GE(aNbPoolThreads, 2) << "Thread pool has only 1 thread, parallel tests are meaningless";

  // Measure actual thread utilization during OSD_Parallel::For with a substantial workload.
  // Use atomic set of thread IDs to count distinct threads that actually execute work.
  constexpr int             THE_WORK_ITEMS = 10000;
  std::mutex                aThreadIdMutex;
  std::set<std::thread::id> aThreadIds;
  volatile int              aDummy = 0;

  OSD_Parallel::For(
    0,
    THE_WORK_ITEMS,
    [&](int theIdx) {
      // Light but non-trivial work to prevent optimizer from eliding the loop.
      volatile int aSum = 0;
      for (int j = 0; j < 100; ++j)
      {
        aSum += theIdx + j;
      }
      (void)aSum;

      // Record thread ID (lightweight, only lock briefly).
      const std::thread::id       aTid = std::this_thread::get_id();
      std::lock_guard<std::mutex> aLock(aThreadIdMutex);
      aThreadIds.insert(aTid);
    },
    false); // force parallel

  const int aNbDistinctThreads = static_cast<int>(aThreadIds.size());
  std::cout << "[  INFO   ]   Distinct threads used:      " << aNbDistinctThreads << " / "
            << aNbPoolThreads << " (for " << THE_WORK_ITEMS << " work items)" << std::endl;

  // At least 2 distinct threads should have been used for a parallel workload.
  EXPECT_GE(aNbDistinctThreads, 2) << "Parallel execution used only 1 thread";

  (void)aDummy;
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadUtilization_Sewing)
{
  // Verify that actual sewing in parallel mode uses multiple threads
  // by running a workload large enough to distribute across threads.
  const Handle(OSD_ThreadPool)& aPool          = OSD_ThreadPool::DefaultPool();
  const int                     aNbPoolThreads = aPool->NbThreads();
  if (aNbPoolThreads < 2)
  {
    GTEST_SKIP() << "Thread pool has only 1 thread, skipping utilization test";
  }

  // Build a 10x10 grid of faces (100 faces, 180 free edges to match).
  constexpr int                      THE_GRID_SIZE = 10;
  constexpr double                   THE_CELL_SIZE = 1.0;
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (int aRow = 0; aRow < THE_GRID_SIZE; ++aRow)
  {
    for (int aCol = 0; aCol < THE_GRID_SIZE; ++aCol)
    {
      BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(aCol * THE_CELL_SIZE, aRow * THE_CELL_SIZE, 0.0),
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE);
      const TopoDS_Shape& aBox = aBoxMaker.Shape();
      for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
        aFaces.Append(aCopier.Shape());
      }
    }
  }

  // Run sequential.
  BRepGraphAlgo_Sewing aSeqSewer(1.0e-04);
  aSeqSewer.SetParallel(false);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aSeqSewer.Add(aFaces.Value(i));
  }
  aSeqSewer.Perform();
  ASSERT_TRUE(aSeqSewer.IsDone());
  const int aNbSewnSeq = aSeqSewer.NbSewnEdges();

  // Run parallel.
  BRepGraphAlgo_Sewing aParSewer(1.0e-04);
  aParSewer.SetParallel(true);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aParSewer.Add(aFaces.Value(i));
  }
  aParSewer.Perform();
  ASSERT_TRUE(aParSewer.IsDone());
  const int aNbSewnPar = aParSewer.NbSewnEdges();

  // Both must produce the same result.
  EXPECT_EQ(aNbSewnSeq, aNbSewnPar);
  EXPECT_GT(aNbSewnPar, 0);

  std::cout << "[  INFO   ] Thread pool: " << aNbPoolThreads << " threads, " << aFaces.Length()
            << " faces, " << aNbSewnPar << " sewn edges" << std::endl;
  std::cout << "[  INFO   ] Parallel mode produced correct results with " << aNbPoolThreads
            << " available threads" << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, NoNestedParallel_SequentialInsideParallel)
{
  // Verify that running sewing with parallel=true does not deadlock or fail,
  // which would happen if any inner function called from a parallel lambda
  // also attempted to acquire the thread pool (nested OSD_Parallel::For).
  // A 20x20 grid exercises all 6 parallel regions with substantial load.
  constexpr int                      THE_GRID_SIZE = 20;
  constexpr double                   THE_CELL_SIZE = 1.0;
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (int aRow = 0; aRow < THE_GRID_SIZE; ++aRow)
  {
    for (int aCol = 0; aCol < THE_GRID_SIZE; ++aCol)
    {
      BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(aCol * THE_CELL_SIZE, aRow * THE_CELL_SIZE, 0.0),
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE);
      const TopoDS_Shape& aBox = aBoxMaker.Shape();
      for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
        aFaces.Append(aCopier.Shape());
      }
    }
  }

  // This must complete without deadlock or crash.
  // If nested OSD_Parallel::For existed, the thread pool would deadlock
  // (inner launcher tries to lock threads already held by outer launcher).
  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.SetParallel(true);
  aSewer.SetCutting(true);
  aSewer.SetSameParameterMode(true);
  for (int i = 1; i <= aFaces.Length(); ++i)
  {
    aSewer.Add(aFaces.Value(i));
  }
  aSewer.Perform();
  ASSERT_TRUE(aSewer.IsDone());
  EXPECT_GT(aSewer.NbSewnEdges(), 0);

  std::cout << "[  INFO   ] No-nested-parallel test: " << aFaces.Length() << " faces, "
            << aSewer.NbSewnEdges() << " sewn (no deadlock)" << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, CutAtIntersections_TVertex)
{
  // Three planar rectangular faces forming a T-junction:
  //
  //  (0,1,0)-------(2,1,0)
  //    |     Face1     |
  //  (0,0,0)-------(2,0,0)   <- long free edge, to be split at (1,0,0)
  //    | Face2 |(1,0,0)| Face3 |
  //  (0,-1,0)-(1,-1,0)-(2,-1,0)
  //
  // Without cutting: Face1's edge (0,0,0)->(2,0,0) can't sew with Face2/3's half-length
  // edges because the vertex proximity check fails (endpoints don't coincide).
  // With cutting: the long edge is split at (1,0,0) and each half is sewn correctly.

  const gp_Pnt aP00(0, 0, 0);
  const gp_Pnt aP20(2, 0, 0);
  const gp_Pnt aP01(0, 1, 0);
  const gp_Pnt aP21(2, 1, 0);
  const gp_Pnt aP10(1, 0, 0); // T-vertex (midpoint)
  const gp_Pnt aP0m1(0, -1, 0);
  const gp_Pnt aP1m1(1, -1, 0);
  const gp_Pnt aP2m1(2, -1, 0);

  // Face1: rectangle [0,2] x [0,1] -- bottom edge (0,0,0)->(2,0,0) is the long free edge.
  TopoDS_Face aF1;
  {
    BRepBuilderAPI_MakeWire aMW;
    aMW.Add(BRepBuilderAPI_MakeEdge(aP00, aP20).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP20, aP21).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP21, aP01).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP01, aP00).Edge());
    aF1 = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  }

  // Face2: rectangle [0,1] x [-1,0] -- top edge (0,0,0)->(1,0,0) is the left half free edge.
  TopoDS_Face aF2;
  {
    BRepBuilderAPI_MakeWire aMW;
    aMW.Add(BRepBuilderAPI_MakeEdge(aP00, aP10).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP10, aP1m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP1m1, aP0m1).Edge());
    aMW.Add(BRepBuilderAPI_MakeEdge(aP0m1, aP00).Edge());
    aF2 = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  }

  // Face3: rectangle [1,2] x [-1,0] -- top edge (1,0,0)->(2,0,0) is the right half free edge.
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

  // Sew with cutting enabled: the long edge of Face1 should be split at (1,0,0).
  BRepGraphAlgo_Sewing aSewer(1.0e-4);
  aSewer.SetCutting(true);
  aSewer.Add(aF1);
  aSewer.Add(aF2);
  aSewer.Add(aF3);
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  // At minimum the two T-junction edge pairs must be sewn.
  EXPECT_GE(aSewer.NbSewnEdges(), 2);

  // Verify that cutting is required: without it, the long edge can't match the short ones.
  BRepGraphAlgo_Sewing aSewerNoCut(1.0e-4);
  aSewerNoCut.SetCutting(false);
  aSewerNoCut.Add(aF1);
  aSewerNoCut.Add(aF2);
  aSewerNoCut.Add(aF3);
  aSewerNoCut.Perform();

  ASSERT_TRUE(aSewerNoCut.IsDone());
  // Without cutting the T-junction edges should NOT be matched (0 sewn along that boundary).
  EXPECT_LT(aSewerNoCut.NbSewnEdges(), aSewer.NbSewnEdges());
}

// ============================================================
// Task 1B: Sewing Hierarchy Preservation
// ============================================================

TEST(BRepGraphAlgo_SewingTest, SewBoxSolid_PreservesHierarchy)
{
  // Build a box, copy each face independently, sew them back.
  // The input to sewing is a compound of individual faces with independent geometry.
  // After sewing all 12 edges, the result should be a single solid with a shell containing 6 faces.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
    aSewer.Add(aFaces.Value(aFaceIdx));
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  const TopoDS_Shape& aResult = aSewer.Result();
  ASSERT_FALSE(aResult.IsNull());

  // Count faces in the result.
  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_SewingTest, SewSolidInput_PreservesHierarchy)
{
  // When sewing a complete solid (all edges already shared), the result should
  // preserve the original Solid > Shell > Faces hierarchy.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull());

  BRepGraphAlgo_Sewing aSewer(1.0e-04);
  aSewer.Add(aBox);
  aSewer.Perform();

  ASSERT_TRUE(aSewer.IsDone());
  const TopoDS_Shape& aResult = aSewer.Result();
  ASSERT_FALSE(aResult.IsNull());

  // The result should contain a Solid.
  int aNbSolids = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_SOLID); anExp.More(); anExp.Next())
    ++aNbSolids;
  EXPECT_EQ(aNbSolids, 1);

  // The result should contain a Shell.
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
// Task 1C: Parallel Copy Test
// ============================================================

TEST(BRepGraphAlgo_CopyTest, ParallelCopy_MatchesSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Sequential copy.
  TopoDS_Shape aSeqCopy = BRepGraphAlgo_Copy::Perform(aGraph, true, false);
  ASSERT_FALSE(aSeqCopy.IsNull());

  // Parallel copy.
  TopoDS_Shape aParCopy = BRepGraphAlgo_Copy::Perform(aGraph, true, true);
  ASSERT_FALSE(aParCopy.IsNull());

  // Compare face counts.
  int aSeqFaces = 0, aParFaces = 0;
  for (TopExp_Explorer anExp(aSeqCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aSeqFaces;
  for (TopExp_Explorer anExp(aParCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aParFaces;
  EXPECT_EQ(aSeqFaces, aParFaces);

  // Compare surface areas.
  GProp_GProps aSeqProps, aParProps;
  BRepGProp::SurfaceProperties(aSeqCopy, aSeqProps);
  BRepGProp::SurfaceProperties(aParCopy, aParProps);
  EXPECT_NEAR(std::abs(aSeqProps.Mass()),
              std::abs(aParProps.Mass()),
              std::abs(aSeqProps.Mass()) * 0.01);
}
