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
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
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

NCollection_Vector<BRepGraph_CoEdgeRefId> coEdgeRefsOfWire(const BRepGraph&       theGraph,
                                                           const BRepGraph_WireId theWireId)
{
  NCollection_Vector<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph_NodeId                    aParentNode = BRepGraph_WireId(theWireId.Index);
  const BRepGraph::RefsView&                aRefs       = theGraph.Refs();
  for (int aRefIdx = 0; aRefIdx < aRefs.CoEdges().Nb(); ++aRefIdx)
  {
    const BRepGraph_CoEdgeRefId    aRefId(aRefIdx);
    const BRepGraphInc::CoEdgeRef& aRef = aRefs.CoEdges().Entry(aRefId);
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

  const BRepGraphAlgo_Validate::Options anAuditOpts = BRepGraphAlgo_Validate::Options::Audit();
  const BRepGraphAlgo_Validate::Result  anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, anAuditOpts);
  EXPECT_TRUE(anAuditResult.IsValid());
  EXPECT_EQ(anAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

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
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find an edge that has a valid start vertex and remove it.
  int aVtxToRemove = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().Edges().Nb(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId         anEdgeId(anEdgeIdx);
    const BRepGraphInc::VertexRef& aStartRef = BRepGraph_Tool::Edge::StartVertex(aGraph, anEdgeId);
    if (aStartRef.VertexDefId.IsValid())
    {
      aVtxToRemove = aStartRef.VertexDefId.Index;
      break;
    }
  }
  ASSERT_GE(aVtxToRemove, 0);

  // Remove the vertex without fixing edges referencing it.
  aGraph.Builder().RemoveNode(BRepGraph_VertexId(aVtxToRemove));

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aDefaultResult.IsValid());

  const BRepGraphAlgo_Validate::Result anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
  EXPECT_GT(anAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

TEST(BRepGraphAlgo_ValidateTest, WireConnectivity_DisconnectedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Wires().Nb(), 0);

  // Corrupt a wire by swapping its vertex reference to break connectivity.
  // Find a wire with at least 2 edges.
  int aTargetWire = -1;
  for (int aWireIdx = 0; aWireIdx < aGraph.Topo().Wires().Nb(); ++aWireIdx)
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
  const BRepGraphInc::CoEdgeRef& aFirstCR = aGraph.Refs().CoEdges().Entry(aWireRefIds.Value(0));
  const BRepGraphInc::CoEdgeDef& aFirstCoEdge =
    aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(aFirstCR.CoEdgeDefId));
  const BRepGraph_NodeId aFirstEdgeId(aFirstCoEdge.EdgeDefId);
  ASSERT_TRUE(aFirstEdgeId.IsValid());

  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aFirstEdge =
    aGraph.Builder().MutEdge(BRepGraph_EdgeId(aFirstEdgeId.Index));

  // Find a vertex different from the current end vertex.
  const BRepGraph_VertexId anOrigEndVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).VertexDefId;
  const BRepGraph_VertexId anOrigStartVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->StartVertexRefId).VertexDefId;
  const BRepGraph_NodeId anOrigEnd = BRepGraph_NodeId(anOrigEndVtx);
  for (int aVtxIdx = 0; aVtxIdx < aGraph.Topo().Vertices().Nb(); ++aVtxIdx)
  {
    if (BRepGraph_VertexId(aVtxIdx) != anOrigEnd
        && BRepGraph_VertexId(aVtxIdx) != BRepGraph_NodeId(anOrigStartVtx))
    {
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aMutEndRef =
        aGraph.Builder().MutVertexRef(aFirstEdge->EndVertexRefId);
      aMutEndRef->VertexDefId = BRepGraph_VertexId(aVtxIdx);
      break;
    }
  }

  ASSERT_NE(aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).VertexDefId, anOrigEndVtx);

  const BRepGraphAlgo_Validate::Result aResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
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
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt edge's Curve3d to null.
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge = aGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
  anEdge->Curve3DRepId                           = BRepGraph_Curve3DRepId();

  const BRepGraphAlgo_Validate::Result aResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
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
  const int anOrigEdgeCount = aGraph.Topo().Edges().Nb();

  // Find a non-degenerate edge with valid vertices to split.
  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const BRepGraph_EdgeId       anCandEdgeId(i);
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(anCandEdgeId);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid()
        && BRepGraph_Tool::Edge::StartVertex(aGraph, anCandEdgeId).VertexDefId.IsValid()
        && BRepGraph_Tool::Edge::EndVertex(aGraph, anCandEdgeId).VertexDefId.IsValid())
    {
      anEdgeId    = BRepGraph_EdgeId(i);
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  // Create a split vertex at the midpoint.
  gp_Pnt aMidPt;
  BRepGraph_Tool::Edge::Curve(aGraph, anEdgeId)->D0(aSplitParam, aMidPt);
  BRepGraph_VertexId aSplitVtx = aGraph.Builder().AddVertex(
    aMidPt,
    BRepGraph_Tool::Edge::Tolerance(aGraph, anEdgeId));

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Builder().SplitEdge(anEdgeId, aSplitVtx, aSplitParam, aSubA, aSubB);

  // Two new sub-edges should have been created.
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), anOrigEdgeCount + 2);
  EXPECT_TRUE(aSubA.IsValid());
  EXPECT_TRUE(aSubB.IsValid());

  // Original edge should be marked removed.
  EXPECT_TRUE(aGraph.Topo().Edges().Definition(anEdgeId).IsRemoved);
}

TEST(BRepGraphAlgo_ValidateTest, CorruptedPCurve_FaceDefIdOutOfBounds)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt a CoEdge's FaceDefId to an out-of-range value.
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0);
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdgeDef =
    aGraph.Builder().MutCoEdge(BRepGraph_CoEdgeId(0));
  aCoEdgeDef->FaceDefId = BRepGraph_FaceId(aGraph.Topo().Faces().Nb() + 999);

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_FALSE(aDefaultResult.IsValid());

  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraphAlgo_Validate::Result anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
}

TEST(BRepGraphAlgo_ValidateTest, LightweightAndAudit_DetectActiveCountDrift)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbActiveFacesBefore = aGraph.Topo().Faces().NbActive();
  ASSERT_GT(aNbActiveFacesBefore, 0);

  // Intentionally bypass RemoveNode() to simulate counter drift bug class.
  BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef = aGraph.Builder().MutFace(BRepGraph_FaceId(0));
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

  const BRepGraphAlgo_Validate::Result anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
  EXPECT_GT(anAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

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

TEST(BRepGraphAlgo_ValidateTest, DeepDetectsIdDriftButLightweightSkipsIt)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef = aGraph.Builder().MutFace(BRepGraph_FaceId(0));
  aFaceDef->Id                                     = BRepGraph_FaceId(42);

  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_TRUE(aLightResult.IsValid());

  const BRepGraphAlgo_Validate::Result anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());

  const BRepGraphAlgo_Validate::Result aDefaultResult = BRepGraphAlgo_Validate::Perform(aGraph);
  EXPECT_TRUE(aDefaultResult.IsValid());
}

TEST(BRepGraphAlgo_ValidateTest, Audit_ValidatesCoEdgeUIDsFromBuilderWireCreation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> anEdges;
  anEdges.Append(std::make_pair(BRepGraph_EdgeId(0), TopAbs_FORWARD));
  const BRepGraph_WireId aWireId = aGraph.Builder().AddWire(anEdges);
  ASSERT_TRUE(aWireId.IsValid());

  const NCollection_Vector<BRepGraph_CoEdgeRefId> aWireRefIds =
    coEdgeRefsOfWire(aGraph, aWireId);
  ASSERT_EQ(aWireRefIds.Length(), 1);
  const BRepGraph_NodeId aCoEdgeId =
    BRepGraph_CoEdgeId(aGraph.Refs().CoEdges().Entry(aWireRefIds.Value(0)).CoEdgeDefId.Index);
  EXPECT_TRUE(aGraph.UIDs().Of(aCoEdgeId).IsValid());

  const BRepGraphAlgo_Validate::Result anAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  if (!anAuditResult.IsValid())
  {
    for (int anIdx = 0; anIdx < anAuditResult.Issues.Length(); ++anIdx)
    {
      const BRepGraphAlgo_Validate::Issue& anIssue = anAuditResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(anAuditResult.IsValid());
  EXPECT_EQ(anAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);
}

// ---------------------------------------------------------------------------
// Assembly validation tests
// ---------------------------------------------------------------------------

TEST(BRepGraphAlgo_ValidateTest, AssemblyGraph_ValidProduct_NoIssuesInAudit)
{
  // Build a box; Build() auto-creates a root part product.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);

  // Identify the auto-created part product.
  const occ::handle<NCollection_BaseAllocator> anAllocator = new NCollection_IncAllocator();
  const NCollection_Vector<BRepGraph_ProductId> aInitialRootProducts =
    aGraph.Topo().Products().RootProducts(anAllocator);
  ASSERT_GT(aInitialRootProducts.Length(), 0);
  const BRepGraph_ProductId aPartProduct = aInitialRootProducts.Value(0);
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartProduct));

  // Explicitly create an assembly product and add two occurrences of the part.
  const BRepGraph_ProductId aAssemblyProduct = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aAssemblyProduct.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Builder().AddOccurrence(aAssemblyProduct, aPartProduct, TopLoc_Location());
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Builder().AddOccurrence(aAssemblyProduct, aPartProduct, TopLoc_Location(aTrsf));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // Verify assembly structure.
  EXPECT_TRUE(aGraph.Topo().Products().IsAssembly(aAssemblyProduct));
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyProduct), 2);

  // Rebuild reverse index after assembly modifications.
  aGraph.Builder().CommitMutation();

  // Audit should pass on the explicitly constructed assembly.
  const BRepGraphAlgo_Validate::Result aAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  if (!aAuditResult.IsValid())
  {
    for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
    {
      const BRepGraphAlgo_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(aAuditResult.IsValid());
}

TEST(BRepGraphAlgo_ValidateTest, AssemblyGraph_CorruptedProductShapeRootId_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);

  // Corrupt ShapeRootId to an out-of-bounds value.
  BRepGraph_MutGuard<BRepGraphInc::ProductDef> aProduct =
    aGraph.Builder().MutProduct(BRepGraph_ProductId(0));
  aProduct->ShapeRootId = BRepGraph_SolidId(aGraph.Topo().Solids().Nb() + 1);

  const BRepGraphAlgo_Validate::Result aAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Product with out-of-bounds ShapeRootId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraphAlgo_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraphAlgo_Validate::Severity::Error
        && anIssue.Description.Search("ShapeRootId out of bounds") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError)
    << "Audit should report 'ProductDef.ShapeRootId out of bounds'.";
}

TEST(BRepGraphAlgo_ValidateTest, AssemblyGraph_CorruptedOccurrenceProductDefId_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Create an assembly with one occurrence.
  const BRepGraph_ProductId aRootAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aRootAssembly.IsValid());

  const occ::handle<NCollection_BaseAllocator> anAllocator = new NCollection_IncAllocator();
  const NCollection_Vector<BRepGraph_ProductId> aRootProducts = aGraph.Topo().Products().RootProducts(anAllocator);
  BRepGraph_ProductId                           aPartId;
  for (int i = 0; i < aRootProducts.Length(); ++i)
  {
    const BRepGraph_ProductId aProductId = aRootProducts.Value(i);
    if (aGraph.Topo().Products().IsPart(aProductId))
    {
      aPartId = aRootProducts.Value(i);
      break;
    }
  }
  ASSERT_TRUE(aPartId.IsValid());

  const BRepGraph_OccurrenceId anOccId =
    aGraph.Builder().AddOccurrence(aRootAssembly, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Corrupt the occurrence's ProductDefId to an invalid index.
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Builder().MutOccurrence(anOccId);
  anOccDef->ProductDefId = BRepGraph_ProductId(aGraph.Topo().Products().Nb() + 1);

  const BRepGraphAlgo_Validate::Result aAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence with out-of-bounds ProductDefId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraphAlgo_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraphAlgo_Validate::Severity::Error
        && anIssue.Description.Search("ProductDefId invalid") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError)
    << "Audit should report 'OccurrenceDef.ProductDefId invalid'.";
}

TEST(BRepGraphAlgo_ValidateTest, LightweightVsAudit_RemovedVertexReference_Differential)
{
  // Verifies that removed-node isolation is an Audit-only check.
  // RemoveNode(vertex) correctly updates active counts (Lightweight passes)
  // but leaves edges referencing the removed vertex (Audit detects).
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find a vertex referenced by at least one edge.
  int aVtxToRemove = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().Edges().Nb(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId         anEdgeId(anEdgeIdx);
    const BRepGraphInc::VertexRef& aStartRef = BRepGraph_Tool::Edge::StartVertex(aGraph, anEdgeId);
    if (aStartRef.VertexDefId.IsValid())
    {
      aVtxToRemove = aStartRef.VertexDefId.Index;
      break;
    }
  }
  ASSERT_GE(aVtxToRemove, 0) << "Need a vertex referenced by an edge.";

  // Remove the vertex. RemoveNode correctly decrements active count
  // but does NOT fix edges that still reference it.
  aGraph.Builder().RemoveNode(BRepGraph_VertexId(aVtxToRemove));

  // Lightweight only checks active counts - should pass.
  const BRepGraphAlgo_Validate::Result aLightResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Lightweight());
  EXPECT_TRUE(aLightResult.IsValid())
    << "Lightweight should not check removed-node isolation.";

  // Audit runs checkRemovedNodeIsolation - should detect the dangling reference.
  const BRepGraphAlgo_Validate::Result aAuditResult =
    BRepGraphAlgo_Validate::Perform(aGraph, BRepGraphAlgo_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Audit should detect edges referencing a removed vertex.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraphAlgo_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (int anIdx = 0; anIdx < aAuditResult.Issues.Length(); ++anIdx)
  {
    const BRepGraphAlgo_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraphAlgo_Validate::Severity::Error
        && anIssue.Description.Search("references removed") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError)
    << "Audit should report 'Non-removed EdgeDef references removed StartVertexEntity'.";
}

