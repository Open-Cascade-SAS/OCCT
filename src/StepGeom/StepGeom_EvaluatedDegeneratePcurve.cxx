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
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_EvaluatedDegeneratePcurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_EvaluatedDegeneratePcurve,StepGeom_DegeneratePcurve)

StepGeom_EvaluatedDegeneratePcurve::StepGeom_EvaluatedDegeneratePcurve ()  {}

void StepGeom_EvaluatedDegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve,
	const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	// --- classe own fields ---
	equivalentPoint = aEquivalentPoint;
	// --- classe inherited fields ---
	StepGeom_DegeneratePcurve::Init(aName, aBasisSurface, aReferenceToCurve);
}


void StepGeom_EvaluatedDegeneratePcurve::SetEquivalentPoint(const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	equivalentPoint = aEquivalentPoint;
}

Handle(StepGeom_CartesianPoint) StepGeom_EvaluatedDegeneratePcurve::EquivalentPoint() const
{
	return equivalentPoint;
}
