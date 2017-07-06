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
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_RepresentationRelationship,Standard_Transient)

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
