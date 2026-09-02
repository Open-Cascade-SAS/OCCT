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
#include <BRepGraph_CacheDerivedState.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_ParityOrientation.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>

#include <atomic>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

// ---------- New ownership and lookup helpers (query-surface-expansion) ----------

class BRepGraph_QuerySurfaceTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myBoxGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
      myBoxGraph.Shapes().Add(aBoxMaker.Shape());

    BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
    myCylGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
      myCylGraph.Shapes().Add(aCylMaker.Shape());
  }

  BRepGraph myBoxGraph;
  BRepGraph myCylGraph;
};

TEST_F(BRepGraph_QuerySurfaceTest, Face_NbWires_BoxFaceHasOneWire)
{
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(myBoxGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    const uint32_t aNb = BRepGraph_Tool::Face::NbWires(myBoxGraph, aFaceId);
    EXPECT_EQ(aNb, 1u) << "Box face " << aFaceId.Index << " should have exactly 1 wire";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_Bounds_BoxFaceHasFiniteBounds)
{
  const BRepGraph_FaceId aFaceId(0);
  double                 uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
  BRepGraph_Tool::Face::Bounds(myBoxGraph, aFaceId, uMin, uMax, vMin, vMax);

  EXPECT_LT(uMin, uMax) << "UMin should be less than UMax for a valid face";
  EXPECT_LT(vMin, vMax) << "VMin should be less than VMax for a valid face";
  EXPECT_LT(uMin, Precision::Infinite());
  EXPECT_LT(vMin, Precision::Infinite());
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_Bounds_CylinderFaceReturnsSurfaceBounds)
{
  // Verify that Bounds() returns the same values as Surface()->Bounds() for each face.
  const uint32_t aNbFaces = myCylGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    if (!BRepGraph_Tool::Face::HasSurface(myCylGraph, aFaceId))
    {
      continue;
    }

    double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
    BRepGraph_Tool::Face::Bounds(myCylGraph, aFaceId, uMin, uMax, vMin, vMax);

    double expUMin = 0.0, expUMax = 0.0, expVMin = 0.0, expVMax = 0.0;
    BRepGraph_Tool::Face::Surface(myCylGraph, aFaceId)->Bounds(expUMin, expUMax, expVMin, expVMax);

    EXPECT_EQ(uMin, expUMin) << "UMin mismatch on face " << aFaceId.Index;
    EXPECT_EQ(uMax, expUMax) << "UMax mismatch on face " << aFaceId.Index;
    EXPECT_EQ(vMin, expVMin) << "VMin mismatch on face " << aFaceId.Index;
    EXPECT_EQ(vMax, expVMax) << "VMax mismatch on face " << aFaceId.Index;
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Wire_FaceOf_ReturnsValidFace)
{
  const BRepGraph_WireId aWireId(0);
  const BRepGraph_FaceId aFace = BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aWireId);
  EXPECT_TRUE(aFace.IsValid()) << "Wire 0 should have an owning face";
}

TEST_F(BRepGraph_QuerySurfaceTest, Wire_FaceOf_SkipsRemovedParentWireRefs)
{
  const BRepGraph_WireId aWireId(0);
  const BRepGraph_FaceId aFirstFace = BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aWireId);
  ASSERT_TRUE(aFirstFace.IsValid());

  BRepGraph_FaceId aSecondFace;
  for (BRepGraph_FaceId aFaceIt = BRepGraph_FaceId::Start();
       aFaceIt.IsValid(myBoxGraph.Topo().Faces().Nb());
       ++aFaceIt)
  {
    if (aFaceIt != aFirstFace)
    {
      aSecondFace = aFaceIt;
      break;
    }
  }
  ASSERT_TRUE(aSecondFace.IsValid());

  const BRepGraph_WireRefId aSecondRef =
    myBoxGraph.Editor().Faces().Append(aSecondFace, aWireId, BRepGraphInc::ParityOrientation());
  ASSERT_TRUE(aSecondRef.IsValid());
  EXPECT_EQ(BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aWireId), aFirstFace);

  BRepGraph_WireRefId aFirstRef;
  const BRepGraphInc::FaceRelations& aFirstRelations =
    myBoxGraph.Topo().Faces().Relations(aFirstFace);
  for (const BRepGraph_WireRefId& aRefId : aFirstRelations.WireRefIds)
  {
    if (!aRefId.IsRemoved(myBoxGraph)
        && myBoxGraph.Refs().Wires().Entry(aRefId).ChildWireId == aWireId)
    {
      aFirstRef = aRefId;
      break;
    }
  }
  ASSERT_TRUE(aFirstRef.IsValid());

  ASSERT_TRUE(myBoxGraph.Editor().Faces().RemoveWire(aFirstFace, aFirstRef));
  EXPECT_EQ(BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aWireId), aSecondFace);
}

TEST_F(BRepGraph_QuerySurfaceTest, Wire_IsOuter_FirstWireOfBoxFaceIsOuter)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_WireId anOuterWire = BRepGraph_Tool::Face::OuterWire(myBoxGraph, aFaceId);
  ASSERT_TRUE(anOuterWire.IsValid());
  EXPECT_TRUE(BRepGraph_Tool::Wire::IsOuter(myBoxGraph, anOuterWire))
    << "OuterWire-found wire should be flagged IsOuter";
}

TEST_F(BRepGraph_QuerySurfaceTest, WireRef_ContextPreservesParentFaceAndOuterRole)
{
  const BRepGraph_FaceId aFirstFace(0);
  const BRepGraph_WireRefId anOuterRef = BRepGraph_Tool::Face::OuterWireRef(myBoxGraph, aFirstFace);
  ASSERT_TRUE(anOuterRef.IsValid());
  EXPECT_EQ(BRepGraph_Tool::Wire::FaceOf(myBoxGraph, anOuterRef), aFirstFace);
  EXPECT_TRUE(BRepGraph_Tool::Wire::IsOuter(myBoxGraph, anOuterRef));
  EXPECT_EQ(BRepGraph_Tool::Face::OuterWire(myBoxGraph, aFirstFace),
            myBoxGraph.Refs().Wires().Entry(anOuterRef).ChildWireId);

  BRepGraph_FaceId aSecondFace;
  for (BRepGraph_FaceId aFace = BRepGraph_FaceId::Start();
       aFace.IsValid(myBoxGraph.Topo().Faces().Nb());
       ++aFace)
  {
    if (aFace != aFirstFace)
    {
      aSecondFace = aFace;
      break;
    }
  }
  ASSERT_TRUE(aSecondFace.IsValid());

  const BRepGraph_WireId aSharedWire = myBoxGraph.Refs().Wires().Entry(anOuterRef).ChildWireId;
  const BRepGraph_WireRefId aSecondRef =
    myBoxGraph.Editor().Faces().Append(aSecondFace, aSharedWire, TopAbs_REVERSED);
  ASSERT_TRUE(aSecondRef.IsValid());

  EXPECT_EQ(BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aSecondRef), aSecondFace);
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_CurveOnSurfaceMatchesReversedWireOccurrence)
{
  const BRepGraph_FaceId aFace(0);
  const BRepGraph_WireRefId aWireRef = BRepGraph_Tool::Face::OuterWireRef(myBoxGraph, aFace);
  ASSERT_TRUE(aWireRef.IsValid());
  const BRepGraph_WireId aWire = myBoxGraph.Refs().Wires().Entry(aWireRef).ChildWireId;
  ASSERT_FALSE(myBoxGraph.Topo().Wires().Relations(aWire).CoEdgeIds.IsEmpty());
  const BRepGraph_CoEdgeId aCoEdge = myBoxGraph.Topo().Wires().Relations(aWire).CoEdgeIds.First();

  myBoxGraph.Editor().Wires().SetRefOrientation(aWireRef, TopAbs_REVERSED);
  const BRepGraph_Tool::EdgeUsage anEdgeUsage{
    BRepGraph_Tool::CoEdge::EdgeOf(myBoxGraph, aCoEdge),
    TopLoc_Location(),
    TopAbs::Compose(TopAbs_REVERSED,
                    BRepGraph_Tool::CoEdge::Orientation(myBoxGraph, aCoEdge))};
  const BRepGraph_Tool::FaceUsage aFaceUsage{aFace, TopLoc_Location(), TopAbs_FORWARD};

  EXPECT_TRUE(BRepGraph_Tool::Edge::CurveOnSurface(myBoxGraph, anEdgeUsage, aFaceUsage)
                .IsCurveOnSurface());
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_CurveOnSurfaceAcceptsSecondSharedWireParent)
{
  const BRepGraph_FaceId aFirstFace(0);
  const BRepGraph_WireRefId aFirstRef =
    BRepGraph_Tool::Face::OuterWireRef(myBoxGraph, aFirstFace);
  ASSERT_TRUE(aFirstRef.IsValid());
  const BRepGraph_WireId aWire = myBoxGraph.Refs().Wires().Entry(aFirstRef).ChildWireId;
  ASSERT_FALSE(myBoxGraph.Topo().Wires().Relations(aWire).CoEdgeIds.IsEmpty());
  const BRepGraph_CoEdgeId aCoEdge = myBoxGraph.Topo().Wires().Relations(aWire).CoEdgeIds.First();

  const BRepGraph_FaceId aSecondFace(1);
  ASSERT_TRUE(myBoxGraph.Editor().Faces().Append(aSecondFace, aWire, TopAbs_FORWARD).IsValid());
  myBoxGraph.Editor().CoEdges().SetFaceId(aCoEdge, BRepGraph_FaceId());
  myBoxGraph.Editor().CoEdges().ClearPCurve(aCoEdge);

  const BRepGraphInc::CoEdgeInstance aUsage{
    aCoEdge, TopLoc_Location(), BRepGraph_Tool::CoEdge::Orientation(myBoxGraph, aCoEdge)};
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::IsInFaceContext(myBoxGraph, aCoEdge, aFirstFace));
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::IsInFaceContext(myBoxGraph, aCoEdge, aSecondFace));
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurveAdaptorInFace(myBoxGraph, aCoEdge, aFirstFace)
                .IsInitialized());
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurveAdaptorInFace(myBoxGraph, aCoEdge, aSecondFace)
                .IsInitialized());
  EXPECT_FALSE(BRepGraph_Tool::Edge::CurveOnSurface(myBoxGraph, aUsage, aFirstFace).IsNull());
  EXPECT_FALSE(BRepGraph_Tool::Edge::CurveOnSurface(myBoxGraph, aUsage, aSecondFace).IsNull());
}

TEST_F(BRepGraph_QuerySurfaceTest, WireRef_ContextRejectsOutOfRangeReference)
{
  const BRepGraph_WireRefId anInvalidRef(myBoxGraph.Refs().Wires().Nb());

  EXPECT_FALSE(BRepGraph_Tool::Wire::Usage(myBoxGraph, anInvalidRef).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Wire::FaceOf(myBoxGraph, anInvalidRef).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Wire::IsOuter(myBoxGraph, anInvalidRef));
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_OuterWire_CacheRefreshesAfterFaceMutation)
{
  const BRepGraph_FaceId aFaceId(0);
  ASSERT_TRUE(aFaceId.IsValid(myBoxGraph.Topo().Faces().Nb()));

  EXPECT_TRUE(myBoxGraph.CacheRegistry().Find<BRepGraph_CacheDerivedState>().IsNull());

  const BRepGraph_WireId anOuterWire = BRepGraph_Tool::Face::OuterWire(myBoxGraph, aFaceId);
  ASSERT_TRUE(anOuterWire.IsValid());
  EXPECT_FALSE(myBoxGraph.CacheRegistry().Find<BRepGraph_CacheDerivedState>().IsNull());

  const BRepGraphInc::FaceRelations& aFaceRelations = myBoxGraph.Topo().Faces().Relations(aFaceId);
  ASSERT_FALSE(aFaceRelations.WireRefIds.IsEmpty());
  ASSERT_TRUE(myBoxGraph.Editor().Faces().RemoveWire(aFaceId, aFaceRelations.WireRefIds.First()));

  EXPECT_FALSE(BRepGraph_Tool::Face::OuterWire(myBoxGraph, aFaceId).IsValid());
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_OuterWire_InvalidFaceDoesNotCreateCache)
{
  EXPECT_FALSE(BRepGraph_Tool::Face::OuterWire(myBoxGraph, BRepGraph_FaceId::Invalid()).IsValid());
  EXPECT_TRUE(myBoxGraph.CacheRegistry().Find<BRepGraph_CacheDerivedState>().IsNull());
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_OuterWire_PeriodicFacesHaveCachedOuterWire)
{
  auto checkOuterWires = [](BRepGraph& theGraph) {
    ASSERT_GT(theGraph.Topo().Faces().Nb(), 0u);
    for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    {
      const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
      const BRepGraph_WireId aFirstWire = BRepGraph_Tool::Face::OuterWire(theGraph, aFaceId);
      ASSERT_TRUE(aFirstWire.IsValid()) << "Face " << aFaceId.Index << " has no outer wire";
      EXPECT_TRUE(BRepGraph_Tool::Wire::IsOuter(theGraph, aFirstWire))
        << "Wire " << aFirstWire.Index << " should be marked as outer";

      const BRepGraph_WireId aCachedWire = BRepGraph_Tool::Face::OuterWire(theGraph, aFaceId);
      EXPECT_EQ(aCachedWire, aFirstWire) << "Cached outer wire changed for face " << aFaceId.Index;
    }
  };

  checkOuterWires(myCylGraph);

  BRepGraph aSphereGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes =
    aSphereGraph.Shapes().Add(BRepPrimAPI_MakeSphere(10.0).Shape());
  ASSERT_FALSE(aSphereGraph.IsEmpty());
  checkOuterWires(aSphereGraph);
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_NbFaces_BoxEdgeHasExactlyTwoFaces)
{
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
       anEdgeId.IsValid(myBoxGraph.Topo().Edges().Nb());
       ++anEdgeId)
  {
    const uint32_t aNbFaces = BRepGraph_Tool::Edge::NbFaces(myBoxGraph, anEdgeId);
    EXPECT_EQ(aNbFaces, 2u) << "Box edge " << anEdgeId.Index
                            << " should be shared by exactly 2 faces";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_IsManifold_BoxEdgesAreManifold)
{
  const uint32_t aNbEdges = myBoxGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::IsManifold(myBoxGraph, anEdgeId))
      << "Box edge " << anEdgeId.Index << " should be manifold";
    EXPECT_FALSE(BRepGraph_Tool::Edge::IsBoundary(myBoxGraph, anEdgeId))
      << "Box edge " << anEdgeId.Index << " should not be a boundary-only edge";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_IsClosed_BoxEdgesAreOpen)
{
  const uint32_t aNbEdges = myBoxGraph.Topo().Edges().Nb();
  ASSERT_GT(aNbEdges, 0);
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_FALSE(BRepGraph_Tool::Edge::IsClosed(myBoxGraph, anEdgeId))
      << "Box edge " << anEdgeId.Index << " has distinct endpoints, should not be IsClosed";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_IsClosed_DerivedFromVertexTopology)
{
  const BRepGraph_EdgeId anEdgeId(0);
  ASSERT_TRUE(anEdgeId.IsValid(myBoxGraph.Topo().Edges().Nb()));
  // Box edges have distinct start/end vertices, so IsClosed is derived as false.
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsClosed(myBoxGraph, anEdgeId));
}

TEST_F(BRepGraph_QuerySurfaceTest, CoEdge_PolygonOnTriangulation_RoundTrip)
{
  ASSERT_GT(myBoxGraph.Topo().CoEdges().Nb(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  const BRepGraph_FaceId   aFaceId = BRepGraph_Tool::CoEdge::FaceOf(myBoxGraph, aCoEdgeId);
  EXPECT_FALSE(myBoxGraph.Mesh().Persistent().CoEdges().HasPolygonOnTriangulation(aCoEdgeId));
  EXPECT_TRUE(myBoxGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId).IsNull());

  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(1, 1, false);
  myBoxGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri = new Poly_PolygonOnTriangulation(2, false);
  myBoxGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolyOnTri);

  EXPECT_TRUE(myBoxGraph.Mesh().Persistent().CoEdges().HasPolygonOnTriangulation(aCoEdgeId));
  EXPECT_EQ(myBoxGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId).get(),
            aPolyOnTri.get());
}

TEST_F(BRepGraph_QuerySurfaceTest, CoEdge_UVPoints_UsesPlanarFallbackWithoutStoredPCurve)
{
  BRepGraph_CoEdgeId aCoEdgeId;
  for (BRepGraph_EdgeIterator anEdgeIt(myBoxGraph); anEdgeIt.More() && !aCoEdgeId.IsValid();
       anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      myBoxGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCandidate : aCoEdges)
    {
      if (!BRepGraph_Tool::CoEdge::IsReversed(myBoxGraph, aCandidate)
          && !BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCandidate).IsNull())
      {
        aCoEdgeId = aCandidate;
        break;
      }
    }
  }
  ASSERT_TRUE(aCoEdgeId.IsValid(myBoxGraph.Topo().CoEdges().Nb()));

  const std::pair<gp_Pnt2d, gp_Pnt2d> aStoredUV =
    BRepGraph_Tool::CoEdge::UVPoints(myBoxGraph, aCoEdgeId);

  myBoxGraph.Editor().CoEdges().ClearPCurve(aCoEdgeId);
  ASSERT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(myBoxGraph, aCoEdgeId));
  ASSERT_TRUE(BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCoEdgeId).IsNull());

  const std::pair<gp_Pnt2d, gp_Pnt2d> aFallbackUV =
    BRepGraph_Tool::CoEdge::UVPoints(myBoxGraph, aCoEdgeId);
  EXPECT_NEAR(aFallbackUV.first.X(), aStoredUV.first.X(), Precision::Confusion());
  EXPECT_NEAR(aFallbackUV.first.Y(), aStoredUV.first.Y(), Precision::Confusion());
  EXPECT_NEAR(aFallbackUV.second.X(), aStoredUV.second.X(), Precision::Confusion());
  EXPECT_NEAR(aFallbackUV.second.Y(), aStoredUV.second.Y(), Precision::Confusion());

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(myBoxGraph, aCoEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCoEdgeId).IsNull());
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_CurveOnSurface_UsesPlanarFallbackWithoutStoredPCurve)
{
  BRepGraph_CoEdgeId aCoEdgeId;
  for (BRepGraph_EdgeIterator anEdgeIt(myBoxGraph); anEdgeIt.More() && !aCoEdgeId.IsValid();
       anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      myBoxGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCandidate : aCoEdges)
    {
      if (!BRepGraph_Tool::CoEdge::IsReversed(myBoxGraph, aCandidate)
          && !BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCandidate).IsNull())
      {
        aCoEdgeId = aCandidate;
        break;
      }
    }
  }
  ASSERT_TRUE(aCoEdgeId.IsValid(myBoxGraph.Topo().CoEdges().Nb()));

  const BRepGraph_FaceId aFaceId = BRepGraph_Tool::CoEdge::FaceOf(myBoxGraph, aCoEdgeId);
  ASSERT_TRUE(aFaceId.IsValid(myBoxGraph.Topo().Faces().Nb()));

  const BRepGraph_Tool::CoEdgeUsage aUsage{
    aCoEdgeId,
    TopLoc_Location(),
    BRepGraph_Tool::CoEdge::Orientation(myBoxGraph, aCoEdgeId)};
  const occ::handle<Adaptor3d_CurveOnSurface> aStored =
    BRepGraph_Tool::Edge::CurveOnSurface(myBoxGraph, aUsage, aFaceId);
  ASSERT_FALSE(aStored.IsNull());

  const double aFirst = aStored->FirstParameter();
  const double aLast  = aStored->LastParameter();
  const gp_Pnt aStoredFirst = aStored->Value(aFirst);
  const gp_Pnt aStoredLast  = aStored->Value(aLast);

  myBoxGraph.Editor().CoEdges().ClearPCurve(aCoEdgeId);
  ASSERT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(myBoxGraph, aCoEdgeId));
  ASSERT_TRUE(BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCoEdgeId).IsNull());

  const occ::handle<Adaptor3d_CurveOnSurface> aFallback =
    BRepGraph_Tool::Edge::CurveOnSurface(myBoxGraph, aUsage, aFaceId);
  ASSERT_FALSE(aFallback.IsNull());

  EXPECT_NEAR(aFallback->FirstParameter(), aFirst, Precision::Confusion());
  EXPECT_NEAR(aFallback->LastParameter(), aLast, Precision::Confusion());
  EXPECT_TRUE(aFallback->Value(aFirst).IsEqual(aStoredFirst, Precision::Confusion()));
  EXPECT_TRUE(aFallback->Value(aLast).IsEqual(aStoredLast, Precision::Confusion()));

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(myBoxGraph, aCoEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurve(myBoxGraph, aCoEdgeId).IsNull());
}

TEST(BRepGraph_ToolTest, CoEdge_PlanarFallbackRejectsEqualRange)
{
  BRepGraph                aGraph;
  BRepGraph::EditorView&   aBuilder = aGraph.Editor();
  const BRepGraph_VertexId aVertex =
    aBuilder.Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(), 1.0);
  const BRepGraph_EdgeId         anEdge =
    aBuilder.Edges().Add(aVertex, aVertex, aCircle, 0.0, 0.0, Precision::Confusion());
  const BRepGraph_FaceId   aFace   = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                          BRepGraph_WireId(),
                                                          NCollection_Array1<BRepGraph_WireId>(),
                                                          Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge = aBuilder.CoEdges().Add(anEdge, TopAbs_FORWARD);
  aBuilder.CoEdges().SetFaceId(aCoEdge, aFace, false);

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge).IsInitialized());
}

TEST(BRepGraph_ToolTest, CoEdge_StoredPCurveRejectsReversedRange)
{
  BRepGraph                aGraph;
  BRepGraph::EditorView&   aBuilder = aGraph.Editor();
  const BRepGraph_VertexId aFirst =
    aBuilder.Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_VertexId aLast =
    aBuilder.Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId   anEdge = aBuilder.Edges().Add(aFirst,
                                                         aLast,
                                                         new Geom_BezierCurve(aPoles),
                                                         0.0,
                                                         1.0,
                                                         Precision::Confusion());
  const BRepGraph_FaceId   aFace  = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                         BRepGraph_WireId(),
                                                         NCollection_Array1<BRepGraph_WireId>(),
                                                         Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge =
    aBuilder.CoEdges().Add(anEdge,
                           aFace,
                           new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)),
                           1.0,
                           0.0,
                           TopAbs_FORWARD);

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge).IsInitialized());
}

TEST(BRepGraph_ToolTest, CoEdge_PlanarFallbackRejectsRangeOutsideTrimmedCurve)
{
  BRepGraph                  aGraph;
  BRepGraph::EditorView&     aBuilder = aGraph.Editor();
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  const BRepGraph_VertexId aFirst = aBuilder.Vertices().Add(aPoles.First(), Precision::Confusion());
  const BRepGraph_VertexId aLast  = aBuilder.Vertices().Add(aPoles.Last(), Precision::Confusion());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_TrimmedCurve(new Geom_BezierCurve(aPoles), 0.25, 0.75);
  const BRepGraph_EdgeId anEdge =
    aBuilder.Edges().Add(aFirst, aLast, aCurve, 0.0, 1.0, Precision::Confusion());
  const BRepGraph_FaceId   aFace   = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                          BRepGraph_WireId(),
                                                          NCollection_Array1<BRepGraph_WireId>(),
                                                          Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge = aBuilder.CoEdges().Add(anEdge, TopAbs_FORWARD);
  aBuilder.CoEdges().SetFaceId(aCoEdge, aFace, false);

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge).IsInitialized());
}

TEST(BRepGraph_ToolTest, CoEdge_PlanarFallbackRejectsRangeOutsideBoundedCurve)
{
  BRepGraph                  aGraph;
  BRepGraph::EditorView&     aBuilder = aGraph.Editor();
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  const BRepGraph_VertexId aFirst = aBuilder.Vertices().Add(aPoles.First(), Precision::Confusion());
  const BRepGraph_VertexId aLast  = aBuilder.Vertices().Add(aPoles.Last(), Precision::Confusion());
  const occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  const BRepGraph_EdgeId              anEdge =
    aBuilder.Edges().Add(aFirst, aLast, aCurve, -1.0, 2.0, Precision::Confusion());
  const BRepGraph_FaceId   aFace   = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                          BRepGraph_WireId(),
                                                          NCollection_Array1<BRepGraph_WireId>(),
                                                          Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge = aBuilder.CoEdges().Add(anEdge, TopAbs_FORWARD);
  aBuilder.CoEdges().SetFaceId(aCoEdge, aFace, false);

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge).IsInitialized());
}

TEST(BRepGraph_ToolTest, CoEdge_PlanarFallbackAcceptsShiftedPeriodicRange)
{
  BRepGraph                aGraph;
  BRepGraph::EditorView&   aBuilder = aGraph.Editor();
  const BRepGraph_VertexId aVertex =
    aBuilder.Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(), 1.0);
  const BRepGraph_EdgeId         anEdge =
    aBuilder.Edges().Add(aVertex, aVertex, aCircle, 2.0 * M_PI, 4.0 * M_PI, Precision::Confusion());
  const BRepGraph_FaceId   aFace   = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                          BRepGraph_WireId(),
                                                          NCollection_Array1<BRepGraph_WireId>(),
                                                          Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge = aBuilder.CoEdges().Add(anEdge, TopAbs_FORWARD);
  aBuilder.CoEdges().SetFaceId(aCoEdge, aFace, false);

  const Geom2dAdaptor_Curve aPCurve = BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge);
  ASSERT_TRUE(aPCurve.IsInitialized());
  EXPECT_NEAR(aPCurve.FirstParameter(), 2.0 * M_PI, Precision::PConfusion());
  EXPECT_NEAR(aPCurve.LastParameter(), 4.0 * M_PI, Precision::PConfusion());
}

TEST(BRepGraph_ToolTest, CoEdge_PlanarFallbackRejectsReversedPeriodicRange)
{
  BRepGraph                aGraph;
  BRepGraph::EditorView&   aBuilder = aGraph.Editor();
  const BRepGraph_VertexId aVertex =
    aBuilder.Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_EdgeId   anEdge  = aBuilder.Edges().Add(aVertex,
                                                          aVertex,
                                                          new Geom_Circle(gp_Ax2(), 1.0),
                                                          2.0 * M_PI,
                                                          0.0,
                                                          Precision::Confusion());
  const BRepGraph_FaceId   aFace   = aBuilder.Faces().Add(new Geom_Plane(gp_Pln()),
                                                          BRepGraph_WireId(),
                                                          NCollection_Array1<BRepGraph_WireId>(),
                                                          Precision::Confusion());
  const BRepGraph_CoEdgeId aCoEdge = aBuilder.CoEdges().Add(anEdge, TopAbs_FORWARD);
  aBuilder.CoEdges().SetFaceId(aCoEdge, aFace, false);

  EXPECT_FALSE(BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdge).IsInitialized());
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_PolygonOnTriangulation_ResolvesViaFace)
{
  ASSERT_GT(myBoxGraph.Topo().CoEdges().Nb(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  const BRepGraph_EdgeId   anEdgeId = BRepGraph_Tool::CoEdge::EdgeOf(myBoxGraph, aCoEdgeId);
  const BRepGraph_FaceId   aFaceId  = BRepGraph_Tool::CoEdge::FaceOf(myBoxGraph, aCoEdgeId);
  ASSERT_TRUE(anEdgeId.IsValid());
  ASSERT_TRUE(aFaceId.IsValid());

  EXPECT_FALSE(myBoxGraph.Mesh().Persistent().Edges().HasPolygonOnTriangulation(anEdgeId, aFaceId));

  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(1, 1, false);
  myBoxGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri = new Poly_PolygonOnTriangulation(2, false);
  myBoxGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolyOnTri);

  EXPECT_TRUE(myBoxGraph.Mesh().Persistent().Edges().HasPolygonOnTriangulation(anEdgeId, aFaceId));
  EXPECT_EQ(myBoxGraph.Mesh().Persistent().Edges().PolygonOnTriangulation(anEdgeId, aFaceId).get(),
            aPolyOnTri.get());
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_CachedTriangulation_DefaultEmpty)
{
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(myBoxGraph.Topo().Faces().Nb()); ++aFaceId)
  {
    EXPECT_FALSE(myBoxGraph.Mesh().Cache().Faces().Has(aFaceId));
    EXPECT_TRUE(myBoxGraph.Mesh().Cache().Faces().Triangulation(aFaceId).IsNull());
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_CachedTriangulation_SetAndRead)
{
  ASSERT_GT(myBoxGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_FaceId aFaceId(0);

  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(1, 1, false);

  myBoxGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);

  EXPECT_TRUE(myBoxGraph.Mesh().Cache().Faces().Has(aFaceId));
  EXPECT_EQ(myBoxGraph.Mesh().Cache().Faces().Triangulation(aFaceId).get(), aTri.get());
}

TEST_F(BRepGraph_QuerySurfaceTest, Vertex_NbEdges_BoxVertexHasThreeEdges)
{
  const uint32_t aNbVertices = myBoxGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const int aNbEdges = BRepGraph_Tool::Vertex::NbEdges(myBoxGraph, aVertexId);
    EXPECT_EQ(aNbEdges, 3) << "Box vertex " << aVertexId.Index << " should be incident to 3 edges";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Shell_IsClosed_BoxShellIsClosed)
{
  ASSERT_GE(myBoxGraph.Topo().Shells().Nb(), 1);
  EXPECT_TRUE(BRepGraph_Tool::Shell::IsClosed(myBoxGraph, BRepGraph_ShellId::Start()))
    << "Box shell should be closed";
}

TEST_F(BRepGraph_QuerySurfaceTest, DerivedStateQueriesAreConcurrentReadSafe)
{
  const BRepGraph& aGraph = myBoxGraph;
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0u);
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0u);
  ASSERT_GT(aGraph.Topo().Wires().Nb(), 0u);
  ASSERT_GT(aGraph.Topo().Shells().Nb(), 0u);

  constexpr int THE_NB_THREADS    = 8;
  constexpr int THE_NB_ITERATIONS = 128;

  std::atomic<bool> aStart{false};
  std::atomic<int>  aFailures{0};
  std::vector<std::thread> aThreads;
  aThreads.reserve(THE_NB_THREADS);

  for (int aThread = 0; aThread < THE_NB_THREADS; ++aThread)
  {
    aThreads.emplace_back(
      [&]() {
        while (!aStart.load(std::memory_order_acquire))
        {
          std::this_thread::yield();
        }

        for (int anIter = 0; anIter < THE_NB_ITERATIONS; ++anIter)
        {
          for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
               anEdgeId.IsValid(aGraph.Topo().Edges().Nb());
               ++anEdgeId)
          {
            if (BRepGraph_Tool::Edge::IsClosed(aGraph, anEdgeId)
                || BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeId))
            {
              aFailures.fetch_add(1, std::memory_order_relaxed);
            }
          }

          for (BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::Start();
               aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb());
               ++aCoEdgeId)
          {
            if (!BRepGraph_Tool::CoEdge::SameRange(aGraph, aCoEdgeId)
                || !BRepGraph_Tool::CoEdge::SameParameter(aGraph, aCoEdgeId))
            {
              aFailures.fetch_add(1, std::memory_order_relaxed);
            }
          }

          for (BRepGraph_WireId aWireId = BRepGraph_WireId::Start();
               aWireId.IsValid(aGraph.Topo().Wires().Nb());
               ++aWireId)
          {
            if (!BRepGraph_Tool::Wire::IsClosed(aGraph, aWireId))
            {
              aFailures.fetch_add(1, std::memory_order_relaxed);
            }
          }

          for (BRepGraph_ShellId aShellId = BRepGraph_ShellId::Start();
               aShellId.IsValid(aGraph.Topo().Shells().Nb());
               ++aShellId)
          {
            if (!BRepGraph_Tool::Shell::IsClosed(aGraph, aShellId))
            {
              aFailures.fetch_add(1, std::memory_order_relaxed);
            }
          }
        }
      });
  }

  aStart.store(true, std::memory_order_release);
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  EXPECT_EQ(aFailures.load(std::memory_order_relaxed), 0);
}

TEST_F(BRepGraph_QuerySurfaceTest, Shell_NbFaces_BoxShellHasSixFaces)
{
  ASSERT_GE(myBoxGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_Tool::Shell::NbFaces(myBoxGraph, BRepGraph_ShellId::Start()), 6)
    << "Box shell should reference 6 faces";
}
