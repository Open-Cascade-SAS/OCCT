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
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

namespace
{
template <typename IteratorT>
static int countIterator(IteratorT theIterator)
{
  int aCount = 0;
  for (; theIterator.More(); theIterator.Next())
  {
    ++aCount;
  }
  return aCount;
}
} // namespace

class BRepGraph_ReverseIteratorTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    BRepGraph_Builder::Perform(myGraph, aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_ReverseIteratorTest, FacesOfEdge_BoxEdgeSharedByTwoFaces)
{
  // Every edge of a box is shared by exactly 2 faces.
  const BRepGraph_EdgeId anEdgeId(0);
  const int              aCount =
    countIterator(BRepGraph_FacesOfEdge(myGraph, myGraph.Topo().Edges().Faces(anEdgeId)));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, EdgesOfVertex_BoxVertexSharedByThreeEdges)
{
  // Every vertex of a box is shared by exactly 3 edges.
  const BRepGraph_VertexId aVertexId(0);
  const int                aCount =
    countIterator(BRepGraph_EdgesOfVertex(myGraph, myGraph.Topo().Vertices().Edges(aVertexId)));
  EXPECT_EQ(aCount, 3);
}

TEST_F(BRepGraph_ReverseIteratorTest, SolidsOfShell_BoxShellHasOneSolid)
{
  const BRepGraph_ShellId aShellId(0);
  const int               aCount =
    countIterator(BRepGraph_SolidsOfShell(myGraph, myGraph.Topo().Shells().Solids(aShellId)));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, ShellsOfFace_BoxFaceHasOneShell)
{
  const BRepGraph_FaceId aFaceId(0);
  const int              aCount =
    countIterator(BRepGraph_ShellsOfFace(myGraph, myGraph.Topo().Faces().Shells(aFaceId)));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, FacesOfWire_BoxWireHasOneFace)
{
  const BRepGraph_WireId aWireId(0);
  const int              aCount =
    countIterator(BRepGraph_FacesOfWire(myGraph, myGraph.Topo().Wires().Faces(aWireId)));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, WiresOfEdge_BoxEdgeBelongsToTwoWires)
{
  const BRepGraph_EdgeId anEdgeId(0);
  const int              aCount =
    countIterator(BRepGraph_WiresOfEdge(myGraph, myGraph.Topo().Edges().Wires(anEdgeId)));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, SequentialIteration_SkipsRemovedParent)
{
  const BRepGraph_EdgeId                      anEdgeId(0);
  const NCollection_Vector<BRepGraph_FaceId>& aFaces = myGraph.Topo().Edges().Faces(anEdgeId);
  ASSERT_EQ(aFaces.Length(), 2);

  // Remove one parent face.
  myGraph.Editor().Gen().RemoveNode(aFaces.Value(0));

  const int aCount = countIterator(BRepGraph_FacesOfEdge(myGraph, aFaces));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, IndexedAccess_DoesNotSkipRemoved)
{
  const BRepGraph_EdgeId                      anEdgeId(0);
  const NCollection_Vector<BRepGraph_FaceId>& aFaces = myGraph.Topo().Edges().Faces(anEdgeId);
  ASSERT_EQ(aFaces.Length(), 2);

  myGraph.Editor().Gen().RemoveNode(aFaces.Value(0));

  BRepGraph_FacesOfEdge anIt(myGraph, aFaces);
  // Length() returns raw count including removed.
  EXPECT_EQ(anIt.Length(), 2);
  // Value() returns the raw entry without filtering.
  EXPECT_TRUE(anIt.Value(0).IsValid(myGraph.Topo().Faces().Nb()));
  EXPECT_TRUE(anIt.Value(1).IsValid(myGraph.Topo().Faces().Nb()));
}

TEST_F(BRepGraph_ReverseIteratorTest, RangeFor_WorksCorrectly)
{
  const BRepGraph_EdgeId anEdgeId(0);
  int                    aCount = 0;
  for (const BRepGraph_FaceId aFaceId :
       BRepGraph_FacesOfEdge(myGraph, myGraph.Topo().Edges().Faces(anEdgeId)))
  {
    EXPECT_TRUE(aFaceId.IsValid(myGraph.Topo().Faces().Nb()));
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, RefsShellsOfFace_ReturnsValidRefId)
{
  const BRepGraph_FaceId     aFaceId(0);
  const BRepGraph_ShellId    aShellId(0);
  BRepGraph_RefsShellsOfFace anIt(myGraph, myGraph.Topo().Faces().Shells(aFaceId), aFaceId);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.CurrentParentId(), aShellId);
  EXPECT_TRUE(anIt.CurrentRefId().IsValid());
}

TEST_F(BRepGraph_ReverseIteratorTest, RefsEdgesOfVertex_ReturnsValidRefId)
{
  const BRepGraph_VertexId aVertexId(0);
  int                      aCount = 0;
  for (BRepGraph_RefsEdgesOfVertex anIt(myGraph,
                                        myGraph.Topo().Vertices().Edges(aVertexId),
                                        aVertexId);
       anIt.More();
       anIt.Next())
  {
    EXPECT_TRUE(anIt.CurrentParentId().IsValid(myGraph.Topo().Edges().Nb()));
    EXPECT_TRUE(anIt.CurrentRefId().IsValid());
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

TEST_F(BRepGraph_ReverseIteratorTest, CoEdgesOfEdge_BoxEdgeHasCoEdges)
{
  // Each edge of a box is used in 2 coedges (one per adjacent face).
  const BRepGraph_EdgeId anEdgeId(0);
  const int              aCount =
    countIterator(BRepGraph_CoEdgesOfEdge(myGraph, myGraph.Topo().Edges().CoEdges(anEdgeId)));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, WiresOfCoEdge_BoxCoEdgeBelongsToOneWire)
{
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  const int                aCount =
    countIterator(BRepGraph_WiresOfCoEdge(myGraph, myGraph.Topo().CoEdges().Wires(aCoEdgeId)));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, Definition_ReturnsFaceDefinition)
{
  const BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FacesOfEdge  anIt(myGraph, myGraph.Topo().Edges().Faces(anEdgeId));
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::FaceDef& aFaceDef = anIt.Definition();
  EXPECT_FALSE(aFaceDef.IsRemoved);
  // Face must have at least one wire.
  EXPECT_GE(aFaceDef.WireRefIds.Length(), 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, Definition_ReturnsEdgeDefinition)
{
  const BRepGraph_VertexId aVertexId(0);
  BRepGraph_EdgesOfVertex  anIt(myGraph, myGraph.Topo().Vertices().Edges(aVertexId));
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::EdgeDef& anEdgeDef = anIt.Definition();
  EXPECT_FALSE(anEdgeDef.IsRemoved);
  EXPECT_TRUE(anEdgeDef.StartVertexRefId.IsValid());
  EXPECT_TRUE(anEdgeDef.EndVertexRefId.IsValid());
}

TEST_F(BRepGraph_ReverseIteratorTest, SkipsRemovedParent_EdgesOfVertex)
{
  const BRepGraph_VertexId                    aVertexId(0);
  const NCollection_Vector<BRepGraph_EdgeId>& anEdges = myGraph.Topo().Vertices().Edges(aVertexId);
  // Box vertex touches 3 edges.
  ASSERT_EQ(anEdges.Length(), 3);

  myGraph.Editor().Gen().RemoveNode(anEdges.Value(0));

  const int aCount = countIterator(BRepGraph_EdgesOfVertex(myGraph, anEdges));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, SkipsRemovedParent_AllRemoved)
{
  const BRepGraph_EdgeId                      anEdgeId(0);
  const NCollection_Vector<BRepGraph_FaceId>& aFaces = myGraph.Topo().Edges().Faces(anEdgeId);
  ASSERT_EQ(aFaces.Length(), 2);

  myGraph.Editor().Gen().RemoveNode(aFaces.Value(0));
  myGraph.Editor().Gen().RemoveNode(aFaces.Value(1));

  const int aCount = countIterator(BRepGraph_FacesOfEdge(myGraph, aFaces));
  EXPECT_EQ(aCount, 0);
}

TEST_F(BRepGraph_ReverseIteratorTest, StartingIndex_SkipsToPosition)
{
  const BRepGraph_VertexId                    aVertexId(0);
  const NCollection_Vector<BRepGraph_EdgeId>& anEdges = myGraph.Topo().Vertices().Edges(aVertexId);
  ASSERT_EQ(anEdges.Length(), 3);

  // Start at index 1 — should yield only edges at index >= 1.
  BRepGraph_EdgesOfVertex anIt(myGraph, anEdges, 1);
  const int               aCount = countIterator(anIt);
  EXPECT_EQ(aCount, 2);
}
