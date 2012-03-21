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

#include <StepVisual_ContextDependentOverRidingStyledItem.ixx>


StepVisual_ContextDependentOverRidingStyledItem::StepVisual_ContextDependentOverRidingStyledItem ()  {}

void StepVisual_ContextDependentOverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{

	StepVisual_OverRidingStyledItem::Init(aName, aStyles, aItem, aOverRiddenStyle);
}

void StepVisual_ContextDependentOverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle,
	const Handle(StepVisual_HArray1OfStyleContextSelect)& aStyleContext)
{
	// --- classe own fields ---
	styleContext = aStyleContext;
	// --- classe inherited fields ---
	StepVisual_OverRidingStyledItem::Init(aName, aStyles, aItem, aOverRiddenStyle);
}


void StepVisual_ContextDependentOverRidingStyledItem::SetStyleContext(const Handle(StepVisual_HArray1OfStyleContextSelect)& aStyleContext)
{
	styleContext = aStyleContext;
}

Handle(StepVisual_HArray1OfStyleContextSelect) StepVisual_ContextDependentOverRidingStyledItem::StyleContext() const
{
	return styleContext;
}

StepVisual_StyleContextSelect StepVisual_ContextDependentOverRidingStyledItem::StyleContextValue(const Standard_Integer num) const
{
	return styleContext->Value(num);
}

Standard_Integer StepVisual_ContextDependentOverRidingStyledItem::NbStyleContext () const
{
	return styleContext->Length();
}
