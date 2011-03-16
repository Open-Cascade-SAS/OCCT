
#include <RWStepVisual_RWPresentationLayerUsage.ixx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationRepresentation.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PresentationLayerUsage.hxx>


RWStepVisual_RWPresentationLayerUsage::RWStepVisual_RWPresentationLayerUsage () {}

void RWStepVisual_RWPresentationLayerUsage::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PresentationLayerUsage)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"presentation_layer_usage")) return;

	// --- own fields
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationRepresentation.hxx>
	Handle(StepVisual_PresentationLayerAssignment) pla;
	Handle(StepVisual_PresentationRepresentation)  pr;

	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"assignment", ach, STANDARD_TYPE(StepVisual_PresentationLayerAssignment), pla);
	//szv#4:S4163:12Mar99 `stat1 =` not needed
	data->ReadEntity(num, 2,"presentation", ach, STANDARD_TYPE(StepVisual_PresentationRepresentation), pr);

	//--- Initialisation of the read entity ---


	ent->Init(pla,pr);
}


void RWStepVisual_RWPresentationLayerUsage::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PresentationLayerUsage)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Assignment());
	SW.Send(ent->Presentation());
}


void RWStepVisual_RWPresentationLayerUsage::Share(const Handle(StepVisual_PresentationLayerUsage)& ent, Interface_EntityIterator& iter) const
{
	iter.AddItem(ent->Assignment());
	iter.AddItem(ent->Presentation());
}
