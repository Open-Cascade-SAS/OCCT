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

#ifndef _StepAP203_CcDesignDateAndTimeAssignment_HeaderFile
#define _StepAP203_CcDesignDateAndTimeAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP203_DateTimeItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
class StepBasic_DateAndTime;
class StepBasic_DateTimeRole;

//! Representation of STEP entity CcDesignDateAndTimeAssignment
class StepAP203_CcDesignDateAndTimeAssignment : public StepBasic_DateAndTimeAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignDateAndTimeAssignment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepBasic_DateAndTime>&           aDateAndTimeAssignment_AssignedDateAndTime,
    const occ::handle<StepBasic_DateTimeRole>&          aDateAndTimeAssignment_Role,
    const occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>>& aItems);

  //! Returns field Items
  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>> Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>>& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignDateAndTimeAssignment, StepBasic_DateAndTimeAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP203_DateTimeItem>> theItems;
};

#endif // _StepAP203_CcDesignDateAndTimeAssignment_HeaderFile
