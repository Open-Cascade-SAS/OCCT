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

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cmath>
#include <iostream>

#include <gtest/gtest.h>

TEST(BRepGraph_TransformTest, TranslateBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 200.0, 300.0));

  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_TransformTest, TranslateBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());

  // Verify area is preserved by summing individual face areas.
  double    aTransArea = 0.0;
  const int aNbFaces   = aResultGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    TopoDS_Shape aFace = aResultGraph.Shapes().Reconstruct(aFaceId);
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    aTransArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aTransArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_TransformTest, TranslateBox_VertexPointsShifted)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const double aDx = 100.0, aDy = 200.0, aDz = 300.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());
  ASSERT_EQ(aResultGraph.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  // Verify that all vertices have been shifted.
  const int aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);
    const gp_Pnt aTransPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, aVertexId);
    EXPECT_NEAR(aTransPt.X(), anOrigPt.X() + aDx, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " X mismatch";
    EXPECT_NEAR(aTransPt.Y(), anOrigPt.Y() + aDy, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " Y mismatch";
    EXPECT_NEAR(aTransPt.Z(), anOrigPt.Z() + aDz, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " Z mismatch";
  }
}

TEST(BRepGraph_TransformTest, LocationOnly_NoCopyGeom)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const double aDx = 50.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, 0.0, 0.0));

  // theCopyGeom = false: location-only, no geometry modification.
  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, false);
  ASSERT_TRUE(aResultGraph.IsDone());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aResultGraph.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  // Vertex definition points must NOT be modified (location-only mode).
  const int aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);
    const gp_Pnt aGraphPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, aVertexId);
    EXPECT_NEAR(aGraphPt.X(), anOrigPt.X(), Precision::Confusion())
      << "Vertex " << aVertexId.Index << " point should not be modified";
  }

  // Verify the transform is stored on the shape-root OccurrenceRef's LocalLocation.
  ASSERT_GT(aResultGraph.Topo().Products().Nb(), 0);
  const BRepGraph_ProductId       aProductId = BRepGraph_ProductId::Start();
  const BRepGraphInc::ProductDef& aProductDef =
    aResultGraph.Topo().Products().Definition(aProductId);
  ASSERT_GE(aProductDef.OccurrenceRefIds.Length(), 1);
  const BRepGraphInc::OccurrenceRef& aOccRef =
    aResultGraph.Refs().Occurrences().Entry(aProductDef.OccurrenceRefIds.Value(0));
  const TopLoc_Location& aRootLoc = aOccRef.LocalLocation;
  EXPECT_FALSE(aRootLoc.IsIdentity());
  const gp_Trsf aProductTrsf = aRootLoc.Transformation();
  EXPECT_NEAR(aProductTrsf.Value(1, 4), aDx, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(2, 4), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(3, 4), 0.0, Precision::Confusion());

  // Verify that reconstructed solid + LocalLocation produces correct geometry.
  ASSERT_GT(aResultGraph.Topo().Solids().Nb(), 0);
  TopoDS_Shape aTransSolid = aResultGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aTransSolid.IsNull());
  aTransSolid.Location(aRootLoc);

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);

  GProp_GProps aTransProps;
  BRepGProp::SurfaceProperties(aTransSolid, aTransProps);
  EXPECT_NEAR(aTransProps.CentreOfMass().X(), aOrigProps.CentreOfMass().X() + aDx, 1.0);
}

TEST(BRepGraph_TransformTest, TransformSingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));

  BRepGraph aResultGraph =
    BRepGraph_Transform::TransformFace(aGraph, BRepGraph_FaceId::Start(), aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 1);
}

TEST(BRepGraph_TransformTest, CopyMesh_TriangulationNodesTransformed)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  // Manually create a triangulation with known node positions on the first face.
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  occ::handle<Poly_Triangulation> aSrcTri = new Poly_Triangulation(3, 1, false);
  aSrcTri->SetNode(1, gp_Pnt(1.0, 2.0, 3.0));
  aSrcTri->SetNode(2, gp_Pnt(4.0, 5.0, 6.0));
  aSrcTri->SetNode(3, gp_Pnt(7.0, 8.0, 9.0));
  aSrcTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aSrcTri->Deflection(0.1);

  const BRepGraph_TriangulationRepId aTriRepId =
    BRepGraph_Tool::Mesh::CreateTriangulationRep(aGraph, aSrcTri);
  aGraph.Editor().Faces().SetTriangulationRep(aFaceId, aTriRepId);
  BRepGraph_Tool::Mesh::AppendCachedTriangulation(aGraph, aFaceId, aTriRepId);

  const double aDx = 5.0, aDy = 10.0, aDz = 15.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  // Transform with theCopyMesh = true.
  BRepGraph aResult = BRepGraph_Transform::Perform(aGraph, aTrsf, true, true);
  ASSERT_TRUE(aResult.IsDone());

  // The persistent triangulation on the first face must be present and transformed.
  const BRepGraph_TriangulationRepId aNewTriRepId =
    aResult.Topo().Faces().Definition(aFaceId).TriangulationRepId;
  ASSERT_TRUE(aNewTriRepId.IsValid(aResult.Mesh().Poly().NbTriangulations()));

  const occ::handle<Poly_Triangulation>& aNewTri =
    aResult.Mesh().Poly().TriangulationRep(aNewTriRepId).Triangulation;
  ASSERT_FALSE(aNewTri.IsNull());
  ASSERT_EQ(aNewTri->NbNodes(), 3);

  EXPECT_NEAR(aNewTri->Node(1).X(), 1.0 + aDx, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(1).Y(), 2.0 + aDy, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(1).Z(), 3.0 + aDz, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(3).X(), 7.0 + aDx, Precision::Confusion());

  // The source triangulation must not be mutated.
  EXPECT_NEAR(aSrcTri->Node(1).X(), 1.0, Precision::Confusion());

  // Deflection must scale (translation has scale = 1, so no change).
  EXPECT_NEAR(aNewTri->Deflection(), 0.1, Precision::Confusion());
}

TEST(BRepGraph_TransformTest, CopyMesh_False_TriangulationInvalidated)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0, 0, 0));
  aTri->SetNode(2, gp_Pnt(1, 0, 0));
  aTri->SetNode(3, gp_Pnt(0, 1, 0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  const BRepGraph_TriangulationRepId aTriRepId =
    BRepGraph_Tool::Mesh::CreateTriangulationRep(aGraph, aTri);
  aGraph.Editor().Faces().SetTriangulationRep(aFaceId, aTriRepId);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));

  // Default: theCopyMesh = false -> triangulations are discarded.
  BRepGraph aResult = BRepGraph_Transform::Perform(aGraph, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  const BRepGraph_TriangulationRepId aResultTriRepId =
    aResult.Topo().Faces().Definition(aFaceId).TriangulationRepId;
  EXPECT_FALSE(aResultTriRepId.IsValid());
  EXPECT_FALSE(aResult.Mesh().Faces().HasTriangulation(aFaceId));
}
