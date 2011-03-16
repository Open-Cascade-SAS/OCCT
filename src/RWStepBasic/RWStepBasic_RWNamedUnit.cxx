
#include <RWStepBasic_RWNamedUnit.ixx>
#include <StepBasic_DimensionalExponents.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_NamedUnit.hxx>


RWStepBasic_RWNamedUnit::RWStepBasic_RWNamedUnit () {}

void RWStepBasic_RWNamedUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_NamedUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"named_unit")) return;

	// --- own field : dimensions ---

	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	//--- Initialisation of the read entity ---


	ent->Init(aDimensions);
}


void RWStepBasic_RWNamedUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_NamedUnit)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Dimensions());
}


void RWStepBasic_RWNamedUnit::Share(const Handle(StepBasic_NamedUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());
}

