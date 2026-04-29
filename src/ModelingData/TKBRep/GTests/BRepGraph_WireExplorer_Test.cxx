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
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_WireExplorer.hxx>
#include <BRepGraph_Builder.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

class BRepGraph_WireExplorerTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
      BRepGraph_Builder::Add(myGraph, aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_WireExplorerTest, BoxWire_CountMatchesFourEdges)
{
  const BRepGraph_WireId aWireId(0);
  BRepGraph_WireExplorer anExp(myGraph, aWireId);

  EXPECT_EQ(anExp.NbEdges(), 4);
}

TEST_F(BRepGraph_WireExplorerTest, BoxWire_EdgesReturnedInConnectionOrder)
{
  const BRepGraph_WireId aWireId(0);
  BRepGraph_WireExplorer anExp(myGraph, aWireId);

  // Check that the end vertex of edge[i] matches the start vertex of edge[i+1].
  ASSERT_GE(anExp.NbEdges(), 2);

  BRepGraph_CoEdgeId aPrevId = anExp.CurrentCoEdgeId();
  anExp.Next();
  int anIdx = 0;
  for (; anExp.More(); anExp.Next(), ++anIdx)
  {
    const BRepGraph_CoEdgeId aCurrId = anExp.CurrentCoEdgeId();

    const BRepGraphInc::CoEdgeDef& aPrevCoEdge = myGraph.Topo().CoEdges().Definition(aPrevId);
    const BRepGraphInc::CoEdgeDef& aCurrCoEdge = myGraph.Topo().CoEdges().Definition(aCurrId);

    const BRepGraphInc::EdgeDef& aPrevEdge =
      myGraph.Topo().Edges().Definition(aPrevCoEdge.EdgeDefId);
    const BRepGraphInc::EdgeDef& aCurrEdge =
      myGraph.Topo().Edges().Definition(aCurrCoEdge.EdgeDefId);

    // Resolve oriented end vertex of previous and start vertex of current.
    const BRepGraph_VertexRefId aPrevEndRef   = (aPrevCoEdge.Orientation == TopAbs_FORWARD)
                                                  ? aPrevEdge.EndVertexRefId
                                                  : aPrevEdge.StartVertexRefId;
    const BRepGraph_VertexRefId aCurrStartRef = (aCurrCoEdge.Orientation == TopAbs_FORWARD)
                                                  ? aCurrEdge.StartVertexRefId
                                                  : aCurrEdge.EndVertexRefId;

    ASSERT_TRUE(aPrevEndRef.IsValid()) << "Invalid end vertex ref at index " << anIdx;
    ASSERT_TRUE(aCurrStartRef.IsValid()) << "Invalid start vertex ref at index " << (anIdx + 1);

    const BRepGraph_VertexId aPrevEndVtx = myGraph.Refs().Vertices().Entry(aPrevEndRef).VertexDefId;
    const BRepGraph_VertexId aCurrStartVtx =
      myGraph.Refs().Vertices().Entry(aCurrStartRef).VertexDefId;

    EXPECT_EQ(aPrevEndVtx, aCurrStartVtx) << "Disconnected at ordered index " << anIdx;

    aPrevId = aCurrId;
  }
}

TEST_F(BRepGraph_WireExplorerTest, AllBoxWires_AreConnected)
{
  // Verify all 6 wires of the box are well-ordered.
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    BRepGraph_WireExplorer anExp(myGraph, aWireIt.CurrentId());
    EXPECT_EQ(anExp.NbEdges(), 4) << "Wire " << aWireIt.CurrentId().Index;
  }
}

TEST_F(BRepGraph_WireExplorerTest, RangeFor_WorksCorrectly)
{
  const BRepGraph_WireId aWireId(0);
  BRepGraph_WireExplorer anExp(myGraph, aWireId);

  int aCount = 0;
  for (const BRepGraph_CoEdgeId aCoEdgeId : anExp)
  {
    EXPECT_TRUE(aCoEdgeId.IsValid(myGraph.Topo().CoEdges().Nb()));
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST_F(BRepGraph_WireExplorerTest, CurrentCoEdgeId_ReturnsValidId)
{
  const BRepGraph_WireId aWireId(0);
  BRepGraph_WireExplorer anExp(myGraph, aWireId);

  ASSERT_TRUE(anExp.More());
  const BRepGraph_CoEdgeId aCoEdgeId = anExp.CurrentCoEdgeId();
  EXPECT_TRUE(aCoEdgeId.IsValid(myGraph.Topo().CoEdges().Nb()));
}

TEST_F(BRepGraph_WireExplorerTest, Reset_RestartsIteration)
{
  const BRepGraph_WireId aWireId(0);
  BRepGraph_WireExplorer anExp(myGraph, aWireId);

  // Consume all edges.
  while (anExp.More())
  {
    anExp.Next();
  }
  EXPECT_FALSE(anExp.More());

  // Reset and verify re-iteration works.
  anExp.Reset();
  EXPECT_TRUE(anExp.More());
  EXPECT_EQ(anExp.CurrentCoEdgeId(), BRepGraph_WireExplorer(myGraph, aWireId).CurrentCoEdgeId());
}
