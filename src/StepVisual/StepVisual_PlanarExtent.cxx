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
#include <StepVisual_PlanarExtent.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PlanarExtent,StepGeom_GeometricRepresentationItem)

StepVisual_PlanarExtent::StepVisual_PlanarExtent ()  {}

void StepVisual_PlanarExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aSizeInX,
	const Standard_Real aSizeInY)
{
	// --- classe own fields ---
	sizeInX = aSizeInX;
	sizeInY = aSizeInY;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_PlanarExtent::SetSizeInX(const Standard_Real aSizeInX)
{
	sizeInX = aSizeInX;
}

Standard_Real StepVisual_PlanarExtent::SizeInX() const
{
	return sizeInX;
}

void StepVisual_PlanarExtent::SetSizeInY(const Standard_Real aSizeInY)
{
	sizeInY = aSizeInY;
}

Standard_Real StepVisual_PlanarExtent::SizeInY() const
{
	return sizeInY;
}
