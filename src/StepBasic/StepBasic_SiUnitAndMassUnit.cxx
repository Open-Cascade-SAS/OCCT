#include <StepBasic_SiUnitAndMassUnit.ixx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_MassUnit.hxx>


//=======================================================================
//function : StepBasic_SiUnitAndLengthUnit
//purpose  : 
//=======================================================================

StepBasic_SiUnitAndMassUnit::StepBasic_SiUnitAndMassUnit ()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndMassUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndMassUnit::Init(const Standard_Boolean hasAprefix,
                                       const StepBasic_SiPrefix aPrefix,
                                       const StepBasic_SiUnitName aName)
{
  // --- classe inherited fields ---
  // --- ANDOR componant fields ---
  massUnit = new StepBasic_MassUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  massUnit->Init(aDimensions);

  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
}


//=======================================================================
//function : SetMassUnit
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndMassUnit::SetMassUnit(const Handle(StepBasic_MassUnit)& aMassUnit)
{
  massUnit = aMassUnit;
}


//=======================================================================
//function : MassUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_MassUnit) StepBasic_SiUnitAndMassUnit::MassUnit() const
{
  return massUnit;
}


