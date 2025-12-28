// Created on: 2016-08-25
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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
#include "RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel.hxx>

//=================================================================================================

RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel::
  RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel()
= default;

//=================================================================================================

void RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                        data,
  const int                                                                          num0,
  occ::handle<Interface_Check>&                                                      ach,
  const occ::handle<StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel>& ent) const
{
  int num = 0;
  data->NamedForComplex("REPRESENTATION", "RPRSNT", num0, num, ach);
  if (!data->CheckNbParams(num, 3, ach, "shape_aspect"))
    return;

  // name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  // items
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems;
  occ::handle<StepRepr_RepresentationItem>                                   anItem;
  int                                                                        nsub;
  if (data->ReadSubList(num, 2, "items", ach, nsub))
  {
    int nb  = data->NbParams(nsub);
    anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      if (data->ReadEntity(nsub,
                           i,
                           "representation_item",
                           ach,
                           STANDARD_TYPE(StepRepr_RepresentationItem),
                           anItem))
        anItems->SetValue(i, anItem);
    }
  }

  // context_of_items
  occ::handle<StepRepr_RepresentationContext> aContextOfItems;
  data->ReadEntity(num,
                   3,
                   "context_of_items",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationContext),
                   aContextOfItems);

  // Initialize the entity
  ent->Init(aName, anItems, aContextOfItems);
}

//=================================================================================================

void RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel::WriteStep(
  StepData_StepWriter&                                                               SW,
  const occ::handle<StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel>& ent) const
{
  SW.StartEntity("CHARACTERIZED_OBJECT");
  SW.SendDerived();
  SW.SendDerived();
  SW.StartEntity("CHARACTERIZED_REPRESENTATION");
  SW.StartEntity("DRAUGHTING_MODEL");
  SW.StartEntity("REPRESENTATION");
  SW.Send(ent->Name());
  SW.OpenSub();
  for (int i = 1; i <= ent->NbItems(); i++)
  {
    SW.Send(ent->ItemsValue(i));
  }
  SW.CloseSub();
  SW.Send(ent->ContextOfItems());
}

//=================================================================================================

void RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel::Share(
  const occ::handle<StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel>& ent,
  Interface_EntityIterator&                                                          iter) const
{
  int nbElem = ent->NbItems();
  for (int i = 1; i <= nbElem; i++)
  {
    iter.GetOneItem(ent->ItemsValue(i));
  }
  iter.GetOneItem(ent->ContextOfItems());
}
