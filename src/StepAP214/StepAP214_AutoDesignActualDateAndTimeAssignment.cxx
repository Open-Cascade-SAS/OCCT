#include <StepAP214_AutoDesignActualDateAndTimeAssignment.ixx>


StepAP214_AutoDesignActualDateAndTimeAssignment::StepAP214_AutoDesignActualDateAndTimeAssignment ()  {}

void StepAP214_AutoDesignActualDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole)
{

	StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}

void StepAP214_AutoDesignActualDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}


void StepAP214_AutoDesignActualDateAndTimeAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem) StepAP214_AutoDesignActualDateAndTimeAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignDateAndTimeItem StepAP214_AutoDesignActualDateAndTimeAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignActualDateAndTimeAssignment::NbItems () const
{
	return items->Length();
}
