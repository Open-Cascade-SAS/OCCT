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
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakeSphereTest, FullSphere)
{
  const double           aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();

  ASSERT_TRUE(aSphere.IsDone()) << "Full sphere creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Full sphere shape is not valid";
}

TEST(BRepPrimAPI_MakeSphereTest, FaceCount)
{
  BRepPrimAPI_MakeSphere aSphere(5.0);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 1) << "Full sphere should have 1 face";
}

TEST(BRepPrimAPI_MakeSphereTest, Volume)
{
  const double           aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  double anExpectedVolume = (4.0 / 3.0) * M_PI * aRadius * aRadius * aRadius;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, 0.01);
}

TEST(BRepPrimAPI_MakeSphereTest, PartialSphere_AngleLimited)
{
  const double aRadius = 5.0;
  // Half sphere: angle from 0 to PI
  BRepPrimAPI_MakeSphere aSphere(aRadius, M_PI);
  TopoDS_Shape           aShape = aSphere.Shape();

  ASSERT_TRUE(aSphere.IsDone()) << "Partial sphere creation failed";
  ASSERT_FALSE(aShape.IsNull());

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Partial sphere shape is not valid";

  // Partial sphere should have more faces than a full sphere
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GT(aFaceCount, 1) << "Partial sphere should have more than 1 face";
}

TEST(BRepPrimAPI_MakeSphereTest, CenterOfMass)
{
  const double           aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  gp_Pnt aCOM = aProps.CentreOfMass();

  // Center of mass of full sphere should be at origin
  EXPECT_NEAR(aCOM.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Z(), 0.0, Precision::Confusion());
}
