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
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_StyledItem.hxx>
#include <TCollection_HAsciiString.hxx>

StepVisual_StyledItem::StepVisual_StyledItem ()  {}

void StepVisual_StyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_StyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem)
{
	// --- classe own fields ---
	styles = aStyles;
	item = aItem;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_StyledItem::SetStyles(const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfPresentationStyleAssignment) StepVisual_StyledItem::Styles() const
{
	return styles;
}

Handle(StepVisual_PresentationStyleAssignment) StepVisual_StyledItem::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_StyledItem::NbStyles () const
{
	if (styles.IsNull()) return 0;
	return styles->Length();
}

void StepVisual_StyledItem::SetItem(const Handle(StepRepr_RepresentationItem)& aItem)
{
	item = aItem;
}

Handle(StepRepr_RepresentationItem) StepVisual_StyledItem::Item() const
{
	return item;
}
