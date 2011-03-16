
#include <RWStepShape_RWVertex.ixx>


RWStepShape_RWVertex::RWStepShape_RWVertex () {}

void RWStepShape_RWVertex::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_Vertex)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"vertex")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepShape_RWVertex::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_Vertex)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
