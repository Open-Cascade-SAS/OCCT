// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
#include <RWStepBasic_RWContractType.hxx>
#include <StepBasic_ContractType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWContractType
//purpose  : 
//=======================================================================
RWStepBasic_RWContractType::RWStepBasic_RWContractType ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractType::ReadStep (const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(StepBasic_ContractType) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"contract_type") ) return;

  // Own fields of ContractType

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 1, "description", ach, aDescription);

  // Initialize entity
  ent->Init(aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractType::WriteStep (StepData_StepWriter& SW,
                                            const Handle(StepBasic_ContractType) &ent) const
{

  // Own fields of ContractType

  SW.Send (ent->Description());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractType::Share (const Handle(StepBasic_ContractType) &,
                                        Interface_EntityIterator&) const
{
  // Own fields of ContractType
}
