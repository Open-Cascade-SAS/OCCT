// File:	StepBasic_ConversionBasedUnitAndVolumeUnit.cxx
// Created:	Tue Oct 12 13:26:11 1999
// Author:	data exchange team
//		<det@friendox>


#include <StepBasic_ConversionBasedUnitAndVolumeUnit.ixx>

StepBasic_ConversionBasedUnitAndVolumeUnit::StepBasic_ConversionBasedUnitAndVolumeUnit()
{
}

void StepBasic_ConversionBasedUnitAndVolumeUnit::SetVolumeUnit(const Handle(StepBasic_VolumeUnit)& aVolumeUnit)
{
  volumeUnit = aVolumeUnit;
}

Handle(StepBasic_VolumeUnit) StepBasic_ConversionBasedUnitAndVolumeUnit::VolumeUnit() const
{
  return volumeUnit;
}

