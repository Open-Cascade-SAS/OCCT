#include <StepAP214_AutoDesignPersonAndOrganizationAssignment.ixx>


StepAP214_AutoDesignPersonAndOrganizationAssignment::StepAP214_AutoDesignPersonAndOrganizationAssignment ()  {}

void StepAP214_AutoDesignPersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole)
{

	StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}

void StepAP214_AutoDesignPersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}


void StepAP214_AutoDesignPersonAndOrganizationAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem) StepAP214_AutoDesignPersonAndOrganizationAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignPersonAndOrganizationAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignPersonAndOrganizationAssignment::NbItems () const
{
	return items->Length();
}
