// Created on: 2015-07-22
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_ValueQualifier.hxx>

//=======================================================================
//function : RWSteprepr_RWReprItemAndLengthMeasureWithUnitAndQRI
//purpose  : 
//=======================================================================
RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num0,
   Handle(Interface_Check)& ach,
   const Handle(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI)& ent) const
{
  Standard_Integer num = 0;//num0;
  data->NamedForComplex("MEASURE_WITH_UNIT","MSWTUN",num0,num,ach);
  if (!data->CheckNbParams(num,2,ach,"measure_with_unit")) return;
  // --- own field : valueComponent ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,1, "value_component", ach, mvc);
  // --- own field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2,"unit_component", ach, aUnitComponent);
  Handle(StepBasic_MeasureWithUnit) aMeasureWithUnit = new StepBasic_MeasureWithUnit;
  aMeasureWithUnit->Init(mvc, aUnitComponent);
  
  data->NamedForComplex("QUALIFIED_REPRESENTATION_ITEM","QLRPIT",num0,num,ach);
  if (!data->CheckNbParams(num,1,ach,"qualified_representation_item")) return;
  // --- own field : qualifiers ---
  Handle(StepShape_HArray1OfValueQualifier) quals;
  Standard_Integer nsub;
  if (data->ReadSubList (num, 1, "qualifiers", ach, nsub)) {
    Standard_Integer nb = data->NbParams(nsub);
    quals = new StepShape_HArray1OfValueQualifier (1, nb);
    for (Standard_Integer i = 1; i <= nb; i++) {
      StepShape_ValueQualifier aVQ;
      if (data->ReadEntity (nsub, i, "qualifier" ,ach, aVQ))
        quals->SetValue (i, aVQ);
    }
  }

  Handle(StepShape_QualifiedRepresentationItem) aQRI = new StepShape_QualifiedRepresentationItem();
  aQRI->SetQualifiers(quals);

  data->NamedForComplex("REPRESENTATION_ITEM","RPRITM",num0,num,ach);
  if (!data->CheckNbParams(num,1,ach,"representation_item")) return;
  // --- own field : name ---
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);
  Handle(StepRepr_RepresentationItem) aReprItem = new StepRepr_RepresentationItem;
  aReprItem->Init(aName);

  //--- Initialisation of the read entity ---
  ent->Init(aMeasureWithUnit,aReprItem, aQRI);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI)& ent) const
{
  SW.StartEntity("LENGTH_MEASURE_WITH_UNIT");
  SW.StartEntity("MEASURE_REPRESENTATION_ITEM");
  SW.StartEntity("MEASURE_WITH_UNIT");
  SW.Send(ent->GetMeasureWithUnit()->ValueComponentMember());
  SW.Send(ent->GetMeasureWithUnit()->UnitComponent().Value());
  SW.StartEntity ("QUALIFIED_REPRESENTATION_ITEM");
  Handle(StepShape_QualifiedRepresentationItem) aQRI = ent->GetQualifiedRepresentationItem();
  Standard_Integer i, nbq = aQRI->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i ++) 
    SW.Send (aQRI->QualifiersValue(i).Value());
  SW.CloseSub();
  SW.StartEntity("REPRESENTATION_ITEM");
  SW.Send(ent->Name());
}
