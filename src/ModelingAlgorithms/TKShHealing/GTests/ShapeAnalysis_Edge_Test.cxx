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
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Array1.hxx>
#include <ShapeFix_Edge.hxx>
#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <gtest/gtest.h>

TEST(ShapeAnalysis_EdgeTest, HasCurve3d)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  ShapeAnalysis_Edge anAnalyzer;
  EXPECT_TRUE(anAnalyzer.HasCurve3d(anEdge));
}

TEST(ShapeAnalysis_EdgeTest, FirstVertex_LastVertex)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  ShapeAnalysis_Edge  anAnalyzer;
  const TopoDS_Vertex aFirstVertex = anAnalyzer.FirstVertex(anEdge);
  const TopoDS_Vertex aLastVertex  = anAnalyzer.LastVertex(anEdge);

  EXPECT_FALSE(aFirstVertex.IsNull());
  EXPECT_FALSE(aLastVertex.IsNull());
}

TEST(ShapeAnalysis_EdgeTest, IsClosed_OpenEdge)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  ShapeAnalysis_Edge anAnalyzer;
  EXPECT_FALSE(anAnalyzer.IsClosed3d(anEdge));
}

TEST(ShapeAnalysis_EdgeTest, IsClosed_ClosedEdge)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp::DZ()), 5.0);
  BRepBuilderAPI_MakeEdge  aMakeEdge(aCircle);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  ShapeAnalysis_Edge anAnalyzer;
  EXPECT_TRUE(anAnalyzer.IsClosed3d(anEdge));
}

TEST(ShapeAnalysis_EdgeTest, IsSeam_NonSeam)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  TopExp_Explorer aFaceExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(aFaceExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());

  TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());

  ShapeAnalysis_Edge anAnalyzer;
  EXPECT_FALSE(anAnalyzer.IsSeam(anEdge, aFace));
}

//=================================================================================================

TEST(ShapeAnalysis_EdgeTest, SameParameterDeviationBetweenControlPoints)
{
  // Each quadratic span meets the 3D line at the sampling points and bows away between them.
  constexpr double aDeviation = 0.01;
  for (const int aNbControl : {7, 23, 41})
  {
    const int                    aNbSpans = aNbControl - 1;
    NCollection_Array1<gp_Pnt2d> aPoles(1, 2 * aNbSpans + 1);
    NCollection_Array1<double>   aKnots(1, aNbSpans + 1);
    NCollection_Array1<int>      aMultiplicities(1, aNbSpans + 1);
    for (int aSpan = 0; aSpan <= aNbSpans; ++aSpan)
    {
      aPoles.ChangeAt(2 * aSpan)      = gp_Pnt2d(aSpan, 0.0);
      aKnots.ChangeAt(aSpan)          = aSpan;
      aMultiplicities.ChangeAt(aSpan) = (aSpan == 0 || aSpan == aNbSpans) ? 3 : 2;
      if (aSpan < aNbSpans)
      {
        aPoles.ChangeAt(2 * aSpan + 1) = gp_Pnt2d(aSpan + 0.5, 2.0 * aDeviation);
      }
    }
    const occ::handle<Geom2d_BSplineCurve> aPCurve =
      new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
    const occ::handle<Geom_Plane> aSurface = new Geom_Plane(gp_Pln(gp::XOY()));
    TopoDS_Edge  anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(aNbSpans, 0, 0));
    BRep_Builder aBuilder;
    aBuilder.UpdateEdge(anEdge, aPCurve, aSurface, TopLoc_Location(), Precision::Confusion());
    aBuilder.Range(anEdge, aSurface, TopLoc_Location(), 0.0, aNbSpans);
    aBuilder.SameRange(anEdge, true);
    aBuilder.SameParameter(anEdge, true);

    ShapeAnalysis_Edge anAnalysis;
    double             aMaxDeviation = 0.0;
    EXPECT_TRUE(anAnalysis.CheckSameParameter(anEdge, aMaxDeviation, aNbControl));
    EXPECT_TRUE(anAnalysis.Status(ShapeExtend_DONE1));
    EXPECT_NEAR(aMaxDeviation, aDeviation, aDeviation * 2.e-5);

    ShapeFix_Edge aFixer;
    EXPECT_TRUE(aFixer.FixSameParameter(anEdge));
    EXPECT_GE(BRep_Tool::Tolerance(anEdge), aDeviation);
    EXPECT_NEAR(BRep_Tool::Tolerance(anEdge), aDeviation, aDeviation * 2.e-5);
  }
}
