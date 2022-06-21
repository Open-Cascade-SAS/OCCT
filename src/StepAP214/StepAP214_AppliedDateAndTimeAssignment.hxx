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

#ifndef _StepAP214_AppliedDateAndTimeAssignment_HeaderFile
#define _StepAP214_AppliedDateAndTimeAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfDateAndTimeItem.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_DateAndTime;
class StepBasic_DateTimeRole;
class StepAP214_DateAndTimeItem;


class StepAP214_AppliedDateAndTimeAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedDateAndTimeAssignment, StepBasic_DateAndTimeAssignment)


class StepAP214_AppliedDateAndTimeAssignment : public StepBasic_DateAndTimeAssignment
{

public:

  
  //! Returns a AppliedDateAndTimeAssignment
  Standard_EXPORT StepAP214_AppliedDateAndTimeAssignment();
  
  Standard_EXPORT void Init (const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime, const Handle(StepBasic_DateTimeRole)& aRole, const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems);
  
  Standard_EXPORT void SetItems (const Handle(StepAP214_HArray1OfDateAndTimeItem)& aItems);
  
  Standard_EXPORT Handle(StepAP214_HArray1OfDateAndTimeItem) Items() const;
  
  Standard_EXPORT StepAP214_DateAndTimeItem ItemsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbItems() const;




  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedDateAndTimeAssignment,StepBasic_DateAndTimeAssignment)

protected:




private:


  Handle(StepAP214_HArray1OfDateAndTimeItem) items;


};







#endif // _StepAP214_AppliedDateAndTimeAssignment_HeaderFile
