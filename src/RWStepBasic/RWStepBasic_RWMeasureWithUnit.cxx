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
#include <RWStepBasic_RWMeasureWithUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

