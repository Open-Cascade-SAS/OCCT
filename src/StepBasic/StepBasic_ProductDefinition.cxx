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
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinition,Standard_Transient)

StepBasic_ProductDefinition::StepBasic_ProductDefinition ()  {}

void StepBasic_ProductDefinition::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_ProductDefinitionFormation)& aFormation,
	const Handle(StepBasic_ProductDefinitionContext)& aFrameOfReference)
{
	// --- classe own fields ---
	id = aId;
	description = aDescription;
	formation = aFormation;
	frameOfReference = aFrameOfReference;
}


void StepBasic_ProductDefinition::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinition::Id() const
{
	return id;
}

void StepBasic_ProductDefinition::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinition::Description() const
{
	return description;
}

void StepBasic_ProductDefinition::SetFormation(const Handle(StepBasic_ProductDefinitionFormation)& aFormation)
{
	formation = aFormation;
}

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductDefinition::Formation() const
{
	return formation;
}

void StepBasic_ProductDefinition::SetFrameOfReference(const Handle(StepBasic_ProductDefinitionContext)& aFrameOfReference)
{
	frameOfReference = aFrameOfReference;
}

Handle(StepBasic_ProductDefinitionContext) StepBasic_ProductDefinition::FrameOfReference() const
{
	return frameOfReference;
}
