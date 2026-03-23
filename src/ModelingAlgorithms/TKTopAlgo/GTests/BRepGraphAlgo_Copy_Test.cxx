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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <cmath>
#include <iostream>

#include <gtest/gtest.h>

TEST(BRepGraphAlgo_CopyTest, CopyBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_CopyTest, CopyBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  // Sum face areas since result may be a compound.
  double aCopyArea = 0.0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aCopyArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraphAlgo_CopyTest, CopyBox_GeometryIsIndependent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  // Verify that the copy has independent geometry (different TShape pointers).
  TopExp_Explorer anOrigExp(aBox, TopAbs_FACE);
  TopExp_Explorer aCopyExp(aCopy, TopAbs_FACE);
  ASSERT_TRUE(anOrigExp.More());
  ASSERT_TRUE(aCopyExp.More());
  // The TShapes should be different objects.
  EXPECT_NE(anOrigExp.Current().TShape().get(), aCopyExp.Current().TShape().get());
}

TEST(BRepGraphAlgo_CopyTest, CopyBox_SharedGeometry)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // theCopyGeom = false: geometry is shared.
  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, false);
  ASSERT_FALSE(aCopy.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_CopyTest, CopyCylinder_FaceCount)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  int anOrigFaces = 0, aCopyFaces = 0;
  for (TopExp_Explorer anExp(aCyl, TopAbs_FACE); anExp.More(); anExp.Next())
    ++anOrigFaces;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aCopyFaces;
  EXPECT_EQ(aCopyFaces, anOrigFaces);
}

TEST(BRepGraphAlgo_CopyTest, CopySingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  TopoDS_Shape aCopiedFace = BRepGraphAlgo_Copy::CopyFace(aGraph, 0, true);
  ASSERT_FALSE(aCopiedFace.IsNull());
  EXPECT_EQ(aCopiedFace.ShapeType(), TopAbs_FACE);

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aCopiedFace, aProps);
  EXPECT_GT(std::abs(aProps.Mass()), 1.0);
}

TEST(BRepGraphAlgo_CopyTest, CopyFacesOnly_Compound)
{
  // Build graph from individual faces (no shells/solids).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
    aBB.Add(aCompound, aCopier.Shape());
  }

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 6);
  ASSERT_EQ(aGraph.Defs().NbSolids(), 0);
  ASSERT_EQ(aGraph.Defs().NbShells(), 0);

  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

// ============================================================
// SameParameter / SameRange round-trip
// ============================================================

TEST(BRepGraphAlgo_CopyTest, CopyBox_SameParameter_Preserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  // All edges in the copied shape must preserve SameParameter = true.
  for (TopExp_Explorer anExp(aCopy, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    EXPECT_TRUE(BRep_Tool::SameParameter(anEdge))
        << "Copied edge lost SameParameter flag";
    EXPECT_TRUE(BRep_Tool::SameRange(anEdge))
        << "Copied edge lost SameRange flag";
  }
}

// ============================================================
// Regularity (BRep_CurveOn2Surfaces) preserved via EncodeRegularity
// ============================================================

TEST(BRepGraphAlgo_CopyTest, FusedBoxes_Regularity_AreaPreserved)
{
  // Fuse two adjacent boxes to produce a shape with regularity edges.
  // Verify that the copy preserves area (geometry integrity after EncodeRegularity call).
  TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aFused, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Build(aFused);
  ASSERT_TRUE(aGraph.IsDone());

  const TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_FALSE(aCopy.IsNull());

  // Verify face count is the same.
  int anOrigFaces = 0, aCopyFaces = 0;
  for (TopExp_Explorer anExp(aFused, TopAbs_FACE); anExp.More(); anExp.Next())
    ++anOrigFaces;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aCopyFaces;
  EXPECT_EQ(aCopyFaces, anOrigFaces);

  // Verify area is preserved.
  double aCopyArea = 0.0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aCopyArea += std::abs(aProps.Mass());
  }
  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

// ============================================================
// Performance comparison: BRepGraphAlgo_Copy vs BRepBuilderAPI_Copy.
// ============================================================

TEST(BRepGraphAlgo_CopyTest, Performance_VsLegacy)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  constexpr int THE_NB_ITERS = 1000;

  // Graph-based copy: build graph once, copy many times.
  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  OSD_Timer aGraphTimer;
  aGraphTimer.Start();
  for (int anIter = 0; anIter < THE_NB_ITERS; ++anIter)
  {
    TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
    (void)aCopy;
  }
  aGraphTimer.Stop();

  // Legacy copy.
  OSD_Timer aLegacyTimer;
  aLegacyTimer.Start();
  for (int anIter = 0; anIter < THE_NB_ITERS; ++anIter)
  {
    BRepBuilderAPI_Copy aCopier(aBox, true);
    TopoDS_Shape aCopy = aCopier.Shape();
    (void)aCopy;
  }
  aLegacyTimer.Stop();

  std::cout << "[  PERF   ] BRepGraphAlgo_Copy: " << aGraphTimer.ElapsedTime()
            << " s (" << THE_NB_ITERS << " iters)" << std::endl;
  std::cout << "[  PERF   ] BRepBuilderAPI_Copy: " << aLegacyTimer.ElapsedTime()
            << " s (" << THE_NB_ITERS << " iters)" << std::endl;
  if (aGraphTimer.ElapsedTime() > 1.0e-9)
  {
    std::cout << "[  PERF   ] Speedup: "
              << aLegacyTimer.ElapsedTime() / aGraphTimer.ElapsedTime() << "x" << std::endl;
  }
}
