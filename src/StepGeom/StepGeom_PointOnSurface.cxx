#include <StepGeom_PointOnSurface.ixx>


StepGeom_PointOnSurface::StepGeom_PointOnSurface ()  {}

void StepGeom_PointOnSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_PointOnSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Standard_Real aPointParameterU,
	const Standard_Real aPointParameterV)
{
	// --- classe own fields ---
	basisSurface = aBasisSurface;
	pointParameterU = aPointParameterU;
	pointParameterV = aPointParameterV;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_PointOnSurface::SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface)
{
	basisSurface = aBasisSurface;
}

Handle(StepGeom_Surface) StepGeom_PointOnSurface::BasisSurface() const
{
	return basisSurface;
}

void StepGeom_PointOnSurface::SetPointParameterU(const Standard_Real aPointParameterU)
{
	pointParameterU = aPointParameterU;
}

Standard_Real StepGeom_PointOnSurface::PointParameterU() const
{
	return pointParameterU;
}

void StepGeom_PointOnSurface::SetPointParameterV(const Standard_Real aPointParameterV)
{
	pointParameterV = aPointParameterV;
}

Standard_Real StepGeom_PointOnSurface::PointParameterV() const
{
	return pointParameterV;
}
