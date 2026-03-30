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
#include <BRepGraph_DeferredScope.hxx>
#include <BRepGraph_PathView.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>

#include <mutex>

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

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_EdgeMutation_IncrementsOwnGen)
{
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);

  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  // In deferred mode, the entity's OwnGen is incremented.
  EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
  myGraph.Builder().EndDeferredInvalidation();
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_PropagatesUpOnFlush)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  // During deferred mode: edge is mutated, but parent wire/face are NOT yet.
  const NCollection_Vector<BRepGraph_WireId>& aWires =
    myGraph.Topo().WiresOfEdge(BRepGraph_EdgeId(0));
  ASSERT_GT(aWires.Length(), 0);
  EXPECT_EQ(myGraph.Topo().Wire(aWires.Value(0)).SubtreeGen, 0u);

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: wire and face SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Wire(aWires.Value(0)).SubtreeGen, 0u);

  // Check propagation to face.
  for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, BRepGraph_FaceId(aFI), aWires.Value(0)))
    {
      EXPECT_GT(myGraph.Topo().Face(BRepGraph_FaceId(aFI)).SubtreeGen, 0u);
      break;
    }
  }

  // Check propagation to shell and solid.
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectFaceMutation_PropagatesUp)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutFace(BRepGraph_FaceId(0));

  // Face was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Face(BRepGraph_FaceId(0)).OwnGen, 0u);
  // Shell not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: shell and solid SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectShellMutation_PropagatesUp)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutShell(BRepGraph_ShellId(0));

  // Shell was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).OwnGen, 0u);
  // Solid not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: solid SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_MultipleEdges_BatchPropagation)
{
  myGraph.Builder().BeginDeferredInvalidation();

  const int aNbEdges = myGraph.Topo().NbEdges();
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    myGraph.Builder().MutEdge(BRepGraph_EdgeId(anEdgeIdx))->Tolerance = 0.1;
  }

  // During deferred mode: all edges mutated, but no parent propagation yet.
  for (int aWireIdx = 0; aWireIdx < myGraph.Topo().NbWires(); ++aWireIdx)
  {
    EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(aWireIdx)).SubtreeGen, 0u);
  }

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: all wires, faces, shells, solids should have SubtreeGen propagated.
  for (int aWireIdx = 0; aWireIdx < myGraph.Topo().NbWires(); ++aWireIdx)
  {
    EXPECT_GT(myGraph.Topo().Wire(BRepGraph_WireId(aWireIdx)).SubtreeGen, 0u);
  }
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_GT(myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx)).SubtreeGen, 0u);
  }
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ReconstructAfterFlush_Succeeds)
{
  // Modify an edge in deferred mode and verify reconstruction still works.
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().EndDeferredInvalidation();

  // Reconstruction should succeed (shape cache was cleared on flush).
  const BRepGraph_NodeId aSolidId = BRepGraph_NodeId::Solid(0);
  TopoDS_Shape           aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aSolidId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ParallelMutation_WithExternalSync)
{
  const int aNbEdges = myGraph.Topo().NbEdges();
  ASSERT_GT(aNbEdges, 1);

  // Deferred mode is NOT internally thread-safe. Parallel callers must
  // provide external synchronization around each MutGuard usage.
  std::mutex aMutex;
  myGraph.Builder().BeginDeferredInvalidation();
  OSD_Parallel::For(
    0,
    aNbEdges,
    [&](int theIdx) {
      std::lock_guard<std::mutex> aLock(aMutex);
      myGraph.Builder().MutEdge(BRepGraph_EdgeId(theIdx))->Tolerance = 0.1 + theIdx * 0.01;
    },
    false);
  myGraph.Builder().EndDeferredInvalidation();

  // All edges should be mutated (directly: OwnGen).
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx)).OwnGen, 0u);
    EXPECT_NEAR(myGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx)).Tolerance,
                0.1 + anEdgeIdx * 0.01,
                Precision::Confusion());
  }

  // Propagation should have happened on flush (parents: SubtreeGen).
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_NoMutations_FlushIsSafe)
{
  // Begin/End with no mutations in between should be a no-op.
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().EndDeferredInvalidation();

  // Nothing should be mutated.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(0)).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(0)).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, EndWithoutBegin_IsIdempotent)
{
  // Calling End without Begin should be a safe no-op.
  EXPECT_NO_THROW(myGraph.Builder().EndDeferredInvalidation());

  // Nothing should be mutated or cleared.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(0)).OwnGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredScope_NestedGuards_FlushOnlyOnOuterDestruction)
{
  const NCollection_Vector<BRepGraph_WireId>& aWires = myGraph.Topo().WiresOfEdge(BRepGraph_EdgeId(0));
  ASSERT_GT(aWires.Length(), 0);
  const BRepGraph_WireId aWireId = aWires.Value(0);

  {
    BRepGraph_DeferredScope anOuterScope(myGraph);
    EXPECT_TRUE(myGraph.Builder().IsDeferredMode());

    {
      BRepGraph_DeferredScope anInnerScope(myGraph);
      EXPECT_TRUE(myGraph.Builder().IsDeferredMode());
      myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
    }

    EXPECT_TRUE(myGraph.Builder().IsDeferredMode());
    EXPECT_EQ(myGraph.Topo().Wire(aWireId).SubtreeGen, 0u);
  }

  EXPECT_FALSE(myGraph.Builder().IsDeferredMode());
  EXPECT_GT(myGraph.Topo().Wire(aWireId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DoubleEnd_IsIdempotent)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().EndDeferredInvalidation();

  // Second End should be a safe no-op.
  EXPECT_NO_THROW(myGraph.Builder().EndDeferredInvalidation());
  EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_SameEdgeMutatedTwice)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.1;
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().EndDeferredInvalidation();

  // Last write wins.
  EXPECT_NEAR(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).Tolerance, 0.5, Precision::Confusion());
  EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectWireMutation_PropagatesUp)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutWire(BRepGraph_WireId(0));

  // Wire was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Wire(BRepGraph_WireId(0)).OwnGen, 0u);
  // Face not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(0)).SubtreeGen, 0u);

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: face, shell, solid SubtreeGen should be propagated.
  bool aFacePropagated = false;
  for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, BRepGraph_FaceId(aFI), BRepGraph_WireId(0))
        && myGraph.Topo().Face(BRepGraph_FaceId(aFI)).SubtreeGen > 0u)
    {
      aFacePropagated = true;
      break;
    }
  }
  EXPECT_TRUE(aFacePropagated);
  EXPECT_GT(myGraph.Topo().Shell(BRepGraph_ShellId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solid(BRepGraph_SolidId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_OccurrenceMutation_PropagatesSubtreeGenToProduct)
{
  // Build an assembly: root product + child occurrence referencing it.
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId(0);
  const BRepGraph_ProductId aAssemblyId = myGraph.Builder().AddAssemblyProduct();
  const BRepGraph_OccurrenceId anOccId =
    myGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Verify parent product starts clean.
  EXPECT_EQ(myGraph.Topo().Product(aAssemblyId).SubtreeGen, 0u);

  // Mutate occurrence placement in deferred mode.
  myGraph.Builder().BeginDeferredInvalidation();
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
    myGraph.Builder().MutOccurrence(anOccId)->Placement = TopLoc_Location(aTrsf);
  }

  // During deferred mode: occurrence OwnGen incremented, but parent product NOT yet.
  EXPECT_GT(myGraph.Topo().Occurrence(anOccId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Product(aAssemblyId).SubtreeGen, 0u);

  myGraph.Builder().EndDeferredInvalidation();

  // After flush: parent assembly product must have SubtreeGen incremented exactly once.
  EXPECT_EQ(myGraph.Topo().Product(aAssemblyId).SubtreeGen, 1u);
  // Parent's OwnGen must remain 0 - its own data didn't change.
  EXPECT_EQ(myGraph.Topo().Product(aAssemblyId).OwnGen, 0u);
}
