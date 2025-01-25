// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepBasic_ApprovalDateTime_HeaderFile
#define _StepBasic_ApprovalDateTime_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_DateTimeSelect.hxx>
#include <Standard_Transient.hxx>
class StepBasic_Approval;

class StepBasic_ApprovalDateTime;
DEFINE_STANDARD_HANDLE(StepBasic_ApprovalDateTime, Standard_Transient)

//! Added from StepBasic Rev2 to Rev4
class StepBasic_ApprovalDateTime : public Standard_Transient
{

public:
  Standard_EXPORT StepBasic_ApprovalDateTime();

  Standard_EXPORT void Init(const StepBasic_DateTimeSelect&   aDateTime,
                            const Handle(StepBasic_Approval)& aDatedApproval);

  Standard_EXPORT void SetDateTime(const StepBasic_DateTimeSelect& aDateTime);

  Standard_EXPORT StepBasic_DateTimeSelect DateTime() const;

  Standard_EXPORT void SetDatedApproval(const Handle(StepBasic_Approval)& aDatedApproval);

  Standard_EXPORT Handle(StepBasic_Approval) DatedApproval() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ApprovalDateTime, Standard_Transient)

protected:
private:
  StepBasic_DateTimeSelect   theDateTime;
  Handle(StepBasic_Approval) theDatedApproval;
};

#endif // _StepBasic_ApprovalDateTime_HeaderFile
