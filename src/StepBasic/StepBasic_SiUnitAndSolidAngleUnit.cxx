#include <StepBasic_SiUnitAndSolidAngleUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>


StepBasic_SiUnitAndSolidAngleUnit::StepBasic_SiUnitAndSolidAngleUnit ()
{
}

void StepBasic_SiUnitAndSolidAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnitAndSolidAngleUnit::Init(const Standard_Boolean hasAprefix,
					     const StepBasic_SiPrefix aPrefix,
					     const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
    
  // --- ANDOR componant fields ---
  solidAngleUnit = new StepBasic_SolidAngleUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  solidAngleUnit->Init(aDimensions);
}


void StepBasic_SiUnitAndSolidAngleUnit::SetSolidAngleUnit(const Handle(StepBasic_SolidAngleUnit)& aSolidAngleUnit)
{
  solidAngleUnit = aSolidAngleUnit;
}

Handle(StepBasic_SolidAngleUnit) StepBasic_SiUnitAndSolidAngleUnit::SolidAngleUnit() const
{
  return solidAngleUnit;
}

