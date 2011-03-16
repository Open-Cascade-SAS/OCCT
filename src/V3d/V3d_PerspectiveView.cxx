// File         V3d_PerspectiveView.cxx
// Created      September 1992
// Author       GG
// Modified	07-10-96 : CQ ; correction PRO4522
// Modified	23-02-98 : FMN ; Replace PI by Standard_PI
// Modified     25-02-98 : FMN ; PERF.27: Optimisation of view creation from an existing view
// Modified     02-09-98 : FMN ; Correction problem of angle calculation in V3d_PerspectiveView::Angle(). 
// Modified     23-11-98 : FMN ; PRO14896: Correction of the management of the perspective (cf Programming Guide)
//				 If the angle is modified the WindowLimit changes not the focal.
//		22-12-98 : FMN ; Rename CSF_WALKTHROW into CSF_WALKTHROUGH

// IMP240100       //GG -> Activates WalkThrough model.

//-Copyright    MatraDatavision 1991,1992

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
#include <Viewer_BadValue.hxx>

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

  Viewer_BadValue_Raise_if ( Angle <= 0. || Angle >= Standard_PI, "V3d_PerspectiveView::SetAngle, bad angle");

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
  Standard_Real angle = Standard_PI ;
  
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
  Standard_Real Umin,Vmin,Umax,Vmax,Xrp,Yrp,Zrp,du,dv;

  Viewer_BadValue_Raise_if ( ZNear <= 0. || ZFar <= 0. || ZNear >= ZFar, "V3d_PerspectiveView::SetPerspective, bad distances");
  Viewer_BadValue_Raise_if ( Angle <= 0. || Angle >= Standard_PI, "V3d_PerspectiveView::SetAngle, bad angle");

  Graphic3d_Vertex PRP = MyViewMapping.ProjectionReferencePoint() ;
  Xrp = Yrp = Zrp = 0.;

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
