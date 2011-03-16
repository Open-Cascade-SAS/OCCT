#include <StepAP214_AutoDesignApprovalAssignment.ixx>


StepAP214_AutoDesignApprovalAssignment::StepAP214_AutoDesignApprovalAssignment ()  {}

void StepAP214_AutoDesignApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval)
{

	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}

void StepAP214_AutoDesignApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval,
	const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}


void StepAP214_AutoDesignApprovalAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem) StepAP214_AutoDesignApprovalAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignApprovalAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignApprovalAssignment::NbItems () const
{
	return items->Length();
}
