
#include <RWStepVisual_RWPresentationSet.ixx>


RWStepVisual_RWPresentationSet::RWStepVisual_RWPresentationSet () {}

void RWStepVisual_RWPresentationSet::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepVisual_PresentationSet)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,0,ach,"presentation_set")) return;
  
  //--- Initialisation of the read entity ---
  
}
void RWStepVisual_RWPresentationSet::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepVisual_PresentationSet)& ent) const
{
}

