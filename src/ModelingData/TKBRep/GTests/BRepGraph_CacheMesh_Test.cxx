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
#include <BRepGraph_CopyRemap.hxx>
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

void writeFaceMesh(BRepGraph&                             theGraph,
                   const BRepGraph_CacheMesh::SlotId      theSlot,
                   const BRepGraph_FaceId                 theFaceId,
                   const occ::handle<Poly_Triangulation>& theTriangulation)
{
  occ::handle<BRepGraph_CacheMesh> aCache = theGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  BRepGraph_CacheMesh::FaceMeshEntry& anEntry = aCache->ChangeFaceMesh(theSlot, theFaceId);
  anEntry.Triangulation                       = theTriangulation;
  aCache->BindFresh(anEntry, theFaceId, theSlot);
  aCache->BumpFaceMeshGeneration(theFaceId, theSlot);
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

class SlotMeshDriver : public BRepGraph_CacheMesh::Driver
{
public:
  DEFINE_STANDARD_RTTI_INLINE(SlotMeshDriver, BRepGraph_CacheMesh::Driver)

  explicit SlotMeshDriver(const size_t theHash)
      : myHash(theHash)
  {
  }

  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("ff915308-f479-4b83-a901-ce4e3469c860");
    return THE_ID;
  }

  size_t RecipeHash() const override { return myHash; }

  bool Fill(BRepGraph&                           theGraph,
            const BRepGraph_CacheMesh::SlotId    theSlot,
            const BRepGraph_CacheMesh::DirtySet& theDirtySet,
            const Message_ProgressRange&) override
  {
    ++FillCount;
    LastSlot      = theSlot;
    LastFaceCount = static_cast<uint32_t>(theDirtySet.Faces.Size());

    for (const BRepGraph_FaceId& aFaceId : theDirtySet.Faces)
    {
      const occ::handle<Poly_Triangulation> aTri = makeTrivialTriangulation();
      writeFaceMesh(theGraph, theSlot, aFaceId, aTri);
    }
    return true;
  }

  occ::handle<BRepGraph_CacheMesh::Driver> Copy(const BRepGraph_CopyRemap&) const override
  {
    return new SlotMeshDriver(myHash);
  }

  uint32_t                    FillCount     = 0;
  BRepGraph_CacheMesh::SlotId LastSlot      = BRepGraph_CacheMesh::DefaultDisplaySlot;
  uint32_t                    LastFaceCount = 0;

private:
  size_t myHash = 0;
};

} // namespace

TEST(BRepGraph_CacheMeshTest, UnregisterMissingSlotDoesNotCreateSlot)
{
  BRepGraph                              aGraph;
  const occ::handle<BRepGraph_CacheMesh> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCache.IsNull());

  constexpr BRepGraph_CacheMesh::SlotId aMissingSlot = 42;
  EXPECT_EQ(aCache->State(aMissingSlot).Generation, 1u);

  aCache->UnregisterDriver(aMissingSlot);

  const BRepGraph_CacheMesh::SlotState aState = aCache->State(aMissingSlot);
  EXPECT_EQ(aState.Generation, 1u);
  EXPECT_EQ(aState.RecipeHash, 0u);
  EXPECT_FALSE(aState.HasDriver);
}

TEST(BRepGraph_CacheMeshTest, CopyFreshRejectsDivergentTargetWithEqualCounters)
{
  BRepGraph              aSource = makeBoxGraph();
  BRepGraph              aTarget = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aSource);
  ASSERT_TRUE(aFaceId.IsValid(aSource.Topo().Faces().Nb()));

  writeFaceMesh(aSource,
                BRepGraph_CacheMesh::DefaultDisplaySlot,
                aFaceId,
                makeTrivialTriangulation());
  aSource.CacheRegistry().CopyFreshCachesTo(aTarget,
                                            BRepGraph_CopyRemap::MappingKind::Identity,
                                            BRepGraph_CopyRemap::Mode::Copy,
                                            BRepGraph_CopyRemap::FreshnessPolicy::MatchTarget);

  const occ::handle<BRepGraph_CacheMesh> aTargetCache =
    aTarget.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aTargetCache.IsNull());
  EXPECT_EQ(aTargetCache->FindFaceMesh(aFaceId), nullptr);
}

TEST(BRepGraph_CacheMeshTest, FaceSlots_AreIsolatedAndActiveDisplaySlotRoutesEffectiveView)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<BRepGraph_CacheMesh> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  constexpr BRepGraph_CacheMesh::SlotId aDefaultSlot = BRepGraph_CacheMesh::DefaultDisplaySlot;
  constexpr BRepGraph_CacheMesh::SlotId aPreviewSlot = 3;

  const occ::handle<Poly_Triangulation> aDefaultTri = makeTrivialTriangulation();
  const occ::handle<Poly_Triangulation> aPreviewTri = makeTrivialTriangulation();
  writeFaceMesh(aGraph, aDefaultSlot, aFaceId, aDefaultTri);
  writeFaceMesh(aGraph, aPreviewSlot, aFaceId, aPreviewTri);

  ASSERT_NE(aCache->FindFaceMesh(aDefaultSlot, aFaceId), nullptr);
  ASSERT_NE(aCache->FindFaceMesh(aPreviewSlot, aFaceId), nullptr);
  EXPECT_EQ(aCache->FindFaceMesh(aDefaultSlot, aFaceId)->Triangulation, aDefaultTri);
  EXPECT_EQ(aCache->FindFaceMesh(aPreviewSlot, aFaceId)->Triangulation, aPreviewTri);

  aCache->SetActiveDisplaySlot(aDefaultSlot);
  EXPECT_EQ(aGraph.Mesh().Effective().Faces().Triangulation(aFaceId), aDefaultTri);

  aCache->SetActiveDisplaySlot(aPreviewSlot);
  EXPECT_EQ(aGraph.Mesh().Effective().Faces().Triangulation(aFaceId), aPreviewTri);

  aCache->ClearFaceMesh(aDefaultSlot, aFaceId);
  EXPECT_EQ(aCache->FindFaceMesh(aDefaultSlot, aFaceId), nullptr);
  ASSERT_NE(aCache->FindFaceMesh(aPreviewSlot, aFaceId), nullptr);
  EXPECT_EQ(aGraph.Mesh().Effective().Faces().Triangulation(aFaceId), aPreviewTri);
}

TEST(BRepGraph_CacheMeshTest, Ensure_UsesPerSlotDriverAndDirtyState)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<BRepGraph_CacheMesh> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  constexpr BRepGraph_CacheMesh::SlotId aCoarseSlot = 1;
  constexpr BRepGraph_CacheMesh::SlotId aFineSlot   = 5;

  const occ::handle<SlotMeshDriver> aCoarseDriver = new SlotMeshDriver(0x101u);
  const occ::handle<SlotMeshDriver> aFineDriver   = new SlotMeshDriver(0x202u);
  aCache->RegisterDriver(aCoarseSlot, aCoarseDriver);
  aCache->RegisterDriver(aFineSlot, aFineDriver);

  EXPECT_TRUE(aCache->Needs(aGraph, BRepGraph_NodeId(aFaceId), aCoarseSlot));
  EXPECT_TRUE(aCache->Needs(aGraph, BRepGraph_NodeId(aFaceId), aFineSlot));

  ASSERT_TRUE(aCache->Ensure(aGraph, BRepGraph_NodeId(aFaceId), aCoarseSlot));
  EXPECT_EQ(aCoarseDriver->FillCount, 1u);
  EXPECT_EQ(aCoarseDriver->LastSlot, aCoarseSlot);
  EXPECT_EQ(aCoarseDriver->LastFaceCount, 1u);
  EXPECT_FALSE(aCache->Needs(aGraph, BRepGraph_NodeId(aFaceId), aCoarseSlot));
  EXPECT_TRUE(aCache->Needs(aGraph, BRepGraph_NodeId(aFaceId), aFineSlot));

  ASSERT_TRUE(aCache->Ensure(aGraph, BRepGraph_NodeId(aFaceId), aFineSlot));
  EXPECT_EQ(aFineDriver->FillCount, 1u);
  EXPECT_EQ(aFineDriver->LastSlot, aFineSlot);
  EXPECT_EQ(aFineDriver->LastFaceCount, 1u);
  EXPECT_FALSE(aCache->Needs(aGraph, BRepGraph_NodeId(aFaceId), aFineSlot));

  ASSERT_NE(aCache->FindFaceMesh(aCoarseSlot, aFaceId), nullptr);
  ASSERT_NE(aCache->FindFaceMesh(aFineSlot, aFaceId), nullptr);
  EXPECT_NE(aCache->FindFaceMesh(aCoarseSlot, aFaceId)->Triangulation,
            aCache->FindFaceMesh(aFineSlot, aFaceId)->Triangulation);
}

TEST(BRepGraph_CacheMeshTest, Ensure_DeduplicatesRepeatedFaceRequests)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const occ::handle<BRepGraph_CacheMesh> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  const occ::handle<SlotMeshDriver> aDriver = new SlotMeshDriver(0x303u);
  aCache->RegisterDriver(BRepGraph_CacheMesh::DefaultDisplaySlot, aDriver);

  NCollection_Array1<BRepGraph_NodeId> aNodes(1, 3);
  aNodes.SetValue(1, BRepGraph_NodeId(aFaceId));
  aNodes.SetValue(2, BRepGraph_NodeId(aFaceId));
  aNodes.SetValue(3, BRepGraph_NodeId(aFaceId));

  ASSERT_TRUE(aCache->Ensure(aGraph, aNodes));
  EXPECT_EQ(aDriver->FillCount, 1u);
  EXPECT_EQ(aDriver->LastFaceCount, 1u);

  ASSERT_TRUE(aCache->Ensure(aGraph, aNodes));
  EXPECT_EQ(aDriver->FillCount, 1u) << "Fresh repeated requests must not refill cache";
}

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

TEST(BRepGraph_CacheMeshTest, CoEdge_FaceNotYetMeshed_RecordedGenerationZero)
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
    << "PolygonOnTri must be stale when face has never been meshed (stored generation=0 vs default=0)";

  writeFaceMesh(aGraph, aFaceId);

  EXPECT_EQ(aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>()->FindCoEdgePolygonOnTri(aCoEdgeId),
            nullptr)
    << "PolygonOnTri must remain stale after face is meshed (MeshGeneration bumped to 1)";
}

TEST(BRepGraph_CacheMeshTest, GenerationTracksEffectiveMeshChanges)
{
  BRepGraph                              aGraph = makeBoxGraph();
  const occ::handle<BRepGraph_CacheMesh> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  const uint64_t anInitialRevision = aCache->Generation();
  EXPECT_EQ(aCache->Generation(), anInitialRevision);

  const BRepGraph_FaceId aFace = firstFaceId(aGraph);
  ASSERT_TRUE(aFace.IsValid());
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFace, makeTrivialTriangulation());
  const uint64_t aFaceRevision = aCache->Generation();
  EXPECT_GT(aFaceRevision, anInitialRevision);

  const BRepGraph_EdgeId anEdge = firstEdgeId(aGraph);
  ASSERT_TRUE(anEdge.IsValid());
  writeEdgeMesh(aGraph, anEdge);
  const uint64_t anEdgeRevision = aCache->Generation();
  EXPECT_GT(anEdgeRevision, aFaceRevision);

  aCache->SetActiveDisplaySlot(1);
  const uint64_t aSlotRevision = aCache->Generation();
  EXPECT_GT(aSlotRevision, anEdgeRevision);
  aCache->SetActiveDisplaySlot(1);
  EXPECT_EQ(aCache->Generation(), aSlotRevision);

  aCache->Clear();
  EXPECT_GT(aCache->Generation(), aSlotRevision);
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
    << "Face MeshGeneration must exceed the stored coedge generation after clear+rewrite";
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

TEST(BRepGraph_CacheMeshTest, InvalidateFaceMesh_PreservesCoEdgePolygon2D)
{
  BRepGraph              aGraph  = makeBoxGraph();
  const BRepGraph_FaceId aFaceId = firstFaceId(aGraph);
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  writeCoEdgeMesh(aGraph, aCoEdgeId, aFaceId);
  occ::handle<BRepGraph_CacheMesh> aCache = aGraph.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCache.IsNull());
  ASSERT_NE(aCache->FindCoEdgePolygon2D(aCoEdgeId), nullptr);
  ASSERT_NE(aCache->FindCoEdgePolygonOnTri(aCoEdgeId), nullptr);

  aCache->InvalidateFaceMesh(aFaceId);

  EXPECT_EQ(aCache->FindFaceMesh(aFaceId), nullptr);
  EXPECT_NE(aCache->FindCoEdgePolygon2D(aCoEdgeId), nullptr)
    << "Quality-only invalidation must keep coedge Polygon2D";
  EXPECT_EQ(aCache->FindCoEdgePolygonOnTri(aCoEdgeId), nullptr);
}
