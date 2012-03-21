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


#include <RWStepShape_RWBoxDomain.ixx>
#include <StepGeom_CartesianPoint.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_BoxDomain.hxx>


RWStepShape_RWBoxDomain::RWStepShape_RWBoxDomain () {}

void RWStepShape_RWBoxDomain::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_BoxDomain)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"box_domain")) return;

	// --- own field : corner ---

	Handle(StepGeom_CartesianPoint) aCorner;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"corner", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aCorner);

	// --- own field : xlength ---

	Standard_Real aXlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"xlength",ach,aXlength);

	// --- own field : ylength ---

	Standard_Real aYlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"ylength",ach,aYlength);

	// --- own field : zlength ---

	Standard_Real aZlength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"zlength",ach,aZlength);

	//--- Initialisation of the read entity ---


	ent->Init(aCorner, aXlength, aYlength, aZlength);
}


void RWStepShape_RWBoxDomain::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_BoxDomain)& ent) const
{

	// --- own field : corner ---

	SW.Send(ent->Corner());

	// --- own field : xlength ---

	SW.Send(ent->Xlength());

	// --- own field : ylength ---

	SW.Send(ent->Ylength());

	// --- own field : zlength ---

	SW.Send(ent->Zlength());
}


void RWStepShape_RWBoxDomain::Share(const Handle(StepShape_BoxDomain)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Corner());
}

