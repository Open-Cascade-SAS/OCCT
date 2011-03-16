// File:	StepShape_DimensionalCharacteristicRepresentation.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepShape_DimensionalCharacteristicRepresentation.ixx>

//=======================================================================
//function : StepShape_DimensionalCharacteristicRepresentation
//purpose  : 
//=======================================================================

StepShape_DimensionalCharacteristicRepresentation::StepShape_DimensionalCharacteristicRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalCharacteristicRepresentation::Init (const StepShape_DimensionalCharacteristic &aDimension,
                                                              const Handle(StepShape_ShapeDimensionRepresentation) &aRepresentation)
{

  theDimension = aDimension;

  theRepresentation = aRepresentation;
}

//=======================================================================
//function : Dimension
//purpose  : 
//=======================================================================

StepShape_DimensionalCharacteristic StepShape_DimensionalCharacteristicRepresentation::Dimension () const
{
  return theDimension;
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void StepShape_DimensionalCharacteristicRepresentation::SetDimension (const StepShape_DimensionalCharacteristic &aDimension)
{
  theDimension = aDimension;
}

//=======================================================================
//function : Representation
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeDimensionRepresentation) StepShape_DimensionalCharacteristicRepresentation::Representation () const
{
  return theRepresentation;
}

//=======================================================================
//function : SetRepresentation
//purpose  : 
//=======================================================================

void StepShape_DimensionalCharacteristicRepresentation::SetRepresentation (const Handle(StepShape_ShapeDimensionRepresentation) &aRepresentation)
{
  theRepresentation = aRepresentation;
}
