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


#include <RWStepGeom_RWLine.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Vector.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Line.hxx>


RWStepGeom_RWLine::RWStepGeom_RWLine () {}

void RWStepGeom_RWLine::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Line)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"line")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : pnt ---

	Handle(StepGeom_CartesianPoint) aPnt;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"pnt", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aPnt);

	// --- own field : dir ---

	Handle(StepGeom_Vector) aDir;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"dir", ach, STANDARD_TYPE(StepGeom_Vector), aDir);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPnt, aDir);
}


void RWStepGeom_RWLine::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Line)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : pnt ---

	SW.Send(ent->Pnt());

	// --- own field : dir ---

	SW.Send(ent->Dir());
}


void RWStepGeom_RWLine::Share(const Handle(StepGeom_Line)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Pnt());


	iter.GetOneItem(ent->Dir());
}

