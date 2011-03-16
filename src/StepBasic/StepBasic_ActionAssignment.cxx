// File:	StepBasic_ActionAssignment.cxx
// Created:	Fri Nov 26 16:26:29 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ActionAssignment.ixx>

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
