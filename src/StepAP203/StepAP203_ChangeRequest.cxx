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
#include <StepAP203_ChangeRequest.hxx>
#include <StepBasic_VersionedActionRequest.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP203_ChangeRequest,StepBasic_ActionRequestAssignment)

//=======================================================================
//function : StepAP203_ChangeRequest
//purpose  : 
//=======================================================================
StepAP203_ChangeRequest::StepAP203_ChangeRequest ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_ChangeRequest::Init (const Handle(StepBasic_VersionedActionRequest) &aActionRequestAssignment_AssignedActionRequest,
                                    const Handle(StepAP203_HArray1OfChangeRequestItem) &aItems)
{
  StepBasic_ActionRequestAssignment::Init(aActionRequestAssignment_AssignedActionRequest);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfChangeRequestItem) StepAP203_ChangeRequest::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_ChangeRequest::SetItems (const Handle(StepAP203_HArray1OfChangeRequestItem) &aItems)
{
  theItems = aItems;
}
