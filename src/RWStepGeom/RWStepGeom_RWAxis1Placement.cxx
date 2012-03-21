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


#include <RWStepGeom_RWAxis1Placement.ixx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_CartesianPoint.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Axis1Placement.hxx>


RWStepGeom_RWAxis1Placement::RWStepGeom_RWAxis1Placement () {}

void RWStepGeom_RWAxis1Placement::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Axis1Placement)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"axis1_placement")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : location ---

	Handle(StepGeom_CartesianPoint) aLocation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"location", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aLocation);

	// --- own field : axis ---

	Handle(StepGeom_Direction) aAxis;
	Standard_Boolean hasAaxis = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadEntity(num, 3,"axis", ach, STANDARD_TYPE(StepGeom_Direction), aAxis);
	}
	else {
	  hasAaxis = Standard_False;
	  aAxis.Nullify();
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aLocation, hasAaxis, aAxis);
}


void RWStepGeom_RWAxis1Placement::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Axis1Placement)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field location ---

	SW.Send(ent->Location());

	// --- own field : axis ---

	Standard_Boolean hasAaxis = ent->HasAxis();
	if (hasAaxis) {
	  SW.Send(ent->Axis());
	}
	else {
	  SW.SendUndef();
	}
}


void RWStepGeom_RWAxis1Placement::Share(const Handle(StepGeom_Axis1Placement)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Location());

	if (ent->HasAxis()) {
	  iter.GetOneItem(ent->Axis());
	}

}

