#include <StepBasic_SiUnitAndTimeUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_TimeUnit.hxx>


StepBasic_SiUnitAndTimeUnit::StepBasic_SiUnitAndTimeUnit ()  
{
}

void StepBasic_SiUnitAndTimeUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnitAndTimeUnit::Init(const Standard_Boolean hasAprefix,
				       const StepBasic_SiPrefix aPrefix,
				       const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);

  // --- ANDOR componant fields ---
  timeUnit = new StepBasic_TimeUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  timeUnit->Init(aDimensions);
}


void StepBasic_SiUnitAndTimeUnit::SetTimeUnit(const Handle(StepBasic_TimeUnit)& aTimeUnit)
{
  timeUnit = aTimeUnit;
}

Handle(StepBasic_TimeUnit) StepBasic_SiUnitAndTimeUnit::TimeUnit() const
{
  return timeUnit;
}


