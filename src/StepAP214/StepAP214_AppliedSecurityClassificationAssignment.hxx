// Created on: 1999-03-10
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

#ifndef _StepAP214_AppliedSecurityClassificationAssignment_HeaderFile
#define _StepAP214_AppliedSecurityClassificationAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfSecurityClassificationItem.hxx>
#include <StepBasic_SecurityClassificationAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_SecurityClassification;
class StepAP214_SecurityClassificationItem;

class StepAP214_AppliedSecurityClassificationAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedSecurityClassificationAssignment,
                       StepBasic_SecurityClassificationAssignment)

class StepAP214_AppliedSecurityClassificationAssignment
    : public StepBasic_SecurityClassificationAssignment
{

public:
  //! Returns a AppliedSecurityClassificationAssignment
  Standard_EXPORT StepAP214_AppliedSecurityClassificationAssignment();

  Standard_EXPORT void Init(
    const Handle(StepBasic_SecurityClassification)&              aAssignedSecurityClassification,
    const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems);

  Standard_EXPORT void SetItems(
    const Handle(StepAP214_HArray1OfSecurityClassificationItem)& aItems);

  Standard_EXPORT Handle(StepAP214_HArray1OfSecurityClassificationItem) Items() const;

  Standard_EXPORT const StepAP214_SecurityClassificationItem& ItemsValue(
    const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedSecurityClassificationAssignment,
                          StepBasic_SecurityClassificationAssignment)

protected:
private:
  Handle(StepAP214_HArray1OfSecurityClassificationItem) items;
};

#endif // _StepAP214_AppliedSecurityClassificationAssignment_HeaderFile
