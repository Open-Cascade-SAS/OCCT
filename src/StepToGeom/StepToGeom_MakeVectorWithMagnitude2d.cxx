// Created on: 1993-08-04
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

// sln 23.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude2d(...) function)

#include <StepToGeom_MakeVectorWithMagnitude2d.ixx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Direction.hxx>
#include <StepToGeom_MakeDirection2d.hxx>
#include <Geom2d_Direction.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom2d a partir d' un Vector de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeVectorWithMagnitude2d::Convert (const Handle(StepGeom_Vector)& SV, Handle(Geom2d_VectorWithMagnitude)& CV)
{
  // sln 23.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude2d(...) function)
  Handle(Geom2d_Direction) D;
  if (StepToGeom_MakeDirection2d::Convert(SV->Orientation(),D))
  {
    const gp_Vec2d V(D->Dir2d().XY() * SV->Magnitude());
    CV = new Geom2d_VectorWithMagnitude(V);
    return Standard_True;
  }
  return Standard_False;
}
