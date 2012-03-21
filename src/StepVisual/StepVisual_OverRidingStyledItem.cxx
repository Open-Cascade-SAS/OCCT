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

#include <StepVisual_OverRidingStyledItem.ixx>


StepVisual_OverRidingStyledItem::StepVisual_OverRidingStyledItem ()  {}

void StepVisual_OverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem)
{

	StepVisual_StyledItem::Init(aName, aStyles, aItem);
}

void StepVisual_OverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
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
