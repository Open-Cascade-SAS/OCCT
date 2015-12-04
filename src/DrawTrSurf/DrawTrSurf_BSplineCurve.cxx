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
#include <DrawTrSurf_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BSplineCurve,DrawTrSurf_Curve)

DrawTrSurf_BSplineCurve::DrawTrSurf_BSplineCurve (
   const Handle(Geom_BSplineCurve)& C) :
   DrawTrSurf_Curve (C, Draw_vert, 16, 0.05, 1) {

      drawKnots = Standard_True;
      knotsForm = Draw_Losange;
      knotsLook = Draw_violet;
      knotsDim  = 5;
      drawPoles = Standard_True;
      polesLook = Draw_rouge;
   }




   DrawTrSurf_BSplineCurve::DrawTrSurf_BSplineCurve (
   const Handle(Geom_BSplineCurve)& C, const Draw_Color& CurvColor,
   const Draw_Color& PolesColor, const Draw_Color& KnotsColor,
   const Draw_MarkerShape KnotsShape, const Standard_Integer KnotsSize,
   const Standard_Boolean ShowPoles, const Standard_Boolean ShowKnots, 
   const Standard_Integer Discret, const Standard_Real Deflection,
   const Standard_Integer DrawMode)
   : DrawTrSurf_Curve (C , CurvColor, Discret, Deflection, DrawMode) {   

      drawKnots = ShowKnots;
      knotsForm = KnotsShape;
      knotsLook = KnotsColor;
      knotsDim  = KnotsSize;
      drawPoles = ShowPoles;
      polesLook = PolesColor;
   }



  void DrawTrSurf_BSplineCurve::DrawOn (Draw_Display& dis) const {


    Handle(Geom_BSplineCurve) C = Handle(Geom_BSplineCurve)::DownCast(curv);
    if (drawPoles) {
      Standard_Integer NbPoles = C->NbPoles();
      dis.SetColor(polesLook);
      TColgp_Array1OfPnt CPoles (1, NbPoles);
      C->Poles (CPoles);
      dis.MoveTo(CPoles(1));
      for (Standard_Integer i = 2; i <= NbPoles; i++) {
        dis.DrawTo(CPoles(i));
      }
      if (C->IsPeriodic())
	dis.DrawTo(CPoles(1));
    }
    
    DrawTrSurf_Curve::DrawOn(dis);
    
    if (drawKnots) {
      Standard_Integer NbKnots = C->NbKnots();
      TColStd_Array1OfReal CKnots (1, NbKnots);
      C->Knots (CKnots); 
      dis.SetColor(knotsLook);
      Standard_Integer first = C->FirstUKnotIndex();
      Standard_Integer last  = C->LastUKnotIndex();
      for (Standard_Integer i = first; i <= last; i++) {
	dis.DrawMarker (C->Value (CKnots (i)), knotsForm, knotsDim);
      }
    }
  }


  void DrawTrSurf_BSplineCurve::DrawOn (
 
  Draw_Display& dis,
  const Standard_Boolean ShowPoles,
  const Standard_Boolean ShowKnots
  ) const {


    Handle(Geom_BSplineCurve) C = Handle(Geom_BSplineCurve)::DownCast(curv);
    if (drawPoles && ShowPoles) {
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

    if (drawKnots && ShowKnots) {
      Standard_Integer NbKnots = C->NbKnots();
      TColStd_Array1OfReal CKnots (1, NbKnots);
      C->Knots (CKnots); 
      dis.SetColor(knotsLook);
      for (Standard_Integer i = 1; i <= NbKnots; i++) {
	dis.DrawMarker (C->Value (CKnots (i)), knotsForm, knotsDim);
      }
    }
  }




  void DrawTrSurf_BSplineCurve::DrawOn (

   Draw_Display& dis, 
   const Standard_Real    U1,
   const Standard_Real    U2,
   const Standard_Integer Pindex,
   const Standard_Boolean ShowPoles,
   const Standard_Boolean ShowKnots
   ) const {
    

    Handle(Geom_BSplineCurve) C = Handle(Geom_BSplineCurve)::DownCast(curv);
    Standard_Real Eps1 = Abs(Epsilon (U1));
    Standard_Real Eps2 = Abs(Epsilon (U2));
    Standard_Integer I1, J1, I2, J2;
    C->LocateU (U1, Eps1, I1, J1);
    C->LocateU (U2, Eps2, I2, J2);
    Standard_Integer ka  = C->FirstUKnotIndex ();
    Standard_Integer kb  = C->LastUKnotIndex ();


    if (drawPoles && ShowPoles) {
      Standard_Integer NbPoles = C->NbPoles();
      dis.SetColor(polesLook);
      TColgp_Array1OfPnt CPoles (1, NbPoles);
      C->Poles (CPoles);
      if (Pindex == 0) {
        dis.MoveTo(CPoles(1));
        for (Standard_Integer i = 2; i <= NbPoles; i++) {
          dis.DrawTo(CPoles(i));
        }
      }
      else if (Pindex == 1) {
        dis.MoveTo(CPoles(1));
        dis.DrawTo(CPoles(2));        
      }
      else if (Pindex == NbPoles) {
        dis.MoveTo(CPoles(NbPoles-1));
        dis.DrawTo(CPoles(NbPoles));
      }
      else {
        dis.MoveTo(CPoles(Pindex-1));
        dis.DrawTo(CPoles(Pindex));
        dis.DrawTo(CPoles(Pindex+1));
      }
    }



    dis.SetColor(look);
    Standard_Integer Degree = C->Degree();

    if (Degree == 1) {    
      dis.MoveTo(C->Value(U1));
      dis.DrawTo(C->Value(U2));
    }
    else {
      Standard_Integer NbPoints;
      Standard_Integer Discret = GetDiscretisation();
      Standard_Real Ustart = C->Knot (ka);
      Standard_Real Uend   = C->Knot (kb);
      Standard_Real Du, U, Ua, Ub, Uk1, Uk2;

      if (I1 > ka)  { ka = I1;  Uk1 = U1; }
      else {
        U = U1;
        NbPoints = (Standard_Integer) Abs (Discret * (U1 - Ustart) / (Ustart - Uend));
        NbPoints = Max (NbPoints, 30);
        Du = (Ustart - U1) / NbPoints;
        dis.MoveTo(C->Value (U));
        for (Standard_Integer i = 1; i <= NbPoints - 2; i++) {
           U+= Du;
           dis.DrawTo(C->Value (U));
        }
        dis.DrawTo(C->Value (Ustart));
        Uk1 = Ustart;
      }

      if (J2 < kb)  { kb = J2;  Uk2 = U2; }
      else {
        Uk2 = Uend;
        U   = Uend;
        NbPoints = (Standard_Integer) Abs (Discret * (U2 - Uend) / (Ustart - Uend));
        NbPoints = Max (NbPoints, 30);
        Du = (U2 - Uend) / NbPoints;
        dis.MoveTo(C->Value (U));
        for (Standard_Integer i = 1; i <= NbPoints - 2; i++) {
           U+= Du;
           dis.DrawTo(C->Value (U));
        }
        dis.DrawTo(C->Value (U2));
      }


      for (Standard_Integer k = ka; k < kb; k++) {
        if (k == ka) { 
          Ua = Uk1;
          Ub = C->Knot (k+1);
        }
        else if (k == kb-1) {
          Ua = C->Knot (k);
          Ub = Uk2;
	}
        else {
          Ua = C->Knot (k);
          Ub = C->Knot (k+1);
	}
        U  = Ua;
        NbPoints = (Standard_Integer) Abs (Discret * (Ua - Ub) / (Ustart - Uend));
        NbPoints = Max (NbPoints, 30);
        Du = (Ub - Ua) / NbPoints;
        dis.MoveTo(C->Value (U));
        for (Standard_Integer i = 1; i <= NbPoints - 2; i++) {
           U+= Du;
           dis.DrawTo(C->Value (U));
        }
        dis.DrawTo(C->Value (Ub));
      }
    }


    if (drawKnots && ShowKnots) {
      Standard_Integer NbKnots = C->NbKnots();
      TColStd_Array1OfReal CKnots (1, NbKnots);
      C->Knots (CKnots); 
      dis.SetColor(knotsLook);
      for (Standard_Integer i = J1; i <= I2; i++) {
	dis.DrawMarker (C->Value (CKnots (i)), knotsForm, knotsDim);
      }
    }

   }



   void DrawTrSurf_BSplineCurve::ShowPoles () { drawPoles = Standard_True;  }


   void DrawTrSurf_BSplineCurve::ShowKnots () { drawKnots = Standard_True;  }


   void DrawTrSurf_BSplineCurve::ClearPoles () { drawPoles = Standard_False; }


   void DrawTrSurf_BSplineCurve::ClearKnots () { drawKnots = Standard_False; }


   void DrawTrSurf_BSplineCurve::FindPole (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real XPrec,
   Standard_Integer& Index) const {

     Handle(Geom_BSplineCurve) bc = Handle(Geom_BSplineCurve)::DownCast(curv);
     Standard_Real Prec = XPrec / D.Zoom();
     gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
     Index++;
     Standard_Integer NbPoles = bc->NbPoles();
     while (Index <= NbPoles) {
       if (D.Project(bc->Pole(Index)).Distance(p1) <= Prec)
	 return;
       Index++;
     }
     Index = 0;
   }


   void DrawTrSurf_BSplineCurve::FindKnot (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D, const Standard_Real Prec,
   Standard_Integer& Index) const {

     Handle(Geom_BSplineCurve) bc = Handle(Geom_BSplineCurve)::DownCast(curv);
     gp_Pnt2d p1(X,Y);
     Index++;
     Standard_Integer NbKnots = bc->NbKnots();
     while (Index <= NbKnots) {
       if (D.Project(bc->Value(bc->Knot(Index))).Distance(p1) <= Prec)
	 return;
       Index++;
     }
     Index = 0;
   }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_BSplineCurve::Copy()const 
{
  Handle(DrawTrSurf_BSplineCurve) DC = new DrawTrSurf_BSplineCurve
    (Handle(Geom_BSplineCurve)::DownCast(curv->Copy()),
     look,polesLook,knotsLook,knotsForm,knotsDim,
     drawPoles,drawKnots,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DC;
}


