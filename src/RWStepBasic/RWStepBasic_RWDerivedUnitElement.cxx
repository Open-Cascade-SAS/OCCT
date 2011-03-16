
#include <RWStepBasic_RWDerivedUnitElement.ixx>
#include <StepBasic_NamedUnit.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_DerivedUnitElement.hxx>


RWStepBasic_RWDerivedUnitElement::RWStepBasic_RWDerivedUnitElement () {}

void RWStepBasic_RWDerivedUnitElement::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DerivedUnitElement)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"derived_unit_element")) return;

	// --- own fields

	Handle(StepBasic_NamedUnit) nu;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"unit", ach, STANDARD_TYPE(StepBasic_NamedUnit), nu);
	Standard_Real expo;
	//szv#4:S4163:12Mar99 `stat1 =` not needed
	data->ReadReal (num,2,"exponent", ach, expo);

	//--- Initialisation of the read entity ---


	ent->Init(nu,expo);
}


void RWStepBasic_RWDerivedUnitElement::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DerivedUnitElement)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Unit());
	SW.Send(ent->Exponent());
}


void RWStepBasic_RWDerivedUnitElement::Share(const Handle(StepBasic_DerivedUnitElement)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Unit());
}
