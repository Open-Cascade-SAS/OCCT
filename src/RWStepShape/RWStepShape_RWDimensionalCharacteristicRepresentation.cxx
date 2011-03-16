// File:	RWStepShape_RWDimensionalCharacteristicRepresentation.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
