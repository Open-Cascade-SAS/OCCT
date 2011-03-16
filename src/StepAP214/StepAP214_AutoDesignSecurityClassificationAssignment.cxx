#include <StepAP214_AutoDesignSecurityClassificationAssignment.ixx>


StepAP214_AutoDesignSecurityClassificationAssignment::StepAP214_AutoDesignSecurityClassificationAssignment ()  {}

void StepAP214_AutoDesignSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification)
{

	StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}

void StepAP214_AutoDesignSecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification,
	const Handle(StepBasic_HArray1OfApproval)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_SecurityClassificationAssignment::Init(aAssignedSecurityClassification);
}


void StepAP214_AutoDesignSecurityClassificationAssignment::SetItems(const Handle(StepBasic_HArray1OfApproval)& aItems)
{
	items = aItems;
}

Handle(StepBasic_HArray1OfApproval) StepAP214_AutoDesignSecurityClassificationAssignment::Items() const
{
	return items;
}

Handle(StepBasic_Approval) StepAP214_AutoDesignSecurityClassificationAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignSecurityClassificationAssignment::NbItems () const
{
	if (items.IsNull()) return 0;
	return items->Length();
}
