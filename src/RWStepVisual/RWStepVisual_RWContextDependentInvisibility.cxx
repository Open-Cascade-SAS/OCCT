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


#include <RWStepVisual_RWContextDependentInvisibility.ixx>
#include <StepVisual_InvisibilityContext.hxx>
#include <StepVisual_HArray1OfInvisibleItem.hxx>
#include <StepVisual_InvisibleItem.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_ContextDependentInvisibility.hxx>


RWStepVisual_RWContextDependentInvisibility::RWStepVisual_RWContextDependentInvisibility () {}

void RWStepVisual_RWContextDependentInvisibility::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_ContextDependentInvisibility)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"context_dependent_invisibility")) return;

	// --- inherited field : invisibleItems ---

	Handle(StepVisual_HArray1OfInvisibleItem) aInvisibleItems;
	StepVisual_InvisibleItem aInvisibleItemsItem;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"invisible_items",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aInvisibleItems = new StepVisual_HArray1OfInvisibleItem (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	    if (data->ReadEntity (nsub1,i1,"invisible_items",ach,aInvisibleItemsItem))
	      aInvisibleItems->SetValue(i1,aInvisibleItemsItem);
	  }
	}

	// --- own field : presentationContext ---

	StepVisual_InvisibilityContext aPresentationContext;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"presentation_context",ach,aPresentationContext);

	//--- Initialisation of the read entity ---


	ent->Init(aInvisibleItems, aPresentationContext);
}


void RWStepVisual_RWContextDependentInvisibility::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_ContextDependentInvisibility)& ent) const
{

	// --- inherited field invisibleItems ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbInvisibleItems();  i1 ++) {
	  SW.Send(ent->InvisibleItemsValue(i1).Value());
	}
	SW.CloseSub();

	// --- own field : presentationContext ---

	SW.Send(ent->PresentationContext().Value());
}


void RWStepVisual_RWContextDependentInvisibility::Share(const Handle(StepVisual_ContextDependentInvisibility)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbInvisibleItems();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->InvisibleItemsValue(is1).Value());
	}



	iter.GetOneItem(ent->PresentationContext().Value());
}

