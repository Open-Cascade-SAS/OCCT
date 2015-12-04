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

#ifndef _DrawTrSurf_BSplineCurve2d_HeaderFile
#define _DrawTrSurf_BSplineCurve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Draw_MarkerShape.hxx>
#include <Draw_Color.hxx>
#include <Standard_Integer.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Standard_Real.hxx>
class Geom2d_BSplineCurve;
class Draw_Color;
class Draw_Display;
class Draw_Drawable3D;


class DrawTrSurf_BSplineCurve2d;
DEFINE_STANDARD_HANDLE(DrawTrSurf_BSplineCurve2d, DrawTrSurf_Curve2d)


class DrawTrSurf_BSplineCurve2d : public DrawTrSurf_Curve2d
{

public:

  

  //! creates a drawable BSpline curve from a BSpline curve of
  //! package Geom2d.
  Standard_EXPORT DrawTrSurf_BSplineCurve2d(const Handle(Geom2d_BSplineCurve)& C);
  
  Standard_EXPORT DrawTrSurf_BSplineCurve2d(const Handle(Geom2d_BSplineCurve)& C, const Draw_Color& CurvColor, const Draw_Color& PolesColor, const Draw_Color& KnotsColor, const Draw_MarkerShape KnotsShape, const Standard_Integer KnotsSize, const Standard_Boolean ShowPoles, const Standard_Boolean ShowKnots, const Standard_Integer Discret);
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT void ShowPoles();
  
  Standard_EXPORT void ShowKnots();
  
  Standard_EXPORT void ClearPoles();
  
  Standard_EXPORT void ClearKnots();
  
  //! Returns in <Index> the index of the first pole  of the
  //! curve projected by the Display <D> at a distance lower
  //! than <Prec> from <X,Y>. If no pole  is found  index is
  //! set to 0, else index is always  greater than the input
  //! value of index.
  Standard_EXPORT void FindPole (const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real Prec, Standard_Integer& Index) const;
  
  Standard_EXPORT void FindKnot (const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real Prec, Standard_Integer& Index) const;
  
    void SetPolesColor (const Draw_Color& aColor);
  
    void SetKnotsColor (const Draw_Color& aColor);
  
    void SetKnotsShape (const Draw_MarkerShape Shape);
  
    Draw_MarkerShape KnotsShape() const;
  
    Draw_Color KnotsColor() const;
  
    Draw_Color PolesColor() const;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_BSplineCurve2d,DrawTrSurf_Curve2d)

protected:




private:


  Standard_Boolean drawPoles;
  Standard_Boolean drawKnots;
  Draw_MarkerShape knotsForm;
  Draw_Color knotsLook;
  Standard_Integer knotsDim;
  Draw_Color polesLook;


};


#include <DrawTrSurf_BSplineCurve2d.lxx>





#endif // _DrawTrSurf_BSplineCurve2d_HeaderFile
