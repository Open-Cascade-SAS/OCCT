// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <DrawTrSurf_BezierCurve.ixx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve (
   const Handle(Geom_BezierCurve)& C) 
   : DrawTrSurf_Curve (C, Draw_vert, 16, 0.05, 1) {

      drawPoles = Standard_True;
      polesLook = Draw_rouge;
   }



   DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve (
   const Handle(Geom_BezierCurve)& C, const Draw_Color& CurvColor,
   const Draw_Color& PolesColor, const Standard_Boolean ShowPoles, 
   const Standard_Integer Discret,const Standard_Real Deflection,
   const Standard_Integer DrawMode ) : 
   DrawTrSurf_Curve (C, CurvColor, Discret, Deflection, DrawMode) {

      drawPoles = ShowPoles;
      polesLook = PolesColor;
  }


   void DrawTrSurf_BezierCurve::DrawOn (Draw_Display& dis) const {


    Handle(Geom_BezierCurve) C = Handle(Geom_BezierCurve)::DownCast(curv);

    if (drawPoles) {
      Standard_Integer NbPoles = C->NbPoles();
      dis.SetColor(polesLook);
      TColgp_Array1OfPnt CPoles (1, NbPoles);
      C->Poles (CPoles);
      dis.MoveTo(CPoles(1));
      for (Standard_Integer i = 2; i <= NbPoles; i++) {
        dis.DrawTo(CPoles(i));
      }
    }

    DrawTrSurf_Curve::DrawOn(dis);
  }



   void DrawTrSurf_BezierCurve::ShowPoles () { drawPoles = Standard_True; }


   void DrawTrSurf_BezierCurve::ClearPoles () { drawPoles = Standard_False; }


   void DrawTrSurf_BezierCurve::FindPole (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D,
   const Standard_Real XPrec, Standard_Integer& Index) const {
    
     Handle(Geom_BezierCurve) bz = Handle(Geom_BezierCurve)::DownCast(curv);
     gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
     Standard_Real Prec = XPrec / D.Zoom();
     Index++;
     Standard_Integer NbPoles = bz->NbPoles();
     while (Index <= NbPoles) {
       if (D.Project(bz->Pole(Index)).Distance(p1) <= Prec)
	 return;
       Index++;
     }
     Index = 0;
   }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_BezierCurve::Copy()const 
{
  Handle(DrawTrSurf_BezierCurve) DC = new DrawTrSurf_BezierCurve
    (Handle(Geom_BezierCurve)::DownCast(curv->Copy()),
     look,polesLook,
     drawPoles,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DC;
}





