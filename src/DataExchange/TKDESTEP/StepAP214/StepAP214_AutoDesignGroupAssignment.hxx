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

#ifndef _StepAP214_AutoDesignGroupAssignment_HeaderFile
#define _StepAP214_AutoDesignGroupAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfAutoDesignGroupedItem.hxx>
#include <StepBasic_GroupAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Group;
class StepAP214_AutoDesignGroupedItem;

class StepAP214_AutoDesignGroupAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AutoDesignGroupAssignment, StepBasic_GroupAssignment)

class StepAP214_AutoDesignGroupAssignment : public StepBasic_GroupAssignment
{

public:
  //! Returns a AutoDesignGroupAssignment
  Standard_EXPORT StepAP214_AutoDesignGroupAssignment();

  Standard_EXPORT void Init(const Handle(StepBasic_Group)&                          aAssignedGroup,
                            const Handle(StepAP214_HArray1OfAutoDesignGroupedItem)& aItems);

  Standard_EXPORT void SetItems(const Handle(StepAP214_HArray1OfAutoDesignGroupedItem)& aItems);

  Standard_EXPORT Handle(StepAP214_HArray1OfAutoDesignGroupedItem) Items() const;

  Standard_EXPORT StepAP214_AutoDesignGroupedItem ItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AutoDesignGroupAssignment, StepBasic_GroupAssignment)

protected:
private:
  Handle(StepAP214_HArray1OfAutoDesignGroupedItem) items;
};

#endif // _StepAP214_AutoDesignGroupAssignment_HeaderFile
