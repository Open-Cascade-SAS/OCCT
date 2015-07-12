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
#include <RWStepShape_RWPlusMinusTolerance.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_DimensionalCharacteristic.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_ToleranceMethodDefinition.hxx>

RWStepShape_RWPlusMinusTolerance::RWStepShape_RWPlusMinusTolerance () {}

void RWStepShape_RWPlusMinusTolerance::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_PlusMinusTolerance)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"plus_minus_tolerance")) return;

	// --- own field : range ---

	StepShape_ToleranceMethodDefinition aRange;
	data->ReadEntity (num,1,"range",ach,aRange);

	// --- own field : tolerance_dimension ---

	StepShape_DimensionalCharacteristic aTD;
	data->ReadEntity (num,2,"2dtolerance_dimensionrange",ach,aTD);

	//--- Initialisation of the read entity ---

	ent->Init(aRange,aTD);
}


void RWStepShape_RWPlusMinusTolerance::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_PlusMinusTolerance)& ent) const
{
  SW.Send (ent->Range().Value());
  SW.Send (ent->TolerancedDimension().Value());
}


void RWStepShape_RWPlusMinusTolerance::Share(const Handle(StepShape_PlusMinusTolerance)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->Range().Value());
  iter.AddItem (ent->TolerancedDimension().Value());
}

