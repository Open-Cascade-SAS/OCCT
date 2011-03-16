#include <StepBasic_ConversionBasedUnitAndLengthUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_LengthUnit.hxx>


StepBasic_ConversionBasedUnitAndLengthUnit::StepBasic_ConversionBasedUnitAndLengthUnit ()  {}

void StepBasic_ConversionBasedUnitAndLengthUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndLengthUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
						      const Handle(TCollection_HAsciiString)& aName,
						      const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
  
  // --- ANDOR componant fields ---
  lengthUnit = new StepBasic_LengthUnit();
  lengthUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndLengthUnit::SetLengthUnit(const Handle(StepBasic_LengthUnit)& aLengthUnit)
{
  lengthUnit = aLengthUnit;
}

Handle(StepBasic_LengthUnit) StepBasic_ConversionBasedUnitAndLengthUnit::LengthUnit() const
{
  return lengthUnit;
}

