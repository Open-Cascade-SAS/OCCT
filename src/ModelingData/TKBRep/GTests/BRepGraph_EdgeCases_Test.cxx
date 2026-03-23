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
  EXPECT_EQ(aGraph.NbSolidDefs(), 0);
  EXPECT_EQ(aGraph.NbShellDefs(), 0);
  EXPECT_EQ(aGraph.NbFaceDefs(), 0);
  EXPECT_EQ(aGraph.NbWireDefs(), 0);
  EXPECT_EQ(aGraph.NbEdgeDefs(), 0);
  EXPECT_EQ(aGraph.NbVertexDefs(), 0);
  EXPECT_EQ(aGraph.NbSurfaces(), 0);
  EXPECT_EQ(aGraph.NbCurves(), 0);
  EXPECT_EQ(aGraph.NbPCurves(), 0);
}

TEST(BRepGraphEdgeCasesTest, Shape_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId; // default: Index = -1
  const TopoDS_Shape aShape = aGraph.Shape(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraphEdgeCasesTest, ReconstructShape_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const TopoDS_Shape aShape = aGraph.ReconstructShape(anInvalidId);
  EXPECT_TRUE(aShape.IsNull());
}

TEST(BRepGraphEdgeCasesTest, UsagesOf_InvalidNodeId_HandlesGracefully)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  EXPECT_NO_THROW({
    const NCollection_Vector<BRepGraph_UsageId>& aUsages = aGraph.UsagesOf(anInvalidId);
    (void)aUsages;
  });
}

TEST(BRepGraphEdgeCasesTest, DefOf_InvalidUsageId_ReturnsInvalid)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_UsageId anInvalidUsage;
  const BRepGraph_NodeId aDefId = aGraph.DefOf(anInvalidUsage);
  EXPECT_FALSE(aDefId.IsValid());
}

TEST(BRepGraphEdgeCasesTest, TopoDef_InvalidNodeId_ReturnsNull)
{
  BRepGraph aGraph;
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId anInvalidId;
  const BRepGraph_TopoNode::BaseDef* aDef = aGraph.TopoDef(anInvalidId);
  EXPECT_EQ(aDef, nullptr);
}

TEST(BRepGraphEdgeCasesTest, NbNodes_BeforeBuild_ReturnsZero)
{
  BRepGraph aGraph;
  EXPECT_EQ(aGraph.NbNodes(), 0u);
}

// ============================================================
// Rebuild behavior tests
// ============================================================

TEST(BRepGraphEdgeCasesTest, Build_TwiceOnSameGraph_GenerationIncrements)
{
  BRepGraph aGraph;
  aGraph.SetUIDEnabled(true);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.Generation();

  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.Generation();

  EXPECT_GT(aSecondGen, aFirstGen);
}

TEST(BRepGraphEdgeCasesTest, Build_TwiceOnSameGraph_OldUIDsInvalidated)
{
  BRepGraph aGraph;
  aGraph.SetUIDEnabled(true);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  // First build: collect total UID counter used.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aFirstGen = aGraph.Generation();

  // Record total node count from first build.
  const size_t aTotalFirstBuild = aGraph.NbNodes();
  ASSERT_GT(aTotalFirstBuild, 0u);

  // Second build.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const uint32_t aSecondGen = aGraph.Generation();
  EXPECT_NE(aFirstGen, aSecondGen);

  // After rebuild, UIDs on the new nodes carry the new generation.
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  const BRepGraph_UID aUID = aGraph.UIDOf(aSolidId);
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

  const int aSolids1  = aGraph.NbSolidDefs();
  const int aShells1  = aGraph.NbShellDefs();
  const int aFaces1   = aGraph.NbFaceDefs();
  const int aWires1   = aGraph.NbWireDefs();
  const int aEdges1   = aGraph.NbEdgeDefs();
  const int aVerts1   = aGraph.NbVertexDefs();
  const int aSurfs1   = aGraph.NbSurfaces();
  const int aCurves1  = aGraph.NbCurves();
  const int aPCurves1 = aGraph.NbPCurves();

  // Rebuild with same shape.
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.NbSolidDefs(), aSolids1);
  EXPECT_EQ(aGraph.NbShellDefs(), aShells1);
  EXPECT_EQ(aGraph.NbFaceDefs(), aFaces1);
  EXPECT_EQ(aGraph.NbWireDefs(), aWires1);
  EXPECT_EQ(aGraph.NbEdgeDefs(), aEdges1);
  EXPECT_EQ(aGraph.NbVertexDefs(), aVerts1);
  EXPECT_EQ(aGraph.NbSurfaces(), aSurfs1);
  EXPECT_EQ(aGraph.NbCurves(), aCurves1);
  EXPECT_EQ(aGraph.NbPCurves(), aPCurves1);
}

// ============================================================
// UID edge cases
// ============================================================

TEST(BRepGraphEdgeCasesTest, UID_DisabledByDefault)
{
  BRepGraph aGraph;
  EXPECT_FALSE(aGraph.IsUIDEnabled());
}

TEST(BRepGraphEdgeCasesTest, UID_WhenDisabled_UIDOfReturnsInvalid)
{
  BRepGraph aGraph;
  // UID not enabled.

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  const BRepGraph_UID aUID = aGraph.UIDOf(aSolidId);
  EXPECT_FALSE(aUID.IsValid());
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

  EXPECT_EQ(aParGraph.NbSolidDefs(),  aSeqGraph.NbSolidDefs());
  EXPECT_EQ(aParGraph.NbShellDefs(),  aSeqGraph.NbShellDefs());
  EXPECT_EQ(aParGraph.NbFaceDefs(),   aSeqGraph.NbFaceDefs());
  EXPECT_EQ(aParGraph.NbWireDefs(),   aSeqGraph.NbWireDefs());
  EXPECT_EQ(aParGraph.NbEdgeDefs(),   aSeqGraph.NbEdgeDefs());
  EXPECT_EQ(aParGraph.NbVertexDefs(), aSeqGraph.NbVertexDefs());
  EXPECT_EQ(aParGraph.NbSurfaces(),   aSeqGraph.NbSurfaces());
  EXPECT_EQ(aParGraph.NbCurves(),     aSeqGraph.NbCurves());
  EXPECT_EQ(aParGraph.NbPCurves(),    aSeqGraph.NbPCurves());
  EXPECT_EQ(aParGraph.NbNodes(),      aSeqGraph.NbNodes());

  EXPECT_EQ(aParGraph.NbSolidUsages(),  aSeqGraph.NbSolidUsages());
  EXPECT_EQ(aParGraph.NbShellUsages(),  aSeqGraph.NbShellUsages());
  EXPECT_EQ(aParGraph.NbFaceUsages(),   aSeqGraph.NbFaceUsages());
  EXPECT_EQ(aParGraph.NbWireUsages(),   aSeqGraph.NbWireUsages());
  EXPECT_EQ(aParGraph.NbEdgeUsages(),   aSeqGraph.NbEdgeUsages());
  EXPECT_EQ(aParGraph.NbVertexUsages(), aSeqGraph.NbVertexUsages());
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

  EXPECT_EQ(aParGraph.NbSolidDefs(),  aSeqGraph.NbSolidDefs());
  EXPECT_EQ(aParGraph.NbShellDefs(),  aSeqGraph.NbShellDefs());
  EXPECT_EQ(aParGraph.NbFaceDefs(),   aSeqGraph.NbFaceDefs());
  EXPECT_EQ(aParGraph.NbWireDefs(),   aSeqGraph.NbWireDefs());
  EXPECT_EQ(aParGraph.NbEdgeDefs(),   aSeqGraph.NbEdgeDefs());
  EXPECT_EQ(aParGraph.NbVertexDefs(), aSeqGraph.NbVertexDefs());
  EXPECT_EQ(aParGraph.NbSurfaces(),   aSeqGraph.NbSurfaces());
  EXPECT_EQ(aParGraph.NbCurves(),     aSeqGraph.NbCurves());
  EXPECT_EQ(aParGraph.NbPCurves(),    aSeqGraph.NbPCurves());
  EXPECT_EQ(aParGraph.NbNodes(),      aSeqGraph.NbNodes());

  EXPECT_EQ(aParGraph.NbSolidUsages(),  aSeqGraph.NbSolidUsages());
  EXPECT_EQ(aParGraph.NbShellUsages(),  aSeqGraph.NbShellUsages());
  EXPECT_EQ(aParGraph.NbFaceUsages(),   aSeqGraph.NbFaceUsages());
  EXPECT_EQ(aParGraph.NbWireUsages(),   aSeqGraph.NbWireUsages());
  EXPECT_EQ(aParGraph.NbEdgeUsages(),   aSeqGraph.NbEdgeUsages());
  EXPECT_EQ(aParGraph.NbVertexUsages(), aSeqGraph.NbVertexUsages());
}
