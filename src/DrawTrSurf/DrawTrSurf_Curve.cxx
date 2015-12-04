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
#include <DrawTrSurf_Curve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Curve,DrawTrSurf_Drawable)

Standard_Real DrawTrSurf_CurveLimit = 400;
extern Standard_Boolean Draw_Bounds;


//=======================================================================
//function : DrawTrSurf_Curve
//purpose  : 
//=======================================================================

DrawTrSurf_Curve::DrawTrSurf_Curve (const Handle(Geom_Curve)& C,
				    const Standard_Boolean DispOrigin) :
       DrawTrSurf_Drawable (16, 0.01, 1),
       curv(C),
       look(Draw_vert),
       disporigin(DispOrigin),
       dispcurvradius(Standard_False),
       radiusmax(1.0e3),
       radiusratio(0.1)
{
}



//=======================================================================
//function : DrawTrSurf_Curve
//purpose  : 
//=======================================================================

DrawTrSurf_Curve::DrawTrSurf_Curve (const Handle(Geom_Curve)& C, 
				    const Draw_Color& aColor, 
				    const Standard_Integer Discret,
				    const Standard_Real Deflection, 
				    const Standard_Integer DrawMode,
				    const Standard_Boolean DispOrigin,
				    const Standard_Boolean DispCurvRadius,
				    const Standard_Real  RadiusMax,
				    const Standard_Real  RadiusRatio) :
       DrawTrSurf_Drawable (Discret,Deflection, DrawMode),
       curv(C),
       look(aColor),
       disporigin(DispOrigin),
       dispcurvradius(DispCurvRadius),
       radiusmax(RadiusMax),
       radiusratio(RadiusRatio)
{
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Curve::DrawOn (Draw_Display& dis) const 
{
  Standard_Real First = curv->FirstParameter();
  Standard_Real Last  = curv->LastParameter();
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf) {
    gp_Pnt P1,P2;
    Standard_Real delta = 1;
    if (firstInf && lastInf) {
      do {
	delta *= 2;
	First = - delta;
	Last  =   delta;
	curv->D0(First,P1);
	curv->D0(Last,P2);
      } while (P1.Distance(P2) < DrawTrSurf_CurveLimit);
    }
    else if (firstInf) {
      curv->D0(Last,P2);
      do {
	delta *= 2;
	First = Last - delta;
	curv->D0(First,P1);
      } while (P1.Distance(P2) < DrawTrSurf_CurveLimit);
    }
    else if (lastInf) {
      curv->D0(First,P1);
      do {
	delta *= 2;
	Last = First + delta;
	curv->D0(Last,P2);
      } while (P1.Distance(P2) < DrawTrSurf_CurveLimit);
    }
  }    
  
  dis.SetColor (look);
  GeomAdaptor_Curve C(curv,First,Last);
  DrawCurveOn(C,dis);

  // mark the orientation
  if (disporigin) {
    Draw_Bounds = Standard_False;
    gp_Pnt P;
    gp_Vec V;
    C.D1(Last,P,V);
    gp_Pnt2d p1,p2;
    dis.Project(P,p1);
    P.Translate(V);
    dis.Project(P,p2);
    gp_Vec2d v(p1,p2);
    if (v.Magnitude() > gp::Resolution()) {
      Standard_Real L = 20 / dis.Zoom();
      Standard_Real H = 10 / dis.Zoom();
      gp_Dir2d d(v);
      p2.SetCoord(p1.X() - L*d.X() - H*d.Y(), p1.Y() - L*d.Y() + H*d.X());
      dis.MoveTo(p2);
      p2.SetCoord(p1.X() - L*d.X() + H*d.Y(), p1.Y() - L*d.Y() - H*d.X());
      dis.DrawTo(p1);
      dis.DrawTo(p2);
    }
    Draw_Bounds = Standard_True;
  }
// Draw the curvature Radius      
  if (dispcurvradius && (C.GetType() != GeomAbs_Line)) {
    Standard_Integer ii;
    Standard_Integer intrv, nbintv = C.NbIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,nbintv+1);
    C.Intervals(TI,GeomAbs_CN);
    Standard_Real Resolution = 1.0e-9, Curvature;
    GeomLProp_CLProps LProp(curv, 2, Resolution);
    gp_Pnt P1, P2;    

    for (intrv = 1; intrv <= nbintv; intrv++) {
	Standard_Real t = TI(intrv);
	Standard_Real step = (TI(intrv+1) - t) / GetDiscretisation();
	Standard_Real LRad, ratio;
	for (ii = 1; ii <= GetDiscretisation(); ii++) {	 
	  LProp.SetParameter(t);
          if (LProp.IsTangentDefined()) {
	     Curvature = Abs(LProp.Curvature());
	     if ( Curvature >  Resolution) {
	       curv->D0(t, P1);
	       dis.MoveTo(P1);
	       LRad = 1./Curvature;
	       ratio = ( (  LRad > radiusmax) ? radiusmax/LRad : 1 );
	       ratio *= radiusratio;
	       LProp.CentreOfCurvature(P2);
	       gp_Vec V(P1, P2);
	       dis.DrawTo(P1.Translated(ratio*V));
	    }
	   }
	   t += step;
	}
      }
  }
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_Curve::Copy()const 
{
  Handle(DrawTrSurf_Curve) DC = new DrawTrSurf_Curve
    (Handle(Geom_Curve)::DownCast(curv->Copy()),
     look,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DC;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  DrawTrSurf_Curve::Dump(Standard_OStream& S)const 
{
  GeomTools_CurveSet::PrintCurve(curv,S);
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  DrawTrSurf_Curve::Whatis(Draw_Interpretor& S)const 
{
  S << " a 3d curve";
}
