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
#include <RWStepGeom_RWHyperbola.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement.hxx>
#include <StepGeom_Hyperbola.hxx>

RWStepGeom_RWHyperbola::RWStepGeom_RWHyperbola () {}

void RWStepGeom_RWHyperbola::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Hyperbola)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"hyperbola")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	StepGeom_Axis2Placement aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"position",ach,aPosition);

	// --- own field : semiAxis ---

	Standard_Real aSemiAxis;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"semi_axis",ach,aSemiAxis);

	// --- own field : semiImagAxis ---

	Standard_Real aSemiImagAxis;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"semi_imag_axis",ach,aSemiImagAxis);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aSemiAxis, aSemiImagAxis);
}


void RWStepGeom_RWHyperbola::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Hyperbola)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position().Value());

	// --- own field : semiAxis ---

	SW.Send(ent->SemiAxis());

	// --- own field : semiImagAxis ---

	SW.Send(ent->SemiImagAxis());
}


void RWStepGeom_RWHyperbola::Share(const Handle(StepGeom_Hyperbola)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position().Value());
}

