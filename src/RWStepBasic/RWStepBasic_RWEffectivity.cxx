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
#include <RWStepBasic_RWEffectivity.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepBasic_RWEffectivity::RWStepBasic_RWEffectivity () {}

void RWStepBasic_RWEffectivity::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Effectivity)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"effectivity")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	//--- Initialisation of the read entity ---


	ent->Init(aId);
}


void RWStepBasic_RWEffectivity::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Effectivity)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());
}


void RWStepBasic_RWEffectivity::Share(const Handle(StepBasic_Effectivity)&, Interface_EntityIterator&) const
{
}

