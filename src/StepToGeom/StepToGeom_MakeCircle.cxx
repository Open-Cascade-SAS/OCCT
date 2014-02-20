// Created on: 1993-07-02
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

#include <StepToGeom_MakeCircle.ixx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Circle de Geom a partir d' un Circle de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCircle::Convert
    (const Handle(StepGeom_Circle)& SC,
     Handle(Geom_Circle)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A;
    if (StepToGeom_MakeAxis2Placement::Convert 
          (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()),A))
    {
      CC = new Geom_Circle(A->Ax2(),SC->Radius() * UnitsMethods::LengthFactor());
      return Standard_True;
	}
  }
  return Standard_False;
}
