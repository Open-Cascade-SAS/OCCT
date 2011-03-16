// File:	StepBasic_ConversionBasedUnitAndAreaUnit.cxx
// Created:	Tue Oct 12 13:20:03 1999
// Author:	data exchange team
//		<det@friendox>


#include <StepBasic_ConversionBasedUnitAndAreaUnit.ixx>

StepBasic_ConversionBasedUnitAndAreaUnit::StepBasic_ConversionBasedUnitAndAreaUnit()
{
}

void StepBasic_ConversionBasedUnitAndAreaUnit::SetAreaUnit(const Handle(StepBasic_AreaUnit)& anAreaUnit)
{
  areaUnit = anAreaUnit;
}

Handle(StepBasic_AreaUnit) StepBasic_ConversionBasedUnitAndAreaUnit::AreaUnit() const
{
  return areaUnit;
}
