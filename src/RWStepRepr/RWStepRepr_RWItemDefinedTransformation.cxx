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
#include <RWStepRepr_RWItemDefinedTransformation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepRepr_RWItemDefinedTransformation::RWStepRepr_RWItemDefinedTransformation () {}

void RWStepRepr_RWItemDefinedTransformation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_ItemDefinedTransformation)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"item_defined_transformation")) return;

	// --- own fields
	Handle(TCollection_HAsciiString) name,description;
	//szv#4:S4163:12Mar99 `Standard_Boolean st =` not needed
	data->ReadString (num,1,"name",ach,name);
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `st =` not needed
	  data->ReadString (num,2,"description",ach,description);
	}
	Handle(StepRepr_RepresentationItem) ti1,ti2;
	//szv#4:S4163:12Mar99 `st =` not needed
	data->ReadEntity (num,3,"transform_item_1",ach, STANDARD_TYPE(StepRepr_RepresentationItem), ti1);
	data->ReadEntity (num,4,"transform_item_2",ach, STANDARD_TYPE(StepRepr_RepresentationItem), ti2);

	//--- Initialisation of the read entity ---


	ent->Init(name,description,ti1,ti2);
}


void RWStepRepr_RWItemDefinedTransformation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_ItemDefinedTransformation)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Name());
	SW.Send(ent->Description());
	SW.Send(ent->TransformItem1());
	SW.Send(ent->TransformItem2());
}


void RWStepRepr_RWItemDefinedTransformation::Share(const Handle(StepRepr_ItemDefinedTransformation)& ent, Interface_EntityIterator& iter) const
{
	iter.AddItem(ent->TransformItem1());
	iter.AddItem(ent->TransformItem2());
}

