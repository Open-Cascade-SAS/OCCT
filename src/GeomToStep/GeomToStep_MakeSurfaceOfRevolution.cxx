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


#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomToStep_MakeAxis1Placement.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeSurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Curve.hxx>
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeSurfaceOfRevolution::Value() - no result");
  return theSurfaceOfRevolution;
}
