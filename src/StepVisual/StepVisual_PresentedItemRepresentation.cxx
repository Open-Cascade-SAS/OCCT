#include <StepVisual_PresentedItemRepresentation.ixx>

StepVisual_PresentedItemRepresentation::StepVisual_PresentedItemRepresentation  ()    {  }

void  StepVisual_PresentedItemRepresentation::Init
  (const StepVisual_PresentationRepresentationSelect& aPresentation,
   const Handle(StepVisual_PresentedItem)& aItem)
{  thePresentation = aPresentation;  theItem = aItem;  }

void  StepVisual_PresentedItemRepresentation::SetPresentation (const StepVisual_PresentationRepresentationSelect& aPresentation)
{  thePresentation = aPresentation;  }

StepVisual_PresentationRepresentationSelect  StepVisual_PresentedItemRepresentation::Presentation () const
{  return thePresentation;  }

void  StepVisual_PresentedItemRepresentation::SetItem (const Handle(StepVisual_PresentedItem)& aItem)
{  theItem = aItem;  }

Handle(StepVisual_PresentedItem)  StepVisual_PresentedItemRepresentation::Item () const
{  return theItem;  }
