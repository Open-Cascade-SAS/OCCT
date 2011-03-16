#include <StepAP214_AutoDesignNominalDateAssignment.ixx>


StepAP214_AutoDesignNominalDateAssignment::StepAP214_AutoDesignNominalDateAssignment ()  {}

void StepAP214_AutoDesignNominalDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole)
{

	StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}

void StepAP214_AutoDesignNominalDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignDatedItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}


void StepAP214_AutoDesignNominalDateAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignDatedItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDatedItem) StepAP214_AutoDesignNominalDateAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignDatedItem StepAP214_AutoDesignNominalDateAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignNominalDateAssignment::NbItems () const
{
	return items->Length();
}
