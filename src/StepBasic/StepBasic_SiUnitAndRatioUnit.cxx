#include <StepBasic_SiUnitAndRatioUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_RatioUnit.hxx>


StepBasic_SiUnitAndRatioUnit::StepBasic_SiUnitAndRatioUnit ()
{
}

void StepBasic_SiUnitAndRatioUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnitAndRatioUnit::Init(const Standard_Boolean hasAprefix,
					const StepBasic_SiPrefix aPrefix,
					const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
    
  // --- ANDOR componant fields ---
  ratioUnit = new StepBasic_RatioUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  ratioUnit->Init(aDimensions);
}

void StepBasic_SiUnitAndRatioUnit::SetRatioUnit(const Handle(StepBasic_RatioUnit)& aRatioUnit)
{
  ratioUnit = aRatioUnit;
}

Handle(StepBasic_RatioUnit) StepBasic_SiUnitAndRatioUnit::RatioUnit() const
{
  return ratioUnit;
}

