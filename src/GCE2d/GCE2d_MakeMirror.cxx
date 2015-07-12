// Created on: 1992-10-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <GCE2d_MakeMirror.hxx>
#include <Geom2d_Transformation.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a un point.              +
//=========================================================================
GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Pnt2d&  Point ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(Point);
}

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a une droite.            +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Ax2d&  Axis ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(Axis);
}

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a une droite.            +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Lin2d&  Line ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(gp_Ax2d(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom2d par rapport a une droite definie  +
//   par un point et une direction.                                       +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Pnt2d&  Point ,
				   const gp_Dir2d&  Direc ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(gp_Ax2d(Point,Direc));
}

const Handle(Geom2d_Transformation)& GCE2d_MakeMirror::Value() const
{ 
  return TheMirror;
}
