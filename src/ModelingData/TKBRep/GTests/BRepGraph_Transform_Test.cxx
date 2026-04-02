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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
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
  aGraph.Build(aBox);
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
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());

  // Verify area is preserved by summing individual face areas.
  double aTransArea = 0.0;
  for (int aFaceIdx = 0; aFaceIdx < aResultGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    TopoDS_Shape aFace = aResultGraph.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx));
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
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const double aDx = 100.0, aDy = 200.0, aDz = 300.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  BRepGraph aResultGraph = BRepGraph_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());
  ASSERT_EQ(aResultGraph.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  // Verify that all vertices have been shifted.
  for (int anIdx = 0; anIdx < aGraph.Topo().Vertices().Nb(); ++anIdx)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(anIdx));
    const gp_Pnt aTransPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, BRepGraph_VertexId(anIdx));
    EXPECT_NEAR(aTransPt.X(), anOrigPt.X() + aDx, Precision::Confusion())
      << "Vertex " << anIdx << " X mismatch";
    EXPECT_NEAR(aTransPt.Y(), anOrigPt.Y() + aDy, Precision::Confusion())
      << "Vertex " << anIdx << " Y mismatch";
    EXPECT_NEAR(aTransPt.Z(), anOrigPt.Z() + aDz, Precision::Confusion())
      << "Vertex " << anIdx << " Z mismatch";
  }
}

TEST(BRepGraph_TransformTest, LocationOnly_NoCopyGeom)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
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
  for (int anIdx = 0; anIdx < aGraph.Topo().Vertices().Nb(); ++anIdx)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(anIdx));
    const gp_Pnt aGraphPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, BRepGraph_VertexId(anIdx));
    EXPECT_NEAR(aGraphPt.X(), anOrigPt.X(), Precision::Confusion())
      << "Vertex " << anIdx << " point should not be modified";
  }

  // Verify the transform is stored on Product::RootLocation.
  ASSERT_GT(aResultGraph.Topo().Products().Nb(), 0);
  const TopLoc_Location& aRootLoc =
    aResultGraph.Topo().Products().Definition(BRepGraph_ProductId(0)).RootLocation;
  EXPECT_FALSE(aRootLoc.IsIdentity());
  const gp_Trsf aProductTrsf = aRootLoc.Transformation();
  EXPECT_NEAR(aProductTrsf.Value(1, 4), aDx, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(2, 4), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(3, 4), 0.0, Precision::Confusion());

  // Verify that reconstructed solid + RootLocation produces correct geometry.
  ASSERT_GT(aResultGraph.Topo().Solids().Nb(), 0);
  TopoDS_Shape aTransSolid = aResultGraph.Shapes().Reconstruct(BRepGraph_SolidId(0));
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
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));

  BRepGraph aResultGraph =
    BRepGraph_Transform::TransformFace(aGraph, BRepGraph_FaceId(0), aTrsf, true);
  ASSERT_TRUE(aResultGraph.IsDone());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 1);
}
