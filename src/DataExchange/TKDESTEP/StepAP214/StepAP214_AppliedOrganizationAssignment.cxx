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

#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_OrganizationItem.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_OrganizationRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedOrganizationAssignment,
                           StepBasic_OrganizationAssignment)

StepAP214_AppliedOrganizationAssignment::StepAP214_AppliedOrganizationAssignment() {}

void StepAP214_AppliedOrganizationAssignment::Init(
  const occ::handle<StepBasic_Organization>&              aAssignedOrganization,
  const occ::handle<StepBasic_OrganizationRole>&          aRole,
  const occ::handle<NCollection_HArray1<StepAP214_OrganizationItem>>& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_OrganizationAssignment::Init(aAssignedOrganization, aRole);
}

void StepAP214_AppliedOrganizationAssignment::SetItems(
  const occ::handle<NCollection_HArray1<StepAP214_OrganizationItem>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<StepAP214_OrganizationItem>> StepAP214_AppliedOrganizationAssignment::Items() const
{
  return items;
}

StepAP214_OrganizationItem StepAP214_AppliedOrganizationAssignment::ItemsValue(
  const int num) const
{
  return items->Value(num);
}

int StepAP214_AppliedOrganizationAssignment::NbItems() const
{
  return items->Length();
}
