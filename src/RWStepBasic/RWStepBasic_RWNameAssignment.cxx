// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWNameAssignment.hxx>
#include <StepBasic_NameAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWNameAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWNameAssignment::RWStepBasic_RWNameAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepBasic_NameAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"name_assignment") ) return;

  // Own fields of NameAssignment

  Handle(TCollection_HAsciiString) aAssignedName;
  data->ReadString (num, 1, "assigned_name", ach, aAssignedName);

  // Initialize entity
  ent->Init(aAssignedName);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepBasic_NameAssignment) &ent) const
{

  // Own fields of NameAssignment

  SW.Send (ent->AssignedName());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::Share (const Handle(StepBasic_NameAssignment) &,
                                          Interface_EntityIterator&) const
{
  // Own fields of NameAssignment
}
