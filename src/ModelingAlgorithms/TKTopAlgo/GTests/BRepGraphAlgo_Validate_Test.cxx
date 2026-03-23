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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Validate.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

TEST(BRepGraphAlgo_ValidateTest, CleanGraph_NoIssues)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Validate::Result aResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aResult.IsValid());
  EXPECT_EQ(aResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
  EXPECT_EQ(aResult.NbIssues(BRepGraphAlgo_Validate::Severity::Warning), 0);
}

TEST(BRepGraphAlgo_ValidateTest, AfterGeomDeduplicate_NoIssues)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  const BRepGraphAlgo_Validate::Result aResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aResult.IsValid());
  EXPECT_EQ(aResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, DetectsRemovedNodeReference)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbVertices(), 0);

  // Find an edge that has a valid start vertex and remove it.
  int aVtxToRemove = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.StartVertexDefId().IsValid())
    {
      aVtxToRemove = anEdge.StartVertexIdx;
      break;
    }
  }
  ASSERT_GE(aVtxToRemove, 0);

  // Remove the vertex without fixing edges referencing it.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Vertex(aVtxToRemove));

  const BRepGraphAlgo_Validate::Result aResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_GT(aResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, WireConnectivity_DisconnectedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbWires(), 0);

  // Corrupt a wire by swapping its vertex reference to break connectivity.
  // Find a wire with at least 2 edges.
  int aTargetWire = -1;
  for (int aWireIdx = 0; aWireIdx < aGraph.Defs().NbWires(); ++aWireIdx)
  {
    if (aGraph.Defs().Wire(aWireIdx).CoEdgeRefs.Length() >= 2)
    {
      aTargetWire = aWireIdx;
      break;
    }
  }
  ASSERT_GE(aTargetWire, 0);

  // Get the first edge in the wire and corrupt its end vertex.
  const BRepGraph_TopoNode::WireDef& aWireDef = aGraph.Defs().Wire(aTargetWire);
  const BRepGraphInc::CoEdgeRef& aFirstCR = aWireDef.CoEdgeRefs.Value(0);
  const BRepGraph_TopoNode::CoEdgeDef& aFirstCoEdge = aGraph.Defs().CoEdge(aFirstCR.CoEdgeIdx);
  const BRepGraph_NodeId aFirstEdgeId(BRepGraph_NodeId::Kind::Edge, aFirstCoEdge.EdgeIdx);
  ASSERT_TRUE(aFirstEdgeId.IsValid());

  BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aFirstEdge = aGraph.Mut().EdgeDef(aFirstEdgeId.Index);

  // Find a vertex different from the current end vertex.
  BRepGraph_NodeId anOrigEnd = aFirstEdge->EndVertexDefId();
  for (int aVtxIdx = 0; aVtxIdx < aGraph.Defs().NbVertices(); ++aVtxIdx)
  {
    if (BRepGraph_NodeId::Vertex(aVtxIdx) != anOrigEnd
        && BRepGraph_NodeId::Vertex(aVtxIdx) != aFirstEdge->StartVertexDefId())
    {
      aFirstEdge->EndVertexIdx = aVtxIdx;
      break;
    }
  }

  ASSERT_NE(aFirstEdge->EndVertexDefId(), anOrigEnd);

  const BRepGraphAlgo_Validate::Result aResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aResult.IsValid());

  // Check that at least one connectivity error was found.
  bool aFoundConnectivity = false;
  for (int anIdx = 0; anIdx < aResult.Issues.Length(); ++anIdx)
  {
    const BRepGraphAlgo_Validate::Issue& anIssue = aResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraphAlgo_Validate::Severity::Error)
    {
      TCollection_AsciiString aDesc = anIssue.Description;
      if (aDesc.Search("Wire edges not connected") > 0)
      {
        aFoundConnectivity = true;
        break;
      }
    }
  }
  EXPECT_TRUE(aFoundConnectivity);
}

TEST(BRepGraphAlgo_ValidateTest, BoundsCheck_InvalidIndex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  // Corrupt edge's Curve3d to null.
  BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge = aGraph.Mut().EdgeDef(0);
  anEdge->Curve3d.Nullify();

  const BRepGraphAlgo_Validate::Result aResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_GT(aResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, AfterSplitEdge_ProducesSubEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const int anOrigEdgeCount = aGraph.Defs().NbEdges();

  // Find a non-degenerate edge with valid vertices to split.
  BRepGraph_NodeId anEdgeId;
  double aSplitParam = 0.0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(i);
    if (!anEdgeDef.IsDegenerate && !anEdgeDef.Curve3d.IsNull()
        && anEdgeDef.StartVertexDefId().IsValid() && anEdgeDef.EndVertexDefId().IsValid())
    {
      anEdgeId    = BRepGraph_NodeId::Edge(i);
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  // Create a split vertex at the midpoint.
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeId.Index);
  gp_Pnt aMidPt;
  anEdgeDef.Curve3d->D0(aSplitParam, aMidPt);
  BRepGraph_NodeId aSplitVtx = aGraph.Builder().AddVertexDef(aMidPt, anEdgeDef.Tolerance);

  BRepGraph_NodeId aSubA, aSubB;
  aGraph.Mut().SplitEdge(anEdgeId, aSplitVtx, aSplitParam, aSubA, aSubB);

  // Two new sub-edges should have been created.
  EXPECT_EQ(aGraph.Defs().NbEdges(), anOrigEdgeCount + 2);
  EXPECT_TRUE(aSubA.IsValid());
  EXPECT_TRUE(aSubB.IsValid());

  // Original edge should be marked removed.
  EXPECT_TRUE(aGraph.Defs().Edge(anEdgeId.Index).IsRemoved);
}

TEST(BRepGraphAlgo_ValidateTest, CorruptedPCurve_FaceDefIdOutOfBounds)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  // Corrupt a CoEdge's FaceDefId to an out-of-range value.
  ASSERT_GT(aGraph.Defs().NbCoEdges(), 0);
  BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef> aCoEdgeDef = aGraph.MutCoEdge(0);
  aCoEdgeDef->FaceDefId = BRepGraph_NodeId::Face(aGraph.Defs().NbFaces() + 999);

  const BRepGraphAlgo_Validate::Result aValResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aValResult.IsValid());
}
