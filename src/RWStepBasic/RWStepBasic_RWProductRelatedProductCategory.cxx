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


#include <RWStepBasic_RWProductRelatedProductCategory.ixx>
#include <StepBasic_HArray1OfProduct.hxx>
#include <StepBasic_Product.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ProductRelatedProductCategory.hxx>


RWStepBasic_RWProductRelatedProductCategory::RWStepBasic_RWProductRelatedProductCategory () {}

void RWStepBasic_RWProductRelatedProductCategory::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductRelatedProductCategory)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"product_related_product_category")) return;

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

	// --- own field : products ---

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


void RWStepBasic_RWProductRelatedProductCategory::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductRelatedProductCategory)& ent) const
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

	// --- own field : products ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbProducts();  i3 ++) {
	  SW.Send(ent->ProductsValue(i3));
	}
	SW.CloseSub();
}


void RWStepBasic_RWProductRelatedProductCategory::Share(const Handle(StepBasic_ProductRelatedProductCategory)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbProducts();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ProductsValue(is1));
	}

}

