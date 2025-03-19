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

#ifndef _StepBasic_ContractAssignment_HeaderFile
#define _StepBasic_ContractAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Contract;

class StepBasic_ContractAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_ContractAssignment, Standard_Transient)

//! Representation of STEP entity ContractAssignment
class StepBasic_ContractAssignment : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_ContractAssignment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepBasic_Contract)& aAssignedContract);

  //! Returns field AssignedContract
  Standard_EXPORT Handle(StepBasic_Contract) AssignedContract() const;

  //! Set field AssignedContract
  Standard_EXPORT void SetAssignedContract(const Handle(StepBasic_Contract)& AssignedContract);

  DEFINE_STANDARD_RTTIEXT(StepBasic_ContractAssignment, Standard_Transient)

protected:
private:
  Handle(StepBasic_Contract) theAssignedContract;
};

#endif // _StepBasic_ContractAssignment_HeaderFile
