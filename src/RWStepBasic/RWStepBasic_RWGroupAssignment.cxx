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
#include <RWStepBasic_RWGroupAssignment.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWGroupAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWGroupAssignment::RWStepBasic_RWGroupAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepBasic_GroupAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"group_assignment") ) return;

  // Own fields of GroupAssignment

  Handle(StepBasic_Group) aAssignedGroup;
  data->ReadEntity (num, 1, "assigned_group", ach, STANDARD_TYPE(StepBasic_Group), aAssignedGroup);

  // Initialize entity
  ent->Init(aAssignedGroup);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepBasic_GroupAssignment) &ent) const
{

  // Own fields of GroupAssignment

  SW.Send (ent->AssignedGroup());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::Share (const Handle(StepBasic_GroupAssignment) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of GroupAssignment

  iter.AddItem (ent->AssignedGroup());
}
