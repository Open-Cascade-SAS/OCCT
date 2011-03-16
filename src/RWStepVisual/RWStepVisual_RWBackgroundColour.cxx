
#include <RWStepVisual_RWBackgroundColour.ixx>
#include <StepVisual_AreaOrView.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_BackgroundColour.hxx>


RWStepVisual_RWBackgroundColour::RWStepVisual_RWBackgroundColour () {}

void RWStepVisual_RWBackgroundColour::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_BackgroundColour)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"background_colour")) return;

	// --- own field : presentation ---

	StepVisual_AreaOrView aPresentation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num,1,"presentation",ach,aPresentation);

	//--- Initialisation of the read entity ---


	ent->Init(aPresentation);
}


void RWStepVisual_RWBackgroundColour::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_BackgroundColour)& ent) const
{

	// --- own field : presentation ---

	SW.Send(ent->Presentation().Value());
}


void RWStepVisual_RWBackgroundColour::Share(const Handle(StepVisual_BackgroundColour)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Presentation().Value());
}

