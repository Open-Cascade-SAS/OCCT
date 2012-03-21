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

#include <StepBasic_SecurityClassification.ixx>


StepBasic_SecurityClassification::StepBasic_SecurityClassification ()  {}

void StepBasic_SecurityClassification::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aPurpose,
	const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel)
{
	// --- classe own fields ---
	name = aName;
	purpose = aPurpose;
	securityLevel = aSecurityLevel;
}


void StepBasic_SecurityClassification::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_SecurityClassification::Name() const
{
	return name;
}

void StepBasic_SecurityClassification::SetPurpose(const Handle(TCollection_HAsciiString)& aPurpose)
{
	purpose = aPurpose;
}

Handle(TCollection_HAsciiString) StepBasic_SecurityClassification::Purpose() const
{
	return purpose;
}

void StepBasic_SecurityClassification::SetSecurityLevel(const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel)
{
	securityLevel = aSecurityLevel;
}

Handle(StepBasic_SecurityClassificationLevel) StepBasic_SecurityClassification::SecurityLevel() const
{
	return securityLevel;
}
