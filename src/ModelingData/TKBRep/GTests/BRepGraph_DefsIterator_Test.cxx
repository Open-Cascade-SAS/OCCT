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
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <cmath>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
template <typename IteratorT>
static size_t countIterator(IteratorT theIterator)
{
  size_t aCount = 0;
  for (; theIterator.More(); theIterator.Next())
  {
    ++aCount;
  }
  return aCount;
}

static TopoDS_Edge makeEdgeWithInternalVertex()
{
  BRep_Builder            aBuilder;
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aMakeEdge.Edge();

  TopoDS_Vertex anInternalVertex;
  aBuilder.MakeVertex(anInternalVertex, gp_Pnt(5, 0, 0), Precision::Confusion());
  aBuilder.Add(anEdge, anInternalVertex.Oriented(TopAbs_INTERNAL));
  return anEdge;
}

static TopoDS_Face wrapEdgeInFace(const TopoDS_Edge& theEdge)
{
  BRep_Builder                  aBuilder;
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face                   aFace;
  aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, theEdge);
  aBuilder.Add(aFace, aWire);
  return aFace;
}
} // namespace

class BRepGraph_DefsIteratorTest : public testing::Test
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

TEST_F(BRepGraph_DefsIteratorTest, BoxHierarchy_TraversesSingleLevelChildren)
{
  ASSERT_EQ(countIterator(BRepGraph_DefsShellOfSolid(myGraph, BRepGraph_SolidId::Start())), 1);
  ASSERT_EQ(countIterator(BRepGraph_DefsFaceOfShell(myGraph, BRepGraph_ShellId::Start())), 6);
  ASSERT_EQ(countIterator(BRepGraph_DefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 1);
  ASSERT_EQ(countIterator(BRepGraph_DefsEdgeOfWire(myGraph, BRepGraph_WireId::Start())), 4);
  ASSERT_EQ(countIterator(BRepGraph_DefsCoEdgeOfWire(myGraph, BRepGraph_WireId::Start())), 4);
  ASSERT_EQ(countIterator(BRepGraph_DefsVertexOfEdge(myGraph, BRepGraph_EdgeId::Start())), 2);
}

TEST_F(BRepGraph_DefsIteratorTest, EdgeOfWire_YieldsEdgeDefinitions)
{
  BRepGraph_DefsEdgeOfWire anIt(myGraph, BRepGraph_WireId::Start());
  ASSERT_TRUE(anIt.More());
  const BRepGraph_EdgeId   anEdgeId  = anIt.CurrentId();
  const BRepGraph_CoEdgeId aCoEdgeId = anIt.CurrentRefId();
  EXPECT_TRUE(anEdgeId.IsValid(myGraph.Topo().Edges().Nb()));
  EXPECT_TRUE(aCoEdgeId.IsValid(myGraph.Topo().CoEdges().Nb()));
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId, anEdgeId);
  EXPECT_TRUE(anIt.Current().StartVertexRefId.IsValid());
  EXPECT_TRUE(anIt.Current().EndVertexRefId.IsValid());
}

TEST_F(BRepGraph_DefsIteratorTest, CoEdgeOfWire_YieldsCoEdgeDefinitions)
{
  BRepGraph_DefsCoEdgeOfWire anIt(myGraph, BRepGraph_WireId::Start());
  ASSERT_TRUE(anIt.More());
  EXPECT_TRUE(anIt.CurrentId().IsValid(myGraph.Topo().CoEdges().Nb()));
  EXPECT_EQ(anIt.CurrentRefId(), anIt.CurrentId());
  EXPECT_TRUE(anIt.Current().ChildEdgeId.IsValid(myGraph.Topo().Edges().Nb()));
}

TEST_F(BRepGraph_DefsIteratorTest, VertexOfEdge_ExposesBoundaryVertexRef)
{
  BRepGraph_DefsVertexOfEdge anIt(myGraph, BRepGraph_EdgeId::Start());
  ASSERT_TRUE(anIt.More());
  const BRepGraph_VertexId    aVertexId = anIt.CurrentId();
  const BRepGraph_VertexRefId aRefId    = anIt.CurrentRefId();
  EXPECT_TRUE(aVertexId.IsValid(myGraph.Topo().Vertices().Nb()));
  EXPECT_TRUE(aRefId.IsValid(myGraph.Refs().Vertices().Nb()));
  EXPECT_EQ(myGraph.Refs().Vertices().Entry(aRefId).ChildVertexId, aVertexId);
}

TEST(BRepGraph_DefsIteratorTestStandalone, VertexOfEdge_EnumeratesBoundaryVerticesOnly)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(wrapEdgeInFace(makeEdgeWithInternalVertex()));

  bool   aFoundSplitVertex = false;
  size_t aCount            = 0;
  for (BRepGraph_DefsVertexOfEdge anIt(aGraph, BRepGraph_EdgeId::Start()); anIt.More(); anIt.Next())
  {
    ++aCount;
    const gp_Pnt& aPoint = anIt.Current().Point;
    if (std::abs(aPoint.X() - 5.0) <= Precision::Confusion())
    {
      aFoundSplitVertex = true;
    }
  }

  EXPECT_EQ(aCount, 2);
  EXPECT_FALSE(aFoundSplitVertex);
}

TEST_F(BRepGraph_DefsIteratorTest, ChildOfCompound_EnumeratesHeterogeneousChildren)
{
  const BRepGraph_VertexId aLooseVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.01);
  ASSERT_TRUE(aLooseVertex.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_SolidId::Start());
  aChildren.Append(aLooseVertex);
  const BRepGraph_CompoundId aCompound = myGraph.Editor().Compounds().Add(aChildren.ToArray1());

  ASSERT_TRUE(aCompound.IsValid());
  BRepGraph_DefsChildOfCompound anIt(myGraph, aCompound);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.CurrentId().NodeKind, BRepGraph_NodeId::Kind::Solid);
  anIt.Next();
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.CurrentId().NodeKind, BRepGraph_NodeId::Kind::Vertex);
}

TEST_F(BRepGraph_DefsIteratorTest, ChildOfCompound_SkipsOutOfRangeChildNode)
{
  const BRepGraph_VertexId aLooseVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.01);
  ASSERT_TRUE(aLooseVertex.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_SolidId::Start());
  aChildren.Append(aLooseVertex);
  aChildren.Append(aLooseVertex);
  const BRepGraph_CompoundId aCompound = myGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefs =
    myGraph.Topo().Compounds().Relations(aCompound).ChildRefIds;
  ASSERT_EQ(aChildRefs.Size(), 3);
  {
    BRepGraph_MutGuard<BRepGraphInc::ChildRef> aBadRef =
      myGraph.Editor().Gen().MutChildRef(aChildRefs.Value(1));
    aBadRef.Internal().ChildNodeId =
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, myGraph.Topo().Vertices().Nb());
  }

  uint32_t aCount = 0;
  for (BRepGraph_DefsChildOfCompound anIt(myGraph, aCompound); anIt.More(); anIt.Next())
  {
    EXPECT_TRUE(myGraph.Topo().Gen().IsActive(anIt.CurrentId()));
    ++aCount;
  }
  EXPECT_EQ(aCount, 2u);
}

TEST_F(BRepGraph_DefsIteratorTest, ChildOfCompound_SkipsRemovedChildNode)
{
  const BRepGraph_VertexId aLooseVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.01);
  ASSERT_TRUE(aLooseVertex.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_SolidId::Start());
  aChildren.Append(aLooseVertex);
  const BRepGraph_CompoundId aCompound = myGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  myGraph.Editor().Gen().RemoveNode(aLooseVertex);

  ASSERT_TRUE(BRepGraph_DefsChildOfCompound(myGraph, aCompound).More());
  EXPECT_EQ(countIterator(BRepGraph_DefsChildOfCompound(myGraph, aCompound)), 1);
}

TEST_F(BRepGraph_DefsIteratorTest, SolidOfCompSolid_EnumeratesDirectSolids)
{
  const BRepGraph_SolidId aSolidA = myGraph.Editor().Solids().Add();
  const BRepGraph_SolidId aSolidB = myGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolidA.IsValid());
  ASSERT_TRUE(aSolidB.IsValid());

  NCollection_LinearVector<BRepGraph_SolidId> aSolids;
  aSolids.Append(aSolidA);
  aSolids.Append(aSolidB);
  const BRepGraph_CompSolidId aCompSolid = myGraph.Editor().CompSolids().Add(aSolids.ToArray1());

  ASSERT_TRUE(aCompSolid.IsValid());
  EXPECT_EQ(countIterator(BRepGraph_DefsSolidOfCompSolid(myGraph, aCompSolid)), 2);
}

TEST_F(BRepGraph_DefsIteratorTest, OccurrenceOfProduct_EnumeratesDirectOccurrences)
{
  const BRepGraph_ProductId aPart = myGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  myGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  EXPECT_TRUE(myGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location()).IsValid());
  EXPECT_TRUE(myGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location()).IsValid());

  EXPECT_EQ(countIterator(BRepGraph_DefsOccurrenceOfProduct(myGraph, anAssembly)), 2);
}

TEST_F(BRepGraph_DefsIteratorTest, RemovedWireRef_IsSkipped)
{
  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    myGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start());
  ASSERT_EQ(aWireRefs.Size(), 1);

  myGraph.Editor().Gen().RemoveRef(aWireRefs.Value(0));

  EXPECT_EQ(countIterator(BRepGraph_DefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 0);
}
