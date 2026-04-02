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
#include <TopoDS_Iterator.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

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
  aGraph.Build(aBox);
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
  aGraph.Build(aCyl);
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
  aGraph.Build(aSph);
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

  aStorage.AppendSurfaceRep().Id       = BRepGraph_SurfaceRepId(0);
  aStorage.AppendCurve3DRep().Id       = BRepGraph_Curve3DRepId(0);
  aStorage.AppendCurve2DRep().Id       = BRepGraph_Curve2DRepId(0);
  aStorage.AppendTriangulationRep().Id = BRepGraph_TriangulationRepId(0);
  aStorage.AppendPolygon3DRep().Id     = BRepGraph_Polygon3DRepId(0);
  aStorage.AppendPolygon2DRep().Id     = BRepGraph_Polygon2DRepId(0);
  aStorage.AppendPolygonOnTriRep().Id  = BRepGraph_PolygonOnTriRepId(0);

  EXPECT_EQ(aStorage.NbActiveSurfaces(), 1);
  EXPECT_EQ(aStorage.NbActiveCurves3D(), 1);
  EXPECT_EQ(aStorage.NbActiveCurves2D(), 1);
  EXPECT_EQ(aStorage.NbActiveTriangulations(), 1);
  EXPECT_EQ(aStorage.NbActivePolygons3D(), 1);
  EXPECT_EQ(aStorage.NbActivePolygons2D(), 1);
  EXPECT_EQ(aStorage.NbActivePolygonsOnTri(), 1);

  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_SurfaceRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Curve3DRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Curve2DRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_TriangulationRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Polygon3DRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_Polygon2DRepId(0)));
  EXPECT_TRUE(aStorage.MarkRemovedRep(BRepGraph_PolygonOnTriRepId(0)));

  EXPECT_TRUE(aStorage.SurfaceRep(BRepGraph_SurfaceRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.Curve3DRep(BRepGraph_Curve3DRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.Curve2DRep(BRepGraph_Curve2DRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.TriangulationRep(BRepGraph_TriangulationRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.Polygon3DRep(BRepGraph_Polygon3DRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.Polygon2DRep(BRepGraph_Polygon2DRepId(0)).IsRemoved);
  EXPECT_TRUE(aStorage.PolygonOnTriRep(BRepGraph_PolygonOnTriRepId(0)).IsRemoved);

  EXPECT_EQ(aStorage.NbActiveSurfaces(), 0);
  EXPECT_EQ(aStorage.NbActiveCurves3D(), 0);
  EXPECT_EQ(aStorage.NbActiveCurves2D(), 0);
  EXPECT_EQ(aStorage.NbActiveTriangulations(), 0);
  EXPECT_EQ(aStorage.NbActivePolygons3D(), 0);
  EXPECT_EQ(aStorage.NbActivePolygons2D(), 0);
  EXPECT_EQ(aStorage.NbActivePolygonsOnTri(), 0);

  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_SurfaceRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Curve3DRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Curve2DRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_TriangulationRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Polygon3DRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_Polygon2DRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_PolygonOnTriRepId(0)));
  EXPECT_FALSE(aStorage.MarkRemovedRep(BRepGraph_RepId()));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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
    const NCollection_Vector<BRepGraph_WireId>* aWires =
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
    const NCollection_Vector<BRepGraph_FaceId>* aFaces =
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
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId(0));
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
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
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
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
    if (aCoEdgeIdxs == nullptr)
      continue;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aStorage.CoEdge(aCoEdgeId);
      if (aCE.Sense == TopAbs_FORWARD && aCE.SeamPairId.IsValid())
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
    const NCollection_Vector<BRepGraph_FaceId>* aFaces =
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
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_SolidId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_FaceId(0));
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

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId(0));
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_FaceId(0));
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

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId(0));
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

  const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId(0));
  EXPECT_GE(BRepGraph_TestTools::CountWireRefsOfFace(aStorage, BRepGraph_FaceId(0)), 1);
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

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_CompoundId(0));
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
