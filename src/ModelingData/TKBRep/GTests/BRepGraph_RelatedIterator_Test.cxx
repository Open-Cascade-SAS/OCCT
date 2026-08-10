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
#include <BRepGraph_RelatedIterator.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
static bool hasRelatedNode(const BRepGraph&                              theGraph,
                           const BRepGraph_NodeId                        theSource,
                           const BRepGraph_NodeId&                       theNode,
                           const BRepGraph_RelatedIterator::RelationKind theRelation)
{
  for (BRepGraph_RelatedIterator anIt(theGraph, theSource); anIt.More(); anIt.Next())
  {
    if (anIt.Current() == theNode && anIt.CurrentRelation() == theRelation)
    {
      return true;
    }
  }
  return false;
}

static uint32_t countRelations(const BRepGraph&                              theGraph,
                               const BRepGraph_NodeId                        theNode,
                               const BRepGraph_RelatedIterator::RelationKind theRelation)
{
  uint32_t aCount = 0;
  for (BRepGraph_RelatedIterator anIt(theGraph, theNode); anIt.More(); anIt.Next())
  {
    if (anIt.CurrentRelation() == theRelation)
    {
      ++aCount;
    }
  }
  return aCount;
}
} // namespace

class BRepGraph_RelatedIteratorTest : public testing::Test
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

TEST_F(BRepGraph_RelatedIteratorTest, FaceOfBox_ReturnsBoundaryEdgesAndOuterWire)
{
  const BRepGraph_FaceId aFaceId(0);

  const uint32_t aBoundaryEdgeCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(aFaceId),
                   BRepGraph_RelatedIterator::RelationKind::BoundaryEdge);
  const uint32_t anAdjacentFaceCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(aFaceId),
                   BRepGraph_RelatedIterator::RelationKind::AdjacentFace);
  const uint32_t anOuterWireCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(aFaceId),
                   BRepGraph_RelatedIterator::RelationKind::OuterWire);

  EXPECT_EQ(aBoundaryEdgeCount, 4);
  EXPECT_EQ(anAdjacentFaceCount, 4);
  EXPECT_EQ(anOuterWireCount, 1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(aFaceId),
                             BRepGraph_NodeId(BRepGraph_WireId::Start()),
                             BRepGraph_RelatedIterator::RelationKind::OuterWire));
}

TEST_F(BRepGraph_RelatedIteratorTest, EdgeOfBox_ReturnsIncidentVerticesAndFaces)
{
  const BRepGraph_EdgeId anEdgeId(0);

  const uint32_t aVertexCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(anEdgeId),
                   BRepGraph_RelatedIterator::RelationKind::IncidentVertex);
  const uint32_t aFaceCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(anEdgeId),
                   BRepGraph_RelatedIterator::RelationKind::ReferencedByFace);

  EXPECT_EQ(aVertexCount, 2);
  EXPECT_EQ(aFaceCount, 2);
}

TEST_F(BRepGraph_RelatedIteratorTest, AssemblyNodes_YieldNoRelations)
{
  // Assembly/container nodes have no topological relations - use ChildExplorer instead.
  const BRepGraph_ProductId aPartProduct =
    myGraph.Editor().Products().Add(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  myGraph.Editor().Products().AppendDocumentRoot(aPartProduct);
  const BRepGraph_ProductId aRootAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  const BRepGraph_OccurrenceId anOccurrenceId =
    myGraph.Editor().Products().Append(aRootAssembly, aPartProduct, TopLoc_Location(aTrsf));
  ASSERT_TRUE(anOccurrenceId.IsValid());

  BRepGraph_RelatedIterator aProductIt(myGraph, BRepGraph_NodeId(aRootAssembly));
  EXPECT_FALSE(aProductIt.More());

  BRepGraph_RelatedIterator anOccIt(myGraph, BRepGraph_NodeId(anOccurrenceId));
  EXPECT_FALSE(anOccIt.More());
}

TEST_F(BRepGraph_RelatedIteratorTest, InvalidNode_ReturnsEmpty)
{
  BRepGraph_RelatedIterator anIt(myGraph, BRepGraph_NodeId());
  EXPECT_FALSE(anIt.More());
}

TEST_F(BRepGraph_RelatedIteratorTest, EdgeReferencedByFace_RemovedFaceIsSkipped)
{
  // A box edge is shared by exactly 2 faces. Removing one face should leave
  // exactly 1 ReferencedByFace relation for the edge. This specifically
  // tests that removed faces are not skipped incorrectly (no double-increment).
  const BRepGraph_EdgeId anEdgeId(0);

  // Find the two faces that reference this edge.
  BRepGraph_FacesOfEdge aFaceIt = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(aFaceIt.More());
  const BRepGraph_FaceId aRemovedFace = aFaceIt.CurrentId();
  aFaceIt.Next();
  ASSERT_TRUE(aFaceIt.More());
  const BRepGraph_FaceId aSurvivor = aFaceIt.CurrentId();
  aFaceIt.Next();
  ASSERT_FALSE(aFaceIt.More());

  // Remove the first face.
  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aRemovedFace));

  // The edge should report exactly 1 ReferencedByFace (the surviving face).
  const uint32_t aFaceCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(anEdgeId),
                   BRepGraph_RelatedIterator::RelationKind::ReferencedByFace);
  EXPECT_EQ(aFaceCount, 1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(anEdgeId),
                             BRepGraph_NodeId(aSurvivor),
                             BRepGraph_RelatedIterator::RelationKind::ReferencedByFace));
}

TEST_F(BRepGraph_RelatedIteratorTest, ContainerNodes_YieldNoRelations)
{
  // Solid, Shell are containers - no topological relations.
  BRepGraph_RelatedIterator aSolidIt(myGraph, BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  EXPECT_FALSE(aSolidIt.More());

  BRepGraph_RelatedIterator aShellIt(myGraph, BRepGraph_NodeId(BRepGraph_ShellId::Start()));
  EXPECT_FALSE(aShellIt.More());
}

TEST_F(BRepGraph_RelatedIteratorTest, WireOfBox_ReturnsCoEdges)
{
  const BRepGraph_WireId aWireId(0);
  const uint32_t         aCount = countRelations(myGraph,
                                         BRepGraph_NodeId(aWireId),
                                         BRepGraph_RelatedIterator::RelationKind::WireCoEdge);
  EXPECT_EQ(aCount, 4);
}

TEST_F(BRepGraph_RelatedIteratorTest, WireOfBox_ReturnsOwningFace)
{
  const BRepGraph_WireId aWireId(0);
  const uint32_t         aCount = countRelations(myGraph,
                                         BRepGraph_NodeId(aWireId),
                                         BRepGraph_RelatedIterator::RelationKind::OwningFace);
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraph_RelatedIteratorTest, VertexOfBox_ReturnsIncidentEdges)
{
  const BRepGraph_VertexId aVertexId(0);
  const uint32_t           aCount = countRelations(myGraph,
                                         BRepGraph_NodeId(aVertexId),
                                         BRepGraph_RelatedIterator::RelationKind::IncidentEdge);
  // Each vertex of a box touches exactly 3 edges.
  EXPECT_EQ(aCount, 3);
}

TEST_F(BRepGraph_RelatedIteratorTest, CoEdgeOfBox_ReturnsParentEdgeAndOwningFace)
{
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  const uint32_t           aParentEdgeCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(aCoEdgeId),
                   BRepGraph_RelatedIterator::RelationKind::ParentEdge);
  const uint32_t aOwningFaceCount =
    countRelations(myGraph,
                   BRepGraph_NodeId(aCoEdgeId),
                   BRepGraph_RelatedIterator::RelationKind::OwningFace);
  EXPECT_EQ(aParentEdgeCount, 1);
  EXPECT_EQ(aOwningFaceCount, 1);
}

TEST(BRepGraph_RelatedIteratorStandalone, Compound_YieldsNoRelations)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  aBuilder.Add(aCompound, BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_CompoundId aCompoundId;
  for (BRepGraph_CompoundIterator anIt(aGraph); anIt.More(); anIt.Next())
  {
    aCompoundId = anIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aCompoundId.IsValid());

  BRepGraph_RelatedIterator anIt(aGraph, BRepGraph_NodeId(aCompoundId));
  EXPECT_FALSE(anIt.More());
}

TEST_F(BRepGraph_RelatedIteratorTest, RangeFor_WorksCorrectly)
{
  const BRepGraph_FaceId aFaceId(0);
  uint32_t               aCount = 0;
  for (const BRepGraph_NodeId& aNode :
       BRepGraph_RelatedIterator(myGraph, BRepGraph_NodeId(aFaceId)))
  {
    EXPECT_TRUE(aNode.IsValid());
    ++aCount;
  }
  // Face has 4 boundary edges + adjacent faces + 1 outer wire = at least 9.
  EXPECT_GE(aCount, 9);
}

TEST_F(BRepGraph_RelatedIteratorTest, EdgeOfBox_AllRelationsSequential)
{
  // Edge 0 of a box is shared by 2 faces and has 2 vertices.
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraph_NodeId anEdgeNode(anEdgeId);

  NCollection_DynamicArray<BRepGraph_NodeId> aFaces;
  NCollection_DynamicArray<BRepGraph_NodeId> aVertices;

  for (BRepGraph_RelatedIterator anIt(myGraph, anEdgeNode); anIt.More(); anIt.Next())
  {
    if (anIt.CurrentRelation() == BRepGraph_RelatedIterator::RelationKind::ReferencedByFace)
    {
      aFaces.Append(anIt.Current());
    }
    else if (anIt.CurrentRelation() == BRepGraph_RelatedIterator::RelationKind::IncidentVertex)
    {
      aVertices.Append(anIt.Current());
    }
  }

  EXPECT_EQ(aFaces.Size(), 2);
  EXPECT_EQ(aVertices.Size(), 2);
  // All yielded nodes must be distinct.
  if (aFaces.Size() == 2)
  {
    EXPECT_NE(aFaces.Value(0), aFaces.Value(1));
  }
  if (aVertices.Size() == 2)
  {
    EXPECT_NE(aVertices.Value(0), aVertices.Value(1));
  }
}

TEST_F(BRepGraph_RelatedIteratorTest, EdgeOfBox_RemovedFace_CorrectTransition)
{
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraph_NodeId anEdgeNode(anEdgeId);

  // Remove one parent face.
  BRepGraph_FacesOfEdge aParentFaceIt = myGraph.Topo().Edges().FacesOf(anEdgeId);
  ASSERT_TRUE(aParentFaceIt.More());
  const BRepGraph_FaceId aRemovedFace = aParentFaceIt.CurrentId();
  aParentFaceIt.Next();
  ASSERT_TRUE(aParentFaceIt.More());
  aParentFaceIt.Next();
  ASSERT_FALSE(aParentFaceIt.More());
  myGraph.Editor().Gen().RemoveNode(aRemovedFace);

  // Iterate - expect 1 face + 2 vertices, confirming correct stage transition.
  const uint32_t aFaceCount =
    countRelations(myGraph, anEdgeNode, BRepGraph_RelatedIterator::RelationKind::ReferencedByFace);
  const uint32_t aVertexCount =
    countRelations(myGraph, anEdgeNode, BRepGraph_RelatedIterator::RelationKind::IncidentVertex);
  EXPECT_EQ(aFaceCount, 1);
  EXPECT_EQ(aVertexCount, 2);
}

TEST_F(BRepGraph_RelatedIteratorTest, VertexOfBox_AllParentEdgesYielded)
{
  // Vertex 0 of a box is touched by 3 edges.
  const BRepGraph_VertexId aVertexId(0);
  const BRepGraph_NodeId   aVertexNode(aVertexId);

  NCollection_DynamicArray<BRepGraph_NodeId> anEdges;
  for (BRepGraph_RelatedIterator anIt(myGraph, aVertexNode); anIt.More(); anIt.Next())
  {
    if (anIt.CurrentRelation() == BRepGraph_RelatedIterator::RelationKind::IncidentEdge)
    {
      anEdges.Append(anIt.Current());
    }
  }

  EXPECT_EQ(anEdges.Size(), 3);
  // All yielded edges must be distinct.
  for (size_t i = 0; i < anEdges.Size(); ++i)
  {
    for (size_t j = i + 1; j < anEdges.Size(); ++j)
    {
      EXPECT_NE(anEdges.Value(i), anEdges.Value(j));
    }
  }
}
