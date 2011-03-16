
#include <RWStepVisual_RWColour.ixx>


RWStepVisual_RWColour::RWStepVisual_RWColour () {}

void RWStepVisual_RWColour::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_Colour)& ent) const
{
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,0,ach,"colour")) return;
  
  //--- Initialisation of the read entity ---
}

void RWStepVisual_RWColour::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_Colour)& ent) const
{
}
