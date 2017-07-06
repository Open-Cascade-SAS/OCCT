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
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
#include <StepBasic_DateTimeRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DateAndTimeAssignment,Standard_Transient)

void StepBasic_DateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole)
{
	// --- classe own fields ---
	assignedDateAndTime = aAssignedDateAndTime;
	role = aRole;
}


void StepBasic_DateAndTimeAssignment::SetAssignedDateAndTime(const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime)
{
	assignedDateAndTime = aAssignedDateAndTime;
}

Handle(StepBasic_DateAndTime) StepBasic_DateAndTimeAssignment::AssignedDateAndTime() const
{
	return assignedDateAndTime;
}

void StepBasic_DateAndTimeAssignment::SetRole(const Handle(StepBasic_DateTimeRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_DateTimeRole) StepBasic_DateAndTimeAssignment::Role() const
{
	return role;
}
