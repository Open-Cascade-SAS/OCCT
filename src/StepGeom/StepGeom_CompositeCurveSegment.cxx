#include <StepGeom_CompositeCurveSegment.ixx>


StepGeom_CompositeCurveSegment::StepGeom_CompositeCurveSegment ()  {}

void StepGeom_CompositeCurveSegment::Init(
	const StepGeom_TransitionCode aTransition,
	const Standard_Boolean aSameSense,
	const Handle(StepGeom_Curve)& aParentCurve)
{
	// --- classe own fields ---
	transition = aTransition;
	sameSense = aSameSense;
	parentCurve = aParentCurve;
}


void StepGeom_CompositeCurveSegment::SetTransition(const StepGeom_TransitionCode aTransition)
{
	transition = aTransition;
}

StepGeom_TransitionCode StepGeom_CompositeCurveSegment::Transition() const
{
	return transition;
}

void StepGeom_CompositeCurveSegment::SetSameSense(const Standard_Boolean aSameSense)
{
	sameSense = aSameSense;
}

Standard_Boolean StepGeom_CompositeCurveSegment::SameSense() const
{
	return sameSense;
}

void StepGeom_CompositeCurveSegment::SetParentCurve(const Handle(StepGeom_Curve)& aParentCurve)
{
	parentCurve = aParentCurve;
}

Handle(StepGeom_Curve) StepGeom_CompositeCurveSegment::ParentCurve() const
{
	return parentCurve;
}
