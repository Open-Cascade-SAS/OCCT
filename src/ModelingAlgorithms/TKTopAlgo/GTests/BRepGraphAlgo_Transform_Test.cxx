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

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphAlgo_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cmath>
#include <iostream>

#include <gtest/gtest.h>

TEST(BRepGraphAlgo_TransformTest, TranslateBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 200.0, 300.0));

  TopoDS_Shape aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_FALSE(aTransformed.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aTransformed, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_TransformTest, TranslateBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  TopoDS_Shape aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf, true);
  ASSERT_FALSE(aTransformed.IsNull());

  double aTransArea = 0.0;
  for (TopExp_Explorer anExp(aTransformed, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aTransArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aTransArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraphAlgo_TransformTest, TranslateBox_CentroidShifted)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  gp_Pnt anOrigCentroid = aOrigProps.CentreOfMass();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const double aDx = 100.0, aDy = 200.0, aDz = 300.0;
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  TopoDS_Shape aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf, false);
  ASSERT_FALSE(aTransformed.IsNull());

  GProp_GProps aTransProps;
  BRepGProp::SurfaceProperties(aTransformed, aTransProps);
  gp_Pnt aTransCentroid = aTransProps.CentreOfMass();

  EXPECT_NEAR(aTransCentroid.X(), anOrigCentroid.X() + aDx, 1.0);
  EXPECT_NEAR(aTransCentroid.Y(), anOrigCentroid.Y() + aDy, 1.0);
  EXPECT_NEAR(aTransCentroid.Z(), anOrigCentroid.Z() + aDz, 1.0);
}

TEST(BRepGraphAlgo_TransformTest, LocationOnly_NoCopyGeom)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  // theCopyGeom = false: should be very fast (location change only).
  TopoDS_Shape aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf, false);
  ASSERT_FALSE(aTransformed.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aTransformed, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraphAlgo_TransformTest, TransformSingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));

  TopoDS_Shape aTransFace = BRepGraphAlgo_Transform::TransformFace(aGraph, 0, aTrsf, true);
  ASSERT_FALSE(aTransFace.IsNull());
  EXPECT_EQ(aTransFace.ShapeType(), TopAbs_FACE);
}

// Performance comparison: BRepGraphAlgo_Transform vs BRepBuilderAPI_Transform.
TEST(BRepGraphAlgo_TransformTest, Performance_VsLegacy)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 200.0, 300.0));

  constexpr int THE_NB_ITERS = 1000;

  // Graph-based transform: build graph once, transform many times.
  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  OSD_Timer aGraphTimer;
  aGraphTimer.Start();
  for (int anIter = 0; anIter < THE_NB_ITERS; ++anIter)
  {
    TopoDS_Shape aResult = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf, true);
    (void)aResult;
  }
  aGraphTimer.Stop();

  // Legacy transform.
  OSD_Timer aLegacyTimer;
  aLegacyTimer.Start();
  for (int anIter = 0; anIter < THE_NB_ITERS; ++anIter)
  {
    BRepBuilderAPI_Transform aTransformer(aBox, aTrsf, true);
    TopoDS_Shape aResult = aTransformer.Shape();
    (void)aResult;
  }
  aLegacyTimer.Stop();

  std::cout << "[  PERF   ] BRepGraphAlgo_Transform: " << aGraphTimer.ElapsedTime()
            << " s (" << THE_NB_ITERS << " iters)" << std::endl;
  std::cout << "[  PERF   ] BRepBuilderAPI_Transform: " << aLegacyTimer.ElapsedTime()
            << " s (" << THE_NB_ITERS << " iters)" << std::endl;
  if (aGraphTimer.ElapsedTime() > 1.0e-9)
  {
    std::cout << "[  PERF   ] Speedup: "
              << aLegacyTimer.ElapsedTime() / aGraphTimer.ElapsedTime() << "x" << std::endl;
  }
}
