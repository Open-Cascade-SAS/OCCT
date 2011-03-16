#include <StepGeom_OffsetCurve3d.ixx>


StepGeom_OffsetCurve3d::StepGeom_OffsetCurve3d ()  {}

void StepGeom_OffsetCurve3d::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_OffsetCurve3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aBasisCurve,
	const Standard_Real aDistance,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	basisCurve = aBasisCurve;
	distance = aDistance;
	selfIntersect = aSelfIntersect;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_OffsetCurve3d::SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve)
{
	basisCurve = aBasisCurve;
}

Handle(StepGeom_Curve) StepGeom_OffsetCurve3d::BasisCurve() const
{
	return basisCurve;
}

void StepGeom_OffsetCurve3d::SetDistance(const Standard_Real aDistance)
{
	distance = aDistance;
}

Standard_Real StepGeom_OffsetCurve3d::Distance() const
{
	return distance;
}

void StepGeom_OffsetCurve3d::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_OffsetCurve3d::SelfIntersect() const
{
	return selfIntersect;
}

void StepGeom_OffsetCurve3d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
}

Handle(StepGeom_Direction) StepGeom_OffsetCurve3d::RefDirection() const
{
	return refDirection;
}
