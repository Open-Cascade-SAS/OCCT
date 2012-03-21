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

#include <StepGeom_RectangularTrimmedSurface.ixx>


StepGeom_RectangularTrimmedSurface::StepGeom_RectangularTrimmedSurface ()  {}

void StepGeom_RectangularTrimmedSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

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
