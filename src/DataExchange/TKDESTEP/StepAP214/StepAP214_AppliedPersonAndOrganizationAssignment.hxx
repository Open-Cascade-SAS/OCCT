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

#include <StepAP214_PersonAndOrganizationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_PersonAndOrganizationAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_PersonAndOrganization;
class StepBasic_PersonAndOrganizationRole;
class StepAP214_PersonAndOrganizationItem;

class StepAP214_AppliedPersonAndOrganizationAssignment
    : public StepBasic_PersonAndOrganizationAssignment
{

public:
  //! Returns a AutoDesignDateAndPersonAssignment
  Standard_EXPORT StepAP214_AppliedPersonAndOrganizationAssignment();

  Standard_EXPORT void Init(
    const occ::handle<StepBasic_PersonAndOrganization>&              aAssignedPersonAndOrganization,
    const occ::handle<StepBasic_PersonAndOrganizationRole>&          aRole,
    const occ::handle<NCollection_HArray1<StepAP214_PersonAndOrganizationItem>>& aItems);

  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP214_PersonAndOrganizationItem>>& aItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP214_PersonAndOrganizationItem>> Items() const;

  Standard_EXPORT StepAP214_PersonAndOrganizationItem ItemsValue(const int num) const;

  Standard_EXPORT int NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedPersonAndOrganizationAssignment,
                          StepBasic_PersonAndOrganizationAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP214_PersonAndOrganizationItem>> items;
};

#endif // _StepAP214_AppliedPersonAndOrganizationAssignment_HeaderFile
