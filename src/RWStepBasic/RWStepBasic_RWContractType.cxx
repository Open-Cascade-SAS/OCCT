// File:	RWStepBasic_RWContractType.cxx
// Created:	Fri Nov 26 16:26:38 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWContractType.ixx>

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

void RWStepBasic_RWContractType::Share (const Handle(StepBasic_ContractType) &ent,
                                        Interface_EntityIterator& iter) const
{

  // Own fields of ContractType
}
