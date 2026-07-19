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
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Validate.hxx>

#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <TopAbs_Orientation.hxx>

#include <tuple>

#include <gtest/gtest.h>

// =======================================================================
// Group 1 - Compact on sparse models (no removed nodes -> no-op or valid pass)
// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_BareVertices_NoOp)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7).IsValid());

  const uint32_t aVtxBefore = aGraph.Topo().Vertices().NbActive();
  ASSERT_EQ(aVtxBefore, 3);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 3);
  const BRepGraph_Validate::Result aValidateResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aValidateResult.IsValid())
    << (aValidateResult.Issues.IsEmpty() ? ""
                                         : aValidateResult.Issues.First().Description.ToCString());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_FreeEdgeNoCurve_NoOp)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  ASSERT_TRUE(aV0.IsValid() && aV1.IsValid());
  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  ASSERT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_FreeWireSingleCoEdge_NoOp)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  ASSERT_TRUE(aV0.IsValid() && aV1.IsValid());
  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  ASSERT_TRUE(aGraph.Editor().Wires().Add(aCoEdges.ToArray1()).IsValid());

  ASSERT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Wires().NbActive(), 1);
  ASSERT_GE(aGraph.Topo().CoEdges().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Wires().NbActive(), 1);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_EmptyCompound_NoOp)
{
  BRepGraph                                  aGraph;
  NCollection_LinearVector<BRepGraph_NodeId> anEmpty;
  ASSERT_TRUE(aGraph.Editor().Compounds().Add(anEmpty.ToArray1()).IsValid());

  ASSERT_EQ(aGraph.Topo().Compounds().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Compounds().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_EmptyShell_NoOp)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Shells().Add().IsValid());

  ASSERT_EQ(aGraph.Topo().Shells().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Shells().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_EmptySolid_NoOp)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Solids().Add().IsValid());

  ASSERT_EQ(aGraph.Topo().Solids().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Solids().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_WirelessSurfacelessFace_NoOp)
{
  BRepGraph                                  aGraph;
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  ASSERT_TRUE(aGraph.Editor()
                .Faces()
                .Add(occ::handle<Geom_Surface>(), BRepGraph_WireId(), anEmpty.ToArray1(), 1.e-7)
                .IsValid());

  ASSERT_EQ(aGraph.Topo().Faces().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================
// Group 2 - Compact after RemoveNode on sparse models
// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_RemoveVertexFromFreeEdge_RemapsEdgeRefs)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  ASSERT_TRUE(aV0.IsValid() && aV1.IsValid());
  const BRepGraph_EdgeId aRvEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  ASSERT_TRUE(aRvEdge.IsValid());

  ASSERT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  ASSERT_EQ(aGraph.Topo().Edges().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aV0));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_RemoveEdgeFromFreeWire_RetiresCoEdgeAndPrunesRef)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  ASSERT_TRUE(aGraph.Editor().Wires().Add(aCoEdges.ToArray1()).IsValid());

  const uint32_t aCoEdgesBefore = aGraph.Topo().CoEdges().NbActive();
  ASSERT_GE(aCoEdgesBefore, 1);
  ASSERT_EQ(aGraph.Topo().Wires().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Edges().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdge));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().CoEdges().NbActive(), aCoEdgesBefore - 1)
    << "Phase 4a should retire orphaned CoEdge";

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().CoEdges().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Wires().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_RemoveFace_FreeWireCoEdgesSurvive)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());

  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  const uint32_t aCoEdgesBefore = aGraph.Topo().CoEdges().NbActive();
  const uint32_t aWiresBefore   = aGraph.Topo().Wires().NbActive();
  ASSERT_GE(aCoEdgesBefore, 1);
  ASSERT_EQ(aWiresBefore, 1);
  ASSERT_EQ(aGraph.Topo().Faces().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFace));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().CoEdges().NbActive(), aCoEdgesBefore)
    << "Free-wire CoEdges must survive face removal (FaceId cleared, ChildEdgeId valid)";
  EXPECT_EQ(aGraph.Topo().Wires().NbActive(), aWiresBefore) << "Wire must survive face removal";

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().CoEdges().NbActive(), aCoEdgesBefore);
  EXPECT_EQ(aGraph.Topo().Wires().NbActive(), aWiresBefore);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_RemoveTopologyChild_OccurrenceRetired)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFace);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);

  const BRepGraph_ProductId aProduct = aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid));
  ASSERT_TRUE(aProduct.IsValid());
  aGraph.Editor().Products().AppendDocumentRoot(aProduct);

  ASSERT_EQ(aGraph.Topo().Products().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Occurrences().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Solids().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aSolid));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Solids().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Occurrences().NbActive(), 0)
    << "Phase 10 should retire occurrence whose ChildNodeId was removed";
  EXPECT_EQ(aGraph.Topo().Products().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().Solids().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Occurrences().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Products().NbActive(), 1);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Compact_MultipleRemovals_VertexEdgeFace_Validates)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aExtraVtx = aGraph.Editor().Vertices().Add(gp_Pnt(100, 0, 0), 1.e-7);
  ASSERT_TRUE(aExtraVtx.IsValid());

  const BRepGraph_VertexId aV0f = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1f = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   aFreeEdge =
    aGraph.Editor().Edges().Add(aV0f, aV1f, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 1, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 1, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());
  ASSERT_TRUE(aFreeEdge.IsValid());

  const uint32_t aVtxBefore = aGraph.Topo().Vertices().NbActive();

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aExtraVtx));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFreeEdge));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFace));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), aVtxBefore - 1);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), 0);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
}

// =======================================================================
// Group 3 - Deduplicate on sparse models
// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_BareVertices_NoRewrite)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7).IsValid());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_EQ(aRes.NbSurfaceRewrites, 0);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_TwoIdenticalVertices_MergeWhenSafe)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());

  ASSERT_EQ(aGraph.Topo().Vertices().NbActive(), 2);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe             = true;
  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  EXPECT_GE(aRes.NbMergedVertices, 1);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_TwoIdenticalVertices_NoMergeByDefault)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7).IsValid());

  ASSERT_EQ(aGraph.Topo().Vertices().NbActive(), 2);

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbMergedVertices, 0);
  EXPECT_FALSE(aRes.IsEntityMergeApplied);
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_FreeEdgesWithCurve_CanonicalizesCurves)
{
  BRepGraph                     aGraph;
  const BRepGraph_VertexId      aV0    = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId      aV1    = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_VertexId      aV0b   = aGraph.Editor().Vertices().Add(gp_Pnt(0, 1, 0), 1.e-7);
  const BRepGraph_VertexId      aV1b   = aGraph.Editor().Vertices().Add(gp_Pnt(10, 1, 0), 1.e-7);
  const occ::handle<Geom_Curve> aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  const occ::handle<Geom_Curve> aLine2 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  ASSERT_NE(aLine1.get(), aLine2.get());

  ASSERT_TRUE(aGraph.Editor().Edges().Add(aV0, aV1, aLine1, 0.0, 10.0, 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Edges().Add(aV0b, aV1b, aLine2, 0.0, 10.0, 1.e-7).IsValid());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalCurves, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_FreeWireSingleCoEdge_NoGeometryRewrite)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  ASSERT_TRUE(aGraph.Editor().Wires().Add(aCoEdges.ToArray1()).IsValid());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 1);
  EXPECT_EQ(aRes.NbCurveRewrites, 0);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Dedup_EmptyCompound_NoOp)
{
  BRepGraph                                  aGraph;
  NCollection_LinearVector<BRepGraph_NodeId> anEmpty;
  ASSERT_TRUE(aGraph.Editor().Compounds().Add(anEmpty.ToArray1()).IsValid());

  const BRepGraph_Deduplicate::Result aRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aRes.NbCanonicalSurfaces, 0);
  EXPECT_EQ(aRes.NbCanonicalCurves, 0);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph).IsValid());
}

// =======================================================================
// Group 4 - CleanupRemovedReferences edge cases
// =======================================================================

TEST(BRepGraph_SparseModelTest, Cleanup_RemoveFace_CoEdgeFaceIdCleared_CoEdgeSurvives)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFace));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  for (BRepGraph_CoEdgeIterator anIt(aGraph); anIt.More(); anIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCE = anIt.Current();
    EXPECT_FALSE(anIt.CurrentId().IsRemoved(aGraph))
      << "Free-wire CoEdge must not be retired by Phase 4a";
    EXPECT_FALSE(aCE.FaceId.IsValid())
      << "FaceId should be cleared for CoEdges whose face was removed";
    EXPECT_TRUE(aCE.ChildEdgeId.IsValid()) << "ChildEdgeId must remain valid for free-wire CoEdges";
  }
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Cleanup_RemoveEdge_FreeWireCoEdgeRetired)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  ASSERT_TRUE(aGraph.Editor().Wires().Add(aCoEdges.ToArray1()).IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdge));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().CoEdges().NbActive(), 0)
    << "All CoEdges should be removed (Phase 4a orphan retirement)";
  EXPECT_GE(aGraph.Topo().CoEdges().Nb(), 1)
    << "Total CoEdges (including removed) should still reflect the pre-cleanup count";
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Cleanup_RemoveVertex_EdgeVertexRefsCleared)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  ASSERT_TRUE(
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7).IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aV0));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Edges().NbActive(), 1);

  for (BRepGraph_EdgeIterator anIt(aGraph); anIt.More(); anIt.Next())
  {
    const BRepGraphInc::EdgeDef& anED = anIt.Current();
    EXPECT_FALSE(anED.StartVertexRefId.IsValid())
      << "StartVertexRef should be cleared for removed vertex";
    EXPECT_TRUE(anED.EndVertexRefId.IsValid()) << "EndVertexRef to surviving vertex must persist";
  }
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Cleanup_RemoveWire_FaceWireRefPruned)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aWire));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Wires().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().NbActive(), 1);
  const BRepGraphInc::FaceRelations& aFaceRelations =
    aGraph.Topo().Faces().Relations(BRepGraph_FaceId::Start());
  EXPECT_EQ(aFaceRelations.WireRefIds.Size(), 0) << "WireRef should be pruned by Phase 5";
}

// =======================================================================
// Group 5 - Product / Occurrence canonical pattern stress
// =======================================================================

TEST(BRepGraph_SparseModelTest, Product_EmptyProduct_CompactAndAudit)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Products().Add().IsValid());

  ASSERT_EQ(aGraph.Topo().Products().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Occurrences().NbActive(), 0);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aRes.NbNodesBefore, aRes.NbNodesAfter);
  EXPECT_EQ(aGraph.Topo().Products().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Product_FullCanonical_RemoveSolid_WholeChainCleans)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFace);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);
  const BRepGraph_ProductId aProduct = aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid));
  ASSERT_TRUE(aProduct.IsValid());
  aGraph.Editor().Products().AppendDocumentRoot(aProduct);

  ASSERT_EQ(aGraph.Topo().Products().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Occurrences().NbActive(), 1);
  ASSERT_EQ(aGraph.Topo().Solids().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aSolid));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Solids().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Occurrences().NbActive(), 0)
    << "Phase 10 retires occurrence with removed ChildNodeId";

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().Solids().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Occurrences().NbActive(), 0);
  EXPECT_EQ(aGraph.Topo().Products().NbActive(), 1);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Product_FullCanonical_CompactAndDedup_NoRegression)
{
  BRepGraph                     aGraph;
  const BRepGraph_VertexId      aV0   = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId      aV1   = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const occ::handle<Geom_Curve> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  const BRepGraph_EdgeId anEdge = aGraph.Editor().Edges().Add(aV0, aV1, aLine, 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShell, aFace);
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolid, aShell);
  const BRepGraph_ProductId aProduct = aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid));
  ASSERT_TRUE(aProduct.IsValid());
  aGraph.Editor().Products().AppendDocumentRoot(aProduct);

  std::ignore = BRepGraph_Compact::Perform(aGraph);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());

  const BRepGraph_Deduplicate::Result aDedupRes = BRepGraph_Deduplicate::Perform(aGraph);
  EXPECT_EQ(aDedupRes.NbCanonicalSurfaces, 1);
  EXPECT_EQ(aDedupRes.NbCanonicalCurves, 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

// =======================================================================
// Group 6 - Stress pipelines (multiple operations combined)
// =======================================================================

TEST(BRepGraph_SparseModelTest, Pipeline_RemoveCleanupCompact_GraphValid)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0     = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1     = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_VertexId aVExtra = aGraph.Editor().Vertices().Add(gp_Pnt(100, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFace));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aVExtra));
  aGraph.Editor().Gen().CleanupRemovedReferences();
  std::ignore = BRepGraph_Compact::Perform(aGraph);
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Pipeline_RemoveCleanupDedupCompact_GraphValid)
{
  BRepGraph                     aGraph;
  const BRepGraph_VertexId      aV0a   = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId      aV1a   = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_VertexId      aV0b   = aGraph.Editor().Vertices().Add(gp_Pnt(0, 1, 0), 1.e-7);
  const BRepGraph_VertexId      aV1b   = aGraph.Editor().Vertices().Add(gp_Pnt(10, 1, 0), 1.e-7);
  const occ::handle<Geom_Curve> aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  const occ::handle<Geom_Curve> aLine2 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  ASSERT_NE(aLine1.get(), aLine2.get());

  ASSERT_TRUE(aGraph.Editor().Edges().Add(aV0a, aV1a, aLine1, 0.0, 10.0, 1.e-7).IsValid());
  ASSERT_TRUE(aGraph.Editor().Edges().Add(aV0b, aV1b, aLine2, 0.0, 10.0, 1.e-7).IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aV0a));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  BRepGraph_Deduplicate::Options aDedupOpts;
  aDedupOpts.MergeEntitiesWhenSafe = true;
  std::ignore                      = BRepGraph_Deduplicate::Perform(aGraph, aDedupOpts);
  std::ignore                      = BRepGraph_Compact::Perform(aGraph);

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}

// =======================================================================

TEST(BRepGraph_SparseModelTest, Pipeline_CompoundWithEmptyShells_CompactAndAudit)
{
  BRepGraph               aGraph;
  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  const BRepGraph_ShellId aShell2 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell1.IsValid() && aShell2.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(aShell1));
  aChildren.Append(BRepGraph_NodeId(aShell2));
  ASSERT_TRUE(aGraph.Editor().Compounds().Add(aChildren.ToArray1()).IsValid());

  ASSERT_EQ(aGraph.Topo().Shells().NbActive(), 2);
  ASSERT_EQ(aGraph.Topo().Compounds().NbActive(), 1);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aShell1));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_EQ(aGraph.Topo().Shells().NbActive(), 1);

  [[maybe_unused]] const BRepGraph_Compact::Result aRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aGraph.Topo().Shells().NbActive(), 1);
  EXPECT_EQ(aGraph.Topo().Compounds().NbActive(), 1);
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());
}
