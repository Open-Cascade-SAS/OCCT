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
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleUsage,Standard_Transient)

StepVisual_SurfaceStyleUsage::StepVisual_SurfaceStyleUsage ()  {}

void StepVisual_SurfaceStyleUsage::Init(
	const StepVisual_SurfaceSide aSide,
	const Handle(StepVisual_SurfaceSideStyle)& aStyle)
{
	// --- classe own fields ---
	side = aSide;
	style = aStyle;
}


void StepVisual_SurfaceStyleUsage::SetSide(const StepVisual_SurfaceSide aSide)
{
	side = aSide;
}

StepVisual_SurfaceSide StepVisual_SurfaceStyleUsage::Side() const
{
	return side;
}

void StepVisual_SurfaceStyleUsage::SetStyle(const Handle(StepVisual_SurfaceSideStyle)& aStyle)
{
	style = aStyle;
}

Handle(StepVisual_SurfaceSideStyle) StepVisual_SurfaceStyleUsage::Style() const
{
	return style;
}
