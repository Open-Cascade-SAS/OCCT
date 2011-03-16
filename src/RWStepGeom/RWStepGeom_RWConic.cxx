
#include <RWStepGeom_RWConic.ixx>
#include <StepGeom_Axis2Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Conic.hxx>


RWStepGeom_RWConic::RWStepGeom_RWConic () {}

void RWStepGeom_RWConic::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Conic)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"conic")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : position ---

	StepGeom_Axis2Placement aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"position",ach,aPosition);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition);
}


void RWStepGeom_RWConic::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Conic)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : position ---

	SW.Send(ent->Position().Value());
}


void RWStepGeom_RWConic::Share(const Handle(StepGeom_Conic)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position().Value());
}

