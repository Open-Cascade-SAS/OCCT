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
#include <RWStepGeom_RWRectangularTrimmedSurface.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_Surface.hxx>

RWStepGeom_RWRectangularTrimmedSurface::RWStepGeom_RWRectangularTrimmedSurface () {}

void RWStepGeom_RWRectangularTrimmedSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_RectangularTrimmedSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,8,ach,"rectangular_trimmed_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : basisSurface ---

	Handle(StepGeom_Surface) aBasisSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

	// --- own field : u1 ---

	Standard_Real aU1;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"u1",ach,aU1);

	// --- own field : u2 ---

	Standard_Real aU2;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"u2",ach,aU2);

	// --- own field : v1 ---

	Standard_Real aV1;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadReal (num,5,"v1",ach,aV1);

	// --- own field : v2 ---

	Standard_Real aV2;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadReal (num,6,"v2",ach,aV2);

	// --- own field : usense ---

	Standard_Boolean aUsense;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	data->ReadBoolean (num,7,"usense",ach,aUsense);

	// --- own field : vsense ---

	Standard_Boolean aVsense;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	data->ReadBoolean (num,8,"vsense",ach,aVsense);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBasisSurface, aU1, aU2, aV1, aV2, aUsense, aVsense);
}


void RWStepGeom_RWRectangularTrimmedSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_RectangularTrimmedSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : basisSurface ---

	SW.Send(ent->BasisSurface());

	// --- own field : u1 ---

	SW.Send(ent->U1());

	// --- own field : u2 ---

	SW.Send(ent->U2());

	// --- own field : v1 ---

	SW.Send(ent->V1());

	// --- own field : v2 ---

	SW.Send(ent->V2());

	// --- own field : usense ---

	SW.SendBoolean(ent->Usense());

	// --- own field : vsense ---

	SW.SendBoolean(ent->Vsense());
}


void RWStepGeom_RWRectangularTrimmedSurface::Share(const Handle(StepGeom_RectangularTrimmedSurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BasisSurface());
}

