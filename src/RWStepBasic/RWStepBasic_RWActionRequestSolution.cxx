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
#include <RWStepBasic_RWActionRequestSolution.hxx>
#include <StepBasic_ActionMethod.hxx>
#include <StepBasic_ActionRequestSolution.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWActionRequestSolution
//purpose  : 
//=======================================================================
RWStepBasic_RWActionRequestSolution::RWStepBasic_RWActionRequestSolution ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_ActionRequestSolution) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"action_request_solution") ) return;

  // Own fields of ActionRequestSolution

  Handle(StepBasic_ActionMethod) aMethod;
  data->ReadEntity (num, 1, "method", ach, STANDARD_TYPE(StepBasic_ActionMethod), aMethod);

  Handle(StepBasic_VersionedActionRequest) aRequest;
  data->ReadEntity (num, 2, "request", ach, STANDARD_TYPE(StepBasic_VersionedActionRequest), aRequest);

  // Initialize entity
  ent->Init(aMethod,
            aRequest);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_ActionRequestSolution) &ent) const
{

  // Own fields of ActionRequestSolution

  SW.Send (ent->Method());

  SW.Send (ent->Request());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::Share (const Handle(StepBasic_ActionRequestSolution) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Own fields of ActionRequestSolution

  iter.AddItem (ent->Method());

  iter.AddItem (ent->Request());
}
