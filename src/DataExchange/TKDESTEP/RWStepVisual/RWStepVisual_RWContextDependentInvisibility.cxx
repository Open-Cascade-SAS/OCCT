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
#include "RWStepVisual_RWContextDependentInvisibility.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_ContextDependentInvisibility.hxx>
#include <StepVisual_InvisibleItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_InvisibilityContext.hxx>

RWStepVisual_RWContextDependentInvisibility::RWStepVisual_RWContextDependentInvisibility() =
  default;

void RWStepVisual_RWContextDependentInvisibility::ReadStep(
  const occ::handle<StepData_StepReaderData>&                 data,
  const int                                                   num,
  occ::handle<Interface_Check>&                               ach,
  const occ::handle<StepVisual_ContextDependentInvisibility>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "context_dependent_invisibility"))
    return;

  // --- inherited field : invisibleItems ---

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

  // --- own field : presentationContext ---

  StepVisual_InvisibilityContext aPresentationContext;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "presentation_context", ach, aPresentationContext);

  //--- Initialisation of the read entity ---

  ent->Init(aInvisibleItems, aPresentationContext);
}

void RWStepVisual_RWContextDependentInvisibility::WriteStep(
  StepData_StepWriter&                                        SW,
  const occ::handle<StepVisual_ContextDependentInvisibility>& ent) const
{

  // --- inherited field invisibleItems ---

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->NbInvisibleItems(); i1++)
  {
    SW.Send(ent->InvisibleItemsValue(i1).Value());
  }
  SW.CloseSub();

  // --- own field : presentationContext ---

  SW.Send(ent->PresentationContext().Value());
}

void RWStepVisual_RWContextDependentInvisibility::Share(
  const occ::handle<StepVisual_ContextDependentInvisibility>& ent,
  Interface_EntityIterator&                                   iter) const
{

  int nbElem1 = ent->NbInvisibleItems();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->InvisibleItemsValue(is1).Value());
  }

  iter.GetOneItem(ent->PresentationContext().Value());
}
