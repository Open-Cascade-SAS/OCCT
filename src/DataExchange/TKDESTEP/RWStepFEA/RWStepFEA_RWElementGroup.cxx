// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepFEA_RWElementGroup.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_ElementGroup.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepFEA_FeaModel.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepFEA_RWElementGroup::RWStepFEA_RWElementGroup() {}

//=================================================================================================

void RWStepFEA_RWElementGroup::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                        const int                                   num,
                                        occ::handle<Interface_Check>&               ach,
                                        const occ::handle<StepFEA_ElementGroup>&    ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "element_group"))
    return;

  // Inherited fields of Group

  occ::handle<TCollection_HAsciiString> aGroup_Name;
  data->ReadString(num, 1, "group.name", ach, aGroup_Name);

  occ::handle<TCollection_HAsciiString> aGroup_Description;
  data->ReadString(num, 2, "group.description", ach, aGroup_Description);

  // Inherited fields of FeaGroup

  occ::handle<StepFEA_FeaModel> aFeaGroup_ModelRef;
  data->ReadEntity(num,
                   3,
                   "fea_group.model_ref",
                   ach,
                   STANDARD_TYPE(StepFEA_FeaModel),
                   aFeaGroup_ModelRef);

  // Own fields of ElementGroup

  occ::handle<NCollection_HArray1<occ::handle<StepFEA_ElementRepresentation>>> aElements;
  int                                                                          sub4 = 0;
  if (data->ReadSubList(num, 4, "elements", ach, sub4))
  {
    int nb0   = data->NbParams(sub4);
    aElements = new NCollection_HArray1<occ::handle<StepFEA_ElementRepresentation>>(1, nb0);
    int num2  = sub4;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<StepFEA_ElementRepresentation> anIt0;
      data->ReadEntity(num2,
                       i0,
                       "element_representation",
                       ach,
                       STANDARD_TYPE(StepFEA_ElementRepresentation),
                       anIt0);
      aElements->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aGroup_Name, aGroup_Description, aFeaGroup_ModelRef, aElements);
}

//=================================================================================================

void RWStepFEA_RWElementGroup::WriteStep(StepData_StepWriter&                     SW,
                                         const occ::handle<StepFEA_ElementGroup>& ent) const
{

  // Inherited fields of Group

  SW.Send(ent->StepBasic_Group::Name());

  SW.Send(ent->StepBasic_Group::Description());

  // Inherited fields of FeaGroup

  SW.Send(ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of ElementGroup

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->Elements()->Length(); i3++)
  {
    occ::handle<StepFEA_ElementRepresentation> Var0 = ent->Elements()->Value(i3);
    SW.Send(Var0);
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepFEA_RWElementGroup::Share(const occ::handle<StepFEA_ElementGroup>& ent,
                                     Interface_EntityIterator&                iter) const
{

  // Inherited fields of Group

  // Inherited fields of FeaGroup

  iter.AddItem(ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of ElementGroup

  for (int i2 = 1; i2 <= ent->Elements()->Length(); i2++)
  {
    occ::handle<StepFEA_ElementRepresentation> Var0 = ent->Elements()->Value(i2);
    iter.AddItem(Var0);
  }
}
