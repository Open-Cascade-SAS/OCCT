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


#include <RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource.ixx>
#include <StepBasic_Source.hxx>
#include <StepBasic_Product.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>

#include <TCollection_AsciiString.hxx>


// --- Enum : Source ---
static TCollection_AsciiString sBought(".BOUGHT.");
static TCollection_AsciiString sNotKnown(".NOT_KNOWN.");
static TCollection_AsciiString sMade(".MADE.");

RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource () {}

void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinitionFormationWithSpecifiedSource)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"product_definition_formation_with_specified_source")) return;

	// --- inherited field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- inherited field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"description",ach,aDescription);

	// --- inherited field : ofProduct ---

	Handle(StepBasic_Product) aOfProduct;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"of_product", ach, STANDARD_TYPE(StepBasic_Product), aOfProduct);

	// --- own field : makeOrBuy ---

	StepBasic_Source aMakeOrBuy = StepBasic_sNotKnown;
	if (data->ParamType(num,4) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,4);
	  if      (sBought.IsEqual(text)) aMakeOrBuy = StepBasic_sBought;
	  else if (sNotKnown.IsEqual(text)) aMakeOrBuy = StepBasic_sNotKnown;
	  else if (sMade.IsEqual(text)) aMakeOrBuy = StepBasic_sMade;
	  else ach->AddFail("Enumeration source has not an allowed value");
	}
	else ach->AddFail("Parameter #4 (make_or_buy) is not an enumeration");

	//--- Initialisation of the read entity ---


	ent->Init(aId, aDescription, aOfProduct, aMakeOrBuy);
}


void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinitionFormationWithSpecifiedSource)& ent) const
{

	// --- inherited field id ---

	SW.Send(ent->Id());

	// --- inherited field description ---

	SW.Send(ent->Description());

	// --- inherited field ofProduct ---

	SW.Send(ent->OfProduct());

	// --- own field : makeOrBuy ---

	switch(ent->MakeOrBuy()) {
	  case StepBasic_sBought : SW.SendEnum (sBought); break;
	  case StepBasic_sNotKnown : SW.SendEnum (sNotKnown); break;
	  case StepBasic_sMade : SW.SendEnum (sMade); break;
	}
}


void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::Share(const Handle(StepBasic_ProductDefinitionFormationWithSpecifiedSource)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->OfProduct());
}

