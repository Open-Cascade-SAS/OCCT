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

#include <StepBasic_Approval.ixx>


StepBasic_Approval::StepBasic_Approval ()  {}

void StepBasic_Approval::Init(
	const Handle(StepBasic_ApprovalStatus)& aStatus,
	const Handle(TCollection_HAsciiString)& aLevel)
{
	// --- classe own fields ---
	status = aStatus;
	level = aLevel;
}


void StepBasic_Approval::SetStatus(const Handle(StepBasic_ApprovalStatus)& aStatus)
{
	status = aStatus;
}

Handle(StepBasic_ApprovalStatus) StepBasic_Approval::Status() const
{
	return status;
}

void StepBasic_Approval::SetLevel(const Handle(TCollection_HAsciiString)& aLevel)
{
	level = aLevel;
}

Handle(TCollection_HAsciiString) StepBasic_Approval::Level() const
{
	return level;
}
