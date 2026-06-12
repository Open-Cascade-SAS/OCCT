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
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_VersionStamp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopLoc_Location.hxx>

#include <gtest/gtest.h>

class BRepGraph_MutationGenTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = myGraph.Shapes().Add(aBox);
    ASSERT_FALSE(myGraph.IsEmpty());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_MutationGenTest, OwnGen_IncrementedOnMutation)
{
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 0u);

  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.5);

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 1u);
}

TEST(BRepGraph_MutationGenEditorTest, EditorCreationRegistersOwnGeneration)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  const BRepGraph_CoEdgeId aCoEdge = aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aCoEdge);
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());

  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(new Geom_Plane(gp_Pln()), aWire, anInnerWires.ToArray1(), 1.e-7);
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  const BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid);
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());

  NCollection_LinearVector<BRepGraph_SolidId> aSolids;
  aSolids.Append(aSolid);
  const BRepGraph_CompSolidId aCompSolid = aGraph.Editor().CompSolids().Add(aSolids.ToArray1());

  EXPECT_EQ(aGraph.Topo().Vertices().Definition(aV0).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Vertices().Definition(aV1).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).OwnGen, 3u);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdge).OwnGen, 2u);
  EXPECT_EQ(aGraph.Topo().Wires().Definition(aWire).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(aFace).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Shells().Definition(aShell).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Solids().Definition(aSolid).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Compounds().Definition(aCompound).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().CompSolids().Definition(aCompSolid).OwnGen, 1u);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraph_MutationGenEditorTest, ProductAppendRegistersBothProductsAndOccurrence)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_ProductId aParent = aGraph.Editor().Products().Add();
  const BRepGraph_ProductId aChild  = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aChild);

  const BRepGraph_VersionStamp aParentStamp  = aGraph.UIDs().StampOf(aParent);
  const BRepGraph_VersionStamp aChildStamp   = aGraph.UIDs().StampOf(aChild);
  const uint32_t               aParentOwnGen = aGraph.Topo().Products().Definition(aParent).OwnGen;
  const uint32_t               aChildOwnGen  = aGraph.Topo().Products().Definition(aChild).OwnGen;

  const BRepGraph_OccurrenceId anOccurrence =
    aGraph.Editor().Products().Append(aParent, aChild, TopLoc_Location());

  ASSERT_TRUE(anOccurrence.IsValid());
  EXPECT_TRUE(aGraph.UIDs().IsStale(aParentStamp));
  EXPECT_TRUE(aGraph.UIDs().IsStale(aChildStamp));
  EXPECT_EQ(aGraph.Topo().Occurrences().Definition(anOccurrence).OwnGen, 1u);
  EXPECT_EQ(aGraph.Topo().Products().Definition(aParent).OwnGen, aParentOwnGen + 1u);
  EXPECT_EQ(aGraph.Topo().Products().Definition(aChild).OwnGen, aChildOwnGen + 1u);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_MultipleIncrements)
{
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.1);
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.2);

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 2u);
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_DeferredMode)
{
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.5);

  // OwnGen is incremented even in deferred mode.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);

  myGraph.Editor().EndDeferredInvalidation();

  // Still 1 after flush - flush doesn't re-increment.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_PropagatedParent_Incremented)
{
  // Mutate an edge - parent wire/face/shell/solid get SubtreeGen incremented
  // via propagation, enabling generation-based cache freshness on parents.
  // Parent OwnGen must NOT change (only the edge itself was directly mutated).
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.5);

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 1u);

  // At least one parent in each level must have SubtreeGen incremented,
  // but OwnGen must remain 0 (parent's own data was not touched).
  bool aAnyWireSubtreeIncremented = false;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Current().SubtreeGen > 0u)
    {
      aAnyWireSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // Verify parent OwnGen did NOT change (split behavior).
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    EXPECT_EQ(aWireIt.Current().OwnGen, 0u);
  }

  bool aAnyFaceSubtreeIncremented = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (aFaceIt.Current().SubtreeGen > 0u)
    {
      aAnyFaceSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);

  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_EQ(aFaceIt.Current().OwnGen, 0u);
  }

  bool aAnyShellSubtreeIncremented = false;
  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
  {
    if (aShellIt.Current().SubtreeGen > 0u)
    {
      aAnyShellSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnyShellSubtreeIncremented);

  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
  {
    EXPECT_EQ(aShellIt.Current().OwnGen, 0u);
  }

  bool aAnySolidSubtreeIncremented = false;
  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
  {
    if (aSolidIt.Current().SubtreeGen > 0u)
    {
      aAnySolidSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnySolidSubtreeIncremented);

  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
  {
    EXPECT_EQ(aSolidIt.Current().OwnGen, 0u);
  }
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_DeferredPropagatedParent_Incremented)
{
  // Store baselines before mutation.
  const uint32_t aEdgeOwnGenBefore =
    myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen;
  NCollection_DynamicArray<uint32_t> aWireSubtreeGensBefore;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    aWireSubtreeGensBefore.Append(aWireIt.Current().SubtreeGen);
  }
  NCollection_DynamicArray<uint32_t> aFaceSubtreeGensBefore;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    aFaceSubtreeGensBefore.Append(aFaceIt.Current().SubtreeGen);
  }

  // Deferred mutation + flush.
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.5);
  myGraph.Editor().EndDeferredInvalidation();

  // Directly mutated edge: OwnGen incremented by exactly 1.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen,
            aEdgeOwnGenBefore + 1);

  // At least one parent wire must have SubtreeGen incremented vs its baseline.
  bool aAnyWireSubtreeIncremented = false;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Current().SubtreeGen
        > aWireSubtreeGensBefore.Value(static_cast<int>(aWireIt.CurrentId().Index)))
    {
      aAnyWireSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // At least one parent face must have SubtreeGen incremented vs its baseline.
  bool aAnyFaceSubtreeIncremented = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (aFaceIt.Current().SubtreeGen
        > aFaceSubtreeGensBefore.Value(static_cast<int>(aFaceIt.CurrentId().Index)))
    {
      aAnyFaceSubtreeIncremented = true;
    }
  }
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_SolidMutation_PropagatesThroughRootOccurrence)
{
  ASSERT_EQ(myGraph.RootProductIds().Size(), 1);
  const BRepGraph_ProductId    aRootProduct = myGraph.RootProductIds().Value(0);
  const BRepGraph_OccurrenceId aRootOccurrence =
    myGraph.Topo().Products().Component(aRootProduct, 0);
  ASSERT_TRUE(aRootOccurrence.IsValid());

  const uint32_t anOccurrenceSubtreeGenBefore =
    myGraph.Topo().Occurrences().Definition(aRootOccurrence).SubtreeGen;
  const uint32_t aProductSubtreeGenBefore =
    myGraph.Topo().Products().Definition(aRootProduct).SubtreeGen;

  const BRepGraph_ShellId aShell = myGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());
  const BRepGraph_ShellRefId aShellRef =
    myGraph.Editor().Solids().Append(BRepGraph_SolidId::Start(), aShell);
  ASSERT_TRUE(aShellRef.IsValid());

  EXPECT_GT(myGraph.Topo().Occurrences().Definition(aRootOccurrence).SubtreeGen,
            anOccurrenceSubtreeGenBefore);
  EXPECT_GT(myGraph.Topo().Products().Definition(aRootProduct).SubtreeGen,
            aProductSubtreeGenBefore);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_DeferredSolidMutation_PropagatesThroughRootOccurrence)
{
  ASSERT_EQ(myGraph.RootProductIds().Size(), 1);
  const BRepGraph_ProductId    aRootProduct = myGraph.RootProductIds().Value(0);
  const BRepGraph_OccurrenceId aRootOccurrence =
    myGraph.Topo().Products().Component(aRootProduct, 0);
  ASSERT_TRUE(aRootOccurrence.IsValid());

  const uint32_t anOccurrenceSubtreeGenBefore =
    myGraph.Topo().Occurrences().Definition(aRootOccurrence).SubtreeGen;
  const uint32_t aProductSubtreeGenBefore =
    myGraph.Topo().Products().Definition(aRootProduct).SubtreeGen;

  myGraph.Editor().BeginDeferredInvalidation();
  const BRepGraph_ShellId aShell = myGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());
  const BRepGraph_ShellRefId aShellRef =
    myGraph.Editor().Solids().Append(BRepGraph_SolidId::Start(), aShell);
  ASSERT_TRUE(aShellRef.IsValid());

  EXPECT_EQ(myGraph.Topo().Occurrences().Definition(aRootOccurrence).SubtreeGen,
            anOccurrenceSubtreeGenBefore);
  EXPECT_EQ(myGraph.Topo().Products().Definition(aRootProduct).SubtreeGen,
            aProductSubtreeGenBefore);

  myGraph.Editor().EndDeferredInvalidation();

  EXPECT_GT(myGraph.Topo().Occurrences().Definition(aRootOccurrence).SubtreeGen,
            anOccurrenceSubtreeGenBefore);
  EXPECT_GT(myGraph.Topo().Products().Definition(aRootProduct).SubtreeGen,
            aProductSubtreeGenBefore);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_SurfacePropagatesSubtreeGenToFace)
{
  const BRepGraph_FaceId aFaceId(0);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);

  myGraph.Editor().Faces().ClearSurface(aFaceId);

  EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve3DPropagatesSubtreeGenToEdge)
{
  const BRepGraph_EdgeId anEdgeId(0);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);

  myGraph.Editor().Edges().ClearCurve(anEdgeId);

  EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve2DPropagatesSubtreeGenToCoEdge)
{
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(myGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
    EXPECT_EQ(aCoEdgeIt.Current().OwnGen, 0u);
    EXPECT_EQ(aCoEdgeIt.Current().SubtreeGen, 0u);

    myGraph.Editor().CoEdges().ClearPCurve(aCoEdgeId);

    EXPECT_GT(myGraph.Topo().CoEdges().Definition(aCoEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().CoEdges().Definition(aCoEdgeId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_TriangulationPropagatesSubtreeGenToFace)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_EQ(aFaceIt.Current().OwnGen, 0u);
    EXPECT_EQ(aFaceIt.Current().SubtreeGen, 0u);

    myGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);

    EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Polygon3DPropagatesSubtreeGenToEdge)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_EQ(anEdgeIt.Current().OwnGen, 0u);
    EXPECT_EQ(anEdgeIt.Current().SubtreeGen, 0u);

    myGraph.Editor().Edges().ClearPersistentPolygon3D(anEdgeId);

    EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);
    return;
  }
}
