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

#include <StepBasic_Organization.ixx>


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
