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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_RelatedIterator.hxx>
#include <BRepGraph_TopoView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
static bool hasRelatedNode(const BRepGraph&                                theGraph,
                           const BRepGraph_NodeId                          theSource,
                           const BRepGraph_NodeId&                         theNode,
                           const BRepGraph_RelatedIterator::RelationKind   theRelation)
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

static int countRelations(const BRepGraph&                              theGraph,
                          const BRepGraph_NodeId                        theNode,
                          const BRepGraph_RelatedIterator::RelationKind theRelation)
{
  int aCount = 0;
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
    myGraph.Build(aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_RelatedIteratorTest, FaceOfBox_ReturnsBoundaryEdgesAndOuterWire)
{
  const BRepGraph_FaceId aFaceId(0);

  const int aBoundaryEdgeCount = countRelations(myGraph,
                                                BRepGraph_NodeId(aFaceId),
                                                BRepGraph_RelatedIterator::RelationKind::BoundaryEdge);
  const int anAdjacentFaceCount = countRelations(myGraph,
                                                 BRepGraph_NodeId(aFaceId),
                                                 BRepGraph_RelatedIterator::RelationKind::AdjacentFace);
  const int anOuterWireCount = countRelations(myGraph,
                                              BRepGraph_NodeId(aFaceId),
                                              BRepGraph_RelatedIterator::RelationKind::OuterWire);

  EXPECT_EQ(aBoundaryEdgeCount, 4);
  EXPECT_EQ(anAdjacentFaceCount, 4);
  EXPECT_EQ(anOuterWireCount, 1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(aFaceId),
                             BRepGraph_NodeId(BRepGraph_WireId(0)),
                             BRepGraph_RelatedIterator::RelationKind::OuterWire));
}

TEST_F(BRepGraph_RelatedIteratorTest, EdgeOfBox_ReturnsIncidentVerticesAndFaces)
{
  const BRepGraph_EdgeId anEdgeId(0);

  const int aVertexCount = countRelations(myGraph,
                                          BRepGraph_NodeId(anEdgeId),
                                          BRepGraph_RelatedIterator::RelationKind::IncidentVertex);
  const int aFaceCount = countRelations(myGraph,
                                        BRepGraph_NodeId(anEdgeId),
                                        BRepGraph_RelatedIterator::RelationKind::ReferencedByFace);

  EXPECT_EQ(aVertexCount, 2);
  EXPECT_EQ(aFaceCount, 2);
}

TEST_F(BRepGraph_RelatedIteratorTest, ProductAndOccurrence_ReturnExpectedAssemblyLinks)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Builder().AddProduct(BRepGraph_NodeId(BRepGraph_SolidId(0)));
  const BRepGraph_ProductId aRootAssembly = myGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(aRootAssembly.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  const BRepGraph_OccurrenceId anOccurrenceId =
    myGraph.Builder().AddOccurrence(aRootAssembly, aPartProduct, TopLoc_Location(aTrsf));
  ASSERT_TRUE(anOccurrenceId.IsValid());

  EXPECT_EQ(countRelations(myGraph,
                           BRepGraph_NodeId(aRootAssembly),
                           BRepGraph_RelatedIterator::RelationKind::ChildOccurrence),
            1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(aRootAssembly),
                             BRepGraph_NodeId(anOccurrenceId),
                             BRepGraph_RelatedIterator::RelationKind::ChildOccurrence));

  EXPECT_EQ(countRelations(myGraph,
                           BRepGraph_NodeId(anOccurrenceId),
                           BRepGraph_RelatedIterator::RelationKind::ReferencedProduct),
            1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(anOccurrenceId),
                             BRepGraph_NodeId(aPartProduct),
                             BRepGraph_RelatedIterator::RelationKind::ReferencedProduct));

  EXPECT_EQ(countRelations(myGraph,
                           BRepGraph_NodeId(anOccurrenceId),
                           BRepGraph_RelatedIterator::RelationKind::ParentProduct),
            1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(anOccurrenceId),
                             BRepGraph_NodeId(aRootAssembly),
                             BRepGraph_RelatedIterator::RelationKind::ParentProduct));
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
  const NCollection_Vector<BRepGraph_FaceId>& aFaces =
    myGraph.Topo().Edges().Faces(anEdgeId);
  ASSERT_EQ(aFaces.Length(), 2);

  // Remove the first face.
  const BRepGraph_FaceId aRemovedFace = aFaces.Value(0);
  const BRepGraph_FaceId aSurvivor    = aFaces.Value(1);
  myGraph.Builder().RemoveNode(BRepGraph_NodeId(aRemovedFace));

  // The edge should report exactly 1 ReferencedByFace (the surviving face).
  const int aFaceCount = countRelations(myGraph,
                                        BRepGraph_NodeId(anEdgeId),
                                        BRepGraph_RelatedIterator::RelationKind::ReferencedByFace);
  EXPECT_EQ(aFaceCount, 1);
  EXPECT_TRUE(hasRelatedNode(myGraph,
                             BRepGraph_NodeId(anEdgeId),
                             BRepGraph_NodeId(aSurvivor),
                             BRepGraph_RelatedIterator::RelationKind::ReferencedByFace));
}