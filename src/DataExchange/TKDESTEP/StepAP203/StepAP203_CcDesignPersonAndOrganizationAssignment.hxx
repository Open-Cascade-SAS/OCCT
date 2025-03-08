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

#ifndef _StepAP203_CcDesignPersonAndOrganizationAssignment_HeaderFile
#define _StepAP203_CcDesignPersonAndOrganizationAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP203_HArray1OfPersonOrganizationItem.hxx>
#include <StepBasic_PersonAndOrganizationAssignment.hxx>
class StepBasic_PersonAndOrganization;
class StepBasic_PersonAndOrganizationRole;

class StepAP203_CcDesignPersonAndOrganizationAssignment;
DEFINE_STANDARD_HANDLE(StepAP203_CcDesignPersonAndOrganizationAssignment,
                       StepBasic_PersonAndOrganizationAssignment)

//! Representation of STEP entity CcDesignPersonAndOrganizationAssignment
class StepAP203_CcDesignPersonAndOrganizationAssignment
    : public StepBasic_PersonAndOrganizationAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignPersonAndOrganizationAssignment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(StepBasic_PersonAndOrganization)&
      aPersonAndOrganizationAssignment_AssignedPersonAndOrganization,
    const Handle(StepBasic_PersonAndOrganizationRole)&       aPersonAndOrganizationAssignment_Role,
    const Handle(StepAP203_HArray1OfPersonOrganizationItem)& aItems);

  //! Returns field Items
  Standard_EXPORT Handle(StepAP203_HArray1OfPersonOrganizationItem) Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(const Handle(StepAP203_HArray1OfPersonOrganizationItem)& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignPersonAndOrganizationAssignment,
                          StepBasic_PersonAndOrganizationAssignment)

protected:
private:
  Handle(StepAP203_HArray1OfPersonOrganizationItem) theItems;
};

#endif // _StepAP203_CcDesignPersonAndOrganizationAssignment_HeaderFile
