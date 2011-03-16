#include <StepBasic_SiUnitAndPlaneAngleUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_PlaneAngleUnit.hxx>


StepBasic_SiUnitAndPlaneAngleUnit::StepBasic_SiUnitAndPlaneAngleUnit ()
{
}

void StepBasic_SiUnitAndPlaneAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnitAndPlaneAngleUnit::Init(const Standard_Boolean hasAprefix,
					     const StepBasic_SiPrefix aPrefix,
					     const StepBasic_SiUnitName aName)
{
  // --- classe inherited fields ---
  // --- ANDOR componant fields ---
  planeAngleUnit = new StepBasic_PlaneAngleUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  planeAngleUnit->Init(aDimensions);

  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
}


void StepBasic_SiUnitAndPlaneAngleUnit::SetPlaneAngleUnit(const Handle(StepBasic_PlaneAngleUnit)& aPlaneAngleUnit)
{
  planeAngleUnit = aPlaneAngleUnit;
}

Handle(StepBasic_PlaneAngleUnit) StepBasic_SiUnitAndPlaneAngleUnit::PlaneAngleUnit() const
{
  return planeAngleUnit;
}
