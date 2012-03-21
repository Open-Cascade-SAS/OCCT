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
#include <RWStepRepr_RWItemDefinedTransformation.ixx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>

#include <Interface_EntityIterator.hxx>


#include <StepRepr_ItemDefinedTransformation.hxx>


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

