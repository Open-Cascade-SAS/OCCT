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
#include "RWStepShape_RWAdvancedBrepShapeRepresentation.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>

RWStepShape_RWAdvancedBrepShapeRepresentation::RWStepShape_RWAdvancedBrepShapeRepresentation() {}

void RWStepShape_RWAdvancedBrepShapeRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&                   data,
  const int                                                     num,
  occ::handle<Interface_Check>&                                 ach,
  const occ::handle<StepShape_AdvancedBrepShapeRepresentation>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "advanced_brep_shape_representation"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : items ---

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aItems;
  occ::handle<StepRepr_RepresentationItem>                                   anent2;
  int                                                                        nsub2;
  if (data->ReadSubList(num, 2, "items", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aItems  = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2,
                           i2,
                           "representation_item",
                           ach,
                           STANDARD_TYPE(StepRepr_RepresentationItem),
                           anent2))
        aItems->SetValue(i2, anent2);
    }
  }

  // --- inherited field : contextOfItems ---

  occ::handle<StepRepr_RepresentationContext> aContextOfItems;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "context_of_items",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationContext),
                   aContextOfItems);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aItems, aContextOfItems);
}

void RWStepShape_RWAdvancedBrepShapeRepresentation::WriteStep(
  StepData_StepWriter&                                          SW,
  const occ::handle<StepShape_AdvancedBrepShapeRepresentation>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field items ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbItems(); i2++)
  {
    SW.Send(ent->ItemsValue(i2));
  }
  SW.CloseSub();

  // --- inherited field contextOfItems ---

  SW.Send(ent->ContextOfItems());
}

void RWStepShape_RWAdvancedBrepShapeRepresentation::Share(
  const occ::handle<StepShape_AdvancedBrepShapeRepresentation>& ent,
  Interface_EntityIterator&                                     iter) const
{

  int nbElem1 = ent->NbItems();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ItemsValue(is1));
  }

  iter.GetOneItem(ent->ContextOfItems());
}
