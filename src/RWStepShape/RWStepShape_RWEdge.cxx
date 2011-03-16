
#include <RWStepShape_RWEdge.ixx>
#include <StepShape_Vertex.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_Edge.hxx>


RWStepShape_RWEdge::RWStepShape_RWEdge () {}

void RWStepShape_RWEdge::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_Edge)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"edge")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : edgeStart ---

	Handle(StepShape_Vertex) aEdgeStart;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"edge_start", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeStart);

	// --- own field : edgeEnd ---

	Handle(StepShape_Vertex) aEdgeEnd;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"edge_end", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeEnd);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aEdgeStart, aEdgeEnd);
}


void RWStepShape_RWEdge::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_Edge)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : edgeStart ---

	SW.Send(ent->EdgeStart());

	// --- own field : edgeEnd ---

	SW.Send(ent->EdgeEnd());
}


void RWStepShape_RWEdge::Share(const Handle(StepShape_Edge)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->EdgeStart());


	iter.GetOneItem(ent->EdgeEnd());
}

