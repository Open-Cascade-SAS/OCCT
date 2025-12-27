// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepAP214_AppliedExternalIdentificationAssignment_HeaderFile
#define _StepAP214_AppliedExternalIdentificationAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfExternalIdentificationItem.hxx>
#include <StepBasic_ExternalIdentificationAssignment.hxx>
class TCollection_HAsciiString;
class StepBasic_IdentificationRole;
class StepBasic_ExternalSource;

//! Representation of STEP entity AppliedExternalIdentificationAssignment
class StepAP214_AppliedExternalIdentificationAssignment
    : public StepBasic_ExternalIdentificationAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP214_AppliedExternalIdentificationAssignment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&     aIdentificationAssignment_AssignedId,
    const occ::handle<StepBasic_IdentificationRole>& aIdentificationAssignment_Role,
    const occ::handle<StepBasic_ExternalSource>&     aExternalIdentificationAssignment_Source,
    const occ::handle<StepAP214_HArray1OfExternalIdentificationItem>& aItems);

  //! Returns field Items
  Standard_EXPORT occ::handle<StepAP214_HArray1OfExternalIdentificationItem> Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(const occ::handle<StepAP214_HArray1OfExternalIdentificationItem>& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedExternalIdentificationAssignment,
                          StepBasic_ExternalIdentificationAssignment)

private:
  occ::handle<StepAP214_HArray1OfExternalIdentificationItem> theItems;
};

#endif // _StepAP214_AppliedExternalIdentificationAssignment_HeaderFile
