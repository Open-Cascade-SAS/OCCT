// File:	StepAP214_AppliedGroupAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepAP214_AppliedGroupAssignment.ixx>

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
