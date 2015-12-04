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

#include <Standard_Type.hxx>
#include <StepAP203_CcDesignContract.hxx>
#include <StepBasic_Contract.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP203_CcDesignContract,StepBasic_ContractAssignment)

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
