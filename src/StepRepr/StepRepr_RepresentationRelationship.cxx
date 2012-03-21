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

#include <StepRepr_RepresentationRelationship.ixx>


StepRepr_RepresentationRelationship::StepRepr_RepresentationRelationship ()  {}

void StepRepr_RepresentationRelationship::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepRepr_Representation)& aRep1,
	const Handle(StepRepr_Representation)& aRep2)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	rep1 = aRep1;
	rep2 = aRep2;
}


void StepRepr_RepresentationRelationship::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationRelationship::Name() const
{
	return name;
}

void StepRepr_RepresentationRelationship::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationRelationship::Description() const
{
	return description;
}

void StepRepr_RepresentationRelationship::SetRep1(const Handle(StepRepr_Representation)& aRep1)
{
	rep1 = aRep1;
}

Handle(StepRepr_Representation) StepRepr_RepresentationRelationship::Rep1() const
{
	return rep1;
}

void StepRepr_RepresentationRelationship::SetRep2(const Handle(StepRepr_Representation)& aRep2)
{
	rep2 = aRep2;
}

Handle(StepRepr_Representation) StepRepr_RepresentationRelationship::Rep2() const
{
	return rep2;
}
