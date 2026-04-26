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
#include <BRepGraph_Builder.hxx>

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
static int countIterator(IteratorT theIterator)
{
  int aCount = 0;
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

static TopoDS_Face makeFaceWithDirectVertex()
{
  BRep_Builder                  aBuilder;
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face                   aFace;
  aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Wire             aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, aMakeEdge.Edge());
  aBuilder.Add(aFace, aWire);

  TopoDS_Vertex aDirectVertex;
  aBuilder.MakeVertex(aDirectVertex, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBuilder.Add(aFace, aDirectVertex.Oriented(TopAbs_INTERNAL));
  return aFace;
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
    myGraph.Clear(); (void)BRepGraph_Builder::Add(myGraph, aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_RefsIteratorTest, BoxHierarchy_YieldsReferenceIds)
{
  EXPECT_EQ(countIterator(BRepGraph_RefsShellOfSolid(myGraph, BRepGraph_SolidId::Start())), 1);
  EXPECT_EQ(countIterator(BRepGraph_RefsFaceOfShell(myGraph, BRepGraph_ShellId::Start())), 6);
  EXPECT_EQ(countIterator(BRepGraph_RefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 1);
  EXPECT_EQ(countIterator(BRepGraph_RefsCoEdgeOfWire(myGraph, BRepGraph_WireId::Start())), 4);
  EXPECT_EQ(countIterator(BRepGraph_RefsVertexOfEdge(myGraph, BRepGraph_EdgeId::Start())), 2);
}

TEST_F(BRepGraph_RefsIteratorTest, CurrentId_ResolvesToExpectedEntry)
{
  BRepGraph_RefsWireOfFace anIt(myGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::WireRef& aWireRef = myGraph.Refs().Wires().Entry(anIt.CurrentId());
  EXPECT_TRUE(aWireRef.WireDefId.IsValid(myGraph.Topo().Wires().Nb()));
}

TEST(BRepGraph_RefsIteratorTestStandalone, VertexOfEdge_ExposesInternalVertexRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, wrapEdgeInFace(makeEdgeWithInternalVertex()));

  BRepGraph_EdgeId aEdgeWithInternal;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (anEdgeIt.Current().InternalVertexRefIds.Length() == 1)
    {
      aEdgeWithInternal = anEdgeIt.CurrentId();
      break;
    }
  }

  ASSERT_TRUE(aEdgeWithInternal.IsValid());

  bool aFoundInternal = false;
  int  aCount         = 0;
  for (BRepGraph_RefsVertexOfEdge anIt(aGraph, aEdgeWithInternal); anIt.More(); anIt.Next())
  {
    ++aCount;
    const BRepGraphInc::VertexRef& aVertexRef = aGraph.Refs().Vertices().Entry(anIt.CurrentId());
    if (aVertexRef.Orientation == TopAbs_INTERNAL)
    {
      aFoundInternal = true;
    }
  }

  EXPECT_EQ(aCount, 3);
  EXPECT_TRUE(aFoundInternal);
}

TEST(BRepGraph_RefsIteratorTestStandalone, VertexOfFace_ExposesDirectVertexRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, makeFaceWithDirectVertex());

  BRepGraph_RefsVertexOfFace anIt(aGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anIt.More());

  const BRepGraphInc::VertexRef& aVertexRef = aGraph.Refs().Vertices().Entry(anIt.CurrentId());
  EXPECT_EQ(aVertexRef.Orientation, TopAbs_INTERNAL);
  EXPECT_TRUE(aVertexRef.VertexDefId.IsValid(aGraph.Topo().Vertices().Nb()));
}

TEST_F(BRepGraph_RefsIteratorTest, ChildOfCompound_EnumeratesChildRefs)
{
  const BRepGraph_VertexId aLooseVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.01);
  ASSERT_TRUE(aLooseVertex.IsValid());

  NCollection_DynamicArray<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_SolidId::Start());
  aChildren.Append(aLooseVertex);
  const BRepGraph_CompoundId aCompound = myGraph.Editor().Compounds().Add(aChildren);

  ASSERT_TRUE(aCompound.IsValid());
  BRepGraph_RefsChildOfCompound anIt(myGraph, aCompound);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(anIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Solid);
  anIt.Next();
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(anIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Vertex);
}

TEST_F(BRepGraph_RefsIteratorTest, OccurrenceOfProduct_EnumeratesOccurrenceRefs)
{
  const BRepGraph_ProductId aPart = myGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  const BRepGraph_ProductId anAssembly = myGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  EXPECT_TRUE(
    myGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location()).IsValid());
  EXPECT_TRUE(
    myGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location()).IsValid());

  EXPECT_EQ(countIterator(BRepGraph_RefsOccurrenceOfProduct(myGraph, anAssembly)), 2);
}

TEST_F(BRepGraph_RefsIteratorTest, AuxChildRefsOfShellAndSolid_EnumerateInjectedChildRefs)
{
  NCollection_DynamicArray<BRepGraph_NodeId> aShellChildren;
  aShellChildren.Append(BRepGraph_WireId::Start());
  aShellChildren.Append(BRepGraph_EdgeId::Start());
  const BRepGraph_CompoundId aShellSeed = myGraph.Editor().Compounds().Add(aShellChildren);
  ASSERT_TRUE(aShellSeed.IsValid());

  {
    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aShell =
      myGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start());
    for (const BRepGraph_ChildRefId& aRefId :
         myGraph.Topo().Compounds().Definition(aShellSeed).ChildRefIds)
    {
      aShell->AuxChildRefIds.Append(aRefId);
    }
  }

  BRepGraph_RefsChildOfShell aShellIt(myGraph, BRepGraph_ShellId::Start());
  ASSERT_TRUE(aShellIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(aShellIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Wire);
  aShellIt.Next();
  ASSERT_TRUE(aShellIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(aShellIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Edge);

  NCollection_DynamicArray<BRepGraph_NodeId> aSolidChildren;
  aSolidChildren.Append(BRepGraph_EdgeId(1));
  aSolidChildren.Append(BRepGraph_VertexId::Start());
  const BRepGraph_CompoundId aSolidSeed = myGraph.Editor().Compounds().Add(aSolidChildren);
  ASSERT_TRUE(aSolidSeed.IsValid());

  {
    BRepGraph_MutGuard<BRepGraphInc::SolidDef> aSolid =
      myGraph.Editor().Solids().Mut(BRepGraph_SolidId::Start());
    for (const BRepGraph_ChildRefId& aRefId :
         myGraph.Topo().Compounds().Definition(aSolidSeed).ChildRefIds)
    {
      aSolid->AuxChildRefIds.Append(aRefId);
    }
  }

  BRepGraph_RefsChildOfSolid aSolidIt(myGraph, BRepGraph_SolidId::Start());
  ASSERT_TRUE(aSolidIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(aSolidIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Edge);
  aSolidIt.Next();
  ASSERT_TRUE(aSolidIt.More());
  EXPECT_EQ(myGraph.Refs().Children().Entry(aSolidIt.CurrentId()).ChildDefId.NodeKind,
            BRepGraph_NodeId::Kind::Vertex);
}

TEST_F(BRepGraph_RefsIteratorTest, RemovedWireRef_IsSkipped)
{
  const NCollection_DynamicArray<BRepGraph_WireRefId>& aWireRefs =
    myGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start());
  ASSERT_EQ(aWireRefs.Length(), 1);

  {
    BRepGraph_MutGuard<BRepGraphInc::WireRef> aWireRef =
      myGraph.Editor().Wires().MutRef(aWireRefs.Value(0));
    aWireRef->IsRemoved = true;
  }

  EXPECT_EQ(countIterator(BRepGraph_RefsWireOfFace(myGraph, BRepGraph_FaceId::Start())), 0);
}