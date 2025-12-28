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

class DrawTrSurf_BezierSurface : public DrawTrSurf_Surface
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BezierSurface, DrawTrSurf_Surface)
  Draw_Drawable3D_FACTORY
public:
  //! creates a drawable Bezier curve from a Bezier curve of package Geom.
  Standard_EXPORT DrawTrSurf_BezierSurface(const occ::handle<Geom_BezierSurface>& S);

  Standard_EXPORT DrawTrSurf_BezierSurface(const occ::handle<Geom_BezierSurface>& S,
                                           const int                              NbUIsos,
                                           const int                              NbVIsos,
                                           const Draw_Color&                      BoundsColor,
                                           const Draw_Color&                      IsosColor,
                                           const Draw_Color&                      PolesColor,
                                           const bool                             ShowPoles,
                                           const int                              Discret,
                                           const double                           Deflection,
                                           const int                              DrawMode);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  void ShowPoles() { drawPoles = true; }

  void ClearPoles() { drawPoles = false; }

  Standard_EXPORT void FindPole(const double        X,
                                const double        Y,
                                const Draw_Display& D,
                                const double        Prec,
                                int&                UIndex,
                                int&                VIndex) const;

  void SetPolesColor(const Draw_Color& theColor) { polesLook = theColor; }

  Draw_Color PolesColor() const { return polesLook; }

  //! For variable copy.
  Standard_EXPORT occ::handle<Draw_Drawable3D> Copy() const override;

private:
  bool       drawPoles;
  Draw_Color polesLook;
};

#endif // _DrawTrSurf_BezierSurface_HeaderFile
