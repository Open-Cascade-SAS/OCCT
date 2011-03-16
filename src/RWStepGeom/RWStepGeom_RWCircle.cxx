
#include <RWStepGeom_RWCircle.ixx>
#include <StepGeom_Axis2Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Circle.hxx>


RWStepGeom_RWCircle::RWStepGeom_RWCircle () {}

void RWStepGeom_RWCircle::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Circle)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"circle")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	StepGeom_Axis2Placement aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"position",ach,aPosition);

	// --- own field : radius ---

	Standard_Real aRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"radius",ach,aRadius);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aRadius);
}


void RWStepGeom_RWCircle::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Circle)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position().Value());

	// --- own field : radius ---

	SW.Send(ent->Radius());
}


void RWStepGeom_RWCircle::Share(const Handle(StepGeom_Circle)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position().Value());
}

