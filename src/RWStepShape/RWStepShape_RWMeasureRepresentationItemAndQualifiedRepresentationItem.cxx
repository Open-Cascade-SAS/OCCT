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
#include <RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>
#include <StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <StepShape_ValueQualifier.hxx>

RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem () {}

void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent) const
{

//  Complex Entity : MeasureReprItem + QualifiedreprItem : so, add ReprItem

  //  --- Instance of plex componant : MeasureReprItem

  Standard_Integer num = 0;
  data->NamedForComplex("MEASURE_REPRESENTATION_ITEM","MSRPIT",num0,num,ach);

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,2,ach,"measure_representation_item")) return;

  // --- inherited from measure_with_unit : value_component ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num, 1, "value_component", ach, mvc);

  // --- inherited from measure_with_unit : unit_component ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity (num, 2, "unit_component", ach, aUnitComponent);


  //  --- Instance of plex componant : QualifiedReprItem

  data->NamedForComplex("QUALIFIED_REPRESENTATION_ITEM","QLRPIT",num0,num,ach);

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,1,ach,"qualified_representation_item")) return;

  // --- own field : qualifiers ---

  Handle(StepShape_HArray1OfValueQualifier) quals;
  Standard_Integer nsub1;
  if (data->ReadSubList (num,1,"qualifiers",ach,nsub1)) {
    Standard_Integer nb1 = data->NbParams(nsub1);
    quals = new StepShape_HArray1OfValueQualifier (1,nb1);
    for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
      StepShape_ValueQualifier VQ;
      if (data->ReadEntity (nsub1,i1,"qualifier",ach,VQ))
	quals->SetValue (i1,VQ);
    }
  }


  //  --- Instance of plex componant : RepresentationItem

  data->NamedForComplex("REPRESENTATION_ITEM","RPRITM",num0,num,ach);

  if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

  // --- inherited field from this component : name ---

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);

  //--- Initialisation of the read entity ---

  ent->Init(aName, mvc,aUnitComponent,quals);
}


void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent) const
{
//  Complex Entity : MeasureReprItem + QualifiedreprItem : so, add ReprItem

  //  --- Instance of plex componant : MeasureReprItem

  SW.StartEntity ("MEASURE_REPRESENTATION_ITEM");

  // --- inherited from measure_with_unit : value_component ---
  SW.Send(ent->Measure()->ValueComponentMember());
  
  // --- inherited from measure_with_unit : unit_component ---
  SW.Send(ent->Measure()->UnitComponent().Value());

  //  --- Instance of plex componant : QualifiedReprItem

  SW.StartEntity ("QUALIFIED_REPRESENTATION_ITEM");

  // --- own field : qualifiers ---
  Standard_Integer i, nbq = ent->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i ++) SW.Send (ent->QualifiersValue(i).Value());
  SW.CloseSub();

  //  --- Instance of plex componant : ReprsentationItem

  SW.StartEntity ("REPRESENTATION_ITEM");

  // --- inherited field name ---

  SW.Send(ent->Name());
}


void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::Share(const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->Measure()->UnitComponent().Value());

  Standard_Integer i, nbq = ent->NbQualifiers();
  for (i = 1; i <= nbq; i ++) iter.AddItem (ent->QualifiersValue(i).Value());
}

