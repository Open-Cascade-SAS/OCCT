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

#ifndef _StepAP214_AutoDesignNominalDateAndTimeAssignment_HeaderFile
#define _StepAP214_AutoDesignNominalDateAndTimeAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_AutoDesignDateAndTimeItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_DateAndTime;
class StepBasic_DateTimeRole;
class StepAP214_AutoDesignDateAndTimeItem;

class StepAP214_AutoDesignNominalDateAndTimeAssignment : public StepBasic_DateAndTimeAssignment
{

public:
  //! Returns a AutoDesignNominalDateAndTimeAssignment
  Standard_EXPORT StepAP214_AutoDesignNominalDateAndTimeAssignment();

  Standard_EXPORT void Init(const occ::handle<StepBasic_DateAndTime>&  aAssignedDateAndTime,
                            const occ::handle<StepBasic_DateTimeRole>& aRole,
                            const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDateAndTimeItem>>& aItems);

  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDateAndTimeItem>>& aItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP214_AutoDesignDateAndTimeItem>> Items() const;

  Standard_EXPORT StepAP214_AutoDesignDateAndTimeItem ItemsValue(const int num) const;

  Standard_EXPORT int NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AutoDesignNominalDateAndTimeAssignment,
                          StepBasic_DateAndTimeAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP214_AutoDesignDateAndTimeItem>> items;
};

#endif // _StepAP214_AutoDesignNominalDateAndTimeAssignment_HeaderFile
