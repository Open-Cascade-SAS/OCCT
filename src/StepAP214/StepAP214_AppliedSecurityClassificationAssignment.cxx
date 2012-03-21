// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepAP214_AppliedSecurityClassificationAssignment.ixx>


StepAP214_AppliedSecurityClassificationAssignment::StepAP214_AppliedSecurityClassificationAssignment ()  {}

void StepAP214_AppliedSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification)
{

  StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}

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
