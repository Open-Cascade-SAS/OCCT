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

#ifndef _StepAP203_CcDesignApproval_HeaderFile
#define _StepAP203_CcDesignApproval_HeaderFile

#include <Standard.hxx>

#include <StepAP203_ApprovedItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_ApprovalAssignment.hxx>
class StepBasic_Approval;

//! Representation of STEP entity CcDesignApproval
class StepAP203_CcDesignApproval : public StepBasic_ApprovalAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignApproval();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepBasic_Approval>& aApprovalAssignment_AssignedApproval,
    const occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>>& aItems);

  //! Returns field Items
  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>> Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(
    const occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>>& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignApproval, StepBasic_ApprovalAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP203_ApprovedItem>> theItems;
};

#endif // _StepAP203_CcDesignApproval_HeaderFile
