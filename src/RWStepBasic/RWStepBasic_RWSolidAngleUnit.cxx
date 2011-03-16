
#include <RWStepBasic_RWSolidAngleUnit.ixx>
#include <StepBasic_DimensionalExponents.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_SolidAngleUnit.hxx>


RWStepBasic_RWSolidAngleUnit::RWStepBasic_RWSolidAngleUnit () {}

void RWStepBasic_RWSolidAngleUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_SolidAngleUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"solid_angle_unit")) return;

	// --- inherited field : dimensions ---

	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	//--- Initialisation of the read entity ---


	ent->Init(aDimensions);
}


void RWStepBasic_RWSolidAngleUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_SolidAngleUnit)& ent) const
{

	// --- inherited field dimensions ---

	SW.Send(ent->Dimensions());
}


void RWStepBasic_RWSolidAngleUnit::Share(const Handle(StepBasic_SolidAngleUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());
}

