// File:	StepAP203_StartRequest.cxx
// Created:	Fri Nov 26 16:26:40 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_StartRequest.ixx>

//=======================================================================
//function : StepAP203_StartRequest
//purpose  : 
//=======================================================================

StepAP203_StartRequest::StepAP203_StartRequest ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_StartRequest::Init (const Handle(StepBasic_VersionedActionRequest) &aActionRequestAssignment_AssignedActionRequest,
                                   const Handle(StepAP203_HArray1OfStartRequestItem) &aItems)
{
  StepBasic_ActionRequestAssignment::Init(aActionRequestAssignment_AssignedActionRequest);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfStartRequestItem) StepAP203_StartRequest::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_StartRequest::SetItems (const Handle(StepAP203_HArray1OfStartRequestItem) &aItems)
{
  theItems = aItems;
}
