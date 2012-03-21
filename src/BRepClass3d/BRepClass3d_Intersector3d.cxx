// Created on: 1994-04-01
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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



//  Modified by skv - Fri Mar  4 12:07:34 2005 OCC7966

#include <BRepClass3d_Intersector3d.ixx>

#include <IntCurveSurface_IntersectionPoint.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs.hxx>


#include <IntCurveSurface_HInter.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Geom_Line.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepClass_FaceClassifier.hxx>

#include <GeomAdaptor_Curve.hxx>

#include <GeomAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>

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

  Standard_Boolean IsUPer, IsVPer;
  Standard_Real uperiod=0, vperiod=0;
  if ((IsUPer = surface.IsUPeriodic()))
    uperiod = surface.UPeriod();
  if ((IsVPer = surface.IsVPeriodic()))
    vperiod = surface.VPeriod();

  Standard_Real U1, U2, V1, V2;
  U1 = surface.FirstUParameter();
  U2 = surface.LastUParameter();
  V1 = surface.FirstVParameter();
  V2 = surface.LastVParameter();
  
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



