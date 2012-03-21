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

#include <DrawTrSurf_BezierSurface.ixx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

DrawTrSurf_BezierSurface::DrawTrSurf_BezierSurface (
   const Handle(Geom_BezierSurface)& S)
   : DrawTrSurf_Surface (S, 1, 1, Draw_jaune, Draw_bleu, 30, 0.05, 0) {

     drawPoles = Standard_True;
     polesLook  = Draw_rouge;
   }



   DrawTrSurf_BezierSurface::DrawTrSurf_BezierSurface (
   const Handle(Geom_BezierSurface)& S,
   const Standard_Integer NbUIsos, const Standard_Integer NbVIsos,
   const Draw_Color& BoundsColor, const Draw_Color& IsosColor, 
   const Draw_Color& PolesColor, const Standard_Boolean ShowPoles,
   const Standard_Integer Discret,const Standard_Real Deflection,
   const Standard_Integer DrawMode)
   : DrawTrSurf_Surface (S, NbUIsos, NbVIsos, BoundsColor, IsosColor, 
     Discret, Deflection, DrawMode){

     drawPoles = ShowPoles;
     polesLook  = PolesColor;
   }



   void DrawTrSurf_BezierSurface::DrawOn (Draw_Display& dis) const {

    Standard_Integer i,j;
    Handle(Geom_BezierSurface) S = Handle(Geom_BezierSurface)::DownCast(surf);

    if (drawPoles) {
      Standard_Integer NbUPoles = S->NbUPoles();
      Standard_Integer NbVPoles = S->NbVPoles();
      dis.SetColor(polesLook);
      TColgp_Array2OfPnt SPoles (1, NbUPoles, 1, NbVPoles);
      S->Poles (SPoles);
      for (j = 1; j <= NbVPoles; j++) {
        dis.MoveTo(SPoles(1, j));
        for (i = 2; i <= NbUPoles; i++) {
          dis.DrawTo(SPoles(i, j));
        }
      }
      for (i = 1; i <= NbUPoles; i++) {
        dis.MoveTo(SPoles(i, 1));
        for (j = 2; j <= NbVPoles; j++) {
          dis.DrawTo(SPoles(i, j));
        }
      }
    }


    DrawTrSurf_Surface::DrawOn(dis);
  }


   void DrawTrSurf_BezierSurface::ShowPoles () { drawPoles = Standard_True; }


   void DrawTrSurf_BezierSurface::ClearPoles () { drawPoles = Standard_False; }


   void DrawTrSurf_BezierSurface::FindPole (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D,
   const Standard_Real XPrec, Standard_Integer& UIndex, Standard_Integer& VIndex) const {

     Handle(Geom_BezierSurface) bs = Handle(Geom_BezierSurface)::DownCast(surf);
     gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
     Standard_Real Prec = XPrec / D.Zoom();
     UIndex++;
     VIndex++;
     Standard_Integer NbUPoles = bs->NbUPoles();
     Standard_Integer NbVPoles = bs->NbVPoles();
     while (VIndex <= NbVPoles) {
       while (UIndex <= NbUPoles) {
         if (D.Project(bs->Pole(UIndex, VIndex)).Distance(p1) <= Prec)
	   return;
         UIndex++;
       }
       UIndex = 1;
       VIndex++;
     }
     UIndex = VIndex = 0;
   }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_BezierSurface::Copy()const 
{
  Handle(DrawTrSurf_BezierSurface) DS = new DrawTrSurf_BezierSurface
    (Handle(Geom_BezierSurface)::DownCast(surf->Copy()),
     nbUIsos,nbVIsos,
     boundsLook,isosLook,polesLook,drawPoles,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DS;
}







