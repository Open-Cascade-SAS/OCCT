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


#include <RWStepBasic_RWMeasureWithUnit.ixx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_MeasureWithUnit.hxx>


//=======================================================================
//function : RWStepBasic_RWMeasureWithUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWMeasureWithUnit::RWStepBasic_RWMeasureWithUnit () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
					      const Standard_Integer num,
					      Handle(Interface_Check)& ach,
					      const Handle(StepBasic_MeasureWithUnit)& ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,2,ach,"measure_with_unit")) return;

  // --- own field : valueComponent ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,1, "value_component", ach, mvc);

  // --- own field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2,"unit_component", ach, aUnitComponent);

  //--- Initialisation of the read entity ---
  ent->Init(mvc, aUnitComponent);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::WriteStep (StepData_StepWriter& SW,
					       const Handle(StepBasic_MeasureWithUnit)& ent) const
{
  // --- own field : valueComponent ---
  SW.Send(ent->ValueComponentMember());

  // --- own field : unitComponent ---
  SW.Send(ent->UnitComponent().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::Share (const Handle(StepBasic_MeasureWithUnit)& ent, 
					   Interface_EntityIterator& iter) const
{

  iter.AddItem(ent->UnitComponent().Value());
}

