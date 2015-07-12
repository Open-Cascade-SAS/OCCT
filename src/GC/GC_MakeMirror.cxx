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


#include <GC_MakeMirror.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une symetrie de Geom par rapport a un point.              +
//=========================================================================
GC_MakeMirror::GC_MakeMirror(const gp_Pnt&  Point ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Point);
}

//=========================================================================
//   Creation d une symetrie de Geom par rapport a une droite.            +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Ax1&  Axis ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Axis);
}

//=========================================================================
//   Creation d une symetrie de Geom par rapport a une droite.            +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Lin&  Line ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(gp_Ax1(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom par rapport a une droite definie  +
//   par un point et une direction.                                       +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Pnt&  Point ,
					const gp_Dir&  Direc ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(gp_Ax1(Point,Direc));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom par rapport a un plan defini par  +
//   un Ax2 (Normale au plan et axe x du plan).                           +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Ax2&  Plane ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Plane);
}

//=========================================================================
//   Creation d une symetrie 3d de gp par rapport a un plan Plane.        +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Pln&  Plane ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Plane.Position().Ax2());
}

const Handle(Geom_Transformation)& GC_MakeMirror::Value() const
{ 
  return TheMirror;
}
