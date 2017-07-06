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
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PresentationSize.hxx>
#include <StepVisual_PresentationSizeAssignmentSelect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PresentationSize,Standard_Transient)

StepVisual_PresentationSize::StepVisual_PresentationSize ()  {}

void StepVisual_PresentationSize::Init(
	const StepVisual_PresentationSizeAssignmentSelect& aUnit,
	const Handle(StepVisual_PlanarBox)& aSize)
{
	// --- classe own fields ---
	unit = aUnit;
	size = aSize;
}


void StepVisual_PresentationSize::SetUnit(const StepVisual_PresentationSizeAssignmentSelect& aUnit)
{
	unit = aUnit;
}

StepVisual_PresentationSizeAssignmentSelect StepVisual_PresentationSize::Unit() const
{
	return unit;
}

void StepVisual_PresentationSize::SetSize(const Handle(StepVisual_PlanarBox)& aSize)
{
	size = aSize;
}

Handle(StepVisual_PlanarBox) StepVisual_PresentationSize::Size() const
{
	return size;
}
