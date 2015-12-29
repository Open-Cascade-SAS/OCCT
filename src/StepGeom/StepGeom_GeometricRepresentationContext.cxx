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
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_GeometricRepresentationContext,StepRepr_RepresentationContext)

StepGeom_GeometricRepresentationContext::StepGeom_GeometricRepresentationContext ()  {}

void StepGeom_GeometricRepresentationContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Standard_Integer aCoordinateSpaceDimension)
{
	// --- classe own fields ---
	coordinateSpaceDimension = aCoordinateSpaceDimension;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepGeom_GeometricRepresentationContext::SetCoordinateSpaceDimension(const Standard_Integer aCoordinateSpaceDimension)
{
	coordinateSpaceDimension = aCoordinateSpaceDimension;
}

Standard_Integer StepGeom_GeometricRepresentationContext::CoordinateSpaceDimension() const
{
	return coordinateSpaceDimension;
}
