// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWActionAssignment.hxx>
#include <StepBasic_Action.hxx>
#include <StepBasic_ActionAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWActionAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWActionAssignment::RWStepBasic_RWActionAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepBasic_ActionAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"action_assignment") ) return;

  // Own fields of ActionAssignment

  Handle(StepBasic_Action) aAssignedAction;
  data->ReadEntity (num, 1, "assigned_action", ach, STANDARD_TYPE(StepBasic_Action), aAssignedAction);

  // Initialize entity
  ent->Init(aAssignedAction);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepBasic_ActionAssignment) &ent) const
{

  // Own fields of ActionAssignment

  SW.Send (ent->AssignedAction());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::Share (const Handle(StepBasic_ActionAssignment) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of ActionAssignment

  iter.AddItem (ent->AssignedAction());
}
