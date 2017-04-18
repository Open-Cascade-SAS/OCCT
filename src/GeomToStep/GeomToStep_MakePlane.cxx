// Created on: 1993-06-17
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom_Plane.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakePlane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Plane.hxx>
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakePlane::Value() - no result");
  return thePlane;
}

