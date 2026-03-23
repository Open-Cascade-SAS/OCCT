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
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_PCurveContext.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
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
  NCollection_Map<int> aSurfSet;
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(aFaceIdx);
    if (aFaceDef.SurfNodeId.IsValid())
    {
      aSurfSet.Add(aFaceDef.SurfNodeId.Index);
    }
  }
  return aSurfSet.Extent();
}

//=================================================================================================

int nbUniqueEdgeCurveDefs(const BRepGraph& theGraph)
{
  NCollection_Map<int> aCurveSet;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
    {
      aCurveSet.Add(anEdgeDef.CurveNodeId.Index);
    }
  }
  return aCurveSet.Extent();
}

//=================================================================================================

int nbPCurveEntries(const BRepGraph& theGraph)
{
  int aCount = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    aCount += theGraph.Defs().Edge(anEdgeIdx).PCurves.Length();
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
  NCollection_Map<int> aPCurveSet;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
    for (int aPCIter = 0; aPCIter < anEdgeDef.PCurves.Length(); ++aPCIter)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(aPCIter);
      if (aPCEntry.PCurveNodeId.IsValid())
      {
        aPCurveSet.Add(aPCEntry.PCurveNodeId.Index);
      }
    }
  }
  return aPCurveSet.Extent();
}

int addDuplicatePCurvesToAllEdges(BRepGraph& theGraph)
{
  int aDupCount = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.PCurves.IsEmpty())
    {
      continue;
    }

    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(0);
    const BRepGraph_GeomNode::PCurve& aPCNode = theGraph.Geom().PCurve(aPCEntry.PCurveNodeId.Index);
    if (aPCNode.Curve2d.IsNull())
    {
      continue;
    }

    theGraph.Mut().AddPCurveToEdge(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx),
                                   aPCEntry.FaceDefId,
                                   aPCNode.Curve2d,
                                   aPCNode.ParamFirst,
                                   aPCNode.ParamLast,
                                   aPCEntry.EdgeOrientation);
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
  // TwoCopiedFaces: 2 surfaces → 1 canonical, 8 curves → 4 canonical, 8 PCurves → 8 canonical.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbCanonicalPCurves, 8);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
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
  // 5 canonicalize + 1 nullify surface + 4 nullify curves = 10.
  EXPECT_EQ(aGraph.History().NbRecords(), aHistoryBefore + 10);
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

  aGraph.SetHistoryEnabled(false);
  ASSERT_FALSE(aGraph.IsHistoryEnabled());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = true;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_FALSE(aGraph.IsHistoryEnabled());
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
  // 5 canonicalize + 1 nullify surface + 4 nullify curves = 10.
  EXPECT_EQ(aRes.NbHistoryRecords, 10);
  EXPECT_FALSE(aRes.IsDefMergeApplied);
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

TEST(BRepGraphAlgo_DeduplicateTest, CanonicalizePCurves_ContextStrict)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  bool aDuplicated = false;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.PCurves.IsEmpty())
    {
      continue;
    }

    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(0);
    const BRepGraph_GeomNode::PCurve& aPCNode = aGraph.Geom().PCurve(aPCEntry.PCurveNodeId.Index);
    if (aPCNode.Curve2d.IsNull())
    {
      continue;
    }

    aGraph.Mut().AddPCurveToEdge(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx),
                                 aPCEntry.FaceDefId,
                                 aPCNode.Curve2d,
                                 aPCNode.ParamFirst,
                                 aPCNode.ParamLast,
                                 aPCEntry.EdgeOrientation);
    aDuplicated = true;
    break;
  }

  ASSERT_TRUE(aDuplicated);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = false;

  const int aBeforePCurves = nbPCurveEntries(aGraph);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbPCurveRewrites, 1);
  EXPECT_EQ(nbPCurveEntries(aGraph), aBeforePCurves);
  EXPECT_EQ(countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizePCurve")),
            1);
}

TEST(BRepGraphAlgo_DeduplicateTest, NotDoneGraph_ReturnsEmptyResult)
{
  BRepGraph aGraph;
  // Do not call Build() — graph is not done.
  ASSERT_FALSE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbCanonicalPCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbNullifiedPCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsDefMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, Idempotent_SecondRunNoRewrites)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes1 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 1);
  ASSERT_EQ(aRes1.NbCurveRewrites, 4);

  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes2.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes2.NbCurveRewrites, 0);
  EXPECT_EQ(aRes2.NbPCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, FullBox_AllSurfacesUnique)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // A box has 6 faces with 6 distinct Geom_Plane instances (different origins/normals).
  // No surface dedup should occur — all 6 are canonical.
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
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 8);
  ASSERT_EQ(aGraph.Geom().NbPCurves(), 8);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Canonical + duplicates == total for each geometry type.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalSurfaces + (aGraph.Geom().NbSurfaces() - aRes.NbCanonicalSurfaces),
            aGraph.Geom().NbSurfaces());
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbCanonicalCurves + (aGraph.Geom().NbCurves() - aRes.NbCanonicalCurves),
            aGraph.Geom().NbCurves());
  EXPECT_EQ(aRes.NbCanonicalPCurves, 8);
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
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, MultipleCopies_NWayDedup)
{
  const int aNbCopies = 4;
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(aNbCopies));
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Defs().NbFaces(), aNbCopies);

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
  ASSERT_EQ(aGraph.Defs().NbFaces(), 3);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 3);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 11);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // Only the two identical box faces should be deduped; cylinder face is untouched.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 2);
  EXPECT_EQ(aRes.NbCanonicalCurves, 7);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 4);
  // 7 canonicalize + 1 nullify surface + 4 nullify curves + 2 nullify PCurves = 14.
  EXPECT_EQ(aRes.NbHistoryRecords, 14);
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

  // All face defs should point to the same SurfNodeId.
  NCollection_Map<int> aSurfIds;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(aFaceIdx);
    if (aFaceDef.SurfNodeId.IsValid())
    {
      aSurfIds.Add(aFaceDef.SurfNodeId.Index);
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
  ASSERT_EQ(aRes.NbHistoryRecords, 10);

  // TwoCopiedFaces: 1 surface + 4 curve canonicalizations + 1 nullify surface + 4 nullify curves.
  const int aNbSurfOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizeSurface"));
  const int aNbCurveOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizeCurve"));
  const int aNbPCurveOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:CanonicalizePCurve"));
  const int aNbNullSurfOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:NullifyOrphanSurface"));
  const int aNbNullCurveOps =
    countHistoryRecordsByOp(aGraph, TCollection_AsciiString("Dedup:NullifyOrphanCurve"));

  EXPECT_EQ(aNbSurfOps, 1);
  EXPECT_EQ(aNbCurveOps, 4);
  EXPECT_EQ(aNbPCurveOps, 0);
  EXPECT_EQ(aNbNullSurfOps, 1);
  EXPECT_EQ(aNbNullCurveOps, 4);
  EXPECT_EQ(aNbSurfOps + aNbCurveOps + aNbPCurveOps + aNbNullSurfOps + aNbNullCurveOps, 10);
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
  EXPECT_EQ(aRes.NbCanonicalPCurves, 8);

  // But no rewrites.
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
}

// ---------------------------------------------------------------------------
// Tolerance and Options tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, DefaultOptionsStruct_HasExpectedDefaults)
{
  BRepGraphAlgo_Deduplicate::Options anOpts;
  EXPECT_FALSE(anOpts.AnalyzeOnly);
  EXPECT_TRUE(anOpts.HistoryMode);
  EXPECT_FALSE(anOpts.MergeDefsWhenSafe);
  EXPECT_NEAR(anOpts.CompTolerance, Precision::Angular(), 1e-20);
  EXPECT_NEAR(anOpts.HashTolerance, Precision::Confusion(), 1e-20);
}

TEST(BRepGraphAlgo_DeduplicateTest, DefaultResultStruct_AllZeroed)
{
  BRepGraphAlgo_Deduplicate::Result aRes;
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbCanonicalPCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbNullifiedPCurves, 0);
  EXPECT_EQ(aRes.NbHistoryRecords, 0);
  EXPECT_FALSE(aRes.IsDefMergeApplied);
}

TEST(BRepGraphAlgo_DeduplicateTest, MergeDefsWhenSafe_NotAppliedYet)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.MergeDefsWhenSafe = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  // MergeDefsWhenSafe is reserved — should never report true.
  EXPECT_FALSE(aRes.IsDefMergeApplied);
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
  ASSERT_EQ(aGraph.Defs().NbFaces(), 3);
  ASSERT_EQ(aGraph.Defs().NbEdges(), 12);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 3);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 12);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 4);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 2);
  EXPECT_EQ(aRes.NbCurveRewrites, 8);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
  // 10 canonicalize + 2 nullify surfaces + 8 nullify curves = 20.
  EXPECT_EQ(aRes.NbHistoryRecords, 20);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 1);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, ThreeDistinctPrimitives_MinimalDedup)
{
  BRepGraph aGraph;
  aGraph.Build(makeThreeDistinctPrimitives());
  ASSERT_TRUE(aGraph.IsDone());

  // Box(6 faces) + Sphere(1 face) + Cone(3 faces) = 10 faces, 10 surfaces, 16 curves.
  ASSERT_EQ(aGraph.Defs().NbFaces(), 10);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 10);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 16);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // 9 canonical surfaces (1 surface matches between cone bottom and sphere/box plane).
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 9);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_EQ(nbUniqueFaceSurfaceDefs(aGraph), 9);
  EXPECT_EQ(nbUniqueEdgeCurveDefs(aGraph), 16);
}

TEST(BRepGraphAlgo_DeduplicateTest, TwoIdenticalBoxes_SurfacesAndCurvesDeduped)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  // Two identical boxes: 12 faces, 24 edges, 12 geom surfaces, 24 geom curves.
  ASSERT_EQ(aGraph.Defs().NbFaces(), 12);
  ASSERT_EQ(aGraph.Defs().NbEdges(), 24);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 12);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 24);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 6);
  EXPECT_EQ(aRes.NbCanonicalCurves, 12);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 6);
  EXPECT_EQ(aRes.NbCurveRewrites, 12);
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
  // 18 canonicalize + 6 nullify surfaces + 12 nullify curves = 36.
  EXPECT_EQ(aRes.NbHistoryRecords, 36);
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

  // TwoCopiedFaces: 8 geom curves, 4 canonical → 4 duplicates.
  ASSERT_EQ(aGraph.Geom().NbCurves(), 8);

  BRepGraphAlgo_Deduplicate::Options anAnalyze;
  anAnalyze.AnalyzeOnly = true;
  const BRepGraphAlgo_Deduplicate::Result anAnalysis =
    BRepGraphAlgo_Deduplicate::Perform(aGraph, anAnalyze);
  EXPECT_EQ(anAnalysis.NbCanonicalCurves, 4);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // 4 duplicate curve nodes → exactly 4 edge curve rewrites.
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
  EXPECT_EQ(aRes.NbPCurveRewrites, aDupCount);
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
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
  EXPECT_EQ(nbUniquePCurveNodes(aGraph), aUniqueBefore);
  // Canonical count should be less than total PCurves.
  EXPECT_LT(aRes.NbCanonicalPCurves, aGraph.Geom().NbPCurves());
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
  EXPECT_EQ(aRes.NbPCurveRewrites, 0);
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
  ASSERT_EQ(aRes.NbHistoryRecords, 10);

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
        // FindOriginal should eventually reach a root — the canonical node.
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
  ASSERT_EQ(aRes.NbHistoryRecords, 10);

  // For each history record, FindDerived on the original should contain the replacements.
  // Canonicalize records have 1 replacement; nullify records have 0.
  int aNbCanonMappings = 0;
  int aNbNullifyMappings = 0;
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
      if (aMapIter.Value().IsEmpty())
      {
        // Nullify record — no derived nodes.
        ++aNbNullifyMappings;
      }
      else
      {
        EXPECT_EQ(aDerived.Length(), 1);
        ++aNbCanonMappings;
      }
    }
  }
  EXPECT_EQ(aNbCanonMappings, 5);
  EXPECT_EQ(aNbNullifyMappings, 5);
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

  // Run once with history to get 5 records.
  BRepGraphAlgo_Deduplicate::Options anOpts1;
  anOpts1.HistoryMode = true;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts1);
  EXPECT_EQ(aGraph.History().NbRecords(), 10);

  // Fresh graph, run with history off — no records should be added.
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

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllSurfNodeIdsValid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(aFaceIdx);
    if (aFaceDef.SurfNodeId.IsValid())
    {
      EXPECT_GE(aFaceDef.SurfNodeId.Index, 0);
      EXPECT_LT(aFaceDef.SurfNodeId.Index, aGraph.Geom().NbSurfaces());
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllCurveNodeIdsValid)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
    {
      EXPECT_GE(anEdgeDef.CurveNodeId.Index, 0);
      EXPECT_LT(anEdgeDef.CurveNodeId.Index, aGraph.Geom().NbCurves());
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_AllPCurveNodeIdsValid)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  BRepBuilderAPI_Copy aCopy(anExp.Current(), true);

  BRepGraph aGraph;
  aGraph.Build(aCopy.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)addDuplicatePCurvesToAllEdges(aGraph);
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    for (int aPCIter = 0; aPCIter < anEdgeDef.PCurves.Length(); ++aPCIter)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(aPCIter);
      if (aPCEntry.PCurveNodeId.IsValid())
      {
        EXPECT_GE(aPCEntry.PCurveNodeId.Index, 0);
        EXPECT_LT(aPCEntry.PCurveNodeId.Index, aGraph.Geom().NbPCurves());
      }
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_CanonicalSurfaceGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeNCopiedIdenticalFaces(4));
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // All face defs should point to a surface with non-null geometry.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(aFaceIdx);
    if (aFaceDef.SurfNodeId.IsValid())
    {
      const BRepGraph_GeomNode::Surf& aSurfNode = aGraph.Geom().Surface(aFaceDef.SurfNodeId.Index);
      EXPECT_FALSE(aSurfNode.Surface.IsNull());
    }
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, AfterDedup_CanonicalCurveGeomNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
    {
      const BRepGraph_GeomNode::Curve& aCurveNode =
        aGraph.Geom().Curve(anEdgeDef.CurveNodeId.Index);
      EXPECT_FALSE(aCurveNode.CurveGeom.IsNull());
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

  ASSERT_EQ(aGraph.Defs().NbFaces(), aNbCopies);

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
  ASSERT_EQ(aGraph.Geom().NbCurves(), 8);

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

  // Surface and curve dedup is idempotent.
  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes2.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes2.NbCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, Idempotent_TwoIdenticalBoxes)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoIdenticalBoxes());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes1 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  ASSERT_EQ(aRes1.NbSurfaceRewrites, 6);
  ASSERT_EQ(aRes1.NbCurveRewrites, 12);

  const BRepGraphAlgo_Deduplicate::Result aRes2 = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes2.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes2.NbCurveRewrites, 0);
}

TEST(BRepGraphAlgo_DeduplicateTest, PCurveDedup_RewritesReduceUniquePCurveNodes)
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
  ASSERT_GT(aRes.NbPCurveRewrites, 0);

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

  aGraph.SetHistoryEnabled(true);
  ASSERT_TRUE(aGraph.IsHistoryEnabled());

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.HistoryMode = false;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Should be restored to the original value (true).
  EXPECT_TRUE(aGraph.IsHistoryEnabled());
}

TEST(BRepGraphAlgo_DeduplicateTest, RestoresHistoryFlag_AnalyzeOnlyPath)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  aGraph.SetHistoryEnabled(true);

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;
  anOpts.HistoryMode = false;
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);

  // Restored even when exiting through the AnalyzeOnly early-return.
  EXPECT_TRUE(aGraph.IsHistoryEnabled());
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
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 8);
  ASSERT_EQ(aGraph.Geom().NbPCurves(), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Dedup rewrites references, but does not remove geometry nodes.
  EXPECT_EQ(aGraph.Geom().NbSurfaces(), 2);
  EXPECT_EQ(aGraph.Geom().NbCurves(), 8);
  EXPECT_EQ(aGraph.Geom().NbPCurves(), 8);
}

TEST(BRepGraphAlgo_DeduplicateTest, DefCountsUnchanged_AfterDedup)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // TwoCopiedFaces: 2 face defs, 8 edge defs.
  ASSERT_EQ(aGraph.Defs().NbFaces(), 2);
  ASSERT_EQ(aGraph.Defs().NbEdges(), 8);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Dedup never removes or adds defs.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 2);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 8);
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

  // Two sphere face copies: 2 faces, 6 edges, 2 surfaces, 2 curves, 12 PCurves.
  ASSERT_EQ(aGraph.Defs().NbFaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 2);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 1);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 1);
  EXPECT_EQ(aRes.NbPCurveRewrites, 4);
  // 6 canonicalize + 1 nullify surface + 1 nullify curve + 4 nullify PCurves = 12.
  EXPECT_EQ(aRes.NbHistoryRecords, 12);
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
  ASSERT_EQ(aGraph.Defs().NbFaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 6);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aRes.NbCanonicalCurves, 3);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 3);
  EXPECT_EQ(aRes.NbPCurveRewrites, 4);
  // 8 canonicalize + 1 nullify surface + 3 nullify curves + 4 nullify PCurves = 16.
  EXPECT_EQ(aRes.NbHistoryRecords, 16);
}

TEST(BRepGraphAlgo_DeduplicateTest, PCurveContext_WorksAsDataMapKey)
{
  // Default-constructed context has negative indices.
  const BRepGraph_PCurveContext aDefault;
  EXPECT_EQ(aDefault.EdgeDefIndex, -1);
  EXPECT_EQ(aDefault.FaceDefIndex, -1);

  // Two contexts differing only by orientation are distinct keys.
  const BRepGraph_PCurveContext aCtxFwd(0, 1, TopAbs_FORWARD);
  const BRepGraph_PCurveContext aCtxRev(0, 1, TopAbs_REVERSED);
  EXPECT_NE(aCtxFwd, aCtxRev);

  // Same context hashes/compares equal and works as NCollection_DataMap key.
  const BRepGraph_PCurveContext aCtxFwd2(0, 1, TopAbs_FORWARD);
  EXPECT_EQ(aCtxFwd, aCtxFwd2);

  NCollection_DataMap<BRepGraph_PCurveContext, int> aMap;
  aMap.Bind(aCtxFwd, 10);
  aMap.Bind(aCtxRev, 20);

  ASSERT_TRUE(aMap.IsBound(aCtxFwd));
  ASSERT_TRUE(aMap.IsBound(aCtxRev));
  EXPECT_EQ(aMap.Find(aCtxFwd), 10);
  EXPECT_EQ(aMap.Find(aCtxRev), 20);

  // Lookup with equivalent key returns the same value.
  EXPECT_EQ(aMap.Find(aCtxFwd2), 10);
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
  ASSERT_EQ(aGraph.Defs().NbFaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().NbCurves(), 6);

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  // Different radii → different cylindrical surfaces → no surface/curve dedup.
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 2);
  EXPECT_EQ(aRes.NbCanonicalCurves, 6);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  // PCurve dedup still happens within context groups.
  EXPECT_EQ(aRes.NbPCurveRewrites, 4);
  // 4 canonicalize PCurves + 4 nullify PCurves = 8.
  EXPECT_EQ(aRes.NbHistoryRecords, 8);
}

// ---------------------------------------------------------------------------
// Back-reference and orphan nullification tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_DeduplicateTest, BackRefs_SurfaceRewrite_UpdatesFaceDefUsers)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: each surface has exactly 1 FaceDefUser.
  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 2);
  ASSERT_EQ(aGraph.Geom().Surface(0).FaceDefUsers.Length(), 1);
  ASSERT_EQ(aGraph.Geom().Surface(1).FaceDefUsers.Length(), 1);

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup: canonical surface has 2 users, orphan surface has 0.
  // Find which is canonical (the one with users).
  int aCanonIdx = -1;
  int anOrphanIdx = -1;
  for (int aSurfIdx = 0; aSurfIdx < aGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    if (aGraph.Geom().Surface(aSurfIdx).FaceDefUsers.Length() == 2)
      aCanonIdx = aSurfIdx;
    else if (aGraph.Geom().Surface(aSurfIdx).FaceDefUsers.IsEmpty())
      anOrphanIdx = aSurfIdx;
  }
  EXPECT_GE(aCanonIdx, 0);
  EXPECT_GE(anOrphanIdx, 0);
  EXPECT_NE(aCanonIdx, anOrphanIdx);
}

TEST(BRepGraphAlgo_DeduplicateTest, BackRefs_CurveRewrite_UpdatesEdgeDefUsers)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Before dedup: 8 curves, each with 1 EdgeDefUser.
  ASSERT_EQ(aGraph.Geom().NbCurves(), 8);
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    ASSERT_EQ(aGraph.Geom().Curve(aCurveIdx).EdgeDefUsers.Length(), 1);
  }

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup: 4 canonical curves should have 2 users each, 4 orphans have 0.
  int aNbCanonical = 0;
  int aNbOrphan = 0;
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    const int aNbUsers = aGraph.Geom().Curve(aCurveIdx).EdgeDefUsers.Length();
    if (aNbUsers == 2)
      ++aNbCanonical;
    else if (aNbUsers == 0)
      ++aNbOrphan;
  }
  EXPECT_EQ(aNbCanonical, 4);
  EXPECT_EQ(aNbOrphan, 4);
}

TEST(BRepGraphAlgo_DeduplicateTest, FacesOnSurface_AfterDedup_ReturnsCorrectDefs)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // After dedup, all face defs point to one canonical surface.
  // FacesOnSurface for the canonical should return both face defs.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(aFaceIdx);
    if (!aFaceDef.SurfNodeId.IsValid())
    {
      continue;
    }
    const NCollection_Vector<BRepGraph_NodeId>& aFacesOnSurf =
      aGraph.Geom().FacesOnSurface(aFaceDef.SurfNodeId);
    EXPECT_EQ(aFacesOnSurf.Length(), 2);
  }
}

TEST(BRepGraphAlgo_DeduplicateTest, Nullify_OrphanedSurface_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 1);

  // Find the orphaned surface and verify its handle is null.
  int aNbNullSurfs = 0;
  int aNbLiveSurfs = 0;
  for (int aSurfIdx = 0; aSurfIdx < aGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurf = aGraph.Geom().Surface(aSurfIdx);
    if (aSurf.Surface.IsNull())
      ++aNbNullSurfs;
    else
      ++aNbLiveSurfs;
  }
  EXPECT_EQ(aNbNullSurfs, 1);
  EXPECT_EQ(aNbLiveSurfs, 1);
}

TEST(BRepGraphAlgo_DeduplicateTest, Nullify_OrphanedCurve_HandleIsNull)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbNullifiedCurves, 4);

  // Canonical curves should be non-null, orphaned curves should be null.
  int aNbNullCurves = 0;
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    if (aGraph.Geom().Curve(aCurveIdx).CurveGeom.IsNull())
      ++aNbNullCurves;
  }
  EXPECT_EQ(aNbNullCurves, 4);
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

  const int aTotalPCurves = aGraph.Geom().NbPCurves();

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_GT(aRes.NbPCurveRewrites, 0);
  EXPECT_GT(aRes.NbNullifiedPCurves, 0);

  // Orphaned PCurve handles should be null.
  int aNbNullPCurves = 0;
  for (int aPCIdx = 0; aPCIdx < aTotalPCurves; ++aPCIdx)
  {
    if (aGraph.Geom().PCurve(aPCIdx).Curve2d.IsNull())
      ++aNbNullPCurves;
  }
  EXPECT_EQ(aNbNullPCurves, aRes.NbNullifiedPCurves);
}

TEST(BRepGraphAlgo_DeduplicateTest, AnalyzeOnly_NoBackRefChangesOrNullification)
{
  BRepGraph aGraph;
  aGraph.Build(makeTwoCopiedIdenticalFaces());
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot back-refs before.
  NCollection_Vector<int> aSurfUserCounts;
  for (int aSurfIdx = 0; aSurfIdx < aGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    aSurfUserCounts.Append(aGraph.Geom().Surface(aSurfIdx).FaceDefUsers.Length());
  }
  NCollection_Vector<int> aCurveUserCounts;
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    aCurveUserCounts.Append(aGraph.Geom().Curve(aCurveIdx).EdgeDefUsers.Length());
  }

  BRepGraphAlgo_Deduplicate::Options anOpts;
  anOpts.AnalyzeOnly = true;

  const BRepGraphAlgo_Deduplicate::Result aRes = BRepGraphAlgo_Deduplicate::Perform(aGraph, anOpts);
  EXPECT_EQ(aRes.NbNullifiedSurfaces, 0);
  EXPECT_EQ(aRes.NbNullifiedCurves, 0);
  EXPECT_EQ(aRes.NbNullifiedPCurves, 0);

  // Back-refs unchanged.
  for (int aSurfIdx = 0; aSurfIdx < aGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    EXPECT_EQ(aGraph.Geom().Surface(aSurfIdx).FaceDefUsers.Length(),
              aSurfUserCounts.Value(aSurfIdx));
  }
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    EXPECT_EQ(aGraph.Geom().Curve(aCurveIdx).EdgeDefUsers.Length(),
              aCurveUserCounts.Value(aCurveIdx));
  }

  // All handles still non-null.
  for (int aSurfIdx = 0; aSurfIdx < aGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    EXPECT_FALSE(aGraph.Geom().Surface(aSurfIdx).Surface.IsNull());
  }
  for (int aCurveIdx = 0; aCurveIdx < aGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    EXPECT_FALSE(aGraph.Geom().Curve(aCurveIdx).CurveGeom.IsNull());
  }
}
