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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepBasic_Action.hxx>
#include <StepBasic_ActionAssignment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ActionAssignment,Standard_Transient)

//=======================================================================
//function : StepBasic_ActionAssignment
//purpose  : 
//=======================================================================
StepBasic_ActionAssignment::StepBasic_ActionAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ActionAssignment::Init (const Handle(StepBasic_Action) &aAssignedAction)
{

  theAssignedAction = aAssignedAction;
}

//=======================================================================
//function : AssignedAction
//purpose  : 
//=======================================================================

Handle(StepBasic_Action) StepBasic_ActionAssignment::AssignedAction () const
{
  return theAssignedAction;
}

//=======================================================================
//function : SetAssignedAction
//purpose  : 
//=======================================================================

void StepBasic_ActionAssignment::SetAssignedAction (const Handle(StepBasic_Action) &aAssignedAction)
{
  theAssignedAction = aAssignedAction;
}
