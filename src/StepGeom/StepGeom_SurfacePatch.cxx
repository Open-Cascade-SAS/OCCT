#include <StepGeom_SurfacePatch.ixx>


StepGeom_SurfacePatch::StepGeom_SurfacePatch ()  {}

void StepGeom_SurfacePatch::Init(
	const Handle(StepGeom_BoundedSurface)& aParentSurface,
	const StepGeom_TransitionCode aUTransition,
	const StepGeom_TransitionCode aVTransition,
	const Standard_Boolean aUSense,
	const Standard_Boolean aVSense)
{
	// --- classe own fields ---
	parentSurface = aParentSurface;
	uTransition = aUTransition;
	vTransition = aVTransition;
	uSense = aUSense;
	vSense = aVSense;
}


void StepGeom_SurfacePatch::SetParentSurface(const Handle(StepGeom_BoundedSurface)& aParentSurface)
{
	parentSurface = aParentSurface;
}

Handle(StepGeom_BoundedSurface) StepGeom_SurfacePatch::ParentSurface() const
{
	return parentSurface;
}

void StepGeom_SurfacePatch::SetUTransition(const StepGeom_TransitionCode aUTransition)
{
	uTransition = aUTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::UTransition() const
{
	return uTransition;
}

void StepGeom_SurfacePatch::SetVTransition(const StepGeom_TransitionCode aVTransition)
{
	vTransition = aVTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::VTransition() const
{
	return vTransition;
}

void StepGeom_SurfacePatch::SetUSense(const Standard_Boolean aUSense)
{
	uSense = aUSense;
}

Standard_Boolean StepGeom_SurfacePatch::USense() const
{
	return uSense;
}

void StepGeom_SurfacePatch::SetVSense(const Standard_Boolean aVSense)
{
	vSense = aVSense;
}

Standard_Boolean StepGeom_SurfacePatch::VSense() const
{
	return vSense;
}
