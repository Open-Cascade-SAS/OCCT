#include <StepAP214_AppliedPersonAndOrganizationAssignment.ixx>


StepAP214_AppliedPersonAndOrganizationAssignment::StepAP214_AppliedPersonAndOrganizationAssignment ()  {}

void StepAP214_AppliedPersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole)
{

  StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
  }

void StepAP214_AppliedPersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole,
	const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_PersonAndOrganizationAssignment::Init(aAssignedPersonAndOrganization, aRole);
}


void StepAP214_AppliedPersonAndOrganizationAssignment::SetItems(const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfPersonAndOrganizationItem) StepAP214_AppliedPersonAndOrganizationAssignment::Items() const
{
  return items;
}

StepAP214_PersonAndOrganizationItem StepAP214_AppliedPersonAndOrganizationAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedPersonAndOrganizationAssignment::NbItems () const
{
  return items->Length();
}
