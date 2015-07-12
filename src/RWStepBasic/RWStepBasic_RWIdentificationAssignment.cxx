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
#include <RWStepBasic_RWIdentificationAssignment.hxx>
#include <StepBasic_IdentificationAssignment.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWIdentificationAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWIdentificationAssignment::RWStepBasic_RWIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepBasic_IdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"identification_assignment") ) return;

  // Own fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aAssignedId;
  data->ReadString (num, 1, "assigned_id", ach, aAssignedId);

  Handle(StepBasic_IdentificationRole) aRole;
  data->ReadEntity (num, 2, "role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aRole);

  // Initialize entity
  ent->Init(aAssignedId,
            aRole);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepBasic_IdentificationAssignment) &ent) const
{

  // Own fields of IdentificationAssignment

  SW.Send (ent->AssignedId());

  SW.Send (ent->Role());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::Share (const Handle(StepBasic_IdentificationAssignment) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Own fields of IdentificationAssignment

  iter.AddItem (ent->Role());
}
