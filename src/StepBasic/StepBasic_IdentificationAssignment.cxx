// File:	StepBasic_IdentificationAssignment.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_IdentificationAssignment.ixx>

//=======================================================================
//function : StepBasic_IdentificationAssignment
//purpose  : 
//=======================================================================

StepBasic_IdentificationAssignment::StepBasic_IdentificationAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_IdentificationAssignment::Init (const Handle(TCollection_HAsciiString) &aAssignedId,
                                               const Handle(StepBasic_IdentificationRole) &aRole)
{

  theAssignedId = aAssignedId;

  theRole = aRole;
}

//=======================================================================
//function : AssignedId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_IdentificationAssignment::AssignedId () const
{
  return theAssignedId;
}

//=======================================================================
//function : SetAssignedId
//purpose  : 
//=======================================================================

void StepBasic_IdentificationAssignment::SetAssignedId (const Handle(TCollection_HAsciiString) &aAssignedId)
{
  theAssignedId = aAssignedId;
}

//=======================================================================
//function : Role
//purpose  : 
//=======================================================================

Handle(StepBasic_IdentificationRole) StepBasic_IdentificationAssignment::Role () const
{
  return theRole;
}

//=======================================================================
//function : SetRole
//purpose  : 
//=======================================================================

void StepBasic_IdentificationAssignment::SetRole (const Handle(StepBasic_IdentificationRole) &aRole)
{
  theRole = aRole;
}
