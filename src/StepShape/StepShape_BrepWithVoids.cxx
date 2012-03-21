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

#include <StepShape_BrepWithVoids.ixx>


StepShape_BrepWithVoids::StepShape_BrepWithVoids ()  {}

void StepShape_BrepWithVoids::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter)
{

	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}

void StepShape_BrepWithVoids::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter,
	const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	// --- classe own fields ---
	voids = aVoids;
	// --- classe inherited fields ---
	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}


void StepShape_BrepWithVoids::SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	voids = aVoids;
}

Handle(StepShape_HArray1OfOrientedClosedShell) StepShape_BrepWithVoids::Voids() const
{
	return voids;
}

Handle(StepShape_OrientedClosedShell) StepShape_BrepWithVoids::VoidsValue(const Standard_Integer num) const
{
	return voids->Value(num);
}

Standard_Integer StepShape_BrepWithVoids::NbVoids () const
{
	if (voids.IsNull()) return 0;
	return voids->Length();
}
