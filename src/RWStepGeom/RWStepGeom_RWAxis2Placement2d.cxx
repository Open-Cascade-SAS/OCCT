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
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWAxis2Placement2d.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>

RWStepGeom_RWAxis2Placement2d::RWStepGeom_RWAxis2Placement2d () {}

void RWStepGeom_RWAxis2Placement2d::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Axis2Placement2d)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"axis2_placement_2d")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : location ---

	Handle(StepGeom_CartesianPoint) aLocation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"location", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aLocation);

	// --- own field : refDirection ---

	Handle(StepGeom_Direction) aRefDirection;
	Standard_Boolean hasArefDirection = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadEntity(num, 3,"ref_direction", ach, STANDARD_TYPE(StepGeom_Direction), aRefDirection);
	}
	else {
	  hasArefDirection = Standard_False;
	  aRefDirection.Nullify();
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aLocation, hasArefDirection, aRefDirection);
}


void RWStepGeom_RWAxis2Placement2d::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Axis2Placement2d)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field location ---

	SW.Send(ent->Location());

	// --- own field : refDirection ---

	Standard_Boolean hasArefDirection = ent->HasRefDirection();
	if (hasArefDirection) {
	  SW.Send(ent->RefDirection());
	}
	else {
	  SW.SendUndef();
	}
}


void RWStepGeom_RWAxis2Placement2d::Share(const Handle(StepGeom_Axis2Placement2d)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Location());

	if (ent->HasRefDirection()) {
	  iter.GetOneItem(ent->RefDirection());
	}

}

