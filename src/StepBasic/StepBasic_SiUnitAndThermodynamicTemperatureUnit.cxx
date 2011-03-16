#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>


//=======================================================================
//function : StepBasic_SiUnitAndThermodynamicTemperatureUnit
//purpose  : 
//=======================================================================

StepBasic_SiUnitAndThermodynamicTemperatureUnit::StepBasic_SiUnitAndThermodynamicTemperatureUnit ()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::Init(const Standard_Boolean hasAprefix,
                                                           const StepBasic_SiPrefix aPrefix,
                                                           const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
    
  // --- ANDOR componant fields ---
  thermodynamicTemperatureUnit = new StepBasic_ThermodynamicTemperatureUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  thermodynamicTemperatureUnit->Init(aDimensions);
}


//=======================================================================
//function : SetThermodynamicTemperatureUnit
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::SetThermodynamicTemperatureUnit
  (const Handle(StepBasic_ThermodynamicTemperatureUnit)& aThermodynamicTemperatureUnit)
{
  thermodynamicTemperatureUnit = aThermodynamicTemperatureUnit;
}


//=======================================================================
//function : SolidAngleUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_ThermodynamicTemperatureUnit) StepBasic_SiUnitAndThermodynamicTemperatureUnit::ThermodynamicTemperatureUnit() const
{
  return thermodynamicTemperatureUnit;
}

