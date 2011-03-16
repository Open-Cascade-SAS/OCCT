
#include <RWStepShape_RWVertexLoop.ixx>
#include <StepShape_Vertex.hxx>


#include <Interface_EntityIterator.hxx>


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

