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
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Organization,Standard_Transient)

StepBasic_Organization::StepBasic_Organization ()  {}

void StepBasic_Organization::Init(
	const Standard_Boolean hasAid,
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	hasId = hasAid;
	id = aId;
	name = aName;
	description = aDescription;
}


void StepBasic_Organization::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
	hasId = Standard_True;
}

void StepBasic_Organization::UnSetId()
{
	hasId = Standard_False;
	id.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Id() const
{
	return id;
}

Standard_Boolean StepBasic_Organization::HasId() const
{
	return hasId;
}

void StepBasic_Organization::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Name() const
{
	return name;
}

void StepBasic_Organization::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Description() const
{
	return description;
}
