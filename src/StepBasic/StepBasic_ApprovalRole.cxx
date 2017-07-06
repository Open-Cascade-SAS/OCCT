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
#include <StepBasic_ApprovalRole.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApprovalRole,Standard_Transient)

StepBasic_ApprovalRole::StepBasic_ApprovalRole ()  {}

void StepBasic_ApprovalRole::Init(
	const Handle(TCollection_HAsciiString)& aRole)
{
	// --- classe own fields ---
	role = aRole;
}


void StepBasic_ApprovalRole::SetRole(const Handle(TCollection_HAsciiString)& aRole)
{
	role = aRole;
}

Handle(TCollection_HAsciiString) StepBasic_ApprovalRole::Role() const
{
	return role;
}
