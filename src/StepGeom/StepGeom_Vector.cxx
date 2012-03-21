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

#include <StepGeom_Vector.ixx>


StepGeom_Vector::StepGeom_Vector ()  {}

void StepGeom_Vector::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Vector::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Direction)& aOrientation,
	const Standard_Real aMagnitude)
{
	// --- classe own fields ---
	orientation = aOrientation;
	magnitude = aMagnitude;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Vector::SetOrientation(const Handle(StepGeom_Direction)& aOrientation)
{
	orientation = aOrientation;
}

Handle(StepGeom_Direction) StepGeom_Vector::Orientation() const
{
	return orientation;
}

void StepGeom_Vector::SetMagnitude(const Standard_Real aMagnitude)
{
	magnitude = aMagnitude;
}

Standard_Real StepGeom_Vector::Magnitude() const
{
	return magnitude;
}
