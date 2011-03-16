
#include <RWStepBasic_RWProductDefinitionContext.ixx>
#include <StepBasic_ApplicationContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ProductDefinitionContext.hxx>


RWStepBasic_RWProductDefinitionContext::RWStepBasic_RWProductDefinitionContext () {}

void RWStepBasic_RWProductDefinitionContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinitionContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"product_definition_context")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : frameOfReference ---

	Handle(StepBasic_ApplicationContext) aFrameOfReference;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aFrameOfReference);

	// --- own field : lifeCycleStage ---

	Handle(TCollection_HAsciiString) aLifeCycleStage;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,3,"life_cycle_stage",ach,aLifeCycleStage);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aFrameOfReference, aLifeCycleStage);
}


void RWStepBasic_RWProductDefinitionContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinitionContext)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field frameOfReference ---

	SW.Send(ent->FrameOfReference());

	// --- own field : lifeCycleStage ---

	SW.Send(ent->LifeCycleStage());
}


void RWStepBasic_RWProductDefinitionContext::Share(const Handle(StepBasic_ProductDefinitionContext)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FrameOfReference());
}

