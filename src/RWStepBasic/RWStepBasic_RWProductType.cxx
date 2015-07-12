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
#include <RWStepBasic_RWProductType.hxx>
#include <StepBasic_HArray1OfProduct.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProductType::RWStepBasic_RWProductType () {}

void RWStepBasic_RWProductType::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductType)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"product_type")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	Standard_Boolean hasAdescription = Standard_True;
	if (data->IsParamDefined(num,2)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	else {
	  hasAdescription = Standard_False;
	  aDescription.Nullify();
	}

	// --- inherited field : products ---

	Handle(StepBasic_HArray1OfProduct) aProducts;
	Handle(StepBasic_Product) anent3;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"products",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aProducts = new StepBasic_HArray1OfProduct (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3, i3,"product", ach, STANDARD_TYPE(StepBasic_Product), anent3))
	      aProducts->SetValue(i3, anent3);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, hasAdescription, aDescription, aProducts);
}


void RWStepBasic_RWProductType::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductType)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field description ---

	Standard_Boolean hasAdescription = ent->HasDescription();
	if (hasAdescription) {
	  SW.Send(ent->Description());
	}
	else {
	  SW.SendUndef();
	}

	// --- inherited field products ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbProducts();  i3 ++) {
	  SW.Send(ent->ProductsValue(i3));
	}
	SW.CloseSub();
}


void RWStepBasic_RWProductType::Share(const Handle(StepBasic_ProductType)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbProducts();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ProductsValue(is1));
	}

}

