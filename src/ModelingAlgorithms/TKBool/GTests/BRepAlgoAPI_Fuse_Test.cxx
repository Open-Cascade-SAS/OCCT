// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <GProp_GProps.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

// OCC822_1: BRepMesh_IncrementalMesh correctness test - boolean operations with cylinders and
// cones. Creates two pairs (inner/outer) of cylinders and cones, fuses each pair, then cuts inner
// from outer.
TEST(BRepAlgoAPI_FuseTest, CylinderAndCone_FuseThenCut)
{
  const gp_Ax2       anAxis1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  const TopoDS_Shape aCylIn  = BRepPrimAPI_MakeCylinder(anAxis1, 40, 110).Shape();
  const TopoDS_Shape aCylOut = BRepPrimAPI_MakeCylinder(anAxis1, 50, 100).Shape();

  const gp_Ax2       anAxis2(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::NZ));
  const TopoDS_Shape aConIn  = BRepPrimAPI_MakeCone(anAxis2, 40, 60, 110).Shape();
  const TopoDS_Shape aConOut = BRepPrimAPI_MakeCone(anAxis2, 50, 70, 100).Shape();

  BRepAlgoAPI_Fuse aFuseIn(aCylIn, aConIn);
  ASSERT_TRUE(aFuseIn.IsDone());

  BRepAlgoAPI_Fuse aFuseOut(aCylOut, aConOut);
  ASSERT_TRUE(aFuseOut.IsDone());

  BRepAlgoAPI_Cut aCut(aFuseOut.Shape(), aFuseIn.Shape());
  ASSERT_TRUE(aCut.IsDone());

  const TopoDS_Shape aResult = aCut.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 133931.0, 133.931); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

// OCC822_2: BRepMesh_IncrementalMesh correctness test - fuse of box and sphere.
TEST(BRepAlgoAPI_FuseTest, BoxAndSphere)
{
  const TopoDS_Shape aBox    = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), gp_Pnt(100, 100, 100)).Shape();
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 50, 50), 25.0).Shape();

  BRepAlgoAPI_Fuse aFuse(aBox, aSphere);
  ASSERT_TRUE(aFuse.IsDone());

  const TopoDS_Shape aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 61963.5, 61.9635); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

// OCC823: BRepAlgoAPI_Fuse correctness test - fuse of two nearly-parallel cylinders.
TEST(BRepAlgoAPI_FuseTest, TwoCylinders)
{
  const gp_Ax2       anAxis1(gp_Pnt(40, 50, 0), gp_Dir(100, 0, 0));
  const TopoDS_Shape aCyl1 = BRepPrimAPI_MakeCylinder(anAxis1, 20, 100).Shape();

  constexpr double   aSize = 0.001;
  const gp_Ax2       anAxis2(gp_Pnt(100, 50, aSize), gp_Dir(0, aSize, 80));
  const TopoDS_Shape aCyl2 = BRepPrimAPI_MakeCylinder(anAxis2, 20, 80).Shape();

  BRepAlgoAPI_Fuse aFuse(aCyl2, aCyl1);
  ASSERT_TRUE(aFuse.IsDone());

  const TopoDS_Shape aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 23189.5, 23.1895); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

// OCC824: BRepAlgoAPI_Fuse correctness test - fuse of cylinder and sphere.
TEST(BRepAlgoAPI_FuseTest, CylinderAndSphere)
{
  const gp_Pnt       aCenter(100, 0, 0);
  const gp_Ax2       anAxis(aCenter, gp_Dir(gp_Dir::D::NX));
  const TopoDS_Shape aCyl    = BRepPrimAPI_MakeCylinder(anAxis, 20, 100).Shape();
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(aCenter, 20.0).Shape();

  BRepAlgoAPI_Fuse aFuse(aCyl, aSphere);
  ASSERT_TRUE(aFuse.IsDone());

  const TopoDS_Shape aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 16336.3, 16.3363); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

// OCC826: BRepAlgoAPI_Fuse correctness test - fuse of revolved face and sphere.
// Intersection line passes near the pole of the sphere.
TEST(BRepAlgoAPI_FuseTest, RevolvedFaceAndSphere)
{
  BRepBuilderAPI_MakePolygon aWire;
  const double               aX1 = 181.82808, aX2 = 202.39390;
  const double               aY1 = 31.011970, aY2 = 123.06856;
  aWire.Add(gp_Pnt(aX1, aY1, 0));
  aWire.Add(gp_Pnt(aX2, aY1, 0));
  aWire.Add(gp_Pnt(aX2, aY2, 0));
  aWire.Add(gp_Pnt(aX1, aY2, 0));
  aWire.Add(gp_Pnt(aX1, aY1, 0));

  const TopoDS_Face  aFace = BRepBuilderAPI_MakeFace(aWire.Wire(), false);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 30, 0));
  const TopoDS_Shape aRevol = BRepPrimAPI_MakeRevol(aFace, anAxis, 2.0 * M_PI).Shape();
  const TopoDS_Shape aSphere =
    BRepPrimAPI_MakeSphere(gp_Pnt(166.373, 77.0402, 96.0555), 23.218586).Shape();

  BRepAlgoAPI_Fuse aFuse(aRevol, aSphere);
  ASSERT_TRUE(aFuse.IsDone());

  const TopoDS_Shape aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 272935.0, 272.935); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

// OCC827: BRepAlgoAPI_Fuse correctness test - fuse of revolved solid with two tori.
TEST(BRepAlgoAPI_FuseTest, RevolvedSolidAndTwoTori)
{
  BRepBuilderAPI_MakePolygon aWire;
  aWire.Add(gp_Pnt(10, 0, 0));
  aWire.Add(gp_Pnt(20, 0, 0));
  aWire.Add(gp_Pnt(20, 0, 50));
  aWire.Add(gp_Pnt(10, 0, 50));
  aWire.Add(gp_Pnt(10, 0, 0));

  const TopoDS_Face  aFace = BRepBuilderAPI_MakeFace(aWire.Wire(), false);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 30));
  const TopoDS_Shape aRevol = BRepPrimAPI_MakeRevol(aFace, anAxis, 2.0 * M_PI).Shape();

  constexpr double aMajRad = 15.0;
  constexpr double aMinRad = 5.0;

  const TopoDS_Shape aTor1 =
    BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(0, 0, 50), gp_Dir(0, 0, 30)), aMajRad, aMinRad).Shape();
  const TopoDS_Shape aTor2 =
    BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 30)), aMajRad, aMinRad).Shape();

  BRepAlgoAPI_Fuse aFuse1(aTor1, aRevol);
  ASSERT_TRUE(aFuse1.IsDone());

  BRepAlgoAPI_Fuse aFuse2(aTor2, aFuse1.Shape());
  ASSERT_TRUE(aFuse2.IsDone());

  const TopoDS_Shape aResult = aFuse2.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aResult, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 11847.7, 11.8477); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}
