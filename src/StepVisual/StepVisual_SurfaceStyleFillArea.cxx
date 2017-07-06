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
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_SurfaceStyleFillArea.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleFillArea,Standard_Transient)

StepVisual_SurfaceStyleFillArea::StepVisual_SurfaceStyleFillArea ()  {}

void StepVisual_SurfaceStyleFillArea::Init(
	const Handle(StepVisual_FillAreaStyle)& aFillArea)
{
	// --- classe own fields ---
	fillArea = aFillArea;
}


void StepVisual_SurfaceStyleFillArea::SetFillArea(const Handle(StepVisual_FillAreaStyle)& aFillArea)
{
	fillArea = aFillArea;
}

Handle(StepVisual_FillAreaStyle) StepVisual_SurfaceStyleFillArea::FillArea() const
{
	return fillArea;
}
