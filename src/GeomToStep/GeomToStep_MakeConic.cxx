// Created on: 1993-06-21
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <GeomToStep_MakeConic.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Conic.hxx>
#include <GeomToStep_MakeConic.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <GeomToStep_MakeCircle.hxx>
#include <GeomToStep_MakeEllipse.hxx>
#include <GeomToStep_MakeHyperbola.hxx>
#include <GeomToStep_MakeParabola.hxx>

//=============================================================================
// Creation d' une Conic de prostep a partir d' une Conic de Geom
//=============================================================================

GeomToStep_MakeConic::GeomToStep_MakeConic ( const Handle(Geom_Conic)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom_Circle))) {
    Handle(Geom_Circle) Cer = Handle(Geom_Circle)::DownCast(C);
    GeomToStep_MakeCircle MkCircle(Cer);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
    Handle(Geom_Ellipse) Ell = Handle(Geom_Ellipse)::DownCast(C);
    GeomToStep_MakeEllipse MkEllipse(Ell);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Hyperbola))) {
    Handle(Geom_Hyperbola) Hyp = Handle(Geom_Hyperbola)::DownCast(C);
    GeomToStep_MakeHyperbola MkHyperbola(Hyp);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Parabola))) {
    Handle(Geom_Parabola) Par = Handle(Geom_Parabola)::DownCast(C);
    GeomToStep_MakeParabola MkParabola(Par);
    theConic = MkParabola.Value();
  }
  else {
#ifdef DEBUG
    cout << "3D Curve Type   : " << C->DynamicType() << endl;
#endif
    done = Standard_False;
  }
}	 

//=============================================================================
// Creation d' une Conic2d de prostep a partir d' une Conic de Geom2d
//=============================================================================

GeomToStep_MakeConic::GeomToStep_MakeConic ( const Handle(Geom2d_Conic)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
    Handle(Geom2d_Circle) Cer = Handle(Geom2d_Circle)::DownCast(C);
    GeomToStep_MakeCircle MkCircle(Cer);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse))) {
    Handle(Geom2d_Ellipse) Ell = Handle(Geom2d_Ellipse)::DownCast(C);
    GeomToStep_MakeEllipse MkEllipse(Ell);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Hyperbola))) {
    Handle(Geom2d_Hyperbola) Hyp = Handle(Geom2d_Hyperbola)::DownCast(C);
    GeomToStep_MakeHyperbola MkHyperbola(Hyp);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Parabola))) {
    Handle(Geom2d_Parabola) Par = Handle(Geom2d_Parabola)::DownCast(C);
    GeomToStep_MakeParabola MkParabola(Par);
    theConic = MkParabola.Value();
  }
  else {
    // Attention : Other 2d conics shall be implemented ...
    //             To be performed later !
#ifdef DEBUG
    cout << "2D conic not yet implemented" << endl;
#endif
    done = Standard_False;
  }
}	 

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Conic) &
      GeomToStep_MakeConic::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theConic;
}
