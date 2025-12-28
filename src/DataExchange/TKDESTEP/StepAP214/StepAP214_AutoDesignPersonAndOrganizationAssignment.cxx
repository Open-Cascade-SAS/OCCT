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

#include <StepAP214_AutoDesignPersonAndOrganizationAssignment.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignPersonAndOrganizationAssignment,
                           StepBasic_PersonAndOrganizationAssignment)

StepAP214_AutoDesignPersonAndOrganizationAssignment::
  StepAP214_AutoDesignPersonAndOrganizationAssignment()
= default;

void StepAP214_AutoDesignPersonAndOrganizationAssignment::Init(
  const occ::handle<StepBasic_PersonAndOrganization>&     aAssignedPersonAndOrganization,
  const occ::handle<StepBasic_PersonAndOrganizationRole>& aRole,
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}

void StepAP214_AutoDesignPersonAndOrganizationAssignment::SetItems(
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>
  StepAP214_AutoDesignPersonAndOrganizationAssignment::Items() const
{
  return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignPersonAndOrganizationAssignment::ItemsValue(
  const int num) const
{
  return items->Value(num);
}

int StepAP214_AutoDesignPersonAndOrganizationAssignment::NbItems() const
{
  return items->Length();
}
