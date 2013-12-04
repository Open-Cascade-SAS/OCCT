// Created on: 1993-08-04
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

// sln 23.10.2001. CTS23496: Line is not created if direction have not been succesfully created (StepToGeom_MakeLine2d(...) function)

#include <StepToGeom_MakeLine2d.ixx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeVectorWithMagnitude2d.hxx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Vector.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>

//=============================================================================
// Creation d' une Line de Geom2d a partir d' une Line de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeLine2d::Convert (const Handle(StepGeom_Line)& SC, Handle(Geom2d_Line)& CC)
{
  Handle(Geom2d_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint2d::Convert(SC->Pnt(),P))
  {
    // sln 23.10.2001. CTS23496: Line is not created if direction have not been succesfully created
    Handle(Geom2d_VectorWithMagnitude) D;
    if (StepToGeom_MakeVectorWithMagnitude2d::Convert(SC->Dir(),D))
    {
      const gp_Dir2d D1(D->Vec2d());
      CC = new Geom2d_Line(P->Pnt2d(), D1);
      return Standard_True;
    }
  }
  return Standard_False;
}
