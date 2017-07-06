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


#include <Standard_Type.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_OrganizationAssignment.hxx>
#include <StepBasic_OrganizationRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_OrganizationAssignment,Standard_Transient)

void StepBasic_OrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole)
{
	// --- classe own fields ---
	assignedOrganization = aAssignedOrganization;
	role = aRole;
}


void StepBasic_OrganizationAssignment::SetAssignedOrganization(const Handle(StepBasic_Organization)& aAssignedOrganization)
{
	assignedOrganization = aAssignedOrganization;
}

Handle(StepBasic_Organization) StepBasic_OrganizationAssignment::AssignedOrganization() const
{
	return assignedOrganization;
}

void StepBasic_OrganizationAssignment::SetRole(const Handle(StepBasic_OrganizationRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_OrganizationRole) StepBasic_OrganizationAssignment::Role() const
{
	return role;
}
