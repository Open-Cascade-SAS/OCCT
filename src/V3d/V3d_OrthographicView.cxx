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
