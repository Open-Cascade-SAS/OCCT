#include <StepAP214_AppliedDateAssignment.ixx>


StepAP214_AppliedDateAssignment::StepAP214_AppliedDateAssignment ()  {}

void StepAP214_AppliedDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole)
{

  StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}

void StepAP214_AppliedDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole,
	const Handle(StepAP214_HArray1OfDateItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}


void StepAP214_AppliedDateAssignment::SetItems(const Handle(StepAP214_HArray1OfDateItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfDateItem) StepAP214_AppliedDateAssignment::Items() const
{
  return items;
}

StepAP214_DateItem StepAP214_AppliedDateAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedDateAssignment::NbItems () const
{
  return items->Length();
}
