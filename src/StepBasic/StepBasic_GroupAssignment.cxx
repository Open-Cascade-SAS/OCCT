// File:	StepBasic_GroupAssignment.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_GroupAssignment.ixx>

//=======================================================================
//function : StepBasic_GroupAssignment
//purpose  : 
//=======================================================================

StepBasic_GroupAssignment::StepBasic_GroupAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_GroupAssignment::Init (const Handle(StepBasic_Group) &aAssignedGroup)
{

  theAssignedGroup = aAssignedGroup;
}

//=======================================================================
//function : AssignedGroup
//purpose  : 
//=======================================================================

Handle(StepBasic_Group) StepBasic_GroupAssignment::AssignedGroup () const
{
  return theAssignedGroup;
}

//=======================================================================
//function : SetAssignedGroup
//purpose  : 
//=======================================================================

void StepBasic_GroupAssignment::SetAssignedGroup (const Handle(StepBasic_Group) &aAssignedGroup)
{
  theAssignedGroup = aAssignedGroup;
}
