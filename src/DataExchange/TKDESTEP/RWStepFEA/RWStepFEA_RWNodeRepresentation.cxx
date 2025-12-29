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
#include "RWStepFEA_RWNodeRepresentation.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepFEA_NodeRepresentation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_RepresentationContext.hxx>

//=================================================================================================

RWStepFEA_RWNodeRepresentation::RWStepFEA_RWNodeRepresentation() = default;

//=================================================================================================

void RWStepFEA_RWNodeRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&    data,
  const int                                      num,
  occ::handle<Interface_Check>&                  ach,
  const occ::handle<StepFEA_NodeRepresentation>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "node_representation"))
    return;

  // Inherited fields of Representation

  occ::handle<TCollection_HAsciiString> aRepresentation_Name;
  data->ReadString(num, 1, "representation.name", ach, aRepresentation_Name);

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aRepresentation_Items;
  int                                                                        sub2 = 0;
  if (data->ReadSubList(num, 2, "representation.items", ach, sub2))
  {
    int nb0 = data->NbParams(sub2);
    aRepresentation_Items =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nb0);
    int num2 = sub2;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<StepRepr_RepresentationItem> anIt0;
      data->ReadEntity(num2,
                       i0,
                       "representation_item",
                       ach,
                       STANDARD_TYPE(StepRepr_RepresentationItem),
                       anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  occ::handle<StepRepr_RepresentationContext> aRepresentation_ContextOfItems;
  data->ReadEntity(num,
                   3,
                   "representation.context_of_items",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationContext),
                   aRepresentation_ContextOfItems);

  // Own fields of NodeRepresentation

  occ::handle<StepFEA_FeaModel> aModelRef;
  data->ReadEntity(num, 4, "model_ref", ach, STANDARD_TYPE(StepFEA_FeaModel), aModelRef);

  // Initialize entity
  ent->Init(aRepresentation_Name, aRepresentation_Items, aRepresentation_ContextOfItems, aModelRef);
}

//=================================================================================================

void RWStepFEA_RWNodeRepresentation::WriteStep(
  StepData_StepWriter&                           SW,
  const occ::handle<StepFEA_NodeRepresentation>& ent) const
{

  // Inherited fields of Representation

  SW.Send(ent->StepRepr_Representation::Name());

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->StepRepr_Representation::NbItems(); i1++)
  {
    occ::handle<StepRepr_RepresentationItem> Var0 =
      ent->StepRepr_Representation::Items()->Value(i1);
    SW.Send(Var0);
  }
  SW.CloseSub();

  SW.Send(ent->StepRepr_Representation::ContextOfItems());

  // Own fields of NodeRepresentation

  SW.Send(ent->ModelRef());
}

//=================================================================================================

void RWStepFEA_RWNodeRepresentation::Share(const occ::handle<StepFEA_NodeRepresentation>& ent,
                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  for (int i1 = 1; i1 <= ent->StepRepr_Representation::NbItems(); i1++)
  {
    occ::handle<StepRepr_RepresentationItem> Var0 =
      ent->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem(Var0);
  }

  iter.AddItem(ent->StepRepr_Representation::ContextOfItems());

  // Own fields of NodeRepresentation

  iter.AddItem(ent->ModelRef());
}
