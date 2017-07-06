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
#include <StepBasic_NameAssignment.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_NameAssignment,Standard_Transient)

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
