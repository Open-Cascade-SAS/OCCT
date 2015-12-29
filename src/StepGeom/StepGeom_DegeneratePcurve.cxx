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
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_DegeneratePcurve,StepGeom_Point)

StepGeom_DegeneratePcurve::StepGeom_DegeneratePcurve ()  {}

void StepGeom_DegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{
	// --- classe own fields ---
	basisSurface = aBasisSurface;
	referenceToCurve = aReferenceToCurve;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_DegeneratePcurve::SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface)
{
	basisSurface = aBasisSurface;
}

Handle(StepGeom_Surface) StepGeom_DegeneratePcurve::BasisSurface() const
{
	return basisSurface;
}

void StepGeom_DegeneratePcurve::SetReferenceToCurve(const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{
	referenceToCurve = aReferenceToCurve;
}

Handle(StepRepr_DefinitionalRepresentation) StepGeom_DegeneratePcurve::ReferenceToCurve() const
{
	return referenceToCurve;
}
