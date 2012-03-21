// Created on: 1991-07-16
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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



#include <DrawTrSurf_Drawable.ixx>
#include <GCPnts_UniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Precision.hxx>


//=======================================================================
//function : DrawTrSurf_Drawable
//purpose  : initialise the discretisation
//=======================================================================

DrawTrSurf_Drawable::DrawTrSurf_Drawable (

const Standard_Integer discret,
const Standard_Real    deflection,
const Standard_Integer DrawMode ) :
       myDrawMode (DrawMode),
       myDiscret(discret),
       myDeflection(deflection)
{
}

//=======================================================================
//function : DrawCurve2dOn
//purpose  : draw a 2D curve
//=======================================================================

void DrawTrSurf_Drawable::DrawCurve2dOn (Adaptor2d_Curve2d&   C, 
					 Draw_Display& aDisplay) const
{
  gp_Pnt P;
  
  gp_Pnt2d aPoint2d,
  *aPoint2dPtr ;
  if (myDrawMode == 1) {
    Standard_Real Fleche = myDeflection/aDisplay.Zoom();
    GCPnts_UniformDeflection LineVu(C,Fleche);
    if (LineVu.IsDone()) {
      P = LineVu.Value(1) ;
      aPoint2dPtr = (gp_Pnt2d *) &P ;
      aDisplay.MoveTo(*aPoint2dPtr);
      for (Standard_Integer i = 2; i <= LineVu.NbPoints(); i++) {
        P = LineVu.Value(i) ;
        aPoint2dPtr = (gp_Pnt2d *) &P ;
	aDisplay.DrawTo(*aPoint2dPtr);
      }
    }  
  }
  else {
    Standard_Integer intrv, nbintv = C.NbIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,nbintv+1);
    C.Intervals(TI,GeomAbs_CN);
    C.D0(C.FirstParameter(),aPoint2d);
    aDisplay.MoveTo(aPoint2d);
    for (intrv = 1; intrv <= nbintv; intrv++) {
      if (C.GetType() != GeomAbs_Line) {
	Standard_Real t = TI(intrv);
	Standard_Real step = (TI(intrv+1) - t) / myDiscret;
	for (Standard_Integer i = 1; i < myDiscret; i++) {
	  t += step;
	  C.D0(t,aPoint2d);
	  aDisplay.DrawTo(aPoint2d);
	}
      }
      C.D0(TI(intrv+1),aPoint2d);
      aDisplay.DrawTo(aPoint2d);
    }
  }
}

//=======================================================================
//static function : PlotCurve
//purpose  : draw a 3D curve
//=======================================================================
static void PlotCurve (Draw_Display& aDisplay,
		       const Adaptor3d_Curve& C, 
		       Standard_Real& theFirstParam, 
		       Standard_Real  theHalfStep,
		       const gp_Pnt&  theFirstPnt,
		       const gp_Pnt&  theLastPnt)
{
  Standard_Real IsoRatio = 1.001;
  gp_Pnt Pm;

  C.D0 (theFirstParam + theHalfStep, Pm);
  
  Standard_Real dfLength = theFirstPnt.Distance(theLastPnt);
  if (dfLength < Precision::Confusion() || 
      Pm.Distance(theFirstPnt) + Pm.Distance(theLastPnt) <= IsoRatio*dfLength) {
    aDisplay.DrawTo (theLastPnt);
  } else {
    PlotCurve (aDisplay, C, theFirstParam, theHalfStep/2., theFirstPnt, Pm);
    Standard_Real aLocalF = theFirstParam + theHalfStep;
    PlotCurve (aDisplay, C, aLocalF, theHalfStep/2., Pm, theLastPnt);
  }
}
//=======================================================================
//function : DrawCurveOn
//purpose  : draw a 3D curve
//=======================================================================

void DrawTrSurf_Drawable::DrawCurveOn (Adaptor3d_Curve&   C, 
				       Draw_Display& aDisplay) const
{
  gp_Pnt P;
  if (myDrawMode == 1) {
    Standard_Real Fleche = myDeflection/aDisplay.Zoom();
    GCPnts_UniformDeflection LineVu(C,Fleche);
    if (LineVu.IsDone()) {
      aDisplay.MoveTo(LineVu.Value(1));
      for (Standard_Integer i = 2; i <= LineVu.NbPoints(); i++) {
	aDisplay.DrawTo(LineVu.Value(i));
      }
    }  
  }
  else {
    Standard_Real j;
    Standard_Integer intrv, nbintv = C.NbIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,nbintv+1);
    C.Intervals(TI,GeomAbs_CN);
    C.D0(C.FirstParameter(),P);
    aDisplay.MoveTo(P);
    GeomAbs_CurveType CurvType = C.GetType();
    gp_Pnt aPPnt=P, aNPnt;

    for (intrv = 1; intrv <= nbintv; intrv++) {
      Standard_Real t = TI(intrv);
      Standard_Real step = (TI(intrv+1) - t) / myDiscret;

      switch (CurvType) {
      case GeomAbs_Line :
	break;
      case GeomAbs_Circle :
      case GeomAbs_Ellipse :
	for (j = 1; j < myDiscret; j++) {
	  t += step;
	  C.D0(t,P);
	  aDisplay.DrawTo(P);
	}
	break;
      case GeomAbs_Parabola :
      case GeomAbs_Hyperbola :
      case GeomAbs_BezierCurve :
      case GeomAbs_BSplineCurve :
      case GeomAbs_OtherCurve :
	for (j = 1; j <= myDiscret/2; j++) {
	  C.D0 (t+step*2., aNPnt);
          PlotCurve (aDisplay, C, t, step, aPPnt, aNPnt);
	  aPPnt = aNPnt;
	  t += step*2.;
	}
	break;
      }

      C.D0(TI(intrv+1),P);
      aDisplay.DrawTo(P);
    }
  }
}


//=======================================================================
//function : DrawIsoCurveOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Drawable::DrawIsoCurveOn(Adaptor3d_IsoCurve& C,
					 const GeomAbs_IsoType T,
					 const Standard_Real P,
					 const Standard_Real F,
					 const Standard_Real L,
					 Draw_Display& dis) const 
{
  C.Load(T,P,F,L);
  if ((C.GetType() == GeomAbs_BezierCurve) || 
      (C.GetType() == GeomAbs_BSplineCurve)) {
    GeomAdaptor_Curve GC;
    if (C.GetType() == GeomAbs_BezierCurve) 
      GC.Load(C.Bezier(),F,L);
    else
      GC.Load(C.BSpline(),F,L);
    
    DrawCurveOn(GC,dis);
  }
  else
    DrawCurveOn(C,dis);
  
}
