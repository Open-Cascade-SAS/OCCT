#include <StepGeom_Ellipse.ixx>


StepGeom_Ellipse::StepGeom_Ellipse ()  {}

void StepGeom_Ellipse::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{

	StepGeom_Conic::Init(aName, aPosition);
}

void StepGeom_Ellipse::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aSemiAxis1,
	const Standard_Real aSemiAxis2)
{
	// --- classe own fields ---
	semiAxis1 = aSemiAxis1;
	semiAxis2 = aSemiAxis2;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Ellipse::SetSemiAxis1(const Standard_Real aSemiAxis1)
{
	semiAxis1 = aSemiAxis1;
}

Standard_Real StepGeom_Ellipse::SemiAxis1() const
{
	return semiAxis1;
}

void StepGeom_Ellipse::SetSemiAxis2(const Standard_Real aSemiAxis2)
{
	semiAxis2 = aSemiAxis2;
}

Standard_Real StepGeom_Ellipse::SemiAxis2() const
{
	return semiAxis2;
}
