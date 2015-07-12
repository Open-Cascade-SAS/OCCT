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
#include <RWStepBasic_RWContractAssignment.hxx>
#include <StepBasic_Contract.hxx>
#include <StepBasic_ContractAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWContractAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWContractAssignment::RWStepBasic_RWContractAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepBasic_ContractAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"contract_assignment") ) return;

  // Own fields of ContractAssignment

  Handle(StepBasic_Contract) aAssignedContract;
  data->ReadEntity (num, 1, "assigned_contract", ach, STANDARD_TYPE(StepBasic_Contract), aAssignedContract);

  // Initialize entity
  ent->Init(aAssignedContract);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractAssignment::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepBasic_ContractAssignment) &ent) const
{

  // Own fields of ContractAssignment

  SW.Send (ent->AssignedContract());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWContractAssignment::Share (const Handle(StepBasic_ContractAssignment) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of ContractAssignment

  iter.AddItem (ent->AssignedContract());
}
