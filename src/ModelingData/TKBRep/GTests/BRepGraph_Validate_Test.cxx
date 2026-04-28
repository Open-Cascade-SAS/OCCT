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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <NCollection_IncAllocator.hxx>

#include <gtest/gtest.h>

namespace
{

//=================================================================================================

NCollection_DynamicArray<BRepGraph_CoEdgeRefId> coEdgeRefsOfWire(const BRepGraph&       theGraph,
                                                                 const BRepGraph_WireId theWireId)
{
  NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph_NodeId                          aParentNode   = theWireId;
  const BRepGraph::RefsView&                      aRefs         = theGraph.Refs();
  const int                                       aNbCoEdgeRefs = aRefs.CoEdges().Nb();
  for (BRepGraph_CoEdgeRefId aRefId(0); aRefId.IsValid(aNbCoEdgeRefs); ++aRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = aRefs.CoEdges().Entry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

} // namespace

TEST(BRepGraph_ValidateTest, CleanGraph_NoIssues)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_Validate::Result aResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aResult.IsValid());
  EXPECT_EQ(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
  EXPECT_EQ(aResult.NbIssues(BRepGraph_Validate::Severity::Warning), 0);

  const BRepGraph_Validate::Options anAuditOpts  = BRepGraph_Validate::Options::Audit();
  const BRepGraph_Validate::Result anAuditResult = BRepGraph_Validate::Perform(aGraph, anAuditOpts);
  EXPECT_TRUE(anAuditResult.IsValid());
  EXPECT_EQ(anAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  const BRepGraph_Validate::Options aLightOpts   = BRepGraph_Validate::Options::Lightweight();
  const BRepGraph_Validate::Result  aLightResult = BRepGraph_Validate::Perform(aGraph, aLightOpts);
  EXPECT_TRUE(aLightResult.IsValid());
  EXPECT_EQ(aLightResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

TEST(BRepGraph_ValidateTest, AfterGeomDeduplicate_NoIssues)
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  (void)BRepGraph_Deduplicate::Perform(aGraph);

  const BRepGraph_Validate::Result aResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aResult.IsValid());
  EXPECT_EQ(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

TEST(BRepGraph_ValidateTest, DetectsRemovedNodeReference)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find an edge that has a valid start vertex and remove it.
  BRepGraph_VertexId aVtxToRemove;
  const int          aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::VertexRef& aStartRef =
      BRepGraph_Tool::Edge::StartVertexRef(aGraph, anEdgeId);
    if (aStartRef.VertexDefId.IsValid())
    {
      aVtxToRemove = aStartRef.VertexDefId;
      break;
    }
  }
  ASSERT_TRUE(aVtxToRemove.IsValid());

  // Remove the vertex without fixing edges referencing it.
  aGraph.Editor().Gen().RemoveNode(aVtxToRemove);

  const BRepGraph_Validate::Result aDefaultResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aDefaultResult.IsValid());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
  EXPECT_GT(anAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

TEST(BRepGraph_ValidateTest, WireConnectivity_DisconnectedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Wires().Nb(), 0);

  // Corrupt a wire by swapping its vertex reference to break connectivity.
  // Find a wire with at least 2 edges.
  BRepGraph_WireId aTargetWire;
  const int        aNbWires = aGraph.Topo().Wires().Nb();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    if (coEdgeRefsOfWire(aGraph, aWireId).Length() >= 2)
    {
      aTargetWire = aWireId;
      break;
    }
  }
  ASSERT_TRUE(aTargetWire.IsValid());

  // Get the first edge in the wire and corrupt its end vertex.
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aWireRefIds =
    coEdgeRefsOfWire(aGraph, aTargetWire);
  ASSERT_GE(aWireRefIds.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aFirstCR = aGraph.Refs().CoEdges().Entry(aWireRefIds.Value(0));
  const BRepGraphInc::CoEdgeDef& aFirstCoEdge =
    aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(aFirstCR.CoEdgeDefId));
  const BRepGraph_NodeId aFirstEdgeId(aFirstCoEdge.EdgeDefId);
  ASSERT_TRUE(aFirstEdgeId.IsValid());

  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aFirstEdge =
    aGraph.Editor().Edges().Mut(BRepGraph_EdgeId(aFirstEdgeId));

  // Find a vertex different from the current end vertex.
  const BRepGraph_VertexId anOrigEndVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).VertexDefId;
  const BRepGraph_VertexId anOrigStartVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->StartVertexRefId).VertexDefId;
  const BRepGraph_NodeId anOrigEnd   = BRepGraph_NodeId(anOrigEndVtx);
  const int              aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    if (aVertexId != anOrigEnd && aVertexId != BRepGraph_NodeId(anOrigStartVtx))
    {
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aMutEndRef =
        aGraph.Editor().Vertices().MutRef(aFirstEdge->EndVertexRefId);
      aMutEndRef->VertexDefId = aVertexId;
      break;
    }
  }

  ASSERT_NE(aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).VertexDefId, anOrigEndVtx);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());

  // Check that at least one connectivity error was found.
  bool aFoundConnectivity = false;
  for (int anIdx = 0; anIdx < aResult.Issues.Length(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error)
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

TEST(BRepGraph_ValidateTest, BoundsCheck_InvalidIndex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt edge's Curve3d to null.
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
    aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
  anEdge->Curve3DRepId = BRepGraph_Curve3DRepId();

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_GT(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

TEST(BRepGraph_ValidateTest, AfterSplitEdge_ProducesSubEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  const int anOrigEdgeCount = aGraph.Topo().Edges().Nb();

  // Find a non-degenerate edge with valid vertices to split.
  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const int        aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anCandEdgeId(0); anCandEdgeId.IsValid(aNbEdges); ++anCandEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(anCandEdgeId);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid()
        && BRepGraph_Tool::Edge::StartVertexRef(aGraph, anCandEdgeId).VertexDefId.IsValid()
        && BRepGraph_Tool::Edge::EndVertexRef(aGraph, anCandEdgeId).VertexDefId.IsValid())
    {
      anEdgeId    = anCandEdgeId;
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  // Create a split vertex at the midpoint.
  gp_Pnt aMidPt;
  BRepGraph_Tool::Edge::Curve(aGraph, anEdgeId)->D0(aSplitParam, aMidPt);
  BRepGraph_VertexId aSplitVtx =
    aGraph.Editor().Vertices().Add(aMidPt, BRepGraph_Tool::Edge::Tolerance(aGraph, anEdgeId));

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(anEdgeId, aSplitVtx, aSplitParam, aSubA, aSubB);

  // Two new sub-edges should have been created.
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), anOrigEdgeCount + 2);
  EXPECT_TRUE(aSubA.IsValid());
  EXPECT_TRUE(aSubB.IsValid());

  // Original edge should be marked removed.
  EXPECT_TRUE(aGraph.Topo().Edges().Definition(anEdgeId).IsRemoved);

  // Full Audit must remain clean: every new CoEdge must carry a Curve2DRep,
  // no orphan VertexRefs, no reverse-index drift.
  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(anAuditResult.IsValid())
    << "Audit must remain clean after splitting a non-seam box edge";
}

TEST(BRepGraph_ValidateTest, CorruptedPCurve_FaceDefIdOutOfBounds)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt a CoEdge's FaceDefId to an out-of-range value.
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0);
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdgeDef =
    aGraph.Editor().CoEdges().Mut(BRepGraph_CoEdgeId::Start());
  aCoEdgeDef->FaceDefId = BRepGraph_FaceId(aGraph.Topo().Faces().Nb() + 999);

  const BRepGraph_Validate::Result aDefaultResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_FALSE(aDefaultResult.IsValid());

  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
}

TEST(BRepGraph_ValidateTest, LightweightAndAudit_DetectActiveCountDrift)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbActiveFacesBefore = aGraph.Topo().Faces().NbActive();
  ASSERT_GT(aNbActiveFacesBefore, 0);

  // Intentionally bypass RemoveNode() to simulate counter drift bug class.
  BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef =
    aGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());
  aFaceDef->IsRemoved = true;

  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());
  EXPECT_GT(aLightResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

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

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
  EXPECT_GT(anAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  bool aFoundActiveCountMismatch = false;
  for (int anIdx = 0; anIdx < anAuditResult.Issues.Length(); ++anIdx)
  {
    const TCollection_AsciiString& aDesc = anAuditResult.Issues.Value(anIdx).Description;
    if (aDesc.Search("NbActiveFaces mismatch") > 0)
    {
      aFoundActiveCountMismatch = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundActiveCountMismatch);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), aNbActiveFacesBefore);
}

TEST(BRepGraph_ValidateTest, Audit_ValidatesCoEdgeUIDsFromBuilderWireCreation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> anEdges;
  anEdges.Append(std::make_pair(BRepGraph_EdgeId::Start(), TopAbs_FORWARD));
  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(anEdges);
  ASSERT_TRUE(aWireId.IsValid());

  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aWireRefIds =
    coEdgeRefsOfWire(aGraph, aWireId);
  ASSERT_EQ(aWireRefIds.Length(), 1);
  const BRepGraph_NodeId aCoEdgeId =
    BRepGraph_CoEdgeId(aGraph.Refs().CoEdges().Entry(aWireRefIds.Value(0)).CoEdgeDefId.Index);
  EXPECT_TRUE(aGraph.UIDs().Of(aCoEdgeId).IsValid());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  if (!anAuditResult.IsValid())
  {
    for (int anIdx = 0; anIdx < anAuditResult.Issues.Length(); ++anIdx)
    {
      const BRepGraph_Validate::Issue& anIssue = anAuditResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(anAuditResult.IsValid());
  EXPECT_EQ(anAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

// ---------------------------------------------------------------------------
// Assembly validation tests
// ---------------------------------------------------------------------------

TEST(BRepGraph_ValidateTest, AssemblyGraph_ValidProduct_NoIssuesInAudit)
{
  // Build a box; BRepGraph_Builder::Add() auto-creates a root part product.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);

  // BRepGraph_Builder::Add() auto-creates the part product at index 0.
  const BRepGraph_ProductId aPartProduct = BRepGraph_ProductId::Start();
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartProduct));

  // Explicitly create an assembly product and add two occurrences of the part.
  const BRepGraph_ProductId aAssemblyProduct = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aAssemblyProduct.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().LinkProducts(aAssemblyProduct, aPartProduct, TopLoc_Location());
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().LinkProducts(aAssemblyProduct, aPartProduct, TopLoc_Location(aTrsf));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // Verify assembly structure.
  EXPECT_TRUE(aGraph.Topo().Products().IsAssembly(aAssemblyProduct));
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyProduct), 2);

  // Rebuild reverse index after assembly modifications.
  aGraph.Editor().CommitMutation();

  // Audit should pass on the explicitly constructed assembly.
  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  if (!aAuditResult.IsValid())
  {
    for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
    {
      const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(aAuditResult.IsValid());
}

TEST(BRepGraph_ValidateTest, AssemblyGraph_CorruptedOccurrenceChildDefId_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Occurrences().Nb(), 1);

  // Corrupt the first occurrence's ChildDefId to an out-of-bounds value.
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(BRepGraph_OccurrenceId::Start());
  anOccDef->ChildDefId =
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb() + 999);

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence with out-of-bounds ChildDefId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("ChildDefId invalid") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError) << "Audit should report 'OccurrenceDef.ChildDefId invalid'.";
}

TEST(BRepGraph_ValidateTest,
     AssemblyGraph_CorruptedOccurrenceChildDefId_ProductIndex_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Create an assembly with one occurrence.
  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aRootAssembly.IsValid());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartId));
  ASSERT_TRUE(aPartId.IsValid());

  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().LinkProducts(aRootAssembly, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Corrupt the occurrence's ChildDefId to an invalid index.
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(anOccId);
  anOccDef->ChildDefId =
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, aGraph.Topo().Products().Nb() + 999);

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence with out-of-bounds ChildDefId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("ChildDefId invalid") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError) << "Audit should report 'OccurrenceDef.ChildDefId invalid'.";
}

TEST(BRepGraph_ValidateTest, AssemblyGraph_OccurrenceChildRefersToOccurrence_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Occurrences().Nb(), 0);

  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(BRepGraph_OccurrenceId::Start());
  anOccDef->ChildDefId = BRepGraph_OccurrenceId::Start();

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence child pointing to another occurrence should be detected by audit.";

  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("cannot reference an Occurrence") >= 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError)
    << "Audit should report that OccurrenceDef.ChildDefId cannot reference an Occurrence.";
}

TEST(BRepGraph_ValidateTest, LightweightVsAudit_RemovedVertexReference_Differential)
{
  // Verifies that removed-node isolation is an Audit-only check.
  // RemoveNode(vertex) correctly updates active counts (Lightweight passes)
  // but leaves edges referencing the removed vertex (Audit detects).
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find a vertex referenced by at least one edge.
  BRepGraph_VertexId aVtxToRemove;
  const int          aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::VertexRef& aStartRef =
      BRepGraph_Tool::Edge::StartVertexRef(aGraph, anEdgeId);
    if (aStartRef.VertexDefId.IsValid())
    {
      aVtxToRemove = aStartRef.VertexDefId;
      break;
    }
  }
  ASSERT_TRUE(aVtxToRemove.IsValid()) << "Need a vertex referenced by an edge.";

  // Remove the vertex. RemoveNode correctly decrements active count
  // but does NOT fix edges that still reference it.
  aGraph.Editor().Gen().RemoveNode(aVtxToRemove);

  // Lightweight only checks active counts - should pass.
  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aLightResult.IsValid()) << "Lightweight should not check removed-node isolation.";

  // Audit runs checkRemovedNodeIsolation - should detect the dangling reference.
  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid()) << "Audit should detect edges referencing a removed vertex.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("references removed") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError)
    << "Audit should report 'Non-removed EdgeDef references removed StartVertexEntity'.";
}

TEST(BRepGraph_ValidateTest, Audit_DetectsOrphanWireRef_AfterFaceRemoval)
{
  // Build a simple box; pick one face; corrupt its WireRef so the parent points
  // at a removed face index. The new audit block for orphan WireRefs must flag it.
  BRepGraph          aGraph;
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Find a live wire ref and rewrite its ParentId to a definitely-invalid Face id.
  const BRepGraph::RefsView& aRefs = aGraph.Refs();
  ASSERT_GT(aRefs.Wires().Nb(), 0);
  bool aDidCorrupt = false;
  for (int aRefIdx = 0; aRefIdx < aRefs.Wires().Nb() && !aDidCorrupt; ++aRefIdx)
  {
    const BRepGraph_WireRefId    aRefId(aRefIdx);
    const BRepGraphInc::WireRef& aRef = aRefs.Wires().Entry(aRefId);
    if (aRef.IsRemoved)
      continue;
    BRepGraph_MutGuard<BRepGraphInc::WireRef> aMut = aGraph.Editor().Wires().MutRef(aRefId);
    aMut->ParentId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 9999);
    aDidCorrupt    = true;
  }
  ASSERT_TRUE(aDidCorrupt);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());
  bool aFound = false;
  for (int i = 0; i < aResult.Issues.Length(); ++i)
  {
    if (aResult.Issues.Value(i).Description.Search("Orphan WireRef") >= 0)
    {
      aFound = true;
      break;
    }
  }
  EXPECT_TRUE(aFound) << "Expected 'Orphan WireRef' issue from audit validate";
}
