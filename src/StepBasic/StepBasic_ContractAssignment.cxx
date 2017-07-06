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
#include <StepBasic_Contract.hxx>
#include <StepBasic_ContractAssignment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ContractAssignment,Standard_Transient)

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
