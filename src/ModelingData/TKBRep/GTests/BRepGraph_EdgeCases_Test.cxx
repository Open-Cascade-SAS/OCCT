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
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

// ============================================================
// Null / Empty / Invalid input tests
// ============================================================

TEST(BRepGraphEdgeCasesTest, Build_NullShape_IsDoneFalse)
{
  BRepGraph aGraph;
  TopoDS_Shape aNullShape;
  aGraph.Build(aNullShape);
  EXPECT_FALSE(aGraph.IsDone());
}

TEST(BRepGraphEdgeCasesTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRepGraph aGraph;
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aGraph.Build(aCompound);

  // Whether IsDone is true or false for an empty compound is implementation-defined;
  // the key invariant is that all definition counts are zero.
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 0);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 0);
  EXPECT_EQ(aGraph.Defs().NbWires(), 0);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 0);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 0);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 0);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 0);
}

TEST(BRepGraphEdgeCasesTest, Shape_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId; // default: Index = -1
  const TopoDS_Shape aShape = aGraph.Shapes().Shape(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraphEdgeCasesTest, ReconstructShape_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const TopoDS_Shape aShape = aGraph.Shapes().Reconstruct(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraphEdgeCasesTest, TopoDef_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const BRepGraph_TopoNode::BaseDef* aDef = aGraph.Defs().TopoDef(anInvalidId);
  EXPECT_EQ(aDef, nullptr);
}

TEST(BRepGraphEdgeCasesTest, NbNodes_BeforeBuild_ReturnsZero)
{
  BRepGraph aGraph;
  EXPECT_EQ(aGraph.Defs().NbNodes(), 0u);
}

// ============================================================
// Rebuild behavior tests
// ============================================================

TEST(BRepGraphEdgeCasesTest, Build_TwiceOnSameGraph_GenerationIncrements)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.UIDs().Generation();

  EXPECT_GT(aSecondGen, aFirstGen);
}

TEST(BRepGraphEdgeCasesTest, Build_TwiceOnSameGraph_OldUIDsInvalidated)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  // First build: collect total UID counter used.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  // Record total node count from first build.
  const size_t aTotalFirstBuild = aGraph.Defs().NbNodes();
  ASSERT_GT(aTotalFirstBuild, 0u);

  // Second build.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.UIDs().Generation();
  EXPECT_NE(aFirstGen, aSecondGen);

  // After rebuild, UIDs on the new nodes carry the new generation.
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const BRepGraph_UID aUID = aGraph.UIDs().Of(aSolidId);
  EXPECT_TRUE(aUID.IsValid());
  EXPECT_EQ(aUID.Generation(), aSecondGen);
}

TEST(BRepGraphEdgeCasesTest, Build_TwiceOnSameGraph_CountsResetCorrectly)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aSolids1  = aGraph.Defs().NbSolids();
  const int aShells1  = aGraph.Defs().NbShells();
  const int aFaces1   = aGraph.Defs().NbFaces();
  const int aWires1   = aGraph.Defs().NbWires();
  const int aEdges1   = aGraph.Defs().NbEdges();
  const int aVerts1   = aGraph.Defs().NbVertices();
  const int aSurfs1   = aGraph.Defs().NbFaces();
  const int aCurves1  = aGraph.Defs().NbEdges();

  // Rebuild with same shape.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbSolids(), aSolids1);
  EXPECT_EQ(aGraph.Defs().NbShells(), aShells1);
  EXPECT_EQ(aGraph.Defs().NbFaces(), aFaces1);
  EXPECT_EQ(aGraph.Defs().NbWires(), aWires1);
  EXPECT_EQ(aGraph.Defs().NbEdges(), aEdges1);
  EXPECT_EQ(aGraph.Defs().NbVertices(), aVerts1);
  EXPECT_EQ(aGraph.Defs().NbFaces(), aSurfs1);
  EXPECT_EQ(aGraph.Defs().NbEdges(), aCurves1);
}

// ============================================================
// UID edge cases
// ============================================================

TEST(BRepGraphEdgeCasesTest, UID_AlwaysEnabled_AfterBuild)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const BRepGraph_UID aUID = aGraph.UIDs().Of(aSolidId);
  EXPECT_TRUE(aUID.IsValid());
}

// ============================================================
// Parallel build equivalence tests
// ============================================================

TEST(BRepGraphEdgeCasesTest, ParallelBuild_Sphere_SameAsSequential)
{
  BRepPrimAPI_MakeSphere aSphereMaker(50.0);
  const TopoDS_Shape aSphere = aSphereMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Build(aSphere, false);
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Build(aSphere, true);
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Defs().NbSolids(),  aSeqGraph.Defs().NbSolids());
  EXPECT_EQ(aParGraph.Defs().NbShells(),  aSeqGraph.Defs().NbShells());
  EXPECT_EQ(aParGraph.Defs().NbFaces(),   aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbWires(),   aSeqGraph.Defs().NbWires());
  EXPECT_EQ(aParGraph.Defs().NbEdges(),   aSeqGraph.Defs().NbEdges());
  EXPECT_EQ(aParGraph.Defs().NbVertices(), aSeqGraph.Defs().NbVertices());
  EXPECT_EQ(aParGraph.Defs().NbFaces(),   aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbEdges(),   aSeqGraph.Defs().NbEdges());
  EXPECT_EQ(aParGraph.Defs().NbNodes(),      aSeqGraph.Defs().NbNodes());
}

TEST(BRepGraphEdgeCasesTest, ParallelBuild_Compound_SameAsSequential)
{
  // Build a compound of three boxes at different sizes.
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 30.0, 40.0);
  BRepPrimAPI_MakeBox aBox3(5.0, 15.0, 25.0);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());
  aBuilder.Add(aCompound, aBox3.Shape());

  BRepGraph aSeqGraph;
  aSeqGraph.Build(aCompound, false);
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Build(aCompound, true);
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Defs().NbSolids(),  aSeqGraph.Defs().NbSolids());
  EXPECT_EQ(aParGraph.Defs().NbShells(),  aSeqGraph.Defs().NbShells());
  EXPECT_EQ(aParGraph.Defs().NbFaces(),   aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbWires(),   aSeqGraph.Defs().NbWires());
  EXPECT_EQ(aParGraph.Defs().NbEdges(),   aSeqGraph.Defs().NbEdges());
  EXPECT_EQ(aParGraph.Defs().NbVertices(), aSeqGraph.Defs().NbVertices());
  EXPECT_EQ(aParGraph.Defs().NbFaces(),   aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbEdges(),   aSeqGraph.Defs().NbEdges());
  EXPECT_EQ(aParGraph.Defs().NbNodes(),      aSeqGraph.Defs().NbNodes());
}
