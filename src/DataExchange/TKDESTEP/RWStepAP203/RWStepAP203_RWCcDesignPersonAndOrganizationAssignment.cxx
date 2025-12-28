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
#include "RWStepAP203_RWCcDesignPersonAndOrganizationAssignment.pxx"
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_PersonOrganizationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepAP203_PersonOrganizationItem.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::
  RWStepAP203_RWCcDesignPersonAndOrganizationAssignment()
{
}

//=================================================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::ReadStep(
  const occ::handle<StepData_StepReaderData>&                           data,
  const int                                           num,
  occ::handle<Interface_Check>&                                         ach,
  const occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 3, ach, "cc_design_person_and_organization_assignment"))
    return;

  // Inherited fields of PersonAndOrganizationAssignment

  occ::handle<StepBasic_PersonAndOrganization>
    aPersonAndOrganizationAssignment_AssignedPersonAndOrganization;
  data->ReadEntity(num,
                   1,
                   "person_and_organization_assignment.assigned_person_and_organization",
                   ach,
                   STANDARD_TYPE(StepBasic_PersonAndOrganization),
                   aPersonAndOrganizationAssignment_AssignedPersonAndOrganization);

  occ::handle<StepBasic_PersonAndOrganizationRole> aPersonAndOrganizationAssignment_Role;
  data->ReadEntity(num,
                   2,
                   "person_and_organization_assignment.role",
                   ach,
                   STANDARD_TYPE(StepBasic_PersonAndOrganizationRole),
                   aPersonAndOrganizationAssignment_Role);

  // Own fields of CcDesignPersonAndOrganizationAssignment

  occ::handle<NCollection_HArray1<StepAP203_PersonOrganizationItem>> aItems;
  int                                  sub3 = 0;
  if (data->ReadSubList(num, 3, "items", ach, sub3))
  {
    int num2 = sub3;
    int nb0  = data->NbParams(num2);
    aItems                = new NCollection_HArray1<StepAP203_PersonOrganizationItem>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepAP203_PersonOrganizationItem anIt0;
      data->ReadEntity(num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aPersonAndOrganizationAssignment_AssignedPersonAndOrganization,
            aPersonAndOrganizationAssignment_Role,
            aItems);
}

//=================================================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::WriteStep(
  StepData_StepWriter&                                             SW,
  const occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment>& ent) const
{

  // Inherited fields of PersonAndOrganizationAssignment

  SW.Send(ent->StepBasic_PersonAndOrganizationAssignment::AssignedPersonAndOrganization());

  SW.Send(ent->StepBasic_PersonAndOrganizationAssignment::Role());

  // Own fields of CcDesignPersonAndOrganizationAssignment

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->Items()->Length(); i2++)
  {
    StepAP203_PersonOrganizationItem Var0 = ent->Items()->Value(i2);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::Share(
  const occ::handle<StepAP203_CcDesignPersonAndOrganizationAssignment>& ent,
  Interface_EntityIterator&                                        iter) const
{

  // Inherited fields of PersonAndOrganizationAssignment

  iter.AddItem(ent->StepBasic_PersonAndOrganizationAssignment::AssignedPersonAndOrganization());

  iter.AddItem(ent->StepBasic_PersonAndOrganizationAssignment::Role());

  // Own fields of CcDesignPersonAndOrganizationAssignment

  for (int i3 = 1; i3 <= ent->Items()->Length(); i3++)
  {
    StepAP203_PersonOrganizationItem Var0 = ent->Items()->Value(i3);
    iter.AddItem(Var0.Value());
  }
}
