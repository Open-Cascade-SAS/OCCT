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

//! This class defines a drawable BSplineSurface.
//! With this class you can draw the control points and the knots
//! of the surface.
//! You can use the general class Surface from DrawTrSurf too,
//! if you just want to sea boundaries and isoparametric curves.
class DrawTrSurf_BSplineSurface : public DrawTrSurf_Surface
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BSplineSurface, DrawTrSurf_Surface)
  Draw_Drawable3D_FACTORY
public:
  //! default drawing mode.
  //! The isoparametric curves corresponding to the knots values are drawn.
  //! The control points and the knots points are drawn.
  //! The boundaries are yellow, the isoparametric curves are blues.
  //! For the discretisation 50 points are computed in each parametric direction.
  Standard_EXPORT DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S);

  //! The isoparametric curves corresponding to the knots values are drawn.
  Standard_EXPORT DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S,
                                            const Draw_Color&                       BoundsColor,
                                            const Draw_Color&                       IsosColor,
                                            const Draw_Color&                       PolesColor,
                                            const Draw_Color&                       KnotsColor,
                                            const Draw_MarkerShape                  KnotsShape,
                                            const int                               KnotsSize,
                                            const bool                              ShowPoles,
                                            const bool                              ShowKnots,
                                            const int                               Discret,
                                            const double                            Deflection,
                                            const int                               DrawMode);

  //! Parametric equidistant iso curves are drawn.
  Standard_EXPORT DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S,
                                            const int                               NbUIsos,
                                            const int                               NbVIsos,
                                            const Draw_Color&                       BoundsColor,
                                            const Draw_Color&                       IsosColor,
                                            const Draw_Color&                       PolesColor,
                                            const Draw_Color&                       KnotsColor,
                                            const Draw_MarkerShape                  KnotsShape,
                                            const int                               KnotsSize,
                                            const bool                              ShowPoles,
                                            const bool                              ShowKnots,
                                            const int                               Discret,
                                            const double                            Deflection,
                                            const int                               DrawMode);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  void ShowPoles() { drawPoles = true; }

  void ShowKnots() { drawKnots = true; }

  //! change the number of isoparametric curves to be drawn.
  Standard_EXPORT void ShowIsos(const int Nu, const int Nv) override;

  //! change the number of isoparametric curves to be drawn.
  Standard_EXPORT void ShowKnotsIsos();

  //! rub out all the isoparametric curves.
  Standard_EXPORT void ClearIsos() override;

  void ClearPoles() { drawPoles = false; }

  void ClearKnots() { drawKnots = false; }

  Standard_EXPORT void FindPole(const double        X,
                                const double        Y,
                                const Draw_Display& D,
                                const double        Prec,
                                int&                UIndex,
                                int&                VIndex) const;

  Standard_EXPORT void FindUKnot(const double        X,
                                 const double        Y,
                                 const Draw_Display& D,
                                 const double        Prec,
                                 int&                UIndex) const;

  Standard_EXPORT void FindVKnot(const double        X,
                                 const double        Y,
                                 const Draw_Display& D,
                                 const double        Prec,
                                 int&                VIndex) const;

  void SetPolesColor(const Draw_Color& theColor) { polesLook = theColor; }

  void SetKnotsColor(const Draw_Color& theColor) { knotsLook = theColor; }

  void SetKnotsShape(const Draw_MarkerShape theShape) { knotsForm = theShape; }

  Draw_MarkerShape KnotsShape() const { return knotsForm; }

  Draw_Color KnotsColor() const { return knotsLook; }

  Draw_Color PolesColor() const { return polesLook; }

  //! For variable copy.
  Standard_EXPORT occ::handle<Draw_Drawable3D> Copy() const override;

private:
  bool             drawPoles;
  bool             drawKnots;
  bool             knotsIsos;
  Draw_MarkerShape knotsForm;
  Draw_Color       knotsLook;
  int              knotsDim;
  Draw_Color       polesLook;
};

#endif // _DrawTrSurf_BSplineSurface_HeaderFile
