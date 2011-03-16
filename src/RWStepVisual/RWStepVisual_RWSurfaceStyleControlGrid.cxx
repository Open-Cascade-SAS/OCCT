
#include <RWStepVisual_RWSurfaceStyleControlGrid.ixx>
#include <StepVisual_CurveStyle.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_SurfaceStyleControlGrid.hxx>


RWStepVisual_RWSurfaceStyleControlGrid::RWStepVisual_RWSurfaceStyleControlGrid () {}

void RWStepVisual_RWSurfaceStyleControlGrid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_SurfaceStyleControlGrid)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"surface_style_control_grid")) return;

	// --- own field : styleOfControlGrid ---

	Handle(StepVisual_CurveStyle) aStyleOfControlGrid;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"style_of_control_grid", ach, STANDARD_TYPE(StepVisual_CurveStyle), aStyleOfControlGrid);

	//--- Initialisation of the read entity ---


	ent->Init(aStyleOfControlGrid);
}


void RWStepVisual_RWSurfaceStyleControlGrid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_SurfaceStyleControlGrid)& ent) const
{

	// --- own field : styleOfControlGrid ---

	SW.Send(ent->StyleOfControlGrid());
}


void RWStepVisual_RWSurfaceStyleControlGrid::Share(const Handle(StepVisual_SurfaceStyleControlGrid)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->StyleOfControlGrid());
}

