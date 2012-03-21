// Created on: 1992-10-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <GC_MakeMirror.ixx>
#include <GC_MakeMirror.hxx>
#include <gp_Ax3.hxx>
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

const Handle(Geom_Transformation)& GC_MakeMirror::Operator() const 
{
  return TheMirror;
}

GC_MakeMirror::operator Handle(Geom_Transformation) () const
{
  return TheMirror;
}

