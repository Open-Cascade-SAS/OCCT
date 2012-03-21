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

#include <StepVisual_CompositeTextWithExtent.ixx>


StepVisual_CompositeTextWithExtent::StepVisual_CompositeTextWithExtent ()  {}

void StepVisual_CompositeTextWithExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText)
{

	StepVisual_CompositeText::Init(aName, aCollectedText);
}

void StepVisual_CompositeTextWithExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText,
	const Handle(StepVisual_PlanarExtent)& aExtent)
{
	// --- classe own fields ---
	extent = aExtent;
	// --- classe inherited fields ---
	StepVisual_CompositeText::Init(aName, aCollectedText);
}


void StepVisual_CompositeTextWithExtent::SetExtent(const Handle(StepVisual_PlanarExtent)& aExtent)
{
	extent = aExtent;
}

Handle(StepVisual_PlanarExtent) StepVisual_CompositeTextWithExtent::Extent() const
{
	return extent;
}
