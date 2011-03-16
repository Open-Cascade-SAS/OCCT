#include <StepVisual_PresentationSize.ixx>


StepVisual_PresentationSize::StepVisual_PresentationSize ()  {}

void StepVisual_PresentationSize::Init(
	const StepVisual_PresentationSizeAssignmentSelect& aUnit,
	const Handle(StepVisual_PlanarBox)& aSize)
{
	// --- classe own fields ---
	unit = aUnit;
	size = aSize;
}


void StepVisual_PresentationSize::SetUnit(const StepVisual_PresentationSizeAssignmentSelect& aUnit)
{
	unit = aUnit;
}

StepVisual_PresentationSizeAssignmentSelect StepVisual_PresentationSize::Unit() const
{
	return unit;
}

void StepVisual_PresentationSize::SetSize(const Handle(StepVisual_PlanarBox)& aSize)
{
	size = aSize;
}

Handle(StepVisual_PlanarBox) StepVisual_PresentationSize::Size() const
{
	return size;
}
