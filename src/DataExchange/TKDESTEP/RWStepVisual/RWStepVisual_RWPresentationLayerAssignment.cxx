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
#include "RWStepVisual_RWPresentationLayerAssignment.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>

RWStepVisual_RWPresentationLayerAssignment::RWStepVisual_RWPresentationLayerAssignment() = default;

void RWStepVisual_RWPresentationLayerAssignment::ReadStep(
  const occ::handle<StepData_StepReaderData>&                data,
  const int                                                  num,
  occ::handle<Interface_Check>&                              ach,
  const occ::handle<StepVisual_PresentationLayerAssignment>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "presentation_layer_assignment"))
    return;

  // --- own field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadString(num, 2, "description", ach, aDescription);

  // --- own field : assignedItems ---

  occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> aAssignedItems;
  StepVisual_LayeredItem                                   aAssignedItemsItem;
  int                                                      nsub3;
  if (data->ReadSubList(num, 3, "assigned_items", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    if (nb3 > 0)
    {
      aAssignedItems = new NCollection_HArray1<StepVisual_LayeredItem>(1, nb3);
      for (int i3 = 1; i3 <= nb3; i3++)
      {
        // szv#4:S4163:12Mar99 `bool stat3 =` not needed
        if (data->ReadEntity(nsub3, i3, "assigned_items", ach, aAssignedItemsItem))
          aAssignedItems->SetValue(i3, aAssignedItemsItem);
      }
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDescription, aAssignedItems);
}

void RWStepVisual_RWPresentationLayerAssignment::WriteStep(
  StepData_StepWriter&                                       SW,
  const occ::handle<StepVisual_PresentationLayerAssignment>& ent) const
{

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : description ---

  SW.Send(ent->Description());

  // --- own field : assignedItems ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbAssignedItems(); i3++)
  {
    SW.Send(ent->AssignedItemsValue(i3).Value());
  }
  SW.CloseSub();
}

void RWStepVisual_RWPresentationLayerAssignment::Share(
  const occ::handle<StepVisual_PresentationLayerAssignment>& ent,
  Interface_EntityIterator&                                  iter) const
{

  int nbElem1 = ent->NbAssignedItems();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->AssignedItemsValue(is1).Value());
  }
}
