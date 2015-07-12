// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonOrganizationSelect.hxx>

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
