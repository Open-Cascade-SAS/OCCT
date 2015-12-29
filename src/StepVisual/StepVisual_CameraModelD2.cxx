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
#include <StepVisual_CameraModelD2.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CameraModelD2,StepVisual_CameraModel)

StepVisual_CameraModelD2::StepVisual_CameraModelD2 ()  {}

void StepVisual_CameraModelD2::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_PlanarBox)& aViewWindow,
	const Standard_Boolean aViewWindowClipping)
{
	// --- classe own fields ---
	viewWindow = aViewWindow;
	viewWindowClipping = aViewWindowClipping;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_CameraModelD2::SetViewWindow(const Handle(StepVisual_PlanarBox)& aViewWindow)
{
	viewWindow = aViewWindow;
}

Handle(StepVisual_PlanarBox) StepVisual_CameraModelD2::ViewWindow() const
{
	return viewWindow;
}

void StepVisual_CameraModelD2::SetViewWindowClipping(const Standard_Boolean aViewWindowClipping)
{
	viewWindowClipping = aViewWindowClipping;
}

Standard_Boolean StepVisual_CameraModelD2::ViewWindowClipping() const
{
	return viewWindowClipping;
}
