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
#include <StepShape_FaceBound.hxx>
#include <StepShape_Loop.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_FaceBound,StepShape_TopologicalRepresentationItem)

StepShape_FaceBound::StepShape_FaceBound ()  {}

void StepShape_FaceBound::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Loop)& aBound,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	bound = aBound;
	orientation = aOrientation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_FaceBound::SetBound(const Handle(StepShape_Loop)& aBound)
{
	bound = aBound;
}

Handle(StepShape_Loop) StepShape_FaceBound::Bound() const
{
	return bound;
}

void StepShape_FaceBound::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_FaceBound::Orientation() const
{
	return orientation;
}
