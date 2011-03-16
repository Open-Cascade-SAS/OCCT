#include <StepBasic_ConversionBasedUnitAndTimeUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_TimeUnit.hxx>


StepBasic_ConversionBasedUnitAndTimeUnit::StepBasic_ConversionBasedUnitAndTimeUnit ()
{
}

void StepBasic_ConversionBasedUnitAndTimeUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndTimeUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
						    const Handle(TCollection_HAsciiString)& aName,
						    const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
  
  // --- ANDOR componant fields ---
  timeUnit = new StepBasic_TimeUnit();
  timeUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndTimeUnit::SetTimeUnit(const Handle(StepBasic_TimeUnit)& aTimeUnit)
{
  timeUnit = aTimeUnit;
}

Handle(StepBasic_TimeUnit) StepBasic_ConversionBasedUnitAndTimeUnit::TimeUnit() const
{
  return timeUnit;
}

