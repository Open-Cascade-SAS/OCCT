#include <StepShape_RevolvedAreaSolid.ixx>


StepShape_RevolvedAreaSolid::StepShape_RevolvedAreaSolid ()  {}

void StepShape_RevolvedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea)
{

	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}

void StepShape_RevolvedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea,
	const Handle(StepGeom_Axis1Placement)& aAxis,
	const Standard_Real aAngle)
{
	// --- classe own fields ---
	axis = aAxis;
	angle = aAngle;
	// --- classe inherited fields ---
	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}


void StepShape_RevolvedAreaSolid::SetAxis(const Handle(StepGeom_Axis1Placement)& aAxis)
{
	axis = aAxis;
}

Handle(StepGeom_Axis1Placement) StepShape_RevolvedAreaSolid::Axis() const
{
	return axis;
}

void StepShape_RevolvedAreaSolid::SetAngle(const Standard_Real aAngle)
{
	angle = aAngle;
}

Standard_Real StepShape_RevolvedAreaSolid::Angle() const
{
	return angle;
}
