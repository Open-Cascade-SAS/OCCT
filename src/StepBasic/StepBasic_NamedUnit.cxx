#include <StepBasic_NamedUnit.ixx>


StepBasic_NamedUnit::StepBasic_NamedUnit ()  {}

void StepBasic_NamedUnit::Init(
	const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
	// --- classe own fields ---
	dimensions = aDimensions;
}


void StepBasic_NamedUnit::SetDimensions(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
	dimensions = aDimensions;
}

Handle(StepBasic_DimensionalExponents) StepBasic_NamedUnit::Dimensions() const
{
	return dimensions;
}
