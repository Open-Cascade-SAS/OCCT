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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <DrawTrSurf_BezierCurve2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BezierCurve2d,DrawTrSurf_Curve2d)

DrawTrSurf_BezierCurve2d::DrawTrSurf_BezierCurve2d (
   const Handle(Geom2d_BezierCurve)& C) :
   DrawTrSurf_Curve2d (C, Draw_vert, 50) {

      drawPoles = Standard_True;
      polesLook = Draw_rouge;
   }


   DrawTrSurf_BezierCurve2d::DrawTrSurf_BezierCurve2d (
   const Handle(Geom2d_BezierCurve)& C, const Draw_Color& CurvColor,
   const Draw_Color& PolesColor, const Standard_Boolean ShowPoles,
   const Standard_Integer Discret) : DrawTrSurf_Curve2d (C, CurvColor, Discret) {

      drawPoles = ShowPoles;
      polesLook = PolesColor;
   }


void DrawTrSurf_BezierCurve2d::DrawOn (Draw_Display& dis) const 
{
  
  Handle(Geom2d_BezierCurve) C = Handle(Geom2d_BezierCurve)::DownCast(curv);
  
  if (drawPoles) {
    dis.SetColor(polesLook);
    TColgp_Array1OfPnt2d CPoles (1, C->NbPoles());
    C->Poles (CPoles);
    dis.MoveTo(CPoles(1));
    for (Standard_Integer i = 2; i <= C->NbPoles(); i++) {
      dis.DrawTo(CPoles(i));
    }
  }
  
  DrawTrSurf_Curve2d::DrawOn(dis);
  
}



void DrawTrSurf_BezierCurve2d::ShowPoles () 
{
  drawPoles = Standard_True; 
}


void DrawTrSurf_BezierCurve2d::ClearPoles () 
{
  drawPoles = Standard_False; 
}


void DrawTrSurf_BezierCurve2d::FindPole (
					 const Standard_Real X, 
					 const Standard_Real Y, 
					 const Draw_Display& D,
					 const Standard_Real XPrec, 
					 Standard_Integer& Index) const 
{

  Handle(Geom2d_BezierCurve) bz = Handle(Geom2d_BezierCurve)::DownCast(curv);
  gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
  Standard_Real Prec = XPrec / D.Zoom();
  Index++;
  Standard_Integer NbPoles = bz->NbPoles();
  gp_Pnt P;
  gp_Pnt2d P2d;
  while (Index <= NbPoles) {
    P2d = bz->Pole(Index);
    P.SetCoord (P2d.X(), P2d.Y(), 0.0);
    if (D.Project(P).Distance(p1) <= Prec)
      return;
    Index++;
  }
  Index = 0;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_BezierCurve2d::Copy()const 
{
  Handle(DrawTrSurf_BezierCurve2d) DC = new DrawTrSurf_BezierCurve2d
    (Handle(Geom2d_BezierCurve)::DownCast(curv->Copy()),
     look,polesLook,
     drawPoles,
     GetDiscretisation());
     
  return DC;
}
