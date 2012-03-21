// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

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
