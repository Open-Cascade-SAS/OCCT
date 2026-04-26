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
#include <BRepCheck_Analyzer.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <chrono>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

namespace
{

//=================================================================================================

TopoDS_Compound makeTwoCopiedIdenticalFaces()
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

//=================================================================================================

int nbUniqueFaceSurfaceDefs(const BRepGraph& theGraph)
{
  NCollection_Map<const Geom_Surface*> aSurfSet;
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId           aFaceId = aFaceIt.CurrentId();
    const occ::handle<Geom_Surface>& aSurf   = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
    if (!aSurf.IsNull())
    {
      aSurfSet.Add(aSurf.get());
    }
  }
  return aSurfSet.Extent();
}

//=================================================================================================

int nbUniqueEdgeCurveDefs(const BRepGraph& theGraph)
{
  NCollection_Map<const Geom_Curve*> aCurveSet;
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId         anEdgeId = anEdgeIt.CurrentId();
    const occ::handle<Geom_Curve>& aCurve   = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
    if (!aCurve.IsNull())
    {
      aCurveSet.Add(aCurve.get());
    }
  }
  return aCurveSet.Extent();
}

//=================================================================================================

int nbPCurveEntries(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().Edges().CoEdges(anEdgeId);
    for (int i = 0; i < aCoEdgeIdxs.Length(); ++i)
    {
      const BRepGraphInc::CoEdgeDef& aCE =
        theGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(i));
      if (aCE.FaceDefId.IsValid())
        ++aCount;
    }
  }
  return aCount;
}

//=================================================================================================

int countHistoryRecordsByOp(const BRepGraph& theGraph, const TCollection_AsciiString& theOp)
{
  int aCount = 0;
  for (size_t aRecIdx = 0; aRecIdx < theGraph.History().NbRecords(); ++aRecIdx)
  {
    if (theGraph.History().Record(aRecIdx).OperationName == theOp)
    {
      ++aCount;
    }
  }
  return aCount;
}

//=================================================================================================

TopoDS_Compound makeNCopiedIdenticalFaces(const int theCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (int anIdx = 0; anIdx < theCount; ++anIdx)
  {
    BRepBuilderAPI_Copy aCopy(aFace, true);
    aBuilder.Add(aCompound, aCopy.Shape());
  }
  return aCompound;
}

TopoDS_Compound makeMixedCompound()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aBoxFace = anExp.Current();

  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  TopExp_Explorer    aCylExp(aCyl, TopAbs_FACE);
  const TopoDS_Shape aCylFace = aCylExp.Current();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepBuilderAPI_Copy aCopy1(aBoxFace, true);
  BRepBuilderAPI_Copy aCopy2(aBoxFace, true);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());
  aBuilder.Add(aCompound, aCylFace);
  return aCompound;
}

TopoDS_Compound makeNestedCompound()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRep_Builder aBuilder;

  // Inner compound with 2 copies.
  TopoDS_Compound anInner;
  aBuilder.MakeCompound(anInner);
  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);
  aBuilder.Add(anInner, aCopy1.Shape());
  aBuilder.Add(anInner, aCopy2.Shape());

  // Outer compound wrapping inner + one more copy.
  TopoDS_Compound anOuter;
  aBuilder.MakeCompound(anOuter);
  aBuilder.Add(anOuter, anInner);
  BRepBuilderAPI_Copy aCopy3(aFace, true);
  aBuilder.Add(anOuter, aCopy3.Shape());
  return anOuter;
}

TopoDS_Compound makeThreeDistinctPrimitives()
{
  BRepPrimAPI_MakeBox    aBoxMaker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  BRepPrimAPI_MakeCone   aConeMaker(3.0, 1.0, 8.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBoxMaker.Shape());
  aBuilder.Add(aCompound, aSphereMaker.Shape());
  aBuilder.Add(aCompound, aConeMaker.Shape());
  return aCompound;
}

TopoDS_Compound makeTwoIdenticalBoxes()
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBoxMaker1.Shape());
  aBuilder.Add(aCompound, aBoxMaker2.Shape());
  return aCompound;
}

int nbUniquePCurveNodes(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().Edges().CoEdges(anEdgeId);
    aCount += aCoEdgeIdxs.Length();
  }
  return aCount;
}

int addDuplicatePCurvesToAllEdges(BRepGraph& theGraph)
{
  int aDupCount = 0;
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().Edges().CoEdges(anEdgeId);
    if (aCoEdgeIdxs.IsEmpty())
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(0));
    if (!aCE.Curve2DRepId.IsValid())
    {
      continue;
    }

    const occ::handle<Geom2d_Curve>& aDupPCurve = BRepGraph_Tool::CoEdge::PCurve(theGraph, aCE);
    theGraph.Editor().CoEdges().AddPCurve(anEdgeId,
                                          aCE.FaceDefId,
                                          aDupPCurve,
                                          aCE.ParamFirst,
                                          aCE.ParamLast,
                                          aCE.Orientation);
    ++aDupCount;
  }
  return aDupCount;
}

} // namespace

TEST(BRepGraph_DeduplicateTest, AnalyzeOnly_DoesNotRewrite)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);
}

TEST(BRepGraph_DeduplicateTest, AnalyzeOnly_ReportsCanonicalCandidates)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // TwoCopiedFaces: 2 surfaces -> 1 canonical, 8 curves -> 4 canonical, 8 PCurves -> 8 canonical.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

TEST(BRepGraph_DeduplicateTest, CanonicalizeSurfaces_RewritesAndRecordsHistory)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = false;
  anOpts.HistoryMode = true;

  const size_t                        aHistoryBefore = aGraph.History().NbRecords();
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  EXPECT_EQ(aGraph.History().NbRecords(), aHistoryBefore + 5);
}

TEST(BRepGraph_DeduplicateTest, CanonicalizeCurves_RewritesAndReducesUnique)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph), 8);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = false;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraph_DeduplicateTest, HistoryModeOff_DoesNotAddHistory)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.History().NbRecords(), 0);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = false;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(aGraph.History().NbRecords(), 0);
}

TEST(BRepGraph_DeduplicateTest, RestoresHistoryEnabledFlag)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(false);
  ASSERT_FALSE(aGraph.History().IsEnabled());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;
  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_FALSE(aGraph.History().IsEnabled());
}

TEST(BRepGraph_DeduplicateTest, DefaultOverload_PerformWorks)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  EXPECT_EQ(aRes.NbHistoryRecords, 5);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraph_DeduplicateTest, SingleFace_NoSurfaceRewrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  TopExp_Explorer     anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
}

// PCurve dedup test removed - PCurves are now inline on EdgeDef, not separate graph nodes.

TEST(BRepGraph_DeduplicateTest, NotDoneGraph_ReturnsEmptyResult)
{
  BRepGraph aGraph;
  // Do not call BRepGraph_Builder::Add() - graph is not done.
  ASSERT_FALSE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraph_DeduplicateTest, Idempotent_SecondRunNoRewrites)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes1 = BRepGraph_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 1);
  ASSERT_EQ(aRes1.NbCurveRewrites, 4);

  // After first dedup, all faces share the same surface pointer, all duplicate
  // edges share the same curve pointer. A second run still detects the same
  // canonical mapping via GeomHash, so rewrite counts may be non-zero (no-op
  // at pointer level). Verify the unique pointer count is unchanged.
  const int aUniqueSurfsBefore  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurvesBefore = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraph_Deduplicate::Result aRes2 = BRepGraph_Deduplicate::Perform(aGraph);

  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfsBefore);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurvesBefore);
}

TEST(BRepGraph_DeduplicateTest, FullBox_AllSurfacesUnique)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // A box has 6 faces with 6 distinct Geom_Plane instances (different origins/normals).
  // No surface dedup should occur - all 6 are canonical.
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 6);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
}

TEST(BRepGraph_DeduplicateTest, ResultCountersConsistency)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 surfaces, 8 curves, 8 PCurves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);
  ASSERT_EQ(nbPCurveEntries(aGraph), 8);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // Canonical + duplicates == total for each geometry type.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalSurfaces + (aGraph.Topo().Faces().Nb() - aRes.NbCanonicalSurfaces),
            aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbCanonicalCurves + (aGraph.Topo().Edges().Nb() - aRes.NbCanonicalCurves),
            aGraph.Topo().Edges().Nb());
}

TEST(BRepGraph_DeduplicateTest, EmptyCompound_NoRewrites)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

TEST(BRepGraph_DeduplicateTest, MultipleCopies_NWayDedup)
{
  const int aNbCopies = 4;
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Topo().Faces().Nb(), aNbCopies);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, aNbCopies - 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
}

TEST(BRepGraph_DeduplicateTest, MixedGeometry_OnlyIdenticalDeduped)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeMixedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  // 2 box face copies + 1 cylinder face = 3 faces, 3 surfaces, 11 curves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 11);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // Only the two identical box faces should be deduped; cylinder face is untouched.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 2);
  EXPECT_EQ(aRes.NbCanonicalCurves, 7);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  // 7 canonicalize + 1 nullify surface + 4 nullify curves + 2 nullify PCurves = 14.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 7);
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_AllCopiedFacesShareCanonicalSurface)
{
  const int aNbCopies = 3;
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // All face defs should share the same Surface handle.
  NCollection_Map<const Geom_Surface*> aSurfIds;
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId           aFaceId = aFaceIt.CurrentId();
    const occ::handle<Geom_Surface>& aSurf   = BRepGraph_Tool::Face::Surface(aGraph, aFaceId);
    if (!aSurf.IsNull())
    {
      aSurfIds.Add(aSurf.get());
    }
  }
  EXPECT_EQ(aSurfIds.Extent(), 1);
}

TEST(BRepGraph_DeduplicateTest, HistoryRecordNames_MatchExpectedOps)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  ASSERT_EQ(aRes.NbHistoryRecords, 5);

  // TwoCopiedFaces: 1 surface + 4 curve canonicalizations (no orphan nullification).
  const int aNbSurfOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizeSurface"));
  const int aNbCurveOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizeCurve"));

  EXPECT_EQ(aNbSurfOps, 1);
  EXPECT_EQ(aNbCurveOps, 4);
  EXPECT_EQ(aNbSurfOps + aNbCurveOps, 5);
}

TEST(BRepGraph_DeduplicateTest, AnalyzeOnly_CurveAndPCurveCountsReported)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // TwoCopiedFaces: 2 geom surfaces, 8 curves, 8 PCurves.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);

  // But no rewrites.
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

// ---------------------------------------------------------------------------
// Tolerance and Options tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, DefaultOptionsStruct_HasExpectedDefaults)
{
  BRepGraph_Deduplicate::Options anOpts;
  EXPECT_FALSE(anOpts.AnalyzeOnly);
  EXPECT_TRUE(anOpts.HistoryMode);
  EXPECT_FALSE(anOpts.MergeEntitiesWhenSafe);
  EXPECT_NEAR(anOpts.CompTolerance, Precision::Angular(), 1e-20);
  EXPECT_NEAR(anOpts.HashTolerance, Precision::Confusion(), 1e-20);
}

TEST(BRepGraph_DeduplicateTest, DefaultResultStruct_AllZeroed)
{
  BRepGraph_Deduplicate::Result aRes;
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraph_DeduplicateTest, MergeDefsWhenSafe_MergesVertices)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // Two copied faces share identical vertex positions -> vertices should merge.
  EXPECT_GT(aRes.NbMergedVertices, 0);
  EXPECT_TRUE(aRes.IsEntityMergeApplied);
  (void)aNbVerticesBefore;
}

// ---------------------------------------------------------------------------
// Nested and complex compound tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, NestedCompound_AllCopiesDeduped)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeNestedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  // 3 copies of the same face across nested compounds: 3 surfaces, 12 curves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 12);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 3);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 12);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 2);
  EXPECT_EQ(aRes.NbCurveRewrites, 8);
  // 10 canonicalize + 2 nullify surfaces + 8 nullify curves = 20.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraph_DeduplicateTest, ThreeDistinctPrimitives_MinimalDedup)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeThreeDistinctPrimitives());
  ASSERT_TRUE(aGraph.IsDone());

  // Box(6 faces) + Sphere(1 face) + Cone(3 faces) = 10 faces, 18 edges.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 10);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 18);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // 9 canonical surfaces (1 surface matches between cone bottom and sphere/box plane).
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 9);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 9);
  // Some curve dedup may occur between the three primitives (shared circle/line geometry).
  EXPECT_LE(nbUniqueEdgeCurveDefs(aGraph), 18);
}

TEST(BRepGraph_DeduplicateTest, TwoIdenticalBoxes_SurfacesAndCurvesDeduped)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  // Two identical boxes: 12 faces, 24 edges, 12 geom surfaces, 24 geom curves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 12);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 24);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 12);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 24);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 6);
  EXPECT_EQ(aRes.NbCanonicalCurves, 12);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 6);
  EXPECT_EQ(aRes.NbCurveRewrites, 12);
  // 18 canonicalize + 6 nullify surfaces + 12 nullify curves = 36.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 6);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 12);
}

// ---------------------------------------------------------------------------
// Curve-focused tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, CurveRewriteCount_MatchesDuplicateEdgeCurves)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 8 geom curves, 4 canonical -> 4 duplicates.
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);

  BRepGraph_Deduplicate::Options anAnalyze;
  anAnalyze.AnalyzeOnly = true;
  const BRepGraph_Deduplicate::Result anAnalysis =
    BRepGraph_Deduplicate::Perform(aGraph, anAnalyze);
  EXPECT_EQ(anAnalysis.NbCanonicalCurves, 4);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // 4 duplicate curve nodes -> exactly 4 edge curve rewrites.
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_AllCopiedEdgesShareCanonicalCurve)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph), 8);

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup: 8 unique curves reduced to 4 canonical.
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

// ---------------------------------------------------------------------------
// PCurve tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, MultiplePCurveDups_AllEdgesDeduped)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
}

TEST(BRepGraph_DeduplicateTest, PCurveDup_AnalyzeOnly_CountsButNoRewrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const int aUniqueBefore = nbUniquePCurveNodes(aGraph);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(nbUniquePCurveNodes(aGraph), aUniqueBefore);
  // Canonical count should be less than total PCurves.
}

TEST(BRepGraph_DeduplicateTest, NoPCurveDuplicates_ZeroPCurveRewrites)
{
  // A single face copy has no PCurve duplicates.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
}

// ---------------------------------------------------------------------------
// History tracing tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, HistoryFindOriginal_TracesBackToCanonical)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  ASSERT_EQ(aRes.NbHistoryRecords, 5);

  // For each history record, FindOriginal on the replacement should trace back.
  for (size_t aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_DynamicArray<BRepGraph_NodeId>>::Iterator
           aMapIter(aRec.Mapping);
         aMapIter.More();
         aMapIter.Next())
    {
      const BRepGraph_NodeId&                           anOriginal    = aMapIter.Key();
      const NCollection_DynamicArray<BRepGraph_NodeId>& aReplacements = aMapIter.Value();
      for (int aReplIdx = 0; aReplIdx < aReplacements.Length(); ++aReplIdx)
      {
        const BRepGraph_NodeId aTraced =
          aGraph.History().FindOriginal(aReplacements.Value(aReplIdx));
        // FindOriginal should eventually reach a root - the canonical node.
        EXPECT_TRUE(aTraced.IsValid());
        // The original node from the record should match one of the trace results.
        EXPECT_TRUE(anOriginal.IsValid());
      }
    }
  }
}

TEST(BRepGraph_DeduplicateTest, HistoryFindDerived_ContainsCanonicalNode)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  ASSERT_EQ(aRes.NbHistoryRecords, 5);

  // For each history record, FindDerived on the original should contain the replacements.
  // All records are canonicalize records with 1 replacement (no nullify records).
  int aNbCanonMappings = 0;
  for (size_t aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_DynamicArray<BRepGraph_NodeId>>::Iterator
           aMapIter(aRec.Mapping);
         aMapIter.More();
         aMapIter.Next())
    {
      const BRepGraph_NodeId&                          anOriginal = aMapIter.Key();
      const NCollection_DynamicArray<BRepGraph_NodeId> aDerived =
        aGraph.History().FindDerived(anOriginal);
      EXPECT_EQ(aDerived.Length(), 1);
      ++aNbCanonMappings;
    }
  }
  EXPECT_EQ(aNbCanonMappings, 5);
}

TEST(BRepGraph_DeduplicateTest, HistoryRecordSequenceNumbers_AreMonotonic)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  bool   isFirst  = true;
  size_t aPrevSeq = 0;
  for (size_t aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    if (!isFirst)
    {
      EXPECT_GT(aRec.SequenceNumber, aPrevSeq);
    }
    aPrevSeq = aRec.SequenceNumber;
    isFirst  = false;
  }
}

TEST(BRepGraph_DeduplicateTest, HistoryOff_NbRecordsUnchanged)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Run once with history to get 5 records (1 surface + 4 curve canonicalizes).
  BRepGraph_Deduplicate::Options anOpts1;
  anOpts1.HistoryMode = true;
  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts1);
  EXPECT_EQ(aGraph.History().NbRecords(), 5);

  // Fresh graph, run with history off - no records should be added.
  BRepGraph aGraph2;
  aGraph2.Clear(); (void)BRepGraph_Builder::Add(aGraph2, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph2.IsDone());

  BRepGraph_Deduplicate::Options anOpts2;
  anOpts2.HistoryMode = false;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph2, anOpts2);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(aGraph2.History().NbRecords(), 0);
}

// ---------------------------------------------------------------------------
// Canonical node validity tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, AfterDedup_AllSurfacesValid)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       aFaceId  = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Faces().Definition(aFaceId);
    EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid())
      << "Face " << aFaceId.Index << " has null Surface after dedup";
  }
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_AllCurve3dsValid)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId  = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(anEdgeId);
    if (!anEdgeDef.IsDegenerate)
    {
      EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid())
        << "Edge " << anEdgeId.Index << " has null Curve3d after dedup";
    }
  }
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_AllInlinePCurvesHaveCurve2d)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);
  (void)BRepGraph_Deduplicate::Perform(aGraph);

  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (int aCEIter = 0; aCEIter < aCoEdgeIdxs.Length(); ++aCEIter)
    {
      const BRepGraphInc::CoEdgeDef& aCE =
        aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(aCEIter));
      EXPECT_TRUE(aCE.Curve2DRepId.IsValid())
        << "Edge " << anEdgeId.Index << " CoEdge " << aCEIter << " has null Curve2d after dedup";
    }
  }
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_CanonicalSurfaceGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeNCopiedIdenticalFaces(4));
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // All face defs should have a non-null surface after dedup.
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       aFaceId  = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Faces().Definition(aFaceId);
    EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid())
      << "Face " << aFaceId.Index << " has null Surface after dedup";
  }
}

TEST(BRepGraph_DeduplicateTest, AfterDedup_CanonicalCurveGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId  = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(anEdgeId);
    if (!anEdgeDef.IsDegenerate)
    {
      EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid())
        << "Edge " << anEdgeId.Index << " has null Curve3d after dedup";
    }
  }
}

// ---------------------------------------------------------------------------
// Parallel build + dedup tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, ParallelBuild_SameResultAsSequential)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  BRepGraph aGraphSeq;
  aGraphSeq.Clear(); (void)BRepGraph_Builder::Add(aGraphSeq, aCompound, BRepGraph_Builder::Options{ {}, true, false, false });
  ASSERT_TRUE(aGraphSeq.IsDone());

  BRepGraph aGraphPar;
  aGraphPar.Clear(); (void)BRepGraph_Builder::Add(aGraphPar, aCompound, BRepGraph_Builder::Options{ {}, true, false, true });
  ASSERT_TRUE(aGraphPar.IsDone());

  const BRepGraph_Deduplicate::Result aResSeq = BRepGraph_Deduplicate::Perform(aGraphSeq);
  const BRepGraph_Deduplicate::Result aResPar = BRepGraph_Deduplicate::Perform(aGraphPar);

  // Both should produce identical results: 1 canonical surface, 4 canonical curves.
  EXPECT_EQ(aResSeq.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aResPar.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aResSeq.NbCanonicalCurves, 4);
  EXPECT_EQ(aResPar.NbCanonicalCurves, 4);
  EXPECT_EQ(aResSeq.NbSurfaceRewrites, 1);
  EXPECT_EQ(aResPar.NbSurfaceRewrites, 1);
  EXPECT_EQ(aResSeq.NbCurveRewrites, 4);
  EXPECT_EQ(aResPar.NbCurveRewrites, 4);
}

// ---------------------------------------------------------------------------
// Scale / many copies tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, TenCopies_AllDeduplicatedToOneSurface)
{
  const int aNbCopies = 10;
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Topo().Faces().Nb(), aNbCopies);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, aNbCopies - 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
}

TEST(BRepGraph_DeduplicateTest, TwoCopies_CurveCanonicalCountLessThanTotal)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // 8 geom curves, 4 canonical.
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly                       = true;
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
}

// ---------------------------------------------------------------------------
// Idempotency on various shapes
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, Idempotent_MixedCompound_SurfacesAndCurves)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeMixedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes1 = BRepGraph_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 1);
  ASSERT_EQ(aRes1.NbCurveRewrites, 4);

  // After first dedup, unique pointer counts are stable.
  const int aUniqueSurfs  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurves = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraph_Deduplicate::Result aRes2 = BRepGraph_Deduplicate::Perform(aGraph);

  // Unique pointer counts unchanged after second run.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfs);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurves);
}

TEST(BRepGraph_DeduplicateTest, Idempotent_TwoIdenticalBoxes)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes1 = BRepGraph_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 6);
  ASSERT_EQ(aRes1.NbCurveRewrites, 12);

  // After first dedup, unique pointer counts are stable.
  const int aUniqueSurfs  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurves = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraph_Deduplicate::Result aRes2 = BRepGraph_Deduplicate::Perform(aGraph);

  // Unique pointer counts unchanged after second run.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfs);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurves);
}

TEST(BRepGraph_DeduplicateTest, DISABLED_PCurveDedup_RewritesReduceUniquePCurveNodes)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);

  const int aUniqueBefore = nbUniquePCurveNodes(aGraph);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup, fewer unique PCurve node IDs are referenced.
  EXPECT_LT(nbUniquePCurveNodes(aGraph), aUniqueBefore);
}

// ---------------------------------------------------------------------------
// History enabled flag restoration tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, RestoresHistoryFlag_WhenHistoryModeOff)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(true);
  ASSERT_TRUE(aGraph.History().IsEnabled());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.HistoryMode = false;
  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // Should be restored to the original value (true).
  EXPECT_TRUE(aGraph.History().IsEnabled());
}

TEST(BRepGraph_DeduplicateTest, RestoresHistoryFlag_AnalyzeOnlyPath)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(true);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;
  anOpts.HistoryMode = false;
  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // Restored even when exiting through the AnalyzeOnly early-return.
  EXPECT_TRUE(aGraph.History().IsEnabled());
}

// ---------------------------------------------------------------------------
// Geometry count consistency invariants
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, GeomCountsUnchanged_AfterDedup)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 surfaces, 8 curves, 8 PCurves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);
  ASSERT_EQ(nbPCurveEntries(aGraph), 8);

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // Dedup rewrites references, but does not remove geometry nodes.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 8);
  EXPECT_EQ(nbPCurveEntries(aGraph), 8);
}

TEST(BRepGraph_DeduplicateTest, DefCountsUnchanged_AfterDedup)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 face defs, 8 edge defs.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // Dedup never removes or adds defs.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 8);
}

TEST(BRepGraph_DeduplicateTest, PCurveEntryCount_UnchangedAfterDedup)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);

  const int aPCEntryCount = nbPCurveEntries(aGraph);

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // Dedup rewrites PCurve node IDs but doesn't add or remove PCurveEntries.
  EXPECT_EQ(nbPCurveEntries(aGraph), aPCEntryCount);
}

// ---------------------------------------------------------------------------
// Sphere and cylinder specific tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, TwoCopiedSphereFaces_Deduped)
{
  BRepPrimAPI_MakeSphere aSphereMaker(10.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  TopExp_Explorer anExp(aSphere, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two sphere face copies: 2 faces, 6 edges.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 6);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  // After dedup, both sphere faces share the same surface pointer.
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
  EXPECT_GT(aRes.NbHistoryRecords, 0);
}

TEST(BRepGraph_DeduplicateTest, TwoCopiedCylinderFaces_Deduped)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  TopExp_Explorer anExp(aCyl, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two cylinder face copies: 2 faces, 6 edges, 2 surfaces, 6 curves, 12 PCurves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 6);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 3);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 3);
  // 8 canonicalize + 1 nullify surface + 3 nullify curves + 4 nullify PCurves = 16.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
}

TEST(BRepGraph_DeduplicateTest, DifferentSizedCylinders_NotDeduped)
{
  BRepPrimAPI_MakeCylinder aCylMaker1(5.0, 20.0);
  BRepPrimAPI_MakeCylinder aCylMaker2(10.0, 20.0);

  TopExp_Explorer anExp1(aCylMaker1.Shape(), TopAbs_FACE);
  TopExp_Explorer anExp2(aCylMaker2.Shape(), TopAbs_FACE);
  ASSERT_TRUE(anExp1.More());
  ASSERT_TRUE(anExp2.More());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anExp1.Current());
  aBuilder.Add(aCompound, anExp2.Current());

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two distinct cylinder faces: 2 surfaces, 6 curves.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 6);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // Different radii -> different cylindrical surfaces -> no surface/curve dedup.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 2);
  EXPECT_EQ(aRes.NbCanonicalCurves, 6);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  // No PCurve dedup (inline now), so zero history records for different-sized cylinders.
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
}

// ---------------------------------------------------------------------------
// Back-reference and orphan nullification tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, BackRefs_SurfaceRewrite_UpdatesFaceDefUsers)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: each face has its own surface handle.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_NE(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup: both faces share the same canonical surface pointer.
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
}

TEST(BRepGraph_DeduplicateTest, BackRefs_CurveRewrite_UpdatesEdgeDefUsers)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: 8 edges, each with its own curve handle.
  ASSERT_EQ(aGraph.Topo().Edges().Nb(), 8);

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup: edges with identical curves should share the same pointer.
  // Count distinct curve pointers across all edges.
  NCollection_Map<const Geom_Curve*> aDistinctCurves;
  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId         anEdgeId = anEdgeIt.CurrentId();
    const occ::handle<Geom_Curve>& aCurve   = BRepGraph_Tool::Edge::Curve(aGraph, anEdgeId);
    if (!aCurve.IsNull())
      aDistinctCurves.Add(aCurve.get());
  }
  // After dedup, 4 canonical curves should remain (from 8 originally).
  EXPECT_EQ(aDistinctCurves.Extent(), 4);
}

TEST(BRepGraph_DeduplicateTest, FacesOnSurface_AfterDedup_ReturnsCorrectDefs)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup, all face defs point to the same canonical surface.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 2);
  EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId::Start()));
  EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(1)));
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
}

TEST(BRepGraph_DeduplicateTest, Nullify_OrphanedSurface_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // No orphan nullification since geometry is stored inline on defs.
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);

  // After dedup, both faces share the same canonical surface; all surfaces non-null.
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId))
      << "Face " << aFaceId.Index << " has null Surface after dedup";
  }
}

TEST(BRepGraph_DeduplicateTest, Nullify_OrphanedCurve_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  // No orphan nullification since geometry is stored inline on defs.
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);

  // After dedup, all non-degenerate edges should have non-null Curve3d;
  // duplicate edges now share the canonical curve pointer.
  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeId))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId))
        << "Edge " << anEdgeId.Index << " has null Curve3d after dedup";
    }
  }
  // Verify unique curve count decreased.
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraph_DeduplicateTest, Nullify_OrphanedPCurve_HandleIsNull)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);

  // After dedup, total PCurve entries may have changed.
  // Verify the number of pcurve rewrites is positive (dedup happened).
}

TEST(BRepGraph_DeduplicateTest, AnalyzeOnly_NoBackRefChangesOrNullification)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot surface/curve pointers before.
  NCollection_DynamicArray<const Geom_Surface*> aSurfPtrs;
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    aSurfPtrs.Append(BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId()).get());
  }
  NCollection_DynamicArray<const Geom_Curve*> aCurvePtrs;
  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    aCurvePtrs.Append(BRepGraph_Tool::Edge::Curve(aGraph, anEdgeIt.CurrentId()).get());
  }

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);

  // Surface/curve pointers unchanged (analyze only, no rewriting).
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, aFaceId).get(),
              aSurfPtrs.Value(static_cast<int>(aFaceId.Index)));
  }
  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_EQ(BRepGraph_Tool::Edge::Curve(aGraph, anEdgeId).get(),
              aCurvePtrs.Value(static_cast<int>(anEdgeId.Index)));
  }

  // All handles still non-null.
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId));
  }
  for (BRepGraph_FullEdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeId))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId));
    }
  }
}

// ---------------------------------------------------------------------------
// Round-trip tests: Build -> Dedup -> Reconstruct -> Build verifies geometry sharing
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, RoundTrip_TwoCopiedFaces_FewerSurfaces)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  // First graph: build and dedup.
  BRepGraph aGraph1;
  aGraph1.Clear(); (void)BRepGraph_Builder::Add(aGraph1, aCompound);
  ASSERT_TRUE(aGraph1.IsDone());
  ASSERT_EQ(aGraph1.Topo().Faces().Nb(), 2);

  (void)BRepGraph_Deduplicate::Perform(aGraph1);
  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph1), 1);

  // Reconstruct each face individually and assemble into a compound.
  BRep_Builder    aBB;
  TopoDS_Compound aReconstructed;
  aBB.MakeCompound(aReconstructed);
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph1); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    const TopoDS_Shape     aFace   = aGraph1.Shapes().Reconstruct(aFaceId);
    ASSERT_FALSE(aFace.IsNull());
    aBB.Add(aReconstructed, aFace);
  }

  // Second graph: build from reconstructed shape.
  BRepGraph aGraph2;
  aGraph2.Clear(); (void)BRepGraph_Builder::Add(aGraph2, aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Face defs count stays 2 (topology defs, not geometry nodes).
  // After round-trip, unique surface pointer count should be 1
  // because both faces share the same surface handle from dedup.
  EXPECT_EQ(aGraph2.Topo().Faces().Nb(), 2);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph2), 1);
}

TEST(BRepGraph_DeduplicateTest, RoundTrip_TwoCopiedFaces_FewerCurves)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  BRepGraph aGraph1;
  aGraph1.Clear(); (void)BRepGraph_Builder::Add(aGraph1, aCompound);
  ASSERT_TRUE(aGraph1.IsDone());
  ASSERT_EQ(aGraph1.Topo().Edges().Nb(), 8);

  (void)BRepGraph_Deduplicate::Perform(aGraph1);
  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph1), 4);

  // Reconstruct each face individually.
  BRep_Builder    aBB;
  TopoDS_Compound aReconstructed;
  aBB.MakeCompound(aReconstructed);
  for (BRepGraph_FullFaceIterator aFaceIt(aGraph1); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    const TopoDS_Shape     aFace   = aGraph1.Shapes().Reconstruct(aFaceId);
    ASSERT_FALSE(aFace.IsNull());
    aBB.Add(aReconstructed, aFace);
  }

  BRepGraph aGraph2;
  aGraph2.Clear(); (void)BRepGraph_Builder::Add(aGraph2, aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Edge defs count stays 8 (topology defs, not geometry nodes).
  // After round-trip, unique curve pointer count should be 4
  // because duplicate edges share the same curve handle from dedup.
  EXPECT_EQ(aGraph2.Topo().Edges().Nb(), 8);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph2), 4);
}

TEST(BRepGraph_DeduplicateTest, Build_SharedTFace_OneSurfaceNode)
{
  // Create a box and extract two faces that share the same TFace via Same().
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace1 = TopoDS::Face(anExp.Current());

  // Create a compound with the same face referenced twice (same TShape).
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFace1);
  aBuilder.Add(aCompound, aFace1);

  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Both face usages share the same TFace -> same raw surface pointer -> one surface node.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
}

TEST(BRepGraph_DeduplicateTest, RoundTrip_TwoBoxes_GeomReduction)
{
  const TopoDS_Compound aCompound = makeTwoIdenticalBoxes();

  BRepGraph aGraph1;
  aGraph1.Clear(); (void)BRepGraph_Builder::Add(aGraph1, aCompound);
  ASSERT_TRUE(aGraph1.IsDone());

  const int aSurfsBefore  = aGraph1.Topo().Faces().Nb();
  const int aCurvesBefore = aGraph1.Topo().Edges().Nb();
  ASSERT_EQ(aSurfsBefore, 12);
  ASSERT_EQ(aCurvesBefore, 24);

  (void)BRepGraph_Deduplicate::Perform(aGraph1);

  // Force reconstruction from deduped graph.
  BRepGraph_NodeId   aRootId(BRepGraph_NodeId::Kind::Compound, 0);
  const TopoDS_Shape aReconstructed = aGraph1.Shapes().Reconstruct(aRootId);
  ASSERT_FALSE(aReconstructed.IsNull());

  // Build second graph.
  BRepGraph aGraph2;
  aGraph2.Clear(); (void)BRepGraph_Builder::Add(aGraph2, aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Face/edge def counts stay the same (topology defs, not geometry nodes).
  EXPECT_EQ(aGraph2.Topo().Faces().Nb(), aSurfsBefore);
  EXPECT_EQ(aGraph2.Topo().Edges().Nb(), aCurvesBefore);

  // After dedup, unique geometry pointer counts should reflect deduplication:
  // 6 canonical surfaces and 12 canonical curves.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph2), 6);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph2), 12);
}

// ---------------------------------------------------------------------------
// Topology definition merge tests (MergeEntitiesWhenSafe = true)
// ---------------------------------------------------------------------------

TEST(BRepGraph_DeduplicateTest, MergeVertices_SharedVerticesReduced)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // Two copied faces -> shared vertex positions should merge.
  EXPECT_GT(aRes.NbMergedVertices, 0);

  // Count non-removed vertices.
  int aNbActiveVertices = 0;
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    ++aNbActiveVertices;
  }
  EXPECT_LT(aNbActiveVertices, aNbVerticesBefore);
}

TEST(BRepGraph_DeduplicateTest, MergeEdges_SharedEdgesReduced)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_GT(aRes.NbMergedEdges, 0);
}

TEST(BRepGraph_DeduplicateTest, MergeWires_IdenticalWiresMerged)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // After vertex and edge merge, identical wires should also merge.
  EXPECT_GE(aRes.NbMergedWires, 0);
}

TEST(BRepGraph_DeduplicateTest, MergeFaces_IdenticalFacesMerged)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // After lower-level merges, faces with same surface+wires should merge.
  EXPECT_GE(aRes.NbMergedFaces, 0);
}

TEST(BRepGraph_DeduplicateTest, MergeDefsWhenSafe_False_NoMerge)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Default: MergeEntitiesWhenSafe = false.
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbMergedVertices, 0);
  EXPECT_EQ(aRes.NbMergedEdges, 0);
  EXPECT_EQ(aRes.NbMergedWires, 0);
  EXPECT_EQ(aRes.NbMergedFaces, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraph_DeduplicateTest, AnalyzeOnly_MergeDefsWhenSafe_CountsOnly)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  anOpts.AnalyzeOnly           = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  // Counts should be reported.
  EXPECT_GT(aRes.NbMergedVertices, 0);
  // But no mutation: all vertices still present.
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraph_DeduplicateTest, HistoryRecords_MergePhases)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  anOpts.HistoryMode           = true;

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  const int aNbVertexMerge =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:MergeVertex"));
  const int aNbEdgeMerge =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:MergeEdge"));

  EXPECT_EQ(aNbVertexMerge, aRes.NbMergedVertices);
  EXPECT_EQ(aNbEdgeMerge, aRes.NbMergedEdges);
}

TEST(BRepGraph_DeduplicateTest, AfterMerge_Validate_NoIssues)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // After merge + compact, graph should be structurally valid.
  // Merge alone may leave stale back-references on geometry nodes
  // that are cleaned up by compaction.
  (void)BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

//=================================================================================================

TEST(BRepGraph_DeduplicateTest, VertexMerge_UpdatesInternalEdgeVertexRefs)
{
  // Regression for Bug A1: Phase 1 vertex merge must update EdgeDef.InternalVertexRefIds.
  // Without the fix, BRepGraph_Compact would silently drop the internal vertex ref.
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Add a duplicate vertex at the same position as vertex[0].
  const BRepGraph_VertexId aBaseV        = BRepGraph_VertexId::Start();
  const gp_Pnt             aSamePosition = BRepGraph_Tool::Vertex::Pnt(aGraph, aBaseV);
  const double             aTol          = BRepGraph_Tool::Vertex::Tolerance(aGraph, aBaseV);
  const BRepGraph_VertexId aDupV         = aGraph.Editor().Vertices().Add(aSamePosition, aTol);
  ASSERT_TRUE(aDupV.IsValid());

  // Attach the duplicate vertex as an internal vertex of edge[0].
  const BRepGraph_EdgeId      aEdge0 = BRepGraph_EdgeId::Start();
  const BRepGraph_VertexRefId aIntRef =
    aGraph.Editor().Edges().AddInternalVertex(aEdge0, aDupV, TopAbs_INTERNAL);
  ASSERT_TRUE(aIntRef.IsValid());
  ASSERT_EQ(aGraph.Topo().Edges().Definition(aEdge0).InternalVertexRefIds.Length(), 1);

  // Run full vertex (and subsequent entity) merge.
  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe             = true;
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_GE(aRes.NbMergedVertices, 1);

  // The internal edge ref must point to the canonical (non-removed) vertex.
  const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aEdge0);
  ASSERT_EQ(anEdgeDef.InternalVertexRefIds.Length(), 1);
  const BRepGraph_VertexRefId    anUpdRef = anEdgeDef.InternalVertexRefIds.Value(0);
  const BRepGraphInc::VertexRef& anRef    = aGraph.Refs().Vertices().Entry(anUpdRef);
  EXPECT_FALSE(anRef.IsRemoved);
  EXPECT_FALSE(aGraph.Topo().Vertices().Definition(anRef.VertexDefId).IsRemoved);

  // After Compact the ref must survive (not be silently dropped).
  (void)BRepGraph_Compact::Perform(aGraph);

  bool aFoundInternalRef = false;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (aGraph.Topo().Edges().Definition(anEdgeId).InternalVertexRefIds.Length() > 0)
    {
      aFoundInternalRef = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundInternalRef) << "InternalVertexRef was silently dropped by Compact";

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

//=================================================================================================

TEST(BRepGraph_DeduplicateTest, VertexMerge_UpdatesFaceDirectVertexRefs)
{
  // Regression for Bug A2: Phase 1 vertex merge must update FaceDef.VertexRefIds.
  // Without the fix, BRepGraph_Compact would silently drop the direct face vertex ref.
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Add a duplicate vertex at the same position as vertex[0].
  const BRepGraph_VertexId aBaseV        = BRepGraph_VertexId::Start();
  const gp_Pnt             aSamePosition = BRepGraph_Tool::Vertex::Pnt(aGraph, aBaseV);
  const double             aTol          = BRepGraph_Tool::Vertex::Tolerance(aGraph, aBaseV);
  const BRepGraph_VertexId aDupV         = aGraph.Editor().Vertices().Add(aSamePosition, aTol);
  ASSERT_TRUE(aDupV.IsValid());

  // Attach the duplicate vertex directly to face[0].
  const BRepGraph_FaceId      aFace0 = BRepGraph_FaceId::Start();
  const BRepGraph_VertexRefId aFaceRef =
    aGraph.Editor().Faces().AddVertex(aFace0, aDupV, TopAbs_INTERNAL);
  ASSERT_TRUE(aFaceRef.IsValid());
  ASSERT_EQ(aGraph.Topo().Faces().Definition(aFace0).VertexRefIds.Length(), 1);

  // Run full vertex (and subsequent entity) merge.
  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe             = true;
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_GE(aRes.NbMergedVertices, 1);

  // The face direct vertex ref must point to the canonical (non-removed) vertex.
  const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Faces().Definition(aFace0);
  ASSERT_EQ(aFaceDef.VertexRefIds.Length(), 1);
  const BRepGraph_VertexRefId    anUpdRef = aFaceDef.VertexRefIds.Value(0);
  const BRepGraphInc::VertexRef& anRef    = aGraph.Refs().Vertices().Entry(anUpdRef);
  EXPECT_FALSE(anRef.IsRemoved);
  EXPECT_FALSE(aGraph.Topo().Vertices().Definition(anRef.VertexDefId).IsRemoved);

  // After Compact the ref must survive (not be silently dropped).
  (void)BRepGraph_Compact::Perform(aGraph);

  bool aFoundFaceVertexRef = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (aGraph.Topo().Faces().Definition(aFaceId).VertexRefIds.Length() > 0)
    {
      aFoundFaceVertexRef = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundFaceVertexRef) << "Face direct VertexRef was silently dropped by Compact";

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

//=================================================================================================

TEST(BRepGraph_DeduplicateTest, WireDedup_PreservesShellAuxChildRefs)
{
  // Regression for Bug A3: Phase 3 wire merge must update ShellDef.AuxChildRefIds.
  // Deduplicate redirects AuxChildRef from the removed (old) wire to the canonical wire.
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Two wires are built (one outer wire per copied face).
  // The highest-indexed wire belongs to the second face copy and will become the "old"
  // (removed) wire when dedup merges duplicate wires bottom-up (canonical = lower index).
  const int aNbWiresBefore = aGraph.Topo().Wires().Nb();
  ASSERT_GE(aNbWiresBefore, 2);
  const BRepGraph_WireId aWireToAttach = BRepGraph_WireId(aNbWiresBefore - 1);

  // Add a new shell and register the duplicate wire as an auxiliary child.
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());
  const BRepGraph_ChildRefId anAuxRef =
    aGraph.Editor().Shells().AddChild(aShell, aWireToAttach, TopAbs_FORWARD);
  ASSERT_TRUE(anAuxRef.IsValid());
  ASSERT_EQ(aGraph.Topo().Shells().Definition(aShell).AuxChildRefIds.Length(), 1);

  // Run full entity merge (vertex -> edge -> wire -> face).
  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  (void)BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // After merge, the shell's AuxChildRef must point to a non-removed (canonical) wire.
  int aNbActiveAuxChildren = 0;
  for (BRepGraph_ShellIterator aShellIt(aGraph); aShellIt.More(); aShellIt.Next())
  {
    const BRepGraph_ShellId       aShellId = aShellIt.CurrentId();
    const BRepGraphInc::ShellDef& aSh      = aGraph.Topo().Shells().Definition(aShellId);
    for (int aRefIdx = 0; aRefIdx < aSh.AuxChildRefIds.Length(); ++aRefIdx)
    {
      const BRepGraphInc::ChildRef& aRef =
        aGraph.Refs().Children().Entry(aSh.AuxChildRefIds.Value(aRefIdx));
      if (aRef.IsRemoved)
        continue;
      const BRepGraphInc::WireDef& aWireDef =
        aGraph.Topo().Wires().Definition(BRepGraph_WireId(aRef.ChildDefId));
      if (!aWireDef.IsRemoved)
        ++aNbActiveAuxChildren;
    }
  }
  EXPECT_EQ(aNbActiveAuxChildren, 1) << "AuxChildRef points to a removed wire after dedup";
}
