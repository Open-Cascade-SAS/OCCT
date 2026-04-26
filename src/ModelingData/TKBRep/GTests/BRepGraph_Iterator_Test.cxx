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

#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

class BRepGraph_IteratorTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 = BRepGraph_Builder::Add(myGraph, aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_IteratorTest, FaceIterator_CountMatchesTopology)
{
  int aCount = 0;
  for (BRepGraph_FaceIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}

TEST_F(BRepGraph_IteratorTest, EdgeIterator_CountMatchesTopology)
{
  int aCount = 0;
  for (BRepGraph_EdgeIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 12);
}

TEST_F(BRepGraph_IteratorTest, VertexIterator_CountMatchesTopology)
{
  int aCount = 0;
  for (BRepGraph_VertexIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 8);
}

TEST_F(BRepGraph_IteratorTest, SolidIterator_BoxHasOneSolid)
{
  int aCount = 0;
  for (BRepGraph_SolidIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_IteratorTest, ShellIterator_BoxHasOneShell)
{
  int aCount = 0;
  for (BRepGraph_ShellIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_IteratorTest, WireIterator_BoxHasSixWires)
{
  int aCount = 0;
  for (BRepGraph_WireIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}

TEST_F(BRepGraph_IteratorTest, RootProductIterator_MatchesStoredRoots)
{
  int aCount = 0;
  for (BRepGraph_RootProductIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ASSERT_LT(aCount, myGraph.RootProductIds().Length());
    EXPECT_EQ(anIt.Current(), myGraph.RootProductIds().Value(aCount));
    ++aCount;
  }
  EXPECT_EQ(aCount, myGraph.RootProductIds().Length());
}

TEST_F(BRepGraph_IteratorTest, CurrentId_ReturnsValidTypedIds)
{
  BRepGraph_FaceIterator anIt(myGraph);
  ASSERT_TRUE(anIt.More());
  const BRepGraph_FaceId aFaceId = anIt.CurrentId();
  EXPECT_TRUE(aFaceId.IsValid(myGraph.Topo().Faces().Nb()));
}

TEST_F(BRepGraph_IteratorTest, Current_ReturnsDefinition)
{
  BRepGraph_VertexIterator anIt(myGraph);
  ASSERT_TRUE(anIt.More());
  const BRepGraphInc::VertexDef& aVtx = anIt.Current();
  // Vertex should have a valid point (box vertex coordinates are finite).
  EXPECT_TRUE(std::isfinite(aVtx.Point.X()));
  EXPECT_TRUE(std::isfinite(aVtx.Point.Y()));
  EXPECT_TRUE(std::isfinite(aVtx.Point.Z()));
}

TEST_F(BRepGraph_IteratorTest, RemovedFace_SkippedByDefaultIterator)
{
  const int aNbBefore = myGraph.Topo().Faces().Nb();
  myGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());

  int aCount = 0;
  for (BRepGraph_FaceIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbBefore - 1);
}

TEST_F(BRepGraph_IteratorTest, FullTraverse_IncludesRemovedFace)
{
  const int aNbBefore = myGraph.Topo().Faces().Nb();
  myGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());

  int aCount = 0;
  for (BRepGraph_FullFaceIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbBefore);
}

TEST_F(BRepGraph_IteratorTest, RangeFor_WorksCorrectly)
{
  int aCount = 0;
  for (const BRepGraphInc::FaceDef& aFace : BRepGraph_FaceIterator(myGraph))
  {
    (void)aFace;
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}

TEST(BRepGraph_IteratorStandalone, EmptyGraph_IteratorIsEmpty)
{
  BRepGraph aGraph;
  EXPECT_FALSE(BRepGraph_FaceIterator(aGraph).More());
  EXPECT_FALSE(BRepGraph_EdgeIterator(aGraph).More());
  EXPECT_FALSE(BRepGraph_VertexIterator(aGraph).More());
  EXPECT_FALSE(BRepGraph_SolidIterator(aGraph).More());
}

TEST_F(BRepGraph_IteratorTest, CoEdgeIterator_CountMatchesTopology)
{
  int aCount = 0;
  for (BRepGraph_CoEdgeIterator anIt(myGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  // A box has 6 faces x 4 edges/face = 24 coedges.
  EXPECT_EQ(aCount, 24);
}
