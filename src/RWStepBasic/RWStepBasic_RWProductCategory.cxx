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
#include <RWStepBasic_RWProductCategory.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProductCategory::RWStepBasic_RWProductCategory () {}

void RWStepBasic_RWProductCategory::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductCategory)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"product_category")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

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

	//--- Initialisation of the read entity ---


	ent->Init(aName, hasAdescription, aDescription);
}


void RWStepBasic_RWProductCategory::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductCategory)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	Standard_Boolean hasAdescription = ent->HasDescription();
	if (hasAdescription) {
	  SW.Send(ent->Description());
	}
	else {
	  SW.SendUndef();
	}
}
