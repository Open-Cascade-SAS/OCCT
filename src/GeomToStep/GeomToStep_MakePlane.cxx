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


#include <GeomToStep_MakePlane.ixx>
#include <GeomToStep_MakePlane.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Dir.hxx>
#include <Geom_Plane.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un plane de prostep a partir d' un Pln de gp
//=============================================================================

GeomToStep_MakePlane::GeomToStep_MakePlane( const gp_Pln& P)
{
  Handle(StepGeom_Plane) Plan = new StepGeom_Plane;
  Handle(StepGeom_Axis2Placement3d) aPosition;

  GeomToStep_MakeAxis2Placement3d MkAxis2(P.Position());
  aPosition = MkAxis2.Value();
  Plan->SetPosition(aPosition);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Plan->SetName(name);
  thePlane = Plan;
  done = Standard_True;
}

//=============================================================================
// Creation d' un plane de prostep a partir d' un Plane de Geom
//=============================================================================

GeomToStep_MakePlane::GeomToStep_MakePlane( const Handle(Geom_Plane)& Gpln)
{
  gp_Pln P;
  Handle(StepGeom_Plane) Plan = new StepGeom_Plane;
  Handle(StepGeom_Axis2Placement3d) aPosition;

  P = Gpln->Pln();
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(P.Position());
  aPosition = MkAxis2.Value();
  Plan->SetPosition(aPosition);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Plan->SetName(name);
  thePlane = Plan;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Plane) &
      GeomToStep_MakePlane::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return thePlane;
}

