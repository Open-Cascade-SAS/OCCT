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

#ifndef _StepAP214_AutoDesignActualDateAssignment_HeaderFile
#define _StepAP214_AutoDesignActualDateAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_AutoDesignDatedItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_DateAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Date;
class StepBasic_DateRole;
class StepAP214_AutoDesignDatedItem;

class StepAP214_AutoDesignActualDateAssignment : public StepBasic_DateAssignment
{

public:
  //! Returns a AutoDesignActualDateAssignment
  Standard_EXPORT StepAP214_AutoDesignActualDateAssignment();

  Standard_EXPORT void Init(
    const occ::handle<StepBasic_Date>&                                     aAssignedDate,
    const occ::handle<StepBasic_DateRole>&                                 aRole,
    const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>>& aItems);

  Standard_EXPORT void SetItems(
    const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>>& aItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>> Items() const;

  Standard_EXPORT StepAP214_AutoDesignDatedItem ItemsValue(const int num) const;

  Standard_EXPORT int NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AutoDesignActualDateAssignment, StepBasic_DateAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>> items;
};

#endif // _StepAP214_AutoDesignActualDateAssignment_HeaderFile
