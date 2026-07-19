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

#ifndef _StepBasic_DateAndTimeAssignment_HeaderFile
#define _StepBasic_DateAndTimeAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_DateAndTime;
class StepBasic_DateTimeRole;

class StepBasic_DateAndTimeAssignment : public Standard_Transient
{

public:
  Standard_EXPORT void Init(const occ::handle<StepBasic_DateAndTime>&  aAssignedDateAndTime,
                            const occ::handle<StepBasic_DateTimeRole>& aRole);

  Standard_EXPORT void SetAssignedDateAndTime(
    const occ::handle<StepBasic_DateAndTime>& aAssignedDateAndTime);

  Standard_EXPORT occ::handle<StepBasic_DateAndTime> AssignedDateAndTime() const;

  Standard_EXPORT void SetRole(const occ::handle<StepBasic_DateTimeRole>& aRole);

  Standard_EXPORT occ::handle<StepBasic_DateTimeRole> Role() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_DateAndTimeAssignment, Standard_Transient)

private:
  occ::handle<StepBasic_DateAndTime>  assignedDateAndTime;
  occ::handle<StepBasic_DateTimeRole> role;
};

#endif // _StepBasic_DateAndTimeAssignment_HeaderFile
