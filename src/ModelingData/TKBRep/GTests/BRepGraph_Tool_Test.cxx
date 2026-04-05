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
#include <BRepGraph_Tool.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
static bool hasRelatedNode(const NCollection_Sequence<BRepGraph_Tool::RelatedEntity>& theItems,
                           const BRepGraph_NodeId&                                     theNode,
                           const char*                                                  theRelation)
{
  for (NCollection_Sequence<BRepGraph_Tool::RelatedEntity>::Iterator anIt(theItems); anIt.More(); anIt.Next())
  {
    if (anIt.Value().Node == theNode && anIt.Value().Relation == theRelation)
    {
      return true;
    }
  }
  return false;
}
} // namespace

class BRepGraph_ToolTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myGraph.Build(aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_ToolTest, Related_FaceOfBox_ReturnsBoundaryEdgesAndOuterWire)
{
  const BRepGraph_FaceId aFaceId(0);

  const NCollection_Sequence<BRepGraph_Tool::RelatedEntity> aItems =
    BRepGraph_Tool::Related(myGraph, BRepGraph_NodeId(aFaceId));

  EXPECT_TRUE(hasRelatedNode(aItems, BRepGraph_NodeId(BRepGraph_WireId(0)), "Outer wire"));

  int aBoundaryEdgeCount = 0;
  int anAdjacentFaceCount = 0;
  for (NCollection_Sequence<BRepGraph_Tool::RelatedEntity>::Iterator anIt(aItems); anIt.More(); anIt.Next())
  {
    if (anIt.Value().Relation == "Boundary edge")
    {
      ++aBoundaryEdgeCount;
    }
    if (anIt.Value().Relation == "Adjacent face")
    {
      ++anAdjacentFaceCount;
    }
  }

  EXPECT_EQ(aBoundaryEdgeCount, 4);
  EXPECT_EQ(anAdjacentFaceCount, 4);
}

TEST_F(BRepGraph_ToolTest, Related_EdgeOfBox_ReturnsIncidentVerticesAndFaces)
{
  const BRepGraph_EdgeId anEdgeId(0);

  const NCollection_Sequence<BRepGraph_Tool::RelatedEntity> aItems =
    BRepGraph_Tool::Related(myGraph, BRepGraph_NodeId(anEdgeId));

  int aVertexCount = 0;
  int aFaceCount   = 0;
  for (NCollection_Sequence<BRepGraph_Tool::RelatedEntity>::Iterator anIt(aItems); anIt.More(); anIt.Next())
  {
    if (anIt.Value().Relation == "Incident vertex")
    {
      ++aVertexCount;
    }
    if (anIt.Value().Relation == "Referenced by face")
    {
      ++aFaceCount;
    }
  }

  EXPECT_EQ(aVertexCount, 2);
  EXPECT_EQ(aFaceCount, 2);
}

TEST_F(BRepGraph_ToolTest, Related_ProductAndOccurrence_ReturnExpectedAssemblyLinks)
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

  const NCollection_Sequence<BRepGraph_Tool::RelatedEntity> aProductItems =
    BRepGraph_Tool::Related(myGraph, BRepGraph_NodeId(aRootAssembly));
  EXPECT_TRUE(hasRelatedNode(aProductItems,
                             BRepGraph_NodeId(anOccurrenceId),
                             "Child occurrence"));

  const NCollection_Sequence<BRepGraph_Tool::RelatedEntity> anOccurrenceItems =
    BRepGraph_Tool::Related(myGraph, BRepGraph_NodeId(anOccurrenceId));
  EXPECT_TRUE(hasRelatedNode(anOccurrenceItems,
                             BRepGraph_NodeId(aPartProduct),
                             "Referenced product"));
  EXPECT_TRUE(hasRelatedNode(anOccurrenceItems,
                             BRepGraph_NodeId(aRootAssembly),
                             "Parent product"));
}

TEST_F(BRepGraph_ToolTest, Related_InvalidNode_ReturnsEmpty)
{
  const NCollection_Sequence<BRepGraph_Tool::RelatedEntity> aItems =
    BRepGraph_Tool::Related(myGraph, BRepGraph_NodeId());
  EXPECT_TRUE(aItems.IsEmpty());
}