
#include <StepBasic_PersonOrganizationSelect.ixx>
#include <Interface_Macros.hxx>

StepBasic_PersonOrganizationSelect::StepBasic_PersonOrganizationSelect () {  }

Standard_Integer StepBasic_PersonOrganizationSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Person))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Organization))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_PersonAndOrganization))) return 3;
	return 0;
}

Handle(StepBasic_Person) StepBasic_PersonOrganizationSelect::Person () const
{
	return GetCasted(StepBasic_Person,Value());
}

Handle(StepBasic_Organization) StepBasic_PersonOrganizationSelect::Organization () const
{
	return GetCasted(StepBasic_Organization,Value());
}

Handle(StepBasic_PersonAndOrganization) StepBasic_PersonOrganizationSelect::PersonAndOrganization () const
{
	return GetCasted(StepBasic_PersonAndOrganization,Value());
}
