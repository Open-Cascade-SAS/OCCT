// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent.hxx>
#include <StdObjMgt_MapOfInstantiators.hxx>

#include <StdLPersistent_HArray1.hxx>
#include <StdLPersistent_HArray2.hxx>
#include <StdPersistent_TopoDS.hxx>
#include <ShapePersistent_TopoDS.hxx>
#include <ShapePersistent_HArray1.hxx>
#include <ShapePersistent_HArray2.hxx>
#include <ShapePersistent_HSequence.hxx>
#include <ShapePersistent_Geom2d.hxx>
#include <ShapePersistent_Geom2d_Curve.hxx>
#include <ShapePersistent_Geom.hxx>
#include <ShapePersistent_Geom_Curve.hxx>
#include <ShapePersistent_Geom_Surface.hxx>
#include <ShapePersistent_Poly.hxx>
#include <ShapePersistent_BRep.hxx>


//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void ShapePersistent::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PTopoDS_HArray1OfHShape");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfBoundedCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfBezierCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfBSplineCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfSurface");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom_HArray1OfBoundedSurface");
  theMap.Bind <StdLPersistent_HArray2::Persistent> ("PColPGeom_HArray2OfSurface");
  theMap.Bind <StdLPersistent_HArray2::Persistent> ("PColPGeom_HArray2OfBoundedSurface");
  theMap.Bind <StdLPersistent_HArray2::Persistent> ("PColPGeom_HArray2OfBezierSurface");
  theMap.Bind <StdLPersistent_HArray2::Persistent> ("PColPGeom_HArray2OfBSplineSurface");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom2d_HArray1OfCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom2d_HArray1OfBoundedCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom2d_HArray1OfBezierCurve");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColPGeom2d_HArray1OfBSplineCurve");

  theMap.Bind <StdLPersistent_HArray2::Real>       ("PColStd_HArray2OfReal");

  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_HShape");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Vertex");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Edge");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Wire");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Face");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Shell");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Solid");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_CompSolid");
  theMap.Bind <ShapePersistent_TopoDS::HShape>      ("PTopoDS_Compound");

  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TShape");

  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TVertex");
  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TVertex1");
  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TEdge");
  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TEdge1");
  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TFace");
  theMap.Bind <StdPersistent_TopoDS::TShape>        ("PTopoDS_TFace1");

  theMap.Bind <ShapePersistent_TopoDS::TWire>       ("PTopoDS_TWire");
  theMap.Bind <ShapePersistent_TopoDS::TWire1>      ("PTopoDS_TWire1");
  theMap.Bind <ShapePersistent_TopoDS::TShell>      ("PTopoDS_TShell");
  theMap.Bind <ShapePersistent_TopoDS::TShell1>     ("PTopoDS_TShell1");
  theMap.Bind <ShapePersistent_TopoDS::TSolid>      ("PTopoDS_TSolid");
  theMap.Bind <ShapePersistent_TopoDS::TSolid1>     ("PTopoDS_TSolid1");
  theMap.Bind <ShapePersistent_TopoDS::TCompSolid>  ("PTopoDS_TCompSolid");
  theMap.Bind <ShapePersistent_TopoDS::TCompSolid1> ("PTopoDS_TCompSolid1");
  theMap.Bind <ShapePersistent_TopoDS::TCompound>   ("PTopoDS_TCompound");
  theMap.Bind <ShapePersistent_TopoDS::TCompound1>  ("PTopoDS_TCompound1");

  theMap.Bind <ShapePersistent_HArray1::XYZ>        ("PColgp_HArray1OfXYZ");
  theMap.Bind <ShapePersistent_HArray1::Pnt>        ("PColgp_HArray1OfPnt");
  theMap.Bind <ShapePersistent_HArray1::Dir>        ("PColgp_HArray1OfDir");
  theMap.Bind <ShapePersistent_HArray1::Vec>        ("PColgp_HArray1OfVec");
  theMap.Bind <ShapePersistent_HArray1::XY>         ("PColgp_HArray1OfXY");
  theMap.Bind <ShapePersistent_HArray1::Pnt2d>      ("PColgp_HArray1OfPnt2d");
  theMap.Bind <ShapePersistent_HArray1::Dir2d>      ("PColgp_HArray1OfDir2d");
  theMap.Bind <ShapePersistent_HArray1::Vec2d>      ("PColgp_HArray1OfVec2d");
  theMap.Bind <ShapePersistent_HArray1::Lin2d>      ("PColgp_HArray1OfLin2d");
  theMap.Bind <ShapePersistent_HArray1::Circ2d>     ("PColgp_HArray1OfCirc2d");
  theMap.Bind <ShapePersistent_HArray1::Triangle>   ("PPoly_HArray1OfTriangle");

  theMap.Bind <ShapePersistent_HArray2::XYZ>        ("PColgp_HArray2OfXYZ");
  theMap.Bind <ShapePersistent_HArray2::Pnt>        ("PColgp_HArray2OfPnt");
  theMap.Bind <ShapePersistent_HArray2::Dir>        ("PColgp_HArray2OfDir");
  theMap.Bind <ShapePersistent_HArray2::Vec>        ("PColgp_HArray2OfVec");
  theMap.Bind <ShapePersistent_HArray2::XY>         ("PColgp_HArray2OfXY");
  theMap.Bind <ShapePersistent_HArray2::Pnt2d>      ("PColgp_HArray2OfPnt2d");
  theMap.Bind <ShapePersistent_HArray2::Dir2d>      ("PColgp_HArray2OfDir2d");
  theMap.Bind <ShapePersistent_HArray2::Vec2d>      ("PColgp_HArray2OfVec2d");
  theMap.Bind <ShapePersistent_HArray2::Lin2d>      ("PColgp_HArray2OfLin2d");
  theMap.Bind <ShapePersistent_HArray2::Circ2d>     ("PColgp_HArray2OfCirc2d");

  theMap.Bind <ShapePersistent_HSequence::XYZ>      ("PColgp_HSequenceOfXYZ");
  theMap.Bind <ShapePersistent_HSequence::Pnt>      ("PColgp_HSequenceOfPnt");
  theMap.Bind <ShapePersistent_HSequence::Dir>      ("PColgp_HSequenceOfDir");
  theMap.Bind <ShapePersistent_HSequence::Vec>      ("PColgp_HSequenceOfVec");

  theMap.Bind <ShapePersistent_HSequence::XYZ::Node>
    ("PColgp_SeqNodeOfHSequenceOfXYZ");

  theMap.Bind <ShapePersistent_HSequence::Pnt::Node>
    ("PColgp_SeqNodeOfHSequenceOfPnt");

  theMap.Bind <ShapePersistent_HSequence::Dir::Node>
    ("PColgp_SeqNodeOfHSequenceOfDir");

  theMap.Bind <ShapePersistent_HSequence::Vec::Node>
    ("PColgp_SeqNodeOfHSequenceOfVec");

  theMap.Bind <ShapePersistent_Geom2d::Transformation>
    ("PGeom2d_Transformation");

  theMap.Bind <ShapePersistent_Geom2d::Geometry>
    ("PGeom2d_Geometry");

  theMap.Bind <ShapePersistent_Geom2d::Point>
    ("PGeom2d_Point");

  theMap.Bind <ShapePersistent_Geom2d::CartesianPoint>
    ("PGeom2d_CartesianPoint");

  theMap.Bind <ShapePersistent_Geom2d::Vector>
    ("PGeom2d_Vector");

  theMap.Bind <ShapePersistent_Geom2d::Direction>
    ("PGeom2d_Direction");

  theMap.Bind <ShapePersistent_Geom2d::VectorWithMagnitude>
    ("PGeom2d_VectorWithMagnitude");

  theMap.Bind <ShapePersistent_Geom2d::AxisPlacement>
    ("PGeom2d_AxisPlacement");

  theMap.Bind <ShapePersistent_Geom2d::Curve>
    ("PGeom2d_Curve");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Line>
    ("PGeom2d_Line");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Conic>
    ("PGeom2d_Conic");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Circle>
    ("PGeom2d_Circle");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Ellipse>
    ("PGeom2d_Ellipse");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Hyperbola>
    ("PGeom2d_Hyperbola");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Parabola>
    ("PGeom2d_Parabola");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Bounded>
    ("PGeom2d_BoundedCurve");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Bezier>
    ("PGeom2d_BezierCurve");

  theMap.Bind <ShapePersistent_Geom2d_Curve::BSpline>
    ("PGeom2d_BSplineCurve");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Trimmed>
    ("PGeom2d_TrimmedCurve");

  theMap.Bind <ShapePersistent_Geom2d_Curve::Offset>
    ("PGeom2d_OffsetCurve");

  theMap.Bind <ShapePersistent_Geom::Transformation>
    ("PGeom_Transformation");

  theMap.Bind <ShapePersistent_Geom::Geometry>
    ("PGeom_Geometry");

  theMap.Bind <ShapePersistent_Geom::Point>
    ("PGeom_Point");

  theMap.Bind <ShapePersistent_Geom::CartesianPoint>
    ("PGeom_CartesianPoint");

  theMap.Bind <ShapePersistent_Geom::Vector>
    ("PGeom_Vector");

  theMap.Bind <ShapePersistent_Geom::Direction>
    ("PGeom_Direction");

  theMap.Bind <ShapePersistent_Geom::VectorWithMagnitude>
    ("PGeom_VectorWithMagnitude");

  theMap.Bind <ShapePersistent_Geom::AxisPlacement>
    ("PGeom_AxisPlacement");

  theMap.Bind <ShapePersistent_Geom::Axis1Placement>
    ("PGeom_Axis1Placement");

  theMap.Bind <ShapePersistent_Geom::Axis2Placement>
    ("PGeom_Axis2Placement");

  theMap.Bind <ShapePersistent_Geom::Curve>
    ("PGeom_Curve");

  theMap.Bind <ShapePersistent_Geom_Curve::Line>
    ("PGeom_Line");

  theMap.Bind <ShapePersistent_Geom_Curve::Conic>
    ("PGeom_Conic");

  theMap.Bind <ShapePersistent_Geom_Curve::Circle>
    ("PGeom_Circle");

  theMap.Bind <ShapePersistent_Geom_Curve::Ellipse>
    ("PGeom_Ellipse");

  theMap.Bind <ShapePersistent_Geom_Curve::Hyperbola>
    ("PGeom_Hyperbola");

  theMap.Bind <ShapePersistent_Geom_Curve::Parabola>
    ("PGeom_Parabola");

  theMap.Bind <ShapePersistent_Geom_Curve::Bounded>
    ("PGeom_BoundedCurve");

  theMap.Bind <ShapePersistent_Geom_Curve::Bezier>
    ("PGeom_BezierCurve");

  theMap.Bind <ShapePersistent_Geom_Curve::BSpline>
    ("PGeom_BSplineCurve");

  theMap.Bind <ShapePersistent_Geom_Curve::Trimmed>
    ("PGeom_TrimmedCurve");

  theMap.Bind <ShapePersistent_Geom_Curve::Offset>
    ("PGeom_OffsetCurve");

  theMap.Bind <ShapePersistent_Geom::Surface>
    ("PGeom_Surface");

  theMap.Bind <ShapePersistent_Geom_Surface::Elementary>
    ("PGeom_ElementarySurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Plane>
    ("PGeom_Plane");

  theMap.Bind <ShapePersistent_Geom_Surface::Conical>
    ("PGeom_ConicalSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Cylindrical>
    ("PGeom_CylindricalSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Spherical>
    ("PGeom_SphericalSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Toroidal>
    ("PGeom_ToroidalSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Swept>
    ("PGeom_SweptSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::LinearExtrusion>
    ("PGeom_SurfaceOfLinearExtrusion");

  theMap.Bind <ShapePersistent_Geom_Surface::Revolution>
    ("PGeom_SurfaceOfRevolution");

  theMap.Bind <ShapePersistent_Geom_Surface::Bounded>
    ("PGeom_BoundedSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Bezier>
    ("PGeom_BezierSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::BSpline>
    ("PGeom_BSplineSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::RectangularTrimmed>
    ("PGeom_RectangularTrimmedSurface");

  theMap.Bind <ShapePersistent_Geom_Surface::Offset>
    ("PGeom_OffsetSurface");

  theMap.Bind <ShapePersistent_Poly::Polygon2D>
    ("PPoly_Polygon2D");

  theMap.Bind <ShapePersistent_Poly::Polygon3D>
    ("PPoly_Polygon3D");

  theMap.Bind <ShapePersistent_Poly::PolygonOnTriangulation>
    ("PPoly_PolygonOnTriangulation");

  theMap.Bind <ShapePersistent_Poly::Triangulation>
    ("PPoly_Triangulation");

  theMap.Bind <ShapePersistent_BRep::PointRepresentation>
    ("PBRep_PointRepresentation");

  theMap.Bind <ShapePersistent_BRep::PointOnCurve>
    ("PBRep_PointOnCurve");

  theMap.Bind <ShapePersistent_BRep::PointsOnSurface>
    ("PBRep_PointsOnSurface");

  theMap.Bind <ShapePersistent_BRep::PointOnCurveOnSurface>
    ("PBRep_PointOnCurveOnSurface");

  theMap.Bind <ShapePersistent_BRep::PointOnSurface>
    ("PBRep_PointOnSurface");

  theMap.Bind <ShapePersistent_BRep::CurveRepresentation>
    ("PBRep_CurveRepresentation");

  theMap.Bind <ShapePersistent_BRep::GCurve>
    ("PBRep_GCurve");

  theMap.Bind <ShapePersistent_BRep::Curve3D>
    ("PBRep_Curve3D");

  theMap.Bind <ShapePersistent_BRep::CurveOnSurface>
    ("PBRep_CurveOnSurface");

  theMap.Bind <ShapePersistent_BRep::CurveOnClosedSurface>
    ("PBRep_CurveOnClosedSurface");

  theMap.Bind <ShapePersistent_BRep::Polygon3D>
    ("PBRep_Polygon3D");

  theMap.Bind <ShapePersistent_BRep::PolygonOnTriangulation>
    ("PBRep_PolygonOnTriangulation");

  theMap.Bind <ShapePersistent_BRep::PolygonOnClosedTriangulation>
    ("PBRep_PolygonOnClosedTriangulation");

  theMap.Bind <ShapePersistent_BRep::PolygonOnSurface>
    ("PBRep_PolygonOnSurface");

  theMap.Bind <ShapePersistent_BRep::PolygonOnClosedSurface>
    ("PBRep_PolygonOnClosedSurface");

  theMap.Bind <ShapePersistent_BRep::CurveOn2Surfaces>
    ("PBRep_CurveOn2Surfaces");

  theMap.Bind <ShapePersistent_BRep::TVertex>  ("PBRep_TVertex");
  theMap.Bind <ShapePersistent_BRep::TVertex1> ("PBRep_TVertex1");
  theMap.Bind <ShapePersistent_BRep::TEdge>    ("PBRep_TEdge");
  theMap.Bind <ShapePersistent_BRep::TEdge1>   ("PBRep_TEdge1");
  theMap.Bind <ShapePersistent_BRep::TFace>    ("PBRep_TFace");
  theMap.Bind <ShapePersistent_BRep::TFace1>   ("PBRep_TFace1");
}
