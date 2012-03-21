// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_ApplicationProtocolDefinition.ixx>


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
