// File:	StepAP203_CcDesignContract.cxx
// Created:	Fri Nov 26 16:26:31 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignContract.ixx>

//=======================================================================
//function : StepAP203_CcDesignContract
//purpose  : 
//=======================================================================

StepAP203_CcDesignContract::StepAP203_CcDesignContract ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignContract::Init (const Handle(StepBasic_Contract) &aContractAssignment_AssignedContract,
                                       const Handle(StepAP203_HArray1OfContractedItem) &aItems)
{
  StepBasic_ContractAssignment::Init(aContractAssignment_AssignedContract);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfContractedItem) StepAP203_CcDesignContract::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignContract::SetItems (const Handle(StepAP203_HArray1OfContractedItem) &aItems)
{
  theItems = aItems;
}
