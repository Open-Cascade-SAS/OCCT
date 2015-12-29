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
#include <StepGeom_Direction.hxx>
#include <StepGeom_Vector.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Vector,StepGeom_GeometricRepresentationItem)

StepGeom_Vector::StepGeom_Vector ()  {}

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
