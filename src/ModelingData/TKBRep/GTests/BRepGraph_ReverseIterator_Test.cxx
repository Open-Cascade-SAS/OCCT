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
#include <BRepGraph_ShapesView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_LinearVector.hxx>
#include <gp_Pnt.hxx>
#include <TopAbs.hxx>

#include <gtest/gtest.h>

namespace
{
template <typename IteratorT>
static uint32_t countIterator(IteratorT theIterator)
{
  uint32_t aCount = 0;
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
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
      myGraph.Shapes().Add(aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_ReverseIteratorTest, FacesOfEdge_BoxEdgeSharedByTwoFaces)
{
  // Every edge of a box is shared by exactly 2 faces.
  const BRepGraph_EdgeId anEdgeId(0);
  const uint32_t         aCount = countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, EdgesOfVertex_BoxVertexSharedByThreeEdges)
{
  // Every vertex of a box is shared by exactly 3 edges.
  const BRepGraph_VertexId aVertexId(0);
  const uint32_t           aCount =
    countIterator(BRepGraph_EdgesOfVertex(myGraph, myGraph.Topo().Vertices().Edges(aVertexId)));
  EXPECT_EQ(aCount, 3);
}

TEST_F(BRepGraph_ReverseIteratorTest, SolidsOfShell_BoxShellHasOneSolid)
{
  const BRepGraph_ShellId aShellId(0);
  const uint32_t          aCount = countIterator(
    BRepGraph_SolidsOfShell(myGraph,
                            myGraph.Topo().Shells().Relations(aShellId).ParentShellRefIds));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, ShellsOfFace_BoxFaceHasOneShell)
{
  const BRepGraph_FaceId aFaceId(0);
  const uint32_t         aCount = countIterator(
    BRepGraph_ShellsOfFace(myGraph, myGraph.Topo().Faces().Relations(aFaceId).ParentFaceRefIds));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, FacesOfWire_BoxWireHasOneFace)
{
  const BRepGraph_WireId aWireId(0);
  const uint32_t         aCount = countIterator(
    BRepGraph_FacesOfWire(myGraph, myGraph.Topo().Wires().Relations(aWireId).ParentWireRefIds));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, WiresOfEdge_BoxEdgeBelongsToTwoWires)
{
  const BRepGraph_EdgeId anEdgeId(0);
  const uint32_t         aCount = countIterator(myGraph.Topo().Edges().WiresOf(anEdgeId));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, SequentialIteration_SkipsRemovedParent)
{
  const BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FacesOfEdge  aFaces = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(aFaces.More());
  const BRepGraph_FaceId aFirstFace = aFaces.CurrentId();
  EXPECT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)), 2);

  // Remove one parent face.
  myGraph.Editor().Gen().RemoveNode(aFirstFace);

  const uint32_t aCount = countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId));
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, IndexedAccess_TracksActiveReverseBucket)
{
  const BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FacesOfEdge  aFaces = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(aFaces.More());
  const BRepGraph_FaceId aFirstFace = aFaces.CurrentId();
  EXPECT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)), 2);

  myGraph.Editor().Gen().RemoveNode(aFirstFace);

  BRepGraph_FacesOfEdge anIt = myGraph.Topo().Edges().FacesOf(anEdgeId);
  // Reverse buckets are maintained as active relationships after removals.
  ASSERT_TRUE(anIt.More());
  EXPECT_TRUE(anIt.CurrentId().IsValid(myGraph.Topo().Faces().Nb()));
  anIt.Next();
  EXPECT_FALSE(anIt.More());
}

TEST_F(BRepGraph_ReverseIteratorTest, RangeFor_WorksCorrectly)
{
  const BRepGraph_EdgeId anEdgeId(0);
  uint32_t               aCount = 0;
  for (const BRepGraph_FaceId aFaceId : myGraph.Topo().Edges().FacesOf(anEdgeId))
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
  BRepGraph_RefsShellsOfFace anIt(myGraph,
                                  myGraph.Topo().Faces().Relations(aFaceId).ParentFaceRefIds);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.CurrentParentId(), aShellId);
  EXPECT_TRUE(anIt.CurrentRefId().IsValid());
}

TEST_F(BRepGraph_ReverseIteratorTest, RefsEdgesOfVertex_ReturnsValidRefId)
{
  const BRepGraph_VertexId aVertexId(0);
  uint32_t                 aCount = 0;
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
  const uint32_t         aCount =
    countIterator(BRepGraph_CoEdgesOfEdge(myGraph, myGraph.Topo().Edges().CoEdges(anEdgeId)));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, WiresOfCoEdge_BoxCoEdgeBelongsToOneWire)
{
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  EXPECT_TRUE(myGraph.Topo().CoEdges().Wire(aCoEdgeId).IsValid());
}

TEST_F(BRepGraph_ReverseIteratorTest, Definition_ReturnsFaceDefinition)
{
  const BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FacesOfEdge  anIt = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(anIt.More());

  const BRepGraph_FaceId aFaceId = anIt.CurrentId();
  EXPECT_FALSE(anIt.CurrentId().IsRemoved(myGraph));
  // Face must have at least one wire.
  EXPECT_GE(myGraph.Topo().Faces().Relations(aFaceId).WireRefIds.Size(), 1);
}

TEST_F(BRepGraph_ReverseIteratorTest, Definition_ReturnsEdgeDefinition)
{
  const BRepGraph_VertexId aVertexId(0);
  BRepGraph_EdgesOfVertex  anIt(myGraph, myGraph.Topo().Vertices().Edges(aVertexId));
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::EdgeDef& anEdgeDef = anIt.Definition();
  EXPECT_FALSE(anIt.CurrentId().IsRemoved(myGraph));
  EXPECT_TRUE(anEdgeDef.StartVertexRefId.IsValid());
  EXPECT_TRUE(anEdgeDef.EndVertexRefId.IsValid());
}

TEST_F(BRepGraph_ReverseIteratorTest, SkipsRemovedParent_EdgesOfVertex)
{
  const BRepGraph_VertexId                          aVertexId(0);
  const NCollection_LinearVector<BRepGraph_EdgeId>& anEdges =
    myGraph.Topo().Vertices().Edges(aVertexId);
  // Box vertex touches 3 edges.
  ASSERT_EQ(anEdges.Size(), 3);

  myGraph.Editor().Gen().RemoveNode(anEdges.Value(0));

  const uint32_t aCount = countIterator(BRepGraph_EdgesOfVertex(myGraph, anEdges));
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, SkipsRemovedParent_AllRemoved)
{
  const BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FacesOfEdge  aFaces = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(aFaces.More());
  const BRepGraph_FaceId aFirstFace = aFaces.CurrentId();
  aFaces.Next();
  ASSERT_TRUE(aFaces.More());
  const BRepGraph_FaceId aSecondFace = aFaces.CurrentId();
  EXPECT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)), 2);

  myGraph.Editor().Gen().RemoveNode(aFirstFace);
  myGraph.Editor().Gen().RemoveNode(aSecondFace);

  const uint32_t aCount = countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId));
  EXPECT_EQ(aCount, 0);
}

TEST_F(BRepGraph_ReverseIteratorTest, StartingIndex_SkipsToPosition)
{
  const BRepGraph_VertexId                          aVertexId(0);
  const NCollection_LinearVector<BRepGraph_EdgeId>& anEdges =
    myGraph.Topo().Vertices().Edges(aVertexId);
  ASSERT_EQ(anEdges.Size(), 3);

  // Start at index 1 - should yield only edges at index >= 1.
  BRepGraph_EdgesOfVertex anIt(myGraph, anEdges, 1);
  const uint32_t          aCount = countIterator(anIt);
  EXPECT_EQ(aCount, 2);
}

TEST_F(BRepGraph_ReverseIteratorTest, CompoundsOfChild_AllTopologyKinds)
{
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());

  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(anEdge.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIds;
  aCoEdgeIds.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdgeIds.ToArray1());
  ASSERT_TRUE(aWire.IsValid());
  ASSERT_EQ(aGraph.Topo().Edges().CoEdges(anEdge).Size(), 1);
  const BRepGraph_CoEdgeId aCoEdge = aGraph.Topo().Edges().CoEdges(anEdge).First();

  const BRepGraph_FaceId aFace = aGraph.Editor().Faces().Add(occ::handle<Geom_Surface>(),
                                                             aWire,
                                                             NCollection_Array1<BRepGraph_WireId>(),
                                                             1.0e-7);
  ASSERT_TRUE(aFace.IsValid());
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());
  ASSERT_TRUE(aGraph.Editor().Shells().Append(aShell, aFace).IsValid());
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid.IsValid());
  ASSERT_TRUE(aGraph.Editor().Solids().Append(aSolid, aShell).IsValid());

  NCollection_LinearVector<BRepGraph_SolidId> aSolids;
  aSolids.Append(aSolid);
  const BRepGraph_CompSolidId aCompSolid = aGraph.Editor().CompSolids().Add(aSolids.ToArray1());
  ASSERT_TRUE(aCompSolid.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aNestedChildren;
  aNestedChildren.Append(aSolid);
  const BRepGraph_CompoundId aNestedCompound =
    aGraph.Editor().Compounds().Add(aNestedChildren.ToArray1());
  ASSERT_TRUE(aNestedCompound.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aV0);
  aChildren.Append(aCoEdge);
  aChildren.Append(anEdge);
  aChildren.Append(aWire);
  aChildren.Append(aFace);
  aChildren.Append(aShell);
  aChildren.Append(aSolid);
  aChildren.Append(aCompSolid);
  aChildren.Append(aNestedCompound);
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  BRepGraph_CompoundsOfVertex aVertexParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aV0)));
  ASSERT_TRUE(aVertexParents.More());
  EXPECT_EQ(aVertexParents.CurrentId(), aCompound);
  aVertexParents.Next();
  EXPECT_FALSE(aVertexParents.More());

  BRepGraph_CompoundsOfCoEdge aCoEdgeParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aCoEdge)));
  ASSERT_TRUE(aCoEdgeParents.More());
  EXPECT_EQ(aCoEdgeParents.CurrentId(), aCompound);
  aCoEdgeParents.Next();
  EXPECT_FALSE(aCoEdgeParents.More());

  BRepGraph_CompoundsOfEdge anEdgeParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(anEdge)));
  ASSERT_TRUE(anEdgeParents.More());
  EXPECT_EQ(anEdgeParents.CurrentId(), aCompound);
  anEdgeParents.Next();
  EXPECT_FALSE(anEdgeParents.More());

  BRepGraph_CompoundsOfWire aWireParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aWire)));
  ASSERT_TRUE(aWireParents.More());
  EXPECT_EQ(aWireParents.CurrentId(), aCompound);
  aWireParents.Next();
  EXPECT_FALSE(aWireParents.More());

  BRepGraph_CompoundsOfFace aFaceParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aFace)));
  ASSERT_TRUE(aFaceParents.More());
  EXPECT_EQ(aFaceParents.CurrentId(), aCompound);
  aFaceParents.Next();
  EXPECT_FALSE(aFaceParents.More());

  BRepGraph_CompoundsOfShell aShellParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aShell)));
  ASSERT_TRUE(aShellParents.More());
  EXPECT_EQ(aShellParents.CurrentId(), aCompound);
  aShellParents.Next();
  EXPECT_FALSE(aShellParents.More());

  BRepGraph_CompoundsOfSolid aSolidParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aSolid)));
  ASSERT_TRUE(aSolidParents.More());
  EXPECT_EQ(aSolidParents.CurrentId(), aNestedCompound);
  aSolidParents.Next();
  ASSERT_TRUE(aSolidParents.More());
  EXPECT_EQ(aSolidParents.CurrentId(), aCompound);
  aSolidParents.Next();
  EXPECT_FALSE(aSolidParents.More());

  BRepGraph_CompoundsOfCompSolid aCompSolidParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aCompSolid)));
  ASSERT_TRUE(aCompSolidParents.More());
  EXPECT_EQ(aCompSolidParents.CurrentId(), aCompound);
  aCompSolidParents.Next();
  EXPECT_FALSE(aCompSolidParents.More());

  BRepGraph_CompoundsOfCompound aCompoundParents(
    aGraph,
    aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aNestedCompound)));
  ASSERT_TRUE(aCompoundParents.More());
  EXPECT_EQ(aCompoundParents.CurrentId(), aCompound);
  aCompoundParents.Next();
  EXPECT_FALSE(aCompoundParents.More());

  BRepGraph_CompoundsOfChild aGenericParents(aGraph, aGraph.Topo().Gen().CompoundRefIds(aCoEdge));
  ASSERT_TRUE(aGenericParents.More());
  EXPECT_EQ(aGenericParents.CurrentId(), aCompound);
  aGenericParents.Next();
  EXPECT_FALSE(aGenericParents.More());

  BRepGraph_FacesOfWire aFaceParentsOfWire(aGraph,
                                           aGraph.Topo().Wires().Relations(aWire).ParentWireRefIds);
  ASSERT_TRUE(aFaceParentsOfWire.More());
  EXPECT_EQ(aFaceParentsOfWire.CurrentId(), aFace);
  aFaceParentsOfWire.Next();
  EXPECT_FALSE(aFaceParentsOfWire.More());
}

TEST_F(BRepGraph_ReverseIteratorTest, OccurrencesOfChild_ProductAndTopologyChildren)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(aCompound);
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  ASSERT_TRUE(aPart.IsValid());

  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(anAssembly.IsValid());
  const BRepGraph_OccurrenceId aPartOccurrence =
    aGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(aPartOccurrence.IsValid());

  BRepGraph_OccurrencesOfChild aTopologyOccurrences(
    aGraph,
    aGraph.Topo().Gen().OccurrenceRefIds(aCompound));
  ASSERT_TRUE(aTopologyOccurrences.More());
  const BRepGraph_OccurrenceId aCompoundOccurrence = aTopologyOccurrences.CurrentId();
  ASSERT_TRUE(aCompoundOccurrence.IsValid());
  EXPECT_EQ(aGraph.Topo().Occurrences().Definition(aCompoundOccurrence).ChildNodeId,
            BRepGraph_NodeId(aCompound));
  aTopologyOccurrences.Next();
  EXPECT_FALSE(aTopologyOccurrences.More());

  BRepGraph_OccurrencesOfChild aProductOccurrences(aGraph,
                                                   aGraph.Topo().Gen().OccurrenceRefIds(aPart));
  ASSERT_TRUE(aProductOccurrences.More());
  EXPECT_EQ(aProductOccurrences.CurrentId(), aPartOccurrence);
  aProductOccurrences.Next();
  EXPECT_FALSE(aProductOccurrences.More());
}
