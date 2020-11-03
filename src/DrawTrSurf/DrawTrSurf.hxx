// Created on: 1991-06-24
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _DrawTrSurf_HeaderFile
#define _DrawTrSurf_HeaderFile

#include <Draw_Interpretor.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>

class Geom_BezierCurve;
class Geom_BezierSurface;
class Geom_BSplineCurve;
class Geom_BSplineSurface;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;
class Poly_Polygon2D;
class Poly_Polygon3D;
class Poly_Triangulation;
struct DrawTrSurf_Params;

//! This package supports the display of parametric curves and surfaces.
//!
//! The  Drawable deferred  classes is  inherited from
//! the Drawable3D  class  from  the package Draw,  it
//! adds methods to  draw 3D Curves  and  Curves on 3D
//! Surfaces.
//!
//! The classes Curve Curve2d and Surface are drawable
//! and  can be  used  to  draw a   single  curve from
//! packages Geom or Geom2d or a surface from Geom.
//!
//! The Triangulation and Polygon from the package Poly are also supported.
class DrawTrSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const gp_Pnt& G);
  
  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const gp_Pnt2d& G);
  
  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Geom_Geometry)& G, const Standard_Boolean isSenseMarker = Standard_True);
template <class T> static void Set (const Standard_CString Name, const Handle(T)& Arg, typename opencascade::std::enable_if<opencascade::std::is_base_of<Geom_Geometry, T>::value>::type * = 0) { Set (Name, (const Handle(Geom_Geometry)&)Arg); }
  
  //! Sets <C> in the variable <Name>.  Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Geom2d_Curve)& C, const Standard_Boolean isSenseMarker = Standard_True);
template <class T> static void Set (const Standard_CString Name, const Handle(T)& Arg, typename opencascade::std::enable_if<opencascade::std::is_base_of<Geom2d_Curve, T>::value>::type * = 0) { Set (Name, (const Handle(Geom2d_Curve)&)Arg); }
  
  //! Sets <T> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Triangulation)& T);
  
  //! Sets <P> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Polygon3D)& P);
  
  //! Sets <P> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Polygon2D)& P);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Geometry) Get (Standard_CString& Name);
  
  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static Standard_Boolean GetPoint (Standard_CString& Name, gp_Pnt& P);
  
  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static Standard_Boolean GetPoint2d (Standard_CString& Name, gp_Pnt2d& P);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Curve) GetCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BezierCurve) GetBezierCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BSplineCurve) GetBSplineCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_Curve) GetCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_BezierCurve) GetBezierCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) GetBSplineCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Surface) GetSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BezierSurface) GetBezierSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BSplineSurface) GetBSplineSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Triangulation) GetTriangulation (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Polygon3D) GetPolygon3D (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Polygon2D) GetPolygon2D (Standard_CString& Name);

  //! Return package global parameters.
  Standard_EXPORT static DrawTrSurf_Params& Parameters();

  //! defines display commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& I);

};

#endif // _DrawTrSurf_HeaderFile
