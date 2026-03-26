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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class BRepGraph_DeferredInvalidationTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_EdgeMutation_SetsIsModified)
{
  EXPECT_FALSE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);

  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  // In deferred mode, the entity's IsModified flag is set.
  EXPECT_TRUE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);
  myGraph.EndDeferredInvalidation();
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_PropagatesUpOnFlush)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  // During deferred mode: edge is modified, but parent wire/face are NOT yet.
  const NCollection_Vector<BRepGraph_WireId>& aWires =
    myGraph.Topo().WiresOfEdge(BRepGraph_EdgeId(0));
  ASSERT_GT(aWires.Length(), 0);
  EXPECT_FALSE(myGraph.Topo().Wire(aWires.Value(0)).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: wire and face should be propagated.
  EXPECT_TRUE(myGraph.Topo().Wire(aWires.Value(0)).IsModified);

  // Check propagation to face.
  for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
  {
    if (myGraph.Topo().Face(BRepGraph_FaceId(aFI)).OuterWireDefId().Index == aWires.Value(0).Index)
    {
      EXPECT_TRUE(myGraph.Topo().Face(BRepGraph_FaceId(aFI)).IsModified);
      break;
    }
  }

  // Check propagation to shell and solid.
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectFaceMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutFace(BRepGraph_FaceId(0));

  EXPECT_TRUE(myGraph.Topo().Face(BRepGraph_FaceId(0)).IsModified);
  // Shell not yet propagated during deferred mode.
  EXPECT_FALSE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: shell and solid should be propagated.
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectShellMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutShell(BRepGraph_ShellId(0));

  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);

  myGraph.EndDeferredInvalidation();

  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_MultipleEdges_BatchPropagation)
{
  myGraph.BeginDeferredInvalidation();

  const int aNbEdges = myGraph.Topo().NbEdges();
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    myGraph.MutEdge(BRepGraph_EdgeId(anEdgeIdx))->Tolerance = 0.1;
  }

  // During deferred mode: all edges modified, but no parent propagation yet.
  for (int aWireIdx = 0; aWireIdx < myGraph.Topo().NbWires(); ++aWireIdx)
  {
    EXPECT_FALSE(myGraph.Topo().Wire(BRepGraph_WireId(aWireIdx)).IsModified);
  }

  myGraph.EndDeferredInvalidation();

  // After flush: all wires, faces, shells, solids should be modified.
  for (int aWireIdx = 0; aWireIdx < myGraph.Topo().NbWires(); ++aWireIdx)
  {
    EXPECT_TRUE(myGraph.Topo().Wire(BRepGraph_WireId(aWireIdx)).IsModified);
  }
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx)).IsModified);
  }
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ReconstructAfterFlush_Succeeds)
{
  // Modify an edge in deferred mode and verify reconstruction still works.
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Reconstruction should succeed (shape cache was cleared on flush).
  const BRepGraph_NodeId aSolidId = BRepGraph_NodeId::Solid(0);
  TopoDS_Shape           aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aSolidId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ParallelMutation_NoDataRace)
{
  const int aNbEdges = myGraph.Topo().NbEdges();
  ASSERT_GT(aNbEdges, 1);

  myGraph.BeginDeferredInvalidation();
  OSD_Parallel::For(
    0,
    aNbEdges,
    [&](int theIdx) { myGraph.MutEdge(BRepGraph_EdgeId(theIdx))->Tolerance = 0.1 + theIdx * 0.01; },
    false);
  myGraph.EndDeferredInvalidation();

  // All edges should be modified.
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    EXPECT_TRUE(myGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx)).IsModified);
    EXPECT_NEAR(myGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx)).Tolerance,
                0.1 + anEdgeIdx * 0.01,
                Precision::Confusion());
  }

  // Propagation should have happened on flush.
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_NoMutations_FlushIsSafe)
{
  // Begin/End with no mutations in between should be a no-op.
  myGraph.BeginDeferredInvalidation();
  myGraph.EndDeferredInvalidation();

  // Nothing should be modified.
  EXPECT_FALSE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Wire(BRepGraph_WireId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Face(BRepGraph_FaceId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, EndWithoutBegin_IsIdempotent)
{
  // Calling End without Begin should be a safe no-op.
  EXPECT_NO_THROW(myGraph.EndDeferredInvalidation());

  // Nothing should be modified or cleared.
  EXPECT_FALSE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DoubleEnd_IsIdempotent)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Second End should be a safe no-op.
  EXPECT_NO_THROW(myGraph.EndDeferredInvalidation());
  EXPECT_TRUE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_SameEdgeMutatedTwice)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.1;
  myGraph.MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Last write wins.
  EXPECT_NEAR(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).Tolerance, 0.5, Precision::Confusion());
  EXPECT_TRUE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectWireMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutWire(BRepGraph_WireId(0));

  EXPECT_TRUE(myGraph.Topo().Wire(BRepGraph_WireId(0)).IsModified);
  EXPECT_FALSE(myGraph.Topo().Face(BRepGraph_FaceId(0)).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: face, shell, solid should be propagated.
  bool aFacePropagated = false;
  for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
  {
    if (myGraph.Topo().Face(BRepGraph_FaceId(aFI)).OuterWireDefId().Index == 0
        && myGraph.Topo().Face(BRepGraph_FaceId(aFI)).IsModified)
    {
      aFacePropagated = true;
      break;
    }
  }
  EXPECT_TRUE(aFacePropagated);
  EXPECT_TRUE(myGraph.Topo().Shell(BRepGraph_ShellId(0)).IsModified);
  EXPECT_TRUE(myGraph.Topo().Solid(BRepGraph_SolidId(0)).IsModified);
}
