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
#include <BRepGraph_Iterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>

#include <cmath>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_LinearVector.hxx>

#include <gtest/gtest.h>

namespace
{

//=================================================================================================

const NCollection_LinearVector<BRepGraph_CoEdgeId>& coEdgesOfWire(const BRepGraph&       theGraph,
                                                                  const BRepGraph_WireId theWireId)
{
  return theGraph.Topo().Wires().Relations(theWireId).CoEdgeIds;
}

BRepGraph_EdgeId edgeWithCurve(const BRepGraph&       theGraph,
                               const BRepGraph_EdgeId theSkip = BRepGraph_EdgeId())
{
  for (BRepGraph_EdgeIterator anIt(theGraph); anIt.More(); anIt.Next())
  {
    if (anIt.CurrentId() != theSkip && anIt.Current().Curve3DRepId.IsValid())
    {
      return anIt.CurrentId();
    }
  }
  return BRepGraph_EdgeId();
}

bool hasErrorContaining(const BRepGraph_Validate::Result& theResult, const char* theNeedle)
{
  for (const BRepGraph_Validate::Issue& anIssue : theResult.Issues)
  {
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search(theNeedle) >= 1)
    {
      return true;
    }
  }
  return false;
}

} // namespace

TEST(BRepGraph_ValidateTest, CleanGraph_NoIssues)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  [[maybe_unused]] const BRepGraph_Deduplicate::Result aDedupResult =
    BRepGraph_Deduplicate::Perform(aGraph);

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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find an edge that has a valid start vertex, then corrupt its ref to point to a removed vertex.
  BRepGraph_VertexId    aVtxToRemove;
  BRepGraph_VertexRefId aRefToCorrupt;
  const uint32_t        aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraph_VertexRefId aStartRef = BRepGraph_Tool::Edge::StartVertexId(aGraph, anEdgeId);
    const BRepGraph_Tool::VertexUsage aStart = BRepGraph_Tool::Vertex::Usage(aGraph, aStartRef);
    if (aStart.IsValid())
    {
      aRefToCorrupt = aStartRef;
      break;
    }
  }
  aVtxToRemove = aGraph.Editor().Vertices().Add(gp_Pnt(100.0, 0.0, 0.0), Precision::Confusion());
  ASSERT_TRUE(aVtxToRemove.IsValid());
  ASSERT_TRUE(aRefToCorrupt.IsValid());

  aGraph.Editor().Gen().RemoveNode(aVtxToRemove);
  BRepGraph_MutGuard<BRepGraphInc::VertexRef> aRefMut =
    aGraph.Editor().Vertices().MutRef(aRefToCorrupt);
  aRefMut.Internal().ChildVertexId = aVtxToRemove;

  const BRepGraph_Validate::Result aDefaultResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_FALSE(aDefaultResult.IsValid());

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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Wires().Nb(), 0);

  // Corrupt a wire by swapping its vertex reference to break connectivity.
  // Find a wire with at least 2 edges.
  BRepGraph_WireId aTargetWire;
  const uint32_t   aNbWires = aGraph.Topo().Wires().Nb();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    if (coEdgesOfWire(aGraph, aWireId).Size() >= 2)
    {
      aTargetWire = aWireId;
      break;
    }
  }
  ASSERT_TRUE(aTargetWire.IsValid());

  // Get the first edge in the wire and corrupt its end vertex.
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdgeIds =
    coEdgesOfWire(aGraph, aTargetWire);
  ASSERT_GE(aWireCoEdgeIds.Size(), 1);
  const BRepGraphInc::CoEdgeDef& aFirstCoEdge =
    aGraph.Topo().CoEdges().Definition(aWireCoEdgeIds.Value(0));
  const BRepGraph_NodeId aFirstEdgeId(aFirstCoEdge.ChildEdgeId);
  ASSERT_TRUE(aFirstEdgeId.IsValid());

  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aFirstEdge =
    aGraph.Editor().Edges().Mut(BRepGraph_EdgeId(aFirstEdgeId));

  // Find a vertex different from the current end vertex.
  const BRepGraph_VertexId anOrigEndVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).ChildVertexId;
  const BRepGraph_VertexId anOrigStartVtx =
    aGraph.Refs().Vertices().Entry(aFirstEdge->StartVertexRefId).ChildVertexId;
  const BRepGraph_NodeId anOrigEnd   = BRepGraph_NodeId(anOrigEndVtx);
  const uint32_t         aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    if (aVertexId != anOrigEnd && aVertexId != BRepGraph_NodeId(anOrigStartVtx))
    {
      BRepGraph_MutGuard<BRepGraphInc::VertexRef> aMutEndRef =
        aGraph.Editor().Vertices().MutRef(aFirstEdge->EndVertexRefId);
      aMutEndRef.Internal().ChildVertexId = aVertexId;
      break;
    }
  }

  ASSERT_NE(aGraph.Refs().Vertices().Entry(aFirstEdge->EndVertexRefId).ChildVertexId, anOrigEndVtx);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(hasErrorContaining(aResult, "Wire edges not connected"));

  // Check that at least one connectivity warning was found.
  bool           aFoundConnectivity = false;
  const uint32_t aNbIssues          = static_cast<uint32_t>(aResult.Issues.Size());
  for (uint32_t anIdx = 0; anIdx < aNbIssues; ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Warning)
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt edge's Curve3d to null.
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
    aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
  anEdge.Internal().Curve3DRepId = BRepGraph_EdgeCurve3DRepId();

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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  const uint32_t anOrigEdgeCount = aGraph.Topo().Edges().Nb();

  // Find a non-degenerate edge with valid vertices to split.
  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const uint32_t   aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anCandEdgeId(0); anCandEdgeId.IsValid(aNbEdges); ++anCandEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(anCandEdgeId);
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anCandEdgeId) && anEdgeDef.Curve3DRepId.IsValid()
        && BRepGraph_Tool::Edge::StartVertexId(aGraph, anCandEdgeId).IsValid()
        && BRepGraph_Tool::Edge::EndVertexId(aGraph, anCandEdgeId).IsValid())
    {
      anEdgeId = anCandEdgeId;
      {
        const auto _r = BRepGraph_Tool::Edge::Range(aGraph, anCandEdgeId);
        aSplitParam   = 0.5 * (_r.first + _r.second);
      }
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
  EXPECT_TRUE(anEdgeId.IsRemoved(aGraph));

  // Full Audit must remain clean: every new CoEdge must carry a Curve2DRep,
  // no orphan VertexRefs, no relation-table drift.
  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(anAuditResult.IsValid())
    << "Audit must remain clean after splitting a non-seam box edge";
}

TEST(BRepGraph_ValidateTest, CorruptedPCurve_FaceIdOutOfBounds)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Corrupt a CoEdge's FaceId to an out-of-range value.
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0);
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aCoEdgeDef =
    aGraph.Editor().CoEdges().Mut(BRepGraph_CoEdgeId::Start());
  aCoEdgeDef.Internal().FaceId = BRepGraph_FaceId(aGraph.Topo().Faces().Nb() + 999);

  const BRepGraph_Validate::Result aDefaultResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_FALSE(aDefaultResult.IsValid());

  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(anAuditResult.IsValid());
}

TEST(BRepGraph_ValidateTest, RemoveNodeMaintainsActiveCount)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aNbActiveFacesBefore = aGraph.Topo().Faces().NbActive();
  ASSERT_GT(aNbActiveFacesBefore, 0);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_FaceId::Start()));

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());

  bool aFoundActiveCountMismatch = false;
  for (size_t anIdx = 0; anIdx < anAuditResult.Issues.Size(); ++anIdx)
  {
    const TCollection_AsciiString& aDesc = anAuditResult.Issues.Value(anIdx).Description;
    if (aDesc.Search("NbActiveFaces mismatch") > 0)
    {
      aFoundActiveCountMismatch = true;
      break;
    }
  }
  EXPECT_FALSE(aFoundActiveCountMismatch);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), aNbActiveFacesBefore - 1);
}

TEST(BRepGraph_ValidateTest, Audit_ValidatesCoEdgeUIDsFromBuilderWireCreation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIds;
  aCoEdgeIds.Append(aGraph.Editor().CoEdges().Add(BRepGraph_EdgeId::Start(), TopAbs_FORWARD));
  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aCoEdgeIds.ToArray1());
  ASSERT_TRUE(aWireId.IsValid());

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdgeIds =
    coEdgesOfWire(aGraph, aWireId);
  ASSERT_EQ(aWireCoEdgeIds.Size(), 1);
  const BRepGraph_NodeId aCoEdgeId = aWireCoEdgeIds.Value(0);
  EXPECT_TRUE(aGraph.UIDs().Of(aCoEdgeId).IsValid());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  if (!anAuditResult.IsValid())
  {
    for (size_t anIdx = 0; anIdx < anAuditResult.Issues.Size(); ++anIdx)
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
  // Build a box; BRepGraph::ShapesView::Add() auto-creates a root part product.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);

  // BRepGraph::ShapesView::Add() auto-creates the part product at index 0.
  const BRepGraph_ProductId aPartProduct = BRepGraph_ProductId::Start();
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartProduct));

  // Explicitly create an assembly product and add two occurrences of the part.
  const BRepGraph_ProductId aAssemblyProduct = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyProduct);
  ASSERT_TRUE(aAssemblyProduct.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyProduct, aPartProduct, TopLoc_Location());
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyProduct, aPartProduct, TopLoc_Location(aTrsf));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // Verify assembly structure.
  EXPECT_TRUE(aGraph.Topo().Products().IsAssembly(aAssemblyProduct));
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyProduct), 2);

  // Rebuild relation tables after assembly modifications.
  aGraph.Editor().CommitMutation();

  // Audit should pass on the explicitly constructed assembly.
  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  if (!aAuditResult.IsValid())
  {
    for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
    {
      const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
      ADD_FAILURE() << "Issue[" << anIdx << "] kind=" << static_cast<int>(anIssue.NodeId.NodeKind)
                    << " idx=" << anIssue.NodeId.Index
                    << " desc=" << anIssue.Description.ToCString();
    }
  }
  EXPECT_TRUE(aAuditResult.IsValid());
}

TEST(BRepGraph_ValidateTest, DocumentRootReferencedByOccurrence_Detected)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPartProduct = BRepGraph_ProductId::Start();
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartProduct));

  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);
  ASSERT_TRUE(aRootAssembly.IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aRootAssembly, aPartProduct, TopLoc_Location()).IsValid());

  ASSERT_EQ(aGraph.RootProductIds().Size(), 1);
  ASSERT_EQ(aGraph.RootProductIds().First(), aRootAssembly);

  // Deliberately reintroduce the child Product as a document root. This is the
  // malformed state that makes consumers start a tree from Product[0] even
  // though Product[0] already has an assembly parent.
  aGraph.Editor().Products().AppendDocumentRoot(aPartProduct);

  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid());

  bool aFoundRootIssue = false;
  for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.NodeId == BRepGraph_NodeId(aPartProduct)
        && anIssue.Description.Search("Document root Product is referenced") >= 0)
    {
      aFoundRootIssue = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundRootIssue);
}

TEST(BRepGraph_ValidateTest, AssemblyGraph_CorruptedOccurrenceChildNodeId_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Products().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Occurrences().Nb(), 1);

  // Corrupt the first occurrence's ChildNodeId to an out-of-bounds value.
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(BRepGraph_OccurrenceId::Start());
  anOccDef.Internal().ChildNodeId =
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb() + 999);

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence with out-of-bounds ChildNodeId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("ChildNodeId invalid") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError) << "Audit should report 'OccurrenceDef.ChildNodeId invalid'.";
}

TEST(BRepGraph_ValidateTest,
     AssemblyGraph_CorruptedOccurrenceChildNodeId_ProductIndex_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Create an assembly with one occurrence.
  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);
  ASSERT_TRUE(aRootAssembly.IsValid());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();
  ASSERT_TRUE(aGraph.Topo().Products().IsPart(aPartId));
  ASSERT_TRUE(aPartId.IsValid());

  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aRootAssembly, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Corrupt the occurrence's ChildNodeId to an invalid index.
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(anOccId);
  anOccDef.Internal().ChildNodeId =
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, aGraph.Topo().Products().Nb() + 999);

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence with out-of-bounds ChildNodeId should be detected by audit.";
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
  {
    const BRepGraph_Validate::Issue& anIssue = aAuditResult.Issues.Value(anIdx);
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("ChildNodeId invalid") > 0)
    {
      aFoundExpectedError = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundExpectedError) << "Audit should report 'OccurrenceDef.ChildNodeId invalid'.";
}

TEST(BRepGraph_ValidateTest, AssemblyGraph_OccurrenceChildRefersToOccurrence_DetectedByAudit)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Occurrences().Nb(), 0);

  BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> anOccDef =
    aGraph.Editor().Occurrences().Mut(BRepGraph_OccurrenceId::Start());
  anOccDef.Internal().ChildNodeId = BRepGraph_OccurrenceId::Start();

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid())
    << "Occurrence child pointing to another occurrence should be detected by audit.";

  bool aFoundExpectedError = false;
  for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
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
    << "Audit should report that OccurrenceDef.ChildNodeId cannot reference an Occurrence.";
}

TEST(BRepGraph_ValidateTest, LightweightVsAudit_RemovedVertexReference_Differential)
{
  // Removed-node isolation is now part of the lightweight/default contract too:
  // active refs must not target removed definitions.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  // Find a vertex referenced by at least one edge.
  BRepGraph_VertexId aVtxToRemove;
  const uint32_t     aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraph_VertexRefId aStartRef = BRepGraph_Tool::Edge::StartVertexId(aGraph, anEdgeId);
    const BRepGraph_Tool::VertexUsage aStart = BRepGraph_Tool::Vertex::Usage(aGraph, aStartRef);
    if (aStart.IsValid())
    {
      aVtxToRemove = aStart.DefId;
      break;
    }
  }
  ASSERT_TRUE(aVtxToRemove.IsValid()) << "Need a vertex referenced by an edge.";

  // Remove a loose vertex, then corrupt an active edge ref to point at it.
  const BRepGraph_VertexRefId aRefToCorrupt =
    BRepGraph_Tool::Edge::StartVertexId(aGraph, BRepGraph_EdgeId::Start());
  ASSERT_TRUE(aRefToCorrupt.IsValid());
  aVtxToRemove = aGraph.Editor().Vertices().Add(gp_Pnt(100.0, 0.0, 0.0), Precision::Confusion());
  aGraph.Editor().Gen().RemoveNode(aVtxToRemove);
  BRepGraph_MutGuard<BRepGraphInc::VertexRef> aRefMut =
    aGraph.Editor().Vertices().MutRef(aRefToCorrupt);
  aRefMut.Internal().ChildVertexId = aVtxToRemove;

  const BRepGraph_Validate::Result aLightResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_FALSE(aLightResult.IsValid());

  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aAuditResult.IsValid());
  EXPECT_GT(aAuditResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Verify the specific error message.
  bool aFoundExpectedError = false;
  for (size_t anIdx = 0; anIdx < aAuditResult.Issues.Size(); ++anIdx)
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

TEST(BRepGraph_ValidateTest, Audit_WarnsSurfacedFaceWithNoWireRefs)
{
  BRepGraph          aGraph;
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  bool aDidCorrupt = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More() && !aDidCorrupt; aFaceIt.Next())
  {
    const BRepGraph_FaceId             aFaceId        = aFaceIt.CurrentId();
    const BRepGraphInc::FaceRelations& aFaceRelations = aGraph.Topo().Faces().Relations(aFaceId);
    if (aFaceRelations.WireRefIds.IsEmpty())
    {
      continue;
    }
    ASSERT_TRUE(aGraph.Editor().Faces().RemoveWire(aFaceId, aFaceRelations.WireRefIds.Last()));
    aDidCorrupt = true;
  }
  ASSERT_TRUE(aDidCorrupt);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid());
  EXPECT_FALSE(aResult.Issues.IsEmpty());
  EXPECT_EQ(aResult.Issues.First().Sev, BRepGraph_Validate::Severity::Warning);
  EXPECT_NE(aResult.Issues.First().Description.Search("no wire refs"), -1);
}

TEST(BRepGraph_ValidateTest, Audit_DetectsSharedOwnedUse)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes43 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_EdgeId anEdgeId1 = edgeWithCurve(aGraph);
  ASSERT_TRUE(anEdgeId1.IsValid());
  const BRepGraph_EdgeId anEdgeId2 = edgeWithCurve(aGraph, anEdgeId1);
  ASSERT_TRUE(anEdgeId2.IsValid());
  const BRepGraph_EdgeCurve3DRepId aRepId1 =
    aGraph.Topo().Edges().Definition(anEdgeId1).Curve3DRepId;
  ASSERT_TRUE(aRepId1.IsValid());
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge = aGraph.Editor().Edges().Mut(anEdgeId2);
    anEdge.Internal().Curve3DRepId                   = aRepId1;
  }

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_TRUE(hasErrorContaining(aResult, "Active EdgeCurve3DRep has multiple owners"));
}

TEST(BRepGraph_ValidateTest, Audit_DetectsActiveOwnedUseWithoutOwner)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes44 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_EdgeId anEdgeId = edgeWithCurve(aGraph);
  ASSERT_TRUE(anEdgeId.IsValid());
  const BRepGraph_EdgeCurve3DRepId aRepId = aGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId;
  ASSERT_TRUE(aRepId.IsValid());

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge = aGraph.Editor().Edges().Mut(anEdgeId);
    anEdge.Internal().Curve3DRepId                   = BRepGraph_EdgeCurve3DRepId();
  }

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_TRUE(hasErrorContaining(aResult, "Active EdgeCurve3DRep has no owner"));
}

TEST(BRepGraph_ValidateTest, Synthetic_Box_AuditClean)
{
  BRepGraph aGraph;
  aGraph.Clear();

  auto&                    aVtxOps = aGraph.Editor().Vertices();
  const BRepGraph_VertexId aV0     = aVtxOps.Add(gp_Pnt(0, 0, 0), 1e-7);
  const BRepGraph_VertexId aV1     = aVtxOps.Add(gp_Pnt(10, 0, 0), 1e-7);
  const BRepGraph_VertexId aV2     = aVtxOps.Add(gp_Pnt(10, 20, 0), 1e-7);
  const BRepGraph_VertexId aV3     = aVtxOps.Add(gp_Pnt(0, 20, 0), 1e-7);
  const BRepGraph_VertexId aV4     = aVtxOps.Add(gp_Pnt(0, 0, 30), 1e-7);
  const BRepGraph_VertexId aV5     = aVtxOps.Add(gp_Pnt(10, 0, 30), 1e-7);
  const BRepGraph_VertexId aV6     = aVtxOps.Add(gp_Pnt(10, 20, 30), 1e-7);
  const BRepGraph_VertexId aV7     = aVtxOps.Add(gp_Pnt(0, 20, 30), 1e-7);

  auto&                  anEdgeOps = aGraph.Editor().Edges();
  const BRepGraph_EdgeId aE0 =
    anEdgeOps.Add(aV0, aV1, new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE1 =
    anEdgeOps.Add(aV1, aV2, new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0)), 0.0, 20.0, 1e-7);
  const BRepGraph_EdgeId aE2 =
    anEdgeOps.Add(aV2, aV3, new Geom_Line(gp_Pnt(10, 20, 0), gp_Dir(-1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE3 =
    anEdgeOps.Add(aV3, aV0, new Geom_Line(gp_Pnt(0, 20, 0), gp_Dir(0, -1, 0)), 0.0, 20.0, 1e-7);
  const BRepGraph_EdgeId aE4 =
    anEdgeOps.Add(aV4, aV5, new Geom_Line(gp_Pnt(0, 0, 30), gp_Dir(1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE5 =
    anEdgeOps.Add(aV5, aV6, new Geom_Line(gp_Pnt(10, 0, 30), gp_Dir(0, 1, 0)), 0.0, 20.0, 1e-7);
  const BRepGraph_EdgeId aE6 =
    anEdgeOps.Add(aV6, aV7, new Geom_Line(gp_Pnt(10, 20, 30), gp_Dir(-1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE7 =
    anEdgeOps.Add(aV7, aV4, new Geom_Line(gp_Pnt(0, 20, 30), gp_Dir(0, -1, 0)), 0.0, 20.0, 1e-7);
  const BRepGraph_EdgeId aE8 =
    anEdgeOps.Add(aV0, aV4, new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.0, 30.0, 1e-7);
  const BRepGraph_EdgeId aE9 =
    anEdgeOps.Add(aV1, aV5, new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 0.0, 30.0, 1e-7);
  const BRepGraph_EdgeId aE10 =
    anEdgeOps.Add(aV2, aV6, new Geom_Line(gp_Pnt(10, 20, 0), gp_Dir(0, 0, 1)), 0.0, 30.0, 1e-7);
  const BRepGraph_EdgeId aE11 =
    anEdgeOps.Add(aV3, aV7, new Geom_Line(gp_Pnt(0, 20, 0), gp_Dir(0, 0, 1)), 0.0, 30.0, 1e-7);

  auto& aCoEdgeOps = aGraph.Editor().CoEdges();
  auto  makeWire   = [&](std::initializer_list<BRepGraph_EdgeId>   theEdges,
                      std::initializer_list<TopAbs_Orientation> theOrients) -> BRepGraph_WireId {
    NCollection_LinearVector<BRepGraph_CoEdgeId> aCEs;
    auto                                         anOrientIt = theOrients.begin();
    for (const BRepGraph_EdgeId& anEdge : theEdges)
    {
      aCEs.Append(aCoEdgeOps.Add(anEdge, *anOrientIt));
      ++anOrientIt;
    }
    return aGraph.Editor().Wires().Add(aCEs.ToArray1());
  };

  const BRepGraph_WireId aW0 =
    makeWire({aE0, aE1, aE2, aE3},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_FORWARD});
  const BRepGraph_WireId aW1 =
    makeWire({aE4, aE5, aE6, aE7},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_FORWARD});
  const BRepGraph_WireId aW2 =
    makeWire({aE0, aE9, aE4, aE8},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_REVERSED});
  const BRepGraph_WireId aW3 =
    makeWire({aE1, aE10, aE5, aE9},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_REVERSED});
  const BRepGraph_WireId aW4 =
    makeWire({aE2, aE11, aE6, aE10},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_REVERSED});
  const BRepGraph_WireId aW5 =
    makeWire({aE3, aE8, aE7, aE11},
             {TopAbs_FORWARD, TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_REVERSED});

  auto&                                      aFaceOps = aGraph.Editor().Faces();
  NCollection_LinearVector<BRepGraph_WireId> aNoInner;
  const BRepGraph_FaceId                     aF0 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, -1)))),
                 aW0,
                 aNoInner.ToArray1(),
                 1e-7);
  const BRepGraph_FaceId aF1 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 30), gp_Dir(0, 0, 1)))),
                 aW1,
                 aNoInner.ToArray1(),
                 1e-7);
  const BRepGraph_FaceId aF2 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, -1, 0)))),
                 aW2,
                 aNoInner.ToArray1(),
                 1e-7);
  const BRepGraph_FaceId aF3 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(1, 0, 0)))),
                 aW3,
                 aNoInner.ToArray1(),
                 1e-7);
  const BRepGraph_FaceId aF4 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(10, 20, 0), gp_Dir(0, 1, 0)))),
                 aW4,
                 aNoInner.ToArray1(),
                 1e-7);
  const BRepGraph_FaceId aF5 =
    aFaceOps.Add(new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 20, 0), gp_Dir(-1, 0, 0)))),
                 aW5,
                 aNoInner.ToArray1(),
                 1e-7);

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  for (const BRepGraph_FaceId& aF : {aF0, aF1, aF2, aF3, aF4, aF5})
  {
    aGraph.Editor().Shells().Append(aShell, aF);
  }
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Synthetic box must pass Audit";
  EXPECT_EQ(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);
}

TEST(BRepGraph_ValidateTest, Synthetic_Cylinder_AuditClean)
{
  BRepGraph aGraph;
  aGraph.Clear();

  auto&                    aVtxOps = aGraph.Editor().Vertices();
  const BRepGraph_VertexId aVBot   = aVtxOps.Add(gp_Pnt(5, 0, 0), 1e-7);
  const BRepGraph_VertexId aVTop   = aVtxOps.Add(gp_Pnt(5, 0, 15), 1e-7);

  auto&                    anEdgeOps = aGraph.Editor().Edges();
  occ::handle<Geom_Circle> aBotCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const BRepGraph_EdgeId   aEBot = anEdgeOps.Add(aVBot, aVBot, aBotCircle, 0.0, 2 * M_PI, 1e-7);
  occ::handle<Geom_Circle> aTopCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 15), gp_Dir(0, 0, 1)), 5.0);
  const BRepGraph_EdgeId aETop     = anEdgeOps.Add(aVTop, aVTop, aTopCircle, 0.0, 2 * M_PI, 1e-7);
  occ::handle<Geom_Line> aSeamLine = new Geom_Line(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
  const BRepGraph_EdgeId aESeam    = anEdgeOps.Add(aVBot, aVTop, aSeamLine, 0.0, 15.0, 1e-7);

  auto& aCoEdgeOps = aGraph.Editor().CoEdges();

  NCollection_LinearVector<BRepGraph_CoEdgeId> aBotWireCEs;
  aBotWireCEs.Append(aCoEdgeOps.Add(aEBot, TopAbs_FORWARD));
  aBotWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_REVERSED));
  const BRepGraph_WireId aWBot = aGraph.Editor().Wires().Add(aBotWireCEs.ToArray1());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aTopWireCEs;
  aTopWireCEs.Append(aCoEdgeOps.Add(aETop, TopAbs_FORWARD));
  aTopWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_FORWARD));
  const BRepGraph_WireId aWTop = aGraph.Editor().Wires().Add(aTopWireCEs.ToArray1());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aLatWireCEs;
  aLatWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_FORWARD));
  aLatWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_REVERSED));
  const BRepGraph_WireId aWLat = aGraph.Editor().Wires().Add(aLatWireCEs.ToArray1());

  auto&                                      aFaceOps = aGraph.Editor().Faces();
  NCollection_LinearVector<BRepGraph_WireId> aNoInner;
  occ::handle<Geom_Plane>                    aBotPlane =
    new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, -1))));
  occ::handle<Geom_Plane> aTopPlane =
    new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 15), gp_Dir(0, 0, 1))));
  occ::handle<Geom_CylindricalSurface> aLatSurf =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  const BRepGraph_FaceId aFBot = aFaceOps.Add(aBotPlane, aWBot, aNoInner.ToArray1(), 1e-7);
  const BRepGraph_FaceId aFTop = aFaceOps.Add(aTopPlane, aWTop, aNoInner.ToArray1(), 1e-7);
  const BRepGraph_FaceId aFLat = aFaceOps.Add(aLatSurf, aWLat, aNoInner.ToArray1(), 1e-7);

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFBot);
  aGraph.Editor().Shells().Append(aShell, aFTop);
  aGraph.Editor().Shells().Append(aShell, aFLat);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Synthetic cylinder must pass Audit";
}

TEST(BRepGraph_ValidateTest, Synthetic_Sphere_AuditClean)
{
  BRepGraph aGraph;
  aGraph.Clear();

  auto&                    aVtxOps = aGraph.Editor().Vertices();
  const BRepGraph_VertexId aVSeam  = aVtxOps.Add(gp_Pnt(8, 0, 0), 1e-7);

  auto&                    anEdgeOps = aGraph.Editor().Edges();
  occ::handle<Geom_Circle> aSeamCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 8.0);
  const BRepGraph_EdgeId aESeam = anEdgeOps.Add(aVSeam, aVSeam, aSeamCircle, 0.0, 2 * M_PI, 1e-7);

  auto&                                        aCoEdgeOps = aGraph.Editor().CoEdges();
  NCollection_LinearVector<BRepGraph_CoEdgeId> aWireCEs;
  aWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aESeam, TopAbs_REVERSED));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aWireCEs.ToArray1());

  auto&                                      aFaceOps = aGraph.Editor().Faces();
  NCollection_LinearVector<BRepGraph_WireId> aNoInner;
  occ::handle<Geom_SphericalSurface>         aSphere = new Geom_SphericalSurface(gp_Ax3(), 8.0);
  const BRepGraph_FaceId aFace = aFaceOps.Add(aSphere, aWire, aNoInner.ToArray1(), 1e-7);

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFace);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Synthetic sphere must pass Audit";
}

TEST(BRepGraph_ValidateTest, Synthetic_Compound_AuditClean)
{
  BRepGraph aGraph;
  aGraph.Clear();

  auto&                    aVtxOps = aGraph.Editor().Vertices();
  const BRepGraph_VertexId aV0     = aVtxOps.Add(gp_Pnt(0, 0, 0), 1e-7);
  const BRepGraph_VertexId aV1     = aVtxOps.Add(gp_Pnt(10, 0, 0), 1e-7);
  const BRepGraph_VertexId aV2     = aVtxOps.Add(gp_Pnt(5, 10, 0), 1e-7);

  auto&                  anEdgeOps = aGraph.Editor().Edges();
  const BRepGraph_EdgeId aE0 =
    anEdgeOps.Add(aV0, aV1, new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE1 =
    anEdgeOps.Add(aV1,
                  aV2,
                  new Geom_Line(gp_Pnt(10, 0, 0), gp_Vec(-5.0, 10.0, 0.0).Normalized()),
                  0.0,
                  std::sqrt(125.0),
                  1e-7);
  const BRepGraph_EdgeId aE2 =
    anEdgeOps.Add(aV2,
                  aV0,
                  new Geom_Line(gp_Pnt(5, 10, 0), gp_Vec(-5.0, -10.0, 0.0).Normalized()),
                  0.0,
                  std::sqrt(125.0),
                  1e-7);

  auto&                                        aCoEdgeOps = aGraph.Editor().CoEdges();
  NCollection_LinearVector<BRepGraph_CoEdgeId> aWireCEs;
  aWireCEs.Append(aCoEdgeOps.Add(aE0, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aE1, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aE2, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aWireCEs.ToArray1());

  auto&                                      aFaceOps = aGraph.Editor().Faces();
  NCollection_LinearVector<BRepGraph_WireId> aNoInner;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1))));
  const BRepGraph_FaceId  aFace  = aFaceOps.Add(aPlane, aWire, aNoInner.ToArray1(), 1e-7);

  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell1, aFace);
  const BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid1, aShell1);

  const BRepGraph_VertexId aV3 = aVtxOps.Add(gp_Pnt(20, 0, 0), 1e-7);
  const BRepGraph_VertexId aV4 = aVtxOps.Add(gp_Pnt(30, 0, 0), 1e-7);
  const BRepGraph_VertexId aV5 = aVtxOps.Add(gp_Pnt(25, 10, 0), 1e-7);

  const BRepGraph_EdgeId aE3 =
    anEdgeOps.Add(aV3, aV4, new Geom_Line(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE4 =
    anEdgeOps.Add(aV4,
                  aV5,
                  new Geom_Line(gp_Pnt(30, 0, 0), gp_Vec(-5.0, 10.0, 0.0).Normalized()),
                  0.0,
                  std::sqrt(125.0),
                  1e-7);
  const BRepGraph_EdgeId aE5 =
    anEdgeOps.Add(aV5,
                  aV3,
                  new Geom_Line(gp_Pnt(25, 10, 0), gp_Vec(-5.0, -10.0, 0.0).Normalized()),
                  0.0,
                  std::sqrt(125.0),
                  1e-7);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aWire2CEs;
  aWire2CEs.Append(aCoEdgeOps.Add(aE3, TopAbs_FORWARD));
  aWire2CEs.Append(aCoEdgeOps.Add(aE4, TopAbs_FORWARD));
  aWire2CEs.Append(aCoEdgeOps.Add(aE5, TopAbs_FORWARD));
  const BRepGraph_WireId aWire2 = aGraph.Editor().Wires().Add(aWire2CEs.ToArray1());

  const BRepGraph_FaceId aFace2 = aFaceOps.Add(aPlane, aWire2, aNoInner.ToArray1(), 1e-7);

  const BRepGraph_ShellId aShell2 = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell2, aFace2);
  const BRepGraph_SolidId aSolid2 = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid2, aShell2);

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid1);
  aChildren.Append(aSolid2);
  [[maybe_unused]] const BRepGraph_CompoundId aCompId =
    aGraph.Editor().Compounds().Add(aChildren.ToArray1());

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Synthetic compound must pass Audit";
}

TEST(BRepGraph_ValidateTest, Synthetic_Assembly_AuditClean)
{
  BRepGraph aGraph;
  aGraph.Clear();

  auto&                    aVtxOps = aGraph.Editor().Vertices();
  const BRepGraph_VertexId aV0     = aVtxOps.Add(gp_Pnt(0, 0, 0), 1e-7);
  const BRepGraph_VertexId aV1     = aVtxOps.Add(gp_Pnt(10, 0, 0), 1e-7);
  const BRepGraph_VertexId aV2     = aVtxOps.Add(gp_Pnt(10, 10, 0), 1e-7);
  const BRepGraph_VertexId aV3     = aVtxOps.Add(gp_Pnt(0, 10, 0), 1e-7);

  auto&                  anEdgeOps = aGraph.Editor().Edges();
  const BRepGraph_EdgeId aE0 =
    anEdgeOps.Add(aV0, aV1, new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE1 =
    anEdgeOps.Add(aV1, aV2, new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE2 =
    anEdgeOps.Add(aV2, aV3, new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0)), 0.0, 10.0, 1e-7);
  const BRepGraph_EdgeId aE3 =
    anEdgeOps.Add(aV3, aV0, new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0)), 0.0, 10.0, 1e-7);

  auto&                                        aCoEdgeOps = aGraph.Editor().CoEdges();
  NCollection_LinearVector<BRepGraph_CoEdgeId> aWireCEs;
  aWireCEs.Append(aCoEdgeOps.Add(aE0, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aE1, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aE2, TopAbs_FORWARD));
  aWireCEs.Append(aCoEdgeOps.Add(aE3, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aWireCEs.ToArray1());

  auto&                                      aFaceOps = aGraph.Editor().Faces();
  NCollection_LinearVector<BRepGraph_WireId> aNoInner;
  occ::handle<Geom_Plane>                    aPlane = new Geom_Plane(gp_Pln());
  const BRepGraph_FaceId aFace = aFaceOps.Add(aPlane, aWire, aNoInner.ToArray1(), 1e-7);

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFace);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();

  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(0.0, 0.0, 0.0));
  [[maybe_unused]] const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  [[maybe_unused]] const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  aGraph.Editor().CommitMutation();

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Synthetic assembly must pass Audit";
}
