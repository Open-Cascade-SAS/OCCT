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
#include <StepBasic_IdentificationAssignment.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_IdentificationAssignment,Standard_Transient)

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
