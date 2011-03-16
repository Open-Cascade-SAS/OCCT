#include <StepAP214_AppliedDateAndTimeAssignment.ixx>


StepAP214_AppliedDateAndTimeAssignment::StepAP214_AppliedDateAndTimeAssignment ()  {}

void StepAP214_AppliedDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole)
{

  StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}

void StepAP214_AppliedDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole,
	const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}


void StepAP214_AppliedDateAndTimeAssignment::SetItems(const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfDateAndTimeItem) StepAP214_AppliedDateAndTimeAssignment::Items() const
{
  return items;
}

StepAP214_DateAndTimeItem StepAP214_AppliedDateAndTimeAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedDateAndTimeAssignment::NbItems () const
{
  return items->Length();
}
