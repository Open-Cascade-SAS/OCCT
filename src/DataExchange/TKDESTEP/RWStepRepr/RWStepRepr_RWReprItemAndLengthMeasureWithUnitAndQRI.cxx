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
#include "RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI.pxx"
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_ValueQualifier.hxx>

//=================================================================================================

RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::
  RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI()
{
}

//=================================================================================================

void RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::ReadStep(
  const occ::handle<StepData_StepReaderData>&                         data,
  const int                                         num0,
  occ::handle<Interface_Check>&                                       ach,
  const occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI>& ent) const
{
  int num = 0; // num0;
  data->NamedForComplex("MEASURE_WITH_UNIT", "MSWTUN", num0, num, ach);
  if (!data->CheckNbParams(num, 2, ach, "measure_with_unit"))
    return;
  // --- own field : valueComponent ---
  occ::handle<StepBasic_MeasureValueMember> mvc = new StepBasic_MeasureValueMember;
  data->ReadMember(num, 1, "value_component", ach, mvc);
  // --- own field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2, "unit_component", ach, aUnitComponent);
  occ::handle<StepBasic_MeasureWithUnit> aMeasureWithUnit = new StepBasic_MeasureWithUnit;
  aMeasureWithUnit->Init(mvc, aUnitComponent);

  data->NamedForComplex("QUALIFIED_REPRESENTATION_ITEM", "QLRPIT", num0, num, ach);
  if (!data->CheckNbParams(num, 1, ach, "qualified_representation_item"))
    return;
  // --- own field : qualifiers ---
  occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> quals;
  int                          nsub;
  if (data->ReadSubList(num, 1, "qualifiers", ach, nsub))
  {
    int nb = data->NbParams(nsub);
    quals               = new NCollection_HArray1<StepShape_ValueQualifier>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      StepShape_ValueQualifier aVQ;
      if (data->ReadEntity(nsub, i, "qualifier", ach, aVQ))
        quals->SetValue(i, aVQ);
    }
  }

  occ::handle<StepShape_QualifiedRepresentationItem> aQRI = new StepShape_QualifiedRepresentationItem();
  aQRI->SetQualifiers(quals);

  data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM", num0, num, ach);
  if (!data->CheckNbParams(num, 1, ach, "representation_item"))
    return;
  // --- own field : name ---
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);
  occ::handle<StepRepr_RepresentationItem> aReprItem = new StepRepr_RepresentationItem;
  aReprItem->Init(aName);

  //--- Initialisation of the read entity ---
  ent->Init(aMeasureWithUnit, aReprItem, aQRI);
}

//=================================================================================================

void RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI::WriteStep(
  StepData_StepWriter&                                           SW,
  const occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI>& ent) const
{
  SW.StartEntity("LENGTH_MEASURE_WITH_UNIT");
  SW.StartEntity("MEASURE_REPRESENTATION_ITEM");
  SW.StartEntity("MEASURE_WITH_UNIT");
  SW.Send(ent->GetMeasureWithUnit()->ValueComponentMember());
  SW.Send(ent->GetMeasureWithUnit()->UnitComponent().Value());
  SW.StartEntity("QUALIFIED_REPRESENTATION_ITEM");
  occ::handle<StepShape_QualifiedRepresentationItem> aQRI = ent->GetQualifiedRepresentationItem();
  int                              i, nbq = aQRI->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i++)
    SW.Send(aQRI->QualifiersValue(i).Value());
  SW.CloseSub();
  SW.StartEntity("REPRESENTATION_ITEM");
  SW.Send(ent->Name());
}
