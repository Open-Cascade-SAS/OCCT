
#include <RWStepShape_RWSolidReplica.ixx>
#include <StepShape_SolidModel.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_SolidReplica.hxx>


RWStepShape_RWSolidReplica::RWStepShape_RWSolidReplica () {}

void RWStepShape_RWSolidReplica::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_SolidReplica)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"solid_replica")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : parentSolid ---

	Handle(StepShape_SolidModel) aParentSolid;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"parent_solid", ach, STANDARD_TYPE(StepShape_SolidModel), aParentSolid);

	// --- own field : transformation ---

	Handle(StepGeom_CartesianTransformationOperator3d) aTransformation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"transformation", ach, STANDARD_TYPE(StepGeom_CartesianTransformationOperator3d), aTransformation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aParentSolid, aTransformation);
}


void RWStepShape_RWSolidReplica::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_SolidReplica)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : parentSolid ---

	SW.Send(ent->ParentSolid());

	// --- own field : transformation ---

	SW.Send(ent->Transformation());
}


void RWStepShape_RWSolidReplica::Share(const Handle(StepShape_SolidReplica)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ParentSolid());


	iter.GetOneItem(ent->Transformation());
}

