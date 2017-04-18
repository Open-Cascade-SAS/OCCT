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


#include <Geom2d_Hyperbola.hxx>
#include <Geom_Hyperbola.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeHyperbola.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d'une hyperbola de prostep a partir d'une hyperbola de
// Geom2d
//=============================================================================
GeomToStep_MakeHyperbola::GeomToStep_MakeHyperbola(const Handle(Geom2d_Hyperbola)& C)
{
  gp_Hypr2d gpHyp;
  gpHyp = C->Hypr2d();

  Handle(StepGeom_Hyperbola) HStep = new StepGeom_Hyperbola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement2d)  Ax2Step;
  Standard_Real                   majorR, minorR;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(gpHyp.Axis());
  Ax2Step = MkAxis2.Value();
  majorR = gpHyp.MajorRadius();
  minorR = gpHyp.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  HStep->Init(name, Ax2,majorR,minorR);
  theHyperbola = HStep;
  done = Standard_True;
}

//=============================================================================
// Creation d'une hyperbola de prostep a partir d'une hyperbola de
// Geom
//=============================================================================

 GeomToStep_MakeHyperbola::GeomToStep_MakeHyperbola(const Handle(Geom_Hyperbola)& C)
{
  gp_Hypr gpHyp;
  gpHyp = C->Hypr();

  Handle(StepGeom_Hyperbola) HStep = new StepGeom_Hyperbola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement3d)  Ax2Step;
  Standard_Real                   majorR, minorR;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(gpHyp.Position());
  Ax2Step = MkAxis2.Value();
  majorR = gpHyp.MajorRadius();
  minorR = gpHyp.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Standard_Real fact = UnitsMethods::LengthFactor();
  HStep->Init(name, Ax2,majorR/fact,minorR/fact);
  theHyperbola = HStep;
  done = Standard_True;
}

//=============================================================================
// return the result
//=============================================================================

const Handle(StepGeom_Hyperbola)& GeomToStep_MakeHyperbola::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeHyperbola::Value() - no result");
  return theHyperbola;
}

