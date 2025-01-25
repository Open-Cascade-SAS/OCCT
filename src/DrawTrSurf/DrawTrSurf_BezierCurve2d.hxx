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

#ifndef _DrawTrSurf_BezierCurve2d_HeaderFile
#define _DrawTrSurf_BezierCurve2d_HeaderFile

#include <Draw_Color.hxx>
#include <DrawTrSurf_Curve2d.hxx>

class Geom2d_BezierCurve;

DEFINE_STANDARD_HANDLE(DrawTrSurf_BezierCurve2d, DrawTrSurf_Curve2d)

class DrawTrSurf_BezierCurve2d : public DrawTrSurf_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BezierCurve2d, DrawTrSurf_Curve2d)
  Draw_Drawable3D_FACTORY public :

      //! creates a drawable Bezier curve from a Bezier curve of package Geom2d.
      Standard_EXPORT
      DrawTrSurf_BezierCurve2d(const Handle(Geom2d_BezierCurve)& C);

  Standard_EXPORT DrawTrSurf_BezierCurve2d(const Handle(Geom2d_BezierCurve)& C,
                                           const Draw_Color&                 CurvColor,
                                           const Draw_Color&                 PolesColor,
                                           const Standard_Boolean            ShowPoles,
                                           const Standard_Integer            Discret);

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  void ShowPoles() { drawPoles = Standard_True; }

  void ClearPoles() { drawPoles = Standard_False; }

  //! Returns in <Index> the index of the first pole  of the
  //! curve projected by the Display <D> at a distance lower
  //! than <Prec> from <X,Y>. If no pole  is found  index is
  //! set to 0, else index is always  greater than the input
  //! value of index.
  Standard_EXPORT void FindPole(const Standard_Real X,
                                const Standard_Real Y,
                                const Draw_Display& D,
                                const Standard_Real Prec,
                                Standard_Integer&   Index) const;

  void SetPolesColor(const Draw_Color& theColor) { polesLook = theColor; }

  Draw_Color PolesColor() const { return polesLook; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

private:
  Standard_Boolean drawPoles;
  Draw_Color       polesLook;
};

#endif // _DrawTrSurf_BezierCurve2d_HeaderFile
