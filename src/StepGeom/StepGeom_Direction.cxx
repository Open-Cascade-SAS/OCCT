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
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Direction,StepGeom_GeometricRepresentationItem)

StepGeom_Direction::StepGeom_Direction ()  {}

void StepGeom_Direction::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TColStd_HArray1OfReal)& aDirectionRatios)
{
	// --- classe own fields ---
	directionRatios = aDirectionRatios;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Direction::SetDirectionRatios(const Handle(TColStd_HArray1OfReal)& aDirectionRatios)
{
	directionRatios = aDirectionRatios;
}

Handle(TColStd_HArray1OfReal) StepGeom_Direction::DirectionRatios() const
{
	return directionRatios;
}

Standard_Real StepGeom_Direction::DirectionRatiosValue(const Standard_Integer num) const
{
	return directionRatios->Value(num);
}

Standard_Integer StepGeom_Direction::NbDirectionRatios () const
{
	return directionRatios->Length();
}
