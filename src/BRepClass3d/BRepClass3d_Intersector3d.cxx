// Created on: 1994-04-01
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Face.hxx>

//============================================================================
BRepClass3d_Intersector3d::BRepClass3d_Intersector3d() 
     : done(Standard_False),hasapoint(Standard_False)
{ 
}
//============================================================================
void BRepClass3d_Intersector3d::Perform(const gp_Lin& L,
					const Standard_Real /*Prm*/,
					const Standard_Real Tol,
					const TopoDS_Face& Face) { 

  IntCurveSurface_HInter   HICS; 
  BRepAdaptor_Surface      surface;
  BRepClass_FaceClassifier classifier2d;

  Handle(Geom_Line) geomline = new Geom_Line(L);
  GeomAdaptor_Curve LL(geomline);

  surface.Initialize(Face,Standard_True);

  const Standard_Boolean IsUPer  = surface.IsUPeriodic();
  const Standard_Boolean IsVPer  = surface.IsVPeriodic();
  const Standard_Real    uperiod = IsUPer ? surface.UPeriod() : 0.0;
  const Standard_Real    vperiod = IsVPer ? surface.VPeriod() : 0.0;

  Standard_Real U1 = surface.FirstUParameter();
  Standard_Real U2 = surface.LastUParameter();
  Standard_Real V1 = surface.FirstVParameter();
  Standard_Real V2 = surface.LastVParameter();
  
  //--
  Handle(GeomAdaptor_HCurve) HLL  = new GeomAdaptor_HCurve(LL);
  Handle(BRepAdaptor_HSurface) Hsurface = new BRepAdaptor_HSurface(surface);
  //-- 
  HICS.Perform(HLL,Hsurface);
  
  W=RealLast();
  if(HICS.IsDone()) {
    for(Standard_Integer index=HICS.NbPoints(); index>=1; index--) {  
      gp_Pnt2d Puv(HICS.Point(index).U(),HICS.Point(index).V());

      Standard_Integer N1 = 0;
      Standard_Integer N2 = 0;

      Standard_Real X = Puv.X();
      Standard_Real Y = Puv.Y();

      if(IsUPer) {
        if(X > U2) {
          N1 = RealToInt( (X - U1) / uperiod );
        }
        if(X < U1) {
          N1 = RealToInt( (X - U2) / uperiod );
        }
        Puv.SetX(X - uperiod * N1);
      }

      if(IsVPer) {
        if(Y > V2) {
          N2 = RealToInt ( (Y - V1) / vperiod );
        }
        if(Y < V1) {
          N2 = RealToInt ( (Y - V2) / vperiod );
        }
        Puv.SetY(Y - vperiod * N2);
      }

      classifier2d.Perform(Face,Puv,Tol);
      TopAbs_State currentstate = classifier2d.State();
      if(currentstate==TopAbs_IN || currentstate==TopAbs_ON) { 
	const IntCurveSurface_IntersectionPoint& HICSPoint = HICS.Point(index);
	Standard_Real HICSW = HICSPoint.W();
//  Modified by skv - Fri Mar  4 12:07:34 2005 OCC7966 Begin
	if((W > HICSW) && (HICSW>-Tol)) { 
// 	if(W > HICSW) { 
//  Modified by skv - Fri Mar  4 12:07:34 2005 OCC7966 End
	  hasapoint  = Standard_True;
	  U          = HICSPoint.U();
	  V          = HICSPoint.V();
	  W          = HICSW; 
	  transition = HICSPoint.Transition();
	  pnt        = HICSPoint.Pnt();
	  state      = currentstate;
	  face       = Face;
	  if(Face.Orientation()==TopAbs_REVERSED) { 
	    if(transition == IntCurveSurface_In) 
	      transition = IntCurveSurface_Out;
	    else 
	      transition = IntCurveSurface_In;
	  }
	} 
      } //-- classifier state is IN or ON
      done = Standard_True;
    } //-- Loop on Intersection points.
  } //-- HICS.IsDone()
}
