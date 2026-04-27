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
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <BRep_Builder.hxx>
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

  const BRepGraph_FaceId aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);
  BRepGraph aResultGraph = BRepGraph_Transform::TransformNode(aGraph, aFaceNode, aTrsf, true);
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

TEST(BRepGraph_TransformTest, MoveRef_FaceRef_LocationComposed)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // The solid has shell refs; grab the first one.
  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  ASSERT_GE(aGraph.Topo().Solids().Definition(aSolidId).ShellRefIds.Length(), 1);
  const BRepGraph_ShellRefId aShellRef =
    aGraph.Topo().Solids().Definition(aSolidId).ShellRefIds.Value(0);

  // Verify that the location starts as identity.
  EXPECT_TRUE(aGraph.Refs().Shells().Entry(aShellRef).LocalLocation.IsIdentity());

  const double aDx = 42.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, 0.0, 0.0));

  const bool aOk = BRepGraph_Transform::MoveRef(aGraph, BRepGraph_RefId(aShellRef), aTrsf);
  EXPECT_TRUE(aOk);

  const TopLoc_Location& aLoc = aGraph.Refs().Shells().Entry(aShellRef).LocalLocation;
  EXPECT_FALSE(aLoc.IsIdentity());
  EXPECT_NEAR(aLoc.Transformation().Value(1, 4), aDx, Precision::Confusion());

  // A second MoveRef composes (doubles the translation).
  BRepGraph_Transform::MoveRef(aGraph, BRepGraph_RefId(aShellRef), aTrsf);
  const TopLoc_Location& aLoc2 = aGraph.Refs().Shells().Entry(aShellRef).LocalLocation;
  EXPECT_NEAR(aLoc2.Transformation().Value(1, 4), 2.0 * aDx, Precision::Confusion());
}

TEST(BRepGraph_TransformTest, MoveRef_ScaleRejected)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_SolidId    aSolidId  = BRepGraph_SolidId::Start();
  const BRepGraph_ShellRefId aShellRef =
    aGraph.Topo().Solids().Definition(aSolidId).ShellRefIds.Value(0);

  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(), 2.0); // scale factor != 1

  const bool aOk = BRepGraph_Transform::MoveRef(aGraph, BRepGraph_RefId(aShellRef), aScale);
  EXPECT_FALSE(aOk);
  // Location must remain unchanged (identity).
  EXPECT_TRUE(aGraph.Refs().Shells().Entry(aShellRef).LocalLocation.IsIdentity());
}

TEST(BRepGraph_TransformTest, TransformNode_FaceKind_VertexPointsShifted)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const double aDx = 5.0, aDy = 10.0, aDz = 15.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  const BRepGraph_FaceId aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);

  BRepGraph aResult = BRepGraph_Transform::TransformNode(aGraph, aFaceNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), 1);

  const int aNbV = aResult.Topo().Vertices().Nb();
  ASSERT_GT(aNbV, 0);
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.X(), anOrig.X() + aDx, Precision::Confusion());
    EXPECT_NEAR(aTrans.Y(), anOrig.Y() + aDy, Precision::Confusion());
    EXPECT_NEAR(aTrans.Z(), anOrig.Z() + aDz, Precision::Confusion());
  }
}

TEST(BRepGraph_TransformTest, TransformNode_ShellKind)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  const BRepGraph_ShellId aShellId = BRepGraph_ShellId::Start();
  const BRepGraph_NodeId  aShellNode(BRepGraph_NodeId::Kind::Shell, aShellId.Index);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));

  BRepGraph aResult = BRepGraph_Transform::TransformNode(aGraph, aShellNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  // The shell copy should have the same number of faces as the source shell.
  EXPECT_EQ(aResult.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  // All vertices must be shifted by the translation.
  const int aNbV = aGraph.Topo().Vertices().Nb();
  EXPECT_EQ(aResult.Topo().Vertices().Nb(), aNbV);
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.X(), anOrig.X() + 100.0, Precision::Confusion());
    EXPECT_NEAR(aTrans.Y(), anOrig.Y(), Precision::Confusion());
    EXPECT_NEAR(aTrans.Z(), anOrig.Z(), Precision::Confusion());
  }
}

TEST(BRepGraph_TransformTest, TransformNode_SolidKind)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 1);

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_NodeId  aSolidNode(BRepGraph_NodeId::Kind::Solid, aSolidId.Index);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 50.0, 0.0));

  BRepGraph aResult = BRepGraph_Transform::TransformNode(aGraph, aSolidNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_EQ(aResult.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Shells().Nb(), aGraph.Topo().Shells().Nb());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  const int aNbV = aGraph.Topo().Vertices().Nb();
  EXPECT_EQ(aResult.Topo().Vertices().Nb(), aNbV);
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.Y(), anOrig.Y() + 50.0, Precision::Confusion());
  }
}

TEST(BRepGraph_TransformTest, TransformNode_VertexKind)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes = BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Vertices().Nb(), 1);

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  const BRepGraph_NodeId   aVertexNode(BRepGraph_NodeId::Kind::Vertex, aVertexId.Index);
  const gp_Pnt             anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  BRepGraph aResult = BRepGraph_Transform::TransformNode(aGraph, aVertexNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_EQ(aResult.Topo().Vertices().Nb(), 1);
  const gp_Pnt aTransPt = BRepGraph_Tool::Vertex::Pnt(aResult, BRepGraph_VertexId::Start());
  EXPECT_NEAR(aTransPt.X(), anOrigPt.X() + 1.0, Precision::Confusion());
  EXPECT_NEAR(aTransPt.Y(), anOrigPt.Y() + 2.0, Precision::Confusion());
  EXPECT_NEAR(aTransPt.Z(), anOrigPt.Z() + 3.0, Precision::Confusion());
}

TEST(BRepGraph_TransformTest, TransformNode_CompoundKind)
{
  // ensureCompound must walk children, transform each, and preserve count.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox aBox1(5.0, 5.0, 5.0);
  BRepPrimAPI_MakeBox aBox2(3.0, 3.0, 3.0);
  aBB.Add(aCompound, aBox1.Shape());
  aBB.Add(aCompound, aBox2.Shape());

  BRepGraph aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().Compounds().Nb(), 1);

  const BRepGraph_CompoundId aCompoundId = BRepGraph_CompoundId::Start();
  const BRepGraph_NodeId     aCompoundNode(BRepGraph_NodeId::Kind::Compound, aCompoundId.Index);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

  BRepGraph aResult =
    BRepGraph_Transform::TransformNode(aGraph, aCompoundNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_EQ(aResult.Topo().Compounds().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Solids().Nb(), aGraph.Topo().Solids().Nb());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  const int aNbV = aGraph.Topo().Vertices().Nb();
  EXPECT_EQ(aResult.Topo().Vertices().Nb(), aNbV);
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.X(), anOrig.X() + 20.0, Precision::Confusion());
    EXPECT_NEAR(aTrans.Y(), anOrig.Y(), Precision::Confusion());
  }
}

TEST(BRepGraph_TransformTest, TransformNode_CompSolidKind)
{
  // ensureCompSolid must walk solid refs and transform each.
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);
  BRepPrimAPI_MakeBox aBoxMaker(4.0, 4.0, 4.0);
  aBB.Add(aCompSolid, aBoxMaker.Shape());

  BRepGraph aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aCompSolid);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().CompSolids().Nb(), 1);

  const BRepGraph_CompSolidId aCompSolidId = BRepGraph_CompSolidId::Start();
  const BRepGraph_NodeId      aCompSolidNode(BRepGraph_NodeId::Kind::CompSolid,
                                        aCompSolidId.Index);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 7.0));

  BRepGraph aResult =
    BRepGraph_Transform::TransformNode(aGraph, aCompSolidNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_EQ(aResult.Topo().CompSolids().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Solids().Nb(), aGraph.Topo().Solids().Nb());

  const int aNbV = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.Z(), anOrig.Z() + 7.0, Precision::Confusion());
  }
}

TEST(BRepGraph_TransformTest, TransformNode_NegativeScale_VertexPointsMirrored)
{
  // Smoke-tests the negative-scale geometry path through TransformNode: every
  // vertex point should be mirrored about the origin and the result graph must
  // remain coherent (same face/edge/vertex counts, valid IsDone).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  BRepGraph           aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_NodeId  aSolidNode(BRepGraph_NodeId::Kind::Solid, aSolidId.Index);

  gp_Trsf aTrsf;
  aTrsf.SetMirror(gp_Pnt(0.0, 0.0, 0.0));

  BRepGraph aResult =
    BRepGraph_Transform::TransformNode(aGraph, aSolidNode, aTrsf, true, false);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aResult.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  const int aNbV = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    const gp_Pnt anOrig = BRepGraph_Tool::Vertex::Pnt(aGraph, aVId);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(aResult, aVId);
    EXPECT_NEAR(aTrans.X(), -anOrig.X(), Precision::Confusion());
    EXPECT_NEAR(aTrans.Y(), -anOrig.Y(), Precision::Confusion());
    EXPECT_NEAR(aTrans.Z(), -anOrig.Z(), Precision::Confusion());
  }
}
