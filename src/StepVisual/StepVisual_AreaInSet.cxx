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
#include <StepVisual_AreaInSet.hxx>
#include <StepVisual_PresentationArea.hxx>
#include <StepVisual_PresentationSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_AreaInSet,Standard_Transient)

StepVisual_AreaInSet::StepVisual_AreaInSet ()  {}

void StepVisual_AreaInSet::Init(
	const Handle(StepVisual_PresentationArea)& aArea,
	const Handle(StepVisual_PresentationSet)& aInSet)
{
	// --- classe own fields ---
	area = aArea;
	inSet = aInSet;
}


void StepVisual_AreaInSet::SetArea(const Handle(StepVisual_PresentationArea)& aArea)
{
	area = aArea;
}

Handle(StepVisual_PresentationArea) StepVisual_AreaInSet::Area() const
{
	return area;
}

void StepVisual_AreaInSet::SetInSet(const Handle(StepVisual_PresentationSet)& aInSet)
{
	inSet = aInSet;
}

Handle(StepVisual_PresentationSet) StepVisual_AreaInSet::InSet() const
{
	return inSet;
}
