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

#ifndef _StepBasic_GroupAssignment_HeaderFile
#define _StepBasic_GroupAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Group;


class StepBasic_GroupAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_GroupAssignment, Standard_Transient)

//! Representation of STEP entity GroupAssignment
class StepBasic_GroupAssignment : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_GroupAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_Group)& aAssignedGroup);
  
  //! Returns field AssignedGroup
  Standard_EXPORT Handle(StepBasic_Group) AssignedGroup() const;
  
  //! Set field AssignedGroup
  Standard_EXPORT void SetAssignedGroup (const Handle(StepBasic_Group)& AssignedGroup);




  DEFINE_STANDARD_RTTIEXT(StepBasic_GroupAssignment,Standard_Transient)

protected:




private:


  Handle(StepBasic_Group) theAssignedGroup;


};







#endif // _StepBasic_GroupAssignment_HeaderFile
