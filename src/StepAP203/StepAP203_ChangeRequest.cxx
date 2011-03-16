// File:	StepAP203_ChangeRequest.cxx
// Created:	Fri Nov 26 16:26:35 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_ChangeRequest.ixx>

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
