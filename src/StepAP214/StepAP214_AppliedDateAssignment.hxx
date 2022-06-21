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

#ifndef _StepAP214_AppliedDateAssignment_HeaderFile
#define _StepAP214_AppliedDateAssignment_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfDateItem.hxx>
#include <StepBasic_DateAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Date;
class StepBasic_DateRole;
class StepAP214_DateItem;


class StepAP214_AppliedDateAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedDateAssignment, StepBasic_DateAssignment)


class StepAP214_AppliedDateAssignment : public StepBasic_DateAssignment
{

public:

  
  //! Returns a AppliedDateAssignment
  Standard_EXPORT StepAP214_AppliedDateAssignment();
  
  Standard_EXPORT void Init (const Handle(StepBasic_Date)& aAssignedDate, const Handle(StepBasic_DateRole)& aRole, const Handle(StepAP214_HArray1OfDateItem)& aItems);
  
  Standard_EXPORT void SetItems (const Handle(StepAP214_HArray1OfDateItem)& aItems);
  
  Standard_EXPORT Handle(StepAP214_HArray1OfDateItem) Items() const;
  
  Standard_EXPORT StepAP214_DateItem ItemsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbItems() const;




  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedDateAssignment,StepBasic_DateAssignment)

protected:




private:


  Handle(StepAP214_HArray1OfDateItem) items;


};







#endif // _StepAP214_AppliedDateAssignment_HeaderFile
