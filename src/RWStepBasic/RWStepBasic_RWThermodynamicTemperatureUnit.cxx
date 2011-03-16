// File:	RWStepBasic_RWThermodynamicTemperatureUnit.cxx
// Created:	Thu Dec 12 15:38:09 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepBasic_RWThermodynamicTemperatureUnit.ixx>

//=======================================================================
//function : RWStepBasic_RWThermodynamicTemperatureUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWThermodynamicTemperatureUnit::RWStepBasic_RWThermodynamicTemperatureUnit ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWThermodynamicTemperatureUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                           const Standard_Integer num,
                                                           Handle(Interface_Check)& ach,
                                                           const Handle(StepBasic_ThermodynamicTemperatureUnit) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"thermodynamic_temperature_unit") ) return;

  // Inherited fields of NamedUnit

  Handle(StepBasic_DimensionalExponents) aNamedUnit_Dimensions;
  data->ReadEntity (num, 1, "named_unit.dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aNamedUnit_Dimensions);

  // Initialize entity
  ent->Init(aNamedUnit_Dimensions);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWThermodynamicTemperatureUnit::WriteStep (StepData_StepWriter& SW,
                                                            const Handle(StepBasic_ThermodynamicTemperatureUnit) &ent) const
{

  // Inherited fields of NamedUnit

  SW.Send (ent->StepBasic_NamedUnit::Dimensions());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWThermodynamicTemperatureUnit::Share (const Handle(StepBasic_ThermodynamicTemperatureUnit) &ent,
                                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of NamedUnit

  iter.AddItem (ent->StepBasic_NamedUnit::Dimensions());
}
