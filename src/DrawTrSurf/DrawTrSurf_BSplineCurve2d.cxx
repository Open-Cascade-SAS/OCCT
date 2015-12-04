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
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BSplineCurve2d,DrawTrSurf_Curve2d)

DrawTrSurf_BSplineCurve2d::DrawTrSurf_BSplineCurve2d (
   const Handle(Geom2d_BSplineCurve)& C) 
   : DrawTrSurf_Curve2d (C, Draw_vert, 100) {

      drawKnots = Standard_True;
      knotsForm = Draw_Losange;
      knotsLook = Draw_violet;
      knotsDim  = 5;
      drawPoles = Standard_True;
      polesLook = Draw_rouge;
   }



   DrawTrSurf_BSplineCurve2d::DrawTrSurf_BSplineCurve2d (
   const Handle(Geom2d_BSplineCurve)& C, const Draw_Color& CurvColor,
   const Draw_Color& PolesColor, const Draw_Color& KnotsColor,
   const Draw_MarkerShape KnotsShape, const Standard_Integer KnotsSize,
   const Standard_Boolean ShowPoles, const Standard_Boolean ShowKnots, const Standard_Integer Discret)
   : DrawTrSurf_Curve2d (C, CurvColor, Discret) {

      drawKnots = ShowKnots;
      knotsForm = KnotsShape;
      knotsLook = KnotsColor;
      knotsDim  = KnotsSize;
      drawPoles = ShowPoles;
      polesLook = PolesColor;
   }



void DrawTrSurf_BSplineCurve2d::DrawOn (Draw_Display& dis) const 
{

  Handle(Geom2d_BSplineCurve) C = Handle(Geom2d_BSplineCurve)::DownCast(curv);

  if (drawPoles) {
    Standard_Integer NbPoles = C->NbPoles();
    dis.SetColor(polesLook);
    TColgp_Array1OfPnt2d CPoles (1, NbPoles);
    C->Poles (CPoles);
    dis.MoveTo(CPoles(1));
    for (Standard_Integer i = 2; i <= NbPoles; i++) {
      dis.DrawTo(CPoles(i));
    }
    if (C->IsPeriodic())
      dis.DrawTo(CPoles(1));
  }

  DrawTrSurf_Curve2d::DrawOn(dis);
  
  if (drawKnots) {
    Standard_Integer NbKnots = C->NbKnots();
    TColStd_Array1OfReal CKnots (1, NbKnots);
    C->Knots (CKnots); 
    dis.SetColor(knotsLook);
    for (Standard_Integer i = 1; i <= NbKnots; i++) {
      gp_Pnt2d P = C->Value(CKnots(i));
      dis.DrawMarker (P, knotsForm, knotsDim);
    }
  }
}


   void DrawTrSurf_BSplineCurve2d::ShowPoles () { drawPoles = Standard_True; }


   void DrawTrSurf_BSplineCurve2d::ShowKnots () { drawKnots = Standard_True; }


   void DrawTrSurf_BSplineCurve2d::ClearPoles () { drawPoles = Standard_False; }


   void DrawTrSurf_BSplineCurve2d::ClearKnots () { drawKnots = Standard_False; }


   void DrawTrSurf_BSplineCurve2d::FindPole (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real XPrec,
   Standard_Integer& Index) const {

     Handle(Geom2d_BSplineCurve) bc = Handle(Geom2d_BSplineCurve)::DownCast(curv);
     Standard_Real Prec = XPrec / D.Zoom();
     gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
     Index++;
     Standard_Integer NbPoles = bc->NbPoles();
     gp_Pnt P;
     gp_Pnt2d P2d;
     while (Index <= NbPoles) {
       P2d = bc->Pole(Index);
       P.SetCoord (P2d.X(), P2d.Y(), 0.0);
       if (D.Project(P).Distance(p1) <= Prec)
	 return;
       Index++;
     }
     Index = 0;
   }


   void DrawTrSurf_BSplineCurve2d::FindKnot (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real Prec,
   Standard_Integer& Index) const {

     Handle(Geom2d_BSplineCurve) bc = Handle(Geom2d_BSplineCurve)::DownCast(curv);
     gp_Pnt2d P2d;
     gp_Pnt P;
     gp_Pnt2d p1(X,Y);
     Index++;
     Standard_Integer NbKnots = bc->NbKnots();
     while (Index <= NbKnots) {
       P2d = bc->Value(bc->Knot(Index));
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

Handle(Draw_Drawable3D)  DrawTrSurf_BSplineCurve2d::Copy()const 
{
  Handle(DrawTrSurf_BSplineCurve2d) DC = new DrawTrSurf_BSplineCurve2d
    (Handle(Geom2d_BSplineCurve)::DownCast(curv->Copy()),
     look,polesLook,knotsLook,knotsForm,knotsDim,
     drawPoles,drawKnots,
     GetDiscretisation());
     
  return DC;
}

