#include <StepVisual_AreaInSet.ixx>


StepVisual_AreaInSet::StepVisual_AreaInSet ()  {}

void StepVisual_AreaInSet::Init(
	const Handle(StepVisual_PresentationArea)& aArea,
	const Handle(StepVisual_PresentationSet)& aInSet)
{
	// --- classe own fields ---
	area = aArea;
	inSet = aInSet;
}


void StepVisual_AreaInSet::SetArea(const Handle(StepVisual_PresentationArea)& aArea)
{
	area = aArea;
}

Handle(StepVisual_PresentationArea) StepVisual_AreaInSet::Area() const
{
	return area;
}

void StepVisual_AreaInSet::SetInSet(const Handle(StepVisual_PresentationSet)& aInSet)
{
	inSet = aInSet;
}

Handle(StepVisual_PresentationSet) StepVisual_AreaInSet::InSet() const
{
	return inSet;
}
