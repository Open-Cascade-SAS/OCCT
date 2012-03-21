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

#include <StepVisual_PresentationStyleAssignment.ixx>


StepVisual_PresentationStyleAssignment::StepVisual_PresentationStyleAssignment ()  {}

void StepVisual_PresentationStyleAssignment::Init(
	const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles)
{
	// --- classe own fields ---
	styles = aStyles;
}


void StepVisual_PresentationStyleAssignment::SetStyles(const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfPresentationStyleSelect) StepVisual_PresentationStyleAssignment::Styles() const
{
	return styles;
}

StepVisual_PresentationStyleSelect StepVisual_PresentationStyleAssignment::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_PresentationStyleAssignment::NbStyles () const
{
	return styles->Length();
}
