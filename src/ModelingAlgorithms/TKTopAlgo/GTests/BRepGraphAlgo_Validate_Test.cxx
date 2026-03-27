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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Validate.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

namespace
{

//=================================================================================================

NCollection_Vector<BRepGraph_CoEdgeRefId> coEdgeRefsOfWire(const BRepGraph&       theGraph,
                                                           const BRepGraph_WireId theWireId)
{
  NCollection_Vector<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph_NodeId                    aParentNode = BRepGraph_NodeId::Wire(theWireId.Index);
  const BRepGraph::RefsView&                aRefs       = theGraph.Refs();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbCoEdgeRefs(); ++aRefIdx)
  {
    const BRepGraph_CoEdgeRefId         aRefId(aRefIdx);
    const BRepGraphInc::CoEdgeRefEntry& aRef = aRefs.CoEdge(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

} // namespace

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

  const BRepGraphAlgo_Validate::Options aDeepOpts = BRepGraphAlgo_Validate::Options::DeepAudit();
  const BRepGraphAlgo_Validate::Result  aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, aDeepOpts);
  EXPECT_TRUE(aDeepResult.IsValid());
  EXPECT_EQ(aDeepResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  const BRepGraphAlgo_Validate::Options aLightOpts = BRepGraphAlgo_Validate::Options::Lightweight();
  const BRepGraphAlgo_Validate::Result  aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, aLightOpts);
  EXPECT_TRUE(aLightResult.IsValid());
  EXPECT_EQ(aLightResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, AfterGeomDeduplicate_NoIssues)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
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

  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

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
  ASSERT_GT(aGraph.Topo().NbVertices(), 0);

  // Find an edge that has a valid start vertex and remove it.
  int aVtxToRemove = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId              anEdgeId(anEdgeIdx);
    const BRepGraphInc::VertexRefEntry& aStartRef =
      BRepGraph_Tool::Edge::StartVertex(aGraph, anEdgeId);
    if (aStartRef.VertexDefId.IsValid())
    {
      aVtxToRemove = aStartRef.VertexDefId.Index;
      break;
    }
  }
  ASSERT_GE(aVtxToRemove, 0);

  // Remove the vertex without fixing edges referencing it.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Vertex(aVtxToRemove));

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aDefaultResult.IsValid());

  const BRepGraphAlgo_Validate::Result aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
  EXPECT_FALSE(aDeepResult.IsValid());
  EXPECT_GT(aDeepResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, WireConnectivity_DisconnectedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbWires(), 0);

  // Corrupt a wire by swapping its vertex reference to break connectivity.
  // Find a wire with at least 2 edges.
  int aTargetWire = -1;
  for (int aWireIdx = 0; aWireIdx < aGraph.Topo().NbWires(); ++aWireIdx)
  {
    if (coEdgeRefsOfWire(aGraph, BRepGraph_WireId(aWireIdx)).Length() >= 2)
    {
      aTargetWire = aWireIdx;
      break;
    }
  }
  ASSERT_GE(aTargetWire, 0);

  // Get the first edge in the wire and corrupt its end vertex.
  const NCollection_Vector<BRepGraph_CoEdgeRefId> aWireRefIds =
    coEdgeRefsOfWire(aGraph, BRepGraph_WireId(aTargetWire));
  ASSERT_GE(aWireRefIds.Length(), 1);
  const BRepGraphInc::CoEdgeRefEntry& aFirstCR = aGraph.Refs().CoEdge(aWireRefIds.Value(0));
  const BRepGraphInc::CoEdgeDef&      aFirstCoEdge =
    aGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aFirstCR.CoEdgeDefId));
  const BRepGraph_NodeId aFirstEdgeId(aFirstCoEdge.EdgeDefId);
  ASSERT_TRUE(aFirstEdgeId.IsValid());

  BRepGraph_MutRef<BRepGraphInc::EdgeDef> aFirstEdge =
    aGraph.Builder().MutEdge(BRepGraph_EdgeId(aFirstEdgeId.Index));

  // Find a vertex different from the current end vertex.
  const BRepGraph_VertexId anOrigEndVtx =
    aGraph.Refs().Vertex(aFirstEdge->EndVertexRefId).VertexDefId;
  const BRepGraph_VertexId anOrigStartVtx =
    aGraph.Refs().Vertex(aFirstEdge->StartVertexRefId).VertexDefId;
  const BRepGraph_NodeId anOrigEnd = BRepGraph_NodeId(anOrigEndVtx);
  for (int aVtxIdx = 0; aVtxIdx < aGraph.Topo().NbVertices(); ++aVtxIdx)
  {
    if (BRepGraph_NodeId::Vertex(aVtxIdx) != anOrigEnd
        && BRepGraph_NodeId::Vertex(aVtxIdx) != BRepGraph_NodeId(anOrigStartVtx))
    {
      BRepGraph_MutRefEntry<BRepGraphInc::VertexRefEntry> aMutEndRef =
        aGraph.Builder().MutVertexRef(aFirstEdge->EndVertexRefId);
      aMutEndRef->VertexDefId = BRepGraph_VertexId(aVtxIdx);
      break;
    }
  }

  ASSERT_NE(aGraph.Refs().Vertex(aFirstEdge->EndVertexRefId).VertexDefId, anOrigEndVtx);

  const BRepGraphAlgo_Validate::Result aResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
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
  ASSERT_GT(aGraph.Topo().NbEdges(), 0);

  // Corrupt edge's Curve3d to null.
  BRepGraph_MutRef<BRepGraphInc::EdgeDef> anEdge = aGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
  anEdge->Curve3DRepId                           = BRepGraph_Curve3DRepId();

  const BRepGraphAlgo_Validate::Result aResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
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
  const int anOrigEdgeCount = aGraph.Topo().NbEdges();

  // Find a non-degenerate edge with valid vertices to split.
  BRepGraph_NodeId anEdgeId;
  double           aSplitParam = 0.0;
  for (int i = 0; i < aGraph.Topo().NbEdges(); ++i)
  {
    const BRepGraph_EdgeId       anCandEdgeId(i);
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edge(anCandEdgeId);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid()
        && BRepGraph_Tool::Edge::StartVertex(aGraph, anCandEdgeId).VertexDefId.IsValid()
        && BRepGraph_Tool::Edge::EndVertex(aGraph, anCandEdgeId).VertexDefId.IsValid())
    {
      anEdgeId    = BRepGraph_NodeId::Edge(i);
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  // Create a split vertex at the midpoint.
  gp_Pnt aMidPt;
  BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(anEdgeId.Index))->D0(aSplitParam, aMidPt);
  BRepGraph_NodeId aSplitVtx = aGraph.Builder().AddVertex(
    aMidPt,
    BRepGraph_Tool::Edge::Tolerance(aGraph, BRepGraph_EdgeId(anEdgeId.Index)));

  BRepGraph_NodeId aSubA, aSubB;
  BRepGraph_Mutator::SplitEdge(aGraph, anEdgeId, aSplitVtx, aSplitParam, aSubA, aSubB);

  // Two new sub-edges should have been created.
  EXPECT_EQ(aGraph.Topo().NbEdges(), anOrigEdgeCount + 2);
  EXPECT_TRUE(aSubA.IsValid());
  EXPECT_TRUE(aSubB.IsValid());

  // Original edge should be marked removed.
  EXPECT_TRUE(aGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeId.Index)).IsRemoved);
}

TEST(BRepGraphAlgo_ValidateTest, CorruptedPCurve_FaceDefIdOutOfBounds)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbEdges(), 0);

  // Corrupt a CoEdge's FaceDefId to an out-of-range value.
  ASSERT_GT(aGraph.Topo().NbCoEdges(), 0);
  BRepGraph_MutRef<BRepGraphInc::CoEdgeDef> aCoEdgeDef =
    aGraph.Builder().MutCoEdge(BRepGraph_CoEdgeId(0));
  aCoEdgeDef->FaceDefId = BRepGraph_NodeId::Face(aGraph.Topo().NbFaces() + 999);

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aDefaultResult.IsValid());

  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraphAlgo_Validate::Result aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
  EXPECT_FALSE(aDeepResult.IsValid());
}

TEST(BRepGraphAlgo_ValidateTest, LightweightAndDeepAudit_DetectActiveCountDrift)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbActiveFacesBefore = aGraph.Topo().NbActiveFaces();
  ASSERT_GT(aNbActiveFacesBefore, 0);

  // Intentionally bypass RemoveNode() to simulate counter drift bug class.
  BRepGraph_MutRef<BRepGraphInc::FaceDef> aFaceDef = aGraph.Builder().MutFace(BRepGraph_FaceId(0));
  aFaceDef->IsRemoved                              = true;

  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());
  EXPECT_GT(aLightResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  bool aFoundBoundaryActiveCountMismatch = false;
  for (int anIdx = 0; anIdx < aLightResult.Issues.Length(); ++anIdx)
  {
    const TCollection_AsciiString& aDesc = aLightResult.Issues.Value(anIdx).Description;
    if (aDesc.Search("Mutation boundary active count mismatch for Faces") > 0)
    {
      aFoundBoundaryActiveCountMismatch = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundBoundaryActiveCountMismatch);

  const BRepGraphAlgo_Validate::Result aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
  EXPECT_FALSE(aDeepResult.IsValid());
  EXPECT_GT(aDeepResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  bool aFoundActiveCountMismatch = false;
  for (int anIdx = 0; anIdx < aDeepResult.Issues.Length(); ++anIdx)
  {
    const TCollection_AsciiString& aDesc = aDeepResult.Issues.Value(anIdx).Description;
    if (aDesc.Search("NbActiveFaces mismatch") > 0)
    {
      aFoundActiveCountMismatch = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundActiveCountMismatch);
  EXPECT_EQ(aGraph.Topo().NbActiveFaces(), aNbActiveFacesBefore);
}

TEST(BRepGraphAlgo_ValidateTest, DeepDetectsIdDriftButLightweightSkipsIt)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);

  BRepGraph_MutRef<BRepGraphInc::FaceDef> aFaceDef = aGraph.Builder().MutFace(BRepGraph_FaceId(0));
  aFaceDef->Id                                     = BRepGraph_NodeId::Face(42);

  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_TRUE(aLightResult.IsValid());

  const BRepGraphAlgo_Validate::Result aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
  EXPECT_FALSE(aDeepResult.IsValid());

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aDefaultResult.IsValid());
}

TEST(BRepGraphAlgo_ValidateTest, DeepAudit_ValidatesCoEdgeUIDsFromBuilderWireCreation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbEdges(), 0);

  NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> anEdges;
  anEdges.Append(std::make_pair(BRepGraph_NodeId::Edge(0), TopAbs_FORWARD));
  const BRepGraph_NodeId aWireId = aGraph.Builder().AddWire(anEdges);
  ASSERT_TRUE(aWireId.IsValid());

  const NCollection_Vector<BRepGraph_CoEdgeRefId> aWireRefIds =
    coEdgeRefsOfWire(aGraph, BRepGraph_WireId(aWireId.Index));
  ASSERT_EQ(aWireRefIds.Length(), 1);
  const BRepGraph_NodeId aCoEdgeId =
    BRepGraph_NodeId::CoEdge(aGraph.Refs().CoEdge(aWireRefIds.Value(0)).CoEdgeDefId.Index);
  EXPECT_TRUE(aGraph.UIDs().Of(aCoEdgeId).IsValid());

  const BRepGraphAlgo_Validate::Result aDeepResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::DeepAudit());
  if (!aDeepResult.IsValid())
  {
    for (int anIdx = 0; anIdx < aDeepResult.Issues.Length(); ++anIdx)
    {
      const BRepGraphAlgo_Validate::Issue& anIssue = aDeepResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(aDeepResult.IsValid());
  EXPECT_EQ(aDeepResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}
