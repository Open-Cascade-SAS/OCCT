// File:	TestTopOpeDraw_TTOT.hxx
// Created:	Fri Sep 20 17:28:34 1996
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TestTopOpeDraw_TTOT_HeaderFile
#define _TestTopOpeDraw_TTOT_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Draw.hxx>
#include <Draw_Color.hxx>

class TestTopOpeDraw_TTOT {

public:

  static Standard_EXPORT void ShapeEnumToString
    (const TopAbs_ShapeEnum T, TCollection_AsciiString& N);

  static void OrientationToString
    (const TopAbs_Orientation o, TCollection_AsciiString& N);

  static Standard_EXPORT Standard_Boolean StringToKind
    (const TCollection_AsciiString& N, TopOpeBRepDS_Kind& T);

  static void KindToString
    (const TopOpeBRepDS_Kind T, TCollection_AsciiString& N);

  static Standard_Boolean StringToShapeEnum
    (const TCollection_AsciiString& N, TopAbs_ShapeEnum& T);

  static Draw_ColorKind ShapeColor
    (const TopoDS_Shape& S);

  static Draw_ColorKind KindColor
    (const TopOpeBRepDS_Kind K);

  static Standard_EXPORT Draw_ColorKind GeometryColor
    (const TopOpeBRepDS_Kind K);

  static void CurveToString
    (const GeomAbs_CurveType t, TCollection_AsciiString& N);

  static void SurfaceToString
    (const GeomAbs_SurfaceType t, TCollection_AsciiString& N);

  static Standard_EXPORT void GeometryName
    (const Standard_Integer  I, const TopOpeBRepDS_Kind K, TCollection_AsciiString& N);

  static  Standard_EXPORT void CatCurveName
    (const Standard_Integer I, TCollection_AsciiString& N);

  static void CatOrientation
    (const TopAbs_Orientation o,TCollection_AsciiString& N);

  static void CatOrientation
    (const TopoDS_Shape& S,TCollection_AsciiString& N);

  static void CatCurve
    (const Handle(Geom_Curve) GC, TCollection_AsciiString& N);

  static void CatSurface
    (const Handle(Geom_Surface) GS, TCollection_AsciiString& N);

  static void CatGeometry
    (const TopoDS_Shape& S, TCollection_AsciiString& N);

  static  Standard_EXPORT void CatCurveDisplayName
    (const Standard_Integer I, const Handle(Geom_Curve) GC, TCollection_AsciiString& N);

  static Standard_EXPORT void CurveDisplayName
    (const Standard_Integer I, const Handle(Geom_Curve) GC, TCollection_AsciiString& N);

  static Standard_Boolean Standard_EXPORT ShapeKind
    (const TopoDS_Shape& S,const TopAbs_ShapeEnum TS);

};


#endif
