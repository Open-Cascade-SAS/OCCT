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
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepAP214_SecurityClassificationItem.hxx>
#include <StepBasic_SecurityClassification.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedSecurityClassificationAssignment,StepBasic_SecurityClassificationAssignment)

StepAP214_AppliedSecurityClassificationAssignment::StepAP214_AppliedSecurityClassificationAssignment ()  {}

void StepAP214_AppliedSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification,
	const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}


void StepAP214_AppliedSecurityClassificationAssignment::SetItems(const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfSecurityClassificationItem) StepAP214_AppliedSecurityClassificationAssignment::Items() const
{
  return items;
}

const StepAP214_SecurityClassificationItem& StepAP214_AppliedSecurityClassificationAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedSecurityClassificationAssignment::NbItems () const
{
  if (items.IsNull()) return 0;
  return items->Length();
}
