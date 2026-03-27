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
#include <BRepGraph_TopoView.hxx>
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

TEST(BRepGraph_EdgeCasesTest, Build_NullShape_IsDoneFalse)
{
  BRepGraph    aGraph;
  TopoDS_Shape aNullShape;
  aGraph.Build(aNullShape);
  EXPECT_FALSE(aGraph.IsDone());
}

TEST(BRepGraph_EdgeCasesTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRepGraph       aGraph;
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aGraph.Build(aCompound);

  // Whether IsDone is true or false for an empty compound is implementation-defined;
  // the key invariant is that all definition counts are zero.
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(aGraph.Topo().NbWires(), 0);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 0);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 0);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 0);
}

TEST(BRepGraph_EdgeCasesTest, Shape_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId; // default: Index = -1
  const TopoDS_Shape     aShape = aGraph.Shapes().Shape(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraph_EdgeCasesTest, ReconstructShape_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const TopoDS_Shape     aShape = aGraph.Shapes().Reconstruct(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraph_EdgeCasesTest, TopoEntity_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId             anInvalidId;
  const BRepGraphInc::BaseEntity* aDef = aGraph.Topo().TopoEntity(anInvalidId);
  EXPECT_EQ(aDef, nullptr);
}

TEST(BRepGraph_EdgeCasesTest, NbNodes_BeforeBuild_ReturnsZero)
{
  BRepGraph aGraph;
  EXPECT_EQ(aGraph.Topo().NbNodes(), 0u);
}

// ============================================================
// Rebuild behavior tests
// ============================================================

TEST(BRepGraph_EdgeCasesTest, Build_TwiceOnSameGraph_GenerationIncrements)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.UIDs().Generation();

  EXPECT_GT(aSecondGen, aFirstGen);
}

TEST(BRepGraph_EdgeCasesTest, Build_TwiceOnSameGraph_OldUIDsInvalidated)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  // First build: collect total UID counter used.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  // Record total node count from first build.
  const size_t aTotalFirstBuild = aGraph.Topo().NbNodes();
  ASSERT_GT(aTotalFirstBuild, 0u);

  // Second build.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.UIDs().Generation();
  EXPECT_NE(aFirstGen, aSecondGen);

  // After rebuild, UIDs on the new nodes carry the new generation.
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const BRepGraph_UID    aUID = aGraph.UIDs().Of(aSolidId);
  EXPECT_TRUE(aUID.IsValid());
  EXPECT_EQ(aUID.Generation(), aSecondGen);
}

TEST(BRepGraph_EdgeCasesTest, Build_TwiceOnSameGraph_CountsResetCorrectly)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aSolids1 = aGraph.Topo().NbSolids();
  const int aShells1 = aGraph.Topo().NbShells();
  const int aFaces1  = aGraph.Topo().NbFaces();
  const int aWires1  = aGraph.Topo().NbWires();
  const int aEdges1  = aGraph.Topo().NbEdges();
  const int aVerts1  = aGraph.Topo().NbVertices();
  const int aSurfs1  = aGraph.Topo().NbFaces();
  const int aCurves1 = aGraph.Topo().NbEdges();

  // Rebuild with same shape.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbSolids(), aSolids1);
  EXPECT_EQ(aGraph.Topo().NbShells(), aShells1);
  EXPECT_EQ(aGraph.Topo().NbFaces(), aFaces1);
  EXPECT_EQ(aGraph.Topo().NbWires(), aWires1);
  EXPECT_EQ(aGraph.Topo().NbEdges(), aEdges1);
  EXPECT_EQ(aGraph.Topo().NbVertices(), aVerts1);
  EXPECT_EQ(aGraph.Topo().NbFaces(), aSurfs1);
  EXPECT_EQ(aGraph.Topo().NbEdges(), aCurves1);
}

// ============================================================
// UID edge cases
// ============================================================

TEST(BRepGraph_EdgeCasesTest, UID_AlwaysEnabled_AfterBuild)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const BRepGraph_UID    aUID = aGraph.UIDs().Of(aSolidId);
  EXPECT_TRUE(aUID.IsValid());
}

// ============================================================
// Parallel build equivalence tests
// ============================================================

TEST(BRepGraph_EdgeCasesTest, ParallelBuild_Sphere_SameAsSequential)
{
  BRepPrimAPI_MakeSphere aSphereMaker(50.0);
  const TopoDS_Shape     aSphere = aSphereMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Build(aSphere, false);
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Build(aSphere, true);
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Topo().NbSolids(), aSeqGraph.Topo().NbSolids());
  EXPECT_EQ(aParGraph.Topo().NbShells(), aSeqGraph.Topo().NbShells());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbWires(), aSeqGraph.Topo().NbWires());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
  EXPECT_EQ(aParGraph.Topo().NbVertices(), aSeqGraph.Topo().NbVertices());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
  EXPECT_EQ(aParGraph.Topo().NbNodes(), aSeqGraph.Topo().NbNodes());
}

TEST(BRepGraph_EdgeCasesTest, ParallelBuild_Compound_SameAsSequential)
{
  // Build a compound of three boxes at different sizes.
  BRep_Builder    aBuilder;
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

  EXPECT_EQ(aParGraph.Topo().NbSolids(), aSeqGraph.Topo().NbSolids());
  EXPECT_EQ(aParGraph.Topo().NbShells(), aSeqGraph.Topo().NbShells());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbWires(), aSeqGraph.Topo().NbWires());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
  EXPECT_EQ(aParGraph.Topo().NbVertices(), aSeqGraph.Topo().NbVertices());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
  EXPECT_EQ(aParGraph.Topo().NbNodes(), aSeqGraph.Topo().NbNodes());
}
