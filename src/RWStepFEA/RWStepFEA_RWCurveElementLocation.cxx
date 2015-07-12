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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWCurveElementLocation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_CurveElementLocation.hxx>
#include <StepFEA_FeaParametricPoint.hxx>

//=======================================================================
//function : RWStepFEA_RWCurveElementLocation
//purpose  : 
//=======================================================================
RWStepFEA_RWCurveElementLocation::RWStepFEA_RWCurveElementLocation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepFEA_CurveElementLocation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"curve_element_location") ) return;

  // Own fields of CurveElementLocation

  Handle(StepFEA_FeaParametricPoint) aCoordinate;
  data->ReadEntity (num, 1, "coordinate", ach, STANDARD_TYPE(StepFEA_FeaParametricPoint), aCoordinate);

  // Initialize entity
  ent->Init(aCoordinate);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepFEA_CurveElementLocation) &ent) const
{

  // Own fields of CurveElementLocation

  SW.Send (ent->Coordinate());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::Share (const Handle(StepFEA_CurveElementLocation) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementLocation

  iter.AddItem (ent->Coordinate());
}
