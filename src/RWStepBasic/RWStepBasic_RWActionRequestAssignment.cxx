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
#include <RWStepBasic_RWActionRequestAssignment.hxx>
#include <StepBasic_ActionRequestAssignment.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWActionRequestAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWActionRequestAssignment::RWStepBasic_RWActionRequestAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepBasic_ActionRequestAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"action_request_assignment") ) return;

  // Own fields of ActionRequestAssignment

  Handle(StepBasic_VersionedActionRequest) aAssignedActionRequest;
  data->ReadEntity (num, 1, "assigned_action_request", ach, STANDARD_TYPE(StepBasic_VersionedActionRequest), aAssignedActionRequest);

  // Initialize entity
  ent->Init(aAssignedActionRequest);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepBasic_ActionRequestAssignment) &ent) const
{

  // Own fields of ActionRequestAssignment

  SW.Send (ent->AssignedActionRequest());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::Share (const Handle(StepBasic_ActionRequestAssignment) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Own fields of ActionRequestAssignment

  iter.AddItem (ent->AssignedActionRequest());
}
