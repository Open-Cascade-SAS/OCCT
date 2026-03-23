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
#include <BRepGraphAlgo_Compact.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

namespace
{

constexpr int THE_WARMUP_ITERS = 2;
constexpr int THE_MEASURE_ITERS = 20;

NCollection_Sequence<TopoDS_Shape> makeFaceList(int theNbFaces)
{
  NCollection_Sequence<TopoDS_Shape> aFaces;

  int aNbAdded = 0;
  int aBoxIdx = 0;
  while (aNbAdded < theNbFaces)
  {
    const double aSize = 10.0 + static_cast<double>(aBoxIdx % 7);
    BRepPrimAPI_MakeBox aBoxMaker(aSize, aSize + 1.0, aSize + 2.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();

    for (TopExp_Explorer aFaceExp(aBox, TopAbs_FACE); aFaceExp.More() && aNbAdded < theNbFaces; aFaceExp.Next())
    {
      BRepBuilderAPI_Copy aCopy(aFaceExp.Current(), true);
      aFaces.Append(aCopy.Shape());
      ++aNbAdded;
    }

    ++aBoxIdx;
  }

  return aFaces;
}

TopoDS_Compound makeCopiedFaceCompound(int theNbCopies)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer aFaceExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = aFaceExp.Current();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (int anIdx = 0; anIdx < theNbCopies; ++anIdx)
  {
    BRepBuilderAPI_Copy aCopy(aFace, true);
    aBuilder.Add(aCompound, aCopy.Shape());
  }
  return aCompound;
}

template <typename Func>
double runBenchmark(const char* theLabel, Func theFunc)
{
  for (int aWarmupIter = 0; aWarmupIter < THE_WARMUP_ITERS; ++aWarmupIter)
  {
    theFunc();
  }

  double aTotal = 0.0;
  for (int anIter = 0; anIter < THE_MEASURE_ITERS; ++anIter)
  {
    const std::chrono::steady_clock::time_point aStart = std::chrono::steady_clock::now();
    theFunc();
    const std::chrono::steady_clock::time_point anEnd = std::chrono::steady_clock::now();
    aTotal += std::chrono::duration<double>(anEnd - aStart).count();
  }

  const double anAvg = aTotal / static_cast<double>(THE_MEASURE_ITERS);
  std::cout << "[  PERF   ] " << theLabel << ": avg " << anAvg << " s over "
            << THE_MEASURE_ITERS << " iters" << std::endl;
  return anAvg;
}

} // namespace

TEST(BRepGraphAlgo_Benchmark, Smoke_DeduplicateCompactCycle)
{
  BRepGraph aGraph;
  aGraph.Build(makeCopiedFaceCompound(2));
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aDedupRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_GE(aDedupRes.NbSurfaceRewrites + aDedupRes.NbCurveRewrites, 0);

  const BRepGraphAlgo_Compact::Result aCompactRes = BRepGraphAlgo_Compact::Perform(aGraph);
  EXPECT_GE(aCompactRes.NbNodesBefore, aCompactRes.NbNodesAfter);
}

TEST(BRepGraphAlgo_Benchmark, DISABLED_Sewing_Throughput_500Faces)
{
  const NCollection_Sequence<TopoDS_Shape> aFaces = makeFaceList(500);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
    aBB.Add(aCompound, aFaces.Value(anIdx));

  const double aAvg = runBenchmark("Sewing throughput 500 faces", [&]() {
    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = true;
    BRepGraph aGraph;
    aGraph.Build(aCompound, true);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    EXPECT_TRUE(aResult.IsDone);
  });

  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraphAlgo_Benchmark, DISABLED_Sewing_Throughput_500Faces_Sequential)
{
  const NCollection_Sequence<TopoDS_Shape> aFaces = makeFaceList(500);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= aFaces.Length(); ++anIdx)
    aBB.Add(aCompound, aFaces.Value(anIdx));

  const double aAvg = runBenchmark("Sewing throughput 500 faces sequential", [&]() {
    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = false;
    BRepGraph aGraph;
    aGraph.Build(aCompound);
    BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    EXPECT_TRUE(aResult.IsDone);
  });

  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraphAlgo_Benchmark, DISABLED_DeduplicateCompact_Throughput)
{
  const TopoDS_Compound aFaces = makeCopiedFaceCompound(500);

  const double aAvg = runBenchmark("Deduplicate+Compact 500 copies", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces);
    ASSERT_TRUE(aGraph.IsDone());

    const BRepGraphAlgo_Deduplicate::Result aDedupRes = BRepGraphAlgo_Deduplicate::Perform(aGraph);
    EXPECT_GE(aDedupRes.NbSurfaceRewrites + aDedupRes.NbCurveRewrites, 0);

    const BRepGraphAlgo_Compact::Result aCompactRes = BRepGraphAlgo_Compact::Perform(aGraph);
    EXPECT_GE(aCompactRes.NbNodesBefore, aCompactRes.NbNodesAfter);
  });

  EXPECT_GT(aAvg, 0.0);
}
