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
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakeCylinderTest, FullCylinder)
{
  const double aRadius = 5.0;
  const double aHeight = 10.0;
  BRepPrimAPI_MakeCylinder aCylinder(aRadius, aHeight);
  TopoDS_Shape             aShape = aCylinder.Shape();

  ASSERT_TRUE(aCylinder.IsDone()) << "Full cylinder creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Full cylinder shape is not valid";
}

TEST(BRepPrimAPI_MakeCylinderTest, FaceCount)
{
  BRepPrimAPI_MakeCylinder aCylinder(5.0, 10.0);
  TopoDS_Shape             aShape = aCylinder.Shape();
  ASSERT_TRUE(aCylinder.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  // Full cylinder: 1 lateral + 2 caps = 3
  EXPECT_EQ(aFaceCount, 3) << "Full cylinder should have 3 faces (lateral + 2 caps)";
}

TEST(BRepPrimAPI_MakeCylinderTest, Volume)
{
  const double aRadius = 5.0;
  const double aHeight = 10.0;
  BRepPrimAPI_MakeCylinder aCylinder(aRadius, aHeight);
  TopoDS_Shape             aShape = aCylinder.Shape();
  ASSERT_TRUE(aCylinder.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  double anExpectedVolume = M_PI * aRadius * aRadius * aHeight;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, 0.01);
}

TEST(BRepPrimAPI_MakeCylinderTest, PartialCylinder_AngleLimited)
{
  const double aRadius = 5.0;
  const double aHeight = 10.0;
  // Half cylinder: angle = PI
  BRepPrimAPI_MakeCylinder aCylinder(aRadius, aHeight, M_PI);
  TopoDS_Shape             aShape = aCylinder.Shape();

  ASSERT_TRUE(aCylinder.IsDone()) << "Partial cylinder creation failed";
  ASSERT_FALSE(aShape.IsNull());

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Partial cylinder shape is not valid";

  // Partial cylinder should have more faces: 1 lateral + 2 caps + 2 planar sides = 5
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 5) << "Half cylinder should have 5 faces";
}

TEST(BRepPrimAPI_MakeCylinderTest, ShapeValidity)
{
  BRepPrimAPI_MakeCylinder aCylinder(100.0, 200.0);
  TopoDS_Shape             aShape = aCylinder.Shape();
  ASSERT_TRUE(aCylinder.IsDone());

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Large cylinder shape is not valid";
}
