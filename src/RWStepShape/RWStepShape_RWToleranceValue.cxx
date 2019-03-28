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
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>

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

	Handle(Standard_Transient) LB;
	if(!data->ReadEntity (num,1,"lower_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),LB))
  {
    Handle(StepRepr_MeasureRepresentationItem) aMSR;
    Handle(StepRepr_ReprItemAndMeasureWithUnit) aRIMU;
     
    if(data->ReadEntity (num,1,"lower_bound",ach,
			  STANDARD_TYPE(StepRepr_MeasureRepresentationItem),aMSR) || 
      data->ReadEntity (num,1,"lower_bound",ach,STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit), aRIMU))
    {
      if(!aMSR.IsNull())
        LB = aMSR;
       else if(!aRIMU.IsNull())
        LB = aRIMU;
    }
  }

	// --- own field : upper_bound ---

	Handle(Standard_Transient) UB;
	if(!data->ReadEntity (num,2,"upper_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),UB))
  {
     Handle(StepRepr_MeasureRepresentationItem) aMSR1;
     Handle(StepRepr_ReprItemAndMeasureWithUnit) aRIMU1;
    if(data->ReadEntity (num,2,"upper_bound",ach,STANDARD_TYPE(StepRepr_MeasureRepresentationItem),aMSR1) || 
      data->ReadEntity (num,2,"upper_bound",ach,STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit), aRIMU1))
    {
      if(!aMSR1.IsNull())
        UB = aMSR1;
      else if(!aRIMU1.IsNull())
        UB = aRIMU1;
    }
  }

	//--- Initialisation of the read entity ---
  if( !LB.IsNull() && !UB.IsNull())
    ach->ClearFails();
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

