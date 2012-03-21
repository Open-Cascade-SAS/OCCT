// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
