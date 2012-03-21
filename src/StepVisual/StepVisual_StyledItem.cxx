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

#include <StepVisual_StyledItem.ixx>


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
