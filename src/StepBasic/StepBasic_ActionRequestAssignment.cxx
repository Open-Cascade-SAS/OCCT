// File:	StepBasic_ActionRequestAssignment.cxx
// Created:	Fri Nov 26 16:26:30 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ActionRequestAssignment.ixx>

//=======================================================================
//function : StepBasic_ActionRequestAssignment
//purpose  : 
//=======================================================================

StepBasic_ActionRequestAssignment::StepBasic_ActionRequestAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ActionRequestAssignment::Init (const Handle(StepBasic_VersionedActionRequest) &aAssignedActionRequest)
{

  theAssignedActionRequest = aAssignedActionRequest;
}

//=======================================================================
//function : AssignedActionRequest
//purpose  : 
//=======================================================================

Handle(StepBasic_VersionedActionRequest) StepBasic_ActionRequestAssignment::AssignedActionRequest () const
{
  return theAssignedActionRequest;
}

//=======================================================================
//function : SetAssignedActionRequest
//purpose  : 
//=======================================================================

void StepBasic_ActionRequestAssignment::SetAssignedActionRequest (const Handle(StepBasic_VersionedActionRequest) &aAssignedActionRequest)
{
  theAssignedActionRequest = aAssignedActionRequest;
}
