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
#include <StepVisual_ContextDependentInvisibility.hxx>
#include <StepVisual_InvisibilityContext.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ContextDependentInvisibility,StepVisual_Invisibility)

StepVisual_ContextDependentInvisibility::StepVisual_ContextDependentInvisibility ()  {}

void StepVisual_ContextDependentInvisibility::Init(
	const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems,
	const StepVisual_InvisibilityContext& aPresentationContext)
{
	// --- classe own fields ---
	presentationContext = aPresentationContext;
	// --- classe inherited fields ---
	StepVisual_Invisibility::Init(aInvisibleItems);
}


void StepVisual_ContextDependentInvisibility::SetPresentationContext(const StepVisual_InvisibilityContext& aPresentationContext)
{
	presentationContext = aPresentationContext;
}

StepVisual_InvisibilityContext StepVisual_ContextDependentInvisibility::PresentationContext() const
{
	return presentationContext;
}
