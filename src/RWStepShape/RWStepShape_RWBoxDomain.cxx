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
#include <RWStepShape_RWBoxDomain.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepShape_BoxDomain.hxx>

RWStepShape_RWBoxDomain::RWStepShape_RWBoxDomain () {}

void RWStepShape_RWBoxDomain::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_BoxDomain)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"box_domain")) return;

	// --- own field : corner ---

	Handle(StepGeom_CartesianPoint) aCorner;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"corner", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aCorner);

	// --- own field : xlength ---

	Standard_Real aXlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"xlength",ach,aXlength);

	// --- own field : ylength ---

	Standard_Real aYlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"ylength",ach,aYlength);

	// --- own field : zlength ---

	Standard_Real aZlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"zlength",ach,aZlength);

	//--- Initialisation of the read entity ---


	ent->Init(aCorner, aXlength, aYlength, aZlength);
}


void RWStepShape_RWBoxDomain::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_BoxDomain)& ent) const
{

	// --- own field : corner ---

	SW.Send(ent->Corner());

	// --- own field : xlength ---

	SW.Send(ent->Xlength());

	// --- own field : ylength ---

	SW.Send(ent->Ylength());

	// --- own field : zlength ---

	SW.Send(ent->Zlength());
}


void RWStepShape_RWBoxDomain::Share(const Handle(StepShape_BoxDomain)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Corner());
}

