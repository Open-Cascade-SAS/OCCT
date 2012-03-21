// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <RWStepShape_RWToleranceValue.ixx>
#include <StepBasic_MeasureWithUnit.hxx>


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

