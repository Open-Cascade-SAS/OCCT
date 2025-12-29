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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepAP203_RWChangeRequest::RWStepAP203_RWChangeRequest() = default;

//=================================================================================================

void RWStepAP203_RWChangeRequest::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                                   num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepAP203_ChangeRequest>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "change_request"))
    return;

  // Inherited fields of ActionRequestAssignment

  occ::handle<StepBasic_VersionedActionRequest> aActionRequestAssignment_AssignedActionRequest;
  data->ReadEntity(num,
                   1,
                   "action_request_assignment.assigned_action_request",
                   ach,
                   STANDARD_TYPE(StepBasic_VersionedActionRequest),
                   aActionRequestAssignment_AssignedActionRequest);

  // Own fields of ChangeRequest

  occ::handle<NCollection_HArray1<StepAP203_ChangeRequestItem>> aItems;
  int                                                           sub2 = 0;
  if (data->ReadSubList(num, 2, "items", ach, sub2))
  {
    int num2 = sub2;
    int nb0  = data->NbParams(num2);
    aItems   = new NCollection_HArray1<StepAP203_ChangeRequestItem>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
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

void RWStepAP203_RWChangeRequest::WriteStep(StepData_StepWriter&                        SW,
                                            const occ::handle<StepAP203_ChangeRequest>& ent) const
{

  // Inherited fields of ActionRequestAssignment

  SW.Send(ent->StepBasic_ActionRequestAssignment::AssignedActionRequest());

  // Own fields of ChangeRequest

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->Items()->Length(); i1++)
  {
    StepAP203_ChangeRequestItem Var0 = ent->Items()->Value(i1);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepAP203_RWChangeRequest::Share(const occ::handle<StepAP203_ChangeRequest>& ent,
                                        Interface_EntityIterator&                   iter) const
{

  // Inherited fields of ActionRequestAssignment

  iter.AddItem(ent->StepBasic_ActionRequestAssignment::AssignedActionRequest());

  // Own fields of ChangeRequest

  for (int i2 = 1; i2 <= ent->Items()->Length(); i2++)
  {
    StepAP203_ChangeRequestItem Var0 = ent->Items()->Value(i2);
    iter.AddItem(Var0.Value());
  }
}
