// Created on: 1999-02-16
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

// sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used (StepToGeom_MakeTransformation2d(...) function)

#include <StepToGeom_MakeTransformation2d.ixx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <StepToGeom_MakeDirection2d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Direction.hxx>
#include <gp_Ax2d.hxx>

//=======================================================================
//function : StepToGeom_MakeTransformation2d
//purpose  : 
//=======================================================================

Standard_Boolean StepToGeom_MakeTransformation2d::Convert (const Handle(StepGeom_CartesianTransformationOperator2d)& SCTO, gp_Trsf2d& CT)
{
  //  NB : on ne s interesse ici qu au deplacement rigide
  Handle(Geom2d_CartesianPoint) CP;
  if (StepToGeom_MakeCartesianPoint2d::Convert(SCTO->LocalOrigin(),CP))
  {
    gp_Dir2d D1(1.,0.);
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    const Handle(StepGeom_Direction) A = SCTO->Axis1();
    if (!A.IsNull())
    {
      Handle(Geom2d_Direction) D;
      if (StepToGeom_MakeDirection2d::Convert(A,D))
        D1 = D->Dir2d();
    }
    const gp_Ax2d result(CP->Pnt2d(),D1);
    CT.SetTransformation(result);
    CT = CT.Inverted();
    return Standard_True;
  }
  return Standard_False;
}
