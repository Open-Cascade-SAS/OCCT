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
#include <BRepGraph_RefsIterator.hxx>
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
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

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

class BRepGraph_RefsIteratorTest : public testing::Test
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

TEST_F(BRepGraph_RefsIteratorTest, BoxHierarchy_YieldsReferenceIds)
{
  EXPECT_EQ(countIterator(BRepGraph_RefsShellOfSolid(myGraph, BRepGraph_SolidId::Start())), 1);
  EXPECT_EQ(countIterator(BRepGraph_RefsFaceOfShell(myGraph, BRepGraph_ShellId::Start())), 6);
  EXPECT_EQ(countIterator(BRepGraph_RefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 1);
  EXPECT_EQ(countIterator(BRepGraph_CoEdgesOfWire(myGraph, BRepGraph_WireId::Start())), 4);
  EXPECT_EQ(countIterator(BRepGraph_RefsVertexOfEdge(myGraph, BRepGraph_EdgeId::Start())), 2);
}

TEST_F(BRepGraph_RefsIteratorTest, CurrentId_ResolvesToExpectedEntry)
{
  BRepGraph_RefsWireOfFace anIt(myGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::WireRef& aWireRef = myGraph.Refs().Wires().Entry(anIt.CurrentId());
  EXPECT_TRUE(aWireRef.ChildWireId.IsValid(myGraph.Topo().Wires().Nb()));
}

TEST(BRepGraph_RefsIteratorTestStandalone, VertexOfEdge_ExposesBoundaryVertexRefsOnly)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(wrapEdgeInFace(makeEdgeWithInternalVertex()));

  bool     aFoundInternal = false;
  uint32_t aCount         = 0;
  for (BRepGraph_RefsVertexOfEdge anIt(aGraph, BRepGraph_EdgeId::Start()); anIt.More(); anIt.Next())
  {
    ++aCount;
    const BRepGraphInc::VertexRef& aVertexRef = aGraph.Refs().Vertices().Entry(anIt.CurrentId());
    if (aVertexRef.Orientation == TopAbs_INTERNAL)
    {
      aFoundInternal = true;
    }
  }

  EXPECT_EQ(aCount, 2);
  EXPECT_FALSE(aFoundInternal);
}

TEST_F(BRepGraph_RefsIteratorTest, ChildOfCompound_EnumeratesChildRefs)
{
  const BRepGraph_VertexId aLooseVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.01);
  ASSERT_TRUE(aLooseVertex.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_SolidId::Start());
  aChildren.Append(aLooseVertex);
  const BRepGraph_CompoundId aCompound = myGraph.Editor().Compounds().Add(aChildren.ToArray1());

  ASSERT_TRUE(aCompound.IsValid());
  BRepGraph_RefsChildOfCompound anIt(myGraph, aCompound);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(anIt.CurrentId()).ChildNodeId.NodeKind,
            BRepGraph_NodeId::Kind::Solid);
  anIt.Next();
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(anIt.CurrentId()).ChildNodeId.NodeKind,
            BRepGraph_NodeId::Kind::Vertex);
}

TEST_F(BRepGraph_RefsIteratorTest, ChildOfCompound_SkipsOutOfRangeChildNode)
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
  for (BRepGraph_RefsChildOfCompound anIt(myGraph, aCompound); anIt.More(); anIt.Next())
  {
    const BRepGraph_NodeId aChildNode =
      myGraph.Refs().Children().Entry(anIt.CurrentId()).ChildNodeId;
    EXPECT_TRUE(myGraph.Topo().Gen().IsActive(aChildNode));
    ++aCount;
  }
  EXPECT_EQ(aCount, 2u);
}

TEST_F(BRepGraph_RefsIteratorTest, ChildOfCompound_SkipsRemovedChildNode)
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

  ASSERT_TRUE(BRepGraph_RefsChildOfCompound(myGraph, aCompound).More());
  EXPECT_EQ(countIterator(BRepGraph_RefsChildOfCompound(myGraph, aCompound)), 1u);
}

TEST_F(BRepGraph_RefsIteratorTest, OccurrenceOfProduct_EnumeratesOccurrenceRefs)
{
  const BRepGraph_ProductId aPart = myGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  myGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  EXPECT_TRUE(myGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location()).IsValid());
  EXPECT_TRUE(myGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location()).IsValid());

  EXPECT_EQ(countIterator(BRepGraph_RefsOccurrenceOfProduct(myGraph, anAssembly)), 2);
}

TEST_F(BRepGraph_RefsIteratorTest, RemovedWireRef_IsSkipped)
{
  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    myGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start());
  ASSERT_EQ(aWireRefs.Size(), 1);

  myGraph.Editor().Gen().RemoveRef(aWireRefs.Value(0));

  EXPECT_EQ(countIterator(BRepGraph_RefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 0);
}
