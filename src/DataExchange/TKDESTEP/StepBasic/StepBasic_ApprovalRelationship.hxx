// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_ApprovalRelationship_HeaderFile
#define _StepBasic_ApprovalRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Approval;

class StepBasic_ApprovalRelationship : public Standard_Transient
{

public:
  //! Returns a ApprovalRelationship
  Standard_EXPORT StepBasic_ApprovalRelationship();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<TCollection_HAsciiString>& aDescription,
                            const occ::handle<StepBasic_Approval>&       aRelatingApproval,
                            const occ::handle<StepBasic_Approval>&       aRelatedApproval);

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  Standard_EXPORT void SetRelatingApproval(const occ::handle<StepBasic_Approval>& aRelatingApproval);

  Standard_EXPORT occ::handle<StepBasic_Approval> RelatingApproval() const;

  Standard_EXPORT void SetRelatedApproval(const occ::handle<StepBasic_Approval>& aRelatedApproval);

  Standard_EXPORT occ::handle<StepBasic_Approval> RelatedApproval() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ApprovalRelationship, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> name;
  occ::handle<TCollection_HAsciiString> description;
  occ::handle<StepBasic_Approval>       relatingApproval;
  occ::handle<StepBasic_Approval>       relatedApproval;
};

#endif // _StepBasic_ApprovalRelationship_HeaderFile
