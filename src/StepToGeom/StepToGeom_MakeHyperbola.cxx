// Created on: 1994-09-08
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StepToGeom_MakeHyperbola.ixx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Hyperbola de Geom a partir d' un Hyperbola de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeHyperbola::Convert
    (const Handle(StepGeom_Hyperbola)& SC,
     Handle(Geom_Hyperbola)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A1;
    if (StepToGeom_MakeAxis2Placement::Convert
          (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()),A1))
    {
      const gp_Ax2 A( A1->Ax2() );
      const Standard_Real LF = UnitsMethods::LengthFactor();
      CC = new Geom_Hyperbola(A, SC->SemiAxis() * LF, SC->SemiImagAxis() * LF);
      return Standard_True;
    }
  }
  return Standard_False;
}
