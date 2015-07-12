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
#include <RWStepAP214_RWAppliedPresentedItem.hxx>
#include <StepAP214_AppliedPresentedItem.hxx>
#include <StepAP214_HArray1OfPresentedItemSelect.hxx>
#include <StepAP214_PresentedItemSelect.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepAP214_RWAppliedPresentedItem::RWStepAP214_RWAppliedPresentedItem () {}

void RWStepAP214_RWAppliedPresentedItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AppliedPresentedItem)& ent) const
{


  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,1,ach,"applied_presented_item")) return;
  
  // --- own field : items ---
  
  Handle(StepAP214_HArray1OfPresentedItemSelect) aItems;
  StepAP214_PresentedItemSelect anent1;
  Standard_Integer nsub1;
  if (data->ReadSubList (num,1,"items",ach,nsub1)) {
    Standard_Integer nb1 = data->NbParams(nsub1);
    aItems = new StepAP214_HArray1OfPresentedItemSelect (1, nb1);
    for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
      Standard_Boolean stat1 = data->ReadEntity
	(nsub1, i1,"items", ach, anent1);
      if (stat1) aItems->SetValue(i1, anent1);
    }
  }

  //--- Initialisation of the read entity ---


  ent->Init(aItems);
}


void RWStepAP214_RWAppliedPresentedItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AppliedPresentedItem)& ent) const
{

  // --- own field : items ---
  
  SW.OpenSub();
  for (Standard_Integer i1 = 1;  i1 <= ent->NbItems();  i1 ++) {
    SW.Send(ent->ItemsValue(i1).Value());
  }
  SW.CloseSub();
}


void RWStepAP214_RWAppliedPresentedItem::Share(const Handle(StepAP214_AppliedPresentedItem)& ent, Interface_EntityIterator& iter) const
{

  Standard_Integer nbElem1 = ent->NbItems();
  for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
    iter.GetOneItem(ent->ItemsValue(is1).Value());
  }

}

