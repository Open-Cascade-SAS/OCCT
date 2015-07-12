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
#include <RWStepShape_RWLimitsAndFits.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_LimitsAndFits.hxx>

RWStepShape_RWLimitsAndFits::RWStepShape_RWLimitsAndFits () {}

void RWStepShape_RWLimitsAndFits::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_LimitsAndFits)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"limits_and_fits")) return;

	// --- own field : form_variance ---

	Handle(TCollection_HAsciiString) fv;
	data->ReadString (num,1,"form_variance",ach,fv);

	// --- own field : form_variance ---

	Handle(TCollection_HAsciiString) zv;
	data->ReadString (num,2,"zone_variance",ach,zv);

	// --- own field : grade ---

	Handle(TCollection_HAsciiString) gr;
	data->ReadString (num,3,"grade",ach,gr);

	// --- own field : source ---

	Handle(TCollection_HAsciiString) sou;
	data->ReadString (num,4,"source",ach,sou);

	//--- Initialisation of the read entity ---

	ent->Init(fv,zv,gr,sou);
}


void RWStepShape_RWLimitsAndFits::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_LimitsAndFits)& ent) const
{
	SW.Send(ent->FormVariance());
	SW.Send(ent->ZoneVariance());
	SW.Send(ent->Grade());
	SW.Send(ent->Source());
}

