// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

// Regression coverage for BRepGraph_MeshCache freshness: verifies that a
// cached triangulation becomes stale when the owning Face's OwnGen bumps,
// whether the bump comes from a direct FaceDef mutation or from a
// SurfaceRep mutation that propagates through markRepModified().

#include <BRepGraph.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Poly_Triangulation.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

occ::handle<Poly_Triangulation> makeTrivialTriangulation()
{
  return new Poly_Triangulation(3, 1, false);
}

BRepGraph_FaceId firstFaceId(const BRepGraph& theGraph)
{
  BRepGraph_FaceIterator aFaceIt(theGraph);
  return aFaceIt.More() ? aFaceIt.CurrentId() : BRepGraph_FaceId();
}

} // namespace

TEST(BRepGraph_MeshCacheTest, CacheStaleAfterFaceMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_TriangulationRepId aTriRepId =
    BRepGraph_Tool::Mesh::CreateTriangulationRep(aGraph, makeTrivialTriangulation());
  ASSERT_TRUE(aTriRepId.IsValid());

  BRepGraph_Tool::Mesh::AppendCachedTriangulation(aGraph, aFaceId, aTriRepId);
  BRepGraph_Tool::Mesh::SetCachedActiveIndex(aGraph, aFaceId, 0);

  const BRepGraph_MeshCache::FaceMeshEntry* aBefore = aGraph.Mesh().Faces().CachedMesh(aFaceId);
  ASSERT_NE(aBefore, nullptr) << "CachedMesh must be present immediately after write";

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(aFaceId);
    aGuard->Tolerance += 1.0e-6;
  }

  const BRepGraph_MeshCache::FaceMeshEntry* aAfter = aGraph.Mesh().Faces().CachedMesh(aFaceId);
  EXPECT_EQ(aAfter, nullptr) << "CachedMesh must become null (stale) after Face Mut bumps OwnGen";
}

TEST(BRepGraph_MeshCacheTest, CacheStaleAfterSurfaceRepMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_SurfaceRepId aSurfaceRepId =
    aGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId;
  ASSERT_TRUE(aSurfaceRepId.IsValid());

  const BRepGraph_TriangulationRepId aTriRepId =
    BRepGraph_Tool::Mesh::CreateTriangulationRep(aGraph, makeTrivialTriangulation());
  ASSERT_TRUE(aTriRepId.IsValid());

  BRepGraph_Tool::Mesh::AppendCachedTriangulation(aGraph, aFaceId, aTriRepId);
  BRepGraph_Tool::Mesh::SetCachedActiveIndex(aGraph, aFaceId, 0);

  ASSERT_NE(aGraph.Mesh().Faces().CachedMesh(aFaceId), nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> aGuard =
      aGraph.Editor().Reps().MutSurface(aSurfaceRepId);
    (void)aGuard;
  }

  EXPECT_EQ(aGraph.Mesh().Faces().CachedMesh(aFaceId), nullptr)
    << "CachedMesh must become null after SurfaceRep Mut propagates to Face OwnGen";
}

TEST(BRepGraph_MeshCacheTest, CacheSurvivesUnrelatedMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  BRepGraph_FaceIterator anOther(aGraph);
  anOther.Next();
  ASSERT_TRUE(anOther.More()) << "Box should have >1 face for this test";
  const BRepGraph_FaceId anOtherFaceId = anOther.CurrentId();

  const BRepGraph_TriangulationRepId aTriRepId =
    BRepGraph_Tool::Mesh::CreateTriangulationRep(aGraph, makeTrivialTriangulation());
  ASSERT_TRUE(aTriRepId.IsValid());

  BRepGraph_Tool::Mesh::AppendCachedTriangulation(aGraph, aFaceId, aTriRepId);
  BRepGraph_Tool::Mesh::SetCachedActiveIndex(aGraph, aFaceId, 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(anOtherFaceId);
    aGuard->Tolerance += 1.0e-6;
  }

  EXPECT_NE(aGraph.Mesh().Faces().CachedMesh(aFaceId), nullptr)
    << "Unrelated face mutation must not invalidate this face's cache";
}
