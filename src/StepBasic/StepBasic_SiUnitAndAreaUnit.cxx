// File:	StepBasic_SiUnitAndAreaUnit.cxx
// Created:	Mon Oct 11 15:14:27 1999
// Author:	data exchange team
//		<det@friendox>


#include <StepBasic_SiUnitAndAreaUnit.ixx>

StepBasic_SiUnitAndAreaUnit::StepBasic_SiUnitAndAreaUnit () 
{
}

void StepBasic_SiUnitAndAreaUnit::SetAreaUnit(const Handle(StepBasic_AreaUnit)& anAreaUnit)
{
  areaUnit = anAreaUnit;
}

Handle(StepBasic_AreaUnit) StepBasic_SiUnitAndAreaUnit::AreaUnit() const
{
  return areaUnit;
}

void StepBasic_SiUnitAndAreaUnit::SetDimensions(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::SetDimensions(aDimensions);
}

Handle(StepBasic_DimensionalExponents) StepBasic_SiUnitAndAreaUnit::Dimensions() const
{
  return StepBasic_NamedUnit::Dimensions();
}
