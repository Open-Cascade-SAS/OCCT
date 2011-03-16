#include <StepAP214_AutoDesignNominalDateAndTimeAssignment.ixx>


StepAP214_AutoDesignNominalDateAndTimeAssignment::StepAP214_AutoDesignNominalDateAndTimeAssignment ()  {}

void StepAP214_AutoDesignNominalDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole)
{

	StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}

void StepAP214_AutoDesignNominalDateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole,
	const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_DateAndTimeAssignment::Init(aAssignedDateAndTime, aRole);
}


void StepAP214_AutoDesignNominalDateAndTimeAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignDateAndTimeItem) StepAP214_AutoDesignNominalDateAndTimeAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignDateAndTimeItem StepAP214_AutoDesignNominalDateAndTimeAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignNominalDateAndTimeAssignment::NbItems () const
{
	return items->Length();
}
