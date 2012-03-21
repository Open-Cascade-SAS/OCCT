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


#include <RWStepShape_RWSphere.ixx>
#include <StepGeom_Point.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_Sphere.hxx>


RWStepShape_RWSphere::RWStepShape_RWSphere () {}

void RWStepShape_RWSphere::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_Sphere)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"sphere")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : radius ---

	Standard_Real aRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"radius",ach,aRadius);

	// --- own field : centre ---

	Handle(StepGeom_Point) aCentre;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"centre", ach, STANDARD_TYPE(StepGeom_Point), aCentre);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aRadius, aCentre);
}


void RWStepShape_RWSphere::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_Sphere)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : radius ---

	SW.Send(ent->Radius());

	// --- own field : centre ---

	SW.Send(ent->Centre());
}


void RWStepShape_RWSphere::Share(const Handle(StepShape_Sphere)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Centre());
}

