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

#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepBuilderAPI_SewingTest, SewTwoFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace1 = TopoDS::Face(anExp.Current());
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace2 = TopoDS::Face(anExp.Current());

  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFace1);
  aSewing.Add(aFace2);
  aSewing.Perform();

  const TopoDS_Shape& aResult = aSewing.SewedShape();
  EXPECT_FALSE(aResult.IsNull());
}

TEST(BRepBuilderAPI_SewingTest, NbFreeEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace1 = TopoDS::Face(anExp.Current());
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace2 = TopoDS::Face(anExp.Current());

  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFace1);
  aSewing.Add(aFace2);
  aSewing.Perform();

  // Two faces from a box share one edge; the remaining edges are free.
  EXPECT_GT(aSewing.NbFreeEdges(), 0);
}

TEST(BRepBuilderAPI_SewingTest, NbDegeneratedShapes)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace1 = TopoDS::Face(anExp.Current());
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace2 = TopoDS::Face(anExp.Current());

  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFace1);
  aSewing.Add(aFace2);
  aSewing.Perform();

  // Planar faces should have no degenerated shapes.
  EXPECT_EQ(aSewing.NbDegeneratedShapes(), 0);
}

TEST(BRepBuilderAPI_SewingTest, SetTolerance)
{
  BRepBuilderAPI_Sewing aSewing;
  const double aTol = 0.01;
  aSewing.SetTolerance(aTol);
  EXPECT_NEAR(aSewing.Tolerance(), aTol, Precision::Confusion());
}

TEST(BRepBuilderAPI_SewingTest, SewAllBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepBuilderAPI_Sewing aSewing;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aSewing.Add(anExp.Current());
  }
  aSewing.Perform();

  const TopoDS_Shape& aResult = aSewing.SewedShape();
  EXPECT_FALSE(aResult.IsNull());

  // All 6 faces of a box form a closed shell, so there should be no free edges.
  EXPECT_EQ(aSewing.NbFreeEdges(), 0);
}
