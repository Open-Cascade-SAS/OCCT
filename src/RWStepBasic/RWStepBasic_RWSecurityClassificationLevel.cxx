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
#include <RWStepBasic_RWSecurityClassificationLevel.hxx>
#include <StepBasic_SecurityClassificationLevel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWSecurityClassificationLevel::RWStepBasic_RWSecurityClassificationLevel () {}

void RWStepBasic_RWSecurityClassificationLevel::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_SecurityClassificationLevel)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"security_classification_level")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepBasic_RWSecurityClassificationLevel::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_SecurityClassificationLevel)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
