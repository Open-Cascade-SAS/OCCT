#include <StepGeom_ReparametrisedCompositeCurveSegment.ixx>


StepGeom_ReparametrisedCompositeCurveSegment::StepGeom_ReparametrisedCompositeCurveSegment ()  {}

void StepGeom_ReparametrisedCompositeCurveSegment::Init(
	const StepGeom_TransitionCode aTransition,
	const Standard_Boolean aSameSense,
	const Handle(StepGeom_Curve)& aParentCurve)
{

	StepGeom_CompositeCurveSegment::Init(aTransition, aSameSense, aParentCurve);
}

void StepGeom_ReparametrisedCompositeCurveSegment::Init(
	const StepGeom_TransitionCode aTransition,
	const Standard_Boolean aSameSense,
	const Handle(StepGeom_Curve)& aParentCurve,
	const Standard_Real aParamLength)
{
	// --- classe own fields ---
	paramLength = aParamLength;
	// --- classe inherited fields ---
	StepGeom_CompositeCurveSegment::Init(aTransition, aSameSense, aParentCurve);
}


void StepGeom_ReparametrisedCompositeCurveSegment::SetParamLength(const Standard_Real aParamLength)
{
	paramLength = aParamLength;
}

Standard_Real StepGeom_ReparametrisedCompositeCurveSegment::ParamLength() const
{
	return paramLength;
}
