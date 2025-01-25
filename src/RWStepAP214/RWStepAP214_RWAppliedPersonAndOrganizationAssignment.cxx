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
#include "RWStepAP214_RWAppliedPersonAndOrganizationAssignment.pxx"
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_HArray1OfPersonAndOrganizationItem.hxx>
#include <StepAP214_PersonAndOrganizationItem.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepAP214_RWAppliedPersonAndOrganizationAssignment::
  RWStepAP214_RWAppliedPersonAndOrganizationAssignment()
{
}

void RWStepAP214_RWAppliedPersonAndOrganizationAssignment::ReadStep(
  const Handle(StepData_StepReaderData)&                          data,
  const Standard_Integer                                          num,
  Handle(Interface_Check)&                                        ach,
  const Handle(StepAP214_AppliedPersonAndOrganizationAssignment)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "applied_person_and_organization_assignment"))
    return;

  // --- inherited field : assignedPersonAndOrganization ---

  Handle(StepBasic_PersonAndOrganization) aAssignedPersonAndOrganization;
  data->ReadEntity(num,
                   1,
                   "assigned_person_and_organization",
                   ach,
                   STANDARD_TYPE(StepBasic_PersonAndOrganization),
                   aAssignedPersonAndOrganization);

  // --- inherited field : role ---

  Handle(StepBasic_PersonAndOrganizationRole) aRole;
  data->ReadEntity(num, 2, "role", ach, STANDARD_TYPE(StepBasic_PersonAndOrganizationRole), aRole);

  // --- own field : items ---

  Handle(StepAP214_HArray1OfPersonAndOrganizationItem) aItems;
  StepAP214_PersonAndOrganizationItem                  aItemsItem;
  Standard_Integer                                     nsub3;
  if (data->ReadSubList(num, 3, "items", ach, nsub3))
  {
    Standard_Integer nb3 = data->NbParams(nsub3);
    aItems               = new StepAP214_HArray1OfPersonAndOrganizationItem(1, nb3);
    for (Standard_Integer i3 = 1; i3 <= nb3; i3++)
    {
      Standard_Boolean stat3 = data->ReadEntity(nsub3, i3, "items", ach, aItemsItem);
      if (stat3)
        aItems->SetValue(i3, aItemsItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aAssignedPersonAndOrganization, aRole, aItems);
}

void RWStepAP214_RWAppliedPersonAndOrganizationAssignment::WriteStep(
  StepData_StepWriter&                                            SW,
  const Handle(StepAP214_AppliedPersonAndOrganizationAssignment)& ent) const
{

  // --- inherited field assignedPersonAndOrganization ---

  SW.Send(ent->AssignedPersonAndOrganization());

  // --- inherited field role ---

  SW.Send(ent->Role());

  // --- own field : items ---

  SW.OpenSub();
  for (Standard_Integer i3 = 1; i3 <= ent->NbItems(); i3++)
  {
    SW.Send(ent->ItemsValue(i3).Value());
  }
  SW.CloseSub();
}

void RWStepAP214_RWAppliedPersonAndOrganizationAssignment::Share(
  const Handle(StepAP214_AppliedPersonAndOrganizationAssignment)& ent,
  Interface_EntityIterator&                                       iter) const
{

  iter.GetOneItem(ent->AssignedPersonAndOrganization());
  iter.GetOneItem(ent->Role());
  Standard_Integer nbElem3 = ent->NbItems();
  for (Standard_Integer is3 = 1; is3 <= nbElem3; is3++)
  {
    iter.GetOneItem(ent->ItemsValue(is3).Value());
  }
}
