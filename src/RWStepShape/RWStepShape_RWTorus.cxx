
#include <RWStepShape_RWTorus.ixx>
#include <StepGeom_Axis1Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_Torus.hxx>


RWStepShape_RWTorus::RWStepShape_RWTorus () {}

void RWStepShape_RWTorus::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_Torus)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"torus")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : position ---

	Handle(StepGeom_Axis1Placement) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aPosition);

	// --- own field : majorRadius ---

	Standard_Real aMajorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"major_radius",ach,aMajorRadius);

	// --- own field : minorRadius ---

	Standard_Real aMinorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"minor_radius",ach,aMinorRadius);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aMajorRadius, aMinorRadius);
}


void RWStepShape_RWTorus::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_Torus)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : position ---

	SW.Send(ent->Position());

	// --- own field : majorRadius ---

	SW.Send(ent->MajorRadius());

	// --- own field : minorRadius ---

	SW.Send(ent->MinorRadius());
}


void RWStepShape_RWTorus::Share(const Handle(StepShape_Torus)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

