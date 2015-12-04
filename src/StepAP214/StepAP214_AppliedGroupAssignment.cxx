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
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepBasic_Group.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedGroupAssignment,StepBasic_GroupAssignment)

//=======================================================================
//function : StepAP214_AppliedGroupAssignment
//purpose  : 
//=======================================================================
StepAP214_AppliedGroupAssignment::StepAP214_AppliedGroupAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP214_AppliedGroupAssignment::Init (const Handle(StepBasic_Group) &aGroupAssignment_AssignedGroup,
                                             const Handle(StepAP214_HArray1OfGroupItem) &aItems)
{
  StepBasic_GroupAssignment::Init(aGroupAssignment_AssignedGroup);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP214_HArray1OfGroupItem) StepAP214_AppliedGroupAssignment::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP214_AppliedGroupAssignment::SetItems (const Handle(StepAP214_HArray1OfGroupItem) &aItems)
{
  theItems = aItems;
}
