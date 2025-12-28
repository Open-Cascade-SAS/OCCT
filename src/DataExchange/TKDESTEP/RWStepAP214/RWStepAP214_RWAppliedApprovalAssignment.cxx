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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepAP214_RWAppliedApprovalAssignment.pxx"
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_ApprovalItem.hxx>
#include <StepAP214_ApprovalItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_Approval.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepAP214_RWAppliedApprovalAssignment::RWStepAP214_RWAppliedApprovalAssignment() {}

void RWStepAP214_RWAppliedApprovalAssignment::ReadStep(
  const occ::handle<StepData_StepReaderData>&             data,
  const int                             num,
  occ::handle<Interface_Check>&                           ach,
  const occ::handle<StepAP214_AppliedApprovalAssignment>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "applied_approval_assignment"))
    return;

  // --- inherited field : assignedApproval ---

  occ::handle<StepBasic_Approval> aAssignedApproval;
  data->ReadEntity(num,
                   1,
                   "assigned_approval",
                   ach,
                   STANDARD_TYPE(StepBasic_Approval),
                   aAssignedApproval);

  // --- own field : items ---

  occ::handle<NCollection_HArray1<StepAP214_ApprovalItem>> aItems;
  StepAP214_ApprovalItem                  aItemsItem;
  int                        nsub2;
  if (data->ReadSubList(num, 2, "items", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aItems               = new NCollection_HArray1<StepAP214_ApprovalItem>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      bool stat2 = data->ReadEntity(nsub2, i2, "items", ach, aItemsItem);
      if (stat2)
        aItems->SetValue(i2, aItemsItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aAssignedApproval, aItems);
}

void RWStepAP214_RWAppliedApprovalAssignment::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepAP214_AppliedApprovalAssignment>& ent) const
{

  // --- inherited field assignedApproval ---

  SW.Send(ent->AssignedApproval());

  // --- own field : items ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbItems(); i2++)
  {
    SW.Send(ent->ItemsValue(i2).Value());
  }
  SW.CloseSub();
}

void RWStepAP214_RWAppliedApprovalAssignment::Share(
  const occ::handle<StepAP214_AppliedApprovalAssignment>& ent,
  Interface_EntityIterator&                          iter) const
{

  iter.GetOneItem(ent->AssignedApproval());
  int nbElem2 = ent->NbItems();
  for (int is2 = 1; is2 <= nbElem2; is2++)
  {
    iter.GetOneItem(ent->ItemsValue(is2).Value());
  }
}
