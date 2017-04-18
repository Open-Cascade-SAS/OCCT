// Created on: 1994-09-02
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom2d_Ellipse.hxx>
#include <Geom_Ellipse.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeEllipse.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Ellipse.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d'une ellipse de prostep a partir d'une ellipse 3d de gp
//=============================================================================
GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const gp_Elips& E)
{
#include "GeomToStep_MakeEllipse_gen.pxx"
}


//=============================================================================
// Creation d'une ellipse de prostep a partir d'une ellipse de
// Geom
//=============================================================================

GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const Handle(Geom_Ellipse)& Cer)
{
  gp_Elips E;
  E = Cer->Elips();
#include "GeomToStep_MakeEllipse_gen.pxx"
}


//=============================================================================
// Creation d'une ellipse 2d de prostep a partir d'une ellipse de
// Geom2d
//=============================================================================

GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const Handle(Geom2d_Ellipse)& Cer)
{
  gp_Elips2d E2d;
  E2d = Cer->Elips2d();

  Handle(StepGeom_Ellipse) EStep = new StepGeom_Ellipse;
  StepGeom_Axis2Placement Ax2;
  Handle(StepGeom_Axis2Placement2d) Ax2Step;
  Standard_Real majorR, minorR;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(E2d.Axis());
  Ax2Step = MkAxis2.Value();
  majorR = E2d.MajorRadius();
  minorR = E2d.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  EStep->Init(name, Ax2, majorR, minorR);
  theEllipse = EStep;
  done = Standard_True;

}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Ellipse) &
      GeomToStep_MakeEllipse::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeEllipse::Value() - no result");
  return theEllipse;
}
