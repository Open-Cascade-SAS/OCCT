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

#ifndef _DrawTrSurf_BSplineSurface_HeaderFile
#define _DrawTrSurf_BSplineSurface_HeaderFile

#include <Draw_MarkerShape.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf_Surface.hxx>

class Geom_BSplineSurface;

DEFINE_STANDARD_HANDLE(DrawTrSurf_BSplineSurface, DrawTrSurf_Surface)

//! This class defines a drawable BSplineSurface.
//! With this class you can draw the control points and the knots
//! of the surface.
//! You can use the general class Surface from DrawTrSurf too,
//! if you just want to sea boundaries and isoparametric curves.
class DrawTrSurf_BSplineSurface : public DrawTrSurf_Surface
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BSplineSurface, DrawTrSurf_Surface)
  Draw_Drawable3D_FACTORY public :

      //! default drawing mode.
      //! The isoparametric curves corresponding to the knots values are drawn.
      //! The control points and the knots points are drawn.
      //! The boundaries are yellow, the isoparametric curves are blues.
      //! For the discretisation 50 points are computed in each parametric direction.
      Standard_EXPORT
      DrawTrSurf_BSplineSurface(const Handle(Geom_BSplineSurface)& S);

  //! The isoparametric curves corresponding to the knots values are drawn.
  Standard_EXPORT DrawTrSurf_BSplineSurface(const Handle(Geom_BSplineSurface)& S,
                                            const Draw_Color&                  BoundsColor,
                                            const Draw_Color&                  IsosColor,
                                            const Draw_Color&                  PolesColor,
                                            const Draw_Color&                  KnotsColor,
                                            const Draw_MarkerShape             KnotsShape,
                                            const Standard_Integer             KnotsSize,
                                            const Standard_Boolean             ShowPoles,
                                            const Standard_Boolean             ShowKnots,
                                            const Standard_Integer             Discret,
                                            const Standard_Real                Deflection,
                                            const Standard_Integer             DrawMode);

  //! Parametric equidistant iso curves are drawn.
  Standard_EXPORT DrawTrSurf_BSplineSurface(const Handle(Geom_BSplineSurface)& S,
                                            const Standard_Integer             NbUIsos,
                                            const Standard_Integer             NbVIsos,
                                            const Draw_Color&                  BoundsColor,
                                            const Draw_Color&                  IsosColor,
                                            const Draw_Color&                  PolesColor,
                                            const Draw_Color&                  KnotsColor,
                                            const Draw_MarkerShape             KnotsShape,
                                            const Standard_Integer             KnotsSize,
                                            const Standard_Boolean             ShowPoles,
                                            const Standard_Boolean             ShowKnots,
                                            const Standard_Integer             Discret,
                                            const Standard_Real                Deflection,
                                            const Standard_Integer             DrawMode);

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  void ShowPoles() { drawPoles = Standard_True; }

  void ShowKnots() { drawKnots = Standard_True; }

  //! change the number of isoparametric curves to be drawn.
  Standard_EXPORT virtual void ShowIsos(const Standard_Integer Nu,
                                        const Standard_Integer Nv) Standard_OVERRIDE;

  //! change the number of isoparametric curves to be drawn.
  Standard_EXPORT void ShowKnotsIsos();

  //! rub out all the isoparametric curves.
  Standard_EXPORT virtual void ClearIsos() Standard_OVERRIDE;

  void ClearPoles() { drawPoles = Standard_False; }

  void ClearKnots() { drawKnots = Standard_False; }

  Standard_EXPORT void FindPole(const Standard_Real X,
                                const Standard_Real Y,
                                const Draw_Display& D,
                                const Standard_Real Prec,
                                Standard_Integer&   UIndex,
                                Standard_Integer&   VIndex) const;

  Standard_EXPORT void FindUKnot(const Standard_Real X,
                                 const Standard_Real Y,
                                 const Draw_Display& D,
                                 const Standard_Real Prec,
                                 Standard_Integer&   UIndex) const;

  Standard_EXPORT void FindVKnot(const Standard_Real X,
                                 const Standard_Real Y,
                                 const Draw_Display& D,
                                 const Standard_Real Prec,
                                 Standard_Integer&   VIndex) const;

  void SetPolesColor(const Draw_Color& theColor) { polesLook = theColor; }

  void SetKnotsColor(const Draw_Color& theColor) { knotsLook = theColor; }

  void SetKnotsShape(const Draw_MarkerShape theShape) { knotsForm = theShape; }

  Draw_MarkerShape KnotsShape() const { return knotsForm; }

  Draw_Color KnotsColor() const { return knotsLook; }

  Draw_Color PolesColor() const { return polesLook; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

private:
  Standard_Boolean drawPoles;
  Standard_Boolean drawKnots;
  Standard_Boolean knotsIsos;
  Draw_MarkerShape knotsForm;
  Draw_Color       knotsLook;
  Standard_Integer knotsDim;
  Draw_Color       polesLook;
};

#endif // _DrawTrSurf_BSplineSurface_HeaderFile
