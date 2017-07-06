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


#include <Standard_Type.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_DateTimeSelect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApprovalDateTime,Standard_Transient)

StepBasic_ApprovalDateTime::StepBasic_ApprovalDateTime ()  {  }

void  StepBasic_ApprovalDateTime::Init (const StepBasic_DateTimeSelect& aDateTime, const Handle(StepBasic_Approval)& aDatedApproval)
{
  theDateTime = aDateTime;
  theDatedApproval = aDatedApproval;
}

void  StepBasic_ApprovalDateTime::SetDateTime (const StepBasic_DateTimeSelect& aDateTime)
{  theDateTime = aDateTime;  }

StepBasic_DateTimeSelect  StepBasic_ApprovalDateTime::DateTime () const
{  return theDateTime;  }

void  StepBasic_ApprovalDateTime::SetDatedApproval (const Handle(StepBasic_Approval)& aDatedApproval)
{  theDatedApproval = aDatedApproval;  }

Handle(StepBasic_Approval)  StepBasic_ApprovalDateTime::DatedApproval () const
{  return theDatedApproval;  }
