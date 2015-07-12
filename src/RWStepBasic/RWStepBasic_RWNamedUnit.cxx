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
#include <RWStepBasic_RWNamedUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWNamedUnit::RWStepBasic_RWNamedUnit () {}

void RWStepBasic_RWNamedUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_NamedUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"named_unit")) return;

	// --- own field : dimensions ---

	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	//--- Initialisation of the read entity ---


	ent->Init(aDimensions);
}


void RWStepBasic_RWNamedUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_NamedUnit)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Dimensions());
}


void RWStepBasic_RWNamedUnit::Share(const Handle(StepBasic_NamedUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());
}

