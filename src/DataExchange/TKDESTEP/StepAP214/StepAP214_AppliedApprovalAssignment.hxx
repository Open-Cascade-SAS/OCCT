// Created on: 1999-03-10
// Created by: data exchange team
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

#ifndef _StepAP214_AppliedApprovalAssignment_HeaderFile
#define _StepAP214_AppliedApprovalAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_ApprovalItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_ApprovalAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Approval;
class StepAP214_ApprovalItem;

class StepAP214_AppliedApprovalAssignment : public StepBasic_ApprovalAssignment
{

public:
  //! Returns a AppliedApprovalAssignment
  Standard_EXPORT StepAP214_AppliedApprovalAssignment();

  Standard_EXPORT void Init(const occ::handle<StepBasic_Approval>&              aAssignedApproval,
                            const occ::handle<NCollection_HArray1<StepAP214_ApprovalItem>>& aItems);

  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP214_ApprovalItem>>& aItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP214_ApprovalItem>> Items() const;

  Standard_EXPORT StepAP214_ApprovalItem ItemsValue(const int num) const;

  Standard_EXPORT int NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedApprovalAssignment, StepBasic_ApprovalAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP214_ApprovalItem>> items;
};

#endif // _StepAP214_AppliedApprovalAssignment_HeaderFile
