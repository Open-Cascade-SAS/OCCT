// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepAP214_AppliedGroupAssignment_HeaderFile
#define _StepAP214_AppliedGroupAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepAP214_HArray1OfGroupItem.hxx>
#include <StepBasic_GroupAssignment.hxx>
class StepBasic_Group;


class StepAP214_AppliedGroupAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedGroupAssignment, StepBasic_GroupAssignment)

//! Representation of STEP entity AppliedGroupAssignment
class StepAP214_AppliedGroupAssignment : public StepBasic_GroupAssignment
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepAP214_AppliedGroupAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_Group)& aGroupAssignment_AssignedGroup, const Handle(StepAP214_HArray1OfGroupItem)& aItems);
  
  //! Returns field Items
  Standard_EXPORT Handle(StepAP214_HArray1OfGroupItem) Items() const;
  
  //! Set field Items
  Standard_EXPORT void SetItems (const Handle(StepAP214_HArray1OfGroupItem)& Items);




  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedGroupAssignment,StepBasic_GroupAssignment)

protected:




private:


  Handle(StepAP214_HArray1OfGroupItem) theItems;


};







#endif // _StepAP214_AppliedGroupAssignment_HeaderFile
