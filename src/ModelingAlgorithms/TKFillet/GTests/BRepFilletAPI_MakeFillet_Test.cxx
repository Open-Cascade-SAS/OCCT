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
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

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
