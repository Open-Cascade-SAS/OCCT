#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>


StepBasic_ConversionBasedUnitAndSolidAngleUnit::StepBasic_ConversionBasedUnitAndSolidAngleUnit ()
{
}

void StepBasic_ConversionBasedUnitAndSolidAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndSolidAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
							  const Handle(TCollection_HAsciiString)& aName,
							  const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);

  // --- ANDOR componant fields ---
  solidAngleUnit = new StepBasic_SolidAngleUnit();
  solidAngleUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndSolidAngleUnit::SetSolidAngleUnit(const Handle(StepBasic_SolidAngleUnit)& aSolidAngleUnit)
{
  solidAngleUnit = aSolidAngleUnit;
}

Handle(StepBasic_SolidAngleUnit) StepBasic_ConversionBasedUnitAndSolidAngleUnit::SolidAngleUnit() const
{
  return solidAngleUnit;
}

