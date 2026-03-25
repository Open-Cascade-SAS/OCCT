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
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

namespace
{

constexpr int THE_WARMUP_ITERS  = 2;
constexpr int THE_MEASURE_ITERS = 20;

TopoDS_Compound makeFaceCloud(int theNbFaces)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  int aNbAdded = 0;
  int aBoxIdx  = 0;
  while (aNbAdded < theNbFaces)
  {
    const double        aSize = 10.0 + static_cast<double>(aBoxIdx % 7);
    BRepPrimAPI_MakeBox aBoxMaker(aSize, aSize + 1.0, aSize + 2.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();

    for (TopExp_Explorer aFaceExp(aBox, TopAbs_FACE); aFaceExp.More() && aNbAdded < theNbFaces;
         aFaceExp.Next())
    {
      BRepBuilderAPI_Copy aCopy(aFaceExp.Current(), true);
      aBuilder.Add(aCompound, aCopy.Shape());
      ++aNbAdded;
    }

    ++aBoxIdx;
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
  std::cout << "[  PERF   ] " << theLabel << ": avg " << anAvg << " s over " << THE_MEASURE_ITERS
            << " iters" << std::endl;
  return anAvg;
}

} // namespace

TEST(BRepGraph_BenchmarkTest, Smoke_BuildReconstructAndAdjacency)
{
  const TopoDS_Compound aFaces = makeFaceCloud(120);

  BRepGraph aGraph;
  aGraph.Build(aFaces);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape     aFaceShape = aGraph.Shapes().Reconstruct(aFaceId);
  EXPECT_FALSE(aFaceShape.IsNull());

  const NCollection_Vector<BRepGraph_NodeId> anAdj = aGraph.Spatial().AdjacentFaces(aFaceId);
  EXPECT_GE(anAdj.Length(), 0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Build_100Faces)
{
  const TopoDS_Compound aFaces = makeFaceCloud(100);
  const double          aAvg   = runBenchmark("Build 100 faces", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces);
    EXPECT_TRUE(aGraph.IsDone());
  });
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Build_1000Faces)
{
  const TopoDS_Compound aFaces = makeFaceCloud(1000);
  const double          aAvg   = runBenchmark("Build 1000 faces", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces);
    EXPECT_TRUE(aGraph.IsDone());
  });
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Build_10000Faces)
{
  const TopoDS_Compound aFaces = makeFaceCloud(10000);
  const double          aAvg   = runBenchmark("Build 10000 faces", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces);
    EXPECT_TRUE(aGraph.IsDone());
  });
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Build_1000Faces_Parallel)
{
  const TopoDS_Compound aFaces = makeFaceCloud(1000);
  const double          aAvg   = runBenchmark("Build 1000 faces parallel", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces, true);
    EXPECT_TRUE(aGraph.IsDone());
  });
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Build_10000Faces_Parallel)
{
  const TopoDS_Compound aFaces = makeFaceCloud(10000);
  const double          aAvg   = runBenchmark("Build 10000 faces parallel", [&]() {
    BRepGraph aGraph;
    aGraph.Build(aFaces, true);
    EXPECT_TRUE(aGraph.IsDone());
  });
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_Reconstruct_RoundTrip)
{
  const TopoDS_Compound aFaces = makeFaceCloud(10000);
  BRepGraph             aGraph;
  aGraph.Build(aFaces);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbFaces = aGraph.Defs().NbFaces();
  ASSERT_GT(aNbFaces, 0);

  const double aAvg = runBenchmark("Reconstruct 10000 faces", [&]() {
    for (int anIdx = 0; anIdx < aNbFaces; ++anIdx)
    {
      const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(anIdx);
      const TopoDS_Shape     aShape  = aGraph.Shapes().Reconstruct(aFaceId);
      EXPECT_FALSE(aShape.IsNull());
    }
  });

  const double aPerFace = aAvg / static_cast<double>(aNbFaces);
  std::cout << "[  PERF   ] Reconstruct per-face avg: " << aPerFace << " s" << std::endl;
  EXPECT_GT(aAvg, 0.0);
}

TEST(BRepGraph_BenchmarkTest, DISABLED_SpatialQuery_Throughput)
{
  const TopoDS_Compound aFaces = makeFaceCloud(10000);
  BRepGraph             aGraph;
  aGraph.Build(aFaces);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbFaces = aGraph.Defs().NbFaces();
  ASSERT_GT(aNbFaces, 0);

  const double aAvg = runBenchmark("SpatialQuery 10000 faces", [&]() {
    for (int anIdx = 0; anIdx < aNbFaces; ++anIdx)
    {
      const BRepGraph_NodeId                     aFaceId = BRepGraph_NodeId::Face(anIdx);
      const NCollection_Vector<BRepGraph_NodeId> anAdj   = aGraph.Spatial().AdjacentFaces(aFaceId);
      EXPECT_GE(anAdj.Length(), 0);
    }
  });

  const double aPerQuery = aAvg / static_cast<double>(aNbFaces);
  std::cout << "[  PERF   ] SpatialQuery per-face avg: " << aPerQuery << " s" << std::endl;
  EXPECT_GT(aAvg, 0.0);
}
