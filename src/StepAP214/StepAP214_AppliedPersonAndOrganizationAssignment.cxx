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


#include <Standard_Type.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_PersonAndOrganizationItem.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedPersonAndOrganizationAssignment,StepBasic_PersonAndOrganizationAssignment)

StepAP214_AppliedPersonAndOrganizationAssignment::StepAP214_AppliedPersonAndOrganizationAssignment ()  {}

void StepAP214_AppliedPersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}


void StepAP214_AppliedPersonAndOrganizationAssignment::SetItems(const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfPersonAndOrganizationItem) StepAP214_AppliedPersonAndOrganizationAssignment::Items() const
{
  return items;
}

StepAP214_PersonAndOrganizationItem StepAP214_AppliedPersonAndOrganizationAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedPersonAndOrganizationAssignment::NbItems () const
{
  return items->Length();
}
