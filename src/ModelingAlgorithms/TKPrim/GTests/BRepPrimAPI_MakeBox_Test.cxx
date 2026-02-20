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
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakeBoxTest, UnitBox)
{
  BRepPrimAPI_MakeBox aBox(1.0, 1.0, 1.0);
  TopoDS_Shape        aShape = aBox.Shape();

  ASSERT_TRUE(aBox.IsDone()) << "Unit box creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Unit box shape is not valid";
}

TEST(BRepPrimAPI_MakeBoxTest, TopologyCounts)
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 6) << "Box should have 6 faces";

  int anEdgeCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    anEdgeCount++;
  }
  EXPECT_EQ(anEdgeCount, 24) << "Box should have 24 edge occurrences (12 edges x 2 faces each)";

  int aVertexCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    aVertexCount++;
  }
  EXPECT_EQ(aVertexCount, 48) << "Box should have 48 vertex occurrences (8 vertices x 6 faces)";
}

TEST(BRepPrimAPI_MakeBoxTest, Volume)
{
  const double aWidth  = 3.0;
  const double aHeight = 4.0;
  const double aDepth  = 5.0;

  BRepPrimAPI_MakeBox aBox(aWidth, aHeight, aDepth);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), aWidth * aHeight * aDepth, Precision::Confusion());
}

TEST(BRepPrimAPI_MakeBoxTest, SurfaceArea)
{
  const double aW = 3.0;
  const double aH = 4.0;
  const double aD = 5.0;

  BRepPrimAPI_MakeBox aBox(aW, aH, aD);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aShape, aProps);
  double anExpectedArea = 2.0 * (aW * aH + aW * aD + aH * aD);
  EXPECT_NEAR(aProps.Mass(), anExpectedArea, Precision::Confusion());
}

TEST(BRepPrimAPI_MakeBoxTest, TwoCornerPoints)
{
  gp_Pnt aP1(1.0, 2.0, 3.0);
  gp_Pnt aP2(4.0, 6.0, 8.0);

  BRepPrimAPI_MakeBox aBox(aP1, aP2);
  TopoDS_Shape        aShape = aBox.Shape();

  ASSERT_TRUE(aBox.IsDone()) << "Box from two corner points failed";
  ASSERT_FALSE(aShape.IsNull());

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Box from two points is not valid";

  // Volume = (4-1)*(6-2)*(8-3) = 3*4*5 = 60
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 60.0, Precision::Confusion());
}

TEST(BRepPrimAPI_MakeBoxTest, ShapeValidity)
{
  BRepPrimAPI_MakeBox aBox(100.0, 200.0, 300.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Large box shape is not valid";
}
