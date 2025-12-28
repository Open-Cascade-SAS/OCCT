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

#ifndef _StepAP203_CcDesignContract_HeaderFile
#define _StepAP203_CcDesignContract_HeaderFile

#include <Standard.hxx>

#include <StepAP203_ContractedItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_ContractAssignment.hxx>
class StepBasic_Contract;

//! Representation of STEP entity CcDesignContract
class StepAP203_CcDesignContract : public StepBasic_ContractAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignContract();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<StepBasic_Contract>& aContractAssignment_AssignedContract,
                            const occ::handle<NCollection_HArray1<StepAP203_ContractedItem>>& aItems);

  //! Returns field Items
  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP203_ContractedItem>> Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP203_ContractedItem>>& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignContract, StepBasic_ContractAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP203_ContractedItem>> theItems;
};

#endif // _StepAP203_CcDesignContract_HeaderFile
