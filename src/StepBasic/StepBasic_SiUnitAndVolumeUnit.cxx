// File:	StepBasic_SiUnitAndVolumeUnit.cxx
// Created:	Tue Oct 12 13:14:38 1999
// Author:	data exchange team
//		<det@friendox>


#include <StepBasic_SiUnitAndVolumeUnit.ixx>

StepBasic_SiUnitAndVolumeUnit::StepBasic_SiUnitAndVolumeUnit()
{
}

void StepBasic_SiUnitAndVolumeUnit::SetVolumeUnit(const Handle(StepBasic_VolumeUnit)& aVolumeUnit)
{
  volumeUnit = aVolumeUnit;
}

Handle(StepBasic_VolumeUnit) StepBasic_SiUnitAndVolumeUnit::VolumeUnit() const
{
  return volumeUnit;
}

void StepBasic_SiUnitAndVolumeUnit::SetDimensions(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::SetDimensions(aDimensions);
}

Handle(StepBasic_DimensionalExponents) StepBasic_SiUnitAndVolumeUnit::Dimensions() const
{
  return StepBasic_NamedUnit::Dimensions();
}
