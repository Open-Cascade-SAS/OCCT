
#include <RWStepVisual_RWSurfaceStyleFillArea.ixx>
#include <StepVisual_FillAreaStyle.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_SurfaceStyleFillArea.hxx>


RWStepVisual_RWSurfaceStyleFillArea::RWStepVisual_RWSurfaceStyleFillArea () {}

void RWStepVisual_RWSurfaceStyleFillArea::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_SurfaceStyleFillArea)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"surface_style_fill_area")) return;

	// --- own field : fillArea ---

	Handle(StepVisual_FillAreaStyle) aFillArea;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"fill_area", ach, STANDARD_TYPE(StepVisual_FillAreaStyle), aFillArea);

	//--- Initialisation of the read entity ---


	ent->Init(aFillArea);
}


void RWStepVisual_RWSurfaceStyleFillArea::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_SurfaceStyleFillArea)& ent) const
{

	// --- own field : fillArea ---

	SW.Send(ent->FillArea());
}


void RWStepVisual_RWSurfaceStyleFillArea::Share(const Handle(StepVisual_SurfaceStyleFillArea)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FillArea());
}

