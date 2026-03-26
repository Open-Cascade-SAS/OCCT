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
#include <BRepGraph_History.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <BRepGraphAlgo_SameParameter.hxx>
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

      if (aTemplateGraph.IsDone() && aTemplateGraph.Topo().NbFaces() > 0)
      {
        BRepGraph aTransGraph =
          BRepGraphAlgo_Transform::TransformFace(aTemplateGraph, BRepGraph_FaceId(0), aTrsf, true);
        if (aTransGraph.IsDone())
        {
          aFaces.Append(aTransGraph.Shapes().ReconstructFace(BRepGraph_FaceId(0)));
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
// ===================================================================
// Performance: BRepGraphAlgo_Sewing vs BRepBuilderAPI_Sewing
// ===================================================================

namespace
{

constexpr int THE_NB_PERF_ITERS = 10;

double runGraphSewing(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                      bool                                      theParallel,
                      int&                                      theNbSewn)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
    aBB.Add(aCompound, theFaces.Value(anIdx));

  auto aStart = std::chrono::steady_clock::now();

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Parallel  = theParallel;

  BRepGraph aGraph;
  aGraph.Build(aCompound, theParallel);

  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

  auto aEnd = std::chrono::steady_clock::now();
  theNbSewn = aResult.IsDone ? aResult.NbSewnEdges : 0;
  return std::chrono::duration<double>(aEnd - aStart).count();
}

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

using FaceListBuilder = std::function<NCollection_Sequence<TopoDS_Shape>()>;

void runBenchmark(const char*            theLabel,
                  int                    theNbFaces,
                  const FaceListBuilder& theBuildFaces,
                  int                    theExpSewnSeq,
                  int                    theExpSewnPar)
{
  // Warm-up.
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
    -300);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid20x20)
{
  runBenchmark("20x20 grid", 400, []() { return buildFaceGrid(20, 20, 1.0, 1.0); }, 760, 760);
}

// ===================================================================
// Profiling-oriented: 50 iterations of large workloads.
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

    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aBB.Add(aCompound, aFaces.Value(anIdx));

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = false;

    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aResult.IsDone);
      aNbSewn = aResult.NbSewnEdges;
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

    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aBB.Add(aCompound, aFaces.Value(anIdx));

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = true;

    BRepGraph aGraph;
    aGraph.Build(aCompound, true);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aResult.IsDone);
      aNbSewn = aResult.NbSewnEdges;
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

    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aBB.Add(aCompound, aFaces.Value(anIdx));

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance   = 1.0e-04;
    aOpts.Parallel    = false;
    aOpts.HistoryMode = false;

    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aResult.IsDone);
      aNbSewn = aResult.NbSewnEdges;
      EXPECT_EQ(aGraph.History().NbRecords(), 0);
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

    auto aTimerStart = std::chrono::steady_clock::now();

    BRep_Builder    aBB;
    TopoDS_Compound aCompound;
    aBB.MakeCompound(aCompound);
    for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
      aBB.Add(aCompound, aFaces.Value(anIdx));

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = true;

    BRepGraph aGraph;
    aGraph.Build(aCompound, true);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

    auto aTimerEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(aTimerEnd - aTimerStart).count();
    if (anIter == 0)
    {
      ASSERT_TRUE(aResult.IsDone);
      aNbSewn = aResult.NbSewnEdges;
    }
  }
  std::cout << "[  PERF   ] 1200 faces (200 boxes), parallel, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  EXPECT_GT(aNbSewn, 1000);
}

// ===================================================================
// Performance on real-world STEP model
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_STEPFile)
{
  TCollection_AsciiString aFilePath = "shape1.stp";

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

  const int aNbIters = aNbFaces > 500 ? 5 : (aNbFaces > 100 ? 20 : 50);

  // Warm-up.
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

  EXPECT_GT(aNbSewnSeq, 0);
  EXPECT_GT(aNbSewnPar, 0);
  EXPECT_EQ(aNbSewnSeq, aNbSewnPar);
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadPoolDiagnostics)
{
  const occ::handle<OSD_ThreadPool>& aPool              = OSD_ThreadPool::DefaultPool();
  const int                          aNbPoolThreads     = aPool->NbThreads();
  const int                          aNbDefLaunch       = aPool->NbDefaultThreadsToLaunch();
  const int                          aNbLogical         = OSD_Parallel::NbLogicalProcessors();
  const bool                         isUsingOcctThreads = OSD_Parallel::ToUseOcctThreads();

  std::cout << "[  INFO   ] OSD_Parallel diagnostics:" << std::endl;
  std::cout << "[  INFO   ]   Logical processors:        " << aNbLogical << std::endl;
  std::cout << "[  INFO   ]   ThreadPool NbThreads:       " << aNbPoolThreads
            << " (includes main thread)" << std::endl;
  std::cout << "[  INFO   ]   NbDefaultThreadsToLaunch:   " << aNbDefLaunch << std::endl;
  std::cout << "[  INFO   ]   Using OCCT threads:         "
            << (isUsingOcctThreads ? "YES" : "NO (TBB)") << std::endl;
  std::cout << "[  INFO   ]   Pool has worker threads:    " << (aPool->HasThreads() ? "YES" : "NO")
            << std::endl;

  EXPECT_GE(aNbPoolThreads, 2) << "Thread pool has only 1 thread, parallel tests are meaningless";

  constexpr int             THE_WORK_ITEMS = 10000;
  std::mutex                aThreadIdMutex;
  std::set<std::thread::id> aThreadIds;
  volatile int              aDummy = 0;

  OSD_Parallel::For(
    0,
    THE_WORK_ITEMS,
    [&](int theIdx) {
      volatile int aSum = 0;
      for (int j = 0; j < 100; ++j)
      {
        aSum += theIdx + j;
      }
      (void)aSum;

      const std::thread::id       aTid = std::this_thread::get_id();
      std::lock_guard<std::mutex> aLock(aThreadIdMutex);
      aThreadIds.insert(aTid);
    },
    false);

  const int aNbDistinctThreads = static_cast<int>(aThreadIds.size());
  std::cout << "[  INFO   ]   Distinct threads used:      " << aNbDistinctThreads << " / "
            << aNbPoolThreads << " (for " << THE_WORK_ITEMS << " work items)" << std::endl;

  EXPECT_GE(aNbDistinctThreads, 2) << "Parallel execution used only 1 thread";

  (void)aDummy;
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadUtilization_Sewing)
{
  const occ::handle<OSD_ThreadPool>& aPool          = OSD_ThreadPool::DefaultPool();
  const int                          aNbPoolThreads = aPool->NbThreads();
  if (aNbPoolThreads < 2)
  {
    GTEST_SKIP() << "Thread pool has only 1 thread, skipping utilization test";
  }

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

  // Sequential.
  BRepGraphAlgo_Sewing::Options aSeqOpts;
  aSeqOpts.Tolerance = 1.0e-04;
  aSeqOpts.Parallel  = false;

  BRepGraph                    aSeqGraph;
  BRepGraphAlgo_Sewing::Result aSeqResult = sewOnGraph(aFaces, aSeqOpts, aSeqGraph);
  ASSERT_TRUE(aSeqResult.IsDone);

  // Parallel.
  BRepGraphAlgo_Sewing::Options aParOpts;
  aParOpts.Tolerance = 1.0e-04;
  aParOpts.Parallel  = true;

  BRepGraph                    aParGraph;
  BRepGraphAlgo_Sewing::Result aParResult = sewOnGraph(aFaces, aParOpts, aParGraph);
  ASSERT_TRUE(aParResult.IsDone);

  EXPECT_EQ(aSeqResult.NbSewnEdges, aParResult.NbSewnEdges);
  EXPECT_GT(aParResult.NbSewnEdges, 0);

  std::cout << "[  INFO   ] Thread pool: " << aNbPoolThreads << " threads, " << aFaces.Length()
            << " faces, " << aParResult.NbSewnEdges << " sewn edges" << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, NoNestedParallel_SequentialInsideParallel)
{
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

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance         = 1.0e-04;
  aOpts.Parallel          = true;
  aOpts.Cutting           = true;
  aOpts.SameParameterMode = true;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aFaces, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GT(aResult.NbSewnEdges, 0);

  std::cout << "[  INFO   ] No-nested-parallel test: " << aFaces.Length() << " faces, "
            << aResult.NbSewnEdges << " sewn (no deadlock)" << std::endl;
}
