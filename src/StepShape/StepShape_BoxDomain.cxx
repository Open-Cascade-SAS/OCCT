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
#include <StepShape_BoxDomain.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_BoxDomain,Standard_Transient)

StepShape_BoxDomain::StepShape_BoxDomain ()  {}

void StepShape_BoxDomain::Init(
	const Handle(StepGeom_CartesianPoint)& aCorner,
	const Standard_Real aXlength,
	const Standard_Real aYlength,
	const Standard_Real aZlength)
{
	// --- classe own fields ---
	corner = aCorner;
	xlength = aXlength;
	ylength = aYlength;
	zlength = aZlength;
}


void StepShape_BoxDomain::SetCorner(const Handle(StepGeom_CartesianPoint)& aCorner)
{
	corner = aCorner;
}

Handle(StepGeom_CartesianPoint) StepShape_BoxDomain::Corner() const
{
	return corner;
}

void StepShape_BoxDomain::SetXlength(const Standard_Real aXlength)
{
	xlength = aXlength;
}

Standard_Real StepShape_BoxDomain::Xlength() const
{
	return xlength;
}

void StepShape_BoxDomain::SetYlength(const Standard_Real aYlength)
{
	ylength = aYlength;
}

Standard_Real StepShape_BoxDomain::Ylength() const
{
	return ylength;
}

void StepShape_BoxDomain::SetZlength(const Standard_Real aZlength)
{
	zlength = aZlength;
}

Standard_Real StepShape_BoxDomain::Zlength() const
{
	return zlength;
}
