#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_PlaneAngleUnit.hxx>


StepBasic_ConversionBasedUnitAndPlaneAngleUnit::StepBasic_ConversionBasedUnitAndPlaneAngleUnit ()
{
}

void StepBasic_ConversionBasedUnitAndPlaneAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndPlaneAngleUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
							  const Handle(TCollection_HAsciiString)& aName,
							  const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
	
  // --- ANDOR componant fields ---
  planeAngleUnit = new StepBasic_PlaneAngleUnit();
  planeAngleUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndPlaneAngleUnit::SetPlaneAngleUnit(const Handle(StepBasic_PlaneAngleUnit)& aPlaneAngleUnit)
{
  planeAngleUnit = aPlaneAngleUnit;
}

Handle(StepBasic_PlaneAngleUnit) StepBasic_ConversionBasedUnitAndPlaneAngleUnit::PlaneAngleUnit() const
{
  return planeAngleUnit;
}

