#include <StepAP214_AutoDesignOrganizationAssignment.ixx>


StepAP214_AutoDesignOrganizationAssignment::StepAP214_AutoDesignOrganizationAssignment ()  {}

void StepAP214_AutoDesignOrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole)
{

	StepBasic_OrganizationAssignment::Init(aAssignedOrganization, aRole);
}

void StepAP214_AutoDesignOrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_OrganizationAssignment::Init(aAssignedOrganization, aRole);
}


void StepAP214_AutoDesignOrganizationAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem) StepAP214_AutoDesignOrganizationAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignOrganizationAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignOrganizationAssignment::NbItems () const
{
	return items->Length();
}
