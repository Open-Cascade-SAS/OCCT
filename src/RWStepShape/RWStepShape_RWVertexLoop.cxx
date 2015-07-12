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
#include <RWStepShape_RWVertexLoop.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>

RWStepShape_RWVertexLoop::RWStepShape_RWVertexLoop () {}

void RWStepShape_RWVertexLoop::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_VertexLoop)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"vertex_loop")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : loopVertex ---

	Handle(StepShape_Vertex) aLoopVertex;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"loop_vertex", ach, STANDARD_TYPE(StepShape_Vertex), aLoopVertex);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aLoopVertex);
}


void RWStepShape_RWVertexLoop::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_VertexLoop)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : loopVertex ---

	SW.Send(ent->LoopVertex());
}


void RWStepShape_RWVertexLoop::Share(const Handle(StepShape_VertexLoop)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->LoopVertex());
}

