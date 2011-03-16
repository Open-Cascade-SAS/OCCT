
#include <RWStepShape_RWVertexPoint.ixx>
#include <StepGeom_Point.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_VertexPoint.hxx>


RWStepShape_RWVertexPoint::RWStepShape_RWVertexPoint () {}

void RWStepShape_RWVertexPoint::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_VertexPoint)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"vertex_point")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : vertexGeometry ---

	Handle(StepGeom_Point) aVertexGeometry;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"vertex_geometry", ach, STANDARD_TYPE(StepGeom_Point), aVertexGeometry);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aVertexGeometry);
}


void RWStepShape_RWVertexPoint::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_VertexPoint)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : vertexGeometry ---

	SW.Send(ent->VertexGeometry());
}


void RWStepShape_RWVertexPoint::Share(const Handle(StepShape_VertexPoint)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->VertexGeometry());
}

