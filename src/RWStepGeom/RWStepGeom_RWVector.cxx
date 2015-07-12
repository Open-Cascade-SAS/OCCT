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
#include <Interface_ShareTool.hxx>
#include <RWStepGeom_RWVector.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_Vector.hxx>

RWStepGeom_RWVector::RWStepGeom_RWVector () {}

void RWStepGeom_RWVector::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Vector)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"vector")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : orientation ---

	Handle(StepGeom_Direction) aOrientation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"orientation", ach, STANDARD_TYPE(StepGeom_Direction), aOrientation);

	// --- own field : magnitude ---

	Standard_Real aMagnitude;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"magnitude",ach,aMagnitude);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOrientation, aMagnitude);
}


void RWStepGeom_RWVector::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Vector)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : orientation ---

	SW.Send(ent->Orientation());

	// --- own field : magnitude ---

	SW.Send(ent->Magnitude());
}


void RWStepGeom_RWVector::Share(const Handle(StepGeom_Vector)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Orientation());
}



void RWStepGeom_RWVector::Check
  (const Handle(StepGeom_Vector)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
  if(Abs(ent->Magnitude()) < RealEpsilon()) {
    ach->AddFail("ERROR: Magnitude of Vector = 0.0");
  }
}
