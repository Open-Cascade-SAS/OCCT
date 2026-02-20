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

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepBuilderAPI_CopyTest, CopyIsValid)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepBuilderAPI_Copy aCopier(aBox);
  const TopoDS_Shape& aCopy = aCopier.Shape();
  ASSERT_FALSE(aCopy.IsNull()) << "Copied shape is null";

  BRepCheck_Analyzer anAnalyzer(aCopy);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Copied shape should be valid";
}

TEST(BRepBuilderAPI_CopyTest, CopyVolume)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  GProp_GProps aOriginalProps;
  BRepGProp::VolumeProperties(aBox, aOriginalProps);
  double anOriginalVolume = aOriginalProps.Mass();

  BRepBuilderAPI_Copy aCopier(aBox);
  const TopoDS_Shape& aCopy = aCopier.Shape();
  ASSERT_FALSE(aCopy.IsNull()) << "Copied shape is null";

  GProp_GProps aCopyProps;
  BRepGProp::VolumeProperties(aCopy, aCopyProps);
  double aCopyVolume = aCopyProps.Mass();

  EXPECT_NEAR(aCopyVolume, anOriginalVolume, Precision::Confusion())
    << "Copy volume should match original";
}

TEST(BRepBuilderAPI_CopyTest, CopyIsDistinct)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepBuilderAPI_Copy aCopier(aBox);
  const TopoDS_Shape& aCopy = aCopier.Shape();
  ASSERT_FALSE(aCopy.IsNull()) << "Copied shape is null";

  EXPECT_FALSE(aCopy.IsEqual(aBox))
    << "Copy should not be equal to original (different TShape)";
}

TEST(BRepBuilderAPI_CopyTest, CopyGeomTrue)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepBuilderAPI_Copy aCopier(aBox, true);
  const TopoDS_Shape& aCopy = aCopier.Shape();
  ASSERT_FALSE(aCopy.IsNull()) << "Copied shape is null";

  GProp_GProps aOriginalProps;
  BRepGProp::VolumeProperties(aBox, aOriginalProps);

  GProp_GProps aCopyProps;
  BRepGProp::VolumeProperties(aCopy, aCopyProps);

  EXPECT_NEAR(aCopyProps.Mass(), aOriginalProps.Mass(), Precision::Confusion())
    << "Deep copy volume should match original";
}

TEST(BRepBuilderAPI_CopyTest, CopyGeomFalse)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepBuilderAPI_Copy aCopier(aBox, false);
  const TopoDS_Shape& aCopy = aCopier.Shape();
  ASSERT_FALSE(aCopy.IsNull()) << "Copied shape is null";

  BRepCheck_Analyzer anAnalyzer(aCopy);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Shallow copy should produce a valid shape";
}
