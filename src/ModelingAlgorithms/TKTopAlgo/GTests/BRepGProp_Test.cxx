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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepGPropTest, LinearProperties_EdgeLength)
{
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(3.0, 4.0, 0.0);

  BRepBuilderAPI_MakeEdge aMakeEdge(aP1, aP2);
  ASSERT_TRUE(aMakeEdge.IsDone());

  GProp_GProps aProps;
  BRepGProp::LinearProperties(aMakeEdge.Edge(), aProps);
  EXPECT_NEAR(aProps.Mass(), 5.0, Precision::Confusion()) << "Edge length should be 5";
}

TEST(BRepGPropTest, SurfaceProperties_BoxFaceArea)
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // Total surface area = 2*(10*20 + 10*30 + 20*30) = 2*(200+300+600) = 2200
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 2200.0, Precision::Confusion());
}

TEST(BRepGPropTest, VolumeProperties_UnitBox)
{
  BRepPrimAPI_MakeBox aBox(1.0, 1.0, 1.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 1.0, Precision::Confusion()) << "Unit box volume should be 1";
}

TEST(BRepGPropTest, VolumeProperties_Sphere)
{
  const double aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  double anExpectedVolume = (4.0 / 3.0) * M_PI * aRadius * aRadius * aRadius;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, 0.01) << "Sphere volume mismatch";
}

TEST(BRepGPropTest, VolumeProperties_BoxCenterOfMass)
{
  // Box from (0,0,0) to (10,10,10)
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  gp_Pnt aCOM = aProps.CentreOfMass();

  // Center of mass should be at (5, 5, 5)
  EXPECT_NEAR(aCOM.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Z(), 5.0, Precision::Confusion());
}

TEST(BRepGPropTest, LinearProperties_SkipShared)
{
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // With SkipShared = false, shared edges are counted multiple times
  GProp_GProps aPropsNotSkipped;
  BRepGProp::LinearProperties(aShape, aPropsNotSkipped, false);

  // With SkipShared = true, each edge is counted once
  GProp_GProps aPropsSkipped;
  BRepGProp::LinearProperties(aShape, aPropsSkipped, true);

  // Box has 12 edges, each of length 10 = total 120
  EXPECT_NEAR(aPropsSkipped.Mass(), 120.0, Precision::Confusion())
    << "Total edge length of 10x10x10 box with SkipShared=true should be 120";

  // Without SkipShared, each edge is counted for each face it belongs to (2 faces per edge)
  // So total = 240
  EXPECT_NEAR(aPropsNotSkipped.Mass(), 240.0, Precision::Confusion())
    << "Total edge length with SkipShared=false should be double";
}
