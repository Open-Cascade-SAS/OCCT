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
#include <RWStepVisual_RWColourRgb.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_ColourRgb.hxx>

RWStepVisual_RWColourRgb::RWStepVisual_RWColourRgb () {}

void RWStepVisual_RWColourRgb::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_ColourRgb)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"colour_rgb")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : red ---

	Standard_Real aRed;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"red",ach,aRed);

	// --- own field : green ---

	Standard_Real aGreen;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"green",ach,aGreen);

	// --- own field : blue ---

	Standard_Real aBlue;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"blue",ach,aBlue);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aRed, aGreen, aBlue);
}


void RWStepVisual_RWColourRgb::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_ColourRgb)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : red ---

	SW.Send(ent->Red());

	// --- own field : green ---

	SW.Send(ent->Green());

	// --- own field : blue ---

	SW.Send(ent->Blue());
}
