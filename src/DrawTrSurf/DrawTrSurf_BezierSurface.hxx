// Created on: 1992-05-22
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _DrawTrSurf_BezierSurface_HeaderFile
#define _DrawTrSurf_BezierSurface_HeaderFile

#include <Draw_Color.hxx>
#include <DrawTrSurf_Surface.hxx>

class Geom_BezierSurface;

DEFINE_STANDARD_HANDLE(DrawTrSurf_BezierSurface, DrawTrSurf_Surface)

class DrawTrSurf_BezierSurface : public DrawTrSurf_Surface
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BezierSurface, DrawTrSurf_Surface)
  Draw_Drawable3D_FACTORY
public:

  //! creates a drawable Bezier curve from a Bezier curve of package Geom.
  Standard_EXPORT DrawTrSurf_BezierSurface (const Handle(Geom_BezierSurface)& S);

  Standard_EXPORT DrawTrSurf_BezierSurface (const Handle(Geom_BezierSurface)& S,
                                            const Standard_Integer NbUIsos, const Standard_Integer NbVIsos, const Draw_Color& BoundsColor,
                                            const Draw_Color& IsosColor, const Draw_Color& PolesColor, const Standard_Boolean ShowPoles,
                                            const Standard_Integer Discret, const Standard_Real Deflection, const Standard_Integer DrawMode);

  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;

  void ShowPoles() { drawPoles = Standard_True; }

  void ClearPoles() { drawPoles = Standard_False; }

  Standard_EXPORT void FindPole (const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real Prec, Standard_Integer& UIndex, Standard_Integer& VIndex) const;

  void SetPolesColor (const Draw_Color& theColor) { polesLook = theColor; }

  Draw_Color PolesColor() const { return polesLook; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

private:

  Standard_Boolean drawPoles;
  Draw_Color polesLook;

};

#endif // _DrawTrSurf_BezierSurface_HeaderFile
