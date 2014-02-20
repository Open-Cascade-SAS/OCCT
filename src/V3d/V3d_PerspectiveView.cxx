// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
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

// Modified	07-10-96 : CQ ; correction PRO4522
// Modified	23-02-98 : FMN ; Replace PI by Standard_PI
// Modified     25-02-98 : FMN ; PERF.27: Optimisation of view creation from an existing view
// Modified     02-09-98 : FMN ; Correction problem of angle calculation in V3d_PerspectiveView::Angle(). 
// Modified     23-11-98 : FMN ; PRO14896: Correction of the management of the perspective (cf Programming Guide)
//				 If the angle is modified the WindowLimit changes not the focal.
//		22-12-98 : FMN ; Rename CSF_WALKTHROW into CSF_WALKTHROUGH

// IMP240100       //GG -> Activates WalkThrough model.


//-Version

//-Design       

//-Warning      

//-References

//-Language     C++ 2.1


// for Test method

// for the class
#include <V3d_View.hxx>
#include <V3d_PerspectiveView.ixx>
#include <Visual3d_View.hxx>
#include <V3d_BadValue.hxx>

V3d_PerspectiveView::V3d_PerspectiveView (const Handle(V3d_Viewer)& VM):V3d_View (VM,V3d_PERSPECTIVE) {
  MyViewMapping.SetProjection(Visual3d_TOP_PERSPECTIVE) ;
  SetAngle(VM->DefaultAngle()) ;	
//  SetZoom(10.0, Standard_True);
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

V3d_PerspectiveView::V3d_PerspectiveView (const Handle(V3d_Viewer)& VM, const Handle(V3d_OrthographicView)& V):V3d_View (VM,V,V3d_PERSPECTIVE) {
  MyViewMapping.SetProjection(Visual3d_TOP_PERSPECTIVE) ;
  SetAngle(VM->DefaultAngle()) ;	
//  SetZoom(10.0, Standard_True);
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

V3d_PerspectiveView::V3d_PerspectiveView (const Handle(V3d_Viewer)& VM, const Handle(V3d_PerspectiveView)& V):V3d_View (VM,V,V3d_PERSPECTIVE) {
  MyViewMapping.SetProjection(Visual3d_TOP_PERSPECTIVE) ;
//  SetZoom(10.0, Standard_True);
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

Handle(V3d_PerspectiveView) V3d_PerspectiveView::Copy() const {
  return new V3d_PerspectiveView(this->Viewer(),this);}

//-Methods, in order

void V3d_PerspectiveView::SetAngle(const Standard_Real Angle) {
  
  Standard_Real focale,Umin,Vmin,Umax,Vmax,Dxv,Dyv,Rap,Xrp,Yrp;     

  V3d_BadValue_Raise_if ( Angle <= 0. || Angle >= M_PI, "V3d_PerspectiveView::SetAngle, bad angle");

  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Dxv = Abs(Umax - Umin)/2. ; Dyv = Abs(Vmax - Vmin)/2.;
  focale = Focale();
  Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
  Rap = Dxv / Dyv;
  if( Dxv >= Dyv ) {
      Dyv = Abs(focale * tan(Angle/2.)) ;
      Dxv = Rap * Dyv;
  } else {
      Dxv = Abs(focale * tan(Angle/2.)) ;
      Dyv = Dxv / Rap;
  }
  Umin = Xrp - Dxv ; Umax = Xrp + Dxv ;
  Vmin = Yrp - Dyv ; Vmax = Yrp + Dyv ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();
}

Standard_Real V3d_PerspectiveView::Angle()const  {
  
//  Graphic3d_Vertex Prp ;
  Standard_Real focale,Umin,Vmin,Umax,Vmax,Dxv,Dyv ;     
  Standard_Real angle = M_PI ;
  
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  focale = Focale() ;
  Dxv = (Umax - Umin)/2. ; Dyv = (Vmax - Vmin)/2. ;
  if( focale > 0. ) {
    if( Dxv >= Dyv ) {
      angle = 2.*atan(Dyv/focale) ;
    } else {
      angle = 2.*atan(Dxv/focale) ;
    }
  }
  
  return angle ;
}

void V3d_PerspectiveView::SetPerspective(const Standard_Real Angle, const Standard_Real UVRatio,
                                         const Standard_Real ZNear, const Standard_Real ZFar)
{
  Standard_Real Umin,Vmin,Umax,Vmax,Yrp,Zrp,du,dv;

  V3d_BadValue_Raise_if ( ZNear <= 0. || ZFar <= 0. || ZNear >= ZFar, "V3d_PerspectiveView::SetPerspective, bad distances");
  V3d_BadValue_Raise_if ( Angle <= 0. || Angle >= M_PI, "V3d_PerspectiveView::SetAngle, bad angle");

  Graphic3d_Vertex PRP = MyViewMapping.ProjectionReferencePoint() ;
  Yrp = Zrp = 0.;

  PRP.SetCoord(Zrp, Yrp, Zrp);
  MyViewMapping.SetProjectionReferencePoint(PRP);

  Standard_Real size = (ZFar - ZNear) / 2.;

  MyViewMapping.SetFrontPlaneDistance(size);
  MyViewMapping.SetBackPlaneDistance(-size);
  MyViewMapping.SetViewPlaneDistance(size);

  // recompute window limits by mapping to view plane
  dv = Abs(ZNear * tan(Angle/2.));
  du = dv * UVRatio;
  Umin = -du; Umax = du;
  Vmin = -dv; Vmax = dv;
  MyViewMapping.SetWindowLimit(Umin, Vmin, Umax, Vmax);

  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();
}
