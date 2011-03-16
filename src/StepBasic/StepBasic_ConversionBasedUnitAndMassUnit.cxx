#include <StepBasic_ConversionBasedUnitAndMassUnit.ixx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_MassUnit.hxx>


//=======================================================================
//function : StepBasic_ConversionBasedUnitAndMassUnit
//purpose  : 
//=======================================================================

StepBasic_ConversionBasedUnitAndMassUnit::StepBasic_ConversionBasedUnitAndMassUnit ()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ConversionBasedUnitAndMassUnit::Init
  (const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ConversionBasedUnitAndMassUnit::Init
  (const Handle(StepBasic_DimensionalExponents)& aDimensions,
   const Handle(TCollection_HAsciiString)& aName,
   const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
  
  // --- ANDOR componant fields ---
  massUnit = new StepBasic_MassUnit();
  massUnit->Init(aDimensions);
}


//=======================================================================
//function : SetMassUnit
//purpose  : 
//=======================================================================

void StepBasic_ConversionBasedUnitAndMassUnit::SetMassUnit
  (const Handle(StepBasic_MassUnit)& aMassUnit)
{
  massUnit = aMassUnit;
}


//=======================================================================
//function : MassUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_MassUnit) StepBasic_ConversionBasedUnitAndMassUnit::MassUnit() const
{
  return massUnit;
}

