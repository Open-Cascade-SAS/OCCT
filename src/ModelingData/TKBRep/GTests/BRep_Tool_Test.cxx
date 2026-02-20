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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <gtest/gtest.h>

TEST(BRep_Tool_Test, Pnt_FromVertex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_VERTEX);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Vertex& aVertex = TopoDS::Vertex(anExp.Current());
  gp_Pnt aPnt = BRep_Tool::Pnt(aVertex);

  // The point coordinates should be within the box bounds [0,10] x [0,20] x [0,30]
  EXPECT_GE(aPnt.X(), -Precision::Confusion());
  EXPECT_LE(aPnt.X(), 10.0 + Precision::Confusion());
  EXPECT_GE(aPnt.Y(), -Precision::Confusion());
  EXPECT_LE(aPnt.Y(), 20.0 + Precision::Confusion());
  EXPECT_GE(aPnt.Z(), -Precision::Confusion());
  EXPECT_LE(aPnt.Z(), 30.0 + Precision::Confusion());
}

TEST(BRep_Tool_Test, Tolerance_Vertex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_VERTEX);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Vertex& aVertex = TopoDS::Vertex(anExp.Current());
  double aTol = BRep_Tool::Tolerance(aVertex);
  EXPECT_GE(aTol, 0.0);
}

TEST(BRep_Tool_Test, Tolerance_Edge)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
  double aTol = BRep_Tool::Tolerance(anEdge);
  EXPECT_GE(aTol, 0.0);
}

TEST(BRep_Tool_Test, Tolerance_Face)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
  double aTol = BRep_Tool::Tolerance(aFace);
  EXPECT_GE(aTol, 0.0);
}

TEST(BRep_Tool_Test, Curve_FromEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
  double aFirst = 0.0;
  double aLast = 0.0;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);

  EXPECT_FALSE(aCurve.IsNull()) << "Curve from a box edge should not be null";
  EXPECT_LT(aFirst, aLast) << "First parameter should be less than last parameter";
}

TEST(BRep_Tool_Test, Surface_FromFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);

  EXPECT_FALSE(aSurface.IsNull()) << "Surface from a box face should not be null";
}

TEST(BRep_Tool_Test, IsClosed_CircleEdge)
{
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp::DZ()), 5.0);
  BRepBuilderAPI_MakeEdge anEdgeMaker(aCircle);
  ASSERT_TRUE(anEdgeMaker.IsDone());

  const TopoDS_Edge& anEdge = anEdgeMaker.Edge();
  EXPECT_TRUE(BRep_Tool::IsClosed(anEdge)) << "A full circle edge should be closed";
}

TEST(BRep_Tool_Test, IsClosed_LineEdge)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  ASSERT_TRUE(anEdgeMaker.IsDone());

  const TopoDS_Edge& anEdge = anEdgeMaker.Edge();
  EXPECT_FALSE(BRep_Tool::IsClosed(anEdge)) << "A line segment edge should not be closed";
}

TEST(BRep_Tool_Test, Degenerated)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
  EXPECT_FALSE(BRep_Tool::Degenerated(anEdge)) << "Box edges should not be degenerated";
}
