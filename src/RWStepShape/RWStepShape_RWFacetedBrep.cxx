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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepShape_RWFacetedBrep.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrep.hxx>

RWStepShape_RWFacetedBrep::RWStepShape_RWFacetedBrep () {}

void RWStepShape_RWFacetedBrep::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FacetedBrep)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"faceted_brep")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : outer ---

	Handle(StepShape_ClosedShell) aOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOuter);
}


void RWStepShape_RWFacetedBrep::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FacetedBrep)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field outer ---

	SW.Send(ent->Outer());
}


void RWStepShape_RWFacetedBrep::Share(const Handle(StepShape_FacetedBrep)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Outer());
}

