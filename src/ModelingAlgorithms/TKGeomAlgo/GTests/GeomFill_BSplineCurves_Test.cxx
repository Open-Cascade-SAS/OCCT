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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAPI.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <Standard_Handle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

// OCC28131: BRepOffset_MakeOffset can't create offset with a face made by filling 3 BSpline curves.

namespace
{

//! Builds the filled face from 3 boundary BSpline/Bezier curves (common setup for all OCC28131 tests).
TopoDS_Shape createOCC28131Face()
{
  const double aHeight = 8.5;
  const gp_Pnt aV0(-17.6, 0.0, 0.0);
  const gp_Pnt aV1(0, 32.8, 0.0);

  // Outline Bezier curve
  NCollection_Array1<gp_Pnt> aBezierPoles(1, 4);
  aBezierPoles(1) = aV0;
  aBezierPoles(4) = aV1;
  aBezierPoles(2) = gp_Pnt(aV0.X(), (5.4 / 13.2) * aV1.Y(), 0);
  aBezierPoles(3) = gp_Pnt((6.0 / 6.8) * aV0.X(), aV1.Y(), 0);

  occ::handle<Geom_BezierCurve>  aBezier        = new Geom_BezierCurve(aBezierPoles);
  occ::handle<Geom_BSplineCurve> anOutlineCurve = GeomConvert::CurveToBSplineCurve(aBezier);

  // First side curve from 2D interpolation projected on YZ plane
  occ::handle<Geom_BSplineCurve> aCurve1;
  {
    occ::handle<NCollection_HArray1<gp_Pnt2d>> aHArray = new NCollection_HArray1<gp_Pnt2d>(1, 2);
    aHArray->SetValue(1, gp_Pnt2d(-aV1.Y(), 0));
    aHArray->SetValue(2, gp_Pnt2d(0, aHeight + aHeight / 2));
    Geom2dAPI_Interpolate anInterp(aHArray, false, 1e-6);
    anInterp.Load(gp_Vec2d(0, 1), gp_Vec2d(1, 0));
    anInterp.Perform();
    const gp_Pln aPln{gp_Ax3(gp_Pnt(), gp_Dir(gp_Dir::D::X), gp_Dir(gp_Dir::D::NY))};
    aCurve1 = occ::down_cast<Geom_BSplineCurve>(GeomAPI::To3d(anInterp.Curve(), aPln));
  }

  // Second side curve from 2D interpolation projected on XZ plane
  occ::handle<Geom_BSplineCurve> aCurve2;
  {
    occ::handle<NCollection_HArray1<gp_Pnt2d>> aHArray = new NCollection_HArray1<gp_Pnt2d>(1, 3);
    aHArray->SetValue(1, gp_Pnt2d(-aV0.X(), 0));
    aHArray->SetValue(2, gp_Pnt2d(-aV0.X() - 2.6, aHeight));
    aHArray->SetValue(3, gp_Pnt2d(0, aHeight + aHeight / 2));
    Geom2dAPI_Interpolate anInterp(aHArray, false, 1e-6);
    anInterp.Perform();
    const gp_Pln aPln{gp_Ax3(gp_Pnt(), gp_Dir(gp_Dir::D::NY), gp_Dir(gp_Dir::D::NX))};
    aCurve2 = occ::down_cast<Geom_BSplineCurve>(GeomAPI::To3d(anInterp.Curve(), aPln));
  }

  GeomFill_BSplineCurves aFill;
  aFill.Init(anOutlineCurve, aCurve1, aCurve2, GeomFill_CoonsStyle);

  BRepBuilderAPI_MakeFace aFaceBuilder(aFill.Surface(), 0);
  return aFaceBuilder.IsDone() ? aFaceBuilder.Shape() : TopoDS_Shape();
}

//! Returns the maximum BRep tolerance across all vertices, edges, and faces of theShape.
double maxTolerance(const TopoDS_Shape& theShape)
{
  double aMaxTol = 0.0;
  for (TopExp_Explorer anExp(theShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
    aMaxTol = std::max(aMaxTol, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    aMaxTol = std::max(aMaxTol, BRep_Tool::Tolerance(TopoDS::Edge(anExp.Current())));
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
    aMaxTol = std::max(aMaxTol, BRep_Tool::Tolerance(TopoDS::Face(anExp.Current())));
  return aMaxTol;
}

//! Returns the surface area of theShape.
double surfaceArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

} // namespace

TEST(GeomFill_BSplineCurvesTest, OCC28131_FillSurfaceFromBezierAndInterpolatedCurves)
{
  const TopoDS_Shape aFace = createOCC28131Face();
  ASSERT_FALSE(aFace.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aFace).IsValid());
}

TEST(GeomFill_BSplineCurvesTest, OCC28131_SimpleOffsetOfFilledFace)
{
  const TopoDS_Shape aFace = createOCC28131Face();
  ASSERT_FALSE(aFace.IsNull());

  BRepOffsetAPI_MakeOffsetShape aMaker;
  aMaker.PerformBySimple(aFace, 10.0);
  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape aResult = aMaker.Shape();
  ASSERT_FALSE(aResult.IsNull());

  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  // checkmaxtol -ref 0.205 (1% relative tolerance)
  EXPECT_NEAR(maxTolerance(aResult), 0.205, 0.205 * 0.01);
  // checkprops -s 1693.7 (1% relative tolerance)
  EXPECT_NEAR(surfaceArea(aResult), 1693.7, 1693.7 * 0.01);
}

TEST(GeomFill_BSplineCurvesTest, OCC28131_StandardOffsetOfFilledFace)
{
  const TopoDS_Shape aFace = createOCC28131Face();
  ASSERT_FALSE(aFace.IsNull());

  BRepOffset_MakeOffset aMaker;
  aMaker.Initialize(aFace, 10.0, Precision::Confusion(), BRepOffset_Skin, false, false, GeomAbs_Arc);
  aMaker.MakeOffsetShape();
  ASSERT_FALSE(aMaker.Shape().IsNull());

  // The Draw test runs fixshape before checking — apply ShapeFix_Shape likewise
  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aMaker.Shape());
  aFixer->Perform();
  const TopoDS_Shape aResult = aFixer->Shape();
  ASSERT_FALSE(aResult.IsNull());

  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  // checkmaxtol -ref 0.408
  EXPECT_NEAR(maxTolerance(aResult), 0.408, 0.408 * 0.01);
  // checkprops -s 1693.76
  EXPECT_NEAR(surfaceArea(aResult), 1693.76, 1693.76 * 0.01);
}
