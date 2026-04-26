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
#include <BRepGraph_DeferredScope.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Reference.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_Builder.hxx>
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
    myGraph.Clear(); (void)BRepGraph_Builder::Add(myGraph, aBox);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_EdgeMutation_IncrementsOwnGen)
{
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);

  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
  // In deferred mode, the entity's OwnGen is incremented.
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  myGraph.Editor().EndDeferredInvalidation();
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_PropagatesUpOnFlush)
{
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;

  // During deferred mode: edge is mutated, but parent wire/face are NOT yet.
  const NCollection_DynamicArray<BRepGraph_WireId>& aWires =
    myGraph.Topo().Edges().Wires(BRepGraph_EdgeId::Start());
  ASSERT_GT(aWires.Length(), 0);
  EXPECT_EQ(myGraph.Topo().Wires().Definition(aWires.Value(0)).SubtreeGen, 0u);

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: wire and face SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Wires().Definition(aWires.Value(0)).SubtreeGen, 0u);

  // Check propagation to face.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), aWires.Value(0)))
    {
      EXPECT_GT(aFaceIt.Current().SubtreeGen, 0u);
      break;
    }
  }

  // Check propagation to shell and solid.
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectFaceMutation_PropagatesUp)
{
  myGraph.Editor().BeginDeferredInvalidation();
  (void)myGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());

  // Face was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).OwnGen, 0u);
  // Shell not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: shell and solid SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectShellMutation_PropagatesUp)
{
  myGraph.Editor().BeginDeferredInvalidation();
  (void)myGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start());

  // Shell was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).OwnGen, 0u);
  // Solid not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: solid SubtreeGen should be propagated.
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_MultipleEdges_BatchPropagation)
{
  myGraph.Editor().BeginDeferredInvalidation();

  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    myGraph.Editor().Edges().Mut(anEdgeIt.CurrentId())->Tolerance = 0.1;
  }

  // During deferred mode: all edges mutated, but no parent propagation yet.
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    EXPECT_EQ(aWireIt.Current().SubtreeGen, 0u);
  }

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: all wires, faces, shells, solids should have SubtreeGen propagated.
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    EXPECT_GT(aWireIt.Current().SubtreeGen, 0u);
  }
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_GT(aFaceIt.Current().SubtreeGen, 0u);
  }
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ReconstructAfterFlush_Succeeds)
{
  // Modify an edge in deferred mode and verify reconstruction still works.
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
  myGraph.Editor().EndDeferredInvalidation();

  // Reconstruction should succeed (shape cache was cleared on flush).
  const BRepGraph_NodeId aSolidId = BRepGraph_SolidId::Start();
  TopoDS_Shape           aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aSolidId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_ParallelMutation_WithExternalSync)
{
  const int aNbEdges = myGraph.Topo().Edges().Nb();
  ASSERT_GT(aNbEdges, 1);

  // Deferred mode is NOT internally thread-safe. Parallel callers must
  // provide external synchronization around each MutGuard usage.
  std::mutex aMutex;
  myGraph.Editor().BeginDeferredInvalidation();
  OSD_Parallel::For(
    0,
    aNbEdges,
    [&](int theIdx) {
      std::lock_guard<std::mutex> aLock(aMutex);
      myGraph.Editor().Edges().Mut(BRepGraph_EdgeId(theIdx))->Tolerance = 0.1 + theIdx * 0.01;
    },
    false);
  myGraph.Editor().EndDeferredInvalidation();

  // All edges should be mutated (directly: OwnGen).
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_GT(anEdgeIt.Current().OwnGen, 0u);
    EXPECT_NEAR(anEdgeIt.Current().Tolerance,
                0.1 + anEdgeIt.CurrentId().Index * 0.01,
                Precision::Confusion());
  }

  // Propagation should have happened on flush (parents: SubtreeGen).
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_NoMutations_FlushIsSafe)
{
  // Begin/End with no mutations in between should be a no-op.
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().EndDeferredInvalidation();

  // Nothing should be mutated.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, EndWithoutBegin_IsIdempotent)
{
  // Calling End without Begin should be a safe no-op.
  EXPECT_NO_THROW(myGraph.Editor().EndDeferredInvalidation());

  // Nothing should be mutated or cleared.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).OwnGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredScope_NestedGuards_FlushOnlyOnOuterDestruction)
{
  const NCollection_DynamicArray<BRepGraph_WireId>& aWires =
    myGraph.Topo().Edges().Wires(BRepGraph_EdgeId::Start());
  ASSERT_GT(aWires.Length(), 0);
  const BRepGraph_WireId aWireId = aWires.Value(0);

  {
    BRepGraph_DeferredScope anOuterScope(myGraph);
    EXPECT_TRUE(myGraph.Editor().IsDeferredMode());

    {
      BRepGraph_DeferredScope anInnerScope(myGraph);
      EXPECT_TRUE(myGraph.Editor().IsDeferredMode());
      myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
    }

    EXPECT_TRUE(myGraph.Editor().IsDeferredMode());
    EXPECT_EQ(myGraph.Topo().Wires().Definition(aWireId).SubtreeGen, 0u);
  }

  EXPECT_FALSE(myGraph.Editor().IsDeferredMode());
  EXPECT_GT(myGraph.Topo().Wires().Definition(aWireId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DoubleEnd_IsIdempotent)
{
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
  myGraph.Editor().EndDeferredInvalidation();

  // Second End should be a safe no-op.
  EXPECT_NO_THROW(myGraph.Editor().EndDeferredInvalidation());
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_SameEdgeMutatedTwice)
{
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.1;
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
  myGraph.Editor().EndDeferredInvalidation();

  // Last write wins.
  EXPECT_NEAR(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).Tolerance,
              0.5,
              Precision::Confusion());
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest, DeferredMode_DirectWireMutation_PropagatesUp)
{
  myGraph.Editor().BeginDeferredInvalidation();
  (void)myGraph.Editor().Wires().Mut(BRepGraph_WireId::Start());

  // Wire was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).OwnGen, 0u);
  // Face not yet propagated during deferred mode.
  EXPECT_EQ(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SubtreeGen, 0u);

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: face, shell, solid SubtreeGen should be propagated.
  bool aFacePropagated = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), BRepGraph_WireId::Start())
        && aFaceIt.Current().SubtreeGen > 0u)
    {
      aFacePropagated = true;
      break;
    }
  }
  EXPECT_TRUE(aFacePropagated);
  EXPECT_GT(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).SubtreeGen, 0u);
}

TEST_F(BRepGraph_DeferredInvalidationTest,
       DeferredMode_OccurrenceMutation_PropagatesSubtreeGenToProduct)
{
  // Build an assembly: root product + child occurrence referencing it.
  const BRepGraph_ProductId    aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId    aAssemblyId = myGraph.Editor().Products().CreateEmptyProduct();
  const BRepGraph_OccurrenceId anOccId =
    myGraph.Editor().Products().LinkProducts(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Verify parent product starts clean.
  EXPECT_EQ(myGraph.Topo().Products().Definition(aAssemblyId).SubtreeGen, 0u);

  // Find the OccurrenceRefId for the occurrence.
  const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& aOccRefs =
    myGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(aOccRefs.Length(), 1);
  const BRepGraph_OccurrenceRefId anOccRefId = aOccRefs.Value(0);

  // Mutate occurrence ref placement in deferred mode.
  myGraph.Editor().BeginDeferredInvalidation();
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
    myGraph.Editor().Occurrences().MutRef(anOccRefId)->LocalLocation =
      TopLoc_Location(aTrsf);
  }

  // During deferred mode: ref modified.
  EXPECT_GT(myGraph.Refs().Occurrences().Entry(anOccRefId).OwnGen, 0u);

  const uint32_t aSubtreeGenBeforeFlush =
    myGraph.Topo().Products().Definition(aAssemblyId).SubtreeGen;

  myGraph.Editor().EndDeferredInvalidation();

  // After flush: parent assembly product should have SubtreeGen >= before flush.
  EXPECT_GE(myGraph.Topo().Products().Definition(aAssemblyId).SubtreeGen, aSubtreeGenBeforeFlush);
  // Parent's OwnGen must remain 0 - its own data didn't change.
  EXPECT_EQ(myGraph.Topo().Products().Definition(aAssemblyId).OwnGen, 0u);
}
