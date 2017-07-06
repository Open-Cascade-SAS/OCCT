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
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApplicationProtocolDefinition,Standard_Transient)

StepBasic_ApplicationProtocolDefinition::StepBasic_ApplicationProtocolDefinition ()  {}

void StepBasic_ApplicationProtocolDefinition::Init(
	const Handle(TCollection_HAsciiString)& aStatus,
	const Handle(TCollection_HAsciiString)& aApplicationInterpretedModelSchemaName,
	const Standard_Integer aApplicationProtocolYear,
	const Handle(StepBasic_ApplicationContext)& aApplication)
{
	// --- classe own fields ---
	status = aStatus;
	applicationInterpretedModelSchemaName = aApplicationInterpretedModelSchemaName;
	applicationProtocolYear = aApplicationProtocolYear;
	application = aApplication;
}


void StepBasic_ApplicationProtocolDefinition::SetStatus(const Handle(TCollection_HAsciiString)& aStatus)
{
	status = aStatus;
}

Handle(TCollection_HAsciiString) StepBasic_ApplicationProtocolDefinition::Status() const
{
	return status;
}

void StepBasic_ApplicationProtocolDefinition::SetApplicationInterpretedModelSchemaName(const Handle(TCollection_HAsciiString)& aApplicationInterpretedModelSchemaName)
{
	applicationInterpretedModelSchemaName = aApplicationInterpretedModelSchemaName;
}

Handle(TCollection_HAsciiString) StepBasic_ApplicationProtocolDefinition::ApplicationInterpretedModelSchemaName() const
{
	return applicationInterpretedModelSchemaName;
}

void StepBasic_ApplicationProtocolDefinition::SetApplicationProtocolYear(const Standard_Integer aApplicationProtocolYear)
{
	applicationProtocolYear = aApplicationProtocolYear;
}

Standard_Integer StepBasic_ApplicationProtocolDefinition::ApplicationProtocolYear() const
{
	return applicationProtocolYear;
}

void StepBasic_ApplicationProtocolDefinition::SetApplication(const Handle(StepBasic_ApplicationContext)& aApplication)
{
	application = aApplication;
}

Handle(StepBasic_ApplicationContext) StepBasic_ApplicationProtocolDefinition::Application() const
{
	return application;
}
