// File         V3d_OrthographicView.cxx
// Created      September 1992
// Author       GG
// Modified     25-02-98 : FMN ; PERF.27: Optimization of view creation from an existing view

//-Copyright    MatraDatavision 1991,1992

//-Version

//-Design       

//-Warning      

//-References

//-Language     C++ 2.1

// for Test method

// for the class
#include <V3d_View.hxx>
#include <V3d_OrthographicView.ixx>
#include <V3d_PerspectiveView.hxx>

//-Declarations

//-Aliases

//-Global data definitions

//-Constructors

V3d_OrthographicView::V3d_OrthographicView (const Handle(V3d_Viewer)& VM):V3d_View (VM) {

  MyType = V3d_ORTHOGRAPHIC ;
  MyViewMapping.SetProjection(Visual3d_TOP_PARALLEL) ;
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

V3d_OrthographicView::V3d_OrthographicView (const Handle(V3d_Viewer)& VM, const Handle(V3d_PerspectiveView)& V):V3d_View (VM,V) {

  MyType = V3d_ORTHOGRAPHIC ;
  MyViewMapping.SetProjection(Visual3d_TOP_PARALLEL) ;
  SetZoom(0.1, Standard_True);
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

V3d_OrthographicView::V3d_OrthographicView (const Handle(V3d_Viewer)& VM, const Handle(V3d_OrthographicView)& V):V3d_View (VM,V) {

  MyType = V3d_ORTHOGRAPHIC ;
  MyViewMapping.SetProjection(Visual3d_TOP_PARALLEL) ;
  SetViewMappingDefault() ;
  SetViewOrientationDefault() ;
}

Handle(V3d_OrthographicView) V3d_OrthographicView::Copy () const {
  return new V3d_OrthographicView(this->Viewer(),this);}
