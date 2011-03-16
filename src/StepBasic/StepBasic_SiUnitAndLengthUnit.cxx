#include <StepBasic_SiUnitAndLengthUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_LengthUnit.hxx>


StepBasic_SiUnitAndLengthUnit::StepBasic_SiUnitAndLengthUnit ()
{
}

void StepBasic_SiUnitAndLengthUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnitAndLengthUnit::Init(const Standard_Boolean hasAprefix,
					 const StepBasic_SiPrefix aPrefix,
					 const StepBasic_SiUnitName aName)
{
  // --- classe inherited fields ---
  // --- ANDOR componant fields ---
  lengthUnit = new StepBasic_LengthUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  lengthUnit->Init(aDimensions);

  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
}


void StepBasic_SiUnitAndLengthUnit::SetLengthUnit(const Handle(StepBasic_LengthUnit)& aLengthUnit)
{
  lengthUnit = aLengthUnit;
}

Handle(StepBasic_LengthUnit) StepBasic_SiUnitAndLengthUnit::LengthUnit() const
{
  return lengthUnit;
}


