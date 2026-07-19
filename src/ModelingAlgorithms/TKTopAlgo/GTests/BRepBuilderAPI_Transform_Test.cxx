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

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepBuilderAPI_TransformTest, Translate)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));

  BRepBuilderAPI_Transform aTransform(aBox, aTrsf);
  const TopoDS_Shape&      aResult = aTransform.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Transformed shape is null";

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aResult, aProps);
  gp_Pnt aCenterOfMass = aProps.CentreOfMass();

  EXPECT_NEAR(aCenterOfMass.X(), 105.0, Precision::Confusion())
    << "Center of mass X should be shifted by 100";
  EXPECT_NEAR(aCenterOfMass.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCenterOfMass.Z(), 5.0, Precision::Confusion());
}

TEST(BRepBuilderAPI_TransformTest, Rotate)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  gp_Trsf aTrsf;
  gp_Ax1  aZAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  aTrsf.SetRotation(aZAxis, M_PI / 2.0);

  BRepBuilderAPI_Transform aTransform(aBox, aTrsf);
  const TopoDS_Shape&      aResult = aTransform.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Transformed shape is null";

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aResult, aProps);
  gp_Pnt aCenterOfMass = aProps.CentreOfMass();

  EXPECT_NEAR(aCenterOfMass.X(), -5.0, Precision::Confusion())
    << "Center of mass X should be ~-5 after 90deg rotation";
  EXPECT_NEAR(aCenterOfMass.Y(), 5.0, Precision::Confusion())
    << "Center of mass Y should be ~5 after 90deg rotation";
}

TEST(BRepBuilderAPI_TransformTest, Scale)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0.0, 0.0, 0.0), 2.0);

  BRepBuilderAPI_Transform aTransform(aBox, aTrsf);
  const TopoDS_Shape&      aResult = aTransform.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Transformed shape is null";

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aResult, aProps);
  double aVolume = aProps.Mass();

  EXPECT_NEAR(aVolume, 8000.0, Precision::Confusion())
    << "Volume should be 8x original (2^3 * 1000)";
}

TEST(BRepBuilderAPI_TransformTest, Mirror)
{
  BRepPrimAPI_MakeBox aMakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  gp_Trsf aTrsf;
  gp_Ax2  aYZPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  aTrsf.SetMirror(aYZPlane);

  BRepBuilderAPI_Transform aTransform(aBox, aTrsf);
  const TopoDS_Shape&      aResult = aTransform.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Transformed shape is null";

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aResult, aProps);
  gp_Pnt aCenterOfMass = aProps.CentreOfMass();

  EXPECT_LT(aCenterOfMass.X(), 0.0)
    << "Center of mass X should be negative after mirroring through YZ plane";
}

TEST(BRepBuilderAPI_TransformTest, ShapeValidity)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 50.0, 50.0));

  BRepBuilderAPI_Transform aTransform(aBox, aTrsf);
  const TopoDS_Shape&      aResult = aTransform.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Transformed shape is null";

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Transformed shape should be valid";
}
