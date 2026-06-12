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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraphInc_ParityOrientation.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

// =======================================================================
// Helper: create a simple face
// =======================================================================

static BRepGraph_FaceId createSimpleFace(BRepGraph& theGraph)
{
  const BRepGraph_VertexId aV0 = theGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = theGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    theGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 10.0, 1.e-7);
  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(theGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD));
  const BRepGraph_WireId          aWire  = theGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  NCollection_LinearVector<BRepGraph_WireId> anEmpty;
  return theGraph.Editor().Faces().Add(aPlane, aWire, anEmpty.ToArray1(), 1.e-7);
}

// =======================================================================
// ShellOps::Append (batch)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, ShellOps_AppendBatch_AllForward)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace2 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());
  ASSERT_TRUE(aFace2.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  NCollection_Array1<BRepGraph_FaceId> aFaces(3);
  aFaces.ChangeAt(0) = aFace0;
  aFaces.ChangeAt(1) = aFace1;
  aFaces.ChangeAt(2) = aFace2;

  const NCollection_Array1<BRepGraph_FaceRefId> aRefs =
    aGraph.Editor().Shells().Append(aShell, aFaces);
  EXPECT_EQ(aRefs.Size(), 3);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
  EXPECT_TRUE(aRefs.At(2).IsValid());
}

TEST(BRepGraph_BatchOpsTest, ShellOps_AppendBatch_WithOrientations)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  NCollection_Array1<BRepGraph_FaceId> aFaces(2);
  aFaces.ChangeAt(0) = aFace0;
  aFaces.ChangeAt(1) = aFace1;

  NCollection_Array1<BRepGraphInc::ParityOrientation> aOrientations(2);
  aOrientations.ChangeAt(0) = TopAbs_FORWARD;
  aOrientations.ChangeAt(1) = TopAbs_REVERSED;

  const NCollection_Array1<BRepGraph_FaceRefId> aRefs =
    aGraph.Editor().Shells().Append(aShell, aFaces, aOrientations);
  EXPECT_EQ(aRefs.Size(), 2);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
}

TEST(BRepGraph_BatchOpsTest, ShellOps_AppendBatch_InvalidFace_ReturnsEmpty)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  NCollection_Array1<BRepGraph_FaceId> aFaces(2);
  aFaces.ChangeAt(0) = aFace0;
  aFaces.ChangeAt(1) = BRepGraph_FaceId(999); // invalid

  const NCollection_Array1<BRepGraph_FaceRefId> aRefs =
    aGraph.Editor().Shells().Append(aShell, aFaces);
  EXPECT_EQ(aRefs.Size(), 0);
}

TEST(BRepGraph_BatchOpsTest, ShellOps_AppendBatch_InvalidShell_ReturnsEmpty)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  NCollection_Array1<BRepGraph_FaceId> aFaces(1);
  aFaces.ChangeAt(0) = aFace0;

  const NCollection_Array1<BRepGraph_FaceRefId> aRefs =
    aGraph.Editor().Shells().Append(BRepGraph_ShellId(999), aFaces);
  EXPECT_EQ(aRefs.Size(), 0);
}

// =======================================================================
// ShellOps::RemoveFaces (batch, all-or-nothing)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, ShellOps_RemoveFaces_AllRemoved)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  const BRepGraph_FaceRefId aRef0 = aGraph.Editor().Shells().Append(aShell, aFace0);
  const BRepGraph_FaceRefId aRef1 = aGraph.Editor().Shells().Append(aShell, aFace1);
  ASSERT_TRUE(aRef0.IsValid());
  ASSERT_TRUE(aRef1.IsValid());

  NCollection_Array1<BRepGraph_FaceRefId> aRefs(2);
  aRefs.ChangeAt(0) = aRef0;
  aRefs.ChangeAt(1) = aRef1;

  EXPECT_TRUE(aGraph.Editor().Shells().RemoveFaces(aShell, aRefs));
}

TEST(BRepGraph_BatchOpsTest, ShellOps_RemoveFaces_InvalidRef_ReturnsFalse)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  const BRepGraph_FaceRefId aRef0 = aGraph.Editor().Shells().Append(aShell, aFace0);
  ASSERT_TRUE(aRef0.IsValid());

  NCollection_Array1<BRepGraph_FaceRefId> aRefs(2);
  aRefs.ChangeAt(0) = aRef0;
  aRefs.ChangeAt(1) = BRepGraph_FaceRefId(999); // invalid

  EXPECT_FALSE(aGraph.Editor().Shells().RemoveFaces(aShell, aRefs));
}

// =======================================================================
// SolidOps::Append (batch)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, SolidOps_AppendBatch_AllForward)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  ASSERT_TRUE(aShell1.IsValid());

  aGraph.Editor().Shells().Append(aShell0, aFace0);
  aGraph.Editor().Shells().Append(aShell1, aFace1);

  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid.IsValid());

  NCollection_Array1<BRepGraph_ShellId> aShells(2);
  aShells.ChangeAt(0) = aShell0;
  aShells.ChangeAt(1) = aShell1;

  const NCollection_Array1<BRepGraph_ShellRefId> aRefs =
    aGraph.Editor().Solids().Append(aSolid, aShells);
  EXPECT_EQ(aRefs.Size(), 2);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
}

// =======================================================================
// SolidOps::RemoveShells (batch, all-or-nothing)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, SolidOps_RemoveShells_AllRemoved)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  aGraph.Editor().Shells().Append(aShell0, aFace0);

  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid.IsValid());

  const BRepGraph_ShellRefId aRef0 = aGraph.Editor().Solids().Append(aSolid, aShell0);
  ASSERT_TRUE(aRef0.IsValid());

  NCollection_Array1<BRepGraph_ShellRefId> aRefs(1);
  aRefs.ChangeAt(0) = aRef0;

  EXPECT_TRUE(aGraph.Editor().Solids().RemoveShells(aSolid, aRefs));
}

// =======================================================================
// CompoundOps::Append (batch)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompoundOps_AppendBatch_AllForward)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId                 aCompound =
    aGraph.Editor().Compounds().Add(anEmptyChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  NCollection_Array1<BRepGraph_NodeId> aChildren(2);
  aChildren.ChangeAt(0) = BRepGraph_NodeId(aFace0);
  aChildren.ChangeAt(1) = BRepGraph_NodeId(aFace1);

  const NCollection_Array1<BRepGraph_ChildRefId> aRefs =
    aGraph.Editor().Compounds().Append(aCompound, aChildren);
  EXPECT_EQ(aRefs.Size(), 2);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
}

// =======================================================================
// CompoundOps::RemoveChildren (batch, all-or-nothing)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompoundOps_RemoveChildren_AllRemoved)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId                 aCompound =
    aGraph.Editor().Compounds().Add(anEmptyChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  const BRepGraph_ChildRefId aRef0 =
    aGraph.Editor().Compounds().Append(aCompound, BRepGraph_NodeId(aFace0));
  ASSERT_TRUE(aRef0.IsValid());

  NCollection_Array1<BRepGraph_ChildRefId> aRefs(1);
  aRefs.ChangeAt(0) = aRef0;

  EXPECT_TRUE(aGraph.Editor().Compounds().RemoveChildren(aCompound, aRefs));
}

// =======================================================================
// CompoundOps::ReplaceChild
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompoundOps_ReplaceChild)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId                 aCompound =
    aGraph.Editor().Compounds().Add(anEmptyChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  const BRepGraph_ChildRefId aRef0 =
    aGraph.Editor().Compounds().Append(aCompound, BRepGraph_NodeId(aFace0));
  ASSERT_TRUE(aRef0.IsValid());

  aGraph.Editor().Compounds().ReplaceChild(aRef0, BRepGraph_NodeId(aFace1));
}

// =======================================================================
// CompSolidOps::Append (batch)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompSolidOps_AppendBatch_AllForward)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  ASSERT_TRUE(aShell1.IsValid());

  aGraph.Editor().Shells().Append(aShell0, aFace0);
  aGraph.Editor().Shells().Append(aShell1, aFace1);

  const BRepGraph_SolidId aSolid0 = aGraph.Editor().Solids().Add();
  const BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid0.IsValid());
  ASSERT_TRUE(aSolid1.IsValid());

  aGraph.Editor().Solids().Append(aSolid0, aShell0);
  aGraph.Editor().Solids().Append(aSolid1, aShell1);

  NCollection_LinearVector<BRepGraph_SolidId> anEmptySolids;
  const BRepGraph_CompSolidId                 aCompSolid =
    aGraph.Editor().CompSolids().Add(anEmptySolids.ToArray1());
  ASSERT_TRUE(aCompSolid.IsValid());

  NCollection_Array1<BRepGraph_SolidId> aSolids(2);
  aSolids.ChangeAt(0) = aSolid0;
  aSolids.ChangeAt(1) = aSolid1;

  const NCollection_Array1<BRepGraph_SolidRefId> aRefs =
    aGraph.Editor().CompSolids().Append(aCompSolid, aSolids);
  EXPECT_EQ(aRefs.Size(), 2);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
}

// =======================================================================
// CompSolidOps::RemoveSolids (batch, all-or-nothing)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompSolidOps_RemoveSolids_AllRemoved)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  aGraph.Editor().Shells().Append(aShell0, aFace0);

  const BRepGraph_SolidId aSolid0 = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid0.IsValid());
  aGraph.Editor().Solids().Append(aSolid0, aShell0);

  NCollection_LinearVector<BRepGraph_SolidId> anEmptySolids;
  const BRepGraph_CompSolidId                 aCompSolid =
    aGraph.Editor().CompSolids().Add(anEmptySolids.ToArray1());
  ASSERT_TRUE(aCompSolid.IsValid());

  const BRepGraph_SolidRefId aRef0 = aGraph.Editor().CompSolids().Append(aCompSolid, aSolid0);
  ASSERT_TRUE(aRef0.IsValid());

  NCollection_Array1<BRepGraph_SolidRefId> aRefs(1);
  aRefs.ChangeAt(0) = aRef0;

  EXPECT_TRUE(aGraph.Editor().CompSolids().RemoveSolids(aCompSolid, aRefs));
}

// =======================================================================
// CompSolidOps::ReplaceSolid
// =======================================================================

TEST(BRepGraph_BatchOpsTest, CompSolidOps_ReplaceSolid)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  ASSERT_TRUE(aShell1.IsValid());

  aGraph.Editor().Shells().Append(aShell0, aFace0);
  aGraph.Editor().Shells().Append(aShell1, aFace1);

  const BRepGraph_SolidId aSolid0 = aGraph.Editor().Solids().Add();
  const BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid0.IsValid());
  ASSERT_TRUE(aSolid1.IsValid());

  aGraph.Editor().Solids().Append(aSolid0, aShell0);
  aGraph.Editor().Solids().Append(aSolid1, aShell1);

  NCollection_LinearVector<BRepGraph_SolidId> anEmptySolids;
  const BRepGraph_CompSolidId                 aCompSolid =
    aGraph.Editor().CompSolids().Add(anEmptySolids.ToArray1());
  ASSERT_TRUE(aCompSolid.IsValid());

  const BRepGraph_SolidRefId aRef0 = aGraph.Editor().CompSolids().Append(aCompSolid, aSolid0);
  ASSERT_TRUE(aRef0.IsValid());

  aGraph.Editor().CompSolids().ReplaceSolid(aRef0, aSolid1);
}

// =======================================================================
// ProductOps::Append (batch)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, ProductOps_AppendBatch)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  const BRepGraph_ShellId aShell1 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  ASSERT_TRUE(aShell1.IsValid());

  aGraph.Editor().Shells().Append(aShell0, aFace0);
  aGraph.Editor().Shells().Append(aShell1, aFace1);

  const BRepGraph_SolidId aSolid0 = aGraph.Editor().Solids().Add();
  const BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid0.IsValid());
  ASSERT_TRUE(aSolid1.IsValid());

  aGraph.Editor().Solids().Append(aSolid0, aShell0);
  aGraph.Editor().Solids().Append(aSolid1, aShell1);

  const BRepGraph_ProductId aChildProduct0 =
    aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid0));
  const BRepGraph_ProductId aChildProduct1 =
    aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid1));
  ASSERT_TRUE(aChildProduct0.IsValid());
  ASSERT_TRUE(aChildProduct1.IsValid());

  const BRepGraph_ProductId aParentProduct = aGraph.Editor().Products().Add();
  ASSERT_TRUE(aParentProduct.IsValid());

  NCollection_Array1<BRepGraph_ProductId> aChildren(2);
  aChildren.ChangeAt(0) = aChildProduct0;
  aChildren.ChangeAt(1) = aChildProduct1;

  NCollection_Array1<TopLoc_Location> aPlacements(2);
  aPlacements.ChangeAt(0) = TopLoc_Location();
  aPlacements.ChangeAt(1) = TopLoc_Location();

  const NCollection_Array1<BRepGraph_OccurrenceRefId> aRefs =
    aGraph.Editor().Products().Append(aParentProduct, aChildren, aPlacements);
  EXPECT_EQ(aRefs.Size(), 2);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
}

// =======================================================================
// ProductOps::RemoveOccurrences (batch, all-or-nothing)
// =======================================================================

TEST(BRepGraph_BatchOpsTest, ProductOps_RemoveOccurrences_AllRemoved)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());

  const BRepGraph_ShellId aShell0 = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell0.IsValid());
  aGraph.Editor().Shells().Append(aShell0, aFace0);

  const BRepGraph_SolidId aSolid0 = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid0.IsValid());
  aGraph.Editor().Solids().Append(aSolid0, aShell0);

  const BRepGraph_ProductId aChildProduct =
    aGraph.Editor().Products().Add(BRepGraph_NodeId(aSolid0));
  ASSERT_TRUE(aChildProduct.IsValid());

  const BRepGraph_ProductId aParentProduct = aGraph.Editor().Products().Add();
  ASSERT_TRUE(aParentProduct.IsValid());

  BRepGraph_OccurrenceRefId               anOutRefId;
  [[maybe_unused]] BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aParentProduct,
                                      aChildProduct,
                                      TopLoc_Location(),
                                      BRepGraph_OccurrenceId(),
                                      &anOutRefId);
  ASSERT_TRUE(anOccId.IsValid());
  ASSERT_TRUE(anOutRefId.IsValid());

  NCollection_Array1<BRepGraph_OccurrenceRefId> aRefs(1);
  aRefs.ChangeAt(0) = anOutRefId;

  EXPECT_TRUE(aGraph.Editor().Products().RemoveOccurrences(aParentProduct, aRefs));
}

// =======================================================================
// Validation after batch operations
// =======================================================================

TEST(BRepGraph_BatchOpsTest, Validation_AfterBatchOperations)
{
  BRepGraph aGraph;

  const BRepGraph_FaceId aFace0 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace1 = createSimpleFace(aGraph);
  const BRepGraph_FaceId aFace2 = createSimpleFace(aGraph);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());
  ASSERT_TRUE(aFace2.IsValid());

  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  // Batch append
  NCollection_Array1<BRepGraph_FaceId> aFaces(3);
  aFaces.ChangeAt(0) = aFace0;
  aFaces.ChangeAt(1) = aFace1;
  aFaces.ChangeAt(2) = aFace2;

  const NCollection_Array1<BRepGraph_FaceRefId> aRefs =
    aGraph.Editor().Shells().Append(aShell, aFaces);
  EXPECT_EQ(aRefs.Size(), 3);
  EXPECT_TRUE(aRefs.At(0).IsValid());
  EXPECT_TRUE(aRefs.At(1).IsValid());
  EXPECT_TRUE(aRefs.At(2).IsValid());

  // Batch remove
  NCollection_Array1<BRepGraph_FaceRefId> aRefsToRemove(2);
  aRefsToRemove.ChangeAt(0) = aRefs.At(0);
  aRefsToRemove.ChangeAt(1) = aRefs.At(1);

  EXPECT_TRUE(aGraph.Editor().Shells().RemoveFaces(aShell, aRefsToRemove));
}
