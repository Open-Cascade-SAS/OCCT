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
#include <StepGeom_Axis2Placement.hxx>
#include <StepGeom_Conic.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Conic,StepGeom_Curve)

StepGeom_Conic::StepGeom_Conic ()  {}

void StepGeom_Conic::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{
	// --- classe own fields ---
	position = aPosition;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Conic::SetPosition(const StepGeom_Axis2Placement& aPosition)
{
	position = aPosition;
}

StepGeom_Axis2Placement StepGeom_Conic::Position() const
{
	return position;
}
