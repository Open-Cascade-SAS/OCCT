// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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
#include <Interface_FloatWriter.hxx>
#include <Interface_MSG.hxx>
#include <RWStepRepr_RWMeasureRepresentationItem.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <TCollection_AsciiString.hxx>

#include <string.h>
//=======================================================================
//function : RWStepRepr_RWMeasureRepresentationItem
//purpose  : 
//=======================================================================
RWStepRepr_RWMeasureRepresentationItem::RWStepRepr_RWMeasureRepresentationItem () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMeasureRepresentationItem::ReadStep (const Handle(StepData_StepReaderData)& data,
						       const Standard_Integer num,
						       Handle(Interface_Check)& ach,
						       const Handle(StepRepr_MeasureRepresentationItem)& ent) const
{
  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,3,ach,"measure_representation_item")) return;

  // --- inherited from representation_item : name ---
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);

  // --- inherited from measure_with_unit : value_component ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,2, "value_component", ach, mvc);

  // --- inherited from measure_with_unit : unit_component ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 3,"unit_component", ach, aUnitComponent);

  //--- Initialisation of the read entity ---

  ent->Init(aName, mvc, aUnitComponent);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMeasureRepresentationItem::WriteStep (StepData_StepWriter& SW,
							const Handle(StepRepr_MeasureRepresentationItem)& ent) const
{
  // --- inherited from representation_item : name ---
  SW.Send(ent->Name());

  // --- inherited from measure_with_unit : value_component ---
  SW.Send(ent->Measure()->ValueComponentMember());
  
  // --- inherited from measure_with_unit : unit_component ---
  SW.Send(ent->Measure()->UnitComponent().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWMeasureRepresentationItem::Share(const Handle(StepRepr_MeasureRepresentationItem)& ent, 
						   Interface_EntityIterator& iter) const
{
  
  iter.GetOneItem(ent->Measure()->UnitComponent().Value());
}

