// Created on: 2015-07-13
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <RWStepDimTol_RWToleranceZoneForm.hxx>

#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>

//=======================================================================
//function : RWStepDimTol_RWToleranceZoneForm
//purpose  : 
//=======================================================================

RWStepDimTol_RWToleranceZoneForm::RWStepDimTol_RWToleranceZoneForm ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWToleranceZoneForm::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepDimTol_ToleranceZoneForm) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"tolerance_zone_form") ) return;

  // Own fields of ToleranceZoneForm

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  // Initialize entity
  ent->Init(aName);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWToleranceZoneForm::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepDimTol_ToleranceZoneForm) &ent) const
{
  SW.Send (ent->Name());
}
