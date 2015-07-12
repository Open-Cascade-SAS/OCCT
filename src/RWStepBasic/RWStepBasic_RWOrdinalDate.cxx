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
#include <RWStepBasic_RWOrdinalDate.hxx>
#include <StepBasic_OrdinalDate.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWOrdinalDate::RWStepBasic_RWOrdinalDate () {}

void RWStepBasic_RWOrdinalDate::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"ordinal_date")) return;

	// --- inherited field : yearComponent ---

	Standard_Integer aYearComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"year_component",ach,aYearComponent);

	// --- own field : dayComponent ---

	Standard_Integer aDayComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"day_component",ach,aDayComponent);

	//--- Initialisation of the read entity ---


	ent->Init(aYearComponent, aDayComponent);
}


void RWStepBasic_RWOrdinalDate::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{

	// --- inherited field yearComponent ---

	SW.Send(ent->YearComponent());

	// --- own field : dayComponent ---

	SW.Send(ent->DayComponent());
}
