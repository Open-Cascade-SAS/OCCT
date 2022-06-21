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

#include <StepAP203_HArray1OfDateTimeItem.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
class StepBasic_DateAndTime;
class StepBasic_DateTimeRole;


class StepAP203_CcDesignDateAndTimeAssignment;
DEFINE_STANDARD_HANDLE(StepAP203_CcDesignDateAndTimeAssignment, StepBasic_DateAndTimeAssignment)

//! Representation of STEP entity CcDesignDateAndTimeAssignment
class StepAP203_CcDesignDateAndTimeAssignment : public StepBasic_DateAndTimeAssignment
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignDateAndTimeAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_DateAndTime)& aDateAndTimeAssignment_AssignedDateAndTime, const Handle(StepBasic_DateTimeRole)& aDateAndTimeAssignment_Role, const Handle(StepAP203_HArray1OfDateTimeItem)& aItems);
  
  //! Returns field Items
  Standard_EXPORT Handle(StepAP203_HArray1OfDateTimeItem) Items() const;
  
  //! Set field Items
  Standard_EXPORT void SetItems (const Handle(StepAP203_HArray1OfDateTimeItem)& Items);




  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignDateAndTimeAssignment,StepBasic_DateAndTimeAssignment)

protected:




private:


  Handle(StepAP203_HArray1OfDateTimeItem) theItems;


};







#endif // _StepAP203_CcDesignDateAndTimeAssignment_HeaderFile
