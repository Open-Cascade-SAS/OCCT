#include <StepAP214_AutoDesignDateAndPersonAssignment.ixx>


StepAP214_AutoDesignDateAndPersonAssignment::StepAP214_AutoDesignDateAndPersonAssignment ()  {}

void StepAP214_AutoDesignDateAndPersonAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole)
{

	StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}

void StepAP214_AutoDesignDateAndPersonAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignDateAndPersonItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}


void StepAP214_AutoDesignDateAndPersonAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignDateAndPersonItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDateAndPersonItem) StepAP214_AutoDesignDateAndPersonAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignDateAndPersonItem StepAP214_AutoDesignDateAndPersonAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignDateAndPersonAssignment::NbItems () const
{
	return items->Length();
}
