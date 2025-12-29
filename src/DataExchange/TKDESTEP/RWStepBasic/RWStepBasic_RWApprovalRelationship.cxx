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

// gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWApprovalRelationship.pxx"
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWApprovalRelationship::RWStepBasic_RWApprovalRelationship() = default;

void RWStepBasic_RWApprovalRelationship::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepBasic_ApprovalRelationship>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "approval_relationship"))
    return;

  // --- own field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  if (data->IsParamDefined(num, 2))
  { // gka 05.03.99 S4134 upgrade from CD to DIS
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadString(num, 2, "description", ach, aDescription);
  }
  // --- own field : relatingApproval ---

  occ::handle<StepBasic_Approval> aRelatingApproval;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "relating_approval",
                   ach,
                   STANDARD_TYPE(StepBasic_Approval),
                   aRelatingApproval);

  // --- own field : relatedApproval ---

  occ::handle<StepBasic_Approval> aRelatedApproval;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadEntity(num,
                   4,
                   "related_approval",
                   ach,
                   STANDARD_TYPE(StepBasic_Approval),
                   aRelatedApproval);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDescription, aRelatingApproval, aRelatedApproval);
}

void RWStepBasic_RWApprovalRelationship::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepBasic_ApprovalRelationship>& ent) const
{

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : description ---

  SW.Send(ent->Description());

  // --- own field : relatingApproval ---

  SW.Send(ent->RelatingApproval());

  // --- own field : relatedApproval ---

  SW.Send(ent->RelatedApproval());
}

void RWStepBasic_RWApprovalRelationship::Share(
  const occ::handle<StepBasic_ApprovalRelationship>& ent,
  Interface_EntityIterator&                          iter) const
{

  iter.GetOneItem(ent->RelatingApproval());

  iter.GetOneItem(ent->RelatedApproval());
}
