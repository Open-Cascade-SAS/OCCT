// Created on: 1994-03-28
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _DrawTrSurf_Point_HeaderFile
#define _DrawTrSurf_Point_HeaderFile

#include <gp_Pnt.hxx>
#include <Draw_MarkerShape.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>

DEFINE_STANDARD_HANDLE(DrawTrSurf_Point, Draw_Drawable3D)

//! A drawable point.
class DrawTrSurf_Point : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Point, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY public :

      Standard_EXPORT
      DrawTrSurf_Point(const gp_Pnt& P, const Draw_MarkerShape Shape, const Draw_Color& Col);

  Standard_EXPORT DrawTrSurf_Point(const gp_Pnt2d&        P,
                                   const Draw_MarkerShape Shape,
                                   const Draw_Color&      Col);

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  //! Is a 3D object. (Default True).
  Standard_EXPORT virtual Standard_Boolean Is3D() const Standard_OVERRIDE;

  gp_Pnt Point() const { return myPoint; }

  Standard_EXPORT void Point(const gp_Pnt& P);

  gp_Pnt2d Point2d() const { return gp_Pnt2d(myPoint.X(), myPoint.Y()); }

  Standard_EXPORT void Point2d(const gp_Pnt2d& P);

  void Color(const Draw_Color& theColor) { myColor = theColor; }

  Draw_Color Color() const { return myColor; }

  void Shape(const Draw_MarkerShape theS) { myShape = theS; }

  Draw_MarkerShape Shape() const { return myShape; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const Standard_OVERRIDE;

private:
  gp_Pnt           myPoint;
  Standard_Boolean is3D;
  Draw_MarkerShape myShape;
  Draw_Color       myColor;
};

#endif // _DrawTrSurf_Point_HeaderFile
