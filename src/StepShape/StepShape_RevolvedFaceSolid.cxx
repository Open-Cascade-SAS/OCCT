#include <StepShape_RevolvedFaceSolid.ixx>


StepShape_RevolvedFaceSolid::StepShape_RevolvedFaceSolid ()  {}

void StepShape_RevolvedFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea)
{

	StepShape_SweptFaceSolid::Init(aName, aSweptArea);
}

void StepShape_RevolvedFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea,
	const Handle(StepGeom_Axis1Placement)& aAxis,
	const Standard_Real aAngle)
{
	// --- classe own fields ---
	axis = aAxis;
	angle = aAngle;
	// --- classe inherited fields ---
	StepShape_SweptFaceSolid::Init(aName, aSweptArea);
}


void StepShape_RevolvedFaceSolid::SetAxis(const Handle(StepGeom_Axis1Placement)& aAxis)
{
	axis = aAxis;
}

Handle(StepGeom_Axis1Placement) StepShape_RevolvedFaceSolid::Axis() const
{
	return axis;
}

void StepShape_RevolvedFaceSolid::SetAngle(const Standard_Real aAngle)
{
	angle = aAngle;
}

Standard_Real StepShape_RevolvedFaceSolid::Angle() const
{
	return angle;
}
