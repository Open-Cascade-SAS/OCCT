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
#include <StepVisual_AreaOrView.hxx>
#include <StepVisual_BackgroundColour.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_BackgroundColour,StepVisual_Colour)

StepVisual_BackgroundColour::StepVisual_BackgroundColour ()  {}

void StepVisual_BackgroundColour::Init(
	const StepVisual_AreaOrView& aPresentation)
{
	// --- classe own fields ---
	presentation = aPresentation;
}


void StepVisual_BackgroundColour::SetPresentation(const StepVisual_AreaOrView& aPresentation)
{
	presentation = aPresentation;
}

StepVisual_AreaOrView StepVisual_BackgroundColour::Presentation() const
{
	return presentation;
}
