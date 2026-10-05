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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_Cache.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <atomic>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace
{

bool edgeSameParameter(const BRepGraph& theGraph, BRepGraph_EdgeId theEdge)
{
  const auto& aRel = theGraph.Topo().Edges().Relations(theEdge);
  for (const auto& aCoEdgeId : aRel.CoEdgeIds)
  {
    if (!BRepGraph_Tool::CoEdge::SameParameter(theGraph, aCoEdgeId))
    {
      return false;
    }
  }
  return true;
}

bool edgeSameRange(const BRepGraph& theGraph, BRepGraph_EdgeId theEdge)
{
  const auto& aRel = theGraph.Topo().Edges().Relations(theEdge);
  for (const auto& aCoEdgeId : aRel.CoEdgeIds)
  {
    if (!BRepGraph_Tool::CoEdge::SameRange(theGraph, aCoEdgeId))
    {
      return false;
    }
  }
  return true;
}

class CopyTestCacheService : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(CopyTestCacheService, BRepGraph_Cache)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10026");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("CopyTestCacheService");
    return THE_NAME;
  }

  void Set(const BRepGraph_NodeId theNode) { myNodes.Add(theNode); }

  bool Has(const BRepGraph_NodeId theNode) const { return myNodes.Contains(theNode); }

private:
  NCollection_FlatMap<BRepGraph_NodeId> myNodes;
};

class CopyTestMeshDriver : public BRepGraph_CacheMesh::Driver
{
public:
  DEFINE_STANDARD_RTTI_INLINE(CopyTestMeshDriver, BRepGraph_CacheMesh::Driver)

  explicit CopyTestMeshDriver(const size_t theRecipeHash)
      : myRecipeHash(theRecipeHash)
  {
  }

  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("8f14b24b-4f20-4cb5-88d1-89eb339a8b88");
    return THE_ID;
  }

  size_t RecipeHash() const override { return myRecipeHash; }

  bool Fill(BRepGraph&,
            BRepGraph_CacheMesh::SlotId,
            const BRepGraph_CacheMesh::DirtySet&,
            const Message_ProgressRange&) override
  {
    return false;
  }

  occ::handle<BRepGraph_CacheMesh::Driver> Copy(const BRepGraph_CopyRemap& theCopy) const override
  {
    if (theCopy.IsCompact())
    {
      return occ::handle<BRepGraph_CacheMesh::Driver>(const_cast<CopyTestMeshDriver*>(this));
    }
    return new CopyTestMeshDriver(myRecipeHash);
  }

private:
  size_t myRecipeHash = 0;
};

template <typename IdT>
uint32_t countActive(const BRepGraph& theGraph, const uint32_t theCount)
{
  uint32_t aCount = 0;
  for (IdT anId(0); anId.IsValid(theCount); ++anId)
  {
    if (!anId.IsRemoved(theGraph))
    {
      ++aCount;
    }
  }
  return aCount;
}

static TopoDS_Edge makeEdgeWithInternalVertex()
{
  BRep_Builder            aBB;
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aMakeEdge.Edge();

  TopoDS_Vertex anIntVtx;
  aBB.MakeVertex(anIntVtx, gp_Pnt(5, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, anIntVtx.Oriented(TopAbs_INTERNAL));
  return anEdge;
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

void initMeshHandles(const occ::handle<Poly_Triangulation>&          theTriangulation,
                     const occ::handle<Poly_Polygon3D>&              thePolygon3D,
                     const occ::handle<Poly_Polygon2D>&              thePolygon2D,
                     const occ::handle<Poly_PolygonOnTriangulation>& thePolygonOnTri)
{
  theTriangulation->SetNode(1, gp_Pnt(1.0, 2.0, 3.0));
  theTriangulation->SetNode(2, gp_Pnt(4.0, 5.0, 6.0));
  theTriangulation->SetNode(3, gp_Pnt(7.0, 8.0, 9.0));
  theTriangulation->SetTriangle(1, Poly_Triangle(1, 2, 3));

  thePolygon3D->ChangeNodes().SetValue(1, gp_Pnt(10.0, 11.0, 12.0));
  thePolygon3D->ChangeNodes().SetValue(2, gp_Pnt(13.0, 14.0, 15.0));

  thePolygon2D->ChangeNodes().SetValue(1, gp_Pnt2d(16.0, 17.0));
  thePolygon2D->ChangeNodes().SetValue(2, gp_Pnt2d(18.0, 19.0));

  thePolygonOnTri->SetNode(1, 2);
  thePolygonOnTri->SetNode(2, 3);
}

void expectMeshHandlesCopied(const occ::handle<Poly_Triangulation>&          theCopiedTriangulation,
                             const occ::handle<Poly_Polygon3D>&              theCopiedPolygon3D,
                             const occ::handle<Poly_Polygon2D>&              theCopiedPolygon2D,
                             const occ::handle<Poly_PolygonOnTriangulation>& theCopiedPolygonOnTri,
                             const occ::handle<Poly_Triangulation>&          theSourceTriangulation,
                             const occ::handle<Poly_Polygon3D>&              theSourcePolygon3D,
                             const occ::handle<Poly_Polygon2D>&              theSourcePolygon2D,
                             const occ::handle<Poly_PolygonOnTriangulation>& theSourcePolygonOnTri)
{
  ASSERT_FALSE(theCopiedTriangulation.IsNull());
  ASSERT_FALSE(theCopiedPolygon3D.IsNull());
  ASSERT_FALSE(theCopiedPolygon2D.IsNull());
  ASSERT_FALSE(theCopiedPolygonOnTri.IsNull());

  EXPECT_NE(theCopiedTriangulation.get(), theSourceTriangulation.get());
  EXPECT_NE(theCopiedPolygon3D.get(), theSourcePolygon3D.get());
  EXPECT_NE(theCopiedPolygon2D.get(), theSourcePolygon2D.get());
  EXPECT_NE(theCopiedPolygonOnTri.get(), theSourcePolygonOnTri.get());

  EXPECT_EQ(theCopiedTriangulation->NbNodes(), theSourceTriangulation->NbNodes());
  EXPECT_EQ(theCopiedTriangulation->NbTriangles(), theSourceTriangulation->NbTriangles());
  EXPECT_TRUE(theCopiedTriangulation->Node(2).IsEqual(theSourceTriangulation->Node(2), 0.0));

  EXPECT_EQ(theCopiedPolygon3D->NbNodes(), theSourcePolygon3D->NbNodes());
  EXPECT_TRUE(
    theCopiedPolygon3D->Nodes().Value(2).IsEqual(theSourcePolygon3D->Nodes().Value(2), 0.0));

  EXPECT_EQ(theCopiedPolygon2D->NbNodes(), theSourcePolygon2D->NbNodes());
  EXPECT_TRUE(
    theCopiedPolygon2D->Nodes().Value(2).IsEqual(theSourcePolygon2D->Nodes().Value(2), 0.0));

  EXPECT_EQ(theCopiedPolygonOnTri->NbNodes(), theSourcePolygonOnTri->NbNodes());
  EXPECT_EQ(theCopiedPolygonOnTri->Node(2), theSourcePolygonOnTri->Node(2));
}

void expectMeshHandlesShared(const occ::handle<Poly_Triangulation>&          theSharedTriangulation,
                             const occ::handle<Poly_Polygon3D>&              theSharedPolygon3D,
                             const occ::handle<Poly_Polygon2D>&              theSharedPolygon2D,
                             const occ::handle<Poly_PolygonOnTriangulation>& theSharedPolygonOnTri,
                             const occ::handle<Poly_Triangulation>&          theSourceTriangulation,
                             const occ::handle<Poly_Polygon3D>&              theSourcePolygon3D,
                             const occ::handle<Poly_Polygon2D>&              theSourcePolygon2D,
                             const occ::handle<Poly_PolygonOnTriangulation>& theSourcePolygonOnTri)
{
  ASSERT_FALSE(theSharedTriangulation.IsNull());
  ASSERT_FALSE(theSharedPolygon3D.IsNull());
  ASSERT_FALSE(theSharedPolygon2D.IsNull());
  ASSERT_FALSE(theSharedPolygonOnTri.IsNull());

  EXPECT_EQ(theSharedTriangulation.get(), theSourceTriangulation.get());
  EXPECT_EQ(theSharedPolygon3D.get(), theSourcePolygon3D.get());
  EXPECT_EQ(theSharedPolygon2D.get(), theSourcePolygon2D.get());
  EXPECT_EQ(theSharedPolygonOnTri.get(), theSourcePolygonOnTri.get());
}

} // namespace

TEST(BRepGraph_CopyTest, CopyBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Share,
                                      BRepGraph_Copy::MeshPolicy::Share));
  ASSERT_FALSE(aCopyGraph.IsEmpty());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  // Verify reconstructed shape has correct face count.
  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aCopy.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraph_CopyTest, CopyEmptyGraph_Succeeds)
{
  BRepGraph aSource;
  BRepGraph aTarget;

  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget));
  EXPECT_TRUE(aTarget.IsEmpty());
}

TEST(BRepGraph_CopyTest, CopyEmptyTarget_PreservesShapeAndOriginalBindings)
{
  const TopoDS_Shape aSourceShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  BRepGraph          aSource;
  aSource.Clear();
  ASSERT_TRUE(aSource.Shapes().Add(aSourceShape).IsOk());

  const BRepGraph_NodeId aSourceNode = aSource.Shapes().FindNode(aSourceShape);
  ASSERT_TRUE(aSourceNode.IsValid());
  ASSERT_TRUE(aSource.Shapes().HasOriginal(aSourceNode));
  const TopoDS_Shape anOriginal = aSource.Shapes().Original(aSourceNode);
  ASSERT_FALSE(anOriginal.IsNull());

  BRepGraph aTarget;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget));
  EXPECT_EQ(aTarget.Shapes().FindNode(aSourceShape), aSourceNode);
  EXPECT_TRUE(aTarget.Shapes().HasOriginal(aSourceNode));
  EXPECT_TRUE(aTarget.Shapes().Original(aSourceNode).IsSame(anOriginal));
}

TEST(BRepGraph_CopyTest, PerformIntoNonEmptyTargetAppendsOnlyCopiedRoots)
{
  BRepGraph aTarget;
  aTarget.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aTargetBuild =
    aTarget.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aTarget.IsEmpty());
  ASSERT_EQ(aTarget.RootProductIds().Size(), 1u);
  const BRepGraph_ProductId anExistingRoot = aTarget.RootProductIds().Value(0);

  BRepGraph aSource;
  aSource.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aSourceBuild =
    aSource.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  ASSERT_FALSE(aSource.IsEmpty());
  ASSERT_EQ(aSource.RootProductIds().Size(), 1u);

  const uint32_t aFirstNewProduct = aTarget.Topo().Products().Nb();
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget, BRepGraph_Copy::GeomPolicy::Copy));
  ASSERT_EQ(aTarget.RootProductIds().Size(), 2u);
  EXPECT_EQ(aTarget.RootProductIds().Value(0), anExistingRoot);
  EXPECT_GE(aTarget.RootProductIds().Value(1).Index, aFirstNewProduct);
  EXPECT_NE(aTarget.RootProductIds().Value(1), anExistingRoot);

  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget, BRepGraph_Copy::GeomPolicy::Copy));
  ASSERT_EQ(aTarget.RootProductIds().Size(), 3u);
  EXPECT_EQ(aTarget.RootProductIds().Value(0), anExistingRoot);
  EXPECT_NE(aTarget.RootProductIds().Value(1), aTarget.RootProductIds().Value(2));
}

TEST(BRepGraph_CopyTest, ConcurrentCopiesFromSameSourceAreReadSafe)
{
  constexpr int THE_NB_THREADS    = 8;
  constexpr int THE_NB_ITERATIONS = 24;

  BRepGraph aSource;
  aSource.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildResult =
    aSource.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aSource.IsEmpty());

  std::atomic<int>         aReadyCount{0};
  std::atomic<bool>        toStart{false};
  std::atomic<int>         aFailureCount{0};
  std::vector<std::thread> aThreads;
  aThreads.reserve(static_cast<size_t>(THE_NB_THREADS));

  for (int aThreadIdx = 0; aThreadIdx < THE_NB_THREADS; ++aThreadIdx)
  {
    aThreads.emplace_back([&]() {
      aReadyCount.fetch_add(1);
      while (!toStart.load())
      {
        std::this_thread::yield();
      }

      for (int anIteration = 0; anIteration < THE_NB_ITERATIONS; ++anIteration)
      {
        BRepGraph aCopy;
        if (!BRepGraph_Copy::Perform(aSource, aCopy, BRepGraph_Copy::GeomPolicy::Copy)
            || aCopy.IsEmpty() || aCopy.Topo().Faces().Nb() != aSource.Topo().Faces().Nb()
            || aCopy.Topo().Edges().Nb() != aSource.Topo().Edges().Nb()
            || !BRepGraph_Validate::Perform(aCopy).IsValid())
        {
          aFailureCount.fetch_add(1);
        }
      }
    });
  }

  while (aReadyCount.load() != THE_NB_THREADS)
  {
    std::this_thread::yield();
  }
  toStart.store(true);

  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  EXPECT_EQ(aFailureCount.load(), 0);
}

TEST(BRepGraph_CopyTest, CopyGraph_RemovedOccurrenceActiveCountsMatchFlags)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& anOccurrenceRefs =
    aGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(anOccurrenceRefs.Size(), 1);
  const BRepGraph_OccurrenceRefId anOccRefId = anOccurrenceRefs.Value(0);

  aGraph.Editor().Gen().RemoveSubgraph(anOccId);
  ASSERT_TRUE(anOccId.IsRemoved(aGraph));
  ASSERT_TRUE(anOccRefId.IsRemoved(aGraph));

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Share,
                                      BRepGraph_Copy::MeshPolicy::Share));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  EXPECT_TRUE(anOccId.IsRemoved(aCopyGraph));
  EXPECT_TRUE(anOccRefId.IsRemoved(aCopyGraph));
  EXPECT_EQ(aCopyGraph.Topo().Occurrences().NbActive(),
            countActive<BRepGraph_OccurrenceId>(
              aCopyGraph,
              static_cast<uint32_t>(aCopyGraph.Topo().Occurrences().Nb())));
  EXPECT_EQ(aCopyGraph.Refs().Occurrences().NbActive(),
            countActive<BRepGraph_OccurrenceRefId>(
              aCopyGraph,
              static_cast<uint32_t>(aCopyGraph.Refs().Occurrences().Nb())));
}

TEST(BRepGraph_CopyTest, CopyBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aCopy.IsNull());

  // Sum face areas since result may be a compound.
  double aCopyArea = 0.0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aCopyArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_CopyTest, CopyBox_GeometryIsIndependent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  // Deep copy: surface handles must be different objects.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  ASSERT_GT(aCopyGraph.Topo().Faces().Nb(), 0);
  EXPECT_NE(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get());
}

TEST(BRepGraph_CopyTest, CopyGraph_DropsRuntimeFaceCacheMesh)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTriangulation);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const BRepGraph_CacheMesh::FaceMeshEntry* aCopiedEntry =
    aCopyGraph.Mesh().Cache().Faces().Entry(aFaceId);
  EXPECT_EQ(aCopiedEntry, nullptr);
}

TEST(BRepGraph_CopyTest, CopyGraph_CanCopyFreshRuntimeFaceCacheMesh)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  constexpr size_t                       THE_RECIPE_HASH = 0x9a17u;
  const occ::handle<BRepGraph_CacheMesh> aSourceCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheMesh>();
  aSourceCache->RegisterDriver(BRepGraph_CacheMesh::DefaultDisplaySlot,
                               new CopyTestMeshDriver(THE_RECIPE_HASH));
  const occ::handle<BRepGraph_CacheMesh::Driver> aSourceDriver =
    aSourceCache->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot);

  occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTriangulation);
  ASSERT_NE(aGraph.Mesh().Cache().Faces().Entry(aFaceId), nullptr);
  ASSERT_FALSE(aSourceCache->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot).IsNull());

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Copy,
                                      BRepGraph_Copy::MeshPolicy::Copy,
                                      BRepGraph_Copy::CachePolicy::CopyFresh));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const BRepGraph_CacheMesh::FaceMeshEntry* aCopiedEntry =
    aCopyGraph.Mesh().Cache().Faces().Entry(aFaceId);
  ASSERT_NE(aCopiedEntry, nullptr);
  EXPECT_EQ(aCopiedEntry->Triangulation.get(), aTriangulation.get());

  const occ::handle<BRepGraph_CacheMesh> aCopiedCache =
    aCopyGraph.CacheRegistry().Find<BRepGraph_CacheMesh>();
  ASSERT_FALSE(aCopiedCache.IsNull());
  const occ::handle<BRepGraph_CacheMesh::Driver> aCopiedDriver =
    aCopiedCache->DriverOf(BRepGraph_CacheMesh::DefaultDisplaySlot);
  ASSERT_FALSE(aCopiedDriver.IsNull());
  EXPECT_NE(aCopiedDriver.get(), aSourceDriver.get());
  EXPECT_EQ(aCopiedDriver->RecipeHash(), THE_RECIPE_HASH);
  const BRepGraph_CacheMesh::SlotState aCopiedState =
    aCopiedCache->State(BRepGraph_CacheMesh::DefaultDisplaySlot);
  EXPECT_TRUE(aCopiedState.HasDriver);
  EXPECT_EQ(aCopiedState.RecipeHash, THE_RECIPE_HASH);
}

TEST(BRepGraph_CopyTest, CopyGraph_DropsRuntimeEdgeAndCoEdgeCacheMesh)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Polygon3D>     aPolygon3D     = new Poly_Polygon3D(2, false);
  occ::handle<Poly_Polygon2D>     aPolygon2D     = new Poly_Polygon2D(2);
  occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);

  aGraph.Mesh().Editor().Edges().SetCachedPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Mesh().Editor().CoEdges().SetCachedPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Mesh().Editor().CoEdges().AppendCachedPolygonOnTri(aCoEdgeId, aPolygonOnTri);
  ASSERT_NE(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr);
  ASSERT_TRUE(aGraph.Mesh().Cache().CoEdges().Has(aCoEdgeId));

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const BRepGraph_CacheMesh::EdgeMeshEntry* aCopiedEdge =
    aCopyGraph.Mesh().Cache().Edges().Entry(anEdgeId);
  EXPECT_EQ(aCopiedEdge, nullptr);

  EXPECT_FALSE(aCopyGraph.Mesh().Cache().CoEdges().Has(aCoEdgeId));
}

TEST(BRepGraph_CopyTest, CopyNode_CopiesPersistentMeshAndDropsCache)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation>          aTriangulation = new Poly_Triangulation(3, 1, false);
  occ::handle<Poly_Polygon3D>              aPolygon3D     = new Poly_Polygon3D(2, false);
  occ::handle<Poly_Polygon2D>              aPolygon2D     = new Poly_Polygon2D(2);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  initMeshHandles(aTriangulation, aPolygon3D, aPolygon2D, aPolygonOnTri);

  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTriangulation);
  aGraph.Mesh().Editor().Edges().SetCachedPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Mesh().Editor().CoEdges().SetCachedPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Mesh().Editor().CoEdges().AppendCachedPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  BRepGraph                               aCopyGraph;
  [[maybe_unused]] const BRepGraph_NodeId aCopiedNodeId =
    BRepGraph_Copy::CopyNode(aGraph, aCopyGraph, BRepGraph_NodeId(aFaceId));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const BRepGraph_FaceId   aCopyFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCopyCoEdgeId = firstCoEdgeOfFace(aCopyGraph, aCopyFaceId);
  ASSERT_TRUE(aCopyCoEdgeId.IsValid(aCopyGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId aCopyEdgeId =
    aCopyGraph.Topo().CoEdges().Definition(aCopyCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(aCopyEdgeId.IsValid(aCopyGraph.Topo().Edges().Nb()));

  const BRepGraph_CacheMesh::FaceMeshEntry* aCopiedFace =
    aCopyGraph.Mesh().Cache().Faces().Entry(aCopyFaceId);
  EXPECT_EQ(aCopiedFace, nullptr);

  const BRepGraph_CacheMesh::EdgeMeshEntry* aCopiedEdge =
    aCopyGraph.Mesh().Cache().Edges().Entry(aCopyEdgeId);
  EXPECT_EQ(aCopiedEdge, nullptr);

  EXPECT_FALSE(aCopyGraph.Mesh().Cache().CoEdges().Has(aCopyCoEdgeId));

  expectMeshHandlesCopied(
    aCopyGraph.Mesh().Persistent().Faces().Triangulation(aCopyFaceId),
    aCopyGraph.Mesh().Persistent().Edges().Polygon3D(aCopyEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(aCopyCoEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCopyCoEdgeId),
    aTriangulation,
    aPolygon3D,
    aPolygon2D,
    aPolygonOnTri);
}

TEST(BRepGraph_CopyTest, CopyGraph_CopiesPersistentMeshHandles)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation>          aTriangulation = new Poly_Triangulation(3, 1, false);
  occ::handle<Poly_Polygon3D>              aPolygon3D     = new Poly_Polygon3D(2, false);
  occ::handle<Poly_Polygon2D>              aPolygon2D     = new Poly_Polygon2D(2);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  initMeshHandles(aTriangulation, aPolygon3D, aPolygon2D, aPolygonOnTri);

  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Copy,
                                      BRepGraph_Copy::MeshPolicy::Copy));

  expectMeshHandlesCopied(
    aCopyGraph.Mesh().Persistent().Faces().Triangulation(aFaceId),
    aCopyGraph.Mesh().Persistent().Edges().Polygon3D(anEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(aCoEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId),
    aTriangulation,
    aPolygon3D,
    aPolygon2D,
    aPolygonOnTri);
}

TEST(BRepGraph_CopyTest, CopyGraph_SharesPersistentMeshHandles)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation>          aTriangulation = new Poly_Triangulation(3, 1, false);
  occ::handle<Poly_Polygon3D>              aPolygon3D     = new Poly_Polygon3D(2, false);
  occ::handle<Poly_Polygon2D>              aPolygon2D     = new Poly_Polygon2D(2);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  initMeshHandles(aTriangulation, aPolygon3D, aPolygon2D, aPolygonOnTri);

  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Copy,
                                      BRepGraph_Copy::MeshPolicy::Share));

  expectMeshHandlesShared(
    aCopyGraph.Mesh().Persistent().Faces().Triangulation(aFaceId),
    aCopyGraph.Mesh().Persistent().Edges().Polygon3D(anEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(aCoEdgeId),
    aCopyGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId),
    aTriangulation,
    aPolygon3D,
    aPolygon2D,
    aPolygonOnTri);
}

TEST(BRepGraph_CopyTest, CopyGraph_NonEmptyTargetUsesSetterPathForPersistentMesh)
{
  BRepGraph aSource;
  aSource.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aSourceBuild =
    aSource.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aSource.IsEmpty());

  BRepGraph aTarget;
  aTarget.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aTargetBuild =
    aTarget.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  ASSERT_FALSE(aTarget.IsEmpty());

  const BRepGraph_FaceId   aSourceFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aSourceCoEdgeId = firstCoEdgeOfFace(aSource, aSourceFaceId);
  ASSERT_TRUE(aSourceCoEdgeId.IsValid(aSource.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId aSourceEdgeId =
    aSource.Topo().CoEdges().Definition(aSourceCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(aSourceEdgeId.IsValid(aSource.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation>          aTriangulation = new Poly_Triangulation(3, 1, false);
  occ::handle<Poly_Polygon3D>              aPolygon3D     = new Poly_Polygon3D(2, false);
  occ::handle<Poly_Polygon2D>              aPolygon2D     = new Poly_Polygon2D(2);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  initMeshHandles(aTriangulation, aPolygon3D, aPolygon2D, aPolygonOnTri);

  aSource.Editor().Faces().SetPersistentTriangulation(aSourceFaceId, aTriangulation);
  aSource.Editor().Edges().SetPersistentPolygon3D(aSourceEdgeId, aPolygon3D);
  aSource.Editor().CoEdges().SetPersistentPolygon2D(aSourceCoEdgeId, aPolygon2D);
  aSource.Editor().CoEdges().SetPersistentPolygonOnTri(aSourceCoEdgeId, aPolygonOnTri);

  const uint32_t aTargetFaceCount   = aTarget.Topo().Faces().Nb();
  const uint32_t aTargetEdgeCount   = aTarget.Topo().Edges().Nb();
  const uint32_t aTargetCoEdgeCount = aTarget.Topo().CoEdges().Nb();

  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource,
                                      aTarget,
                                      BRepGraph_Copy::GeomPolicy::Copy,
                                      BRepGraph_Copy::MeshPolicy::Copy));

  const BRepGraph_FaceId   aCopiedFaceId(aTargetFaceCount + aSourceFaceId.Index);
  const BRepGraph_CoEdgeId aCopiedCoEdgeId(aTargetCoEdgeCount + aSourceCoEdgeId.Index);
  const BRepGraph_EdgeId   aCopiedEdgeId(aTargetEdgeCount + aSourceEdgeId.Index);
  ASSERT_TRUE(aCopiedFaceId.IsValid(aTarget.Topo().Faces().Nb()));
  ASSERT_TRUE(aCopiedEdgeId.IsValid(aTarget.Topo().Edges().Nb()));
  ASSERT_TRUE(aCopiedCoEdgeId.IsValid(aTarget.Topo().CoEdges().Nb()));

  expectMeshHandlesCopied(
    aTarget.Mesh().Persistent().Faces().Triangulation(aCopiedFaceId),
    aTarget.Mesh().Persistent().Edges().Polygon3D(aCopiedEdgeId),
    aTarget.Mesh().Persistent().CoEdges().PolygonOnSurface(aCopiedCoEdgeId),
    aTarget.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCopiedCoEdgeId),
    aTriangulation,
    aPolygon3D,
    aPolygon2D,
    aPolygonOnTri);
}

TEST(BRepGraph_CopyTest, CopyNode_DoesNotReviveRemovedPersistentMeshReps)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);
  occ::handle<Poly_Polygon3D>     aPolygon3D     = new Poly_Polygon3D(2, false);
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  ASSERT_TRUE(aGraph.Mesh().Persistent().Faces().Has(aFaceId));
  ASSERT_TRUE(aGraph.Mesh().Persistent().Edges().Has(anEdgeId));

  aGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);
  aGraph.Editor().Edges().ClearPersistentPolygon3D(anEdgeId);
  ASSERT_FALSE(aGraph.Mesh().Persistent().Faces().Has(aFaceId));
  ASSERT_FALSE(aGraph.Mesh().Persistent().Edges().Has(anEdgeId));

  BRepGraph                               aCopyGraph;
  [[maybe_unused]] const BRepGraph_NodeId aCopiedNodeId =
    BRepGraph_Copy::CopyNode(aGraph, aCopyGraph, BRepGraph_NodeId(aFaceId));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const BRepGraph_FaceId   aCopyFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCopyCoEdgeId = firstCoEdgeOfFace(aCopyGraph, aCopyFaceId);
  ASSERT_TRUE(aCopyCoEdgeId.IsValid(aCopyGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId aCopyEdgeId =
    aCopyGraph.Topo().CoEdges().Definition(aCopyCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(aCopyEdgeId.IsValid(aCopyGraph.Topo().Edges().Nb()));

  EXPECT_FALSE(aCopyGraph.Mesh().Persistent().Faces().Has(aCopyFaceId));
  EXPECT_FALSE(aCopyGraph.Mesh().Persistent().Edges().Has(aCopyEdgeId));
}

TEST(BRepGraph_CopyTest, CopyBox_SharedGeometry)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // theCopyGeom = false: geometry is shared.
  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Share);
  ASSERT_FALSE(aCopyGraph.IsEmpty());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);

  // Light copy: surface handles must be the same objects.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  ASSERT_GT(aCopyGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get());
}

TEST(BRepGraph_CopyTest, CopyGraph_DropsGeometryHandles)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId));
  ASSERT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId));
  ASSERT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeId));

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Drop,
                                      BRepGraph_Copy::MeshPolicy::Copy));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  EXPECT_FALSE(BRepGraph_Tool::Face::HasSurface(aCopyGraph, aFaceId));
  EXPECT_FALSE(BRepGraph_Tool::Edge::HasCurve(aCopyGraph, anEdgeId));
  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(aCopyGraph, aCoEdgeId));
  EXPECT_EQ(aCopyGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId,
            BRepGraph_FaceSurfaceRepId());
  EXPECT_EQ(aCopyGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId,
            BRepGraph_EdgeCurve3DRepId());
  EXPECT_EQ(aCopyGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId,
            BRepGraph_CoEdgeCurve2DRepId());
}

TEST(BRepGraph_CopyTest, CopyBox_DoesNotCopyTransientCacheServices)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId            aFaceId(0);
  occ::handle<CopyTestCacheService> aCache = aGraph.CacheRegistry().Ensure<CopyTestCacheService>();
  aCache->Set(aFaceId);
  ASSERT_TRUE(aCache->Has(aFaceId));

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  EXPECT_TRUE(aCopyGraph.CacheRegistry().Find<CopyTestCacheService>().IsNull());
}

TEST(BRepGraph_CopyTest, CopyGraph_RemapsSupplementAttachmentsAndUids)
{
  BRepGraph aGraph;
  aGraph.Clear();
  const occ::handle<BRepGraphSupInc_TopologyStore> aRegisteredStore =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_TopologyStore>();
  ASSERT_FALSE(aRegisteredStore.IsNull());
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildResSupplement =
    aGraph.Shapes().Add(makeEdgeWithInternalVertex());
  ASSERT_FALSE(aGraph.IsEmpty());

  const occ::handle<BRepGraphSupInc_TopologyStore> aSrcStore =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aSrcStore.IsNull());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_EdgeId                                      aEdgeId = BRepGraph_EdgeId::Start();
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aSrcAttached =
    aGraph.Supplements().Attachments(aEdgeId);
  ASSERT_EQ(aSrcAttached.Size(), 1);
  const BRepGraphSupInc_TopologyId aSourceAttachmentId = aSrcAttached.First();
  const BRepGraphSupInc_ItemUID    aUid                = aSrcStore->ItemUID(aSourceAttachmentId);

  BRepGraph aCopyGraph;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aGraph,
                                      aCopyGraph,
                                      BRepGraph_Copy::GeomPolicy::Share,
                                      BRepGraph_Copy::MeshPolicy::Share));
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const occ::handle<BRepGraphSupInc_TopologyStore> aDstStore =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aCopyGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aDstStore.IsNull());

  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aDstAttached =
    aCopyGraph.Supplements().Attachments(aEdgeId);
  ASSERT_EQ(aDstAttached.Size(), 1);
  EXPECT_NE(aDstStore->ItemUID(aDstAttached.First()), aUid);

  const BRepGraphSupInc::TopologyDef* aDstEntry =
    aCopyGraph.Supplements().Attachment(aDstAttached.First());
  ASSERT_NE(aDstEntry, nullptr);
  EXPECT_EQ(aDstEntry->Owner, BRepGraph_NodeId(aEdgeId));
  EXPECT_EQ(aDstEntry->Kind, BRepGraphSupInc::TopologyAttachmentKind::EdgeInternalVertex);
  const TopoDS_Shape* anAttachmentShape = aCopyGraph.Supplements().AttachmentShape(*aDstEntry);
  ASSERT_NE(anAttachmentShape, nullptr);
  EXPECT_EQ(anAttachmentShape->ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(anAttachmentShape->Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRecon = aCopyGraph.Shapes().Reconstruct(aEdgeId);
  ASSERT_FALSE(aRecon.IsNull());

  int aFoundInternal = 0;
  for (TopoDS_Iterator aVIt(aRecon, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() == TopAbs_VERTEX && aVIt.Value().Orientation() == TopAbs_INTERNAL)
    {
      ++aFoundInternal;
    }
  }
  EXPECT_EQ(aFoundInternal, 1);
}

TEST(BRepGraph_CopyTest, CopyNode_CopiesMappedTopologyAttachmentsAndReconstructsShape)
{
  BRepGraph aSource;
  aSource.Clear();
  const occ::handle<BRepGraphSupInc_TopologyStore> aSourceStore =
    aSource.Supplements().EnsureStore<BRepGraphSupInc_TopologyStore>();
  ASSERT_FALSE(aSourceStore.IsNull());
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuild =
    aSource.Shapes().Add(makeEdgeWithInternalVertex());
  const BRepGraph_EdgeId aSourceEdge = BRepGraph_EdgeId::Start();
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aSourceAttachments =
    aSource.Supplements().Attachments(aSourceEdge);
  ASSERT_EQ(aSourceAttachments.Size(), 1u);
  const BRepGraphSupInc_ItemUID aSourceUID = aSourceStore->ItemUID(aSourceAttachments.First());

  BRepGraph              aTarget;
  const BRepGraph_NodeId aCopiedNode =
    BRepGraph_Copy::CopyNode(aSource, aTarget, BRepGraph_NodeId(aSourceEdge));
  ASSERT_TRUE(aCopiedNode.IsValid());
  const BRepGraph_EdgeId aCopiedEdge = BRepGraph_EdgeId::FromNodeId(aCopiedNode);
  const occ::handle<BRepGraphSupInc_TopologyStore> aTargetStore =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aTarget.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aTargetStore.IsNull());
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aTargetAttachments =
    aTarget.Supplements().Attachments(aCopiedEdge);
  ASSERT_EQ(aTargetAttachments.Size(), 1u);
  EXPECT_NE(aTargetStore->ItemUID(aTargetAttachments.First()), aSourceUID);
  const TopoDS_Shape aTargetShape = aTarget.Shapes().Reconstruct(aCopiedEdge);
  ASSERT_FALSE(aTargetShape.IsNull());
  int aTargetInternalVertices = 0;
  for (TopoDS_Iterator anIterator(aTargetShape, false); anIterator.More(); anIterator.Next())
  {
    if (anIterator.Value().ShapeType() == TopAbs_VERTEX
        && anIterator.Value().Orientation() == TopAbs_INTERNAL)
    {
      ++aTargetInternalVertices;
    }
  }
  EXPECT_EQ(aTargetInternalVertices, 1);

  const BRepGraph_NodeId aSameGraphNode =
    BRepGraph_Copy::CopyNode(aSource, aSource, BRepGraph_NodeId(aSourceEdge));
  ASSERT_TRUE(aSameGraphNode.IsValid());
  const BRepGraph_EdgeId aSameGraphEdge = BRepGraph_EdgeId::FromNodeId(aSameGraphNode);
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aSameGraphAttachments =
    aSource.Supplements().Attachments(aSameGraphEdge);
  ASSERT_EQ(aSameGraphAttachments.Size(), 1u);
  EXPECT_NE(aSourceStore->ItemUID(aSameGraphAttachments.First()), aSourceUID);
  const BRepGraphSupInc::TopologyDef* aSameGraphAttachment =
    aSource.Supplements().Attachment(aSameGraphAttachments.First());
  ASSERT_NE(aSameGraphAttachment, nullptr);
  EXPECT_EQ(aSameGraphAttachment->Owner, aSameGraphNode);
  EXPECT_EQ(aSameGraphAttachment->Kind,
            BRepGraphSupInc::TopologyAttachmentKind::EdgeInternalVertex);
  EXPECT_EQ(aSameGraphAttachment->Shape.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(aSameGraphAttachment->Shape.Orientation(), TopAbs_INTERNAL);
  const TopoDS_Shape aSameGraphShape = aSource.Shapes().Reconstruct(aSameGraphEdge);
  ASSERT_FALSE(aSameGraphShape.IsNull());
  int aSameGraphInternalVertices = 0;
  for (TopoDS_Iterator anIterator(aSameGraphShape, false); anIterator.More(); anIterator.Next())
  {
    if (anIterator.Value().ShapeType() == TopAbs_VERTEX
        && anIterator.Value().Orientation() == TopAbs_INTERNAL)
    {
      ++aSameGraphInternalVertices;
    }
  }
  EXPECT_EQ(aSameGraphInternalVertices, 1);
}

TEST(BRepGraph_CopyTest, CopyNode_ExcludesAttachmentsOfUnmappedOwners)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aFirstOwner = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraph_VertexId aSecondOwner =
    aSource.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  BRep_Builder  aBuilder;
  TopoDS_Vertex aFirstAttachmentShape;
  TopoDS_Vertex aSecondAttachmentShape;
  aBuilder.MakeVertex(aFirstAttachmentShape, gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  aBuilder.MakeVertex(aSecondAttachmentShape, gp_Pnt(3.0, 0.0, 0.0), 1.0e-7);
  ASSERT_TRUE(aSource.Supplements()
                .Attach(aFirstOwner,
                        BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape,
                        aFirstAttachmentShape)
                .IsValid());
  ASSERT_TRUE(aSource.Supplements()
                .Attach(aSecondOwner,
                        BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape,
                        aSecondAttachmentShape)
                .IsValid());

  BRepGraph              aTarget;
  const BRepGraph_NodeId aCopiedNode =
    BRepGraph_Copy::CopyNode(aSource, aTarget, BRepGraph_NodeId(aFirstOwner));
  ASSERT_TRUE(aCopiedNode.IsValid());
  const BRepGraph_VertexId aCopiedOwner = BRepGraph_VertexId::FromNodeId(aCopiedNode);
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& aCopiedAttachments =
    aTarget.Supplements().Attachments(aCopiedOwner);
  ASSERT_EQ(aCopiedAttachments.Size(), 1u);
  const BRepGraphSupInc::TopologyDef* anAttachment =
    aTarget.Supplements().Attachment(aCopiedAttachments.First());
  ASSERT_NE(anAttachment, nullptr);
  EXPECT_EQ(anAttachment->Owner, aCopiedNode);
  EXPECT_TRUE(aTarget.Supplements().Attachments(aSecondOwner).IsEmpty());
}

TEST(BRepGraph_CopyTest, CopyGraph_CopiesHistoryThroughLayerRemap)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 2u);

  const BRepGraph_FaceId aOriginalFace    = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId aReplacementFace = BRepGraph_FaceId(1);
  const BRepGraph_UID    anOriginalUID    = aGraph.UIDs().Of(aOriginalFace);
  const BRepGraph_UID    aReplacementUID  = aGraph.UIDs().Of(aReplacementFace);
  ASSERT_TRUE(anOriginalUID.IsValid());
  ASSERT_TRUE(aReplacementUID.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(aReplacementFace);
  aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("CopyHistory",
                                                                  aOriginalFace,
                                                                  aReplacements.ToArray1());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  const occ::handle<BRepGraph_LayerHistory> aCopyHistory =
    aCopyGraph.LayerRegistry().FindLayer<BRepGraph_LayerHistory>();
  ASSERT_FALSE(aCopyHistory.IsNull());

  const BRepGraph_NodeId aCopyOriginal    = aCopyGraph.UIDs().NodeIdFrom(anOriginalUID);
  const BRepGraph_NodeId aCopyReplacement = aCopyGraph.UIDs().NodeIdFrom(aReplacementUID);
  ASSERT_TRUE(aCopyOriginal.IsValid());
  ASSERT_TRUE(aCopyReplacement.IsValid());

  const NCollection_LinearVector<BRepGraph_NodeId>* aModified =
    aCopyHistory->FindModified(aCopyOriginal);
  ASSERT_NE(aModified, nullptr);
  ASSERT_EQ(aModified->Size(), size_t(1));
  EXPECT_EQ(aModified->Value(0), aCopyReplacement);

  const BRepGraph_ItemUID aCopyOriginalUID = aCopyGraph.UIDs().Of(BRepGraph_ItemId(aCopyOriginal));
  const NCollection_LinearVector<BRepGraph_ItemUID>* anItemModified =
    aCopyHistory->FindModified(aCopyOriginalUID);
  ASSERT_NE(anItemModified, nullptr);
  ASSERT_EQ(anItemModified->Size(), size_t(1));
  EXPECT_EQ(anItemModified->Value(0), aCopyGraph.UIDs().Of(BRepGraph_ItemId(aCopyReplacement)));
}

TEST(BRepGraph_CopyTest, CopyGraph_PreservesRefUIDs)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));
  const BRepGraphInc::FaceRelations& aFaceRelations = aGraph.Topo().Faces().Relations(aFaceId);
  ASSERT_FALSE(aFaceRelations.WireRefIds.IsEmpty());
  const BRepGraph_WireRefId aWireRefId  = aFaceRelations.WireRefIds.First();
  const BRepGraph_RefUID    aWireRefUID = aGraph.UIDs().Of(aWireRefId);
  ASSERT_TRUE(aWireRefUID.IsValid());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  ASSERT_TRUE(aWireRefId.IsValid(aCopyGraph.Refs().Wires().Nb()));
  EXPECT_EQ(aCopyGraph.UIDs().Of(aWireRefId), aWireRefUID);

  const BRepGraph_RefId aResolvedRefId = aCopyGraph.UIDs().RefIdFrom(aWireRefUID);
  EXPECT_TRUE(aResolvedRefId.IsValid());
  EXPECT_EQ(aResolvedRefId, BRepGraph_RefId(aWireRefId));

  const BRepGraph_ItemStamp aStamp = aCopyGraph.UIDs().StampOf(aWireRefId);
  EXPECT_TRUE(aStamp.IsValid());
  EXPECT_EQ(aStamp.ItemUID(), BRepGraph_ItemUID::Reference(aWireRefUID.Kind, aWireRefUID.Counter));
}

TEST(BRepGraph_CopyTest, CopyGraph_PreservesDeletedHistory)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  occ::handle<BRepGraph_LayerHistory> aHistory =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>();
  const BRepGraph_FaceId  aDeletedFace    = BRepGraph_FaceId::Start();
  const BRepGraph_ItemUID aDeletedFaceUid = aGraph.UIDs().Of(BRepGraph_ItemId(aDeletedFace));
  ASSERT_TRUE(aDeletedFaceUid.IsValid());

  aGraph.Editor().Gen().RemoveNode(aDeletedFace);
  ASSERT_TRUE(aDeletedFace.IsRemoved(aGraph));
  ASSERT_TRUE(aHistory->IsDeleted(aDeletedFace));
  ASSERT_TRUE(aHistory->IsDeleted(aDeletedFaceUid));
  ASSERT_EQ(aHistory->NbRecords(), size_t(1));

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  occ::handle<BRepGraph_LayerHistory> aCopyHistory =
    aCopyGraph.LayerRegistry().FindLayer<BRepGraph_LayerHistory>();
  ASSERT_FALSE(aCopyHistory.IsNull());

  EXPECT_TRUE(aCopyHistory->IsDeleted(aDeletedFace));
  EXPECT_TRUE(aCopyHistory->IsDeleted(aDeletedFaceUid));
  EXPECT_TRUE(aCopyHistory->HasKnownInput(aDeletedFaceUid));
  EXPECT_TRUE(aCopyHistory->DeletedItemUids().Contains(aDeletedFaceUid));
  EXPECT_EQ(aCopyHistory->NbRecords(), aHistory->NbRecords());
}

TEST(BRepGraph_CopyTest, CopyCylinder_FaceCount)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 = aGraph.Shapes().Add(aCyl);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_CopyTest, CopySingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const BRepGraph_FaceId                  aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId                  aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);
  BRepGraph                               aCopyGraph;
  [[maybe_unused]] const BRepGraph_NodeId aCopiedNodeId =
    BRepGraph_Copy::CopyNode(aGraph, aCopyGraph, aFaceNode);
  ASSERT_FALSE(aCopyGraph.IsEmpty());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 1);

  // A box face has 1 wire with 4 edges and 4 vertices.
  EXPECT_GT(aCopyGraph.Topo().Vertices().Nb(), 0);
  EXPECT_GT(aCopyGraph.Topo().Edges().Nb(), 0);
  EXPECT_GT(aCopyGraph.Topo().Wires().Nb(), 0);

  // No shells/solids in a single-face sub-graph.
  EXPECT_EQ(aCopyGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aCopyGraph.Topo().Solids().Nb(), 0);

  TopoDS_Shape aCopiedFace = aCopyGraph.Shapes().Reconstruct(BRepGraph_FaceId::Start());
  ASSERT_FALSE(aCopiedFace.IsNull());
  EXPECT_EQ(aCopiedFace.ShapeType(), TopAbs_FACE);

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aCopiedFace, aProps);
  EXPECT_GT(std::abs(aProps.Mass()), 1.0);
}

TEST(BRepGraph_CopyTest, CopyNode_SelfCopyDuplicatesFaceSubgraph)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t         aFaceCountBefore   = aGraph.Topo().Faces().Nb();
  const uint32_t         aWireCountBefore   = aGraph.Topo().Wires().Nb();
  const uint32_t         anEdgeCountBefore  = aGraph.Topo().Edges().Nb();
  const uint32_t         aVertexCountBefore = aGraph.Topo().Vertices().Nb();
  const BRepGraph_FaceId aSourceFaceId      = BRepGraph_FaceId::Start();

  const BRepGraph_NodeId aCopiedNodeId =
    BRepGraph_Copy::CopyNode(aGraph, aGraph, BRepGraph_NodeId(aSourceFaceId));
  ASSERT_TRUE(aCopiedNodeId.IsValid());
  ASSERT_EQ(aCopiedNodeId.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_NE(aCopiedNodeId, BRepGraph_NodeId(aSourceFaceId));

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aFaceCountBefore + 1);
  EXPECT_GT(aGraph.Topo().Wires().Nb(), aWireCountBefore);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), anEdgeCountBefore);
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), aVertexCountBefore);

  const BRepGraph_FaceId aCopiedFaceId = BRepGraph_FaceId::FromNodeId(aCopiedNodeId);
  const TopoDS_Shape     aCopiedFace   = aGraph.Shapes().Reconstruct(aCopiedFaceId);
  ASSERT_FALSE(aCopiedFace.IsNull());
  EXPECT_EQ(aCopiedFace.ShapeType(), TopAbs_FACE);
}

TEST(BRepGraph_CopyTest, CopyFacesOnly_Compound)
{
  // Build graph from individual faces (no shells/solids).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
    aBB.Add(aCompound, aCopier.Shape());
  }

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);
  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 0);
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 0);

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aCopyGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aCopyGraph.Topo().Shells().Nb(), 0);
}

// ============================================================
// SameParameter / SameRange round-trip
// ============================================================

TEST(BRepGraph_CopyTest, CopyBox_SameParameter_Preserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  // All edges in the copied graph must preserve SameParameter = true.
  const uint32_t aNbEdges = aCopyGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_TRUE(edgeSameParameter(aCopyGraph, anEdgeId))
      << "Copied edge " << anEdgeId.Index << " lost SameParameter flag";
    EXPECT_TRUE(edgeSameRange(aCopyGraph, anEdgeId))
      << "Copied edge " << anEdgeId.Index << " lost SameRange flag";
  }
}

// ============================================================
// Regularity (BRep_CurveOn2Surfaces) preserved via EncodeRegularity
// ============================================================

TEST(BRepGraph_CopyTest, FusedBoxes_Regularity_AreaPreserved)
{
  // Fuse two adjacent boxes to produce a shape with regularity edges.
  // Verify that the copy preserves area (geometry integrity).
  TopoDS_Shape     aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape     aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aFused, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aFused);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  // Verify node counts match.
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aCopyGraph.Topo().Edges().Nb(), aGraph.Topo().Edges().Nb());

  // Verify area is preserved by summing individual face areas.
  double         aCopyArea = 0.0;
  const uint32_t aNbFaces  = aCopyGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    TopoDS_Shape aFace = aCopyGraph.Shapes().Reconstruct(aFaceId);
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    aCopyArea += std::abs(aProps.Mass());
  }
  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

// ============================================================
// UID preservation
// ============================================================

TEST(BRepGraph_CopyTest, CopyBox_UIDsPreserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aCopyGraph;
  BRepGraph_Copy::Perform(aGraph, aCopyGraph, BRepGraph_Copy::GeomPolicy::Copy);
  ASSERT_FALSE(aCopyGraph.IsEmpty());

  // Helper to check UIDs for a given node kind.
  auto checkUIDs = [&](BRepGraph_NodeId::Kind theKind, uint32_t theCount, const char* theLabel) {
    for (uint32_t anIdx = 0; anIdx < theCount; ++anIdx)
    {
      BRepGraph_NodeId aNodeId(theKind, anIdx);
      BRepGraph_UID    anOrigUID = aGraph.UIDs().Of(aNodeId);
      BRepGraph_UID    aCopyUID  = aCopyGraph.UIDs().Of(aNodeId);
      EXPECT_EQ(aCopyUID.IsValid(), anOrigUID.IsValid())
        << "UID validity mismatch at " << theLabel << " " << anIdx;
      if (anOrigUID.IsValid())
      {
        EXPECT_EQ(aCopyUID, anOrigUID) << "UID mismatch at " << theLabel << " " << anIdx;
      }
    }
  };

  // Verify UIDs for all topology and geometry node types.
  checkUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Topo().Vertices().Nb(), "vertex");
  checkUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().Edges().Nb(), "edge");
  checkUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().Wires().Nb(), "wire");
  checkUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().Faces().Nb(), "face");
  checkUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().Shells().Nb(), "shell");
  checkUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb(), "solid");

  const BRepGraph_FaceSurfaceRepId aSurfaceRepId =
    aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId;
  ASSERT_TRUE(aSurfaceRepId.IsValid());
  const BRepGraph_FaceSurfaceRepId aCopySurfaceRepId =
    aCopyGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId;
  EXPECT_TRUE(aCopySurfaceRepId.IsValid());
}
