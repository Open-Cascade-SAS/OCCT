
#include <RWStepShape_RWFaceOuterBound.ixx>
#include <StepShape_Loop.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_FaceOuterBound.hxx>


RWStepShape_RWFaceOuterBound::RWStepShape_RWFaceOuterBound () {}

void RWStepShape_RWFaceOuterBound::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FaceOuterBound)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"face_outer_bound")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : bound ---

	Handle(StepShape_Loop) aBound;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"bound", ach, STANDARD_TYPE(StepShape_Loop), aBound);

	// --- inherited field : orientation ---

	Standard_Boolean aOrientation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadBoolean (num,3,"orientation",ach,aOrientation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBound, aOrientation);
}


void RWStepShape_RWFaceOuterBound::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FaceOuterBound)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field bound ---

	SW.Send(ent->Bound());

	// --- inherited field orientation ---

	SW.SendBoolean(ent->Orientation());
}


void RWStepShape_RWFaceOuterBound::Share(const Handle(StepShape_FaceOuterBound)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Bound());
}

