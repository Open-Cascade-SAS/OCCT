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
#include "RWStepVisual_RWInvisibility.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_InvisibleItem.hxx>

RWStepVisual_RWInvisibility::RWStepVisual_RWInvisibility() = default;

void RWStepVisual_RWInvisibility::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                                   num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepVisual_Invisibility>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 1, ach, "invisibility"))
    return;

  // --- own field : invisibleItems ---

  occ::handle<NCollection_HArray1<StepVisual_InvisibleItem>> aInvisibleItems;
  StepVisual_InvisibleItem                                   aInvisibleItemsItem;
  int                                                        nsub1;
  if (data->ReadSubList(num, 1, "invisible_items", ach, nsub1))
  {
    int nb1         = data->NbParams(nsub1);
    aInvisibleItems = new NCollection_HArray1<StepVisual_InvisibleItem>(1, nb1);
    for (int i1 = 1; i1 <= nb1; i1++)
    {
      // szv#4:S4163:12Mar99 `bool stat1 =` not needed
      if (data->ReadEntity(nsub1, i1, "invisible_items", ach, aInvisibleItemsItem))
        aInvisibleItems->SetValue(i1, aInvisibleItemsItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aInvisibleItems);
}

void RWStepVisual_RWInvisibility::WriteStep(StepData_StepWriter&                        SW,
                                            const occ::handle<StepVisual_Invisibility>& ent) const
{

  // --- own field : invisibleItems ---

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->NbInvisibleItems(); i1++)
  {
    SW.Send(ent->InvisibleItemsValue(i1).Value());
  }
  SW.CloseSub();
}

void RWStepVisual_RWInvisibility::Share(const occ::handle<StepVisual_Invisibility>& ent,
                                        Interface_EntityIterator&                   iter) const
{

  int nbElem1 = ent->NbInvisibleItems();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->InvisibleItemsValue(is1).Value());
  }
}
