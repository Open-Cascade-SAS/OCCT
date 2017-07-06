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
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_InvisibleItem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_Invisibility,Standard_Transient)

StepVisual_Invisibility::StepVisual_Invisibility ()  {}

void StepVisual_Invisibility::Init(
	const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems)
{
	// --- classe own fields ---
	invisibleItems = aInvisibleItems;
}


void StepVisual_Invisibility::SetInvisibleItems(const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems)
{
	invisibleItems = aInvisibleItems;
}

Handle(StepVisual_HArray1OfInvisibleItem) StepVisual_Invisibility::InvisibleItems() const
{
	return invisibleItems;
}

StepVisual_InvisibleItem StepVisual_Invisibility::InvisibleItemsValue(const Standard_Integer num) const
{
	return invisibleItems->Value(num);
}

Standard_Integer StepVisual_Invisibility::NbInvisibleItems () const
{
	return invisibleItems->Length();
}
