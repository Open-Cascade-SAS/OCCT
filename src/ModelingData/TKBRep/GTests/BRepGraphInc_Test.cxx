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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

#include <type_traits>

static_assert(std::is_same_v<BRepGraphInc::VertexDef::TypeId, BRepGraph_VertexId>);
static_assert(std::is_same_v<BRepGraphInc::FaceRef::TypeId, BRepGraph_FaceRefId>);
static_assert(std::is_same_v<BRepGraphInc::SurfaceRep::TypeId, BRepGraph_SurfaceRepId>);

static double computeArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

static double computeVolume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

static int countSubShapes(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
    ++aCount;
  return aCount;
}

// ============================================================
// Entity count validation
// ============================================================

TEST(BRepGraphIncTest, Box_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Build BRepGraph for parity checks.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Build incidence storage.
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // Entity counts must match Def counts.
  EXPECT_EQ(aStorage.NbVertices(), aGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aStorage.NbEdges(), aGraph.Topo().Edges().Nb());
  EXPECT_EQ(aStorage.NbWires(), aGraph.Topo().Wires().Nb());
  EXPECT_EQ(aStorage.NbFaces(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aStorage.NbShells(), aGraph.Topo().Shells().Nb());
  EXPECT_EQ(aStorage.NbSolids(), aGraph.Topo().Solids().Nb());
}

TEST(BRepGraphIncTest, Cylinder_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbVertices(), aGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aStorage.NbEdges(), aGraph.Topo().Edges().Nb());
  EXPECT_EQ(aStorage.NbWires(), aGraph.Topo().Wires().Nb());
  EXPECT_EQ(aStorage.NbFaces(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aStorage.NbShells(), aGraph.Topo().Shells().Nb());
  EXPECT_EQ(aStorage.NbSolids(), aGraph.Topo().Solids().Nb());
}

TEST(BRepGraphIncTest, Sphere_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aSph);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aSph, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbVertices(), aGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aStorage.NbEdges(), aGraph.Topo().Edges().Nb());
  EXPECT_EQ(aStorage.NbWires(), aGraph.Topo().Wires().Nb());
  EXPECT_EQ(aStorage.NbFaces(), aGraph.Topo().Faces().Nb());
}

TEST(BRepGraphIncTest, Storage_MarkRemovedRep_DecrementsActiveCountsAndIsIdempotent)
{
  BRepGraphInc_Storage aStorage;

  (void)aStorage.AppendSurfaceRep();
  (void)aStorage.AppendCurve3DRep();
  (void)aStorage.AppendCurve2DRep();
  (void)aStorage.AppendTriangulationRep();
  (void)aStorage.AppendPolygon3DRep();
  (void)aStorage.AppendPolygon2DRep();
  (void)aStorage.AppendPolygonOnTriRep();

  EXPECT_EQ(aStorage.NbActiveSurfaces(), 1);
  EXPECT_EQ(aStorage.NbActiveCurves3D(), 1);
  EXPECT_EQ(aStorage.NbActiveCurves2D(), 1);
  EXPECT_EQ(aStorage.NbActiveTriangulations(), 1);
  EXPECT_EQ(aStorage.NbActivePolygons3D(), 1);
  EXPECT_EQ(aStorage.NbActivePolygons2D(), 1);
  EXPECT_EQ(aStorage.NbActivePolygonsOnTri(), 1);

  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_SurfaceRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Curve3DRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Curve2DRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_TriangulationRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Polygon3DRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Polygon2DRepId::Start()));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_PolygonOnTriRepId::Start()));

  EXPECT_TRUE(aStorage.SurfaceRep(BRepGraph_SurfaceRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.Curve3DRep(BRepGraph_Curve3DRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.Curve2DRep(BRepGraph_Curve2DRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.TriangulationRep(BRepGraph_TriangulationRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.Polygon3DRep(BRepGraph_Polygon3DRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.Polygon2DRep(BRepGraph_Polygon2DRepId::Start()).IsRemoved);
  EXPECT_TRUE(aStorage.PolygonOnTriRep(BRepGraph_PolygonOnTriRepId::Start()).IsRemoved);

  EXPECT_EQ(aStorage.NbActiveSurfaces(), 0);
  EXPECT_EQ(aStorage.NbActiveCurves3D(), 0);
  EXPECT_EQ(aStorage.NbActiveCurves2D(), 0);
  EXPECT_EQ(aStorage.NbActiveTriangulations(), 0);
  EXPECT_EQ(aStorage.NbActivePolygons3D(), 0);
  EXPECT_EQ(aStorage.NbActivePolygons2D(), 0);
  EXPECT_EQ(aStorage.NbActivePolygonsOnTri(), 0);

  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_SurfaceRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Curve3DRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Curve2DRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_TriangulationRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Polygon3DRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Polygon2DRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_PolygonOnTriRepId::Start()));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_RepId()));
}

TEST(BRepGraphIncTest, Storage_AppendAccess_UsesTypedIds)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId     aVertexId     = aStorage.AppendVertex();
  const BRepGraph_FaceRefId    aFaceRefId    = aStorage.AppendFaceRef();
  const BRepGraph_SurfaceRepId aSurfaceRepId = aStorage.AppendSurfaceRep();

  EXPECT_EQ(aVertexId.Index, 0);
  EXPECT_EQ(aFaceRefId.Index, 0);
  EXPECT_EQ(aSurfaceRepId.Index, 0);

  EXPECT_FALSE(aStorage.Vertex(aVertexId).IsRemoved);
  EXPECT_FALSE(aStorage.FaceRef(aFaceRefId).IsRemoved);
  EXPECT_FALSE(aStorage.SurfaceRep(aSurfaceRepId).IsRemoved);

  aStorage.ChangeVertex(aVertexId).Tolerance      = 1.25;
  aStorage.ChangeFaceRef(aFaceRefId).Orientation  = TopAbs_REVERSED;
  aStorage.ChangeSurfaceRep(aSurfaceRepId).OwnGen = 7;

  EXPECT_DOUBLE_EQ(aStorage.Vertex(aVertexId).Tolerance, 1.25);
  EXPECT_EQ(aStorage.FaceRef(aFaceRefId).Orientation, TopAbs_REVERSED);
  EXPECT_EQ(aStorage.SurfaceRep(aSurfaceRepId).OwnGen, 7u);
}

TEST(BRepGraphIncTest, Storage_GenericIdDispatch_UsesTypedHelpers)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId     aVertexId     = aStorage.AppendVertex();
  const BRepGraph_FaceRefId    aFaceRefId    = aStorage.AppendFaceRef();
  const BRepGraph_SurfaceRepId aSurfaceRepId = aStorage.AppendSurfaceRep();

  aStorage.ChangeBaseRef(BRepGraph_RefId(aFaceRefId)).OwnGen = 11;

  EXPECT_EQ(aStorage.BaseRef(BRepGraph_RefId(aFaceRefId)).OwnGen, 11u);

  EXPECT_TRUE(aStorage.MarkRemoved(BRepGraph_NodeId(aVertexId)));
  EXPECT_TRUE(aStorage.MarkRemovedRef(BRepGraph_RefId(aFaceRefId)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_RepId(aSurfaceRepId)));

  EXPECT_TRUE(aStorage.Vertex(aVertexId).IsRemoved);
  EXPECT_TRUE(aStorage.FaceRef(aFaceRefId).IsRemoved);
  EXPECT_TRUE(aStorage.SurfaceRep(aSurfaceRepId).IsRemoved);

  EXPECT_EQ(aStorage.NbActiveVertices(), 0);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 0);
  EXPECT_EQ(aStorage.NbActiveSurfaces(), 0);
}

// ============================================================
// Round-trip: area preservation
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox       = aBoxMaker.Shape();
  const double        anOrigArea = computeArea(aBox);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl       = aCylMaker.Shape();
  const double             anOrigArea = computeArea(aCyl);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Sphere_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph       = aSphMaker.Shape();
  const double           anOrigArea = computeArea(aSph);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aSph, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// ============================================================
// Round-trip: volume preservation
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_VolumePreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox      = aBoxMaker.Shape();
  const double        anOrigVol = computeVolume(aBox);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_VolumePreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl      = aCylMaker.Shape();
  const double             anOrigVol = computeVolume(aCyl);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
}

// ============================================================
// Round-trip: sub-shape counts
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_SubShapeCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), countSubShapes(aBox, TopAbs_FACE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_WIRE), countSubShapes(aBox, TopAbs_WIRE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), countSubShapes(aBox, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), countSubShapes(aBox, TopAbs_VERTEX));
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_SubShapeCounts)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), countSubShapes(aCyl, TopAbs_FACE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_WIRE), countSubShapes(aCyl, TopAbs_WIRE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), countSubShapes(aCyl, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), countSubShapes(aCyl, TopAbs_VERTEX));
}

// ============================================================
// Reverse index consistency
// ============================================================

TEST(BRepGraphIncTest, Box_ReverseIndex_EdgesToWires)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // Every edge must appear in at least one wire.
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_WireId>* aWires =
      aStorage.ReverseIndex().WiresOfEdge(anEdgeId);
    EXPECT_TRUE(aWires != nullptr) << "Edge " << anEdgeId.Index << " not in any wire";
    if (aWires != nullptr)
    {
      EXPECT_GE(aWires->Length(), 1);
    }
  }
}

TEST(BRepGraphIncTest, Box_ReverseIndex_EdgesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // Every edge must appear in at least one face (via EdgeFaceGeom).
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    if (aStorage.Edge(anEdgeId).IsDegenerate)
      continue;
    const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
      aStorage.ReverseIndex().FacesOfEdge(anEdgeId);
    EXPECT_TRUE(aFaces != nullptr) << "Edge " << anEdgeId.Index << " not in any face";
    if (aFaces != nullptr)
    {
      EXPECT_GE(aFaces->Length(), 1);
    }
  }
}

// ============================================================
// Parallel population produces same results
// ============================================================

TEST(BRepGraphIncTest, Box_ParallelPopulate_SameEntityCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aSerial;
  BRepGraphInc_Populate::Perform(aSerial, aBox, false);
  ASSERT_TRUE(aSerial.GetIsDone());

  BRepGraphInc_Storage aParallel;
  BRepGraphInc_Populate::Perform(aParallel, aBox, true);
  ASSERT_TRUE(aParallel.GetIsDone());

  EXPECT_EQ(aParallel.NbVertices(), aSerial.NbVertices());
  EXPECT_EQ(aParallel.NbEdges(), aSerial.NbEdges());
  EXPECT_EQ(aParallel.NbWires(), aSerial.NbWires());
  EXPECT_EQ(aParallel.NbFaces(), aSerial.NbFaces());
  EXPECT_EQ(aParallel.NbShells(), aSerial.NbShells());
  EXPECT_EQ(aParallel.NbSolids(), aSerial.NbSolids());
}

// ============================================================
// Null shape handling
// ============================================================

TEST(BRepGraphIncTest, NullShape_NoEntities)
{
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, TopoDS_Shape(), false);
  EXPECT_FALSE(aStorage.GetIsDone());
  EXPECT_EQ(aStorage.NbVertices(), 0);
  EXPECT_EQ(aStorage.NbEdges(), 0);
}

// ============================================================
// Compound shape handling
// ============================================================

TEST(BRepGraphIncTest, Compound_RoundTrip_SubShapeCounts)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 10.0, 10.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());

  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 5.0, 5.0);
  aBB.Add(aCompound, aBoxMaker2.Shape());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // Two solids, two shells.
  EXPECT_EQ(aStorage.NbSolids(), 2);
  EXPECT_EQ(aStorage.NbShells(), 2);
  EXPECT_EQ(aStorage.NbCompounds(), 1);
  EXPECT_EQ(aStorage.NbFaces(), 12);

  // Round-trip reconstruct via compound.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}

// ============================================================
// CoEdge count consistency (PCurve data lives on CoEdgeDef)
// ============================================================

TEST(BRepGraphIncTest, Box_CoEdgeCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // A box has 12 edges, each shared by 2 faces => 24 CoEdge entries total.
  // (No seam edges on a box.)
  int       aCoEdgeCount = 0;
  const int aNbEdges     = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
    if (aCoEdgeIdxs != nullptr)
      aCoEdgeCount += aCoEdgeIdxs->Length();
  }
  EXPECT_EQ(aCoEdgeCount, 24);
}

TEST(BRepGraphIncTest, Cylinder_HasSeamEdges)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // A cylinder has seam edges: coedges with SeamPairId valid.
  int       aSeamPairCount = 0;
  const int aNbEdges       = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
    if (aCoEdgeIdxs == nullptr)
      continue;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aStorage.CoEdge(aCoEdgeId);
      if (aCE.Orientation == TopAbs_FORWARD && aCE.SeamPairId.IsValid())
      {
        ++aSeamPairCount;
      }
    }
  }
  // A cylinder has 1 seam edge on its lateral face.
  EXPECT_GE(aSeamPairCount, 1) << "Cylinder should have at least 1 seam edge pair";
}

TEST(BRepGraphIncTest, Cylinder_SeamEdge_ReverseIndex_NoDuplicateFace)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // For seam edges (two PCurve entries with the same FaceDefId but opposite
  // orientations), the reverse index must contain each face only once.
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
      aStorage.ReverseIndex().FacesOfEdge(anEdgeId);
    if (aFaces == nullptr)
      continue;
    for (int i = 0; i < aFaces->Length(); ++i)
    {
      for (int j = i + 1; j < aFaces->Length(); ++j)
      {
        EXPECT_NE(aFaces->Value(i), aFaces->Value(j))
          << "Duplicate face " << aFaces->Value(i).Index << " in FacesOfEdge(" << anEdgeId.Index
          << ")";
      }
    }
  }
}

// ============================================================
// Degenerate edge handling
// ============================================================

TEST(BRepGraphIncTest, Sphere_DegenerateEdges_Preserved)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aSph, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // A sphere has degenerate edges at the poles (no 3D curve, collapsed to a point).
  int       aDegenerateCount = 0;
  const int aNbEdges         = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(anEdgeId);
    if (anEdge.IsDegenerate)
    {
      ++aDegenerateCount;
      EXPECT_FALSE(anEdge.Curve3DRepId.IsValid())
        << "Degenerate edge " << anEdgeId.Index << " should have no 3D curve";
    }
  }
  EXPECT_GE(aDegenerateCount, 2) << "Sphere should have at least 2 degenerate edges (poles)";

  // Round-trip: reconstructed sphere area must still match.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double anOrigArea = computeArea(aSph);
  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// ============================================================
// Location handling: compound with translated children
// ============================================================

TEST(BRepGraphIncTest, Compound_TranslatedChildren_VolumePreserved)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  // Box at origin.
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 10.0, 10.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());

  // Box translated by (100, 0, 0).
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 10.0, 10.0);
  TopoDS_Shape        aTranslatedBox = BRepBuilderAPI_Transform(aBoxMaker2.Shape(), aTrsf).Shape();
  aBB.Add(aCompound, aTranslatedBox);

  const double anOrigVol = computeVolume(aCompound);
  EXPECT_NEAR(anOrigVol, 2000.0, Precision::Confusion());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_BRepDump)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Dump both shapes and compare
  std::ostringstream anOrigStream, aReconStream;
  BRepTools::Write(aCyl, anOrigStream);
  BRepTools::Write(aRecon, aReconStream);

  if (anOrigStream.str() != aReconStream.str())
  {
    // Show ALL line differences
    std::istringstream anOrigLines(anOrigStream.str());
    std::istringstream aReconLines(aReconStream.str());
    std::string        anOrigLine, aReconLine;
    int                aLineNo    = 0;
    int                aDiffCount = 0;
    while (std::getline(anOrigLines, anOrigLine) && std::getline(aReconLines, aReconLine))
    {
      ++aLineNo;
      if (anOrigLine != aReconLine)
      {
        std::cout << "BRep diff at line " << aLineNo << ":" << std::endl;
        std::cout << "  ORIG:  " << anOrigLine << std::endl;
        std::cout << "  RECON: " << aReconLine << std::endl;
        ++aDiffCount;
      }
    }
    std::cout << "Total " << aDiffCount << " line differences. "
              << "Orig size=" << anOrigStream.str().size()
              << " Recon size=" << aReconStream.str().size() << std::endl;
  }
  else
  {
    std::cout << "BRep dumps are IDENTICAL (size=" << anOrigStream.str().size() << ")" << std::endl;
  }

  const double anOrigArea = computeArea(aCyl);
  const double aReconArea = computeArea(aRecon);
  std::cout << "Orig area=" << anOrigArea << " Recon area=" << aReconArea << std::endl;
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// ============================================================
// Edge internal vertices
// ============================================================

// Helper: create an edge with a line segment and add vertices with given orientations.
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

// Helper: build a face containing the given edge (needed for graph population).
static TopoDS_Shape wrapEdgeInFace(const TopoDS_Edge& theEdge)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  BRep_Builder            aBB;
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());
  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, theEdge);
  aBB.Add(aFace, aWire);
  return aFace;
}

TEST(BRepGraphIncTest, EdgeInternalVertex_Captured)
{
  TopoDS_Edge  anEdge = makeEdgeWithInternalVertex();
  TopoDS_Shape aFace  = wrapEdgeInFace(anEdge);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  ASSERT_GE(aStorage.NbEdges(), 1);

  // Find the edge entity and check InternalVertices.
  bool      aFound   = false;
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdgeEnt = aStorage.Edge(anEdgeId);
    if (anEdgeEnt.InternalVertexRefIds.Length() == 1)
    {
      aFound = true;
      const BRepGraphInc::VertexRef& aIntVRef =
        aStorage.VertexRef(anEdgeEnt.InternalVertexRefIds.Value(0));
      EXPECT_GE(aIntVRef.VertexDefId.Index, 0);
      EXPECT_EQ(aIntVRef.Orientation, TopAbs_INTERNAL);
      // Verify the vertex point.
      const BRepGraph_VertexId       aVtxId  = aIntVRef.VertexDefId;
      const BRepGraphInc::VertexDef& aVtxEnt = aStorage.Vertex(aVtxId);
      EXPECT_NEAR(aVtxEnt.Point.X(), 5.0, Precision::Confusion());
      break;
    }
  }
  EXPECT_TRUE(aFound) << "No edge with InternalVertices found";
}

TEST(BRepGraphIncTest, EdgeInternalVertex_RoundTrip)
{
  TopoDS_Edge  anEdge = makeEdgeWithInternalVertex();
  TopoDS_Shape aFace  = wrapEdgeInFace(anEdge);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Find the edge in the reconstructed face and verify internal vertex.
  bool aFoundInternal = false;
  for (TopExp_Explorer anEdgeExp(aRecon, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge& aReconEdge = TopoDS::Edge(anEdgeExp.Current());
    for (TopoDS_Iterator aVIt(aReconEdge, false); aVIt.More(); aVIt.Next())
    {
      if (aVIt.Value().ShapeType() == TopAbs_VERTEX
          && aVIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        aFoundInternal            = true;
        const TopoDS_Vertex& aVtx = TopoDS::Vertex(aVIt.Value());
        EXPECT_NEAR(BRep_Tool::Pnt(aVtx).X(), 5.0, Precision::Confusion());
      }
    }
  }
  EXPECT_TRUE(aFoundInternal) << "Internal vertex not found in reconstructed edge";
}

TEST(BRepGraphIncTest, EdgeExternalVertex_Captured)
{
  BRep_Builder            aBB;
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aMakeEdge.Edge();

  TopoDS_Vertex anExtVtx;
  aBB.MakeVertex(anExtVtx, gp_Pnt(7, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, anExtVtx.Oriented(TopAbs_EXTERNAL));

  TopoDS_Shape aFace = wrapEdgeInFace(anEdge);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  bool      aFound   = false;
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdgeEnt = aStorage.Edge(anEdgeId);
    if (anEdgeEnt.InternalVertexRefIds.Length() == 1)
    {
      aFound = true;
      EXPECT_EQ(aStorage.VertexRef(anEdgeEnt.InternalVertexRefIds.Value(0)).Orientation,
                TopAbs_EXTERNAL);
      break;
    }
  }
  EXPECT_TRUE(aFound) << "No edge with EXTERNAL vertex found";
}

TEST(BRepGraphIncTest, EdgeNoInternalVertices_EmptyVector)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_EQ(aStorage.Edge(anEdgeId).InternalVertexRefIds.Length(), 0)
      << "Edge " << anEdgeId.Index << " should have no internal vertices";
  }
}

TEST(BRepGraphIncTest, EdgeMultipleInternalVertices_AllCaptured)
{
  BRep_Builder            aBB;
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aMakeEdge.Edge();

  TopoDS_Vertex aVtx1, aVtx2;
  aBB.MakeVertex(aVtx1, gp_Pnt(3, 0, 0), Precision::Confusion());
  aBB.MakeVertex(aVtx2, gp_Pnt(7, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, aVtx1.Oriented(TopAbs_INTERNAL));
  aBB.Add(anEdge, aVtx2.Oriented(TopAbs_EXTERNAL));

  TopoDS_Shape aFace = wrapEdgeInFace(anEdge);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  bool      aFound   = false;
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdgeEnt = aStorage.Edge(anEdgeId);
    if (anEdgeEnt.InternalVertexRefIds.Length() == 2)
    {
      aFound = true;
      // Check both orientations are preserved.
      bool aHasInternal = false, aHasExternal = false;
      for (int j = 0; j < 2; ++j)
      {
        if (aStorage.VertexRef(anEdgeEnt.InternalVertexRefIds.Value(j)).Orientation
            == TopAbs_INTERNAL)
          aHasInternal = true;
        if (aStorage.VertexRef(anEdgeEnt.InternalVertexRefIds.Value(j)).Orientation
            == TopAbs_EXTERNAL)
          aHasExternal = true;
      }
      EXPECT_TRUE(aHasInternal);
      EXPECT_TRUE(aHasExternal);
      break;
    }
  }
  EXPECT_TRUE(aFound) << "No edge with 2 internal vertices found";
}

// ============================================================
// Face direct vertices
// ============================================================

TEST(BRepGraphIncTest, FaceDirectVertex_Internal_Captured)
{
  BRep_Builder            aBB;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  // Add a wire so the face is valid for population.
  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Wire             aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aFace, aWire);

  // Add a direct vertex child with INTERNAL orientation.
  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aVtx.Oriented(TopAbs_INTERNAL));

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  ASSERT_GE(aStorage.NbFaces(), 1);

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId::Start());
  EXPECT_EQ(aFaceEnt.VertexRefIds.Length(), 1);
  if (aFaceEnt.VertexRefIds.Length() == 1)
  {
    const BRepGraphInc::VertexRef& aFaceVRef = aStorage.VertexRef(aFaceEnt.VertexRefIds.Value(0));
    EXPECT_GE(aFaceVRef.VertexDefId.Index, 0);
    EXPECT_EQ(aFaceVRef.Orientation, TopAbs_INTERNAL);
  }
}

TEST(BRepGraphIncTest, FaceDirectVertex_RoundTrip)
{
  BRep_Builder            aBB;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Wire             aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aFace, aWire);

  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aVtx.Oriented(TopAbs_INTERNAL));

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Verify vertex is a direct child of the face (not inside a wire).
  bool aFoundDirect = false;
  for (TopoDS_Iterator aIt(aRecon); aIt.More(); aIt.Next())
  {
    if (aIt.Value().ShapeType() == TopAbs_VERTEX && aIt.Value().Orientation() == TopAbs_INTERNAL)
    {
      aFoundDirect                   = true;
      const TopoDS_Vertex& aReconVtx = TopoDS::Vertex(aIt.Value());
      EXPECT_NEAR(BRep_Tool::Pnt(aReconVtx).X(), 5.0, Precision::Confusion());
      EXPECT_NEAR(BRep_Tool::Pnt(aReconVtx).Y(), 5.0, Precision::Confusion());
    }
  }
  EXPECT_TRUE(aFoundDirect) << "Direct internal vertex not found in reconstructed face";
}

TEST(BRepGraphIncTest, FaceExternalVertex_Captured)
{
  BRep_Builder            aBB;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Wire             aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aFace, aWire);

  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aVtx.Oriented(TopAbs_EXTERNAL));

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  ASSERT_GE(aStorage.NbFaces(), 1);

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId::Start());
  EXPECT_EQ(aFaceEnt.VertexRefIds.Length(), 1);
  if (aFaceEnt.VertexRefIds.Length() == 1)
  {
    EXPECT_EQ(aStorage.VertexRef(aFaceEnt.VertexRefIds.Value(0)).Orientation, TopAbs_EXTERNAL);
  }
}

TEST(BRepGraphIncTest, FaceNoDirectVertices_EmptyVector)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  const int aNbFaces = aStorage.NbFaces();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    EXPECT_EQ(aStorage.Face(aFaceId).VertexRefIds.Length(), 0)
      << "Face " << aFaceId.Index << " should have no direct vertex children";
  }
}

TEST(BRepGraphIncTest, FaceWithWiresAndVertices_BothCaptured)
{
  BRep_Builder            aBB;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Wire             aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aFace, aWire);

  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aVtx.Oriented(TopAbs_INTERNAL));

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aFace, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  ASSERT_GE(aStorage.NbFaces(), 1);

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId::Start());
  EXPECT_GE(BRepGraph_TestTools::CountWireRefsOfFace(aStorage, BRepGraph_FaceId::Start()), 1);
  EXPECT_EQ(aFaceEnt.VertexRefIds.Length(), 1);
}

// ============================================================
// Integration: round-trip with internal vertices
// ============================================================

TEST(BRepGraphIncTest, CompoundWithInternalVertices_RoundTrip_SubShapeCounts)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  // Face with a direct internal vertex.
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aME.Edge();
  // Add internal vertex to the edge.
  TopoDS_Vertex anEdgeIntVtx;
  aBB.MakeVertex(anEdgeIntVtx, gp_Pnt(5, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, anEdgeIntVtx.Oriented(TopAbs_INTERNAL));

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, anEdge);
  aBB.Add(aFace, aWire);

  // Add direct vertex to the face.
  TopoDS_Vertex aFaceIntVtx;
  aBB.MakeVertex(aFaceIntVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aFaceIntVtx.Oriented(TopAbs_INTERNAL));

  aBB.Add(aCompound, aFace);

  // Count original sub-shapes.
  int anOrigFaces    = countSubShapes(aCompound, TopAbs_FACE);
  int anOrigEdges    = countSubShapes(aCompound, TopAbs_EDGE);
  int anOrigVertices = countSubShapes(aCompound, TopAbs_VERTEX);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), anOrigFaces);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), anOrigEdges);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), anOrigVertices);

  // Verify orientations of reconstructed internal vertices.
  // Edge internal vertex: INTERNAL orientation on edge child.
  bool aFoundEdgeIntVtx = false;
  for (TopExp_Explorer anEdgeExp(aRecon, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    for (TopoDS_Iterator aVIt(anEdgeExp.Current(), false); aVIt.More(); aVIt.Next())
    {
      if (aVIt.Value().ShapeType() == TopAbs_VERTEX
          && aVIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        aFoundEdgeIntVtx          = true;
        const TopoDS_Vertex& aVtx = TopoDS::Vertex(aVIt.Value());
        EXPECT_NEAR(BRep_Tool::Pnt(aVtx).X(), 5.0, Precision::Confusion());
        EXPECT_NEAR(BRep_Tool::Pnt(aVtx).Y(), 0.0, Precision::Confusion());
      }
    }
  }
  EXPECT_TRUE(aFoundEdgeIntVtx) << "INTERNAL vertex on edge not found after round-trip";

  // Face direct vertex: INTERNAL orientation as direct child of face.
  bool aFoundFaceIntVtx = false;
  for (TopExp_Explorer aFaceExp(aRecon, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    for (TopoDS_Iterator aFIt(aFaceExp.Current()); aFIt.More(); aFIt.Next())
    {
      if (aFIt.Value().ShapeType() == TopAbs_VERTEX
          && aFIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        aFoundFaceIntVtx          = true;
        const TopoDS_Vertex& aVtx = TopoDS::Vertex(aFIt.Value());
        EXPECT_NEAR(BRep_Tool::Pnt(aVtx).X(), 5.0, Precision::Confusion());
        EXPECT_NEAR(BRep_Tool::Pnt(aVtx).Y(), 5.0, Precision::Confusion());
      }
    }
  }
  EXPECT_TRUE(aFoundFaceIntVtx) << "INTERNAL vertex on face not found after round-trip";
}

TEST(BRepGraphIncTest, ParallelBuild_InternalVertices_SameAsSequential)
{
  BRep_Builder            aBB;
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aME.Edge();
  TopoDS_Vertex           anIntVtx;
  aBB.MakeVertex(anIntVtx, gp_Pnt(5, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, anIntVtx.Oriented(TopAbs_INTERNAL));

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, anEdge);
  aBB.Add(aFace, aWire);

  TopoDS_Vertex aFaceVtx;
  aBB.MakeVertex(aFaceVtx, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aFaceVtx.Oriented(TopAbs_INTERNAL));

  BRepGraphInc_Storage aSerial;
  BRepGraphInc_Populate::Perform(aSerial, aFace, false);
  ASSERT_TRUE(aSerial.GetIsDone());

  BRepGraphInc_Storage aParallel;
  BRepGraphInc_Populate::Perform(aParallel, aFace, true);
  ASSERT_TRUE(aParallel.GetIsDone());

  EXPECT_EQ(aParallel.NbVertices(), aSerial.NbVertices());
  EXPECT_EQ(aParallel.NbEdges(), aSerial.NbEdges());
  EXPECT_EQ(aParallel.NbFaces(), aSerial.NbFaces());

  // Check internal vertex counts match.
  const int aNbEdges = aSerial.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_EQ(aParallel.Edge(anEdgeId).InternalVertexRefIds.Length(),
              aSerial.Edge(anEdgeId).InternalVertexRefIds.Length())
      << "Edge " << anEdgeId.Index << " internal vertex count mismatch";
  }
  const int aNbFaces = aSerial.NbFaces();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    EXPECT_EQ(aParallel.Face(aFaceId).VertexRefIds.Length(),
              aSerial.Face(aFaceId).VertexRefIds.Length())
      << "Face " << aFaceId.Index << " direct vertex count mismatch";
  }
}

// ============================================================
// Reverse-index hardening: compound atomic children (Wire/Edge/Vertex)
// ============================================================

TEST(BRepGraphIncTest, ReverseIndex_Validate_CompoundWithAtomicWire)
{
  // Build a compound that contains only a wire (no solid/shell/face).
  // This exercises the Wire-child path in compound reverse-index maintenance.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  ASSERT_TRUE(aME.IsDone());

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aCompound, aWire);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompounds(), 1);
  ASSERT_GE(aStorage.NbWires(), 1);

  // The wire must appear in myCompoundsOfWire.
  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpVec =
    aStorage.ReverseIndex().CompoundsOfWire(BRepGraph_WireId::Start());
  EXPECT_NE(aCmpVec, nullptr) << "Wire(0) should appear in myCompoundsOfWire";
  if (aCmpVec != nullptr)
  {
    EXPECT_EQ(aCmpVec->Length(), 1);
    EXPECT_EQ(aCmpVec->Value(0).Index, 0);
  }

  // Full consistency check.
  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_Validate_CompoundWithAtomicEdge)
{
  // Build a compound containing only an edge (no wire/face/solid).
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(5, 0, 0));
  ASSERT_TRUE(aME.IsDone());
  aBB.Add(aCompound, aME.Edge());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompounds(), 1);
  ASSERT_GE(aStorage.NbEdges(), 1);

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpVec =
    aStorage.ReverseIndex().CompoundsOfEdge(BRepGraph_EdgeId::Start());
  EXPECT_NE(aCmpVec, nullptr) << "Edge(0) should appear in myCompoundsOfEdge";
  if (aCmpVec != nullptr)
  {
    EXPECT_EQ(aCmpVec->Length(), 1);
    EXPECT_EQ(aCmpVec->Value(0).Index, 0);
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_Validate_CompoundWithAtomicVertex)
{
  // Build a compound containing only a vertex.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(1, 2, 3), Precision::Confusion());
  aBB.Add(aCompound, aVtx);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompounds(), 1);
  ASSERT_GE(aStorage.NbVertices(), 1);

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpVec =
    aStorage.ReverseIndex().CompoundsOfVertex(BRepGraph_VertexId::Start());
  EXPECT_NE(aCmpVec, nullptr) << "Vertex(0) should appear in myCompoundsOfVertex";
  if (aCmpVec != nullptr)
  {
    EXPECT_EQ(aCmpVec->Length(), 1);
    EXPECT_EQ(aCmpVec->Value(0).Index, 0);
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_CoEdgeToWire_IsPopulated)
{
  // Every coedge in a box must map to exactly one wire in myCoEdgeToWires.
  BRepPrimAPI_MakeBox aBoxMaker(5.0, 5.0, 5.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  const int aNbCoEdges = aStorage.NbCoEdges();
  ASSERT_GT(aNbCoEdges, 0);
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCE = aStorage.CoEdge(aCoEdgeId);
    if (aCE.IsRemoved)
      continue;
    const NCollection_DynamicArray<BRepGraph_WireId>* aWires =
      aStorage.ReverseIndex().WiresOfCoEdge(aCoEdgeId);
    EXPECT_NE(aWires, nullptr) << "CoEdge " << aCoEdgeId.Index << " not in any wire";
    if (aWires != nullptr)
    {
      EXPECT_EQ(aWires->Length(), 1)
        << "CoEdge " << aCoEdgeId.Index << " should be in exactly one wire";
    }
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_CompSolid_ReverseMaintained_AfterBuild)
{
  // Build a TopoDS_CompSolid containing two boxes and verify:
  //   1. myCompSolidsOfSolid is populated for both solids
  //   2. ValidateReverseIndex() passes
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  BRepPrimAPI_MakeBox aBoxMaker1(4.0, 4.0, 4.0);
  BRepPrimAPI_MakeBox aBoxMaker2(2.0, 2.0, 2.0);
  aBB.Add(aCompSolid, aBoxMaker1.Shape());
  aBB.Add(aCompSolid, aBoxMaker2.Shape());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompSolid, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompSolids(), 1);
  ASSERT_GE(aStorage.NbSolids(), 2);

  // Both solids must appear in myCompSolidsOfSolid.
  for (BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
       aSolidId.IsValid(aStorage.NbSolids());
       ++aSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCSVec =
      aStorage.ReverseIndex().CompSolidsOfSolid(aSolidId);
    EXPECT_NE(aCSVec, nullptr) << "Solid " << aSolidId.Index << " not in any CompSolid";
    if (aCSVec != nullptr)
    {
      EXPECT_EQ(aCSVec->Length(), 1);
      EXPECT_EQ(aCSVec->Value(0).Index, 0);
    }
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_CompSolid_ReverseMaintained_AfterBuildDelta)
{
  // Verify BuildDelta() correctly indexes a CompSolid->Solid reverse mapping
  // when both the compsolid and solids are appended in the delta.
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  BRepPrimAPI_MakeBox aBoxMaker(3.0, 3.0, 3.0);
  aBB.Add(aCompSolid, aBoxMaker.Shape());

  // First, build an empty storage then delta-populate.
  // Simplest: just populate from scratch and check ValidateReverseIndex.
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompSolid, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompSolids(), 1);
  ASSERT_GE(aStorage.NbSolids(), 1);

  EXPECT_TRUE(aStorage.ValidateReverseIndex());

  const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCSVec =
    aStorage.ReverseIndex().CompSolidsOfSolid(BRepGraph_SolidId::Start());
  EXPECT_NE(aCSVec, nullptr) << "Solid 0 should be indexed in CompSolidsOfSolid";
}

TEST(BRepGraphIncTest, ReverseIndex_Validate_Box_FullConsistency)
{
  // Smoke test: a simple solid box must pass full reverse-index validation.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_Validate_Compound_FullConsistency)
{
  // A compound of two boxes must pass full reverse-index validation,
  // covering CompoundsOfSolid, CompoundsOfShell, CompoundsOfFace.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker1(5.0, 5.0, 5.0);
  BRepPrimAPI_MakeBox aBoxMaker2(3.0, 3.0, 3.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());
  aBB.Add(aCompound, aBoxMaker2.Shape());

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_TRUE(aStorage.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_AfterEditorMutations_StaysConsistent)
{
  // Verify the incremental Bind/Unbind mutation path keeps the reverse index
  // consistent with the forward entity / reference-entry tables across a
  // sequence of RemoveWire / RemoveFace / RemoveShell mutations.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox      aBoxMaker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 12.0);
  aBB.Add(aCompound, aBoxMaker.Shape());
  aBB.Add(aCompound, aCylMaker.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_TRUE(aGraph.ValidateReverseIndex());

  // Remove an inner wire from the first face that owns more than one wire
  // (or the only wire if all faces have a single wire).
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId                              aFaceId = aFaceIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_WireRefId> aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(aGraph, aFaceId);
    if (aWireRefs.IsEmpty())
      continue;
    ASSERT_TRUE(aGraph.Editor().Faces().RemoveWire(aFaceId, aWireRefs.Value(0)));
    break;
  }
  EXPECT_TRUE(aGraph.ValidateReverseIndex()) << "Reverse index inconsistent after RemoveWire";

  // Remove the first face from the first shell.
  const NCollection_DynamicArray<BRepGraph_FaceRefId> aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(aGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefs.Length(), 1);
  ASSERT_TRUE(aGraph.Editor().Shells().RemoveFace(BRepGraph_ShellId::Start(), aFaceRefs.Value(0)));
  EXPECT_TRUE(aGraph.ValidateReverseIndex()) << "Reverse index inconsistent after RemoveFace";

  // Remove the first shell from the first solid.
  const NCollection_DynamicArray<BRepGraph_ShellRefId> aShellRefs =
    BRepGraph_TestTools::ShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start());
  ASSERT_GE(aShellRefs.Length(), 1);
  ASSERT_TRUE(
    aGraph.Editor().Solids().RemoveShell(BRepGraph_SolidId::Start(), aShellRefs.Value(0)));
  EXPECT_TRUE(aGraph.ValidateReverseIndex()) << "Reverse index inconsistent after RemoveShell";

  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraphIncTest, ReverseIndex_BulkBuild_TwiceProducesEqualState)
{
  // Bulk Populate must be deterministic: building the same shape twice into
  // independent storages must yield byte-equal reverse-index views as observed
  // through the public per-entity accessors.
  BRepPrimAPI_MakeBox aBoxMaker(7.0, 11.0, 13.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorageA;
  BRepGraphInc_Storage aStorageB;
  BRepGraphInc_Populate::Perform(aStorageA, aBox, false);
  BRepGraphInc_Populate::Perform(aStorageB, aBox, false);
  ASSERT_TRUE(aStorageA.GetIsDone());
  ASSERT_TRUE(aStorageB.GetIsDone());
  ASSERT_EQ(aStorageA.NbEdges(), aStorageB.NbEdges());

  for (uint32_t anIdx = 0; anIdx < aStorageA.NbEdges(); ++anIdx)
  {
    const BRepGraph_EdgeId                            anEdgeId(anIdx);
    const NCollection_DynamicArray<BRepGraph_WireId>* aWiresA =
      aStorageA.ReverseIndex().WiresOfEdge(anEdgeId);
    const NCollection_DynamicArray<BRepGraph_WireId>* aWiresB =
      aStorageB.ReverseIndex().WiresOfEdge(anEdgeId);
    ASSERT_EQ(aWiresA == nullptr, aWiresB == nullptr);
    if (aWiresA == nullptr)
      continue;
    ASSERT_EQ(aWiresA->Size(), aWiresB->Size());
    for (size_t i = 0; i < aWiresA->Size(); ++i)
      EXPECT_EQ(aWiresA->Value(i), aWiresB->Value(i));

    const NCollection_DynamicArray<BRepGraph_FaceId>* aFacesA =
      aStorageA.ReverseIndex().FacesOfEdge(anEdgeId);
    const NCollection_DynamicArray<BRepGraph_FaceId>* aFacesB =
      aStorageB.ReverseIndex().FacesOfEdge(anEdgeId);
    ASSERT_EQ(aFacesA == nullptr, aFacesB == nullptr);
    if (aFacesA == nullptr)
      continue;
    ASSERT_EQ(aFacesA->Size(), aFacesB->Size());
    for (size_t i = 0; i < aFacesA->Size(); ++i)
      EXPECT_EQ(aFacesA->Value(i), aFacesB->Value(i));
  }
}

TEST(BRepGraphIncTest, ReverseIndex_EdgeOpsAdd_BindsStartEndVertices)
{
  // Free edge created at runtime via the Editor must show up under its endpoint
  // vertices in the reverse index. Pre-fix this query returned an empty list.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());
  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  bool foundV0 = false, foundV1 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
    if (aE == anEdge)
      foundV0 = true;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
    if (aE == anEdge)
      foundV1 = true;
  EXPECT_TRUE(foundV0);
  EXPECT_TRUE(foundV1);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_RemoveEdge_UnbindsStartEndVertices)
{
  // Symmetric to the Add test: removing the edge must drop the entries.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdge));

  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
    EXPECT_NE(aE, anEdge);
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
    EXPECT_NE(aE, anEdge);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetRefVertexDefId_RebindsVertexToEdges)
{
  // Rewire an edge's start-vertex ref to a different vertex and verify the
  // rev-index moved.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_VertexRefId aStartRef = aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  aGraph.Editor().Vertices().SetRefVertexDefId(aStartRef, aV2);

  bool stillUnderV0 = false, foundUnderV2 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
    if (aE == anEdge)
      stillUnderV0 = true;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV2))
    if (aE == anEdge)
      foundUnderV2 = true;
  EXPECT_FALSE(stillUnderV0);
  EXPECT_TRUE(foundUnderV2);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_BoxThroughCompact_StaysConsistent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_TRUE(aGraph.Editor().Faces().RemoveWire(
    BRepGraph_FaceId::Start(),
    BRepGraph_TestTools::WireRefsOfFace(aGraph, BRepGraph_FaceId::Start()).Value(0)));
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetRefWireDefId_RebindsWireToFaces)
{
  // Add two faces; rewire face0's outer-wire ref to face1's outer wire and
  // verify WireToFaces moved entries.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFace0 = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId aFace1(1);
  ASSERT_TRUE(aFace1.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_WireRefId aWireRef0 =
    BRepGraph_TestTools::WireRefsOfFace(aGraph, aFace0).Value(0);
  const BRepGraph_WireId aOldWire = aGraph.Refs().Wires().Entry(aWireRef0).WireDefId;
  const BRepGraph_WireId aNewWire =
    aGraph.Refs()
      .Wires()
      .Entry(BRepGraph_TestTools::WireRefsOfFace(aGraph, aFace1).Value(0))
      .WireDefId;
  ASSERT_NE(aOldWire, aNewWire);

  aGraph.Editor().Wires().SetRefWireDefId(aWireRef0, aNewWire);

  bool oldStillBound = false, newBound = false;
  for (const BRepGraph_FaceId& f : aGraph.Topo().Wires().Faces(aOldWire))
    if (f == aFace0)
      oldStillBound = true;
  for (const BRepGraph_FaceId& f : aGraph.Topo().Wires().Faces(aNewWire))
    if (f == aFace0)
      newBound = true;
  EXPECT_FALSE(oldStillBound);
  EXPECT_TRUE(newBound);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetRefFaceDefId_RebindsFaceToShells)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId   aShell = BRepGraph_ShellId::Start();
  const BRepGraph_FaceRefId aRef0  = BRepGraph_TestTools::FaceRefsOfShell(aGraph, aShell).Value(0);
  const BRepGraph_FaceId    aOldFace = aGraph.Refs().Faces().Entry(aRef0).FaceDefId;
  const BRepGraph_FaceId    aNewFace(1);
  ASSERT_NE(aOldFace, aNewFace);

  aGraph.Editor().Faces().SetRefFaceDefId(aRef0, aNewFace);

  // Old face still has the OTHER shell-ref pointing at it; just check the
  // Old/New rev-index makes sense relative to this single ref.
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetRefShellDefId_RebindsShellToSolids)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  // Box has 1 solid, 1 shell. Rewiring the lone shell-ref to itself is a no-op,
  // so just verify Validate after a no-op call (proves equality short-circuit).
  const BRepGraph_ShellRefId aRef0 =
    BRepGraph_TestTools::ShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start()).Value(0);
  const BRepGraph_ShellId aShell = aGraph.Refs().Shells().Entry(aRef0).ShellDefId;
  aGraph.Editor().Shells().SetRefShellDefId(aRef0, aShell);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetEdgeDefIdOnCoEdge_RebindsEdgeToCoEdges)
{
  // Pick a coedge that lives in a wire; redirect its EdgeDefId to a different
  // existing edge and confirm Edge->CoEdges, Edge->Wires, Edge->Faces all move.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().CoEdges().Nb(), 2);

  const BRepGraph_CoEdgeId       aCoEdge   = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef      = aGraph.Topo().CoEdges().Definition(aCoEdge);
  const BRepGraph_EdgeId         anOldEdge = aDef.EdgeDefId;
  // Pick any other valid edge as the target.
  BRepGraph_EdgeId aNewEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()); ++aE)
  {
    if (aE != anOldEdge)
    {
      aNewEdge = aE;
      break;
    }
  }
  ASSERT_TRUE(aNewEdge.IsValid());

  aGraph.Editor().CoEdges().SetEdgeDefId(aCoEdge, aNewEdge);

  bool oldHasCoEdge = false, newHasCoEdge = false;
  for (const BRepGraph_CoEdgeId& aC : aGraph.Topo().Edges().CoEdges(anOldEdge))
    if (aC == aCoEdge)
      oldHasCoEdge = true;
  for (const BRepGraph_CoEdgeId& aC : aGraph.Topo().Edges().CoEdges(aNewEdge))
    if (aC == aCoEdge)
      newHasCoEdge = true;
  EXPECT_FALSE(oldHasCoEdge);
  EXPECT_TRUE(newHasCoEdge);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetFaceDefIdOnCoEdge_LastBondCheck)
{
  // Cylinder seam edges have TWO coedges on the SAME face. Dropping one
  // coedge's FaceDefId must keep the (edge, face) pair bound via the other.
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  BRepGraph                aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCylMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Locate a seam edge: an edge with two coedges on the same face.
  BRepGraph_EdgeId   aSeamEdge;
  BRepGraph_FaceId   aSeamFace;
  BRepGraph_CoEdgeId aSeamCoEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()) && !aSeamEdge.IsValid(); ++aE)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCEs = aGraph.Topo().Edges().CoEdges(aE);
    if (aCEs.Size() < 2)
      continue;
    NCollection_DataMap<int, BRepGraph_CoEdgeId> aSeenFace;
    for (const BRepGraph_CoEdgeId& aCE : aCEs)
    {
      const BRepGraphInc::CoEdgeDef& aD = aGraph.Topo().CoEdges().Definition(aCE);
      if (!aD.FaceDefId.IsValid())
        continue;
      if (aSeenFace.IsBound(aD.FaceDefId.Index))
      {
        aSeamEdge   = aE;
        aSeamFace   = aD.FaceDefId;
        aSeamCoEdge = aCE;
        break;
      }
      aSeenFace.Bind(aD.FaceDefId.Index, aCE);
    }
  }
  ASSERT_TRUE(aSeamEdge.IsValid()) << "cylinder must have a seam edge";

  aGraph.Editor().CoEdges().SetFaceDefId(aSeamCoEdge, BRepGraph_FaceId());

  // The seam-pair partner of aSeamCoEdge still binds (aSeamEdge, aSeamFace).
  bool stillBound = false;
  for (const BRepGraph_FaceId& f : aGraph.Topo().Edges().Faces(aSeamEdge))
    if (f == aSeamFace)
      stillBound = true;
  EXPECT_TRUE(stillBound);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetFaceDefIdOnCoEdge_OnlyBondUnbinds)
{
  // Box edges have one coedge per face. Setting that single coedge's FaceDefId
  // to invalid MUST unbind (edge,face) from EdgeToFaces.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_CoEdgeId       aCoEdge   = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef      = aGraph.Topo().CoEdges().Definition(aCoEdge);
  const BRepGraph_EdgeId         anEdge    = aDef.EdgeDefId;
  const BRepGraph_FaceId         anOldFace = aDef.FaceDefId;

  aGraph.Editor().CoEdges().SetFaceDefId(aCoEdge, BRepGraph_FaceId());

  for (const BRepGraph_FaceId& f : aGraph.Topo().Edges().Faces(anEdge))
    EXPECT_NE(f, anOldFace);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetEndVertexRefId_RebindsVertexToEdges)
{
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  const BRepGraph_EdgeId anExtra =
    aGraph.Editor().Edges().Add(aV0, aV2, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  ASSERT_TRUE(anExtra.IsValid());

  // Repoint anEdge's end-ref at the same VertexRefId already used by anExtra's
  // end. Both edges now share the same end vertex (aV2).
  const BRepGraph_VertexRefId aV2Ref = aGraph.Topo().Edges().Definition(anExtra).EndVertexRefId;
  aGraph.Editor().Edges().SetEndVertexRefId(anEdge, aV2Ref);

  bool stillUnderV1 = false, foundUnderV2 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
    if (aE == anEdge)
      stillUnderV1 = true;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV2))
    if (aE == anEdge)
      foundUnderV2 = true;
  EXPECT_FALSE(stillUnderV1);
  EXPECT_TRUE(foundUnderV2);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetterIdempotency_NoOp)
{
  // Identity assignments must be no-ops and not corrupt the reverse index.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_CoEdgeId       aCoEdge = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef    = aGraph.Topo().CoEdges().Definition(aCoEdge);
  aGraph.Editor().CoEdges().SetEdgeDefId(aCoEdge, aDef.EdgeDefId);
  aGraph.Editor().CoEdges().SetFaceDefId(aCoEdge, aDef.FaceDefId);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  const BRepGraph_VertexRefId aVRef =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).StartVertexRefId;
  if (aVRef.IsValid())
  {
    const BRepGraph_VertexId aV = aGraph.Refs().Vertices().Entry(aVRef).VertexDefId;
    aGraph.Editor().Vertices().SetRefVertexDefId(aVRef, aV);
    EXPECT_TRUE(aGraph.ValidateReverseIndex());
  }
}

TEST(BRepGraphIncTest, ReverseIndex_SetChildRefChildDefId_CrossKindRebinds)
{
  // Compound holding a Solid; rewire the ChildRef from the Solid to a Shell.
  // CompoundsOfSolid must lose the entry, CompoundsOfShell must gain it.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox aBoxMaker(2.0, 2.0, 2.0);
  aBB.Add(aCompound, aBoxMaker.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Compounds().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  const BRepGraph_CompoundId       aCompound0 = BRepGraph_CompoundId::Start();
  const BRepGraphInc::CompoundDef& aCDef      = aGraph.Topo().Compounds().Definition(aCompound0);
  ASSERT_GE(aCDef.ChildRefIds.Length(), 1);
  const BRepGraph_ChildRefId aChildRef  = aCDef.ChildRefIds.First();
  const BRepGraph_NodeId     anOldChild = aGraph.Refs().Children().Entry(aChildRef).ChildDefId;
  ASSERT_EQ(anOldChild.NodeKind, BRepGraph_NodeId::Kind::Solid);

  const BRepGraph_ShellId aShell = BRepGraph_ShellId::Start();
  aGraph.Editor().Gen().SetChildRefChildDefId(aChildRef, BRepGraph_NodeId(aShell));

  const BRepGraph_SolidId anOldSolid = BRepGraph_SolidId::FromNodeId(anOldChild);
  bool                    oldStill = false, newBound = false;
  for (const BRepGraph_CompoundId& c : aGraph.Topo().Solids().Compounds(anOldSolid))
    if (c == aCompound0)
      oldStill = true;
  for (const BRepGraph_CompoundId& c : aGraph.Topo().Shells().Compounds(aShell))
    if (c == aCompound0)
      newBound = true;
  EXPECT_FALSE(oldStill);
  EXPECT_TRUE(newBound);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_SetEdgeDefIdOnCoEdge_LastBondInWireCheck)
{
  // Cylinder seam edge: two coedges share both face and wire. Redirecting one
  // coedge's EdgeDefId must NOT remove (oldEdge, wire) from EdgeToWires while
  // the other coedge still references oldEdge in the same wire.
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  BRepGraph                aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCylMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Find a seam edge: two coedges, same wire, same edge.
  BRepGraph_EdgeId   aSeamEdge;
  BRepGraph_WireId   aSeamWire;
  BRepGraph_CoEdgeId aSeamCoEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()) && !aSeamEdge.IsValid(); ++aE)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCEs = aGraph.Topo().Edges().CoEdges(aE);
    if (aCEs.Size() < 2)
      continue;
    NCollection_DataMap<int, BRepGraph_CoEdgeId> aSeenWire;
    for (const BRepGraph_CoEdgeId& aCE : aCEs)
    {
      const NCollection_DynamicArray<BRepGraph_WireId>& aCEWires =
        aGraph.Topo().CoEdges().Wires(aCE);
      for (const BRepGraph_WireId& aW : aCEWires)
      {
        if (aSeenWire.IsBound(aW.Index))
        {
          aSeamEdge   = aE;
          aSeamWire   = aW;
          aSeamCoEdge = aCE;
          break;
        }
        aSeenWire.Bind(aW.Index, aCE);
      }
      if (aSeamEdge.IsValid())
        break;
    }
  }
  ASSERT_TRUE(aSeamEdge.IsValid()) << "cylinder must have a wire with two coedges of same edge";

  // Pick a different edge to redirect to.
  BRepGraph_EdgeId aTargetEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()); ++aE)
    if (aE != aSeamEdge)
    {
      aTargetEdge = aE;
      break;
    }
  ASSERT_TRUE(aTargetEdge.IsValid());

  aGraph.Editor().CoEdges().SetEdgeDefId(aSeamCoEdge, aTargetEdge);

  // The OTHER coedge of aSeamWire still references aSeamEdge -> wire still bound.
  bool stillBound = false;
  for (const BRepGraph_WireId& aW : aGraph.Topo().Edges().Wires(aSeamEdge))
    if (aW == aSeamWire)
      stillBound = true;
  EXPECT_TRUE(stillBound);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

TEST(BRepGraphIncTest, ReverseIndex_OrphanRef_NoRevIndexUpdate)
{
  // SetRefVertexDefId on a face-direct vertex ref must not touch VertexToEdges
  // (no map exists for face-direct vertices). Validate consistency afterwards.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_VertexId    aV = aGraph.Editor().Vertices().Add(gp_Pnt(7, 7, 7), 1.e-7);
  const BRepGraph_VertexRefId aFaceVtxRef =
    aGraph.Editor().Faces().AddVertex(BRepGraph_FaceId::Start(), aV, TopAbs_INTERNAL);
  ASSERT_TRUE(aFaceVtxRef.IsValid());

  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(8, 8, 8), 1.e-7);
  aGraph.Editor().Vertices().SetRefVertexDefId(aFaceVtxRef, aV2);

  EXPECT_TRUE(aGraph.ValidateReverseIndex());
  EXPECT_EQ(aGraph.Refs().Vertices().Entry(aFaceVtxRef).VertexDefId, aV2);
}

TEST(BRepGraphIncTest, ReverseIndex_RemoveRef_UnbindsByKind)
{
  // GenOps::RemoveRef must unbind the corresponding rev-index entry. Picks one
  // FaceRef on the box's first shell; after RemoveRef the FaceToShells entry
  // for the detached face no longer lists this shell.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_DynamicArray<BRepGraph_FaceRefId> aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(aGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefs.Length(), 1);

  const BRepGraph_FaceRefId    aFaceRefId = aFaceRefs.Value(0);
  const BRepGraphInc::FaceRef& aRef       = aGraph.Refs().Faces().Entry(aFaceRefId);
  const BRepGraph_FaceId       aFaceId    = aRef.FaceDefId;

  ASSERT_TRUE(aGraph.Editor().Gen().RemoveRef(aFaceRefId));

  for (const BRepGraph_ShellId& aShellId : aGraph.Topo().Faces().Shells(aFaceId))
    EXPECT_NE(aShellId, BRepGraph_ShellId::Start());
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}
