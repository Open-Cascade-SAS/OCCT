// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepFEA_CurveElementLocation.hxx>
#include <StepFEA_FeaParametricPoint.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_CurveElementLocation,Standard_Transient)

//=======================================================================
//function : StepFEA_CurveElementLocation
//purpose  : 
//=======================================================================
StepFEA_CurveElementLocation::StepFEA_CurveElementLocation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementLocation::Init (const Handle(StepFEA_FeaParametricPoint) &aCoordinate)
{

  theCoordinate = aCoordinate;
}

//=======================================================================
//function : Coordinate
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaParametricPoint) StepFEA_CurveElementLocation::Coordinate () const
{
  return theCoordinate;
}

//=======================================================================
//function : SetCoordinate
//purpose  : 
//=======================================================================

void StepFEA_CurveElementLocation::SetCoordinate (const Handle(StepFEA_FeaParametricPoint) &aCoordinate)
{
  theCoordinate = aCoordinate;
}
