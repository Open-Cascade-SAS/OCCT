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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class BRepGraphDeferredInvalidationTest : public testing::Test
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

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_EdgeMutation_SetsIsModified)
{
  EXPECT_FALSE(myGraph.Defs().Edge(0).IsModified);

  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(0)->Tolerance = 0.5;
  // In deferred mode, the entity's IsModified flag is set.
  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);
  myGraph.EndDeferredInvalidation();
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_PropagatesUpOnFlush)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(0)->Tolerance = 0.5;

  // During deferred mode: edge is modified, but parent wire/face are NOT yet.
  const NCollection_Vector<int>& aWires = myGraph.Defs().WiresOfEdge(0);
  ASSERT_GT(aWires.Length(), 0);
  EXPECT_FALSE(myGraph.Defs().Wire(aWires.Value(0)).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: wire and face should be propagated.
  EXPECT_TRUE(myGraph.Defs().Wire(aWires.Value(0)).IsModified);

  // Check propagation to face.
  for (int aFI = 0; aFI < myGraph.Defs().NbFaces(); ++aFI)
  {
    if (myGraph.Defs().Face(aFI).OuterWireIdx() == aWires.Value(0))
    {
      EXPECT_TRUE(myGraph.Defs().Face(aFI).IsModified);
      break;
    }
  }

  // Check propagation to shell and solid.
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_DirectFaceMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutFace(0);

  EXPECT_TRUE(myGraph.Defs().Face(0).IsModified);
  // Shell not yet propagated during deferred mode.
  EXPECT_FALSE(myGraph.Defs().Shell(0).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: shell and solid should be propagated.
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_DirectShellMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutShell(0);

  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Solid(0).IsModified);

  myGraph.EndDeferredInvalidation();

  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_MultipleEdges_BatchPropagation)
{
  myGraph.BeginDeferredInvalidation();

  const int aNbEdges = myGraph.Defs().NbEdges();
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    myGraph.MutEdge(anEdgeIdx)->Tolerance = 0.1;
  }

  // During deferred mode: all edges modified, but no parent propagation yet.
  for (int aWireIdx = 0; aWireIdx < myGraph.Defs().NbWires(); ++aWireIdx)
  {
    EXPECT_FALSE(myGraph.Defs().Wire(aWireIdx).IsModified);
  }

  myGraph.EndDeferredInvalidation();

  // After flush: all wires, faces, shells, solids should be modified.
  for (int aWireIdx = 0; aWireIdx < myGraph.Defs().NbWires(); ++aWireIdx)
  {
    EXPECT_TRUE(myGraph.Defs().Wire(aWireIdx).IsModified);
  }
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(myGraph.Defs().Face(aFaceIdx).IsModified);
  }
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_ReconstructAfterFlush_Succeeds)
{
  // Modify an edge in deferred mode and verify reconstruction still works.
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(0)->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Reconstruction should succeed (shape cache was cleared on flush).
  const BRepGraph_NodeId aSolidId = BRepGraph_NodeId::Solid(0);
  TopoDS_Shape           aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aSolidId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_ParallelMutation_NoDataRace)
{
  const int aNbEdges = myGraph.Defs().NbEdges();
  ASSERT_GT(aNbEdges, 1);

  myGraph.BeginDeferredInvalidation();
  OSD_Parallel::For(
    0,
    aNbEdges,
    [&](int theIdx) { myGraph.MutEdge(theIdx)->Tolerance = 0.1 + theIdx * 0.01; },
    false);
  myGraph.EndDeferredInvalidation();

  // All edges should be modified.
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    EXPECT_TRUE(myGraph.Defs().Edge(anEdgeIdx).IsModified);
    EXPECT_NEAR(myGraph.Defs().Edge(anEdgeIdx).Tolerance,
                0.1 + anEdgeIdx * 0.01,
                Precision::Confusion());
  }

  // Propagation should have happened on flush.
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_NoMutations_FlushIsSafe)
{
  // Begin/End with no mutations in between should be a no-op.
  myGraph.BeginDeferredInvalidation();
  myGraph.EndDeferredInvalidation();

  // Nothing should be modified.
  EXPECT_FALSE(myGraph.Defs().Edge(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Wire(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Face(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, EndWithoutBegin_IsIdempotent)
{
  // Calling End without Begin should be a safe no-op.
  EXPECT_NO_THROW(myGraph.EndDeferredInvalidation());

  // Nothing should be modified or cleared.
  EXPECT_FALSE(myGraph.Defs().Edge(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_DoubleEnd_IsIdempotent)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(0)->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Second End should be a safe no-op.
  EXPECT_NO_THROW(myGraph.EndDeferredInvalidation());
  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_SameEdgeMutatedTwice)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutEdge(0)->Tolerance = 0.1;
  myGraph.MutEdge(0)->Tolerance = 0.5;
  myGraph.EndDeferredInvalidation();

  // Last write wins.
  EXPECT_NEAR(myGraph.Defs().Edge(0).Tolerance, 0.5, Precision::Confusion());
  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}

TEST_F(BRepGraphDeferredInvalidationTest, DeferredMode_DirectWireMutation_PropagatesUp)
{
  myGraph.BeginDeferredInvalidation();
  myGraph.MutWire(0);

  EXPECT_TRUE(myGraph.Defs().Wire(0).IsModified);
  EXPECT_FALSE(myGraph.Defs().Face(0).IsModified);

  myGraph.EndDeferredInvalidation();

  // After flush: face, shell, solid should be propagated.
  bool aFacePropagated = false;
  for (int aFI = 0; aFI < myGraph.Defs().NbFaces(); ++aFI)
  {
    if (myGraph.Defs().Face(aFI).OuterWireIdx() == 0 && myGraph.Defs().Face(aFI).IsModified)
    {
      aFacePropagated = true;
      break;
    }
  }
  EXPECT_TRUE(aFacePropagated);
  EXPECT_TRUE(myGraph.Defs().Shell(0).IsModified);
  EXPECT_TRUE(myGraph.Defs().Solid(0).IsModified);
}
