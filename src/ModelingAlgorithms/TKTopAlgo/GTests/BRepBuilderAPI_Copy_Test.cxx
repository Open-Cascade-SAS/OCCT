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
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
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

  EXPECT_FALSE(aCopy.IsEqual(aBox)) << "Copy should not be equal to original (different TShape)";
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

// caf/nam/A4: CopyShape creates distinct subfaces with matching centers and
// retains the source location when the source is copied after translation.
TEST(BRepBuilderAPI_CopyTest, CafNamedShape_A4_CopyAndTranslate)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(12.0, 13.0, 14.0).Shape();
  BRepBuilderAPI_Copy aCopyBuilder(aBox);
  const TopoDS_Shape aCopy = aCopyBuilder.Shape();
  ASSERT_FALSE(aCopy.IsNull());

  int anOriginalFaceCount = 0;
  int aCopiedFaceCount = 0;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++anOriginalFaceCount;
    GProp_GProps anOriginalProperties;
    BRepGProp::SurfaceProperties(anExplorer.Current(), anOriginalProperties);
    const gp_Pnt anOriginalCenter = anOriginalProperties.CentreOfMass();
    int aMatchingFaces = 0;
    for (TopExp_Explorer aCopyExplorer(aCopy, TopAbs_FACE); aCopyExplorer.More();
         aCopyExplorer.Next())
    {
      GProp_GProps aCopyProperties;
      BRepGProp::SurfaceProperties(aCopyExplorer.Current(), aCopyProperties);
      if (aCopyProperties.CentreOfMass().IsEqual(anOriginalCenter, Precision::Confusion()))
      {
        ++aMatchingFaces;
        EXPECT_FALSE(aCopyExplorer.Current().IsSame(anExplorer.Current()));
      }
    }
    EXPECT_EQ(aMatchingFaces, 1);
  }
  for (TopExp_Explorer anExplorer(aCopy, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aCopiedFaceCount;
  }
  EXPECT_EQ(anOriginalFaceCount, 6);
  EXPECT_EQ(aCopiedFaceCount, anOriginalFaceCount);

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(50.0, 40.0, 30.0));
  const TopoDS_Shape aTranslatedBox = BRepBuilderAPI_Transform(aBox, aTranslation).Shape();
  BRepBuilderAPI_Copy aTranslatedCopyBuilder(aTranslatedBox);
  const TopoDS_Shape aTranslatedCopy = aTranslatedCopyBuilder.Shape();
  ASSERT_FALSE(aTranslatedCopy.IsNull());

  GProp_GProps aTranslatedProperties;
  GProp_GProps aTranslatedCopyProperties;
  BRepGProp::VolumeProperties(aTranslatedBox, aTranslatedProperties);
  BRepGProp::VolumeProperties(aTranslatedCopy, aTranslatedCopyProperties);
  EXPECT_TRUE(aTranslatedProperties.CentreOfMass().IsEqual(
    aTranslatedCopyProperties.CentreOfMass(), Precision::Confusion()));
  EXPECT_TRUE(aTranslatedProperties.CentreOfMass().IsEqual(
    gp_Pnt(56.0, 46.5, 37.0), Precision::Confusion()));
  EXPECT_TRUE(BRepCheck_Analyzer(aTranslatedCopy).IsValid());
}
