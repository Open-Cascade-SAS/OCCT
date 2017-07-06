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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupAssignment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_GroupAssignment,Standard_Transient)

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
