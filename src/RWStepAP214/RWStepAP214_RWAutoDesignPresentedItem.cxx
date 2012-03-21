// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <RWStepAP214_RWAutoDesignPresentedItem.ixx>
#include <StepAP214_HArray1OfAutoDesignPresentedItemSelect.hxx>
#include <StepAP214_AutoDesignPresentedItemSelect.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepAP214_AutoDesignPresentedItem.hxx>


RWStepAP214_RWAutoDesignPresentedItem::RWStepAP214_RWAutoDesignPresentedItem () {}

void RWStepAP214_RWAutoDesignPresentedItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AutoDesignPresentedItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"auto_design_presented_item")) return;

	// --- own field : items ---

	Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect) aItems;
	StepAP214_AutoDesignPresentedItemSelect anent1;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"items",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aItems = new StepAP214_HArray1OfAutoDesignPresentedItemSelect (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    Standard_Boolean stat1 = data->ReadEntity
	      (nsub1, i1,"auto_design_displayed_item", ach, anent1);
	      if (stat1) aItems->SetValue(i1, anent1);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aItems);
}


void RWStepAP214_RWAutoDesignPresentedItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AutoDesignPresentedItem)& ent) const
{

	// --- own field : items ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbItems();  i1 ++) {
	  SW.Send(ent->ItemsValue(i1).Value());
	}
	SW.CloseSub();
}


void RWStepAP214_RWAutoDesignPresentedItem::Share(const Handle(StepAP214_AutoDesignPresentedItem)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbItems();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ItemsValue(is1).Value());
	}

}

