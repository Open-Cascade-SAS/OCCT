// Created by: GG
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

// Modified     25-02-98 : FMN ; PERF.27: Optimization of view creation from an existing view


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
