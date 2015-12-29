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
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_RectangularTrimmedSurface,StepGeom_BoundedSurface)

StepGeom_RectangularTrimmedSurface::StepGeom_RectangularTrimmedSurface ()  {}

void StepGeom_RectangularTrimmedSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Standard_Real aU1,
	const Standard_Real aU2,
	const Standard_Real aV1,
	const Standard_Real aV2,
	const Standard_Boolean aUsense,
	const Standard_Boolean aVsense)
{
	// --- classe own fields ---
	basisSurface = aBasisSurface;
	u1 = aU1;
	u2 = aU2;
	v1 = aV1;
	v2 = aV2;
	usense = aUsense;
	vsense = aVsense;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_RectangularTrimmedSurface::SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface)
{
	basisSurface = aBasisSurface;
}

Handle(StepGeom_Surface) StepGeom_RectangularTrimmedSurface::BasisSurface() const
{
	return basisSurface;
}

void StepGeom_RectangularTrimmedSurface::SetU1(const Standard_Real aU1)
{
	u1 = aU1;
}

Standard_Real StepGeom_RectangularTrimmedSurface::U1() const
{
	return u1;
}

void StepGeom_RectangularTrimmedSurface::SetU2(const Standard_Real aU2)
{
	u2 = aU2;
}

Standard_Real StepGeom_RectangularTrimmedSurface::U2() const
{
	return u2;
}

void StepGeom_RectangularTrimmedSurface::SetV1(const Standard_Real aV1)
{
	v1 = aV1;
}

Standard_Real StepGeom_RectangularTrimmedSurface::V1() const
{
	return v1;
}

void StepGeom_RectangularTrimmedSurface::SetV2(const Standard_Real aV2)
{
	v2 = aV2;
}

Standard_Real StepGeom_RectangularTrimmedSurface::V2() const
{
	return v2;
}

void StepGeom_RectangularTrimmedSurface::SetUsense(const Standard_Boolean aUsense)
{
	usense = aUsense;
}

Standard_Boolean StepGeom_RectangularTrimmedSurface::Usense() const
{
	return usense;
}

void StepGeom_RectangularTrimmedSurface::SetVsense(const Standard_Boolean aVsense)
{
	vsense = aVsense;
}

Standard_Boolean StepGeom_RectangularTrimmedSurface::Vsense() const
{
	return vsense;
}
