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


#include <Geom2d_Parabola.hxx>
#include <Geom_Parabola.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeParabola.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Parabola.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d'une Parabola de prostep a partir d'une Parabola de
// Geom2d
//=============================================================================
GeomToStep_MakeParabola::GeomToStep_MakeParabola(const Handle(Geom2d_Parabola)& C)
{
  gp_Parab2d gpPar;
  gpPar = C->Parab2d();

  Handle(StepGeom_Parabola) PStep = new StepGeom_Parabola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement2d)  Ax2Step;
  Standard_Real                   focal;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(gpPar.Axis());
  Ax2Step = MkAxis2.Value();
  focal = gpPar.Focal();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  PStep->Init(name, Ax2, focal);
  theParabola = PStep;
  done = Standard_True;
}

//=============================================================================
// Creation d'une Parabola de prostep a partir d'une Parabola de
// Geom
//=============================================================================

 GeomToStep_MakeParabola::GeomToStep_MakeParabola(const Handle(Geom_Parabola)& C)
{
  gp_Parab gpPar;
  gpPar = C->Parab();

  Handle(StepGeom_Parabola) PStep = new StepGeom_Parabola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement3d)  Ax2Step;
  Standard_Real                   focal;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(gpPar.Position());
  Ax2Step = MkAxis2.Value();
  focal = gpPar.Focal();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  PStep->Init(name, Ax2, focal / UnitsMethods::LengthFactor());
  theParabola = PStep;
  done = Standard_True;
}

//=============================================================================
// return the result
//=============================================================================

const Handle(StepGeom_Parabola)& GeomToStep_MakeParabola::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeParabola::Value() - no result");
  return theParabola;
}

