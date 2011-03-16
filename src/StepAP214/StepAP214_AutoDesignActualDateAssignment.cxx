#include <StepAP214_AutoDesignActualDateAssignment.ixx>


StepAP214_AutoDesignActualDateAssignment::StepAP214_AutoDesignActualDateAssignment ()  {}

void StepAP214_AutoDesignActualDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole)
{

	StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}

void StepAP214_AutoDesignActualDateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignDatedItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}


void StepAP214_AutoDesignActualDateAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignDatedItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDatedItem) StepAP214_AutoDesignActualDateAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignDatedItem StepAP214_AutoDesignActualDateAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignActualDateAssignment::NbItems () const
{
	return items->Length();
}
