// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <cmath>

#include <gtest/gtest.h>

TEST(BRepOffsetAPI_MakeThickSolidTest, HollowBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Get the first face to remove.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFaceToRemove = TopoDS::Face(anExp.Current());

  NCollection_List<TopoDS_Shape> aFacesToRemove;
  aFacesToRemove.Append(aFaceToRemove);

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidByJoin(aBox, aFacesToRemove, -1.0, Precision::Confusion());
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  EXPECT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

TEST(BRepOffsetAPI_MakeThickSolidTest, HollowBoxVolume)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  GProp_GProps aOrigProps;
  BRepGProp::VolumeProperties(aBox, aOrigProps);
  const double anOrigVolume = aOrigProps.Mass();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFaceToRemove = TopoDS::Face(anExp.Current());

  NCollection_List<TopoDS_Shape> aFacesToRemove;
  aFacesToRemove.Append(aFaceToRemove);

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidByJoin(aBox, aFacesToRemove, -1.0, Precision::Confusion());
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aHollowProps;
  BRepGProp::VolumeProperties(aResult, aHollowProps);
  const double aHollowVolume = aHollowProps.Mass();

  // Hollowed box must have less volume than the original solid.
  EXPECT_LT(aHollowVolume, anOrigVolume);
}

TEST(BRepOffsetAPI_MakeThickSolidTest, MakeThickSolidBySimple)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Extract a single face (non-closed shell) for MakeThickSolidBySimple.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidBySimple(aFace, 2.0);
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  EXPECT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

TEST(BRepOffsetAPI_MakeThickSolidTest, ThickSolidLargerVolume)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Extract a single face for MakeThickSolidBySimple.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  // Compute the area of the original face to estimate expected volume.
  GProp_GProps aFaceProps;
  BRepGProp::SurfaceProperties(aFace, aFaceProps);
  const double aFaceArea = aFaceProps.Mass();

  const double                 anOffset = 2.0;
  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidBySimple(aFace, anOffset);
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aResultProps;
  BRepGProp::VolumeProperties(aResult, aResultProps);
  const double aResultVolume = std::abs(aResultProps.Mass());

  // The thickened face should produce a solid with volume > 0.
  // A planar face of area A thickened by offset d produces volume approximately A*d.
  EXPECT_GT(aResultVolume, aFaceArea * anOffset * 0.5);
}
