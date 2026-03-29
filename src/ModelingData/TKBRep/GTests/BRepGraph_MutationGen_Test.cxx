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
#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

class BRepGraph_MutationGenTest : public testing::Test
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

TEST_F(BRepGraph_MutationGenTest, OwnGen_IncrementedOnMutation)
{
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).SubtreeGen, 0u);

  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).SubtreeGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_MultipleIncrements)
{
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.1;
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.2;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 2u);
}

TEST_F(BRepGraph_MutationGenTest, OwnGen_DeferredMode)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  // OwnGen is incremented even in deferred mode.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 1u);

  myGraph.Builder().EndDeferredInvalidation();

  // Still 1 after flush - flush doesn't re-increment.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_PropagatedParent_Incremented)
{
  // Mutate an edge - parent wire/face/shell/solid get SubtreeGen incremented
  // via propagation, enabling generation-based cache freshness on parents.
  // Parent OwnGen must NOT change (only the edge itself was directly mutated).
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 1u);
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).SubtreeGen, 1u);

  // At least one parent in each level must have SubtreeGen incremented,
  // but OwnGen must remain 0 (parent's own data was not touched).
  bool aAnyWireSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbWires(); ++i)
  {
    if (myGraph.Topo().Wire(BRepGraph_WireId(i)).SubtreeGen > 0u)
      aAnyWireSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // Verify parent OwnGen did NOT change (split behavior).
  for (int i = 0; i < myGraph.Topo().NbWires(); ++i)
    EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(i)).OwnGen, 0u);

  bool aAnyFaceSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
  {
    if (myGraph.Topo().Face(BRepGraph_FaceId(i)).SubtreeGen > 0u)
      aAnyFaceSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);

  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
    EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(i)).OwnGen, 0u);

  bool aAnyShellSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbShells(); ++i)
  {
    if (myGraph.Topo().Shell(BRepGraph_ShellId(i)).SubtreeGen > 0u)
      aAnyShellSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnyShellSubtreeIncremented);

  for (int i = 0; i < myGraph.Topo().NbShells(); ++i)
    EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(i)).OwnGen, 0u);

  bool aAnySolidSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbSolids(); ++i)
  {
    if (myGraph.Topo().Solid(BRepGraph_SolidId(i)).SubtreeGen > 0u)
      aAnySolidSubtreeIncremented = true;
  }
  EXPECT_TRUE(aAnySolidSubtreeIncremented);

  for (int i = 0; i < myGraph.Topo().NbSolids(); ++i)
    EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(i)).OwnGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, SubtreeGen_DeferredPropagatedParent_Incremented)
{
  // Store baselines before mutation.
  const uint32_t aEdgeOwnGenBefore = myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen;
  NCollection_Vector<uint32_t> aWireSubtreeGensBefore;
  for (int i = 0; i < myGraph.Topo().NbWires(); ++i)
    aWireSubtreeGensBefore.Append(myGraph.Topo().Wire(BRepGraph_WireId(i)).SubtreeGen);
  NCollection_Vector<uint32_t> aFaceSubtreeGensBefore;
  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
    aFaceSubtreeGensBefore.Append(myGraph.Topo().Face(BRepGraph_FaceId(i)).SubtreeGen);

  // Deferred mutation + flush.
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().EndDeferredInvalidation();

  // Directly mutated edge: OwnGen incremented by exactly 1.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, aEdgeOwnGenBefore + 1);

  // At least one parent wire must have SubtreeGen incremented vs its baseline.
  bool aAnyWireSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbWires(); ++i)
    if (myGraph.Topo().Wire(BRepGraph_WireId(i)).SubtreeGen > aWireSubtreeGensBefore.Value(i))
      aAnyWireSubtreeIncremented = true;
  EXPECT_TRUE(aAnyWireSubtreeIncremented);

  // At least one parent face must have SubtreeGen incremented vs its baseline.
  bool aAnyFaceSubtreeIncremented = false;
  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
    if (myGraph.Topo().Face(BRepGraph_FaceId(i)).SubtreeGen > aFaceSubtreeGensBefore.Value(i))
      aAnyFaceSubtreeIncremented = true;
  EXPECT_TRUE(aAnyFaceSubtreeIncremented);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_SurfacePropagatesSubtreeGenToFace)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_SurfaceRepId aSurfId = myGraph.Topo().Face(aFaceId).SurfaceRepId;
  ASSERT_TRUE(aSurfId.IsValid());
  EXPECT_EQ(myGraph.Topo().Face(aFaceId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Face(aFaceId).SubtreeGen, 0u);

  {
    BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> aGuard = myGraph.Builder().MutSurface(aSurfId);
    (void)aGuard;
  }

  // Surface is the face's own geometry — rep mutation IS an own-data change.
  EXPECT_GT(myGraph.Topo().Face(aFaceId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Face(aFaceId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve3DPropagatesSubtreeGenToEdge)
{
  const BRepGraph_EdgeId anEdgeId(0);
  const BRepGraph_Curve3DRepId aCurveId = myGraph.Topo().Edge(anEdgeId).Curve3DRepId;
  if (!aCurveId.IsValid())
    return; // Skip degenerate edges without 3D curves.

  EXPECT_EQ(myGraph.Topo().Edge(anEdgeId).OwnGen, 0u);
  EXPECT_EQ(myGraph.Topo().Edge(anEdgeId).SubtreeGen, 0u);

  {
    BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> aGuard = myGraph.Builder().MutCurve3D(aCurveId);
    (void)aGuard;
  }

  // Curve3D is the edge's own geometry — rep mutation IS an own-data change.
  EXPECT_GT(myGraph.Topo().Edge(anEdgeId).OwnGen, 0u);
  EXPECT_GT(myGraph.Topo().Edge(anEdgeId).SubtreeGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Curve2DPropagatesSubtreeGenToCoEdge)
{
  // Find a coedge with a valid PCurve.
  for (int i = 0; i < myGraph.Topo().NbCoEdges(); ++i)
  {
    const BRepGraph_CoEdgeId aCoEdgeId(i);
    const BRepGraph_Curve2DRepId aCurveId = myGraph.Topo().CoEdge(aCoEdgeId).Curve2DRepId;
    if (!aCurveId.IsValid())
      continue;

    EXPECT_EQ(myGraph.Topo().CoEdge(aCoEdgeId).OwnGen, 0u);
    EXPECT_EQ(myGraph.Topo().CoEdge(aCoEdgeId).SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> aGuard = myGraph.Builder().MutCurve2D(aCurveId);
      (void)aGuard;
    }

    // Curve2D is the coedge's own geometry — rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().CoEdge(aCoEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().CoEdge(aCoEdgeId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_TriangulationPropagatesSubtreeGenToFace)
{
  // Find a face with a valid triangulation.
  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
  {
    const BRepGraph_FaceId aFaceId(i);
    const BRepGraphInc::FaceDef& aFace = myGraph.Topo().Face(aFaceId);
    if (aFace.TriangulationRepIds.IsEmpty())
      continue;

    const BRepGraph_TriangulationRepId aTriId = aFace.TriangulationRepIds.Value(0);
    EXPECT_EQ(aFace.OwnGen, 0u);
    EXPECT_EQ(aFace.SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::TriangulationRep> aGuard = myGraph.Builder().MutTriangulation(aTriId);
      (void)aGuard;
    }

    // Triangulation is the face's own mesh — rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().Face(aFaceId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Face(aFaceId).SubtreeGen, 0u);
    return;
  }
}

TEST_F(BRepGraph_MutationGenTest, RepMutation_Polygon3DPropagatesSubtreeGenToEdge)
{
  // Find an edge with a valid Polygon3D.
  for (int i = 0; i < myGraph.Topo().NbEdges(); ++i)
  {
    const BRepGraph_EdgeId anEdgeId(i);
    const BRepGraph_Polygon3DRepId aPolyId = myGraph.Topo().Edge(anEdgeId).Polygon3DRepId;
    if (!aPolyId.IsValid())
      continue;

    EXPECT_EQ(myGraph.Topo().Edge(anEdgeId).OwnGen, 0u);
    EXPECT_EQ(myGraph.Topo().Edge(anEdgeId).SubtreeGen, 0u);

    {
      BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> aGuard = myGraph.Builder().MutPolygon3D(aPolyId);
      (void)aGuard;
    }

    // Polygon3D is the edge's own mesh — rep mutation IS an own-data change.
    EXPECT_GT(myGraph.Topo().Edge(anEdgeId).OwnGen, 0u);
    EXPECT_GT(myGraph.Topo().Edge(anEdgeId).SubtreeGen, 0u);
    return;
  }
}
