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
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_OverRidingStyledItem.hxx>
#include <StepVisual_StyledItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_OverRidingStyledItem,StepVisual_StyledItem)

StepVisual_OverRidingStyledItem::StepVisual_OverRidingStyledItem ()  {}

void StepVisual_OverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(Standard_Transient)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{
	// --- classe own fields ---
	overRiddenStyle = aOverRiddenStyle;
	// --- classe inherited fields ---
	StepVisual_StyledItem::Init(aName, aStyles, aItem);
}


void StepVisual_OverRidingStyledItem::SetOverRiddenStyle(const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{
	overRiddenStyle = aOverRiddenStyle;
}

Handle(StepVisual_StyledItem) StepVisual_OverRidingStyledItem::OverRiddenStyle() const
{
	return overRiddenStyle;
}
