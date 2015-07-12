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
#include <RWStepVisual_RWPlanarBox.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement.hxx>
#include <StepVisual_PlanarBox.hxx>

RWStepVisual_RWPlanarBox::RWStepVisual_RWPlanarBox () {}

void RWStepVisual_RWPlanarBox::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PlanarBox)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"planar_box")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : sizeInX ---

	Standard_Real aSizeInX;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"size_in_x",ach,aSizeInX);

	// --- inherited field : sizeInY ---

	Standard_Real aSizeInY;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"size_in_y",ach,aSizeInY);

	// --- own field : placement ---

	StepGeom_Axis2Placement aPlacement;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num,4,"placement",ach,aPlacement);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSizeInX, aSizeInY, aPlacement);
}


void RWStepVisual_RWPlanarBox::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PlanarBox)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field sizeInX ---

	SW.Send(ent->SizeInX());

	// --- inherited field sizeInY ---

	SW.Send(ent->SizeInY());

	// --- own field : placement ---

	SW.Send(ent->Placement().Value());
}


void RWStepVisual_RWPlanarBox::Share(const Handle(StepVisual_PlanarBox)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Placement().Value());
}

