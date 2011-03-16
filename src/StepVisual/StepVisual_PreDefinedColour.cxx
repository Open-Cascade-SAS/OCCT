#include <StepVisual_PreDefinedColour.ixx>


StepVisual_PreDefinedColour::StepVisual_PreDefinedColour ()  
{
  myItem = new StepVisual_PreDefinedItem;
}

void StepVisual_PreDefinedColour::SetPreDefinedItem (const Handle(StepVisual_PreDefinedItem) &item)  
{
  myItem = item;
}

const Handle(StepVisual_PreDefinedItem) &StepVisual_PreDefinedColour::GetPreDefinedItem () const 
{
  return myItem;
}
