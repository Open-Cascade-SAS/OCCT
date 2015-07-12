// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWThermodynamicTemperatureUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
