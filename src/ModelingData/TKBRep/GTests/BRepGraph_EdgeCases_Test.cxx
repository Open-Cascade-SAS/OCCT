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
#include <BRepGraph_Builder.hxx>
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
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 = BRepGraph_Builder::Add(aGraph, aNullShape);
  EXPECT_FALSE(aGraph.IsDone());
}

TEST(BRepGraph_EdgeCasesTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRepGraph       aGraph;
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 = BRepGraph_Builder::Add(aGraph, aCompound);

  // Whether IsDone is true or false for an empty compound is implementation-defined;
  // the key invariant is that all definition counts are zero.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
}

TEST(BRepGraph_EdgeCasesTest, Shape_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId; // default: Index = -1
  const TopoDS_Shape     aShape = aGraph.Shapes().Shape(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraph_EdgeCasesTest, ReconstructShape_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const TopoDS_Shape     aShape = aGraph.Shapes().Reconstruct(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraph_EdgeCasesTest, TopoEntity_InvalidNodeId_ReturnsNull)
{
  BRepGraph           aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId       anInvalidId;
  const BRepGraphInc::BaseDef* aDef = aGraph.Topo().Gen().TopoEntity(anInvalidId);
  EXPECT_EQ(aDef, nullptr);
}

TEST(BRepGraph_EdgeCasesTest, NbNodes_BeforeBuild_ReturnsZero)
{
  BRepGraph aGraph;
  EXPECT_EQ(aGraph.Topo().Gen().NbNodes(), 0u);
}

// ============================================================
// Rebuild behavior tests
// ============================================================

TEST(BRepGraph_EdgeCasesTest, Build_TwiceOnSameGraph_GenerationIncrements)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 = BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 = BRepGraph_Builder::Add(aGraph, aBox);
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
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 = BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.UIDs().Generation();

  // Record total node count from first build.
  const size_t aTotalFirstBuild = aGraph.Topo().Gen().NbNodes();
  ASSERT_GT(aTotalFirstBuild, 0u);

  // Second build.
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 = BRepGraph_Builder::Add(aGraph, aBox);
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

  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 = BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aSolids1 = aGraph.Topo().Solids().Nb();
  const int aShells1 = aGraph.Topo().Shells().Nb();
  const int aFaces1  = aGraph.Topo().Faces().Nb();
  const int aWires1  = aGraph.Topo().Wires().Nb();
  const int aEdges1  = aGraph.Topo().Edges().Nb();
  const int aVerts1  = aGraph.Topo().Vertices().Nb();
  const int aSurfs1  = aGraph.Topo().Faces().Nb();
  const int aCurves1 = aGraph.Topo().Edges().Nb();

  // Rebuild with same shape.
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 = BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aSolids1);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), aShells1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aFaces1);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), aWires1);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aEdges1);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aVerts1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aSurfs1);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aCurves1);
}

// ============================================================
// UID edge cases
// ============================================================

TEST(BRepGraph_EdgeCasesTest, UID_AlwaysEnabled_AfterBuild)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
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
  aSeqGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 = BRepGraph_Builder::Add(aSeqGraph, aSphere, BRepGraph_Builder::Options{ {}, true, false, false });
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 = BRepGraph_Builder::Add(aParGraph, aSphere, BRepGraph_Builder::Options{ {}, true, false, true });
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Topo().Solids().Nb(), aSeqGraph.Topo().Solids().Nb());
  EXPECT_EQ(aParGraph.Topo().Shells().Nb(), aSeqGraph.Topo().Shells().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Wires().Nb(), aSeqGraph.Topo().Wires().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Vertices().Nb(), aSeqGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Gen().NbNodes(), aSeqGraph.Topo().Gen().NbNodes());
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
  aSeqGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 = BRepGraph_Builder::Add(aSeqGraph, aCompound, BRepGraph_Builder::Options{ {}, true, false, false });
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes16 = BRepGraph_Builder::Add(aParGraph, aCompound, BRepGraph_Builder::Options{ {}, true, false, true });
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Topo().Solids().Nb(), aSeqGraph.Topo().Solids().Nb());
  EXPECT_EQ(aParGraph.Topo().Shells().Nb(), aSeqGraph.Topo().Shells().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Wires().Nb(), aSeqGraph.Topo().Wires().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Vertices().Nb(), aSeqGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Gen().NbNodes(), aSeqGraph.Topo().Gen().NbNodes());
}
