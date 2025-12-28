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
#include "RWStepAP214_RWAppliedPresentedItem.pxx"
#include <StepAP214_AppliedPresentedItem.hxx>
#include <StepAP214_PresentedItemSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepAP214_RWAppliedPresentedItem::RWStepAP214_RWAppliedPresentedItem() = default;

void RWStepAP214_RWAppliedPresentedItem::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepAP214_AppliedPresentedItem>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 1, ach, "applied_presented_item"))
    return;

  // --- own field : items ---

  occ::handle<NCollection_HArray1<StepAP214_PresentedItemSelect>> aItems;
  StepAP214_PresentedItemSelect                                   anent1;
  int                                                             nsub1;
  if (data->ReadSubList(num, 1, "items", ach, nsub1))
  {
    int nb1 = data->NbParams(nsub1);
    aItems  = new NCollection_HArray1<StepAP214_PresentedItemSelect>(1, nb1);
    for (int i1 = 1; i1 <= nb1; i1++)
    {
      bool stat1 = data->ReadEntity(nsub1, i1, "items", ach, anent1);
      if (stat1)
        aItems->SetValue(i1, anent1);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aItems);
}

void RWStepAP214_RWAppliedPresentedItem::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepAP214_AppliedPresentedItem>& ent) const
{

  // --- own field : items ---

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->NbItems(); i1++)
  {
    SW.Send(ent->ItemsValue(i1).Value());
  }
  SW.CloseSub();
}

void RWStepAP214_RWAppliedPresentedItem::Share(
  const occ::handle<StepAP214_AppliedPresentedItem>& ent,
  Interface_EntityIterator&                          iter) const
{

  int nbElem1 = ent->NbItems();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ItemsValue(is1).Value());
  }
}
