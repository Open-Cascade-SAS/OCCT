// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepShape_RWDimensionalCharacteristicRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>

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
