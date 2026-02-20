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
#include <BRepPrimAPI_MakeWedge.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakeWedgeTest, WedgeFromDimensions)
{
  BRepPrimAPI_MakeWedge aWedge(10.0, 10.0, 10.0, 5.0);
  TopoDS_Shape          aShape = aWedge.Shape();

  ASSERT_TRUE(aWedge.IsDone()) << "Wedge creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Wedge shape is not valid";
}

TEST(BRepPrimAPI_MakeWedgeTest, WedgeFaceCount_Ltx5)
{
  BRepPrimAPI_MakeWedge aWedge(10.0, 10.0, 10.0, 5.0);
  TopoDS_Shape          aShape = aWedge.Shape();
  ASSERT_TRUE(aWedge.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 6) << "Wedge with ltx=5 should have 6 faces";
}

TEST(BRepPrimAPI_MakeWedgeTest, WedgeFaceCount_Ltx0)
{
  BRepPrimAPI_MakeWedge aWedge(10.0, 10.0, 10.0, 0.0);
  TopoDS_Shape          aShape = aWedge.Shape();
  ASSERT_TRUE(aWedge.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 5) << "Wedge with ltx=0 should have 5 faces (top degenerates to a line)";
}

TEST(BRepPrimAPI_MakeWedgeTest, WedgeVolume)
{
  const double aDx  = 10.0;
  const double aDy  = 10.0;
  const double aDz  = 10.0;
  const double aLtx = 5.0;

  BRepPrimAPI_MakeWedge aWedge(aDx, aDy, aDz, aLtx);
  TopoDS_Shape          aShape = aWedge.Shape();
  ASSERT_TRUE(aWedge.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);

  // Volume = dy * dz * (dx + ltx) / 2
  const double anExpectedVolume = aDy * aDz * (aDx + aLtx) / 2.0;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, Precision::Confusion());
}

TEST(BRepPrimAPI_MakeWedgeTest, WedgeFullParams)
{
  BRepPrimAPI_MakeWedge aWedge(20.0, 10.0, 20.0, 5.0, 5.0, 15.0, 15.0);
  TopoDS_Shape          aShape = aWedge.Shape();

  ASSERT_TRUE(aWedge.IsDone()) << "Wedge with full parameters creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Wedge with full parameters shape is not valid";
}
