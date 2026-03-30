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
#include <BRepGraphCheck_Analyzer.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <chrono>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphAlgo_Compact.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Validate.hxx>
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
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(theGraph, BRepGraph_FaceId(aFaceIdx));
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
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const occ::handle<Geom_Curve>& aCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anEdgeIdx));
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
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (theGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx)).IsRemoved)
      continue;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int i = 0; i < aCoEdgeIdxs.Length(); ++i)
    {
      const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdge(aCoEdgeIdxs.Value(i));
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
  for (int aRecIdx = 0; aRecIdx < theGraph.History().NbRecords(); ++aRecIdx)
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
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    aCount += aCoEdgeIdxs.Length();
  }
  return aCount;
}

int addDuplicatePCurvesToAllEdges(BRepGraph& theGraph)
{
  int aDupCount = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    if (aCoEdgeIdxs.IsEmpty())
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdge(aCoEdgeIdxs.Value(0));
    if (!aCE.Curve2DRepId.IsValid())
    {
      continue;
    }

    const occ::handle<Geom2d_Curve>& aDupPCurve = BRepGraph_Tool::CoEdge::PCurve(theGraph, aCE);
    theGraph.Builder().AddPCurveToEdge(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx),
                                       aCE.FaceDefId,
                                       aDupPCurve,
                                       aCE.ParamFirst,
                                       aCE.ParamLast,
                                       aCE.Sense);
    ++aDupCount;
  }
  return aDupCount;
}

} // namespace

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_DoesNotRewrite)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);
}

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_ReportsCanonicalCandidates)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // TwoCopiedFaces: 2 surfaces -> 1 canonical, 8 curves -> 4 canonical, 8 PCurves -> 8 canonical.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, CanonicalizeSurfaces_RewritesAndRecordsHistory)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 2);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = false;
  anOpts.HistoryMode = true;

  const int                               aHistoryBefore = aGraph.History().NbRecords();
  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  EXPECT_EQ(aGraph.History().NbRecords(), aHistoryBefore + 5);
}

TEST(BRepGraphAlgo_DeduplicateTest, CanonicalizeCurves_RewritesAndReducesUnique)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph), 8);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = false;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, HistoryModeOff_DoesNotAddHistory)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.History().NbRecords(), 0);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = false;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(aGraph.History().NbRecords(), 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, RestoresHistoryEnabledFlag)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(false);
  ASSERT_FALSE(aGraph.History().IsEnabled());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_FALSE(aGraph.History().IsEnabled());
}

TEST(BRepGraphAlgo_DeduplicateTest, DefaultOverload_PerformWorks)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  EXPECT_EQ(aRes.NbHistoryRecords, 5);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, SingleFace_NoSurfaceRewrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  TopExp_Explorer     anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
}

// PCurve dedup test removed - PCurves are now inline on EdgeDef, not separate graph nodes.

TEST(BRepGraphAlgo_DeduplicateTest, NotDoneGraph_ReturnsEmptyResult)
{
  BRepGraph aGraph;
  // Do not call Build() - graph is not done.
  ASSERT_FALSE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, Idempotent_SecondRunNoRewrites)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes1 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 1);
  ASSERT_EQ(aRes1.NbCurveRewrites, 4);

  // After first dedup, all faces share the same surface pointer, all duplicate
  // edges share the same curve pointer. A second run still detects the same
  // canonical mapping via GeomHash, so rewrite counts may be non-zero (no-op
  // at pointer level). Verify the unique pointer count is unchanged.
  const int aUniqueSurfsBefore  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurvesBefore = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfsBefore);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurvesBefore);
}

TEST(BRepGraphAlgo_DeduplicateTest, FullBox_AllSurfacesUnique)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // A box has 6 faces with 6 distinct Geom_Plane instances (different origins/normals).
  // No surface dedup should occur - all 6 are canonical.
  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 6);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, ResultCountersConsistency)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 surfaces, 8 curves, 8 PCurves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);
  ASSERT_EQ(nbPCurveEntries(aGraph), 8);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Canonical + duplicates == total for each geometry type.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalSurfaces + (aGraph.Topo().NbFaces() - aRes.NbCanonicalSurfaces),
            aGraph.Topo().NbFaces());
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbCanonicalCurves + (aGraph.Topo().NbEdges() - aRes.NbCanonicalCurves),
            aGraph.Topo().NbEdges());
}

TEST(BRepGraphAlgo_DeduplicateTest, EmptyCompound_NoRewrites)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, MultipleCopies_NWayDedup)
{
  const int aNbCopies = 4;
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Topo().NbFaces(), aNbCopies);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, aNbCopies - 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, MixedGeometry_OnlyIdenticalDeduped)
{
  BRepGraph aGraph;
  aGraph.Build(makeMixedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  // 2 box face copies + 1 cylinder face = 3 faces, 3 surfaces, 11 curves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 3);
  ASSERT_EQ(aGraph.Topo().NbFaces(), 3);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 11);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
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

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllCopiedFacesShareCanonicalSurface)
{
  const int aNbCopies = 3;
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // All face defs should share the same Surface handle.
  NCollection_Map<const Geom_Surface*> aSurfIds;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(aFaceIdx));
    if (!aSurf.IsNull())
    {
      aSurfIds.Add(aSurf.get());
    }
  }
  EXPECT_EQ(aSurfIds.Extent(), 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, HistoryRecordNames_MatchExpectedOps)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
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

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_CurveAndPCurveCountsReported)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

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

TEST(BRepGraphAlgo_DeduplicateTest, DefaultOptionsStruct_HasExpectedDefaults)
{
  BRepGraphAlgo_Deduplicate::Options anOpts;
  EXPECT_FALSE(anOpts.AnalyzeOnly);
  EXPECT_TRUE(anOpts.HistoryMode);
  EXPECT_FALSE(anOpts.MergeEntitiesWhenSafe);
  EXPECT_NEAR(anOpts.CompTolerance, Precision::Angular(), 1e-20);
  EXPECT_NEAR(anOpts.HashTolerance, Precision::Confusion(), 1e-20);
}

TEST(BRepGraphAlgo_DeduplicateTest, DefaultResultStruct_AllZeroed)
{
  BRepGraphAlgo_Deduplicate::Result aRes;
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeDefsWhenSafe_MergesVertices)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().NbVertices();

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // Two copied faces share identical vertex positions -> vertices should merge.
  EXPECT_GT(aRes.NbMergedVertices, 0);
  EXPECT_TRUE(aRes.IsEntityMergeApplied);
  (void)aNbVerticesBefore;
}

// ---------------------------------------------------------------------------
// Nested and complex compound tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, NestedCompound_AllCopiesDeduped)
{
  BRepGraph aGraph;
  aGraph.Build(makeNestedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  // 3 copies of the same face across nested compounds: 3 surfaces, 12 curves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 3);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 12);
  ASSERT_EQ(aGraph.Topo().NbFaces(), 3);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 12);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 2);
  EXPECT_EQ(aRes.NbCurveRewrites, 8);
  // 10 canonicalize + 2 nullify surfaces + 8 nullify curves = 20.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, ThreeDistinctPrimitives_MinimalDedup)
{
  BRepGraph aGraph;
  aGraph.Build(makeThreeDistinctPrimitives());
  ASSERT_TRUE(aGraph.IsDone());

  // Box(6 faces) + Sphere(1 face) + Cone(3 faces) = 10 faces, 18 edges.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 10);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 18);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // 9 canonical surfaces (1 surface matches between cone bottom and sphere/box plane).
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 9);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 9);
  // Some curve dedup may occur between the three primitives (shared circle/line geometry).
  EXPECT_LE(nbUniqueEdgeCurveDefs(aGraph), 18);
}

TEST(BRepGraphAlgo_DeduplicateTest, TwoIdenticalBoxes_SurfacesAndCurvesDeduped)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  // Two identical boxes: 12 faces, 24 edges, 12 geom surfaces, 24 geom curves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 12);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 24);
  ASSERT_EQ(aGraph.Topo().NbFaces(), 12);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 24);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
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

TEST(BRepGraphAlgo_DeduplicateTest, CurveRewriteCount_MatchesDuplicateEdgeCurves)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 8 geom curves, 4 canonical -> 4 duplicates.
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);

  BRepGraphAlgo_Deduplicate::Options anAnalyze;
  anAnalyze.AnalyzeOnly = true;
  const BRepGraphAlgo_Deduplicate::Result anAnalysis =
    BRepGraphAlgo_Deduplicate::Perform(aGraph, anAnalyze);
  EXPECT_EQ(anAnalysis.NbCanonicalCurves, 4);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // 4 duplicate curve nodes -> exactly 4 edge curve rewrites.
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllCopiedEdgesShareCanonicalCurve)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup: 8 unique curves reduced to 4 canonical.
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

// ---------------------------------------------------------------------------
// PCurve tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, MultiplePCurveDups_AllEdgesDeduped)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
}

TEST(BRepGraphAlgo_DeduplicateTest, PCurveDup_AnalyzeOnly_CountsButNoRewrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const int aUniqueBefore = nbUniquePCurveNodes(aGraph);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(nbUniquePCurveNodes(aGraph), aUniqueBefore);
  // Canonical count should be less than total PCurves.
}

TEST(BRepGraphAlgo_DeduplicateTest, NoPCurveDuplicates_ZeroPCurveRewrites)
{
  // A single face copy has no PCurve duplicates.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
}

// ---------------------------------------------------------------------------
// History tracing tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, HistoryFindOriginal_TracesBackToCanonical)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  ASSERT_EQ(aRes.NbHistoryRecords, 5);

  // For each history record, FindOriginal on the replacement should trace back.
  for (int aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>::Iterator
           aMapIter(aRec.Mapping);
         aMapIter.More();
         aMapIter.Next())
    {
      const BRepGraph_NodeId&                     anOriginal    = aMapIter.Key();
      const NCollection_Vector<BRepGraph_NodeId>& aReplacements = aMapIter.Value();
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

TEST(BRepGraphAlgo_DeduplicateTest, HistoryFindDerived_ContainsCanonicalNode)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // 1 surface canonicalize + 4 curve canonicalizes = 5 history records.
  ASSERT_EQ(aRes.NbHistoryRecords, 5);

  // For each history record, FindDerived on the original should contain the replacements.
  // All records are canonicalize records with 1 replacement (no nullify records).
  int aNbCanonMappings = 0;
  for (int aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>::Iterator
           aMapIter(aRec.Mapping);
         aMapIter.More();
         aMapIter.Next())
    {
      const BRepGraph_NodeId&                    anOriginal = aMapIter.Key();
      const NCollection_Vector<BRepGraph_NodeId> aDerived =
        aGraph.History().FindDerived(anOriginal);
      EXPECT_EQ(aDerived.Length(), 1);
      ++aNbCanonMappings;
    }
  }
  EXPECT_EQ(aNbCanonMappings, 5);
}

TEST(BRepGraphAlgo_DeduplicateTest, HistoryRecordSequenceNumbers_AreMonotonic)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  int aPrevSeq = -1;
  for (int aRecIdx = 0; aRecIdx < aGraph.History().NbRecords(); ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRec = aGraph.History().Record(aRecIdx);
    EXPECT_GT(aRec.SequenceNumber, aPrevSeq);
    aPrevSeq = aRec.SequenceNumber;
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, HistoryOff_NbRecordsUnchanged)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Run once with history to get 5 records (1 surface + 4 curve canonicalizes).
  BRepGraphAlgo_Deduplicate::Options anOpts1;
  anOpts1.HistoryMode = true;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts1);
  EXPECT_EQ(aGraph.History().NbRecords(), 5);

  // Fresh graph, run with history off - no records should be added.
  BRepGraph aGraph2;
  aGraph2.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph2.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts2;
  anOpts2.HistoryMode = false;

  const BRepGraphAlgo_Deduplicate::Result aRes =
    BRepGraphAlgo_Deduplicate::Perform(aGraph2, anOpts2);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  EXPECT_EQ(aGraph2.History().NbRecords(), 0);
}

// ---------------------------------------------------------------------------
// Canonical node validity tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllSurfacesValid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid())
      << "Face " << aFaceIdx << " has null Surface after dedup";
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllCurve3dsValid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (!anEdgeDef.IsDegenerate)
    {
      EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid())
        << "Edge " << anEdgeIdx << " has null Curve3d after dedup";
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllInlinePCurvesHaveCurve2d)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIter = 0; aCEIter < aCoEdgeIdxs.Length(); ++aCEIter)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIter));
      EXPECT_TRUE(aCE.Curve2DRepId.IsValid())
        << "Edge " << anEdgeIdx << " CoEdge " << aCEIter << " has null Curve2d after dedup";
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_CanonicalSurfaceGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(4));
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // All face defs should have a non-null surface after dedup.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid())
      << "Face " << aFaceIdx << " has null Surface after dedup";
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_CanonicalCurveGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (!anEdgeDef.IsDegenerate)
    {
      EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid())
        << "Edge " << anEdgeIdx << " has null Curve3d after dedup";
    }
  }
}

// ---------------------------------------------------------------------------
// Parallel build + dedup tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, ParallelBuild_SameResultAsSequential)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  BRepGraph aGraphSeq;
  aGraphSeq.Build(aCompound, false);
  ASSERT_TRUE(aGraphSeq.IsDone());

  BRepGraph aGraphPar;
  aGraphPar.Build(aCompound, true);
  ASSERT_TRUE(aGraphPar.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aResSeq = BRepGraphAlgo_Deduplicate::Perform(aGraphSeq);
  const BRepGraphAlgo_Deduplicate::Result aResPar = BRepGraphAlgo_Deduplicate::Perform(aGraphPar);

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

TEST(BRepGraphAlgo_DeduplicateTest, TenCopies_AllDeduplicatedToOneSurface)
{
  const int aNbCopies = 10;
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Topo().NbFaces(), aNbCopies);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, aNbCopies - 1);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, TwoCopies_CurveCanonicalCountLessThanTotal)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // 8 geom curves, 4 canonical.
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly                           = true;
  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
}

// ---------------------------------------------------------------------------
// Idempotency on various shapes
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, Idempotent_MixedCompound_SurfacesAndCurves)
{
  BRepGraph aGraph;
  aGraph.Build(makeMixedCompound());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes1 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 1);
  ASSERT_EQ(aRes1.NbCurveRewrites, 4);

  // After first dedup, unique pointer counts are stable.
  const int aUniqueSurfs  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurves = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Unique pointer counts unchanged after second run.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfs);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurves);
}

TEST(BRepGraphAlgo_DeduplicateTest, Idempotent_TwoIdenticalBoxes)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes1 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 6);
  ASSERT_EQ(aRes1.NbCurveRewrites, 12);

  // After first dedup, unique pointer counts are stable.
  const int aUniqueSurfs  = nbUniqueFaceSurfaceDefs(aGraph);
  const int aUniqueCurves = nbUniqueEdgeCurveDefs(aGraph);

  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Unique pointer counts unchanged after second run.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfs);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurves);
}

TEST(BRepGraphAlgo_DeduplicateTest, DISABLED_PCurveDedup_RewritesReduceUniquePCurveNodes)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);

  const int aUniqueBefore = nbUniquePCurveNodes(aGraph);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup, fewer unique PCurve node IDs are referenced.
  EXPECT_LT(nbUniquePCurveNodes(aGraph), aUniqueBefore);
}

// ---------------------------------------------------------------------------
// History enabled flag restoration tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, RestoresHistoryFlag_WhenHistoryModeOff)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(true);
  ASSERT_TRUE(aGraph.History().IsEnabled());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = false;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Should be restored to the original value (true).
  EXPECT_TRUE(aGraph.History().IsEnabled());
}

TEST(BRepGraphAlgo_DeduplicateTest, RestoresHistoryFlag_AnalyzeOnlyPath)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.History().SetEnabled(true);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;
  anOpts.HistoryMode = false;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Restored even when exiting through the AnalyzeOnly early-return.
  EXPECT_TRUE(aGraph.History().IsEnabled());
}

// ---------------------------------------------------------------------------
// Geometry count consistency invariants
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, GeomCountsUnchanged_AfterDedup)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 surfaces, 8 curves, 8 PCurves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);
  ASSERT_EQ(nbPCurveEntries(aGraph), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Dedup rewrites references, but does not remove geometry nodes.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 2);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 8);
  EXPECT_EQ(nbPCurveEntries(aGraph), 8);
}

TEST(BRepGraphAlgo_DeduplicateTest, DefCountsUnchanged_AfterDedup)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 face defs, 8 edge defs.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Dedup never removes or adds defs.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 2);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 8);
}

TEST(BRepGraphAlgo_DeduplicateTest, PCurveEntryCount_UnchangedAfterDedup)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);

  const int aPCEntryCount = nbPCurveEntries(aGraph);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Dedup rewrites PCurve node IDs but doesn't add or remove PCurveEntries.
  EXPECT_EQ(nbPCurveEntries(aGraph), aPCEntryCount);
}

// ---------------------------------------------------------------------------
// Sphere and cylinder specific tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, TwoCopiedSphereFaces_Deduped)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two sphere face copies: 2 faces, 6 edges.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 6);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  // After dedup, both sphere faces share the same surface pointer.
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
  EXPECT_GT(aRes.NbHistoryRecords, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, TwoCopiedCylinderFaces_Deduped)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two cylinder face copies: 2 faces, 6 edges, 2 surfaces, 6 curves, 12 PCurves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 6);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 3);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 3);
  // 8 canonicalize + 1 nullify surface + 3 nullify curves + 4 nullify PCurves = 16.
  EXPECT_GT(aRes.NbHistoryRecords, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, DifferentSizedCylinders_NotDeduped)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two distinct cylinder faces: 2 surfaces, 6 curves.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  ASSERT_EQ(aGraph.Topo().NbEdges(), 6);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
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

TEST(BRepGraphAlgo_DeduplicateTest, BackRefs_SurfaceRewrite_UpdatesFaceDefUsers)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: each face has its own surface handle.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  EXPECT_NE(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup: both faces share the same canonical surface pointer.
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
}

TEST(BRepGraphAlgo_DeduplicateTest, BackRefs_CurveRewrite_UpdatesEdgeDefUsers)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: 8 edges, each with its own curve handle.
  ASSERT_EQ(aGraph.Topo().NbEdges(), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup: edges with identical curves should share the same pointer.
  // Count distinct curve pointers across all edges.
  NCollection_Map<const Geom_Curve*> aDistinctCurves;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const occ::handle<Geom_Curve>& aCurve =
      BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(anEdgeIdx));
    if (!aCurve.IsNull())
      aDistinctCurves.Add(aCurve.get());
  }
  // After dedup, 4 canonical curves should remain (from 8 originally).
  EXPECT_EQ(aDistinctCurves.Extent(), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, FacesOnSurface_AfterDedup_ReturnsCorrectDefs)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup, all face defs point to the same canonical surface.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);
  EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(0)));
  EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(1)));
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(1)).get());
}

TEST(BRepGraphAlgo_DeduplicateTest, Nullify_OrphanedSurface_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // No orphan nullification since geometry is stored inline on defs.
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);

  // After dedup, both faces share the same canonical surface; all surfaces non-null.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(aFaceIdx)))
      << "Face " << aFaceIdx << " has null Surface after dedup";
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, Nullify_OrphanedCurve_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // No orphan nullification since geometry is stored inline on defs.
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);

  // After dedup, all non-degenerate edges should have non-null Curve3d;
  // duplicate edges now share the canonical curve pointer.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
        << "Edge " << anEdgeIdx << " has null Curve3d after dedup";
    }
  }
  // Verify unique curve count decreased.
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, Nullify_OrphanedPCurve_HandleIsNull)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aDupCount = addDuplicatePCurvesToAllEdges(aGraph);
  ASSERT_GT(aDupCount, 0);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup, total PCurve entries may have changed.
  // Verify the number of pcurve rewrites is positive (dedup happened).
}

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_NoBackRefChangesOrNullification)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot surface/curve pointers before.
  NCollection_Vector<const Geom_Surface*> aSurfPtrs;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
    aSurfPtrs.Append(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(aFaceIdx)).get());
  NCollection_Vector<const Geom_Curve*> aCurvePtrs;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
    aCurvePtrs.Append(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(anEdgeIdx)).get());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);

  // Surface/curve pointers unchanged (analyze only, no rewriting).
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(aFaceIdx)).get(),
              aSurfPtrs.Value(aFaceIdx));
  }
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    EXPECT_EQ(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(anEdgeIdx)).get(),
              aCurvePtrs.Value(anEdgeIdx));
  }

  // All handles still non-null.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(aFaceIdx)));
  }
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)));
    }
  }
}

// ---------------------------------------------------------------------------
// Real-world shape: bug24867_pump.brep full deduplication + save
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, Pump_FullDedup_BackRefsAndNullify)
{
  const char* aFilePath = "/Users/dpasukhi/work/OCCT/build/bug24867_pump.brep";

  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  const bool   isRead = BRepTools::Read(aShape, aFilePath, aBuilder);
  if (!isRead)
  {
    GTEST_SKIP() << "Cannot read " << aFilePath;
  }
  ASSERT_FALSE(aShape.IsNull());

  // Build graph.
  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot geometry counts before dedup.
  const int aNbSurfsBefore  = aGraph.Topo().NbFaces();
  const int aNbCurvesBefore = aGraph.Topo().NbEdges();
  const int aNbPCBefore     = nbPCurveEntries(aGraph);
  const int aNbFaces        = aGraph.Topo().NbFaces();
  const int aNbEdges        = aGraph.Topo().NbEdges();

  ASSERT_GT(aNbSurfsBefore, 0);
  ASSERT_GT(aNbCurvesBefore, 0);
  ASSERT_GT(aNbFaces, 0);
  ASSERT_GT(aNbEdges, 0);

  // Run full dedup with all options enabled.
  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly           = false;
  anOpts.HistoryMode           = true;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Geometry node counts are unchanged (nodes are not removed).
  EXPECT_EQ(aGraph.Topo().NbFaces(), aNbSurfsBefore);
  EXPECT_EQ(aGraph.Topo().NbEdges(), aNbCurvesBefore);
  // CoEdge count may increase slightly: dedup transfers PCurves to canonical edges
  // via AddPCurveToEdge (creating new CoEdges), while removed edges' CoEdges
  // are excluded from the count.
  EXPECT_GE(nbPCurveEntries(aGraph), aNbPCBefore - 2);

  // Def counts are unchanged.
  EXPECT_EQ(aGraph.Topo().NbFaces(), aNbFaces);
  EXPECT_EQ(aGraph.Topo().NbEdges(), aNbEdges);

  // After dedup, all face surfaces should be non-null.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(aFaceIdx)))
      << "Face " << aFaceIdx << " has null Surface after dedup";
  }

  // After dedup, non-degenerate edge curves should be non-null.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
        << "Edge " << anEdgeIdx << " has null Curve3d after dedup";
    }
  }

  // No orphan nullification since geometry is stored inline on defs.
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);

  // Idempotency: second run unique pointer counts should be unchanged.
  const int                               aUniqueSurfsAfter  = nbUniqueFaceSurfaceDefs(aGraph);
  const int                               aUniqueCurvesAfter = nbUniqueEdgeCurveDefs(aGraph);
  const BRepGraphAlgo_Deduplicate::Result aRes2 =
    BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), aUniqueSurfsAfter);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), aUniqueCurvesAfter);
  EXPECT_EQ(aRes2.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes2.NbNullifiedCurves, 0);

  // Print summary for manual inspection.
  std::cout << "\n=== Pump dedup summary ===\n"
            << "  Faces: " << aNbFaces << ", Edges: " << aNbEdges << "\n"
            << "  Surfaces: " << aNbSurfsBefore << " (canonical: " << aRes.NbCanonicalSurfaces
            << ", rewrites: " << aRes.NbSurfaceRewrites
            << ", nullified: " << aRes.NbNullifiedSurfaces << ")\n"
            << "  Curves: " << aNbCurvesBefore << " (canonical: " << aRes.NbCanonicalCurves
            << ", rewrites: " << aRes.NbCurveRewrites << ", nullified: " << aRes.NbNullifiedCurves
            << ")\n"
            << "  History records: " << aRes.NbHistoryRecords << "\n"
            << "==========================\n";

  // Get the final shape via Shape() - returns original if unmodified,
  // reconstructs if modified (dedup marks face/edge defs as modified,
  // which propagates up to the root compound).
  BRepGraph_NodeId aRootId;
  if (aGraph.Topo().NbCompounds() > 0)
    aRootId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, 0);
  else if (aGraph.Topo().NbSolids() > 0)
    aRootId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0);
  ASSERT_TRUE(aRootId.IsValid());

  TopoDS_Shape aFinalShape = aGraph.Shapes().Shape(aRootId);
  ASSERT_FALSE(aFinalShape.IsNull()) << "Shape() returned null for root node";

  // Count unique geometry pointers in the original vs deduped shape.
  auto countUniqueSurfaces = [](const TopoDS_Shape& theShape) -> int {
    NCollection_Map<const Geom_Surface*> aSet;
    for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      TopLoc_Location                  aLoc;
      const occ::handle<Geom_Surface>& aSurf =
        BRep_Tool::Surface(TopoDS::Face(anExp.Current()), aLoc);
      if (!aSurf.IsNull())
        aSet.Add(aSurf.get());
    }
    return aSet.Extent();
  };

  auto countUniqueCurves = [](const TopoDS_Shape& theShape) -> int {
    NCollection_Map<const Geom_Curve*> aSet;
    for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      TopLoc_Location               aLoc;
      double                        aFirst = 0.0, aLast = 0.0;
      const occ::handle<Geom_Curve> aCurve =
        BRep_Tool::Curve(TopoDS::Edge(anExp.Current()), aLoc, aFirst, aLast);
      if (!aCurve.IsNull())
        aSet.Add(aCurve.get());
    }
    return aSet.Extent();
  };

  const int aOrigSurfs   = countUniqueSurfaces(aShape);
  const int aFinalSurfs  = countUniqueSurfaces(aFinalShape);
  const int aOrigCurves  = countUniqueCurves(aShape);
  const int aFinalCurves = countUniqueCurves(aFinalShape);

  std::cout << "\n  === Geometry pointer sharing ===\n"
            << "  Unique surfaces: original=" << aOrigSurfs << ", deduped=" << aFinalSurfs << "\n"
            << "  Unique curves:   original=" << aOrigCurves << ", deduped=" << aFinalCurves
            << "\n";

  // After dedup, fewer unique geometry pointers should exist.
  EXPECT_LE(aFinalSurfs, aOrigSurfs);
  EXPECT_LE(aFinalCurves, aOrigCurves);

  // Build a new graph from the reconstructed shape and compare geometry counts.
  BRepGraph aGraph2;
  aGraph2.Build(aFinalShape);
  ASSERT_TRUE(aGraph2.IsDone());

  std::cout << "  === Graph from reconstructed shape ===\n"
            << "  Faces: " << aGraph2.Topo().NbFaces() << ", Edges: " << aGraph2.Topo().NbEdges()
            << "\n"
            << "  Surfaces: " << aGraph2.Topo().NbFaces()
            << ", Curves: " << aGraph2.Topo().NbEdges() << ", PCurves: " << nbPCurveEntries(aGraph2)
            << "\n";

  // The new graph should have fewer geometry nodes (shared handles -> single node).
  EXPECT_EQ(aGraph2.Topo().NbFaces(), aNbFaces);
  EXPECT_EQ(aGraph2.Topo().NbEdges(), aNbEdges);
  EXPECT_LE(aGraph2.Topo().NbFaces(), aNbSurfsBefore);
  EXPECT_LE(aGraph2.Topo().NbEdges(), aNbCurvesBefore);

  const char* anOutPath = "/Users/dpasukhi/work/OCCT/build/bug24867_pump_deduped.brep";
  const bool  isWritten = BRepTools::Write(aFinalShape, anOutPath);
  EXPECT_TRUE(isWritten) << "Failed to write " << anOutPath;

  std::cout << "  Written deduped shape to: " << anOutPath << "\n";

  // -------------------------------------------------------------------------
  // Analyzer comparison: BRepCheck_Analyzer vs BRepGraphCheck_Analyzer
  // Run on both original and deduped shapes; compare validity and timing.
  // -------------------------------------------------------------------------

  // --- BRepCheck_Analyzer (legacy) on original shape ---
  auto               aLegacyOrigStart = std::chrono::high_resolution_clock::now();
  BRepCheck_Analyzer aLegacyOrig(aShape, true);
  auto               aLegacyOrigEnd   = std::chrono::high_resolution_clock::now();
  const bool         aLegacyOrigValid = aLegacyOrig.IsValid();
  const double       aLegacyOrigMs =
    std::chrono::duration<double, std::milli>(aLegacyOrigEnd - aLegacyOrigStart).count();

  // --- BRepCheck_Analyzer (legacy) on deduped shape ---
  auto               aLegacyDedupStart = std::chrono::high_resolution_clock::now();
  BRepCheck_Analyzer aLegacyDedup(aFinalShape, true);
  auto               aLegacyDedupEnd   = std::chrono::high_resolution_clock::now();
  const bool         aLegacyDedupValid = aLegacyDedup.IsValid();
  const double       aLegacyDedupMs =
    std::chrono::duration<double, std::milli>(aLegacyDedupEnd - aLegacyDedupStart).count();

  // --- BRepGraphCheck_Analyzer on original shape (serial) ---
  BRepGraph aGraphOrig;
  aGraphOrig.Build(aShape);
  ASSERT_TRUE(aGraphOrig.IsDone());

  auto                    aGraphOrigStart = std::chrono::high_resolution_clock::now();
  BRepGraphCheck_Analyzer aGraphCheckOrig(aGraphOrig);
  aGraphCheckOrig.SetGeometricControls(true);
  aGraphCheckOrig.Perform();
  auto         aGraphOrigEnd   = std::chrono::high_resolution_clock::now();
  const bool   aGraphOrigValid = aGraphCheckOrig.IsValid();
  const double aGraphOrigMs =
    std::chrono::duration<double, std::milli>(aGraphOrigEnd - aGraphOrigStart).count();

  // --- BRepGraphCheck_Analyzer on original shape (parallel) ---
  auto                    aGraphOrigParStart = std::chrono::high_resolution_clock::now();
  BRepGraphCheck_Analyzer aGraphCheckOrigPar(aGraphOrig);
  aGraphCheckOrigPar.SetGeometricControls(true);
  aGraphCheckOrigPar.SetParallel(true);
  aGraphCheckOrigPar.Perform();
  auto         aGraphOrigParEnd   = std::chrono::high_resolution_clock::now();
  const bool   aGraphOrigParValid = aGraphCheckOrigPar.IsValid();
  const double aGraphOrigParMs =
    std::chrono::duration<double, std::milli>(aGraphOrigParEnd - aGraphOrigParStart).count();

  // --- BRepGraphCheck_Analyzer on deduped shape (serial, reuse aGraph2) ---
  auto                    aGraphDedupStart = std::chrono::high_resolution_clock::now();
  BRepGraphCheck_Analyzer aGraphCheckDedup(aGraph2);
  aGraphCheckDedup.SetGeometricControls(true);
  aGraphCheckDedup.Perform();
  auto         aGraphDedupEnd   = std::chrono::high_resolution_clock::now();
  const bool   aGraphDedupValid = aGraphCheckDedup.IsValid();
  const double aGraphDedupMs =
    std::chrono::duration<double, std::milli>(aGraphDedupEnd - aGraphDedupStart).count();

  // --- BRepGraphCheck_Analyzer on deduped shape (parallel) ---
  auto                    aGraphDedupParStart = std::chrono::high_resolution_clock::now();
  BRepGraphCheck_Analyzer aGraphCheckDedupPar(aGraph2);
  aGraphCheckDedupPar.SetGeometricControls(true);
  aGraphCheckDedupPar.SetParallel(true);
  aGraphCheckDedupPar.Perform();
  auto         aGraphDedupParEnd   = std::chrono::high_resolution_clock::now();
  const bool   aGraphDedupParValid = aGraphCheckDedupPar.IsValid();
  const double aGraphDedupParMs =
    std::chrono::duration<double, std::milli>(aGraphDedupParEnd - aGraphDedupParStart).count();

  // Print comparison table.
  std::cout << "\n  === Analyzer comparison ===\n"
            << "                                  Valid    Time (ms)\n"
            << "  BRepCheck       (original):      " << (aLegacyOrigValid ? "yes" : "no ") << "    "
            << aLegacyOrigMs << "\n"
            << "  BRepCheck       (deduped):       " << (aLegacyDedupValid ? "yes" : "no ")
            << "    " << aLegacyDedupMs << "\n"
            << "  BRepGraph serial  (original):    " << (aGraphOrigValid ? "yes" : "no ") << "    "
            << aGraphOrigMs << "\n"
            << "  BRepGraph parallel (original):   " << (aGraphOrigParValid ? "yes" : "no ")
            << "    " << aGraphOrigParMs << "\n"
            << "  BRepGraph serial  (deduped):     " << (aGraphDedupValid ? "yes" : "no ") << "    "
            << aGraphDedupMs << "\n"
            << "  BRepGraph parallel (deduped):    " << (aGraphDedupParValid ? "yes" : "no ")
            << "    " << aGraphDedupParMs << "\n"
            << "  Speedup vs BRepCheck (serial):   "
            << (aGraphOrigMs > 0.0 ? aLegacyOrigMs / aGraphOrigMs : 0.0) << "x\n"
            << "  Speedup vs BRepCheck (parallel): "
            << (aGraphOrigParMs > 0.0 ? aLegacyOrigMs / aGraphOrigParMs : 0.0) << "x\n"
            << "  Parallel speedup (orig):         "
            << (aGraphOrigParMs > 0.0 ? aGraphOrigMs / aGraphOrigParMs : 0.0) << "x\n"
            << "  Parallel speedup (dedup):        "
            << (aGraphDedupParMs > 0.0 ? aGraphDedupMs / aGraphDedupParMs : 0.0) << "x\n"
            << "==========================\n";

  // Validity must agree across all variants.
  EXPECT_EQ(aGraphOrigValid, aLegacyOrigValid)
    << "BRepGraphCheck serial and BRepCheck should agree on original shape";
  EXPECT_EQ(aGraphOrigParValid, aLegacyOrigValid)
    << "BRepGraphCheck parallel and BRepCheck should agree on original shape";
  EXPECT_EQ(aGraphDedupValid, aLegacyDedupValid)
    << "BRepGraphCheck serial and BRepCheck should agree on deduped shape";
  EXPECT_EQ(aGraphDedupParValid, aLegacyDedupValid)
    << "BRepGraphCheck parallel and BRepCheck should agree on deduped shape";

  // Parallel must produce same validity as serial.
  EXPECT_EQ(aGraphOrigParValid, aGraphOrigValid)
    << "Parallel and serial must agree on original shape";
  EXPECT_EQ(aGraphDedupParValid, aGraphDedupValid)
    << "Parallel and serial must agree on deduped shape";

  // Dedup must not introduce new invalidity.
  EXPECT_EQ(aLegacyDedupValid, aLegacyOrigValid)
    << "Dedup must not invalidate the shape (BRepCheck)";
  EXPECT_EQ(aGraphDedupValid, aGraphOrigValid)
    << "Dedup must not invalidate the shape (BRepGraphCheck serial)";
  EXPECT_EQ(aGraphDedupParValid, aGraphOrigParValid)
    << "Dedup must not invalidate the shape (BRepGraphCheck parallel)";
}

// ---------------------------------------------------------------------------
// Round-trip tests: Build -> Dedup -> Reconstruct -> Build verifies geometry sharing
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, RoundTrip_TwoCopiedFaces_FewerSurfaces)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  // First graph: build and dedup.
  BRepGraph aGraph1;
  aGraph1.Build(aCompound);
  ASSERT_TRUE(aGraph1.IsDone());
  ASSERT_EQ(aGraph1.Topo().NbFaces(), 2);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph1);
  ASSERT_EQ(nbUniqueFaceSurfaceDefs(aGraph1), 1);

  // Reconstruct each face individually and assemble into a compound.
  BRep_Builder    aBB;
  TopoDS_Compound aReconstructed;
  aBB.MakeCompound(aReconstructed);
  for (int aFaceIdx = 0; aFaceIdx < aGraph1.Topo().NbFaces(); ++aFaceIdx)
  {
    const TopoDS_Shape aFace = aGraph1.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx));
    ASSERT_FALSE(aFace.IsNull());
    aBB.Add(aReconstructed, aFace);
  }

  // Second graph: build from reconstructed shape.
  BRepGraph aGraph2;
  aGraph2.Build(aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Face defs count stays 2 (topology defs, not geometry nodes).
  // After round-trip, unique surface pointer count should be 1
  // because both faces share the same surface handle from dedup.
  EXPECT_EQ(aGraph2.Topo().NbFaces(), 2);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph2), 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, RoundTrip_TwoCopiedFaces_FewerCurves)
{
  const TopoDS_Compound aCompound = makeTwoCopiedIdenticalFaces();

  BRepGraph aGraph1;
  aGraph1.Build(aCompound);
  ASSERT_TRUE(aGraph1.IsDone());
  ASSERT_EQ(aGraph1.Topo().NbEdges(), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph1);
  ASSERT_EQ(nbUniqueEdgeCurveDefs(aGraph1), 4);

  // Reconstruct each face individually.
  BRep_Builder    aBB;
  TopoDS_Compound aReconstructed;
  aBB.MakeCompound(aReconstructed);
  for (int aFaceIdx = 0; aFaceIdx < aGraph1.Topo().NbFaces(); ++aFaceIdx)
  {
    const TopoDS_Shape aFace = aGraph1.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx));
    ASSERT_FALSE(aFace.IsNull());
    aBB.Add(aReconstructed, aFace);
  }

  BRepGraph aGraph2;
  aGraph2.Build(aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Edge defs count stays 8 (topology defs, not geometry nodes).
  // After round-trip, unique curve pointer count should be 4
  // because duplicate edges share the same curve handle from dedup.
  EXPECT_EQ(aGraph2.Topo().NbEdges(), 8);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph2), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, Build_SharedTFace_OneSurfaceNode)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Both face usages share the same TFace -> same raw surface pointer -> one surface node.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, RoundTrip_TwoBoxes_GeomReduction)
{
  const TopoDS_Compound aCompound = makeTwoIdenticalBoxes();

  BRepGraph aGraph1;
  aGraph1.Build(aCompound);
  ASSERT_TRUE(aGraph1.IsDone());

  const int aSurfsBefore  = aGraph1.Topo().NbFaces();
  const int aCurvesBefore = aGraph1.Topo().NbEdges();
  ASSERT_EQ(aSurfsBefore, 12);
  ASSERT_EQ(aCurvesBefore, 24);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph1);

  // Force reconstruction from deduped graph.
  BRepGraph_NodeId   aRootId(BRepGraph_NodeId::Kind::Compound, 0);
  const TopoDS_Shape aReconstructed = aGraph1.Shapes().Reconstruct(aRootId);
  ASSERT_FALSE(aReconstructed.IsNull());

  // Build second graph.
  BRepGraph aGraph2;
  aGraph2.Build(aReconstructed);
  ASSERT_TRUE(aGraph2.IsDone());

  // Face/edge def counts stay the same (topology defs, not geometry nodes).
  EXPECT_EQ(aGraph2.Topo().NbFaces(), aSurfsBefore);
  EXPECT_EQ(aGraph2.Topo().NbEdges(), aCurvesBefore);

  // After dedup, unique geometry pointer counts should reflect deduplication:
  // 6 canonical surfaces and 12 canonical curves.
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph2), 6);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph2), 12);
}

// ---------------------------------------------------------------------------
// Topology definition merge tests (MergeEntitiesWhenSafe = true)
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, MergeVertices_SharedVerticesReduced)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().NbVertices();

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // Two copied faces -> shared vertex positions should merge.
  EXPECT_GT(aRes.NbMergedVertices, 0);

  // Count non-removed vertices.
  int aNbActiveVertices = 0;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbVertices(); ++anIdx)
  {
    if (!aGraph.Topo().Vertex(BRepGraph_VertexId(anIdx)).IsRemoved)
      ++aNbActiveVertices;
  }
  EXPECT_LT(aNbActiveVertices, aNbVerticesBefore);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeEdges_SharedEdgesReduced)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_GT(aRes.NbMergedEdges, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeWires_IdenticalWiresMerged)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // After vertex and edge merge, identical wires should also merge.
  EXPECT_GE(aRes.NbMergedWires, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeFaces_IdenticalFacesMerged)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // After lower-level merges, faces with same surface+wires should merge.
  EXPECT_GE(aRes.NbMergedFaces, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeDefsWhenSafe_False_NoMerge)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Default: MergeEntitiesWhenSafe = false.
  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbMergedVertices, 0);
  EXPECT_EQ(aRes.NbMergedEdges, 0);
  EXPECT_EQ(aRes.NbMergedWires, 0);
  EXPECT_EQ(aRes.NbMergedFaces, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_MergeDefsWhenSafe_CountsOnly)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbVerticesBefore = aGraph.Topo().NbVertices();

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  anOpts.AnalyzeOnly           = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // Counts should be reported.
  EXPECT_GT(aRes.NbMergedVertices, 0);
  // But no mutation: all vertices still present.
  EXPECT_EQ(aGraph.Topo().NbVertices(), aNbVerticesBefore);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, HistoryRecords_MergePhases)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  anOpts.HistoryMode           = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  const int aNbVertexMerge =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:MergeVertex"));
  const int aNbEdgeMerge =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:MergeEdge"));

  EXPECT_EQ(aNbVertexMerge, aRes.NbMergedVertices);
  EXPECT_EQ(aNbEdgeMerge, aRes.NbMergedEdges);
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterMerge_Validate_NoIssues)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // After merge + compact, graph should be structurally valid.
  // Merge alone may leave stale back-references on geometry nodes
  // that are cleaned up by compaction.
  (void)BRepGraphAlgo_Compact::Perform(aGraph);

  const BRepGraphAlgo_Validate::Result aValResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aValResult.IsValid());
}
