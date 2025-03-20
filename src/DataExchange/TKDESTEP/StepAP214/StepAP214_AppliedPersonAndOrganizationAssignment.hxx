// Created on: 1999-03-09
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

#ifndef _StepAP214_AppliedPersonAndOrganizationAssignment_HeaderFile
#define _StepAP214_AppliedPersonAndOrganizationAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfPersonAndOrganizationItem.hxx>
#include <StepBasic_PersonAndOrganizationAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_PersonAndOrganization;
class StepBasic_PersonAndOrganizationRole;
class StepAP214_PersonAndOrganizationItem;

class StepAP214_AppliedPersonAndOrganizationAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedPersonAndOrganizationAssignment,
                       StepBasic_PersonAndOrganizationAssignment)

class StepAP214_AppliedPersonAndOrganizationAssignment
    : public StepBasic_PersonAndOrganizationAssignment
{

public:
  //! Returns a AutoDesignDateAndPersonAssignment
  Standard_EXPORT StepAP214_AppliedPersonAndOrganizationAssignment();

  Standard_EXPORT void Init(
    const Handle(StepBasic_PersonAndOrganization)&              aAssignedPersonAndOrganization,
    const Handle(StepBasic_PersonAndOrganizationRole)&          aRole,
    const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems);

  Standard_EXPORT void SetItems(const Handle(StepAP214_HArray1OfPersonAndOrganizationItem)& aItems);

  Standard_EXPORT Handle(StepAP214_HArray1OfPersonAndOrganizationItem) Items() const;

  Standard_EXPORT StepAP214_PersonAndOrganizationItem ItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedPersonAndOrganizationAssignment,
                          StepBasic_PersonAndOrganizationAssignment)

protected:
private:
  Handle(StepAP214_HArray1OfPersonAndOrganizationItem) items;
};

#endif // _StepAP214_AppliedPersonAndOrganizationAssignment_HeaderFile
