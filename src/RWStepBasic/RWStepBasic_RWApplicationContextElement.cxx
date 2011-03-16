
#include <RWStepBasic_RWApplicationContextElement.ixx>
#include <StepBasic_ApplicationContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ApplicationContextElement.hxx>


RWStepBasic_RWApplicationContextElement::RWStepBasic_RWApplicationContextElement () {}

void RWStepBasic_RWApplicationContextElement::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApplicationContextElement)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"application_context_element")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : frameOfReference ---

	Handle(StepBasic_ApplicationContext) aFrameOfReference;
#ifdef DEB
	Standard_Boolean stat2 = 
#endif
	  data->ReadEntity(num, 2,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aFrameOfReference);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aFrameOfReference);
}


void RWStepBasic_RWApplicationContextElement::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApplicationContextElement)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : frameOfReference ---

	SW.Send(ent->FrameOfReference());
}


void RWStepBasic_RWApplicationContextElement::Share(const Handle(StepBasic_ApplicationContextElement)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FrameOfReference());
}

