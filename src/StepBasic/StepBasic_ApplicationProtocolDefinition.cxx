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
