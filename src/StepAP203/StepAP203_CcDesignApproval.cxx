// File:	StepAP203_CcDesignApproval.cxx
// Created:	Fri Nov 26 16:26:31 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignApproval.ixx>

//=======================================================================
//function : StepAP203_CcDesignApproval
//purpose  : 
//=======================================================================

StepAP203_CcDesignApproval::StepAP203_CcDesignApproval ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignApproval::Init (const Handle(StepBasic_Approval) &aApprovalAssignment_AssignedApproval,
                                       const Handle(StepAP203_HArray1OfApprovedItem) &aItems)
{
  StepBasic_ApprovalAssignment::Init(aApprovalAssignment_AssignedApproval);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfApprovedItem) StepAP203_CcDesignApproval::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignApproval::SetItems (const Handle(StepAP203_HArray1OfApprovedItem) &aItems)
{
  theItems = aItems;
}
