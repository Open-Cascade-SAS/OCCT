// File:	RWStepRepr_RWMeasureRepresentationItem.cxx
// Created:	Wed Sep  8 11:41:37 1999
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <RWStepRepr_RWMeasureRepresentationItem.ixx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>

#include <TCollection_AsciiString.hxx>
#include <string.h>

#include <Interface_EntityIterator.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_MSG.hxx>

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

