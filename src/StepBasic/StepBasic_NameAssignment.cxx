// File:	StepBasic_NameAssignment.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_NameAssignment.ixx>

//=======================================================================
//function : StepBasic_NameAssignment
//purpose  : 
//=======================================================================

StepBasic_NameAssignment::StepBasic_NameAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_NameAssignment::Init (const Handle(TCollection_HAsciiString) &aAssignedName)
{

  theAssignedName = aAssignedName;
}

//=======================================================================
//function : AssignedName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_NameAssignment::AssignedName () const
{
  return theAssignedName;
}

//=======================================================================
//function : SetAssignedName
//purpose  : 
//=======================================================================

void StepBasic_NameAssignment::SetAssignedName (const Handle(TCollection_HAsciiString) &aAssignedName)
{
  theAssignedName = aAssignedName;
}
