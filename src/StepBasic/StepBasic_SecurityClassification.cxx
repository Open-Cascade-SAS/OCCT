#include <StepBasic_SecurityClassification.ixx>


StepBasic_SecurityClassification::StepBasic_SecurityClassification ()  {}

void StepBasic_SecurityClassification::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aPurpose,
	const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel)
{
	// --- classe own fields ---
	name = aName;
	purpose = aPurpose;
	securityLevel = aSecurityLevel;
}


void StepBasic_SecurityClassification::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_SecurityClassification::Name() const
{
	return name;
}

void StepBasic_SecurityClassification::SetPurpose(const Handle(TCollection_HAsciiString)& aPurpose)
{
	purpose = aPurpose;
}

Handle(TCollection_HAsciiString) StepBasic_SecurityClassification::Purpose() const
{
	return purpose;
}

void StepBasic_SecurityClassification::SetSecurityLevel(const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel)
{
	securityLevel = aSecurityLevel;
}

Handle(StepBasic_SecurityClassificationLevel) StepBasic_SecurityClassification::SecurityLevel() const
{
	return securityLevel;
}
