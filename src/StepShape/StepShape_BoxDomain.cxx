#include <StepShape_BoxDomain.ixx>


StepShape_BoxDomain::StepShape_BoxDomain ()  {}

void StepShape_BoxDomain::Init(
	const Handle(StepGeom_CartesianPoint)& aCorner,
	const Standard_Real aXlength,
	const Standard_Real aYlength,
	const Standard_Real aZlength)
{
	// --- classe own fields ---
	corner = aCorner;
	xlength = aXlength;
	ylength = aYlength;
	zlength = aZlength;
}


void StepShape_BoxDomain::SetCorner(const Handle(StepGeom_CartesianPoint)& aCorner)
{
	corner = aCorner;
}

Handle(StepGeom_CartesianPoint) StepShape_BoxDomain::Corner() const
{
	return corner;
}

void StepShape_BoxDomain::SetXlength(const Standard_Real aXlength)
{
	xlength = aXlength;
}

Standard_Real StepShape_BoxDomain::Xlength() const
{
	return xlength;
}

void StepShape_BoxDomain::SetYlength(const Standard_Real aYlength)
{
	ylength = aYlength;
}

Standard_Real StepShape_BoxDomain::Ylength() const
{
	return ylength;
}

void StepShape_BoxDomain::SetZlength(const Standard_Real aZlength)
{
	zlength = aZlength;
}

Standard_Real StepShape_BoxDomain::Zlength() const
{
	return zlength;
}
