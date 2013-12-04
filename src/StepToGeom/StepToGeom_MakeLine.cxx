// Created on: 1993-07-02
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

// sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created (StepToGeom_MakeLine(...) function)

#include <StepToGeom_MakeLine.ixx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>

//=============================================================================
// Creation d' une Line de Geom a partir d' une Line de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeLine::Convert (const Handle(StepGeom_Line)& SC, Handle(Geom_Line)& CC)
{
  Handle(Geom_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint::Convert(SC->Pnt(),P))
  {
    // sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created 
    Handle(Geom_VectorWithMagnitude) D;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SC->Dir(),D))
    {
      if( D->Vec().SquareMagnitude() < Precision::Confusion() * Precision::Confusion())
        return Standard_False;
      const gp_Dir V(D->Vec());
      CC = new Geom_Line(P->Pnt(), V);
      return Standard_True;
    }
  }
  return Standard_False;
}
