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

#ifndef _StepBasic_ActionAssignment_HeaderFile
#define _StepBasic_ActionAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Action;


class StepBasic_ActionAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_ActionAssignment, Standard_Transient)

//! Representation of STEP entity ActionAssignment
class StepBasic_ActionAssignment : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_ActionAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_Action)& aAssignedAction);
  
  //! Returns field AssignedAction
  Standard_EXPORT Handle(StepBasic_Action) AssignedAction() const;
  
  //! Set field AssignedAction
  Standard_EXPORT void SetAssignedAction (const Handle(StepBasic_Action)& AssignedAction);




  DEFINE_STANDARD_RTTIEXT(StepBasic_ActionAssignment,Standard_Transient)

protected:




private:


  Handle(StepBasic_Action) theAssignedAction;


};







#endif // _StepBasic_ActionAssignment_HeaderFile
