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

#include <StepAP214_AutoDesignNominalDateAndTimeAssignment.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignNominalDateAndTimeAssignment,
                           StepBasic_DateAndTimeAssignment)

StepAP214_AutoDesignNominalDateAndTimeAssignment::StepAP214_AutoDesignNominalDateAndTimeAssignment()
{
}

void StepAP214_AutoDesignNominalDateAndTimeAssignment::Init(
  const Handle(StepBasic_DateAndTime)&                        aAssignedDateAndTime,
  const Handle(StepBasic_DateTimeRole)&                       aRole,
  const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}

void StepAP214_AutoDesignNominalDateAndTimeAssignment::SetItems(
  const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)
  StepAP214_AutoDesignNominalDateAndTimeAssignment::Items() const
{
  return items;
}

StepAP214_AutoDesignDateAndTimeItem StepAP214_AutoDesignNominalDateAndTimeAssignment::ItemsValue(
  const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignNominalDateAndTimeAssignment::NbItems() const
{
  return items->Length();
}
