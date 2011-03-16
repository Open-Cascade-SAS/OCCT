#include <StepBasic_ApprovalRelationship.ixx>


StepBasic_ApprovalRelationship::StepBasic_ApprovalRelationship ()  {}

void StepBasic_ApprovalRelationship::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_Approval)& aRelatingApproval,
	const Handle(StepBasic_Approval)& aRelatedApproval)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	relatingApproval = aRelatingApproval;
	relatedApproval = aRelatedApproval;
}


void StepBasic_ApprovalRelationship::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_ApprovalRelationship::Name() const
{
	return name;
}

void StepBasic_ApprovalRelationship::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_ApprovalRelationship::Description() const
{
	return description;
}

void StepBasic_ApprovalRelationship::SetRelatingApproval(const Handle(StepBasic_Approval)& aRelatingApproval)
{
	relatingApproval = aRelatingApproval;
}

Handle(StepBasic_Approval) StepBasic_ApprovalRelationship::RelatingApproval() const
{
	return relatingApproval;
}

void StepBasic_ApprovalRelationship::SetRelatedApproval(const Handle(StepBasic_Approval)& aRelatedApproval)
{
	relatedApproval = aRelatedApproval;
}

Handle(StepBasic_Approval) StepBasic_ApprovalRelationship::RelatedApproval() const
{
	return relatedApproval;
}
