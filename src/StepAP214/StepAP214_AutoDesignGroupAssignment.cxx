#include <StepAP214_AutoDesignGroupAssignment.ixx>


StepAP214_AutoDesignGroupAssignment::StepAP214_AutoDesignGroupAssignment ()  {}

void StepAP214_AutoDesignGroupAssignment::Init(
	const Handle(StepBasic_Group)& aAssignedGroup)
{

	StepBasic_GroupAssignment::Init(aAssignedGroup);
}

void StepAP214_AutoDesignGroupAssignment::Init(
	const Handle(StepBasic_Group)& aAssignedGroup,
	const Handle(StepAP214_HArray1OfAutoDesignGroupedItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_GroupAssignment::Init(aAssignedGroup);
}


void StepAP214_AutoDesignGroupAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignGroupedItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignGroupedItem) StepAP214_AutoDesignGroupAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignGroupedItem StepAP214_AutoDesignGroupAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignGroupAssignment::NbItems () const
{
	return items->Length();
}
