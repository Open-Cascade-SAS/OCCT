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
#include <StepBasic_ActionRequestAssignment.hxx>
#include <StepBasic_VersionedActionRequest.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ActionRequestAssignment,Standard_Transient)

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
