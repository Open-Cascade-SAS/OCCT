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


#include <StepBasic_PersonOrganizationSelect.ixx>
#include <Interface_Macros.hxx>

StepBasic_PersonOrganizationSelect::StepBasic_PersonOrganizationSelect () {  }

Standard_Integer StepBasic_PersonOrganizationSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Person))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Organization))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_PersonAndOrganization))) return 3;
	return 0;
}

Handle(StepBasic_Person) StepBasic_PersonOrganizationSelect::Person () const
{
	return GetCasted(StepBasic_Person,Value());
}

Handle(StepBasic_Organization) StepBasic_PersonOrganizationSelect::Organization () const
{
	return GetCasted(StepBasic_Organization,Value());
}

Handle(StepBasic_PersonAndOrganization) StepBasic_PersonOrganizationSelect::PersonAndOrganization () const
{
	return GetCasted(StepBasic_PersonAndOrganization,Value());
}
