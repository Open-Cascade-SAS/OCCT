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

#include <StepVisual_ColourRgb.ixx>


StepVisual_ColourRgb::StepVisual_ColourRgb ()  {}

void StepVisual_ColourRgb::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepVisual_ColourSpecification::Init(aName);
}

void StepVisual_ColourRgb::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aRed,
	const Standard_Real aGreen,
	const Standard_Real aBlue)
{
	// --- classe own fields ---
	red = aRed;
	green = aGreen;
	blue = aBlue;
	// --- classe inherited fields ---
	StepVisual_ColourSpecification::Init(aName);
}


void StepVisual_ColourRgb::SetRed(const Standard_Real aRed)
{
	red = aRed;
}

Standard_Real StepVisual_ColourRgb::Red() const
{
	return red;
}

void StepVisual_ColourRgb::SetGreen(const Standard_Real aGreen)
{
	green = aGreen;
}

Standard_Real StepVisual_ColourRgb::Green() const
{
	return green;
}

void StepVisual_ColourRgb::SetBlue(const Standard_Real aBlue)
{
	blue = aBlue;
}

Standard_Real StepVisual_ColourRgb::Blue() const
{
	return blue;
}
