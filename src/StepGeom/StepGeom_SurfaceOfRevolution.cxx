#include <StepGeom_SurfaceOfRevolution.ixx>


StepGeom_SurfaceOfRevolution::StepGeom_SurfaceOfRevolution ()  {}

void StepGeom_SurfaceOfRevolution::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve)
{

	StepGeom_SweptSurface::Init(aName, aSweptCurve);
}

void StepGeom_SurfaceOfRevolution::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve,
	const Handle(StepGeom_Axis1Placement)& aAxisPosition)
{
	// --- classe own fields ---
	axisPosition = aAxisPosition;
	// --- classe inherited fields ---
	StepGeom_SweptSurface::Init(aName, aSweptCurve);
}


void StepGeom_SurfaceOfRevolution::SetAxisPosition(const Handle(StepGeom_Axis1Placement)& aAxisPosition)
{
	axisPosition = aAxisPosition;
}

Handle(StepGeom_Axis1Placement) StepGeom_SurfaceOfRevolution::AxisPosition() const
{
	return axisPosition;
}
