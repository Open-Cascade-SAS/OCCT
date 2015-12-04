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

#ifndef _StepAP203_Change_HeaderFile
#define _StepAP203_Change_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepAP203_HArray1OfWorkItem.hxx>
#include <StepBasic_ActionAssignment.hxx>
class StepBasic_Action;


class StepAP203_Change;
DEFINE_STANDARD_HANDLE(StepAP203_Change, StepBasic_ActionAssignment)

//! Representation of STEP entity Change
class StepAP203_Change : public StepBasic_ActionAssignment
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepAP203_Change();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_Action)& aActionAssignment_AssignedAction, const Handle(StepAP203_HArray1OfWorkItem)& aItems);
  
  //! Returns field Items
  Standard_EXPORT Handle(StepAP203_HArray1OfWorkItem) Items() const;
  
  //! Set field Items
  Standard_EXPORT void SetItems (const Handle(StepAP203_HArray1OfWorkItem)& Items);




  DEFINE_STANDARD_RTTIEXT(StepAP203_Change,StepBasic_ActionAssignment)

protected:




private:


  Handle(StepAP203_HArray1OfWorkItem) theItems;


};







#endif // _StepAP203_Change_HeaderFile
