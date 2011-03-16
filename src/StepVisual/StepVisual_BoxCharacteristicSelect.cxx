// File:	StepVisual_BoxCharacteristicSelect.cxx
// Created:	Wed Dec  6 18:27:17 1995
// Author:	Frederic MAUPAS
//		<fma@pronox>

#include <StepVisual_BoxCharacteristicSelect.ixx>

StepVisual_BoxCharacteristicSelect::StepVisual_BoxCharacteristicSelect() {}

void StepVisual_BoxCharacteristicSelect::SetTypeOfContent(const Standard_Integer aType)
{
  theTypeOfContent = aType;
}

Standard_Integer StepVisual_BoxCharacteristicSelect::TypeOfContent() const 
{
  return theTypeOfContent;
}

Standard_Real StepVisual_BoxCharacteristicSelect::RealValue() const
{
  return theRealValue;
}

void StepVisual_BoxCharacteristicSelect::SetRealValue(const Standard_Real aValue)
{
  theRealValue = aValue;
}
