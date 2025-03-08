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
#include "RWStepAP203_RWChangeRequest.pxx"
#include <StepAP203_ChangeRequest.hxx>
#include <StepAP203_ChangeRequestItem.hxx>
#include <StepAP203_HArray1OfChangeRequestItem.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepAP203_RWChangeRequest::RWStepAP203_RWChangeRequest() {}

//=================================================================================================

void RWStepAP203_RWChangeRequest::ReadStep(const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer                 num,
                                           Handle(Interface_Check)&               ach,
                                           const Handle(StepAP203_ChangeRequest)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "change_request"))
    return;

  // Inherited fields of ActionRequestAssignment

  Handle(StepBasic_VersionedActionRequest) aActionRequestAssignment_AssignedActionRequest;
  data->ReadEntity(num,
                   1,
                   "action_request_assignment.assigned_action_request",
                   ach,
                   STANDARD_TYPE(StepBasic_VersionedActionRequest),
                   aActionRequestAssignment_AssignedActionRequest);

  // Own fields of ChangeRequest

  Handle(StepAP203_HArray1OfChangeRequestItem) aItems;
  Standard_Integer                             sub2 = 0;
  if (data->ReadSubList(num, 2, "items", ach, sub2))
  {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0  = data->NbParams(num2);
    aItems                = new StepAP203_HArray1OfChangeRequestItem(1, nb0);
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      StepAP203_ChangeRequestItem anIt0;
      data->ReadEntity(num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aActionRequestAssignment_AssignedActionRequest, aItems);
}

//=================================================================================================

void RWStepAP203_RWChangeRequest::WriteStep(StepData_StepWriter&                   SW,
                                            const Handle(StepAP203_ChangeRequest)& ent) const
{

  // Inherited fields of ActionRequestAssignment

  SW.Send(ent->StepBasic_ActionRequestAssignment::AssignedActionRequest());

  // Own fields of ChangeRequest

  SW.OpenSub();
  for (Standard_Integer i1 = 1; i1 <= ent->Items()->Length(); i1++)
  {
    StepAP203_ChangeRequestItem Var0 = ent->Items()->Value(i1);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepAP203_RWChangeRequest::Share(const Handle(StepAP203_ChangeRequest)& ent,
                                        Interface_EntityIterator&              iter) const
{

  // Inherited fields of ActionRequestAssignment

  iter.AddItem(ent->StepBasic_ActionRequestAssignment::AssignedActionRequest());

  // Own fields of ChangeRequest

  for (Standard_Integer i2 = 1; i2 <= ent->Items()->Length(); i2++)
  {
    StepAP203_ChangeRequestItem Var0 = ent->Items()->Value(i2);
    iter.AddItem(Var0.Value());
  }
}
