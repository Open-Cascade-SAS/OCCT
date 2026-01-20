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
//! The Drawable deferred classes is inherited from
//! the Drawable3D class from the package Draw, it
//! adds methods to draw 3D Curves and Curves on 3D
//! Surfaces.
//!
//! The classes Curve Curve2d and Surface are drawable
//! and can be used to draw a single curve from
//! packages Geom or Geom2d or a surface from Geom.
//!
//! The Triangulation and Polygon from the package Poly are also supported.
class DrawTrSurf
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets <G> in the variable <Name>. Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set(const char* const Name, const gp_Pnt& G);

  //! Sets <G> in the variable <Name>. Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set(const char* const Name, const gp_Pnt2d& G);

  //! Sets <G> in the variable <Name>. Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set(const char* const       Name,
                                  const occ::handle<Geom_Geometry>& G,
                                  const bool       isSenseMarker = true);

  template <class T>
  static void Set(const char* const Name,
                  const occ::handle<T>&       Arg,
                  typename opencascade::std::enable_if<
                    opencascade::std::is_base_of<Geom_Geometry, T>::value>::type* = 0)
  {
    Set(Name, (const occ::handle<Geom_Geometry>&)Arg);
  }

  //! Sets <C> in the variable <Name>. Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set(const char* const      Name,
                                  const occ::handle<Geom2d_Curve>& C,
                                  const bool      isSenseMarker = true);

  template <class T>
  static void Set(const char* const Name,
                  const occ::handle<T>&       Arg,
                  typename opencascade::std::enable_if<
                    opencascade::std::is_base_of<Geom2d_Curve, T>::value>::type* = 0)
  {
    Set(Name, (const occ::handle<Geom2d_Curve>&)Arg);
  }

  //! Sets <T> in the variable <Name>. Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set(const char* const Name, const occ::handle<Poly_Triangulation>& T);

  //! Sets <P> in the variable <Name>. Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set(const char* const Name, const occ::handle<Poly_Polygon3D>& P);

  //! Sets <P> in the variable <Name>. Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set(const char* const Name, const occ::handle<Poly_Polygon2D>& P);
  //! Getthe variable <S>. Returns a null handle if
  //!  none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_Geometry> Get(const char*& Name);

  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static bool GetPoint(const char*& Name, gp_Pnt& P);

  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static bool GetPoint2d(const char*& Name, gp_Pnt2d& P);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_Curve> GetCurve(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_BezierCurve> GetBezierCurve(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> GetBSplineCurve(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom2d_Curve> GetCurve2d(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom2d_BezierCurve> GetBezierCurve2d(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom2d_BSplineCurve> GetBSplineCurve2d(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_Surface> GetSurface(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_BezierSurface> GetBezierSurface(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> GetBSplineSurface(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Poly_Triangulation> GetTriangulation(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Poly_Polygon3D> GetPolygon3D(const char*& Name);

  //! Get the variable <S>. Returns a null handle if
  //! none, and print a warning message.
  Standard_EXPORT static occ::handle<Poly_Polygon2D> GetPolygon2D(const char*& Name);

  //! Return package global parameters.
  Standard_EXPORT static DrawTrSurf_Params& Parameters();

  //! defines display commands.
  Standard_EXPORT static void BasicCommands(Draw_Interpretor& I);
};

#endif // _DrawTrSurf_HeaderFile
