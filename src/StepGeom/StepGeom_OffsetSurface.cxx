#include <StepGeom_OffsetSurface.ixx>


StepGeom_OffsetSurface::StepGeom_OffsetSurface ()  {}

void StepGeom_OffsetSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_OffsetSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Standard_Real aDistance,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	basisSurface = aBasisSurface;
	distance = aDistance;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_OffsetSurface::SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface)
{
	basisSurface = aBasisSurface;
}

Handle(StepGeom_Surface) StepGeom_OffsetSurface::BasisSurface() const
{
	return basisSurface;
}

void StepGeom_OffsetSurface::SetDistance(const Standard_Real aDistance)
{
	distance = aDistance;
}

Standard_Real StepGeom_OffsetSurface::Distance() const
{
	return distance;
}

void StepGeom_OffsetSurface::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_OffsetSurface::SelfIntersect() const
{
	return selfIntersect;
}
