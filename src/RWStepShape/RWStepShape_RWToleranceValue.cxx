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
#include <RWStepShape_RWToleranceValue.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_ToleranceValue.hxx>

RWStepShape_RWToleranceValue::RWStepShape_RWToleranceValue () {}

void RWStepShape_RWToleranceValue::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ToleranceValue)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"tolerance_value")) return;

	// --- own field : lower_bound ---

	Handle(StepBasic_MeasureWithUnit) LB;
	data->ReadEntity (num,1,"lower_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),LB);

	// --- own field : upper_bound ---

	Handle(StepBasic_MeasureWithUnit) UB;
	data->ReadEntity (num,2,"upper_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),UB);

	//--- Initialisation of the read entity ---

	ent->Init(LB,UB);
}


void RWStepShape_RWToleranceValue::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ToleranceValue)& ent) const
{
  SW.Send (ent->LowerBound());
  SW.Send (ent->UpperBound());
}


void RWStepShape_RWToleranceValue::Share(const Handle(StepShape_ToleranceValue)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->LowerBound());
  iter.AddItem (ent->UpperBound());
}

