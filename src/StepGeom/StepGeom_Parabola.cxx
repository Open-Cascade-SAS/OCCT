#include <StepGeom_Parabola.ixx>


StepGeom_Parabola::StepGeom_Parabola ()  {}

void StepGeom_Parabola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{

	StepGeom_Conic::Init(aName, aPosition);
}

void StepGeom_Parabola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aFocalDist)
{
	// --- classe own fields ---
	focalDist = aFocalDist;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Parabola::SetFocalDist(const Standard_Real aFocalDist)
{
	focalDist = aFocalDist;
}

Standard_Real StepGeom_Parabola::FocalDist() const
{
	return focalDist;
}
