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
#include <RWStepVisual_RWCompositeTextWithExtent.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CompositeTextWithExtent.hxx>
#include <StepVisual_HArray1OfTextOrCharacter.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <StepVisual_TextOrCharacter.hxx>

RWStepVisual_RWCompositeTextWithExtent::RWStepVisual_RWCompositeTextWithExtent () {}

void RWStepVisual_RWCompositeTextWithExtent::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CompositeTextWithExtent)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"composite_text_with_extent has not 3 parameter(s)")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : collectedText ---

	Handle(StepVisual_HArray1OfTextOrCharacter) aCollectedText;
	StepVisual_TextOrCharacter aCollectedTextItem;
	Standard_Integer nsub2;
	nsub2 = data->SubListNumber(num, 2, Standard_False);
	if (nsub2 !=0) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aCollectedText = new StepVisual_HArray1OfTextOrCharacter (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2,i2,"collected_text",ach,aCollectedTextItem))
	      aCollectedText->SetValue(i2,aCollectedTextItem);
	  }
	}
	else {
	  ach->AddFail("Parameter #2 (collected_text) is not a LIST");
	}

	// --- own field : extent ---

	Handle(StepVisual_PlanarExtent) aExtent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"extent", ach, STANDARD_TYPE(StepVisual_PlanarExtent), aExtent);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aCollectedText, aExtent);
}


void RWStepVisual_RWCompositeTextWithExtent::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CompositeTextWithExtent)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field collectedText ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbCollectedText();  i2 ++) {
	  SW.Send(ent->CollectedTextValue(i2).Value());
	}
	SW.CloseSub();

	// --- own field : extent ---

	SW.Send(ent->Extent());
}


void RWStepVisual_RWCompositeTextWithExtent::Share(const Handle(StepVisual_CompositeTextWithExtent)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbCollectedText();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->CollectedTextValue(is1).Value());
	}



	iter.GetOneItem(ent->Extent());
}

