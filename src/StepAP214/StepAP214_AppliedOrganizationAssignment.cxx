#include <StepAP214_AppliedOrganizationAssignment.ixx>


StepAP214_AppliedOrganizationAssignment::StepAP214_AppliedOrganizationAssignment ()  {}

void StepAP214_AppliedOrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole)
{

	StepBasic_OrganizationAssignment::Init(aAssignedOrganization, aRole);
}

void StepAP214_AppliedOrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfOrganizationItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_OrganizationAssignment::Init(aAssignedOrganization, aRole);
}


void StepAP214_AppliedOrganizationAssignment::SetItems(const Handle(StepAP214_HArray1OfOrganizationItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfOrganizationItem) StepAP214_AppliedOrganizationAssignment::Items() const
{
	return items;
}

StepAP214_OrganizationItem StepAP214_AppliedOrganizationAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AppliedOrganizationAssignment::NbItems () const
{
	return items->Length();
}
