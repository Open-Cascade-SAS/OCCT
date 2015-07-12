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
#include <RWStepShape_RWRightAngularWedge.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepShape_RightAngularWedge.hxx>

RWStepShape_RWRightAngularWedge::RWStepShape_RWRightAngularWedge () {}

void RWStepShape_RWRightAngularWedge::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_RightAngularWedge)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,6,ach,"right_angular_wedge")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : position ---

	Handle(StepGeom_Axis2Placement3d) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

	// --- own field : x ---

	Standard_Real aX;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"x",ach,aX);

	// --- own field : y ---

	Standard_Real aY;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"y",ach,aY);

	// --- own field : z ---

	Standard_Real aZ;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadReal (num,5,"z",ach,aZ);

	// --- own field : ltx ---

	Standard_Real aLtx;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadReal (num,6,"ltx",ach,aLtx);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aX, aY, aZ, aLtx);
}


void RWStepShape_RWRightAngularWedge::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_RightAngularWedge)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : position ---

	SW.Send(ent->Position());

	// --- own field : x ---

	SW.Send(ent->X());

	// --- own field : y ---

	SW.Send(ent->Y());

	// --- own field : z ---

	SW.Send(ent->Z());

	// --- own field : ltx ---

	SW.Send(ent->Ltx());
}


void RWStepShape_RWRightAngularWedge::Share(const Handle(StepShape_RightAngularWedge)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

