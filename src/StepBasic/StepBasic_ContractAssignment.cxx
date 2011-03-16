// File:	StepBasic_ContractAssignment.cxx
// Created:	Fri Nov 26 16:26:37 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ContractAssignment.ixx>

//=======================================================================
//function : StepBasic_ContractAssignment
//purpose  : 
//=======================================================================

StepBasic_ContractAssignment::StepBasic_ContractAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ContractAssignment::Init (const Handle(StepBasic_Contract) &aAssignedContract)
{

  theAssignedContract = aAssignedContract;
}

//=======================================================================
//function : AssignedContract
//purpose  : 
//=======================================================================

Handle(StepBasic_Contract) StepBasic_ContractAssignment::AssignedContract () const
{
  return theAssignedContract;
}

//=======================================================================
//function : SetAssignedContract
//purpose  : 
//=======================================================================

void StepBasic_ContractAssignment::SetAssignedContract (const Handle(StepBasic_Contract) &aAssignedContract)
{
  theAssignedContract = aAssignedContract;
}
