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

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepClass3d_SolidClassifierTest, PointInside_Box)
{
  BRepPrimAPI_MakeBox aMakeBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepClass3d_SolidClassifier aClassifier(aBox);
  aClassifier.Perform(gp_Pnt(5.0, 5.0, 5.0), Precision::Confusion());

  EXPECT_EQ(aClassifier.State(), TopAbs_IN)
    << "Point (5,5,5) should be inside the box";
}

TEST(BRepClass3d_SolidClassifierTest, PointOutside_Box)
{
  BRepPrimAPI_MakeBox aMakeBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepClass3d_SolidClassifier aClassifier(aBox);
  aClassifier.Perform(gp_Pnt(20.0, 20.0, 20.0), Precision::Confusion());

  EXPECT_EQ(aClassifier.State(), TopAbs_OUT)
    << "Point (20,20,20) should be outside the box";
}

TEST(BRepClass3d_SolidClassifierTest, PointOnFace_Box)
{
  BRepPrimAPI_MakeBox aMakeBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepClass3d_SolidClassifier aClassifier(aBox);
  aClassifier.Perform(gp_Pnt(5.0, 5.0, 0.0), Precision::Confusion());

  EXPECT_EQ(aClassifier.State(), TopAbs_ON)
    << "Point (5,5,0) should be on the bottom face of the box";
}

TEST(BRepClass3d_SolidClassifierTest, PointInside_Sphere)
{
  BRepPrimAPI_MakeSphere aMakeSphere(10.0);
  const TopoDS_Shape& aSphere = aMakeSphere.Shape();
  ASSERT_TRUE(aMakeSphere.IsDone()) << "Sphere creation failed";

  BRepClass3d_SolidClassifier aClassifier(aSphere);
  aClassifier.Perform(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());

  EXPECT_EQ(aClassifier.State(), TopAbs_IN)
    << "Origin should be inside the sphere";
}

TEST(BRepClass3d_SolidClassifierTest, PerformInfinitePoint)
{
  BRepPrimAPI_MakeBox aMakeBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone()) << "Box creation failed";

  BRepClass3d_SolidClassifier aClassifier(aBox);
  aClassifier.PerformInfinitePoint(Precision::Confusion());

  EXPECT_EQ(aClassifier.State(), TopAbs_OUT)
    << "Infinite point should be outside the box";
}
