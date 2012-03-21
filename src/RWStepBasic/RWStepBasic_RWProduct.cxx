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

//gka 05.03.99 S4134 upgrade from CD to DIS
#include <RWStepBasic_RWProduct.ixx>
#include <StepBasic_HArray1OfProductContext.hxx>
#include <StepBasic_ProductContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_Product.hxx>


RWStepBasic_RWProduct::RWStepBasic_RWProduct () {}

void RWStepBasic_RWProduct::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Product)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"product")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,3)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadString (num,3,"description",ach,aDescription);
	}
	// --- own field : frameOfReference ---

	Handle(StepBasic_HArray1OfProductContext) aFrameOfReference;
	Handle(StepBasic_ProductContext) anent4;
	Standard_Integer nsub4;
	if (data->ReadSubList (num,4,"frame_of_reference",ach,nsub4)) {
	  Standard_Integer nb4 = data->NbParams(nsub4);
	  aFrameOfReference = new StepBasic_HArray1OfProductContext (1, nb4);
	  for (Standard_Integer i4 = 1; i4 <= nb4; i4 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	    if (data->ReadEntity (nsub4, i4,"product_context", ach, STANDARD_TYPE(StepBasic_ProductContext), anent4))
	      aFrameOfReference->SetValue(i4, anent4);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aId, aName, aDescription, aFrameOfReference);
}


void RWStepBasic_RWProduct::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Product)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : frameOfReference ---

	SW.OpenSub();
	for (Standard_Integer i4 = 1;  i4 <= ent->NbFrameOfReference();  i4 ++) {
	  SW.Send(ent->FrameOfReferenceValue(i4));
	}
	SW.CloseSub();
}


void RWStepBasic_RWProduct::Share(const Handle(StepBasic_Product)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbFrameOfReference();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->FrameOfReferenceValue(is1));
	}

}

