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

#include <RWStepShape_RWPlusMinusTolerance.ixx>
#include <StepShape_ToleranceMethodDefinition.hxx>
#include <StepShape_DimensionalCharacteristic.hxx>


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

