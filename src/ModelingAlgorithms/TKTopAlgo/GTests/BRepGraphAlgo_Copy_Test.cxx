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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
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

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), aGraph.Topo().NbFaces());

  // Verify reconstructed shape has correct face count.
  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_NodeId::Solid(0));
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

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_NodeId::Solid(0));
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

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Deep copy: surface handles must be different objects.
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);
  ASSERT_GT(aCopyGraph.Topo().NbFaces(), 0);
  EXPECT_NE(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get());
}

TEST(BRepGraphAlgo_CopyTest, CopyBox_SharedGeometry)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // theCopyGeom = false: geometry is shared.
  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, false);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), 6);

  // Light copy: surface handles must be the same objects.
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);
  ASSERT_GT(aCopyGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId(0)).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get());
}

TEST(BRepGraphAlgo_CopyTest, CopyCylinder_FaceCount)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), aGraph.Topo().NbFaces());
}

TEST(BRepGraphAlgo_CopyTest, CopySingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::CopyFace(aGraph, BRepGraph_FaceId(0), true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), 1);

  // A box face has 1 wire with 4 edges and 4 vertices.
  EXPECT_GT(aCopyGraph.Topo().NbVertices(), 0);
  EXPECT_GT(aCopyGraph.Topo().NbEdges(), 0);
  EXPECT_GT(aCopyGraph.Topo().NbWires(), 0);

  // No shells/solids in a single-face sub-graph.
  EXPECT_EQ(aCopyGraph.Topo().NbShells(), 0);
  EXPECT_EQ(aCopyGraph.Topo().NbSolids(), 0);

  TopoDS_Shape aCopiedFace = aCopyGraph.Shapes().ReconstructFace(BRepGraph_FaceId(0));
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

  BRep_Builder    aBB;
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
  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);
  ASSERT_EQ(aGraph.Topo().NbSolids(), 0);
  ASSERT_EQ(aGraph.Topo().NbShells(), 0);

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aCopyGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aCopyGraph.Topo().NbShells(), 0);
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

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // All edges in the copied graph must preserve SameParameter = true.
  for (int anIdx = 0; anIdx < aCopyGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = aCopyGraph.Topo().Edge(BRepGraph_EdgeId(anIdx));
    EXPECT_TRUE(anEdge.SameParameter) << "Copied edge " << anIdx << " lost SameParameter flag";
    EXPECT_TRUE(anEdge.SameRange) << "Copied edge " << anIdx << " lost SameRange flag";
  }
}

// ============================================================
// Regularity (BRep_CurveOn2Surfaces) preserved via EncodeRegularity
// ============================================================

TEST(BRepGraphAlgo_CopyTest, FusedBoxes_Regularity_AreaPreserved)
{
  // Fuse two adjacent boxes to produce a shape with regularity edges.
  // Verify that the copy preserves area (geometry integrity).
  TopoDS_Shape     aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape     aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aFused, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Build(aFused);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Verify node counts match.
  EXPECT_EQ(aCopyGraph.Topo().NbFaces(), aGraph.Topo().NbFaces());
  EXPECT_EQ(aCopyGraph.Topo().NbEdges(), aGraph.Topo().NbEdges());

  // Verify area is preserved by summing individual face areas.
  double aCopyArea = 0.0;
  for (int aFaceIdx = 0; aFaceIdx < aCopyGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    TopoDS_Shape aFace = aCopyGraph.Shapes().ReconstructFace(BRepGraph_FaceId(aFaceIdx));
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    aCopyArea += std::abs(aProps.Mass());
  }
  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

// ============================================================
// UID preservation
// ============================================================

TEST(BRepGraphAlgo_CopyTest, CopyBox_UIDsPreserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraphAlgo_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Helper to check UIDs for a given node kind.
  auto checkUIDs = [&](BRepGraph_NodeId::Kind theKind, int theCount, const char* theLabel) {
    for (int anIdx = 0; anIdx < theCount; ++anIdx)
    {
      BRepGraph_NodeId aNodeId(theKind, anIdx);
      BRepGraph_UID    anOrigUID = aGraph.UIDs().Of(aNodeId);
      BRepGraph_UID    aCopyUID  = aCopyGraph.UIDs().Of(aNodeId);
      EXPECT_EQ(aCopyUID.IsValid(), anOrigUID.IsValid())
        << "UID validity mismatch at " << theLabel << " " << anIdx;
      if (anOrigUID.IsValid())
        EXPECT_EQ(aCopyUID, anOrigUID) << "UID mismatch at " << theLabel << " " << anIdx;
    }
  };

  // Verify UIDs for all topology and geometry node types.
  checkUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Topo().NbVertices(), "vertex");
  checkUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().NbEdges(), "edge");
  checkUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().NbWires(), "wire");
  checkUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().NbFaces(), "face");
  checkUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().NbShells(), "shell");
  checkUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().NbSolids(), "solid");
  // Geometry is now stored inline on face/edge defs; no separate geometry UIDs to check.
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
    BRepGraph aCopy = BRepGraphAlgo_Copy::Perform(aGraph, true);
    (void)aCopy;
  }
  aGraphTimer.Stop();

  // Legacy copy.
  OSD_Timer aLegacyTimer;
  aLegacyTimer.Start();
  for (int anIter = 0; anIter < THE_NB_ITERS; ++anIter)
  {
    BRepBuilderAPI_Copy aCopier(aBox, true);
    TopoDS_Shape        aCopy = aCopier.Shape();
    (void)aCopy;
  }
  aLegacyTimer.Stop();

  std::cout << "[  PERF   ] BRepGraphAlgo_Copy: " << aGraphTimer.ElapsedTime() << " s ("
            << THE_NB_ITERS << " iters)" << std::endl;
  std::cout << "[  PERF   ] BRepBuilderAPI_Copy: " << aLegacyTimer.ElapsedTime() << " s ("
            << THE_NB_ITERS << " iters)" << std::endl;
  if (aGraphTimer.ElapsedTime() > 1.0e-9)
  {
    std::cout << "[  PERF   ] Speedup: " << aLegacyTimer.ElapsedTime() / aGraphTimer.ElapsedTime()
              << "x" << std::endl;
  }
}
