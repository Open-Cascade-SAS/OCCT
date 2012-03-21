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

#include <StepBasic_PersonAndOrganization.ixx>


StepBasic_PersonAndOrganization::StepBasic_PersonAndOrganization ()  {}

void StepBasic_PersonAndOrganization::Init(
	const Handle(StepBasic_Person)& aThePerson,
	const Handle(StepBasic_Organization)& aTheOrganization)
{
	// --- classe own fields ---
	thePerson = aThePerson;
	theOrganization = aTheOrganization;
}


void StepBasic_PersonAndOrganization::SetThePerson(const Handle(StepBasic_Person)& aThePerson)
{
	thePerson = aThePerson;
}

Handle(StepBasic_Person) StepBasic_PersonAndOrganization::ThePerson() const
{
	return thePerson;
}

void StepBasic_PersonAndOrganization::SetTheOrganization(const Handle(StepBasic_Organization)& aTheOrganization)
{
	theOrganization = aTheOrganization;
}

Handle(StepBasic_Organization) StepBasic_PersonAndOrganization::TheOrganization() const
{
	return theOrganization;
}
