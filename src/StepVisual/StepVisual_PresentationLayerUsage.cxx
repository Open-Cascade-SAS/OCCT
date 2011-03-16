#include <StepVisual_PresentationLayerUsage.ixx>

StepVisual_PresentationLayerUsage::StepVisual_PresentationLayerUsage  ()    {  }

void  StepVisual_PresentationLayerUsage::Init
  (const Handle(StepVisual_PresentationLayerAssignment)& aAssignment,
   const Handle(StepVisual_PresentationRepresentation)& aPresentation)
{  theAssignment = aAssignment;  thePresentation = aPresentation;  }

void  StepVisual_PresentationLayerUsage::SetAssignment
  (const Handle(StepVisual_PresentationLayerAssignment)& aAssignment)
{  theAssignment = aAssignment;  }

Handle(StepVisual_PresentationLayerAssignment)  StepVisual_PresentationLayerUsage::Assignment () const
{  return theAssignment;  }

void  StepVisual_PresentationLayerUsage::SetPresentation
  (const Handle(StepVisual_PresentationRepresentation)& aPresentation)
{  thePresentation = aPresentation;  }

Handle(StepVisual_PresentationRepresentation)  StepVisual_PresentationLayerUsage::Presentation () const
{  return thePresentation;  }
