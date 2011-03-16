#include <StepBasic_ConversionBasedUnitAndRatioUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_RatioUnit.hxx>


StepBasic_ConversionBasedUnitAndRatioUnit::StepBasic_ConversionBasedUnitAndRatioUnit ()
{
}

void StepBasic_ConversionBasedUnitAndRatioUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndRatioUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
						     const Handle(TCollection_HAsciiString)& aName,
						     const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
	
  // --- ANDOR componant fields ---
  ratioUnit = new StepBasic_RatioUnit();
  ratioUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndRatioUnit::SetRatioUnit(const Handle(StepBasic_RatioUnit)& aRatioUnit)
{
  ratioUnit = aRatioUnit;
}

Handle(StepBasic_RatioUnit) StepBasic_ConversionBasedUnitAndRatioUnit::RatioUnit() const
{
  return ratioUnit;
}
