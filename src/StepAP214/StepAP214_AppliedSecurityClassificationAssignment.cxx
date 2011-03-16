#include <StepAP214_AppliedSecurityClassificationAssignment.ixx>


StepAP214_AppliedSecurityClassificationAssignment::StepAP214_AppliedSecurityClassificationAssignment ()  {}

void StepAP214_AppliedSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification)
{

  StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}

void StepAP214_AppliedSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification,
	const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}


void StepAP214_AppliedSecurityClassificationAssignment::SetItems(const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfSecurityClassificationItem) StepAP214_AppliedSecurityClassificationAssignment::Items() const
{
  return items;
}

const StepAP214_SecurityClassificationItem& StepAP214_AppliedSecurityClassificationAssignment::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedSecurityClassificationAssignment::NbItems () const
{
  if (items.IsNull()) return 0;
  return items->Length();
}
