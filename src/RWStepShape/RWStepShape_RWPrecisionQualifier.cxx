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
#include <RWStepShape_RWPrecisionQualifier.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_PrecisionQualifier.hxx>

RWStepShape_RWPrecisionQualifier::RWStepShape_RWPrecisionQualifier () {}

void RWStepShape_RWPrecisionQualifier::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_PrecisionQualifier)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"precision_qualifier")) return;

	// --- own field : precision_value ---

	Standard_Integer PV;
	data->ReadInteger (num,1,"precision_value",ach,PV);

	//--- Initialisation of the read entity ---

	ent->Init(PV);
}


void RWStepShape_RWPrecisionQualifier::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_PrecisionQualifier)& ent) const
{
  SW.Send (ent->PrecisionValue());
}
