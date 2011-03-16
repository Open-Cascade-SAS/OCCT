#include <StepBasic_ConversionBasedUnit.ixx>


StepBasic_ConversionBasedUnit::StepBasic_ConversionBasedUnit ()  {}

void StepBasic_ConversionBasedUnit::Init(
	const Handle(StepBasic_DimensionalExponents)& aDimensions)
{

	StepBasic_NamedUnit::Init(aDimensions);
}

void StepBasic_ConversionBasedUnit::Init(
	const Handle(StepBasic_DimensionalExponents)& aDimensions,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
	// --- classe own fields ---
	name = aName;
	conversionFactor = aConversionFactor;
	// --- classe inherited fields ---
	StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnit::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_ConversionBasedUnit::Name() const
{
	return name;
}

void StepBasic_ConversionBasedUnit::SetConversionFactor(const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
	conversionFactor = aConversionFactor;
}

Handle(StepBasic_MeasureWithUnit) StepBasic_ConversionBasedUnit::ConversionFactor() const
{
	return conversionFactor;
}
