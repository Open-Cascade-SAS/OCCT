// Created on: 1993-07-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

// sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude(...) function)

#include <StepToGeom_MakeVectorWithMagnitude.ixx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Direction.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <Geom_Direction.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom a partir d' un Vector de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeVectorWithMagnitude::Convert (const Handle(StepGeom_Vector)& SV, Handle(Geom_VectorWithMagnitude)& CV)
{
  // sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created 
  Handle(Geom_Direction) D;
  if (StepToGeom_MakeDirection::Convert(SV->Orientation(),D))
  {
    const gp_Vec V(D->Dir().XYZ() * SV->Magnitude() * UnitsMethods::LengthFactor());
    CV = new Geom_VectorWithMagnitude(V);
    return Standard_True;
  }
  return Standard_False;
}
