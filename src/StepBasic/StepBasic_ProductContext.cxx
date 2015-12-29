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
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ProductContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductContext,StepBasic_ApplicationContextElement)

StepBasic_ProductContext::StepBasic_ProductContext ()  {}

void StepBasic_ProductContext::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference,
	const Handle(TCollection_HAsciiString)& aDisciplineType)
{
	// --- classe own fields ---
	disciplineType = aDisciplineType;
	// --- classe inherited fields ---
	StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}


void StepBasic_ProductContext::SetDisciplineType(const Handle(TCollection_HAsciiString)& aDisciplineType)
{
	disciplineType = aDisciplineType;
}

Handle(TCollection_HAsciiString) StepBasic_ProductContext::DisciplineType() const
{
	return disciplineType;
}
