
#include <RWStepBasic_RWLengthUnit.ixx>
#include <StepBasic_DimensionalExponents.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_LengthUnit.hxx>


RWStepBasic_RWLengthUnit::RWStepBasic_RWLengthUnit () {}

void RWStepBasic_RWLengthUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_LengthUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"length_unit")) return;

	// --- inherited field : dimensions ---

	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	//--- Initialisation of the read entity ---


	ent->Init(aDimensions);
}


void RWStepBasic_RWLengthUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_LengthUnit)& ent) const
{

	// --- inherited field dimensions ---

	SW.Send(ent->Dimensions());
}


void RWStepBasic_RWLengthUnit::Share(const Handle(StepBasic_LengthUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());
}

