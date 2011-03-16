#include <StepBasic_PersonAndOrganization.ixx>


StepBasic_PersonAndOrganization::StepBasic_PersonAndOrganization ()  {}

void StepBasic_PersonAndOrganization::Init(
	const Handle(StepBasic_Person)& aThePerson,
	const Handle(StepBasic_Organization)& aTheOrganization)
{
	// --- classe own fields ---
	thePerson = aThePerson;
	theOrganization = aTheOrganization;
}


void StepBasic_PersonAndOrganization::SetThePerson(const Handle(StepBasic_Person)& aThePerson)
{
	thePerson = aThePerson;
}

Handle(StepBasic_Person) StepBasic_PersonAndOrganization::ThePerson() const
{
	return thePerson;
}

void StepBasic_PersonAndOrganization::SetTheOrganization(const Handle(StepBasic_Organization)& aTheOrganization)
{
	theOrganization = aTheOrganization;
}

Handle(StepBasic_Organization) StepBasic_PersonAndOrganization::TheOrganization() const
{
	return theOrganization;
}
