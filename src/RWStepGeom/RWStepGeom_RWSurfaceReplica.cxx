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
#include <RWStepGeom_RWSurfaceReplica.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceReplica.hxx>

RWStepGeom_RWSurfaceReplica::RWStepGeom_RWSurfaceReplica () {}

void RWStepGeom_RWSurfaceReplica::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_SurfaceReplica)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"surface_replica")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : parentSurface ---

	Handle(StepGeom_Surface) aParentSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"parent_surface", ach, STANDARD_TYPE(StepGeom_Surface), aParentSurface);

	// --- own field : transformation ---

	Handle(StepGeom_CartesianTransformationOperator3d) aTransformation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"transformation", ach, STANDARD_TYPE(StepGeom_CartesianTransformationOperator3d), aTransformation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aParentSurface, aTransformation);
}


void RWStepGeom_RWSurfaceReplica::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_SurfaceReplica)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : parentSurface ---

	SW.Send(ent->ParentSurface());

	// --- own field : transformation ---

	SW.Send(ent->Transformation());
}


void RWStepGeom_RWSurfaceReplica::Share(const Handle(StepGeom_SurfaceReplica)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ParentSurface());


	iter.GetOneItem(ent->Transformation());
}

