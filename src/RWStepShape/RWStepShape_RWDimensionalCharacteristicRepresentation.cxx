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

#include <RWStepShape_RWDimensionalCharacteristicRepresentation.ixx>

//=======================================================================
//function : RWStepShape_RWDimensionalCharacteristicRepresentation
//purpose  : 
//=======================================================================

RWStepShape_RWDimensionalCharacteristicRepresentation::RWStepShape_RWDimensionalCharacteristicRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalCharacteristicRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                      const Standard_Integer num,
                                                                      Handle(Interface_Check)& ach,
                                                                      const Handle(StepShape_DimensionalCharacteristicRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"dimensional_characteristic_representation") ) return;

  // Own fields of DimensionalCharacteristicRepresentation

  StepShape_DimensionalCharacteristic aDimension;
  data->ReadEntity (num, 1, "dimension", ach, aDimension);

  Handle(StepShape_ShapeDimensionRepresentation) aRepresentation;
  data->ReadEntity (num, 2, "representation", ach, STANDARD_TYPE(StepShape_ShapeDimensionRepresentation), aRepresentation);

  // Initialize entity
  ent->Init(aDimension,
            aRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalCharacteristicRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                       const Handle(StepShape_DimensionalCharacteristicRepresentation) &ent) const
{

  // Own fields of DimensionalCharacteristicRepresentation

  SW.Send (ent->Dimension().Value());

  SW.Send (ent->Representation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalCharacteristicRepresentation::Share (const Handle(StepShape_DimensionalCharacteristicRepresentation) &ent,
                                                                   Interface_EntityIterator& iter) const
{

  // Own fields of DimensionalCharacteristicRepresentation

  iter.AddItem (ent->Dimension().Value());

  iter.AddItem (ent->Representation());
}
