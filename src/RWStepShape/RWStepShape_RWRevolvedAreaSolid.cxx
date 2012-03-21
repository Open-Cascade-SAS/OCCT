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


#include <RWStepShape_RWRevolvedAreaSolid.ixx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_RevolvedAreaSolid.hxx>


RWStepShape_RWRevolvedAreaSolid::RWStepShape_RWRevolvedAreaSolid () {}

void RWStepShape_RWRevolvedAreaSolid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_RevolvedAreaSolid)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"revolved_area_solid")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : sweptArea ---

	Handle(StepGeom_CurveBoundedSurface) aSweptArea;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"swept_area", ach, STANDARD_TYPE(StepGeom_CurveBoundedSurface), aSweptArea);

	// --- own field : axis ---

	Handle(StepGeom_Axis1Placement) aAxis;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"axis", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aAxis);

	// --- own field : angle ---

	Standard_Real aAngle;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"angle",ach,aAngle);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSweptArea, aAxis, aAngle);
}


void RWStepShape_RWRevolvedAreaSolid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_RevolvedAreaSolid)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field sweptArea ---

	SW.Send(ent->SweptArea());

	// --- own field : axis ---

	SW.Send(ent->Axis());

	// --- own field : angle ---

	SW.Send(ent->Angle());
}


void RWStepShape_RWRevolvedAreaSolid::Share(const Handle(StepShape_RevolvedAreaSolid)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->SweptArea());


	iter.GetOneItem(ent->Axis());
}

