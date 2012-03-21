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


#include <RWStepVisual_RWPresentationLayerAssignment.ixx>
#include <StepVisual_HArray1OfLayeredItem.hxx>
#include <StepVisual_LayeredItem.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PresentationLayerAssignment.hxx>


RWStepVisual_RWPresentationLayerAssignment::RWStepVisual_RWPresentationLayerAssignment () {}

void RWStepVisual_RWPresentationLayerAssignment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PresentationLayerAssignment)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"presentation_layer_assignment")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"description",ach,aDescription);

	// --- own field : assignedItems ---

	Handle(StepVisual_HArray1OfLayeredItem) aAssignedItems;
	StepVisual_LayeredItem aAssignedItemsItem;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"assigned_items",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aAssignedItems = new StepVisual_HArray1OfLayeredItem (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3,i3,"assigned_items",ach,aAssignedItemsItem))
	      aAssignedItems->SetValue(i3,aAssignedItemsItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription, aAssignedItems);
}


void RWStepVisual_RWPresentationLayerAssignment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PresentationLayerAssignment)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : assignedItems ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbAssignedItems();  i3 ++) {
	  SW.Send(ent->AssignedItemsValue(i3).Value());
	}
	SW.CloseSub();
}


void RWStepVisual_RWPresentationLayerAssignment::Share(const Handle(StepVisual_PresentationLayerAssignment)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbAssignedItems();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->AssignedItemsValue(is1).Value());
	}

}

