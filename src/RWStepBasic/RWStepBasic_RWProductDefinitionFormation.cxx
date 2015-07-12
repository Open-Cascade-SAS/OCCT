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

//gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWProductDefinitionFormation.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProductDefinitionFormation::RWStepBasic_RWProductDefinitionFormation () {}

void RWStepBasic_RWProductDefinitionFormation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinitionFormation)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"product_definition_formation")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	// --- own field : ofProduct ---

	Handle(StepBasic_Product) aOfProduct;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"of_product", ach, STANDARD_TYPE(StepBasic_Product), aOfProduct);

	//--- Initialisation of the read entity ---


	ent->Init(aId, aDescription, aOfProduct);
}


void RWStepBasic_RWProductDefinitionFormation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinitionFormation)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : ofProduct ---

	SW.Send(ent->OfProduct());
}


void RWStepBasic_RWProductDefinitionFormation::Share(const Handle(StepBasic_ProductDefinitionFormation)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->OfProduct());
}

