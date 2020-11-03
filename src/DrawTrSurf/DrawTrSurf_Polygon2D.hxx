// Created on: 1995-03-10
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _DrawTrSurf_Polygon2D_HeaderFile
#define _DrawTrSurf_Polygon2D_HeaderFile

#include <Draw_Drawable2D.hxx>
#include <Draw_Interpretor.hxx>

class Poly_Polygon2D;

DEFINE_STANDARD_HANDLE(DrawTrSurf_Polygon2D, Draw_Drawable2D)

//! Used to display a 2d polygon.
//! Optional display of nodes.
class DrawTrSurf_Polygon2D : public Draw_Drawable2D
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Polygon2D, Draw_Drawable2D)
  Draw_Drawable3D_FACTORY
public:

  Standard_EXPORT DrawTrSurf_Polygon2D (const Handle(Poly_Polygon2D)& P);

  Handle(Poly_Polygon2D) Polygon2D() const { return myPolygon2D; }

  void ShowNodes (const Standard_Boolean theB) { myNodes = theB; }

  Standard_Boolean ShowNodes() const { return myNodes; }

  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save (Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;

private:

  Handle(Poly_Polygon2D) myPolygon2D;
  Standard_Boolean myNodes;

};

#endif // _DrawTrSurf_Polygon2D_HeaderFile
