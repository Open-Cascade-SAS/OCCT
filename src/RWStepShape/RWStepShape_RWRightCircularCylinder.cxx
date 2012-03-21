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


#include <RWStepShape_RWRightCircularCylinder.ixx>
#include <StepGeom_Axis1Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_RightCircularCylinder.hxx>


RWStepShape_RWRightCircularCylinder::RWStepShape_RWRightCircularCylinder () {}

void RWStepShape_RWRightCircularCylinder::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_RightCircularCylinder)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"right_circular_cylinder")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : position ---

	Handle(StepGeom_Axis1Placement) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aPosition);

	// --- own field : height ---

	Standard_Real aHeight;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"height",ach,aHeight);

	// --- own field : radius ---

	Standard_Real aRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"radius",ach,aRadius);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aHeight, aRadius);
}


void RWStepShape_RWRightCircularCylinder::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_RightCircularCylinder)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : position ---

	SW.Send(ent->Position());

	// --- own field : height ---

	SW.Send(ent->Height());

	// --- own field : radius ---

	SW.Send(ent->Radius());
}


void RWStepShape_RWRightCircularCylinder::Share(const Handle(StepShape_RightCircularCylinder)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

