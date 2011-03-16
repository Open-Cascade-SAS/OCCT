#include <StepAP214_AppliedApprovalAssignment.ixx>


StepAP214_AppliedApprovalAssignment::StepAP214_AppliedApprovalAssignment ()  {}

void StepAP214_AppliedApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval)
{

	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}

void StepAP214_AppliedApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval,
	const Handle(StepAP214_HArray1OfApprovalItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}


void StepAP214_AppliedApprovalAssignment::SetItems(const Handle(StepAP214_HArray1OfApprovalItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfApprovalItem) StepAP214_AppliedApprovalAssignment::Items() const
{
	return items;
}

StepAP214_ApprovalItem StepAP214_AppliedApprovalAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AppliedApprovalAssignment::NbItems () const
{
	return items->Length();
}
