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
#include <StepGeom_Surface.hxx>
#include <StepShape_BoxDomain.hxx>
#include <StepShape_BoxedHalfSpace.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_BoxedHalfSpace,StepShape_HalfSpaceSolid)

StepShape_BoxedHalfSpace::StepShape_BoxedHalfSpace ()  {}

void StepShape_BoxedHalfSpace::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBaseSurface,
	const Standard_Boolean aAgreementFlag,
	const Handle(StepShape_BoxDomain)& aEnclosure)
{
	// --- classe own fields ---
	enclosure = aEnclosure;
	// --- classe inherited fields ---
	StepShape_HalfSpaceSolid::Init(aName, aBaseSurface, aAgreementFlag);
}


void StepShape_BoxedHalfSpace::SetEnclosure(const Handle(StepShape_BoxDomain)& aEnclosure)
{
	enclosure = aEnclosure;
}

Handle(StepShape_BoxDomain) StepShape_BoxedHalfSpace::Enclosure() const
{
	return enclosure;
}
