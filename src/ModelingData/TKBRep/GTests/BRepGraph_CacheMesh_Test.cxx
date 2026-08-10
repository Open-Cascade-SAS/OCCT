// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

// Regression coverage for BRepGraph_CacheMesh freshness: verifies that a
// cached triangulation becomes stale when the owning Face's OwnGen bumps,
// whether the bump comes from a direct FaceDef mutation or from a face-owned
// surface use mutation.

#include <BRepGraph.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
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

BRepGraph_EdgeId firstEdgeId(const BRepGraph& theGraph)
{
  BRepGraph_EdgeIterator anEdgeIt(theGraph);
  return anEdgeIt.More() ? anEdgeIt.CurrentId() : BRepGraph_EdgeId();
}

BRepGraph_VertexId firstVertexId(const BRepGraph& theGraph)
{
  BRepGraph_VertexIterator aVtxIt(theGraph);
  return aVtxIt.More() ? aVtxIt.CurrentId() : BRepGraph_VertexId();
}

BRepGraph_CoEdgeId firstCoEdgeOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  for (BRepGraph_RefsWireOfFace aWireRefIt(theGraph, theFaceId); aWireRefIt.More();
       aWireRefIt.Next())
  {
    const BRepGraph_WireId aWireId =
      theGraph.Refs().Wires().Entry(aWireRefIt.CurrentId()).ChildWireId;
    for (BRepGraph_CoEdgesOfWire aCoEdgeIt(theGraph, aWireId); aCoEdgeIt.More(); aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
      if (theGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceId == theFaceId)
      {
        return aCoEdgeId;
      }
    }
  }
  return BRepGraph_CoEdgeId();
}

void writeFaceMesh(BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
  theGraph.Editor().Faces().SetPersistentTriangulation(theFaceId, aTri);
  theGraph.Mesh().Editor().Faces().SetCachedTriangulation(theFaceId, aTri);
}

void writeCoEdgeMesh(BRepGraph&               theGraph,
                     const BRepGraph_CoEdgeId theCoEdgeId,
                     const BRepGraph_FaceId   theFaceId)
{
  const occ::handle<Poly_Polygon2D> aPoly2D = new Poly_Polygon2D(2);
  theGraph.Mesh().Editor().CoEdges().SetCachedPolygon2D(theCoEdgeId, aPoly2D);
  const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
  theGraph.Mesh().Editor().Faces().SetCachedTriangulation(theFaceId, aTri);
  const occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  theGraph.Mesh().Editor().CoEdges().AppendCachedPolygonOnTri(theCoEdgeId, aPolyOnTri);
}

void writeEdgeMesh(BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
{
  const occ::handle<Poly_Polygon3D> aPoly3D = new Poly_Polygon3D(2, false);
  theGraph.Mesh().Editor().Edges().SetCachedPolygon3D(theEdgeId, aPoly3D);
}

} // namespace

TEST(BRepGraph_CacheMeshTest, CacheStaleAfterFaceMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);

  const BRepGraph_CacheMesh::FaceMeshEntry* aBefore = aGraph.Mesh().Cache().Faces().Entry(aFaceId);
  ASSERT_NE(aBefore, nullptr) << "CachedMesh must be present immediately after write";

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(aFaceId);
    aGraph.Editor().Faces().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  const BRepGraph_CacheMesh::FaceMeshEntry* aAfter = aGraph.Mesh().Cache().Faces().Entry(aFaceId);
  EXPECT_EQ(aAfter, nullptr) << "CachedMesh must become null (stale) after Face Mut bumps OwnGen";
}

TEST(BRepGraph_CacheMeshTest, CacheStaleAfterSurfaceRepMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);

  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  aGraph.Editor().Faces().ClearSurface(aFaceId);

  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "CachedMesh must become null after SurfaceRep Set propagates to Face OwnGen";
}

TEST(BRepGraph_CacheMeshTest, CacheSurvivesUnrelatedMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  BRepGraph_FaceIterator anOther(aGraph);
  anOther.Next();
  ASSERT_TRUE(anOther.More()) << "Box should have >1 face for this test";
  const BRepGraph_FaceId anOtherFaceId = anOther.CurrentId();

  const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(anOtherFaceId);
    aGraph.Editor().Faces().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "Unrelated face mutation must not invalidate this face's cache";
}

TEST(BRepGraph_CacheMeshTest, ClearDropsLargeCacheAndAllowsSmallRegenerate)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<Poly_Triangulation> aCheckTri = new Poly_Triangulation(3, 1, false);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aCheckTri);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  aGraph.CacheRegistry().ClearAll();
  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aCheckTri);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);
  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId)->Triangulation, aCheckTri);
}

// ---- Step 15: New tests for CacheMesh freshness redesign ----

TEST(BRepGraph_CacheMeshTest, FaceCache_StaleAfterEdgeMutation)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  writeFaceMesh(aGraph, aFaceId);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  const BRepGraph_EdgeId anEdgeId = firstEdgeId(aGraph);
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aGuard = aGraph.Editor().Edges().Mut(anEdgeId);
    aGraph.Editor().Edges().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "Face cache must become stale after edge mutation (SubtreeGen propagation)";
}

TEST(BRepGraph_CacheMeshTest, FaceCache_StaleAfterCoEdgeMutation)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeFaceMesh(aGraph, aFaceId);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aGuard = aGraph.Editor().CoEdges().Mut(aCoEdgeId);
    aGraph.Editor().CoEdges().SetOrientation(aGuard, TopAbs_REVERSED);
  }

  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "Face cache must become stale after coedge mutation (SubtreeGen propagation)";
}

TEST(BRepGraph_CacheMeshTest, FaceCache_StaleAfterVertexMutation)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_VertexId aVtxId = firstVertexId(aGraph);
  ASSERT_TRUE(aVtxId.IsValid(aGraph.Topo().Vertices().Nb()));

  writeFaceMesh(aGraph, aFaceId);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard = aGraph.Editor().Vertices().Mut(aVtxId);
    aGraph.Editor().Vertices().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "Face cache must become stale after vertex mutation (SubtreeGen propagation)";
}

TEST(BRepGraph_CacheMeshTest, EdgeCache_StaleAfterVertexMutation)
{
  BRepGraph              aGraph   = makeBoxGraph();
  const BRepGraph_EdgeId anEdgeId = firstEdgeId(aGraph);
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  const BRepGraph_VertexId aVtxId = firstVertexId(aGraph);
  ASSERT_TRUE(aVtxId.IsValid(aGraph.Topo().Vertices().Nb()));

  writeEdgeMesh(aGraph, anEdgeId);
  ASSERT_NE(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard = aGraph.Editor().Vertices().Mut(aVtxId);
    aGraph.Editor().Vertices().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr)
    << "Edge cache must become stale after vertex mutation";
}

TEST(BRepGraph_CacheMeshTest, EdgeCache_FreshAfterUnrelatedFaceMutation)
{
  BRepGraph              aGraph   = makeBoxGraph();
  const BRepGraph_EdgeId anEdgeId = firstEdgeId(aGraph);
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  writeEdgeMesh(aGraph, anEdgeId);
  ASSERT_NE(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr);

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(aFaceId);
    aGraph.Editor().Faces().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_NE(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr)
    << "Edge cache must survive unrelated face mutation";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_Polygon2DFreshAfterFaceMeshChange)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr);

  const occ::handle<Poly_Triangulation> aNewTri = new Poly_Triangulation(4, 1, false);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aNewTri);

  EXPECT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr)
    << "Polygon2D must stay fresh when only face mesh content changes";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_PolygonOnTriStaleAfterFaceMeshChange)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr);

  const occ::handle<Poly_Triangulation> aNewTri = new Poly_Triangulation(4, 1, false);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aNewTri);

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must become stale when face mesh content changes";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_PolygonOnTriStaleAfterFaceTopologyChange)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(aFaceId);
    aGraph.Editor().Faces().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must become stale when face topology changes";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_PolygonOnTriStaleAfterCoEdgeMutation)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aGuard = aGraph.Editor().CoEdges().Mut(aCoEdgeId);
    aGraph.Editor().CoEdges().SetOrientation(aGuard, TopAbs_REVERSED);
  }

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must become stale when coedge topology changes";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_FreshAfterUnrelatedEdgeMutation)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr);

  BRepGraph_EdgeIterator anEdgeIt(aGraph);
  anEdgeIt.Next();
  const BRepGraph_EdgeId anOtherEdgeId = anEdgeIt.CurrentId();
  if (anOtherEdgeId.IsValid(aGraph.Topo().Edges().Nb()))
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aGuard = aGraph.Editor().Edges().Mut(anOtherEdgeId);
    aGraph.Editor().Edges().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr)
    << "Polygon2D must survive unrelated edge mutation";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_UsesCoEdgeDefFaceId)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr);

  const BRepGraphInc::CoEdgeDef& aDef = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
  ASSERT_TRUE(aDef.FaceId.IsValid(aGraph.Topo().Faces().Nb()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aGuard = aGraph.Editor().Faces().Mut(aDef.FaceId);
    aGraph.Editor().Faces().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "Coedge freshness must use CoEdgeDef::FaceId for face binding";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_Polygon2DStaleAfterSlotRecipeChange)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr);

  aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->Clear();

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr)
    << "Polygon2D must become stale after cache clear (SlotGeneration bump)";
}

TEST(BRepGraph_CacheMeshTest, CoEdge_FaceNotYetMeshed_SnapshotZero)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  const occ::handle<Poly_Polygon2D> aPoly2D = new Poly_Polygon2D(2);
  aGraph.Mesh().Editor().CoEdges().SetCachedPolygon2D(aCoEdgeId, aPoly2D);
  const occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  aGraph.Mesh().Editor().CoEdges().AppendCachedPolygonOnTri(aCoEdgeId, aPolyOnTri);

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must be stale when face has never been meshed (snapshot=0 vs default=0)";

  writeFaceMesh(aGraph, aFaceId);

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must remain stale after face is meshed (MeshGeneration bumped to 1)";
}

TEST(BRepGraph_CacheMeshTest, Needs_DetectsChildDrivenFaceStaleness)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  writeFaceMesh(aGraph, aFaceId);
  ASSERT_FALSE(
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->Needs(aGraph, BRepGraph_NodeId(aFaceId)));

  const BRepGraph_VertexId aVtxId = firstVertexId(aGraph);
  ASSERT_TRUE(aVtxId.IsValid(aGraph.Topo().Vertices().Nb()));
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard = aGraph.Editor().Vertices().Mut(aVtxId);
    aGraph.Editor().Vertices().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_TRUE(
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->Needs(aGraph, BRepGraph_NodeId(aFaceId)))
    << "Needs(face) must detect staleness driven by child vertex mutation";
}

TEST(BRepGraph_CacheMeshTest, Needs_DetectsStaleCoedgeOnTri)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_FALSE(
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->Needs(aGraph, BRepGraph_NodeId(aFaceId)));

  const occ::handle<Poly_Triangulation> aNewTri = new Poly_Triangulation(4, 1, false);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aNewTri);

  EXPECT_TRUE(
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->Needs(aGraph, BRepGraph_NodeId(aFaceId)))
    << "Needs(face) must detect coedge PolygonOnTri staleness from face mesh change";
}

TEST(BRepGraph_CacheMeshTest, VertexPropagation_ReachesFaces)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_VertexId aVtxId = firstVertexId(aGraph);
  ASSERT_TRUE(aVtxId.IsValid(aGraph.Topo().Vertices().Nb()));

  writeFaceMesh(aGraph, aFaceId);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard = aGraph.Editor().Vertices().Mut(aVtxId);
    aGraph.Editor().Vertices().SetTolerance(aGuard, aGuard->Tolerance + 1.0e-6);
  }

  EXPECT_EQ(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr)
    << "Vertex mutation must propagate through edge -> wire -> face chain";
}

TEST(BRepGraph_CacheMeshTest, MeshGeneration_MonotonicAcrossClear)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  const BRepGraph_CacheMesh::CoEdgeMeshEntry* aCoEntry =
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId);
  ASSERT_NE(aCoEntry, nullptr);

  aGraph.Mesh().Editor().Faces().Clear(aFaceId);
  writeFaceMesh(aGraph, aFaceId);

  const BRepGraph_CacheMesh::CoEdgeMeshEntry* aAfterEntry =
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->findCoEdgeEntryRaw(
      BRepGraph_CacheMesh::DefaultDisplaySlot,
      aCoEdgeId);
  ASSERT_NE(aAfterEntry, nullptr);
  const BRepGraph_CacheMesh::FaceMeshEntry* aFaceEntry =
    aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->findFaceEntryRaw(
      BRepGraph_CacheMesh::DefaultDisplaySlot,
      aFaceId);
  ASSERT_NE(aFaceEntry, nullptr);
  EXPECT_GT(aFaceEntry->MeshGeneration, aAfterEntry->FaceMeshGeneration)
    << "Face MeshGeneration must exceed coedge snapshot after clear+rewrite";
}

TEST(BRepGraph_CacheMeshTest, FaceClear_PreservesCoEdgePolygon2D)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  ASSERT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr);

  aGraph.Mesh().Editor().Faces().Clear(aFaceId);

  EXPECT_NE(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygon2D(aCoEdgeId),
            nullptr)
    << "FaceOps::Clear must not destroy coedge Polygon2D";
}
