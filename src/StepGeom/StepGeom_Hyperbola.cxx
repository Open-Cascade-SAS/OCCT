#include <StepGeom_Hyperbola.ixx>


StepGeom_Hyperbola::StepGeom_Hyperbola ()  {}

void StepGeom_Hyperbola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{

	StepGeom_Conic::Init(aName, aPosition);
}

void StepGeom_Hyperbola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aSemiAxis,
	const Standard_Real aSemiImagAxis)
{
	// --- classe own fields ---
	semiAxis = aSemiAxis;
	semiImagAxis = aSemiImagAxis;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Hyperbola::SetSemiAxis(const Standard_Real aSemiAxis)
{
	semiAxis = aSemiAxis;
}

Standard_Real StepGeom_Hyperbola::SemiAxis() const
{
	return semiAxis;
}

void StepGeom_Hyperbola::SetSemiImagAxis(const Standard_Real aSemiImagAxis)
{
	semiImagAxis = aSemiImagAxis;
}

Standard_Real StepGeom_Hyperbola::SemiImagAxis() const
{
	return semiImagAxis;
}
