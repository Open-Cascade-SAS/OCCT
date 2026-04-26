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
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

class BRepGraph_MutationGenTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 = BRepGraph_Builder::Add(myGraph, aBox);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_MutationGenTest, OwnGen_IncrementedOnMutation)
{
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 0u);

  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_MultipleIncrements)
{
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.1;
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.2;

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 2u);
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_DeferredMode)
{
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;

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
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).SubtreeGen, 1u);

  // At least one parent in each level must have SubtreeGen incremented,
  // but OwnGen must remain 0 (parent's own data was not touched).
  bool aAnyWireSubtreeIncremented = false;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Current().SubtreeGen > 0u)
      aAnyWireSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // Verify parent OwnGen did NOT change (split behavior).
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
    EXPECT_EQ(aWireIt.Current().OwnGen, 0u);

  bool aAnyFaceSubtreeIncremented = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (aFaceIt.Current().SubtreeGen > 0u)
      aAnyFaceSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);

  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
    EXPECT_EQ(aFaceIt.Current().OwnGen, 0u);

  bool aAnyShellSubtreeIncremented = false;
  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
  {
    if (aShellIt.Current().SubtreeGen > 0u)
      aAnyShellSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyShellSubtreeIncremented);

  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
    EXPECT_EQ(aShellIt.Current().OwnGen, 0u);

  bool aAnySolidSubtreeIncremented = false;
  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
  {
    if (aSolidIt.Current().SubtreeGen > 0u)
      aAnySolidSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnySolidSubtreeIncremented);

  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
    EXPECT_EQ(aSolidIt.Current().OwnGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_DeferredPropagatedParent_Incremented)
{
  // Store baselines before mutation.
  const uint32_t aEdgeOwnGenBefore =
    myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen;
  NCollection_DynamicArray<uint32_t> aWireSubtreeGensBefore;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
    aWireSubtreeGensBefore.Append(aWireIt.Current().SubtreeGen);
  NCollection_DynamicArray<uint32_t> aFaceSubtreeGensBefore;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
    aFaceSubtreeGensBefore.Append(aFaceIt.Current().SubtreeGen);

  // Deferred mutation + flush.
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start())->Tolerance = 0.5;
  myGraph.Editor().EndDeferredInvalidation();

  // Directly mutated edge: OwnGen incremented by exactly 1.
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen,
            aEdgeOwnGenBefore + 1);

  // At least one parent wire must have SubtreeGen incremented vs its baseline.
  bool aAnyWireSubtreeIncremented = false;
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
    if (aWireIt.Current().SubtreeGen
        > aWireSubtreeGensBefore.Value(static_cast<int>(aWireIt.CurrentId().Index)))
      aAnyWireSubtreeIncremented = true;
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // At least one parent face must have SubtreeGen incremented vs its baseline.
  bool aAnyFaceSubtreeIncremented = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
    if (aFaceIt.Current().SubtreeGen
        > aFaceSubtreeGensBefore.Value(static_cast<int>(aFaceIt.CurrentId().Index)))
      aAnyFaceSubtreeIncremented = true;
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_SurfacePropagatesSubtreeGenToFace)
{
  const BRepGraph_FaceId       aFaceId(0);
  const BRepGraph_SurfaceRepId aSurfId = myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId;
  ASSERT_TRUE(aSurfId.IsValid());
  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);

  {
    BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> aGuard =
      myGraph.Editor().Reps().MutSurface(aSurfId);
    (void)aGuard;
  }

  // Surface is the face's own geometry - rep mutation IS an own-data change.
  EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve3DPropagatesSubtreeGenToEdge)
{
  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraph_Curve3DRepId aCurveId = myGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId;
  if (!aCurveId.IsValid())
    return; // Skip degenerate edges without 3D curves.

  EXPECT_EQ(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);

  {
    BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> aGuard =
      myGraph.Editor().Reps().MutCurve3D(aCurveId);
    (void)aGuard;
  }

  // Curve3D is the edge's own geometry - rep mutation IS an own-data change.
  EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve2DPropagatesSubtreeGenToCoEdge)
{
  // Find a coedge with a valid PCurve.
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(myGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_Curve2DRepId aCurveId = aCoEdgeIt.Current().Curve2DRepId;
    if (!aCurveId.IsValid())
      continue;

    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
    EXPECT_EQ(aCoEdgeIt.Current().OwnGen, 0u);
    EXPECT_EQ(aCoEdgeIt.Current().SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> aGuard =
        myGraph.Editor().Reps().MutCurve2D(aCurveId);
      (void)aGuard;
    }

    // Curve2D is the coedge's own geometry - rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().CoEdges().Definition(aCoEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().CoEdges().Definition(aCoEdgeId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_TriangulationPropagatesSubtreeGenToFace)
{
  // Find a face with a valid triangulation.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (!aFace.TriangulationRepId.IsValid())
      continue;

    const BRepGraph_FaceId             aFaceId = aFaceIt.CurrentId();
    const BRepGraph_TriangulationRepId aTriId  = aFace.TriangulationRepId;
    EXPECT_EQ(aFace.OwnGen, 0u);
    EXPECT_EQ(aFace.SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::TriangulationRep> aGuard =
        myGraph.Editor().Reps().MutTriangulation(aTriId);
      (void)aGuard;
    }

    // Triangulation is the face's own mesh - rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Polygon3DPropagatesSubtreeGenToEdge)
{
  // Find an edge with a valid Polygon3D.
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_Polygon3DRepId aPolyId = anEdgeIt.Current().Polygon3DRepId;
    if (!aPolyId.IsValid())
      continue;

    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_EQ(anEdgeIt.Current().OwnGen, 0u);
    EXPECT_EQ(anEdgeIt.Current().SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> aGuard =
        myGraph.Editor().Reps().MutPolygon3D(aPolyId);
      (void)aGuard;
    }

    // Polygon3D is the edge's own mesh - rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Edges().Definition(anEdgeId).SubtreeGen, 0u);
    return;
  }
}
