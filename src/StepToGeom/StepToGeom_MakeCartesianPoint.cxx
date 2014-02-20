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

#include <StepToGeom_MakeCartesianPoint.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un CartesianPoint de Geom a partir d' un CartesianPoint de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCartesianPoint::Convert
  (const Handle(StepGeom_CartesianPoint)& SP,
   Handle(Geom_CartesianPoint)& CP)
{
  if (SP->NbCoordinates() == 3)
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    const Standard_Real X = SP->CoordinatesValue(1) * LF;
    const Standard_Real Y = SP->CoordinatesValue(2) * LF;
    const Standard_Real Z = SP->CoordinatesValue(3) * LF;
    CP = new Geom_CartesianPoint(X, Y, Z);
    return Standard_True;
  }
  return Standard_False;
}
