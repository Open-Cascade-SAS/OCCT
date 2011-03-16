// File:	RWStepBasic_RWContractAssignment.cxx
// Created:	Fri Nov 26 16:26:37 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWContractAssignment.ixx>

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
