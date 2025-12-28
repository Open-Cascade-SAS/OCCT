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

#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApprovalRelationship, Standard_Transient)

StepBasic_ApprovalRelationship::StepBasic_ApprovalRelationship() {}

void StepBasic_ApprovalRelationship::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                          const occ::handle<TCollection_HAsciiString>& aDescription,
                                          const occ::handle<StepBasic_Approval>&       aRelatingApproval,
                                          const occ::handle<StepBasic_Approval>&       aRelatedApproval)
{
  // --- classe own fields ---
  name             = aName;
  description      = aDescription;
  relatingApproval = aRelatingApproval;
  relatedApproval  = aRelatedApproval;
}

void StepBasic_ApprovalRelationship::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepBasic_ApprovalRelationship::Name() const
{
  return name;
}

void StepBasic_ApprovalRelationship::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  description = aDescription;
}

occ::handle<TCollection_HAsciiString> StepBasic_ApprovalRelationship::Description() const
{
  return description;
}

void StepBasic_ApprovalRelationship::SetRelatingApproval(
  const occ::handle<StepBasic_Approval>& aRelatingApproval)
{
  relatingApproval = aRelatingApproval;
}

occ::handle<StepBasic_Approval> StepBasic_ApprovalRelationship::RelatingApproval() const
{
  return relatingApproval;
}

void StepBasic_ApprovalRelationship::SetRelatedApproval(
  const occ::handle<StepBasic_Approval>& aRelatedApproval)
{
  relatedApproval = aRelatedApproval;
}

occ::handle<StepBasic_Approval> StepBasic_ApprovalRelationship::RelatedApproval() const
{
  return relatedApproval;
}
