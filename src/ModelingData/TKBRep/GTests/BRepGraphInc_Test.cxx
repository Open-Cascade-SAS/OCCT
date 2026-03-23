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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

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

  // Build legacy graph.
  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Build incidence storage.
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.IsDone);

  // Entity counts must match Def counts.
  EXPECT_EQ(aStorage.Vertices.Length(), aGraph.Defs().NbVertices());
  EXPECT_EQ(aStorage.Edges.Length(), aGraph.Defs().NbEdges());
  EXPECT_EQ(aStorage.Wires.Length(), aGraph.Defs().NbWires());
  EXPECT_EQ(aStorage.Faces.Length(), aGraph.Defs().NbFaces());
  EXPECT_EQ(aStorage.Shells.Length(), aGraph.Defs().NbShells());
  EXPECT_EQ(aStorage.Solids.Length(), aGraph.Defs().NbSolids());
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
  ASSERT_TRUE(aStorage.IsDone);

  EXPECT_EQ(aStorage.Vertices.Length(), aGraph.Defs().NbVertices());
  EXPECT_EQ(aStorage.Edges.Length(), aGraph.Defs().NbEdges());
  EXPECT_EQ(aStorage.Wires.Length(), aGraph.Defs().NbWires());
  EXPECT_EQ(aStorage.Faces.Length(), aGraph.Defs().NbFaces());
  EXPECT_EQ(aStorage.Shells.Length(), aGraph.Defs().NbShells());
  EXPECT_EQ(aStorage.Solids.Length(), aGraph.Defs().NbSolids());
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
  ASSERT_TRUE(aStorage.IsDone);

  EXPECT_EQ(aStorage.Vertices.Length(), aGraph.Defs().NbVertices());
  EXPECT_EQ(aStorage.Edges.Length(), aGraph.Defs().NbEdges());
  EXPECT_EQ(aStorage.Wires.Length(), aGraph.Defs().NbWires());
  EXPECT_EQ(aStorage.Faces.Length(), aGraph.Defs().NbFaces());
}

// ============================================================
// Round-trip: area preservation
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  const double anOrigArea = computeArea(aBox);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();
  const double anOrigArea = computeArea(aCyl);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Sphere_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape& aSph = aSphMaker.Shape();
  const double anOrigArea = computeArea(aSph);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aSph, false);
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
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
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  const double anOrigVol = computeVolume(aBox);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_VolumePreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();
  const double anOrigVol = computeVolume(aCyl);

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
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
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
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
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
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
  ASSERT_TRUE(aStorage.IsDone);

  // Every edge must appear in at least one wire.
  for (int anEdgeIdx = 0; anEdgeIdx < aStorage.Edges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<int>* aWires = aStorage.ReverseIdx.WiresOfEdge(anEdgeIdx);
    EXPECT_TRUE(aWires != nullptr) << "Edge " << anEdgeIdx << " not in any wire";
    if (aWires != nullptr)
      EXPECT_GE(aWires->Length(), 1);
  }
}

TEST(BRepGraphIncTest, Box_ReverseIndex_EdgesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.IsDone);

  // Every edge must appear in at least one face (via EdgeFaceGeom).
  for (int anEdgeIdx = 0; anEdgeIdx < aStorage.Edges.Length(); ++anEdgeIdx)
  {
    if (aStorage.Edges.Value(anEdgeIdx).IsDegenerate)
      continue;
    const NCollection_Vector<int>* aFaces = aStorage.ReverseIdx.FacesOfEdge(anEdgeIdx);
    EXPECT_TRUE(aFaces != nullptr) << "Edge " << anEdgeIdx << " not in any face";
    if (aFaces != nullptr)
      EXPECT_GE(aFaces->Length(), 1);
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
  ASSERT_TRUE(aSerial.IsDone);

  BRepGraphInc_Storage aParallel;
  BRepGraphInc_Populate::Perform(aParallel, aBox, true);
  ASSERT_TRUE(aParallel.IsDone);

  EXPECT_EQ(aParallel.Vertices.Length(), aSerial.Vertices.Length());
  EXPECT_EQ(aParallel.Edges.Length(), aSerial.Edges.Length());
  EXPECT_EQ(aParallel.Wires.Length(), aSerial.Wires.Length());
  EXPECT_EQ(aParallel.Faces.Length(), aSerial.Faces.Length());
  EXPECT_EQ(aParallel.Shells.Length(), aSerial.Shells.Length());
  EXPECT_EQ(aParallel.Solids.Length(), aSerial.Solids.Length());
}

// ============================================================
// Null shape handling
// ============================================================

TEST(BRepGraphIncTest, NullShape_NoEntities)
{
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, TopoDS_Shape(), false);
  EXPECT_FALSE(aStorage.IsDone);
  EXPECT_EQ(aStorage.Vertices.Length(), 0);
  EXPECT_EQ(aStorage.Edges.Length(), 0);
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
  ASSERT_TRUE(aStorage.IsDone);

  // Two solids, two shells.
  EXPECT_EQ(aStorage.Solids.Length(), 2);
  EXPECT_EQ(aStorage.Shells.Length(), 2);
  EXPECT_EQ(aStorage.Compounds.Length(), 1);
  EXPECT_EQ(aStorage.Faces.Length(), 12);

  // Round-trip reconstruct via compound.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Compound(0));
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}

// ============================================================
// EdgeFaceGeom row count consistency
// ============================================================

TEST(BRepGraphIncTest, Box_EdgeFaceGeomCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aBox, false);
  ASSERT_TRUE(aStorage.IsDone);

  // A box has 12 edges, each shared by 2 faces => 24 EdgeFaceGeom rows.
  // (No seam edges on a box.)
  EXPECT_EQ(aStorage.EdgeFaceGeoms.Length(), 24);
}

TEST(BRepGraphIncTest, Cylinder_HasSeamEdges)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCyl, false);
  ASSERT_TRUE(aStorage.IsDone);

  // A cylinder has seam edges: EdgeFaceGeom rows with same (edgeIdx,faceIdx)
  // but opposite orientation.  Count distinct seam pairs.
  int aSeamPairCount = 0;
  for (int i = 0; i < aStorage.EdgeFaceGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceGeom& aRow1 = aStorage.EdgeFaceGeoms.Value(i);
    if (aRow1.EdgeOrientation != TopAbs_FORWARD)
      continue;
    for (int j = i + 1; j < aStorage.EdgeFaceGeoms.Length(); ++j)
    {
      const BRepGraphInc::EdgeFaceGeom& aRow2 = aStorage.EdgeFaceGeoms.Value(j);
      if (aRow1.EdgeIdx == aRow2.EdgeIdx && aRow1.FaceIdx == aRow2.FaceIdx
          && aRow2.EdgeOrientation == TopAbs_REVERSED)
      {
        ++aSeamPairCount;
        break;
      }
    }
  }
  // A cylinder has 1 seam edge on its lateral face.
  EXPECT_GE(aSeamPairCount, 1) << "Cylinder should have at least 1 seam edge pair";
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
  ASSERT_TRUE(aStorage.IsDone);

  // A sphere has degenerate edges at the poles (no 3D curve, collapsed to a point).
  int aDegenerateCount = 0;
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
  {
    const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.Value(i);
    if (anEdge.IsDegenerate)
    {
      ++aDegenerateCount;
      EXPECT_TRUE(anEdge.Curve3d.IsNull()) << "Degenerate edge " << i << " should have no 3D curve";
    }
  }
  EXPECT_GE(aDegenerateCount, 2) << "Sphere should have at least 2 degenerate edges (poles)";

  // Round-trip: reconstructed sphere area must still match.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Solid(0));
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
  ASSERT_TRUE(aStorage.IsDone);

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aStorage, BRepGraph_NodeId::Compound(0));
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}
