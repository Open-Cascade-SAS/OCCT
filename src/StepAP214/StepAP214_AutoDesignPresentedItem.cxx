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

#include <StepAP214_AutoDesignPresentedItem.ixx>


StepAP214_AutoDesignPresentedItem::StepAP214_AutoDesignPresentedItem ()  {}

void StepAP214_AutoDesignPresentedItem::Init(
	const Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect)& aItems)
{
	// --- classe own fields ---
	items = aItems;
}


void StepAP214_AutoDesignPresentedItem::SetItems(const Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect) StepAP214_AutoDesignPresentedItem::Items() const
{
	return items;
}

StepAP214_AutoDesignPresentedItemSelect  StepAP214_AutoDesignPresentedItem::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignPresentedItem::NbItems () const
{
	if (items.IsNull()) return 0;
	return items->Length();
}
