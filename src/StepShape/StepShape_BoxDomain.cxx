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

#include <StepShape_BoxDomain.ixx>


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
