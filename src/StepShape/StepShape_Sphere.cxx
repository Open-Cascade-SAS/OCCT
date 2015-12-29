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
#include <StepGeom_Point.hxx>
#include <StepShape_Sphere.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Sphere,StepGeom_GeometricRepresentationItem)

StepShape_Sphere::StepShape_Sphere ()  {}

void StepShape_Sphere::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aRadius,
	const Handle(StepGeom_Point)& aCentre)
{
	// --- classe own fields ---
	radius = aRadius;
	centre = aCentre;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Sphere::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepShape_Sphere::Radius() const
{
	return radius;
}

void StepShape_Sphere::SetCentre(const Handle(StepGeom_Point)& aCentre)
{
	centre = aCentre;
}

Handle(StepGeom_Point) StepShape_Sphere::Centre() const
{
	return centre;
}
