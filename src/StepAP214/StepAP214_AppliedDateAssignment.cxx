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
#include <StepAP214_AppliedDateAssignment.hxx>
#include <StepAP214_DateItem.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedDateAssignment,StepBasic_DateAssignment)

StepAP214_AppliedDateAssignment::StepAP214_AppliedDateAssignment ()  {}

void StepAP214_AppliedDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole,
	const Handle(StepAP214_HArray1OfDateItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}


void StepAP214_AppliedDateAssignment::SetItems(const Handle(StepAP214_HArray1OfDateItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfDateItem) StepAP214_AppliedDateAssignment::Items() const
{
  return items;
}

StepAP214_DateItem StepAP214_AppliedDateAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedDateAssignment::NbItems () const
{
  return items->Length();
}
