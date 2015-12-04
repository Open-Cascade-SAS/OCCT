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
#include <StepAP214_AutoDesignPresentedItem.hxx>
#include <StepAP214_AutoDesignPresentedItemSelect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignPresentedItem,StepVisual_PresentedItem)

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
