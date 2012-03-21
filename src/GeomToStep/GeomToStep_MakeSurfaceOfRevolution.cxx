// Created on: 1993-06-17
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


#include <GeomToStep_MakeSurfaceOfRevolution.ixx>
#include <GeomToStep_MakeAxis1Placement.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <gp_Ax1.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une surface_of_revolution de prostep a partir d' une
// SurfaceOfRevolution de Geom
//=============================================================================

GeomToStep_MakeSurfaceOfRevolution::GeomToStep_MakeSurfaceOfRevolution
  ( const Handle(Geom_SurfaceOfRevolution)& S )
	
{
  Handle(StepGeom_SurfaceOfRevolution) Surf;
  Handle(StepGeom_Curve) aSweptCurve;
  Handle(StepGeom_Axis1Placement) aAxisPosition;
  
  GeomToStep_MakeCurve MkSwept(S->BasisCurve());
  GeomToStep_MakeAxis1Placement MkAxis1(S->Axis());
  aSweptCurve = MkSwept.Value();
  aAxisPosition = MkAxis1.Value();
  Surf = new StepGeom_SurfaceOfRevolution;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Surf->Init(name, aSweptCurve, aAxisPosition);
  theSurfaceOfRevolution = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SurfaceOfRevolution) &
      GeomToStep_MakeSurfaceOfRevolution::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSurfaceOfRevolution;
}
