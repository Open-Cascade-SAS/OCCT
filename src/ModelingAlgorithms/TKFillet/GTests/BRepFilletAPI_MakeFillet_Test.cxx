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
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(BRepFilletAPI_MakeFilletTest, FilletOneEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(2.0, anEdge);
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeFilletTest, FilletAllEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aFillet.Add(1.0, TopoDS::Edge(anExp.Current()));
  }
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeFilletTest, FilletMoreFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aFillet.Add(1.0, TopoDS::Edge(anExp.Current()));
  }
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aResult, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GT(aFaceCount, 6);
}

TEST(BRepFilletAPI_MakeFilletTest, FilletVariableRadius)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(1.0, 3.0, anEdge);
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// Test OCC570: BRepFilletAPI_MakeFillet with mixed constant and variable radius.
// Migrated from QABugs_17.cxx OCC570
TEST(BRepFilletAPI_MakeFilletTest, OCC570_MixedVariableConstantRadius)
{
  BRepPrimAPI_MakeBox aBoxMaker(100., 100., 100.);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Take the first wire of the box and its 4 edges
  TopExp_Explorer aWireExp(aBox, TopAbs_WIRE);
  ASSERT_TRUE(aWireExp.More());

  TopExp_Explorer anEdgeExp(aWireExp.Current(), TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE1 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE2 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE3 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE4 = TopoDS::Edge(anEdgeExp.Current());

  // Variable radius law: 4 (parameter, radius) control points
  NCollection_Array1<gp_Pnt2d> aVarRadius(1, 4);
  aVarRadius.SetValue(1, gp_Pnt2d(0.0, 5.0));
  aVarRadius.SetValue(2, gp_Pnt2d(0.3, 15.0));
  aVarRadius.SetValue(3, gp_Pnt2d(0.7, 15.0));
  aVarRadius.SetValue(4, gp_Pnt2d(1.0, 5.0));

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.SetContinuity(GeomAbs_C1, 0.001);
  aFillet.Add(aVarRadius, anE1);
  aFillet.Add(5.0, anE2);
  aFillet.Add(aVarRadius, anE3);
  aFillet.Add(5.0, anE4);

  ASSERT_NO_THROW(aFillet.Build()) << "BRepFilletAPI_MakeFillet::Build should not throw";
  ASSERT_TRUE(aFillet.IsDone()) << "Fillet operation should succeed";

  const TopoDS_Shape& aResult = aFillet.Shape();

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Result shape should be valid";

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aResult, aProps);
  EXPECT_NEAR(aProps.Mass(), 58500., 58500. * 0.01) << "Surface area should be approximately 58500";
}
