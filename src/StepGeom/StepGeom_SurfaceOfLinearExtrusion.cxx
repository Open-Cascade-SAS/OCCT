#include <StepGeom_SurfaceOfLinearExtrusion.ixx>


StepGeom_SurfaceOfLinearExtrusion::StepGeom_SurfaceOfLinearExtrusion ()  {}

void StepGeom_SurfaceOfLinearExtrusion::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve)
{

	StepGeom_SweptSurface::Init(aName, aSweptCurve);
}

void StepGeom_SurfaceOfLinearExtrusion::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve,
	const Handle(StepGeom_Vector)& aExtrusionAxis)
{
	// --- classe own fields ---
	extrusionAxis = aExtrusionAxis;
	// --- classe inherited fields ---
	StepGeom_SweptSurface::Init(aName, aSweptCurve);
}


void StepGeom_SurfaceOfLinearExtrusion::SetExtrusionAxis(const Handle(StepGeom_Vector)& aExtrusionAxis)
{
	extrusionAxis = aExtrusionAxis;
}

Handle(StepGeom_Vector) StepGeom_SurfaceOfLinearExtrusion::ExtrusionAxis() const
{
	return extrusionAxis;
}
