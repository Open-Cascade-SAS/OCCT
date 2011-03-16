
#include <RWStepGeom_RWParabola.ixx>
#include <StepGeom_Axis2Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Parabola.hxx>


RWStepGeom_RWParabola::RWStepGeom_RWParabola () {}

void RWStepGeom_RWParabola::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Parabola)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"parabola")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	StepGeom_Axis2Placement aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"position",ach,aPosition);

	// --- own field : focalDist ---

	Standard_Real aFocalDist;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"focal_dist",ach,aFocalDist);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aFocalDist);
}


void RWStepGeom_RWParabola::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Parabola)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position().Value());

	// --- own field : focalDist ---

	SW.Send(ent->FocalDist());
}


void RWStepGeom_RWParabola::Share(const Handle(StepGeom_Parabola)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position().Value());
}

