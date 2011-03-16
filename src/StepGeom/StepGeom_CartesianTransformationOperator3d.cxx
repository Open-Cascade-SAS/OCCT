#include <StepGeom_CartesianTransformationOperator3d.ixx>


StepGeom_CartesianTransformationOperator3d::StepGeom_CartesianTransformationOperator3d ()  {}

void StepGeom_CartesianTransformationOperator3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAaxis1,
	const Handle(StepGeom_Direction)& aAxis1,
	const Standard_Boolean hasAaxis2,
	const Handle(StepGeom_Direction)& aAxis2,
	const Handle(StepGeom_CartesianPoint)& aLocalOrigin,
	const Standard_Boolean hasAscale,
	const Standard_Real aScale)
{

	StepGeom_CartesianTransformationOperator::Init(aName, hasAaxis1, aAxis1, hasAaxis2, aAxis2, aLocalOrigin, hasAscale, aScale);
}

void StepGeom_CartesianTransformationOperator3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAaxis1,
	const Handle(StepGeom_Direction)& aAxis1,
	const Standard_Boolean hasAaxis2,
	const Handle(StepGeom_Direction)& aAxis2,
	const Handle(StepGeom_CartesianPoint)& aLocalOrigin,
	const Standard_Boolean hasAscale,
	const Standard_Real aScale,
	const Standard_Boolean hasAaxis3,
	const Handle(StepGeom_Direction)& aAxis3)
{
	// --- classe own fields ---
	hasAxis3 = hasAaxis3;
	axis3 = aAxis3;
	// --- classe inherited fields ---
	StepGeom_CartesianTransformationOperator::Init(aName, hasAaxis1, aAxis1, hasAaxis2, aAxis2, aLocalOrigin, hasAscale, aScale);
}


void StepGeom_CartesianTransformationOperator3d::SetAxis3(const Handle(StepGeom_Direction)& aAxis3)
{
	axis3 = aAxis3;
	hasAxis3 = Standard_True;
}

void StepGeom_CartesianTransformationOperator3d::UnSetAxis3()
{
	hasAxis3 = Standard_False;
	axis3.Nullify();
}

Handle(StepGeom_Direction) StepGeom_CartesianTransformationOperator3d::Axis3() const
{
	return axis3;
}

Standard_Boolean StepGeom_CartesianTransformationOperator3d::HasAxis3() const
{
	return hasAxis3;
}
