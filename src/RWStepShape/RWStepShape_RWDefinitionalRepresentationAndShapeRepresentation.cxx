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
#include <RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_DefinitionalRepresentationAndShapeRepresentation.hxx>

RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation::RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation () {}

void RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_DefinitionalRepresentationAndShapeRepresentation)& ent) const
{

	Standard_Integer num = num0;

	// skip definitional_representation
	
	num = data->NextForComplex(num);


	// --- Instance of plex componant definitional_representation ---

	if (!data->CheckNbParams(num,3,ach,"representation")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : items ---

	Handle(StepRepr_HArray1OfRepresentationItem) aItems;
	Handle(StepRepr_RepresentationItem) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"items",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aItems = new StepRepr_HArray1OfRepresentationItem (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    if (data->ReadEntity (nsub2, i2,"representation_item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anent2))
	      aItems->SetValue(i2, anent2);
	  }
	}

	// --- inherited field : contextOfItems ---

	Handle(StepRepr_RepresentationContext) aContextOfItems;
	data->ReadEntity(num, 3,"context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aContextOfItems);

	// skip shape_representation
	num = data->NextForComplex(num);

	//--- Initialisation of the read entity ---
	ent->Init(aName, aItems, aContextOfItems);

}


void RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_DefinitionalRepresentationAndShapeRepresentation)& ent) const
{

	// --- Instance of plex componant ConversionBasedUnit ---

	SW.StartEntity("DEFINITIONAL_REPRESENTATION");

	SW.StartEntity("REPRESENTATION");

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field items ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbItems();  i2 ++) {
	  SW.Send(ent->ItemsValue(i2));
	}
	SW.CloseSub();

	// --- inherited field contextOfItems ---

	SW.Send(ent->ContextOfItems());

	// --- Instance of plex componant LengthUnit ---

	SW.StartEntity("SHAPE_REPRESENTATION");

}


void RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation::Share(const Handle(StepShape_DefinitionalRepresentationAndShapeRepresentation)& ent, 
									   Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbItems();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ItemsValue(is1));
	}



	iter.GetOneItem(ent->ContextOfItems());
}

