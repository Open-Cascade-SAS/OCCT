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
#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_DateAndTimeItem.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedDateAndTimeAssignment,StepBasic_DateAndTimeAssignment)

StepAP214_AppliedDateAndTimeAssignment::StepAP214_AppliedDateAndTimeAssignment ()  {}

void StepAP214_AppliedDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole,
	const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}


void StepAP214_AppliedDateAndTimeAssignment::SetItems(const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfDateAndTimeItem) StepAP214_AppliedDateAndTimeAssignment::Items() const
{
  return items;
}

StepAP214_DateAndTimeItem StepAP214_AppliedDateAndTimeAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedDateAndTimeAssignment::NbItems () const
{
  return items->Length();
}
